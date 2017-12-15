// This library is part of Massive, copyright 2017 Lea Waller.
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "m_common_io_neuroimaging.h"

void read_nii_f4(char* f, u4_t *n,
    u4_t **z, f4_t **e) {
  nifti_image *m = nifti_image_read(f, 1);
  if (!m) {
    fprintf(stderr, RED "Failed to open %s for reading." WHITE "\n", f);
    exit(EXIT_FAILURE);
  }

  const u4_t d[4] = {
     m->nx,
     m->ny,
     m->nz,
     m->nt
  };

  if (debug) {
    printf("nifti size %u %u %u %u\n", d[0], d[1], d[2], d[3]);
  }

  n[0] = d[3];

  if (m->datatype != NIFTI_TYPE_FLOAT32) {
    fprintf(stderr, RED "Nifti data type needs to be float32." WHITE "\n");
    exit(EXIT_FAILURE);
  }
  size_t p = d[0] * d[1] * d[2];
  *z = allocate_u4(p);

  *e = allocate_f4(0);

  f4_t *h = (f4_t*) m->data;

  n[1] = 0;
  for (u4_t i = 0; i < d[0]; ++i) {
    for (u4_t j = 0; j < d[1]; ++j) {
      for (u4_t k = 0; k < d[2]; ++k) {

        f4_t s = 0.0f;
        for (u4_t t = 0; t < d[3]; ++t) {
          f4_t u = h[((t*d[2]+k)*d[1]+j)*d[0]+i];
          s += fabsf(u);

          (*e)[n[1]*n[0]+t] = u;
        }

        if (s > 0.0f) {
          (*z)[n[1]] = (k*d[1]+j)*d[0]+i;

          n[1]++;
          allocate_f4(n[0]);
        }
      }
    }

    showprogress(i, d[0]);
  }

  free(m->data);
  free(m);
}

void read_nii_f8(char* f, u4_t *n,
    u4_t **z, double **e) {
  nifti_image *m = nifti_image_read(f, 1);
  if (!m) {
    fprintf(stderr, RED "Failed to open %s for reading." WHITE "\n", f);
    exit(EXIT_FAILURE);
  }

  const u4_t d[4] = {
     m->nx,
     m->ny,
     m->nz,
     m->nt
  };

  if (debug) {
    printf("nifti size %u %u %u %u\n", d[0], d[1], d[2], d[3]);
  }

  n[0] = d[3];

  if (m->datatype != NIFTI_TYPE_FLOAT32) {
    fprintf(stderr, RED "Nifti data type needs to be float32." WHITE "\n");
    exit(EXIT_FAILURE);
  }
  size_t p = d[0] * d[1] * d[2];
  *z = allocate_u4(p);

  *e = allocate_f8(0);

  f4_t *h = (f4_t*) m->data;

  n[1] = 0;
  for (u4_t i = 0; i < d[0]; ++i) {
    for (u4_t j = 0; j < d[1]; ++j) {
      for (u4_t k = 0; k < d[2]; ++k) {

        double s = 0.0f;
        for (u4_t t = 0; t < d[3]; ++t) {
          double u = (double) h[((t*d[2]+k)*d[1]+j)*d[0]+i];
          s += fabs(u);

          (*e)[n[1]*n[0]+t] = u;
        }

        if (s > 0.0f) {
          (*z)[n[1]] = (k*d[1]+j)*d[0]+i;

          n[1]++;
          allocate_f8(n[0]);
        }
      }
    }

    showprogress(i, d[0]);
  }

  free(m->data);
  free(m);
}

void write_nii_f4(char *f, char* lf, u4_t *n,
    u4_t *z, f4_t *e) {
  FSLIO *m = FslOpen(lf, "rb");

  if (!m->niftiptr) {
    fprintf(stderr, RED "Failed to open %s for reading." WHITE "\n", lf);
    exit(EXIT_FAILURE);
  }

  FSLIO *mw = FslOpen(f, "wb");

  FslCloneHeader(mw, m);

  FslSetDim(mw, m->niftiptr->nx, m->niftiptr->ny, m->niftiptr->nz, n[0]);

  FslSetDataType(mw, NIFTI_TYPE_FLOAT32);

  FslWriteHeader(mw);

  const u4_t d[4] = {
     mw->niftiptr->nx,
     mw->niftiptr->ny,
     mw->niftiptr->nz,
     mw->niftiptr->nt
  };

  size_t p = d[0] * d[1] * d[2];

  f4_t *ew = allocate_f4(p);

  memset(ew, 0, p * sizeof(f4_t));

  for (u4_t i = 0; i < d[3]; ++i) {
    for (u4_t j = 0; j < n[1]; ++j) {
        ew[z[j]] = e[i*n[1]+j];
    }

    FslWriteVolumes(mw, ew, 1);
  }

  FslClose(m);
  FslClose(mw);
}

void write_nii_f8(char *f, char* lf, u4_t *n,
    u4_t *z, double *e) {
  FSLIO *m = FslOpen(lf, "rb");

  if (!m->niftiptr) {
    fprintf(stderr, RED "Failed to open %s for reading." WHITE "\n", lf);
    exit(EXIT_FAILURE);
  }

  FSLIO *mw = FslOpen(f, "wb");

  FslCloneHeader(mw, m);

  FslSetDim(mw, m->niftiptr->nx, m->niftiptr->ny, m->niftiptr->nz, n[0]);

  FslSetDataType(mw, NIFTI_TYPE_FLOAT32);

  FslWriteHeader(mw);

  const u4_t d[4] = {
     mw->niftiptr->nx,
     mw->niftiptr->ny,
     mw->niftiptr->nz,
     mw->niftiptr->nt
  };

  size_t p = d[0] * d[1] * d[2];

  f4_t *ew = allocate_f4(p);

  memset(ew, 0, p * sizeof(f4_t));

  for (u4_t i = 0; i < d[3]; ++i) {
    for (u4_t j = 0; j < n[1]; ++j) {
        ew[z[j]] = (f4_t) e[i*n[1]+j];
    }

    FslWriteVolumes(mw, ew, 1);
  }

  FslClose(m);
  FslClose(mw);
}
