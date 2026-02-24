/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
#include "yacc.h"
#include "alloc.h"
#ifdef TRMEM
    #include "trmem.h"
#endif


#if defined( TRMEM ) && defined( _M_IX86 ) && ( __WATCOMC__ > 1290 )
#define _XSTR(s)    # s
#define TRMEMAPI(x) _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
#endif

#ifdef TRMEM

static _trmem_hdl   TrHdl = _TRMEM_HDL_NONE;
static FILE         *TrFile;       /* file handle we'll write() to */

static void memPrintLine( void *file, const char *buf, size_t len )
{
    /* unused parameters */ (void)file; (void)len;

    fprintf( stderr, "***%s\n", buf );
    if( TrFile != NULL ) {
        fprintf( TrFile, "%s\n", buf );
    }
}

#endif  /* TRMEM */

void MemInit( void )
/******************/
{
#ifdef TRMEM
    TrFile = fopen( "mem.trk", "w" );
    TrHdl = _trmem_open( malloc, free, realloc, strdup,
                                NULL, memPrintLine, _TRMEM_DEF );
    if( TrHdl == _TRMEM_HDL_NONE ) {
        exit( EXIT_FAILURE );
    }
#endif
}

void MemFini( void )
/******************/
{
#ifdef TRMEM
    if( TrHdl != _TRMEM_HDL_NONE ) {
        _trmem_prt_list_ex( TrHdl, 100 );
        _trmem_close( TrHdl );
        if( TrFile != NULL ) {
            fclose( TrFile );
            TrFile = NULL;
        }
        TrHdl = _TRMEM_HDL_NONE;
    }
#endif
}

TRMEMAPI( MemAlloc )
void *MemAlloc( size_t size )
/****************************/
{
    void        *ptr;

#ifdef TRMEM
    ptr = _trmem_alloc( size, _TRMEM_WHO( 1 ), TrHdl );
#else
    ptr = malloc( size );
#endif
    if( ptr == NULL ) {
        msg( "Out of memory\n" );
    }
    return( ptr );
}

TRMEMAPI( MemCAlloc )
void *MemCAlloc( size_t n, size_t size )
/***************************************/
{
    void        *ptr;

    size *= n;
#ifdef TRMEM
    ptr = _trmem_alloc( size, _TRMEM_WHO( 2 ), TrHdl );
#else
    ptr = malloc( size );
#endif
    if( ptr == NULL ) {
        msg( "Out of memory\n" );
    }
    return( memset( ptr, 0, size ) );
}

TRMEMAPI( MemRealloc )
void *MemRealloc( void *old_ptr, size_t newsize )
/************************************************/
{
    void    *ptr;

#ifdef TRMEM
    ptr = _trmem_realloc( old_ptr, newsize, _TRMEM_WHO( 3 ), TrHdl );
#else
    ptr = realloc( old_ptr, newsize );
#endif
    if( ptr == NULL ) {
        msg( "Out of memory\n" );
    }
    return( ptr );
}

TRMEMAPI( MemFree )
void MemFree( void *ptr )
/************************/
{
    if( ptr != NULL ) {
#ifdef TRMEM
        _trmem_free( ptr, _TRMEM_WHO( 4 ), TrHdl );
#else
        free( ptr );
#endif
    }
}

TRMEMAPI( MemStrdup )
char *MemStrdup( const char *str )
/*********************************/
{
    char        *ptr;

    if( str == NULL )
        return( NULL );
#ifdef TRMEM
    ptr = _trmem_strdup( str, _TRMEM_WHO( 5 ), TrHdl );
#else
    ptr = strdup( str );
#endif
    if( ptr == NULL ) {
        msg( "Out of memory\n" );
    }
    return( ptr );
}
