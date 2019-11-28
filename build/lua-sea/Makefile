LIBNAME = sea_c
LUADIR = ../../../skynet/3rd/lua

COPT = -O2
# COPT = -DLPEG_DEBUG -g

CWARNS = -Wall -Wextra -pedantic \
	-Waggregate-return \
	-Wcast-align \
	-Wcast-qual \
	-Wdisabled-optimization \
	-Wpointer-arith \
	-Wshadow \
	-Wsign-compare \
	-Wundef \
	-Wwrite-strings \
	-Wbad-function-cast \
	-Wdeclaration-after-statement \
	-Wmissing-prototypes \
	-Wnested-externs \
	-Wstrict-prototypes \
# -Wunreachable-code \

CFLAGS = $(CWARNS) $(COPT) -std=c99 -I$(LUADIR) -fPIC
CC = gcc

FILES = lsea.o matrix.o

# For Linux
linux:
	make sea_c.so "DLLFLAGS = -shared -fPIC"

# For Mac OS
macosx:
	make sea_c.so "DLLFLAGS = -bundle -undefined dynamic_lookup"

sea_c.so: $(FILES)
	env $(CC) $(DLLFLAGS) $(FILES) -o ../../build/sea_c.so

$(FILES): Makefile

clean:
	rm -f $(FILES) ../../build/sea_c.so

matrix.o: matrix.c
lsea.o: lsea.c
