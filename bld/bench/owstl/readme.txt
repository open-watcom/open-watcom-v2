
OWSTL Benchmark README
======================

This directory contains benchmark programs that exercise various aspects of
Open Watcom STL. The primary intent of these programs is to provide data
for the profiler so that "performance bugs" can be noticed and addressed.
In addition it is hoped that these programs will allow interesting and
useful comparisons between compilers to be made.

These programs are not intended to exercise the facilities of OWSTL in a
realistic way. Instead each program is focused on one individual facility
(or on a small set of closely related facilities) with the intent of
helping that one facility become optimized.

Unlike the other benchmarks, the makefile here only compiles the
programs. It does not run them. The idea is that each program would be
run individually as needed. It might make sense to have a make option
that would run all the programs under wsample, but probably that option
shouldn't be the default.

Note that the programs here are compiled with the -d2 option. This inhibits
optimization, but it is necessary to get good results in the profiler (an
unfortunate, but probably inescapable, circumstance).

The programs are intended to be highly portable so that they can be
compiled with a wide variety of compilers. Note, however, that some of the
program use a custom timer function. For that function to operate correctly
the environment variable PROCESSOR_SPEED needs to be set to the processor
speed in MHz (e.g. 3000 for 3GHz). To compile such programs, first build
the support library in %DEV_DIR%\bench\support.
