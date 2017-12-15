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

#include "m_common_sort.h"

static inline void swap(f4_t * a,
  u4_t i, u4_t j) {
  f4_t b = a[i];
  a[i] = a[j];
  a[j] = b;
}

static inline u4_t partition(f4_t * restrict a,
  u4_t l, u4_t h) {

  f4_t p = a[l];
  u4_t i = l;
  u4_t j = h+1;
  for (;;) {
    do {
      i++;
    } while (a[i] <= p && i <= h);

    do {
      j--;
    } while (a[j] > p);

    if (i >= j) {
      break;
    }

    swap(a, i, j);
  }

  swap(a, l, j);

  return j;
}

static inline void recurrence(f4_t * restrict a,
  u4_t l, u4_t h, u4_t o) {

  if (l < h) {
    u4_t p = partition(a, l, h);

    if (h-l < o) {
      recurrence(a, l, p-1, o);
      recurrence(a, p+1, h, o);
    } else {
      #pragma omp task
      recurrence(a, l, p-1, o);
      #pragma omp task
      recurrence(a, p+1, h, o);
      #pragma omp taskwait
    }
  }
}

void pquicksort(f4_t * restrict a,
  u4_t n) {

  #pragma omp parallel shared(a)
  #pragma omp single nowait
  {
    u4_t o = DIV_UP(n, omp_get_num_threads());
    recurrence(a, 0, n-1, o);
  }
}

static int compare(const void *aa, const void *bb) {
  float a = *((float*) aa);
  float b = *((float*) bb);

  if (a > b) {
    return -1;
  } else if (a < b) {
    return 1;
  } else {
    return 0;
  }
}

void quicksort(f4_t * restrict a,
  u4_t n) {

  qsort(a, n, sizeof(f4_t), compare);
}

void argsort(u4_t * restrict a,
  f4_t * restrict b,
  u4_t n) {

  for (u4_t i = 0; i < (int) n; i++) {
    a[i] = i;
  }

  for (int i = 0; i < (int) n - 1; i++) {
    for (int j = 0; j < (int) n - i - 1; j++) {
      if (b[a[j]] > b[a[j+1]]) {
        u4_t s = a[j];
        a[j] = a[j+1];
        a[j+1] = s;
      }
    }
  }
}
