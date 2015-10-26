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
* Description:  Platform independent tmpfile() implementation.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <fcntl.h>
#include <direct.h>
#include <string.h>
#include <process.h>
#include <unistd.h>
#include <sys/stat.h>
#include "rtdata.h"
#include "rterrno.h"
#include "rtinit.h"
#include "tmpfname.h"
#include "openmode.h"
#include "thread.h"

#define OPEN_MODE   (O_RDWR | O_CREAT | O_BINARY)
#define PMODE       (S_IREAD | S_IWRITE)

/* Netware doesn't define these */
/* Symbolic constants for the access() function */

#if !defined( F_OK )
#define R_OK    4       /*  Test for read permission    */
#define W_OK    2       /*  Test for write permission   */
#define X_OK    1       /*  Test for execute permission */
#define F_OK    0       /*  Test for existence of file  */
#endif

extern void     __MkTmpFile( char *buf, int num );
extern void     __RmTmpFile( FILE *fp );
extern void     (*__RmTmpFileFn)( FILE *fp );

char __tmpfnext = _TMP_INIT_CHAR;

_WCRTLINK FILE *tmpfile( void )         /* create a temporary file */
{
    int         hdl;
    int         old_errno;
    int         our_errno;
    char        suffix1;
    char        suffix2;
    FILE        *fp;
    char        name1[PATH_MAX + _TMPFNAME_LENGTH + 1];
    char        name2[PATH_MAX + _TMPFNAME_LENGTH + 1];

    old_errno = _RWD_errno;
    suffix1 = 0;
    for( ;; ) {
        // Part I
        for( ;; ) {
            __MkTmpFile( name1, suffix1 );
            // if a file by this name does not exist
            if( access( name1, F_OK ) != 0 ) {

                // then let's try to create it
                hdl = sopen( name1, OPEN_MODE, OPENMODE_DENY_COMPAT, PMODE );

                // if we created it then continue with part II
                if( hdl != -1 )
                    break;
                _RWD_errno = EAGAIN;
            }
            suffix1++;
            // give up after _TMP_INIT_CHAR tries  JBS 99/10/26
            if( suffix1 >= _TMP_INIT_CHAR ) return NULL;
        }
        close( hdl );

        // Part II
        /* we now have a empty file. Let's try to rename it
           rename should be an atomic operation in the operating system
           so if it succeeds we can be sure no one else has this file.
           Consider the following sequence:

           task1: access x.y => file does not exist
           task2: access x.y => file does not exist
           task1: fopen  x.y => succeeds
           task2: fopen  x.y => succeeds (now have both tasks with x.y open)
           task1: rename x.y to y.y => succeeds, can use this file
           task2: rename x.y to y.y => fails (because x.y no longer exists)
           task2: start over again to get a new file name
           task2: succeeds second time around since no more race condition
                  with task1.
         */
        suffix2 = _RWD_tmpfnext;    // only one of these per process
        for( ;; ) {
            if( suffix2 == suffix1 ) {
                suffix2++;
            }
            __MkTmpFile( name2, suffix2 );

            if( rename( name1, name2 ) == 0 ) { // if rename worked

                // The file is now ours. Let's try to open it.
                fp = fopen( name2, "wb+" );
                if( fp != NULL ) {
                    fp->_flag |= _TMPFIL;
                    _FP_TMPFCHAR(fp) = suffix2;
                    _RWD_errno = old_errno;
                    return( fp );
                }
                // We couldn't open it, probably because we have run out of handles.
                // Remove the renamed file.
                our_errno = _RWD_errno;
                remove( name2 );
                _RWD_errno = our_errno;
                return( NULL );
            }
            // The rename didn't work or we couldn't open the renamed file.
            // One of two possibilities:
            // (1) The "to" name already exists.
            // (2) Another process renamed it away from us.

            // Check for case (2).
            // Quit if "from" file is gone and start over.
            if( access( name1, F_OK ) != 0 ) break;

            // Must be case (1). Try another "to" name.
            ++suffix2;
            if( suffix2 == 0 ) {
                suffix2 = _TMP_INIT_CHAR;
            }
            _RWD_tmpfnext = suffix2;    // update for all processes
        }
    }
}

/* tmpfil() pulls in a lot of overhead that many programs do not need. But */
/* since temp files are removed on program shutdown, the code to remove    */
/* them would always get linked in even if the program never heard of temp */
/* files. Since we know that temporary files can _only_ be created through */
/* tmpfile(), we can have a dummy __RmTmpFile() by default and use the     */
/* real thing only if tmpfil() was called.                                 */
void __Init_Tmpfl( void )
{
    // Just assign the function address
    __RmTmpFileFn = __RmTmpFile;
}

AXI( __Init_Tmpfl, INIT_PRIORITY_RUNTIME )
