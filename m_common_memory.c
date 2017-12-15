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

#include "m_common_memory.h"

unsigned char* stack_begin;
unsigned char* stack_end;

/* this is based on bigstack as implemented by
   https://github.com/chrchang/plink-ng, plink2/pgenlib_internal.h
   but much more basic
*/

void allocate_stack() {
  u4_t s;

  const char *u = getenv("MASSIVE_STACKSIZE");
  if (u) {
    s = atoi(u);
  } else {
    s = 2048;
  }

  size_t b = (size_t) s * 1024 * 1024;
  #ifndef __APPLE__
  	stack_begin = (unsigned char*) numa_alloc_local(b);
    if (!stack_begin) {
      fprintf(stderr, RED "Error: failed to allocate %u MB of stack for team %d on node %d.\n" WHITE, s, omp_get_thread_num(), numa_node_of_cpu(sched_getcpu()));
      exit(EXIT_FAILURE);
    } else {
      fprintf(stderr, "Allocated %u MB of stack for team %d on node %d.\n", s, omp_get_thread_num(), numa_node_of_cpu(sched_getcpu()));
    }
  #else
    stack_begin = (unsigned char*) malloc(b);
    if (!stack_begin) {
      fprintf(stderr, RED "Error: failed to allocate %u MB of stack for team %d.\n" WHITE, s, omp_get_thread_num());
      exit(EXIT_FAILURE);
    } else {
      fprintf(stderr, "Allocated %u MB of stack for team %d.\n", s, omp_get_thread_num());
    }
  #endif
  stack_end = stack_begin + b;
}

// taken from https://github.com/chrchang/plink-ng, plink2/pgenlib_internal.h
static inline uintptr_t round_up_pow2(uintptr_t val, uintptr_t alignment) {
  uintptr_t alignment_m1 = alignment - 1;
  if (alignment & alignment_m1) {
    fprintf(stderr, RED "Error: invalid alignment. \n" WHITE);
    exit(EXIT_FAILURE);
  }
  return (val + alignment_m1) & (~alignment_m1);
}

void clalign_stack() {
  stack_begin = (unsigned char*) round_up_pow2((uintptr_t) stack_begin, clb);
  if (stack_begin > stack_end) {
    fprintf(stderr, RED "Error: stack overflow, please increase stack size using MASSIVE_STACKSIZE. \n" WHITE);
    exit(EXIT_FAILURE);
  }
}

unsigned char* allocate_u1(size_t s) {
  unsigned char* p = stack_begin;
  stack_begin += s;
  if (stack_begin > stack_end) {
    fprintf(stderr, RED "Error: stack overflow, please increase stack size using MASSIVE_STACKSIZE. \n" WHITE);
    exit(EXIT_FAILURE);
  }
  return p;
}

void free_u1(size_t s) {
  stack_begin -= s;
}

f4_t* allocate_f4(size_t s) {
  return (f4_t*) allocate_u1(s * sizeof(f4_t));
}

void free_f4(size_t s) {
  free_u1(s * sizeof(f4_t));
}

f8_t* allocate_f8(size_t s) {
  return (f8_t*) allocate_u1(s * sizeof(f8_t));
}

void free_f8(size_t s) {
  free_u1(s * sizeof(f8_t));
}

u4_t* allocate_u4(size_t s) {
  return (u4_t*) allocate_u1(s * sizeof(u4_t));
}

void free_u4(size_t s) {
  free_u1(s * sizeof(u4_t));
}

u8_t* allocate_u8(size_t s) {
  return (u8_t*) allocate_u1(s * sizeof(u8_t));
}

void free_u8(size_t s) {
  free_u1(s * sizeof(u8_t));
}

void** allocate_ptr(size_t s) {
  return (void**) allocate_u1(s * sizeof(void*));
}

void free_ptr(size_t s) {
  free_u1(s * sizeof(void*));
}
