/*
  zipmerge.c -- merge zip archives
  Copyright (C) 2004-2008 Dieter Baron and Thomas Klausner

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



#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifndef HAVE_GETOPT
#include "getopt.h"
#endif

#include "zip.h"



char *prg;

#define PROGRAM	"zipmerge"

char *usage = "usage: %s [-DhIiSsV] target-zip zip...\n";

char help_head[] =
    PROGRAM " (" PACKAGE ") by Dieter Baron and Thomas Klausner\n\n";

char help[] = "\n\
  -h       display this help message\n\
  -V       display version number\n\
  -D       ignore directory component in file names\n\
  -I       ignore case in file names\n\
  -i       ask before overwriting files\n\
  -S       don't overwrite identical files\n\
  -s       overwrite identical files without asking\n\
\n\
Report bugs to <libizp@nih.at>.\n";

char version_string[] = PROGRAM " (" PACKAGE " " VERSION ")\n\
Copyright (C) 2008 Dieter Baron and Thomas Klausner\n\
" PACKAGE " comes with ABSOLUTELY NO WARRANTY, to the extent permitted by law.\n";

#define OPTIONS "hVDiIsS"

#define CONFIRM_ALL_YES		0x001
#define CONFIRM_ALL_NO		0x002
#define CONFIRM_SAME_YES	0x010
#define CONFIRM_SAME_NO		0x020

int confirm;
int name_flags;

static int confirm_replace(struct zip *, const char *, int,
			   struct zip *, const char *, int);
static int merge_zip(struct zip *za, const char *, const char *, struct zip **);



int
main(int argc, char *argv[])
{
    struct zip *za;
    struct zip **zs;
    int c, err, i;
    char errstr[1024], *tname;

    prg = argv[0];

    confirm = CONFIRM_ALL_YES;
    name_flags = 0;

    while ((c=getopt(argc, argv, OPTIONS)) != -1) {
	switch (c) {
	case 'D':
	    name_flags |= ZIP_FL_NODIR;
	    break;
	case 'i':
	    confirm &= ~CONFIRM_ALL_YES;
	    break;
	case 'I':
	    name_flags |= ZIP_FL_NOCASE;
	    break;
	case 's':
	    confirm &= ~CONFIRM_SAME_NO;
	    confirm |= CONFIRM_SAME_YES;
	    break;
	case 'S':
	    confirm &= ~CONFIRM_SAME_YES;
	    confirm |= CONFIRM_SAME_NO;
	    break;

	case 'h':
	    fputs(help_head, stdout);
	    printf(usage, prg);
	    fputs(help, stdout);
	    exit(0);
	case 'V':
	    fputs(version_string, stdout);
	    exit(0);

	default:
	    fprintf(stderr, usage, prg);
	    exit(2);
	}
    }

    if (argc < optind+2) {
	fprintf(stderr, usage, prg);
	exit(2);
    }

    tname = argv[optind++];

    if ((zs=malloc(sizeof(zs[0])*(argc-optind))) == NULL) {
	fprintf(stderr, "%s: out of memory\n", prg);
	exit(1);
    }

    if ((za=zip_open(tname, ZIP_CREATE, &err)) == NULL) {
	zip_error_to_str(errstr, sizeof(errstr), err, errno);
	fprintf(stderr, "%s: cannot open zip archive `%s': %s\n",
		prg, tname, errstr);
	exit(1);
    }

    for (i=0; i<argc-optind; i++) {
	if (merge_zip(za, tname, argv[optind+i], zs+i) < 0)
	    exit(1);
    }

    if (zip_close(za) < 0) {
	fprintf(stderr, "%s: cannot write zip archive `%s': %s\n",
		prg, tname, zip_strerror(za));
	exit(1);
    }

    for (i=0; i<argc-optind; i++)
	zip_close(zs[i]);

    exit(0);
}



static int
confirm_replace(struct zip *za, const char *tname, int it,
		struct zip *zs, const char *sname, int is)
{
    char line[1024];
    struct zip_stat st, ss;

    if (confirm & CONFIRM_ALL_YES)
	return 1;
    else if (confirm & CONFIRM_ALL_NO)
	return 0;

    if (zip_stat_index(za, it, ZIP_FL_UNCHANGED, &st) < 0) {
	fprintf(stderr, "%s: cannot stat file %d in `%s': %s\n",
		prg, it, tname, zip_strerror(za));
	return -1;
    }
    if (zip_stat_index(zs, is, 0, &ss) < 0) {
	fprintf(stderr, "%s: cannot stat file %d in `%s': %s\n",
		prg, is, sname, zip_strerror(zs));
	return -1;
    }

    if (st.size == ss.size && st.crc == ss.crc) {
	if (confirm & CONFIRM_SAME_YES)
	    return 1;
	else if (confirm & CONFIRM_SAME_NO)
	    return 0;
    }

    printf("replace `%s' (%llu / %08x) in `%s'\n"
	   "   with `%s' (%llu / %08x) from `%s'? ",
	   st.name, st.size, st.crc, tname,
	   ss.name, ss.size, ss.crc, sname);
    fflush(stdout);

    if (fgets(line, sizeof(line), stdin) == NULL) {
	fprintf(stderr, "%s: read error from stdin: %s\n",
		prg, strerror(errno));
	return -1;
    }

    if (tolower((unsigned char)line[0]) == 'y')
	return 1;

    return 0;
}



static int
merge_zip(struct zip *za, const char *tname, const char *sname,
	  struct zip **zsp)
{
    struct zip *zs;
    struct zip_source *source;
    int i, idx, err;
    char errstr[1024];
    const char *fname;
    
    if ((zs=zip_open(sname, 0, &err)) == NULL) {
	zip_error_to_str(errstr, sizeof(errstr), err, errno);
	fprintf(stderr, "%s: cannot open zip archive `%s': %s\n",
		prg, sname, errstr);
	return -1;
    }

    for (i=0; i<zip_get_num_files(zs); i++) {
	fname = zip_get_name(zs, i, 0);

	if ((idx=zip_name_locate(za, fname, name_flags)) != -1) {
	    switch (confirm_replace(za, tname, idx, zs, sname, i)) {
	    case 0:
		break;
		
	    case 1:
		if ((source=zip_source_zip(za, zs, i, 0, 0, 0)) == NULL
		    || zip_replace(za, idx, source) < 0) {
		    zip_source_free(source);
		    fprintf(stderr,
			    "%s: cannot replace `%s' in `%s': %s\n",
			    prg, fname, tname, zip_strerror(za));
		    return -1;
		}
		break;

	    case -1:
		zip_close(zs);
		return -1;
		
	    default:
		fprintf(stderr,	"%s: internal error: "
			"unexpected return code from confirm (%d)\n",
			prg, err);
		zip_close(zs);
		return -1;
	    }
	}
	else {
	    if ((source=zip_source_zip(za, zs, i, 0, 0, 0)) == NULL
		|| zip_add(za, fname, source) < 0) {
		zip_source_free(source);
		fprintf(stderr,
			"%s: cannot add `%s' to `%s': %s\n",
			prg, fname, tname, zip_strerror(za));
		zip_close(zs);
		return -1;
	    }
	}
    }

    *zsp = zs;
    return 0;
}
