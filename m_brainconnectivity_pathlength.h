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

#ifndef __M_BRAINCONNECTIVITY_PATHLENGTH_H__
#define __M_BRAINCONNECTIVITY_PATHLENGTH_H__

#include "m_common.h"

#ifdef _OPENMP
  #include <omp.h>
#endif

void floydwarshall(af4_ptr restrict c, u4_t n);
void blockfloydwarshall(f4_t * restrict c, u4_t n);

void pathlength(f4_t * restrict c, // input matrix
  f4_t * restrict eg, f4_t * restrict el, f4_t * restrict cpg,
  u4_t n);

#endif
