/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Builder utility functions.
*
****************************************************************************/

#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#ifndef __UNIX__
#include <share.h>
#endif
#include "watcom.h"
#include "builder.h"
#include "memutils.h"
#ifdef TRMEM
#include "trmem.h"
#endif

#include "clibext.h"


#ifdef TRMEM
static _trmem_hdl   TRMemHandle;

static void     TRPrintLine( void *parm, const char *buff, size_t len )
{
    /* unused parameters */ (void)parm; (void)len;

    printf( "%s\n", buff );
}
#endif

void MOpen( void )
/****************/
{
#ifdef TRMEM
    TRMemHandle = _trmem_open( malloc, free, NULL, NULL, NULL, TRPrintLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 | _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
#endif
}

void MClose( void )
/*****************/
{
#ifdef TRMEM
    _trmem_prt_list( TRMemHandle );
    _trmem_close( TRMemHandle );
#endif
}

void Fatal( const char *str, ... )
{
    va_list     args;

    va_start( args, str );
    vfprintf( stderr, str, args );
    va_end( args );
    if( LogFile != NULL ) {
        va_start( args, str );
        vfprintf( LogFile, str, args );
        va_end( args );
    }
    CloseLog();
    MClose();
    exit( 1 );
}

void Log( bool quiet, const char *str, ... )
{
    va_list     args;

    if( !quiet ) {
        va_start( args, str );
        vfprintf( stderr, str, args );
        va_end( args );
    }
    if( LogFile != NULL ) {
        va_start( args, str );
        vfprintf( LogFile, str, args );
        va_end( args );
    }
}

void LogFlush( void )
{
    fflush( stderr );
    if( LogFile != NULL ) {
        fflush( LogFile );
    }
}

void OpenLog( const char *name )
{
#ifdef __UNIX__
    LogFile = fopen( name, "w" );
#else
    LogFile = _fsopen( name, "w", SH_DENYWR );
#endif
    if( LogFile == NULL ) {
        Fatal( "Can not open '%s': %s\n", name, strerror( errno ) );
    }
    setvbuf( LogFile, NULL, _IOLBF, BUFSIZ );
}

void CloseLog( void )
{
    LogFlush();
    if( LogFile != NULL ) {
        fclose( LogFile );
        LogFile = NULL;
    }
}

void *MAlloc( size_t size )
{
    void        *p;

#ifdef TRMEM
    p = _trmem_alloc( size, _trmem_guess_who(), TRMemHandle );
#else
    p = malloc( size );
#endif
    if( p == NULL ) {
        Fatal( "Out of memory!\n" );
    }
    return( p );
}

char *MStrdup( const char *s )
{
    void        *p;

#ifdef TRMEM
    p = _trmem_strdup( s, _trmem_guess_who(), TRMemHandle );
#else
    p = strdup( s );
#endif
    if( p == NULL ) {
        Fatal( "Out of memory!\n" );
    }
    return( p );
}

void MFree( void *p )
{
#ifdef TRMEM
    _trmem_free( p, _trmem_guess_who(), TRMemHandle );
#else
    free( p );
#endif
}

const char *SkipBlanks( const char *p )
{
    while( IS_BLANK( *p ) ) {
        ++p;
    }
    return( p );
}
