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

#ifndef __M_COMMON_MEMORY_H__
#define __M_COMMON_MEMORY_H__

#include "m_common.h"

#ifdef _OPENMP
  #include <omp.h>
#endif

#ifndef __APPLE__
  #include <sched.h>
  #include <numa.h>
#endif

extern unsigned char* stack_begin;
extern unsigned char* stack_end;
#pragma omp threadprivate(stack_begin, stack_end)

void allocate_stack();
void clalign_stack();

unsigned char* allocate_u1(size_t s);
void free_u1(size_t s);

f4_t* allocate_f4(size_t s);
void free_f4(size_t s);

f8_t* allocate_f8(size_t s);
void free_f8(size_t s);

u4_t* allocate_u4(size_t s);
void free_u4(size_t s);

u8_t* allocate_u8(size_t s);
void free_u8(size_t s);

void** allocate_ptr(size_t s);
void free_ptr(size_t s);

#endif
