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
* Description:  Run-time error processor (for optimizing code generator)
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "ftextvar.h"
#include "errcod.h"
#include "rundat.h"
#include "cioconst.h"
#include "xfflags.h"
#include "rtenv.h"

#include <stdarg.h>
#include <string.h>

#define ERR_PREF_SIZE   5
#define ERR_CODE_SIZE   6

void    (*TraceRoutine)(char *) = { NULL };

static  char            ErrorPref[] = { "*ERR*" };

static void noHook( int errcod, char *buffer ) {
//==============================================

    errcod = errcod;
    if( TraceRoutine != NULL ) {
        TraceRoutine( buffer );
    }
}

void    (*ERR_HOOK)( int , char * ) = noHook;

void    FlushStdUnit( void ) {
//======================

    ftnfile     *fcb;

    fcb = Files;
    for(;;) {
        if( fcb == NULL ) return;
        if( fcb->fileptr == FStdOut ) break;
        fcb = fcb->link;
    }
    FlushBuffer( FStdOut );
}

void    WriteErr( int errcode, va_list args ) {
//=============================================

// Write error message.

    char        buffer[ERR_PREF_SIZE+ERR_BUFF_SIZE+1];

    _AccessFIO();
    // Before writing the error message, flush standard output files so that if
    // unit 6 is connected to the standard output device which has been
    // redirected to a file, the file gets flushed before the error message is
    // written.
    FlushStdUnit();
    strcpy( buffer, ErrorPref );
    BldErrCode( errcode, &buffer[ ERR_PREF_SIZE ] );
    __BldErrMsg( errcode, &buffer[ERR_PREF_SIZE + ERR_CODE_SIZE], args );
    if( !__EnterWVIDEO( buffer ) ) {
        StdBuffer();
        StdWriteNL( buffer, strlen( buffer ) );
        ERR_HOOK( errcode, buffer );
        StdFlush();
    }
    _ReleaseFIO();
}

void    ErrHandler( int errcode, va_list args ) {
//===============================================

// Print a run-time error message and halt exection.

    WriteErr( errcode, args );
    __XcptFlags |= XF_FATAL_ERROR;
    if( !(__XcptFlags & XF_IO_INTERRUPTED) ) {
        Suicide();
    }
}

void    RTErr( int errcode, ... ) {
//=================================

// Print a run-time error message and halt execution.

    va_list     args;

    RTSysInit();
    va_start( args, errcode );
    ErrHandler( errcode, args );
    va_end( args );
}
