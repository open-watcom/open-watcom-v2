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
#include <stdarg.h>
#include <string.h>
#include "global.h"
#include "mem.h"
#include "pbide.h"
#include "error.h"

#undef pick
#define pick( x, y )    y

static char     *errMsgs[] = {
    #include "error.lst"
};

jmp_buf         ErrorEnv;

#define ERR_BUF_INCRMT          1024

static  WatIDEErrInfo   *errBuf;
static  unsigned long   errLen;
static  unsigned long   errBufSize;

WatIDEErrInfo *GetErrInfo( void ) {
/**********************************/
    return( errBuf );
}

void FreeErrInfo( WatIDEErrInfo *info ) {
/****************************************/
    if( info != NULL ) MemFree( info );
}

BOOL InitError( void ) {
/***********************/
    errBufSize = ERR_BUF_INCRMT;
    errBuf = MemMalloc( sizeof( WatIDEErrInfo ) + errBufSize );
    errBuf->errcnt = 0;
    errBuf->warncnt = 0;
    errLen = 0;
    return( FALSE );
}

void FiniError( void ) {
/***********************/
    if( errBuf->errcnt == 0 && errBuf->warncnt == 0 ) {
        MemFree( errBuf );
        errBuf = NULL;
    }
    errLen = 0;
    errBufSize = 0;
}

void fmtErr( char *prefix, int errco, va_list args ) {
/****************************************************/
    char                fmtbuf[256];
    int                 len;

    sprintf( fmtbuf, "%s %d:  %n", prefix, errco, &len );
    vsprintf( fmtbuf + len, errMsgs[errco], args );
    len = strlen( fmtbuf );
    if( errLen + len > errBufSize ) {
        errBufSize += ERR_BUF_INCRMT;
        errBuf = MemRealloc( errBuf, sizeof( WatIDEErrInfo ) + errBufSize );
    }
    strcpy( errBuf->errors + errLen, fmtbuf );
    errLen += len;
}

void Error(int errco, ... ) {
/****************************/

    va_list     args;

    errBuf->errcnt ++;
    va_start( args, errco );
    fmtErr( "Error: ", errco, args );
    va_end( args );
    longjmp( ErrorEnv, RC_ERROR );
}

void Warning(int errco, ... ) {
/******************************/

    va_list     args;

    errBuf->warncnt ++;
    va_start( args, errco );
    fmtErr( "Warning: ", errco, args );
    va_end( args );
}
