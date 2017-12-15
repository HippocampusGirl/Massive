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

#ifndef __M_COMMON_TYPES_H__
#define __M_COMMON_TYPES_H__

typedef float f4_t;
typedef double f8_t;

#if defined(__FLOAT128__) || !defined(__clang__)
  typedef __float128 f16_t;
#endif

typedef uint32_t u4_t;
typedef uint64_t u8_t;

#endif
