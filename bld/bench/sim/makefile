# Makefile for sim.
DEFINES = -v -DUNIX
FLAGS = $(DEFINES) -O2
FILES = Makefile sim.bat sim.doc simrun.doc sim.c tob.38-44 liv.42-48

GCC = gcc $(FLAGS)

sim      : sim.c
	   $(GCC) sim.c -o sim

MANIFEST : $(FILES)
	   ls -l $(FILES) > MANIFEST

shar     : $(FILES) MANIFEST
	   shar -o sim.shar MANIFEST $(FILES)

tar      : $(FILES) MANIFEST
	   tar -cf sim.tar MANIFEST $(FILES)
	   compress sim.tar
	   uuencode sim.tar.Z sim.tar.Z > sim.tar.Z.uue
