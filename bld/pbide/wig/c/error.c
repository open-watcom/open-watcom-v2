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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <io.h>
#include "global.h"
#include "options.h"
#include "error.h"
#include "filelist.h"


#undef pick
#define pick( x, y )    y

static char     *errMsgs[] = {
    #include "error.lst"
};

static FILE     *errFile = stdout;

jmp_buf         ErrorEnv;

BOOL InitError( void ) {
/***********************/

    if( errFile == NULL ) {
        if( Options & OPT_ERROR_FILE_SET ) {
            errFile = WigOpenFile( GetErrorFile(), "wt" );
        } else {
            errFile = stdout;
        }
    }
    return( errFile == NULL );
}

void FiniError( void ) {
/***********************/

    char                *fname;
    unsigned long       size;

    if( Options & OPT_ERROR_FILE_SET ) {
        size = 0;
        if( errFile != NULL ) {
            size = ftell( errFile );
            WigCloseFile( errFile );
        }
        if( size == 0 ) {
            fname = GetErrorFile();
            if( !access( fname, F_OK ) ) {
                remove( fname );
            }
        }
    }
    errFile = stdout;
}

void Error(int errco, ... ) {
/***************************/

    va_list     args;

    va_start( args, errco );
    fprintf( errFile, "Error %d:  ", errco );
    vfprintf( errFile, errMsgs[errco], args );
    va_end( args );
    longjmp( ErrorEnv, RC_ERROR );
}


void Warning(int errco, ... ) {
/*****************************/

    va_list     args;

    va_start( args, errco );
    fprintf( errFile, "Warning %d:  ", errco );
    vfprintf( errFile, errMsgs[errco], args );
    va_end( args );
}
