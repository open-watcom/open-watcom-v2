# clinpack.c make file.
# There are four cases to consider:
# gcc -DUNIX -DDP -DROLL
# gcc -DUNIX -DDP -DUNROLL
# gcc -DUNIX -DSP -DROLL
# gcc -DUNIX -DSP -DUNROLL
 
FILES = Makefile clinpack.c clinpack.bat

CC    = gcc -O2 -DUNIX

clinpack        : clinpack.c
		$(CC) -DDP -DROLL clinpack.c -o clinpack_dpr
		$(CC) -DDP -DUNROLL clinpack.c -o clinpack_dpu
		$(CC) -DSP -DROLL clinpack.c -o clinpack_spr
		$(CC) -DSP -DUNROLL clinpack.c -o clinpack_spu

MANIFEST	: $(FILES)
		ls -l $(FILES) > MANIFEST

shar		: $(FILES) MANIFEST
		shar -o clinpack.shar MANIFEST $(FILES)

tar		: $(FILES) MANIFEST
		tar -cf clinpack.tar MANIFEST  $(FILES)
		compress clinpack.tar
