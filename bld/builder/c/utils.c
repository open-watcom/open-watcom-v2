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
* Description:  Builder utility functions.
*
****************************************************************************/

#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#ifndef __UNIX__
#include <share.h>
#endif
#include "watcom.h"
#include "builder.h"

void Fatal( const char *str, ... )
{
    va_list     arg;

    va_start( arg, str );
    vfprintf( stderr, str, arg );
    va_end( arg );
    if( LogFile != NULL ) {
        va_start( arg, str );
        vfprintf( LogFile, str, arg );
        va_end( arg );
        fclose( LogFile );
    }
    exit( 1 );
}

void Log( bool quiet, const char *str, ... )
{
    va_list     arg;

    va_start( arg, str );

    if( !quiet )
        vfprintf( stderr, str, arg );
    va_end( arg );
    if( LogFile != NULL ) {
        va_start( arg, str );
        vfprintf( LogFile, str, arg );
        va_end( arg );
    }
}

void LogFlush( void )
{
    fflush( stderr );
    if( LogFile != NULL )
        fflush( LogFile );
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

void *Alloc( unsigned size )
{
    void        *p;

    p = malloc( size );
    if( p == NULL ) {
        Fatal( "Out of memory!\n" );
    }
    return( p );
}

char *SkipBlanks( const char *p )
{
    while( ( *p == ' ' ) || ( *p == '\t' ) ) {
        ++p;
    }
    return( ( char* ) p );
}
