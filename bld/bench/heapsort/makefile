# heapsort.c Makefile.
DEFINES = -DUNIX
FLAGS = $(DEFINES) -O2
FILES = Makefile heapsort.c heapsort.res heapsort.bat

CC = gcc $(FLAGS)

heapsort	: heapsort.c Makefile 
		$(CC) heapsort.c -o heapsort 

MANIFEST	: $(FILES)
		ls -l $(FILES) > MANIFEST

shar		: $(FILES) MANIFEST
		shar -o heapsort.shar MANIFEST $(FILES)

tar		: $(FILES) MANIFEST
		tar -cf heapsort.tar MANIFEST $(FILES)
		compress heapsort.tar
		uuencode heapsort.tar.Z heapsort.tar.Z > heapsort.tar.Z.uue
