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

#include "m_common_io.h"

static inline void lc(char *f, u4_t *l) {
  FILE *fp = fopen(f, "r");

  *l = 0;

  int a;
  while ((a = getc(fp)) != EOF) {
    if (a == '\n') {
      (*l)++;
    }
  }

  fclose(fp);
}

void write_txt_f4(char *f, u4_t *n,
  f4_t *d) {
  FILE *fp = fopen(f, "w");
  if (!fp) {
    fprintf(stderr, RED "Failed to open %s for writing." WHITE "\n", f);
    exit(EXIT_FAILURE);
  }

  for (u4_t i = 0; i < n[0]; i++) {
		for (u4_t j = 0; j < n[1]; j++) {
			fprintf(fp, "%.9g\t", d[i+n[0]*j]);
		}
		fprintf(fp, "\n");
	}

  fclose(fp);
}

void write_txt_f8(char *f, u4_t *n,
  double *d) {
  FILE *fp = fopen(f, "w");
  if (!fp) {
    fprintf(stderr, RED "Failed to open %s for writing." WHITE "\n", f);
    exit(EXIT_FAILURE);
  }

  for (u4_t i = 0; i < n[0]; i++) {
		for (u4_t j = 0; j < n[1]; j++) {
			fprintf(fp, "%.17g\t", d[i+n[0]*j]);
		}
		fprintf(fp, "\n");
	}

  fclose(fp);
}

void write_ntxt_f4(char *f, u4_t *n,
  f4_t *d, char **cn, char **rn, u4_t *nn) {

  FILE *fp = fopen(f, "w");
  if (!fp) {
    fprintf(stderr, RED "Failed to open %s for writing." WHITE "\n", f);
    exit(EXIT_FAILURE);
  }

  for (u4_t i = 0; i < nn[0]; i++) {
    for (u4_t j = 0; j < nn[1]; j++) {
      fprintf(fp, "\t");
    }
    for (u4_t j = 0; j < n[0]; j++) {
      fprintf(fp, "%s\t", cn[j*nn[0]+i]);
    }
    fprintf(fp, "\n");
  }

  for (u4_t i = 0; i < n[1]; i++) {
    for (u4_t j = 0; j < nn[1]; j++) {
      if (debug) {
        printf("%u\n", i*nn[1]+j);
      }
      fprintf(fp, "%s\t", rn[i*nn[1]+j]);
    }
		for (u4_t j = 0; j < n[0]; j++) {
			fprintf(fp, "%.9g\t", d[i*n[0]+j]);
		}
		fprintf(fp, "\n");
	}

  fclose(fp);
}

void read_genotype(char *b, u4_t *n,
    u4_t **g) {
  char *c = &b[strlen(b)];
  strcpy(c, ".fam");
  lc(c, &n[0]);

  strcpy(c, ".bim");
  lc(c, &n[1]);

  strcpy(c, ".bed");

  size_t v = DIV_UP(n[0], 4);
  size_t w = v * n[1];
  *g = (u4_t*) allocate_u1(w);

  FILE *fp = fopen(c, "rb");
  if (!fp) {
    fprintf(stderr, RED "Failed to open %s for reading." WHITE "\n", c);
    exit(EXIT_FAILURE);
  }

  unsigned char mb[3];
  fread(&mb[0], 1, 3, fp);

  if (mb[0] != 0x6c || mb[1] != 0x1b || mb[2] == 0x01) {
    fprintf(stderr, RED "Invalid genotype fileset." WHITE "\n");
    exit(EXIT_FAILURE);
  }

  fread(*g, 1, w, fp);

  unsigned char ov;
  if (fread(&ov, 1, 1, fp)) {
    fprintf(stderr, RED "Invalid genotype fileset." WHITE "\n");
    exit(EXIT_FAILURE);
  }

  fclose(fp);
}

void read_txt_f4(char *f, u4_t *s, u4_t *n,
    f4_t **e) {
  u4_t l = 1048576;
  char * a = (char*) allocate_u1(l);

  n[0] = 0;
  n[1] = 0;

  FILE *fp = fopen(f, "r");
  if (!fp) {
    fprintf(stderr, RED "Failed to open %s for reading." WHITE "\n", f);
    exit(EXIT_FAILURE);
  }

  *e = allocate_f4(0);

  u4_t i = 0;
  while (fgets(a, l, fp)) {
    if (n[0] >= s[0]) {

      char *g = strtok(a, " \t\n");

      u4_t j = 0;
      u4_t k = 0;
      while (g != NULL) {
        char *p;

        f4_t q = strtof(g, &p);

        if (k >= s[1] && *p == '\0') {
          allocate_f4(1);
          (*e)[i*n[1]+j] = q;

          j++;
        }
        k++;

        g = strtok(NULL, " \t\n");
      }

      if (n[1] == 0) {
        n[1] = j;
      }

      i++;
    }

    n[0]++;
  }
}

void read_txt_f8(char *f, u4_t *s, u4_t *n,
    double **e) {
  u4_t l = 1048576;
  char * a = (char*) allocate_u1(l);

  n[0] = 0;
  n[1] = 0;

  FILE *fp = fopen(f, "r");
  if (!fp) {
    fprintf(stderr, RED "Failed to open %s for reading." WHITE "\n", f);
    exit(EXIT_FAILURE);
  }

  *e = allocate_f8(0);

  u4_t i = 0;
  while (fgets(a, l, fp)) {
    if (n[0] >= s[0]) {

      char *g = strtok(a, " \t\n");

      u4_t j = 0;
      u4_t k = 0;
      while (g != NULL) {
        char *p;

        double q = strtod(g, &p);

        if (k >= s[1] && *p == '\0') {
          allocate_f8(1);
          (*e)[i*n[1]+j] = q;

          j++;
        }
        k++;

        g = strtok(NULL, " \t\n");
      }

      if (n[1] == 0) {
        n[1] = j;
      }

      i++;
    }

    n[0]++;
  }
}

void read_binaryf8_f4(char* f, u4_t *n,
  f4_t **r) {

  int fd = open(f, O_RDONLY, 0);
  if (fd == -1) {
    fprintf(stderr, RED "Failed to open %s for reading." WHITE "\n", f);
    exit(EXIT_FAILURE);
  }

  size_t fs = *n * sizeof(double);
  void *fmap = mmap(NULL, fs, PROT_READ, MAP_PRIVATE, fd, 0);
  if (fmap == MAP_FAILED) {
    fprintf(stderr, RED "Failed map memory for %s for reading." WHITE "\n", f);
    exit(EXIT_FAILURE);
  }

  double *fmapd = (double*) fmap;

  *r = allocate_f4(*n);
  for (u4_t i = 0; i < *n; ++i) {
    (*r)[i] = (f4_t) fmapd[i];
  }

  int rc = munmap(fmap, fs);
  if (rc != 0) {
    fprintf(stderr, RED "Failed unmap memory for %s after reading." WHITE "\n", f);
    exit(EXIT_FAILURE);
  }
  close(fd);
}

void read_binaryf8_f8(char* f, u4_t *n,
  double **r) {

  FILE *fp = fopen(f, "rb");
  if (!fp) {
    fprintf(stderr, RED "Failed to open %s for reading." WHITE "\n", f);
    exit(EXIT_FAILURE);
  }

  *r = allocate_f8(*n);

  fread(*r, *n * sizeof(double), 1, fp);

  fclose(fp);
}
