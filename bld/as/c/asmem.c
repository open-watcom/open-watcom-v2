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


#include "as.h"
#include "wresmem.h"
#include "preproc.h"
#if defined( TRMEM )
    #include "trmem.h"
#endif


#if defined( TRMEM ) && defined( _M_IX86 ) && ( __WATCOMC__ > 1290 )
#define _XSTR(s)    # s
#define TRMEMAPI(x) _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
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

void AsMemInit( void )
/********************/
{
#ifdef TRMEM
    memFile = fopen( "mem.trk", "w" );
    memHandle = _trmem_open( malloc, free, realloc, strdup,
                                NULL, memPrintLine, _TRMEM_ALL );
    if( memHandle == NULL ) {
        exit( EXIT_FAILURE );
    }
#endif
}

void AsMemFini( void )
//********************
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

static void outOfMemory( void )
{
    printf( "Out of memory\n" );
    exit( 1 );
}

TRMEMAPI( AsmAlloc )
pointer AsmAlloc( size_t size )
/*****************************/
{
#ifdef TRMEM
    return( _trmem_alloc( size, _TRMEM_WHO( 1 ), memHandle ) );
#else
    return( malloc( size ) );
#endif
}

TRMEMAPI( AsmStrdup )
char *AsmStrdup( const char *str )
/********************************/
{
#ifdef TRMEM
    return( _trmem_strdup( str, _TRMEM_WHO( 2 ), memHandle ) );
#else
    return( strdup( str ) );
#endif
}

TRMEMAPI( AsmFree )
void AsmFree( pointer ptr )
/*************************/
{
#ifdef TRMEM
    _trmem_free( ptr, _TRMEM_WHO( 3 ), memHandle );
#else
    free( ptr );
#endif
}

TRMEMAPI( AsMemRealloc )
pointer AsMemRealloc( pointer ptr, size_t size )
/**********************************************/
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _TRMEM_WHO( 4 ), memHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}

TRMEMAPI( wres_alloc )
void *wres_alloc( size_t size )
/*****************************/
{
#ifdef TRMEM
    return( _trmem_alloc( size, _TRMEM_WHO( 5 ), memHandle ) );
#else
    return( malloc( size ) );
#endif
}

TRMEMAPI( wres_free )
void wres_free( void *ptr )
/*************************/
{
#ifdef TRMEM
    _trmem_free( ptr, _TRMEM_WHO( 6 ), memHandle );
#else
    free( ptr );
#endif
}

TRMEMAPI( PP_Alloc )
void * PPENTRY PP_Alloc( size_t size )
{
    void        *p;

#ifdef TRMEM
    p = _trmem_alloc( size, _TRMEM_WHO( 7 ), memHandle );
#else
    p = malloc( size );
#endif
    if( p == NULL ) {
        outOfMemory();
    }
    return( p );
}

TRMEMAPI( PP_Free )
void PPENTRY PP_Free( void *p )
{
#ifdef TRMEM
    _trmem_free( p, _TRMEM_WHO( 8 ), memHandle );
#else
    free( p );
#endif
}
