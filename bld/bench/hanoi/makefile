# hanoi.c Makefile.
DEFINES = -DUNIX
FLAGS = $(DEFINES) -O2
FILES = Makefile hanoi.c hanoi.res hanoi.bat

CC = gcc $(FLAGS)

hanoi		: hanoi.c Makefile 
		$(CC) hanoi.c -o hanoi 

MANIFEST	: $(FILES)
		ls -l $(FILES) > MANIFEST

shar		: $(FILES) MANIFEST
		shar -o hanoi.shar MANIFEST $(FILES)

tar		: $(FILES) MANIFEST
		tar -cf hanoi.tar MANIFEST $(FILES)
		compress hanoi.tar
		uuencode hanoi.tar.Z hanoi.tar.Z > hanoi.tar.Z.uue
