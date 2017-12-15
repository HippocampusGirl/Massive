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

#include "m_brainconnectivity_networkdefinition.h"

void cov(f4_t * restrict x, f4_t * restrict c,
  u4_t n, u4_t m) {

  char uplo = 'l';
  char ttrans = 't';

  integer nn = n;
  integer mm = m;

  f4_t alpha = 1.0f / ((f4_t) (m - 1));
  f4_t beta = 0.0f;

  FORTRAN_WRAPPER(ssyrk)(
        &uplo, // c = x * x'
        &ttrans,
        &nn,
        &mm,
        &alpha,
        x,
        &mm,
        &beta,
        c,
        &nn);

  for (u4_t i = 0; i < n; i++) {
    for (u4_t j = 0; j < i; j++) {
      c[i*n+j] = c[j*n+i];
    }
  }
}

void ridgecov(f4_t * restrict x, f4_t * restrict c,
  f4_t rho,
  u4_t n, u4_t m) {

  char uplo = 'l';
  char ttrans = 't';

  integer nn = n;
  integer mm = m;

  f4_t alpha = 1.0f / ((f4_t) (m - 1));
  f4_t beta = 0.0f;

  FORTRAN_WRAPPER(ssyrk)(
        &uplo, // c = x * x'
        &ttrans,
        &nn,
        &mm,
        &alpha,
        x,
        &mm,
        &beta,
        c,
        &nn);

  f4_t ms = 0.0f;
  for (u4_t i = 0; i < n; i++) {
    ms += c[i*n+i] * c[i*n+i];
  }
  ms = 1.0f / sqrt(ms / n);

  double * restrict a = allocate_f8(n*n);

  for (u4_t i = 0; i < n; i++) {
    for (u4_t j = 0; j <= i; j++) {
      double cc = (double) c[j*n+i] * ms;
      a[i*n+j] = cc;
      a[j*n+i] = cc;
    }
  }

  for (u4_t i = 0; i < n; i++) {
    a[i*n+i] += rho;
  }

  integer info;

  double * restrict b = allocate_f8(n*n);
  for (u4_t i = 0; i < n; i++) {
    b[i*n+i] = 1.0f;
  }

  int * restrict ipiv = (int*) allocate_u8(n);

  FORTRAN_WRAPPER(dgesv)(
        &nn,
        &nn,
        a, // a = inv(a)
        &nn,
        ipiv,
        b,
        &nn,
        &info);

  if (info != 0) {
    exit(EXIT_FAILURE);
  }

  for (u4_t i = 0; i < n; i++) {
    for (u4_t j = 0; j < n; j++) {
      c[i*n+j] = -1.0f * (f4_t) b[i*n+j];
    }
  }

  free_u8(n);
  free_f8(n*n);
  free_f8(n*n);
}

void cov2corr(f4_t * restrict c,
  u4_t n) {
  for (u4_t i = 0; i < n; i++) {
    if (fabsf(c[i*n+i]) > FLT_EPSILON) {
      c[i*n+i] = 1.0f / sqrtf(fabsf(c[i*n+i]));
    } else {
      c[i*n+i] = 1.0f;
    }
  }

  for (u4_t i = 0; i < n; i++) {
    for (u4_t j = 0; j < n; j++) {
      if (i != j) {
        c[i*n+j] *= c[i*n+i] * c[j*n+j];
      }
    }
  }

  for (u4_t i = 0; i < n; i++) {
    c[i*n+i] = 0.0f;
  }
}

void corr2z(f4_t * restrict c,
  u4_t n) {
  for (u4_t i = 0; i < n; i++) {
    for (u4_t j = 0; j < n; j++) {
      c[i*n+j] = atanhf(c[i*n+j]);
    }
  }
}

void proportional2absolutethreshold(f4_t * restrict c, f4_t * restrict t,
  u4_t n, u4_t m) {

  pquicksort(c, n*n);

  for (u4_t i = 0; i < m; i++) {
    u4_t k = (u4_t) (t[i] * (f4_t) (n*n));
    int q = ((int) n*n) - (int) k;
    if (q < 0) {
      q = 0;
    }
    t[i] = c[q];
  }
}

void applyabsolutethreshold(f4_t * restrict c, f4_t t,
  u4_t n) {
  #pragma omp parallel for simd
  for (u4_t i = 0; i < n*n; i++) {
    if (c[i] < t) {
      c[i] = 0.0f;
    }
  }
}
