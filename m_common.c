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

#include "m_common.h"

u4_t config = 0;

u4_t debug = 0;

const char version[] = "Massive " VERSION " " __DATE__  " " __TIME__ "\n" \
"http://github.com/HippocampusGirl/Massive\n" \
"(c) 2017 Lea Waller, GNU General Public License v3.0\n\n";

void showprogress(u4_t i, u4_t n) {
  if (!debug) {
    if (i > 0) {
      printf("\r");
    }
    printf("%f%%", ((f4_t) (i+1)) / ((f4_t) n) * 100.0f);
    fflush(stdout);
    if (i >= n - 1) {
      printf("\r                                    \r");
    }
  }
}

void printmatrix(f4_t *a, u4_t n, u4_t m) {
  printf("matrix\n");

  u4_t nn = n;
  if (nn > 16) {
    nn = 16;
  }

  u4_t mm = m;
  if (mm > 16) {
    mm = 16;
  }

  for (uint32_t i = 0; i < nn; i++) {
    for (uint32_t j = 0; j < mm; j++) {
      printf("%.4f\t", a[i*n+j]);
    }
    printf("\n");
  }
  printf("\n");
}
