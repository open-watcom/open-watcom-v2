/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "rtdata.h"
#include "tmpfname.h"

char    *__tmpdir( char * );

#define _TMPFNAME       "__C_TMPx__"
#define _TMPFCHAR       7

_WCRTLINK FILE *tmpfile(void)           /* create a temporary file */
    {
        char    tmpfnext;
        int     fd;
        FILE    *fp;
        char    name[L_tmpnam];
        char    *ptr;

        ptr = __tmpdir( name );
        strcpy( ptr, _TMPFNAME );
        for( tmpfnext = 'a'; ; tmpfnext++ ) {
            ptr[_TMPFCHAR] = tmpfnext;
            fd = open( name, O_RDWR|O_CREAT|O_EXCL|O_TEMP, 0666 );
            if( fd != -1 ) break;
            if( errno != EEXIST ) return( NULL );
            if( tmpfnext == 'z' ) tmpfnext = 'A' - 1;
            if( tmpfnext == 'Z' ) return( NULL );
        }
        fp = fdopen( fd, "wb+" );
        if( fp != NULL ) {
            /* fp->_flag |= _TMPFIL;
               -- Don't turn the TMPFIL bit on... we're going to
                  remove the filename from the file system right away.
                  This is allowed under POSIX. The file will be deleted
                  as soon as it's closed */
            _FP_TMPFCHAR(fp) = tmpfnext;
        } else {
            close( fd );
        }
        remove( name );
        return( fp );
    }
