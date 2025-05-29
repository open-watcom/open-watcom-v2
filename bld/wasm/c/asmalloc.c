/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Memory manipulation routines.
*
****************************************************************************/


/*
    if TRMEM is defined, trmem functions are used which will help tracking
    memory usage.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "asmalloc.h"
#include "fatal.h"
#include "wresmem.h"

#ifdef TRMEM
#include "trmem.h"

static _trmem_hdl   memHandle;
static FILE         *memFile;       /* file handle we'll write() to */

static void memPrintLine( void *file, const char *buf, size_t len )
{
    /* unused parameters */ (void)file; (void)len;

    fprintf( stderr, "***%s\n", buf );
    if( memFile != NULL ) {
        fprintf( memFile, "%s\n", buf );
    }
}
#endif

void MemInit( void )
{
#ifdef TRMEM
    memFile = fopen( "mem.trk", "w" );
    memHandle = _trmem_open( malloc, free, realloc, NULL, NULL, memPrintLine, _TRMEM_ALL );
    if( memHandle == NULL ) {
        exit( EXIT_FAILURE );
    }
#endif
}

void MemFini( void )
{
#ifdef TRMEM
    if( memHandle != NULL ) {
        _trmem_prt_list( memHandle );
        _trmem_close( memHandle );
        if( memFile != NULL ) {
            fclose( memFile );
            memFile = NULL;
        }
        memHandle = NULL;
    }
#endif
}

void *AsmAlloc( size_t size )
{
    void        *ptr;

#ifdef TRMEM
    ptr = _trmem_alloc( size, _trmem_guess_who(), memHandle );
#else
    ptr = malloc( size );
#endif
    if( ptr == NULL ) {
        Fatal( MSG_OUT_OF_MEMORY );
    }
    return( ptr );
}

char *AsmStrDup( const char *str )
{
    size_t      size;
    char        *ptr;

    if( str == NULL )
        return( NULL );
    size = strlen( str ) + 1;
#ifdef TRMEM
    ptr = _trmem_alloc( size, _trmem_guess_who(), memHandle );
#else
    ptr = malloc( size );
#endif
    if( ptr == NULL ) {
        Fatal( MSG_OUT_OF_MEMORY );
    }
    return( strcpy( ptr, str ) );
}

void AsmFree( void *ptr )
{
    if( ptr != NULL ) {
#ifdef TRMEM
        _trmem_free( ptr, _trmem_guess_who(), memHandle );
#else
        free( ptr );
#endif
    }
}

void *wres_alloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), memHandle ) );
#else
    return( malloc( size ) );
#endif
}

void wres_free( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), memHandle );
#else
    free( ptr );
#endif
}
