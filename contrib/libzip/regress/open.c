/*
  $NiH: open.c,v 1.5 2005/06/09 18:49:38 dillo Exp $

  open.c -- test cases for opening zip archives
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

#include "zip.h"
#include "mkname.h"

int open_fail(const char *, int, const char *, int, int);
int open_success(const char *, int, const char *, int);



int
main(int argc, char *argv[])
{
    int fail;

    fail = 0;

    remove("nosuchfile");
    fail += open_fail("nosuchfile", 0, "non-existing", ZIP_ER_OPEN, ENOENT);
    fail += open_fail("Makefile.am", 0, "non-zip", ZIP_ER_NOZIP, 0);
    fail += open_fail("test.zip", ZIP_EXCL, "existing-excl", ZIP_ER_EXISTS, 0);
    /* ZIP_ER_OPEN */
    /* ZIP_ER_READ */
    /* ZIP_ER_SEEK */
    /* ZIP_ER_INCONS */

    fail += open_success("test.zip", 0, "existing", 3);
    fail += open_success("nosuchfile", ZIP_CREATE, "new", 0);

    exit(fail ? 1 : 0);
}



int
open_fail(const char *fname, int flags, const char *desc, int zerr, int serr)
{
    struct zip *z;
    int ze;

    errno = 0;

    if ((z=zip_open(mkname(fname), flags, &ze)) != NULL) {
	printf("fail: opening %s succeeded\n", desc);
	zip_close(z);
	return 1;
    }
    else if (ze != zerr || errno != serr) {
	printf("fail: opening %s returned wrong error %d/%d, expected %d/%d\n",
		desc, ze, errno, zerr, serr);
	return 1;
    }

    return 0;
}



int
open_success(const char *fname, int flags, const char *desc, int nent)
{
    struct zip *z;
    int ze, num;

    if ((z=zip_open(mkname(fname), flags, &ze)) == NULL) {
	printf("fail: opening %s failed (%d)\n", desc, ze);
	return 1;
    }

    num = zip_get_num_files(z);
    zip_close(z);
    
    if (num != nent) {
	printf("fail: opening %s got wrong number of files %d, expected %d\n",
		desc, num, nent);
	return 1;
    }

    return 0;
}
