/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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


//  PPMEM.C - memory allocation routines for pre-processor
//  Any project that includes this pre-processor will probably substitute
//  their own support for these routines.
//
#include "_preproc.h"
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

#endif  /* TRMEM */

void MemInit( void )
/******************/
{
#ifdef TRMEM
    memFile = fopen( "mem.trk", "w" );
    memHandle = _trmem_open( malloc, free, realloc, NULL, NULL, memPrintLine, _TRMEM_ALL & ~_TRMEM_REALLOC_NULL );
    if( memHandle == NULL ) {
        exit( EXIT_FAILURE );
    }
#endif
}

void MemFini( void )
/******************/
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

TRMEMAPI( PP_Malloc )
void * PPENTRY PP_Malloc( size_t size )
{
    void        *p;

#ifdef TRMEM
    p = _trmem_alloc( size, _trmem_guess_who(), memHandle );
#else
    p = malloc( size );
#endif
    if( p == NULL ) {
        outOfMemory();
    }
    return( p );
}

TRMEMAPI( PP_Realloc )
void *PP_Realloc( void *old, size_t size )
{
    void        *p;

#ifdef TRMEM
    p = _trmem_realloc( old, size, _trmem_guess_who(), memHandle );
#else
    p = realloc( old, size );
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
    _trmem_free( p, _trmem_guess_who(), memHandle );
#else
    free( p );
#endif
}
