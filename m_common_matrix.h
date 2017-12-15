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

#ifndef __M_COMMON_MATRIX_H__
#define __M_COMMON_MATRIX_H__

#ifdef __INTEL_MKL__
  #ifndef MKL_DIRECT_CALL
    #define MKL_DIRECT_CALL
  #endif
	#include <mkl.h>

	typedef MKL_INT integer;
  #define FORTRAN_WRAPPER(x) x
// #elif defined(__APPLE__)
//   #include <Accelerate/Accelerate.h>
//   #define FORTRAN_WRAPPER(x) x ## _
#else
	#include <cblas.h>
	#include <lapacke.h>
	typedef lapack_int integer;
  #define FORTRAN_WRAPPER(x) x ## _
#endif

#endif
