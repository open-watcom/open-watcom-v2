/*
  add_from_zip.c -- test case for adding file from archive to archive
  Copyright (C) 2010 Dieter Baron and Thomas Klausner

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
#include <string.h>

#include "zip.h"

static const char *prg;

int
main(int argc, char *argv[])
{
    const char *source;
    const char *archive;
    const char *file;
    int file_idx;
    struct zip *z_in;
    struct zip *z_out;
    struct zip_source *zs;
    char buf[100];
    int err;

    prg = argv[0];

    if (argc != 4) {
	fprintf(stderr, "usage: %s source-archive target-archive file\n", prg);
	return 1;
    }

    source = argv[1];
    archive = argv[2];
    file = argv[3];

    if ((z_in=zip_open(source, ZIP_CHECKCONS, &err)) == NULL) {
	zip_error_to_str(buf, sizeof(buf), err, errno);
	fprintf(stderr, "%s: can't open source zip archive %s: %s\n", prg,
		source, buf);
	return 1;
    }

    if ((file_idx=zip_name_locate(z_in, file, 0)) == -1) {
	zip_error_to_str(buf, sizeof(buf), err, errno);
	fprintf(stderr, "%s: can't find file `%s' in source zip archive %s: %s\n",
		prg, file, source, buf);
	return 1;
    }

    if ((z_out=zip_open(archive, ZIP_CREATE, &err)) == NULL) {
	zip_error_to_str(buf, sizeof(buf), err, errno);
	fprintf(stderr, "%s: can't open target zip archive %s: %s\n", prg,
		archive, buf);
	return 1;
    }

    if ((zs=zip_source_zip(z_out, z_in, file_idx, 0, 0, -1)) == NULL) {
	fprintf(stderr, "%s: error creating file source for `%s': %s\n", prg,
		file, zip_strerror(z_out));
	return 1;
    }

    if (zip_add(z_out, file, zs) == -1) {
	zip_source_free(zs);
	fprintf(stderr, "%s: can't add file `%s': %s\n", prg,
		file, zip_strerror(z_out));
	return 1;
    }

    if (zip_close(z_out) == -1) {
	fprintf(stderr, "%s: can't close zip archive `%s'\n", prg,
		archive);
	return 1;
    }

    return 0;
}
