/*
  $NiH: mkname.c,v 1.1 2005/06/09 18:49:38 dillo Exp $

  mkname.c -- add srcdir to name
  Copyright (C) 2005 Dieter Baron and Thomas Klausner

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



#include <stdio.h>
#include <stdlib.h>

#include "mkname.h"



const char *
mkname(const char *name)
{
    static const char *srcdir;
    static char *fullname;
    static int len = 0;
    static int srcdir_done = 0;

    int nlen;

    if (!srcdir_done) {
	srcdir = getenv("SRCDIR");
	srcdir_done = 1;
    }

    if (!srcdir)
	return name;

    nlen = strlen(srcdir) + strlen(name) + 2;

    if (nlen > len) {
	if (len == 0)
	    fullname = malloc(nlen);
	else
	    fullname = realloc(fullname, nlen);

	if (fullname == NULL) {
	    fprintf(stderr, "malloc failure\n");
	    exit(2);
	}
    }

    sprintf(fullname, "%s/%s", srcdir, name);

    return fullname;
}
