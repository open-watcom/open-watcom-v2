/*
  $NiH: buffadd.c,v 1.11 2005/06/09 20:25:39 wiz Exp $

  buffadd.c -- test cases for adding files from buffer
  Copyright (C) 1999, 2003, 2005 Dieter Baron and Thomas Klausner

  This file is part of libzip, a library to manipulate ZIP archives.
  The authors can be contacted at <nih@giga.or.at>

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
#include <string.h>

#include "zip.h"

char *teststr="This is a test, and it seems to have been successful.\n";
char *testname="testfile.txt";
char *testzip="test_zip.zip";

int
main(int argc, char *argv[])
{
    struct zip *za;
    struct zip_file *zf;
    struct zip_source *zs;
    int err;
    int len;
    
    char buf[2000];

    remove(testzip);
    
    if ((za=zip_open(testzip, ZIP_CREATE, &err)) == NULL) {
	zip_error_to_str(buf, sizeof(buf), err, errno);
	fprintf(stderr,"%s: can't open zip archive %s: %s\n", argv[0],
		testzip, buf);
	exit(1);
    }

    if ((zs=zip_source_buffer(za, teststr, strlen(teststr), 0)) == NULL
	|| zip_add(za, testname, zs) == -1) {
	zip_source_free(zs);
	fprintf(stderr,"%s: can't add buffer '%s': %s\n", argv[0],
		teststr, zip_strerror(za));
	exit(1);
    }

    if (zip_close(za) == -1) {
	fprintf(stderr,"%s: can't close zip archive %s\n", argv[0],
		testzip);
	exit(1);
    }

    if ((za=zip_open(testzip, ZIP_CHECKCONS, &err))==NULL) {
	zip_error_to_str(buf, sizeof(buf), err, errno);
	fprintf(stderr,"%s: can't re-open zip archive %s: %s\n", argv[0],
		testzip, buf);
	exit(1);
    }

    if ((zf=zip_fopen(za, testname, 0))==NULL) {
	fprintf(stderr,"%s: can't fopen file '%s' in '%s': %s\n", argv[0],
		testname, testzip, zip_strerror(za));
	exit(1);
    }

    if ((len=zip_fread(zf, buf, 2000)) < 0) {
	fprintf(stderr,"%s: can't read from '%s' in zip archive '%s': %s\n",
		argv[0], testname, testzip, zip_file_strerror(zf));
	exit(1);
    }
    
    zip_fclose(zf);
    zf = zip_fopen(za, testname, 0);

    /* not NUL-terminated, so we have to check length manually */
    if (len != strlen(teststr) || strncmp(buf, teststr, len)) {
	fprintf(stderr,"%s: wrong data: '%s' instead of '%s'\n", argv[0],
		buf, teststr);
	exit(1);
    }

    if (zip_close(za) == -1) {
	fprintf(stderr,"%s: can't close zip archive %s\n", argv[0],
		testzip);
	exit(1);
    }

    buf[0] = '\0';
    if ((err=zip_fread(zf, buf, 2000)) != -1) {
	fprintf(stderr,"%s: can read from '%s' in closed zip archive '%s': %s\n",
		argv[0], testname, testzip, zip_file_strerror(zf));
	exit(1);
    }
    
    remove(testzip);
    
    return 0;
}
