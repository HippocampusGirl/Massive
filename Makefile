# This library is part of Massive, copyright 2017 Lea Waller.
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by the
# Free Software Foundation, either version 3 of the License, or (at your
# option) any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
# for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http:#www.gnu.org/licenses/>.

VERSION=$(shell cat VERSION)

COMPILER_STR=$(shell $(CC) -V 2>&1 | head -c3)

DEBUGFLAGS=-g -O0

ifeq ($(COMPILER_STR), gcc) # gcc
OPTFLAGS=-O3 -fopenmp -march=native -mtune=native -mieee-fp -DNDEBUG

BASEFLAGS=-g

LINKFLAGS=${OPTFLAGS} -lfslio -lniftiio -lznz -lz
endif

ifeq ($(COMPILER_STR), cla) # clang
OPTFLAGS=-O3 -march=native -mtune=native -ieee-fp -DNDEBUG

BASEFLAGS=-g -std=c99

LINKFLAGS=${OPTFLAGS} -lfslio -lniftiio -lznz -lz
endif

UNAME=$(shell uname)

# ifeq ($(UNAME), Darwin)
# LINKFLAGS+=-framework Accelerate -framework CoreFoundation
# else
LINKFLAGS+=-llapack -lopenblas
# endif

ifeq ($(COMPILER_STR), Int) # icc
OPTFLAGS=-O3 -ipo -xHOST -mtune=native -parallel -fp-model strict -DNDEBUG -qopenmp
OPTFLAGS+=-no-inline-max-total-size -inline-factor=200 -no-inline-max-per-compile

BASEFLAGS=-g -std=gnu99 -restrict -fno-alias -fno-fnalias
BASEFLAGS+=-Wall -Wextra -Wremarks
BASEFLAGS+=-mkl -D__INTEL_MKL__ -DSTATIC_ZLIB
BASEFLAGS+=-fno-omit-frame-pointer

LINKFLAGS=-g -v ${OPTFLAGS} -mkl -static-intel
ifneq ($(UNAME), Darwin)
LINKFLAGS+=-lnuma
LINKFLAGS+=-Wl,-Bstatic -lfslio -lniftiio -lznz -lz -Wl,-Bdynamic
else
LINKFLAGS+=-lgmp -lfslio -lniftiio -lznz -lz
endif
endif

ifeq ($(COMPILER_STR), Cra) # cray cc
OPTFLAGS=-O3 -h fp3 -h list=a -h nobounds -h autothread -h ipa4 -DNDEBUG

BASEFLAGS=-h msglevel_0
BASEFLAGS+=-I/home/b/bemwwlea/include -I${LIBSCI_BASE_DIR}/cray/83/haswell/include
BASEFLAGS+=-DSTATIC_ZLIB

LINKFLAGS=-L${INTEL_PATH}/linux/lib/intel64
LINKFLAGS+=-L${LIBSCI_BASE_DIR}/cray/83/haswell/lib
LINKFLAGS+=-lfslio -lniftiio -lznz -lz
LINKFLAGS+=-lsci_cray_mp -lirc -lpthread -lm
endif

BASEFLAGS+=-DVERSION=\"$(VERSION)\"

CFLAGS=${BASEFLAGS} ${OPTFLAGS}

COMMON_SRC=m_common.c m_common_io.c m_common_io_neuroimaging.c
COMMON_SRC+=m_common_memory.c m_common_sort.c
COMMON_OBJ = $(COMMON_SRC:.c=.o)

BRAINCONNECTIVITY_SRC=m_brainconnectivity_networkdefinition.c
BRAINCONNECTIVITY_SRC+=m_brainconnectivity_pathlength.c
BRAINCONNECTIVITY_SRC+=m_brainconnectivity_triangles.c m_brainconnectivity.c
BRAINCONNECTIVITY_OBJ = $(BRAINCONNECTIVITY_SRC:.c=.o)

all: m_brainconnectivity

%.o: %.c
	${CC} -c $(CFLAGS) -o $@ $<

m_brainconnectivity: $(COMMON_OBJ) $(BRAINCONNECTIVITY_OBJ)
	${CC} $(COMMON_OBJ) $(BRAINCONNECTIVITY_OBJ) -o m_brainconnectivity $(LINKFLAGS)

clean:
	rm -f $(COMMON_OBJ) $(BRAINCONNECTIVITY_OBJ) m_brainconnectivity
	rm -rf *.dSYM
