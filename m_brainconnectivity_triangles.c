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

#include "m_brainconnectivity_triangles.h"

void triangles(f4_t * restrict c,
  f4_t * restrict cg, f4_t * restrict cl,
  u4_t n) {

  #pragma omp parallel for
  for (u4_t i = 0; i < n; i++) {
    #pragma omp simd
    for (u4_t j = 0; j < n; j++) {
      c[i*n+j] = powf(c[i*n+j], 1.0f / 3.0f);
    }
  }

  for (u4_t i = 0; i < n; i++) {
    c[i*n+i] = 0.0f;
  }

  char uplo = 'l';

  integer nn = n;

  f4_t alpha = 1.0f;
  f4_t beta = 0.0f;

  f4_t * v = allocate_f4(n*n);

  FORTRAN_WRAPPER(ssymm)(
        &uplo,
        &uplo,
        &nn,
        &nn,
        &alpha,
        c,
        &nn,
        c,
        &nn,
        &beta,
        v,
        &nn);

  f4_t cgg = 0.0f;

  #pragma omp parallel for reduction(+:cgg)
  for (u4_t i = 0; i < n; i++) {
    f4_t f = 0.0f;
    u4_t k = 0;
    #pragma omp simd
    for (u4_t j = 0; j < n; j++) {
      f += c[i*n+j] * v[i*n+j];
      if (fabsf(c[i*n+j]) > FLT_EPSILON) {
        k++;
      }
    }
    if (k > 0 && fabsf(f) > FLT_EPSILON) {
      float cll = f / ((f4_t) (k * (k - 1)));

      cgg += cll;

      if (cl) {
        cl[i] = cll;
      }
    }
  }

  if (cg) {
    *cg = cgg / (f4_t) n;
  }

  free_f4(n*n);
}
