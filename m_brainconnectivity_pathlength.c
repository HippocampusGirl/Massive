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

#include "m_brainconnectivity_pathlength.h"

static u4_t const block_size = 128;

void floydwarshall(af4_ptr restrict c, u4_t n) {
  for (u4_t k = 0; k < n; k++) {
    #pragma omp parallel for
    for (u4_t i = 0; i < n; i++) {
      #pragma omp simd
      for (u4_t j = 0; j < n; j++) {
        f4_t d = c[i*n+k]+c[k*n+j];
        if (d < c[i*n+j]) {
          c[i*n+j] = d;
        }
      }
    }
  }
}

static inline void sub1(af4_ptr restrict c) {
  for (u4_t k = 0; k < block_size; k++) {
    for (u4_t i = 0; i < block_size; i++) {
      #pragma omp simd
      for (u4_t j = 0; j < block_size; j++) {
        f4_t d = c[i*block_size+k]+c[k*block_size+j];
        if (d < c[i*block_size+j]) {
          c[i*block_size+j] = d;
        }
      }
    }
  }
}

static inline void sub2(af4_ptr restrict a,
  af4_ptr restrict c) {
  for (u4_t k = 0; k < block_size; k++) {
    for (u4_t i = 0; i < block_size; i++) {
      #pragma omp simd
      for (u4_t j = 0; j < block_size; j++) {
        f4_t d = a[i*block_size+k]+c[k*block_size+j];
        if (d < c[i*block_size+j]) {
          c[i*block_size+j] = d;
        }
      }
    }
  }
}

static inline void sub3(af4_ptr restrict b,
  af4_ptr restrict c) {
  for (u4_t k = 0; k < block_size; k++) {
    for (u4_t i = 0; i < block_size; i++) {
      #pragma omp simd
      for (u4_t j = 0; j < block_size; j++) {
        f4_t d = c[i*block_size+k]+b[k*block_size+j];
        if (d < c[i*block_size+j]) {
          c[i*block_size+j] = d;
        }
      }
    }
  }
}

static inline void sub4(af4_ptr restrict a,
  af4_ptr restrict b, af4_ptr restrict c) {
  // this is where most of the computation happens,
  // and chould be further optimized in assembly....

  for (u4_t i = 0; i < block_size; i++) {
    for (u4_t k = 0; k < block_size; k++) {
      for (u4_t j = 0; j < block_size; j++) {
        f4_t d = a[i*block_size+k]+b[k*block_size+j];
        if (d < c[i*block_size+j]) {
          c[i*block_size+j] = d;
        }
      }
    }
  }
}

void blockfloydwarshall(f4_t * restrict c,
  u4_t n) {

  u4_t const m = DIV_UP(n, block_size);
  u4_t const p = m*block_size;

  clalign_stack();
  af4_ptr restrict d = allocate_f4(p*p);
  for (u4_t i = 0; i < p*p; i++) {
    d[i] = INFINITY;
  }

  // pack

  for (u4_t i = 0; i < n; i++) {
    for (u4_t j = 0; j < n; j++) {
      u4_t x = i / block_size;
      u4_t y = j / block_size;

      u4_t a = i % block_size;
      u4_t b = j % block_size;

      d[((x*m+y)*block_size+a)*block_size+b] = c[i*n+j];
    }
  }

  // compute

  #pragma omp parallel shared(d)
  #pragma omp single nowait
  for (u4_t k = 0; k < m; k++) {
    #pragma omp task depend(inout:d[(k*m+k)*block_size*block_size])
    sub1(&d[(k*m+k)*block_size*block_size]);

    for (u4_t i = 0; i < m; i++) {
      if (i != k) {
        #pragma omp task depend(in:d[(k*m+k)*block_size*block_size]) depend(inout:d[(k*m+i)*block_size*block_size])
        sub2(&d[(k*m+k)*block_size*block_size], &d[(k*m+i)*block_size*block_size]);
        #pragma omp task depend(in:d[(k*m+k)*block_size*block_size]) depend(inout:d[(i*m+k)*block_size*block_size])
        sub3(&d[(k*m+k)*block_size*block_size], &d[(i*m+k)*block_size*block_size]);
      }
    }

    for (u4_t i = 0; i < m; i++) {
      for (u4_t j = 0; j < m; j++) {
        if (i != k && j != k) {
          #pragma omp task depend(inout:d[(i*m+k)*block_size*block_size],d[(k*m+j)*block_size*block_size],d[(i*m+j)*block_size*block_size])
          sub4(&d[(i*m+k)*block_size*block_size], &d[(k*m+j)*block_size*block_size], &d[(i*m+j)*block_size*block_size]);
        }
      }
    }
  }

  // unpack

  for (u4_t i = 0; i < n; i++) {
    for (u4_t j = 0; j < n; j++) {
      u4_t x = i / block_size;
      u4_t y = j / block_size;

      u4_t a = i % block_size;
      u4_t b = j % block_size;

      c[i*n+j] = d[((x*m+y)*block_size+a)*block_size+b];
    }
  }

  free_f4(p*p);
}

void pathlength(f4_t * restrict c, // input matrix
  f4_t * restrict eg, f4_t * restrict el, f4_t * restrict cpg,
  u4_t n) {

  #pragma omp parallel for
  for (u4_t i = 0; i < n; i++) {
    #pragma omp simd
    for (u4_t j = 0; j < n; j++) {
      if (fabsf(c[i*n+j]) < FLT_EPSILON) {
        c[i*n+j] = INFINITY;
      } else {
        c[i*n+j] = 1.0f / c[i*n+j];
      }
    }
  }
  for (u4_t i = 0; i < n; i++) {
    c[i*n+i] = 0.0f;
  }

  if (debug) {
    printmatrix(c, n, n);
  }

  blockfloydwarshall(c, n);

  if (debug) {
    printmatrix(c, n, n);
  }

  // if (el) {
  //   for (u4_t i = 0; i < n; i++) {
  //     el[i] = 0.0f;
  //   }
  // }

  u4_t k = 0;
  float es = 0.0f;
  float cps = 0.0f;

  #pragma omp parallel for reduction(+:es,cps)
  for (u4_t i = 0; i < n; i++) {
    for (u4_t j = 0; j < n; j++) {
      if (!isinf(c[i*n+j])) {
        cps += c[i*n+j];
        k++;
      }
      if (i != j && fabsf(c[i*n+j]) > FLT_EPSILON) {
        es += 1.0f / c[i*n+j];
      }
      // if (el) {
      //   if (i != j && fabsf(c[i*n+j]) > FLT_EPSILON) {
      //     el[i] += (1.0f / c[i*n+j]) * ;
      //   }
      // }
    }
  }

  es /= (f4_t) (n * (n - 1));
  cps /= (f4_t) k;

  if (debug) {
    printf("es %f cps %f\n", es, cps);
  }

  if (eg) {
    *eg = es;
  }

  if (cpg) {
    *cpg = cps;
  }

  // if (el) {
  //   for (u4_t i = 0; i < n; i++) {
  //     el[i] /= (f4_t) (n * (n - 1));
  //   }
  // }
}
