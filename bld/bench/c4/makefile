# current options include SMALL and MOD64.
DEFINES = -DUNIX -DSMALL
FLAGS = -O2 $(DEFINES)
FILES = Makefile README input c4.bat c4.c c4.h c4run.doc \
	c4.doc c4.res best.c best.h mach.c play.c play.h trans.c

CC = gcc $(FLAGS)

c4              : c4.o trans.o best.o play.o mach.o
		$(CC) -o c4 c4.o trans.o best.o play.o mach.o

c4.o            : c4.c c4.h Makefile
		$(CC) -c c4.c

trans.o         : trans.c c4.h Makefile
		$(CC) -c trans.c

best.o          : best.c c4.h best.h Makefile
		$(CC) -c best.c

play.o          : play.c c4.h play.h Makefile
		$(CC) -c play.c

mach.o          : mach.c Makefile
		$(CC) -c mach.c

# use the following for optimization levels that preclude separate compilation
together        : c4.c c4.h trans.c best.c best.h play.c play.h mach.c Makefile
		$(CC) -o c4 c4.c trans.c best.c play.c mach.c

MANIFEST        : $(FILES)
		ls -l $(FILES) > MANIFEST

shar            : $(FILES) MANIFEST
		shar -o c4.shar MANIFEST $(FILES)

tar             : $(FILES) MANIFEST
		tar -cf c4.tar MANIFEST  $(FILES)
		compress c4.tar
