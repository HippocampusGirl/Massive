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

#ifndef __M_COMMON_H__
#define __M_COMMON_H__

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <stdio.h>
#include <string.h>

#include <math.h>
#include <float.h>

#include <unistd.h>

#include "m_common_types.h"
#include "m_common_vector.h"
#include "m_common_memory.h"
#include "m_common_matrix.h"
#include "m_common_io.h"
#include "m_common_io_neuroimaging.h"
#include "m_common_sort.h"

extern u4_t config;
extern u4_t debug;

// taken from https://stackoverflow.com/questions/3219393/stdlib-and-colored-output-in-c
#define RED "\x1b[31m"
#define WHITE "\x1b[0m"

// taken from https://github.com/chrchang/plink-ng, plink2/pgenlib_internal.h
#define DIV_UP(val, divisor) (((val) + (divisor) - 1) / (divisor))

void showprogress(u4_t i, u4_t n);
void printmatrix(f4_t *a, u4_t n, u4_t m);

extern const char version[];

#endif
