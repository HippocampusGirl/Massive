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

#ifndef __M_COMMON_VECTOR_H__
#define __M_COMMON_VECTOR_H__

#include <x86intrin.h>

#include "m_common.h"

#ifdef __AVX512F__
 typedef __m512 vf4_t;
 typedef __m512i vu4_t;
#elif defined(__AVX2__)
  typedef __m256 vf4_t;
  typedef __m256i vu4_t;
#else
  #error
#endif

/*
d - doublewords
b - bytes
*/

#define vb sizeof(vu4_t)
#define vd (vb/sizeof(u4_t))
#define clb 64

#ifdef __INTEL_COMPILER
typedef u4_t * au4_ptr __attribute__((align_value(clb)));
typedef f4_t * af4_ptr __attribute__((align_value(clb)));
typedef double * af8_ptr __attribute__((align_value(clb)));
#else
typedef u4_t * au4_ptr __attribute__((__aligned__(clb)));
typedef f4_t * af4_ptr __attribute__((__aligned__(clb)));
typedef double * af8_ptr __attribute__((__aligned__(clb)));
#endif

#endif
