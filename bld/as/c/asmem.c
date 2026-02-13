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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifdef _STANDALONE_

#include "as.h"
#include "wresmem.h"
#if defined( TRMEM )
    #include "trmem.h"
#endif


#if defined( TRMEM )

static _trmem_hdl   memHandle;
static FILE         *memFile;       /* file handle we'll write() to */

static void memPrintLine( void *file, const char *buf, size_t len )
/*****************************************************************/
{
    /* unused parameters */ (void)file; (void)len;

    fprintf( stderr, "***%s\n", buf );
    if( memFile != NULL ) {
        fprintf( memFile, "%s\n", buf );
    }
}

#endif /* TRMEM */

void MemInit( void )
/******************/
{
#ifdef TRMEM
    memFile = fopen( "mem.trk", "w" );
    memHandle = _trmem_open( malloc, free, realloc, NULL, NULL, memPrintLine, _TRMEM_ALL );
    if( memHandle == NULL ) {
        exit( EXIT_FAILURE );
    }
#endif
}

#if defined( TRMEM ) && defined( _M_IX86 )
#pragma aux (WFRM) wres_alloc
#endif
void *wres_alloc( size_t size )
/*****************************/
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), memHandle ) );
#else
    return( malloc( size ) );
#endif
}

#if defined( TRMEM ) && defined( _M_IX86 )
#pragma aux (WFRM) MemRealloc
#endif
pointer MemRealloc( pointer ptr, size_t size )
/********************************************/
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _trmem_guess_who(), memHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}

#if defined( TRMEM ) && defined( _M_IX86 )
#pragma aux (WFRM) wres_free
#endif
void wres_free( void *ptr )
/*************************/
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), memHandle );
#else
    free( ptr );
#endif
}

void MemFini( void )
//******************
{
#ifdef TRMEM
    if( memHandle != NULL ) {
        _trmem_prt_list_ex( memHandle, 100 );
        _trmem_close( memHandle );
        if( memFile != NULL ) {
            fclose( memFile );
            memFile = NULL;
        }
        memHandle = NULL;
    }
#endif
}

#if defined( TRMEM ) && defined( _M_IX86 )
#pragma aux (WFRM) MemAlloc
#endif
pointer MemAlloc( size_t size )
/*****************************/
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), memHandle ) );
#else
    return( malloc( size ) );
#endif
}

#if defined( TRMEM ) && defined( _M_IX86 )
#pragma aux (WFRM) MemFree
#endif
void MemFree( pointer ptr )
/*************************/
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), memHandle );
#else
    free( ptr );
#endif
}

#else /* !_STANDALONE_ */

#include "as.h"
#include "asalloc.h"


pointer MemAlloc( size_t size )
/*****************************/
{
    return( AsmAlloc( size ) );
}

void MemFree( pointer ptr )
/*************************/
{
    AsmFree( ptr );
}

#endif /* _STANDALONE_ */
