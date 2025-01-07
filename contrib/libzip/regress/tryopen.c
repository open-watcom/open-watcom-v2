/*
  tryopen.c -- tool for tests that try opening zip archives
  Copyright (C) 1999-2011 Dieter Baron and Thomas Klausner

  This file is part of libzip, a library to manipulate ZIP archives.
  The authors can be contacted at <libzip@nih.at>

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
  3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.
 
  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef HAVE_GETOPT
#include "getopt.h"
#endif
#ifndef WIN32
#include <unistd.h>
#endif

#include "zip.h"

const char *prg;

const char *usage = "usage: %s [-cen] file\n";



int
main(int argc, char *argv[])
{
    const char *fname;
    struct zip *z;
    int c, count, flags, ze;

    flags = 0;
    prg = argv[0];

    while ((c=getopt(argc, argv, "cen")) != -1) {
	switch (c) {
	case 'c':
	    flags |= ZIP_CHECKCONS;
	    break;
	case 'e':
	    flags |= ZIP_EXCL;
	    break;
	case 'n':
	    flags |= ZIP_CREATE;
	    break;

	default:
	    fprintf(stderr, usage, prg);
	    return 1;
	}
    }
    if (argc != optind+1) {
	fprintf(stderr, usage, prg);
	return 1;
    }

    fname = argv[optind];
    errno = 0;

    if ((z=zip_open(fname, flags, &ze)) != NULL) {
	count = zip_get_num_files(z);
	printf("opening `%s' succeeded, %d entries\n", fname, count);
	zip_close(z);
	return 0;
    }

    printf("opening `%s' returned error %d", fname, ze);
    if (zip_error_get_sys_type(ze) == ZIP_ET_SYS)
		printf("/%d", errno);
    printf("\n");
    return 1;
}
