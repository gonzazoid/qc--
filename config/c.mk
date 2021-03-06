# ------------------------------------------------------------------ 
# $Id$
#
# rules and tools to compile C source code
# ------------------------------------------------------------------ 

CC =        $config_cc
CFLAGS =    -Wall

%.o:        %.c
	$CC $CFLAGS -c $stem.c -o $stem.o

%.a:
	ar cr $target $prereq
