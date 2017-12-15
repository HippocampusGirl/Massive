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

#ifndef __M_COMMON_IO_H__
#define __M_COMMON_IO_H__

#include "m_common.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

void write_txt_f4(char *f, u4_t *n, f4_t *d);
void write_txt_f8(char *f, u4_t *n, double *d);

void write_ntxt_f4(char *f, u4_t *n, f4_t *d, char **cn, char **rn, u4_t *nn);

void read_genotype(char *b, u4_t *n, u4_t **g);

void read_txt_f4(char *f, u4_t *s, u4_t *n, f4_t **d);
void read_txt_f8(char *f, u4_t *s, u4_t *n, double **d);

void read_binaryf8_f4(char* f, u4_t *n, f4_t **r);
void read_binaryf8_f8(char* f, u4_t *n, double **r);

#endif
