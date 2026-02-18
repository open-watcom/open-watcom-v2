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
* Description:  FORTRAN compiler memory manager
*
****************************************************************************/


#include "ftnstd.h"
#include "errcod.h"
#include "stmtsw.h"
#include "global.h"
#include "fmemmgr.h"
#include "ferror.h"
#include "frl.h"
#include "inout.h"
#include "cle.h"
#include "fmeminit.h"
#include "utility.h"
#include "wresmem.h"
#ifdef TRMEM
    #include "trmem.h"
#endif

#include "clibext.h"
#include "cspawn.h"


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

void    FMemInit( void ) {
//========================

    UnFreeMem = 0;
#if defined( TRMEM )
    memFile = fopen( "mem.trk", "w" );
    memHandle = _trmem_open( malloc, free, _TRMEM_NO_REALLOC, strdup,
                            NULL, memPrintLine, _TRMEM_DEF );
    if( memHandle == NULL ) {
        exit( EXIT_FAILURE );
    }
#else
    SysMemInit();
#endif
}

void    FMemErrors( void ) {
//========================

    ProgSw &= ~PS_ERROR; // we always want to report memory problems
    if( UnFreeMem > 0 ) {
        CompErr( CP_MEMORY_NOT_FREED );
    } else if( UnFreeMem < 0 ) {
        CompErr( CP_FREEING_UNOWNED_MEMORY );
    }
}


void    FMemFini( void )
//======================
{
#if defined( TRMEM )
    if( memHandle != NULL ) {
        _trmem_prt_list_ex( memHandle, 100 );
        _trmem_close( memHandle );
        if( memFile != NULL ) {
            fclose( memFile );
            memFile = NULL;
        }
        memHandle = NULL;
    }
#else
    FMemErrors();
    SysMemFini();
#endif
}

static void no_mem( void )
{
    if( (ProgSw & PS_STMT_TOO_BIG) == 0
      && (StmtSw & SS_SCANNING) && (ITHead != NULL) ) {
        FreeITNodes( ITHead );
        ITHead = NULL;
        Error( MO_LIST_TOO_BIG );
        ProgSw |= PS_STMT_TOO_BIG;
    } else {
        ProgSw |= PS_FATAL_ERROR;
        PurgeAll(); // free up memory so we can process the error
        Error( MO_DYNAMIC_OUT );
        CSuicide();
    }
}

TRMEMAPI( FMemAlloc )
void    *FMemAlloc( size_t size ) {
//=================================

    void        *p;

#if defined( TRMEM )
    p = _trmem_alloc( size, _TRMEM_WHO( 1 ), memHandle );
#else
    p = malloc( size );
#endif
    if( p == NULL ) {
        FrlFini( &ITPool );
#if defined( TRMEM )
        p = _trmem_alloc( size, _TRMEM_WHO( 1 ), memHandle );
#else
        p = malloc( size );
#endif
        if( p == NULL ) {
            no_mem();
        } else {
            UnFreeMem++;
        }
    } else {
        UnFreeMem++;
    }
    return( p );
}

TRMEMAPI( wres_alloc )
void    *wres_alloc( size_t size )
//================================
{
#if defined( TRMEM )
    return( _trmem_alloc( size, _TRMEM_WHO( 2 ), memHandle ) );
#else
    return( malloc( size ) );
#endif
}


TRMEMAPI( FMemStrdup )
char    *FMemStrdup( const char *str )
//====================================
{
    void        *p;

#if defined( TRMEM )
    p = _trmem_strdup( str, _TRMEM_WHO( 3 ), memHandle );
#else
    p = strdup( str );
#endif
    if( p == NULL ) {
        FrlFini( &ITPool );
#if defined( TRMEM )
        p = _trmem_strdup( str, _TRMEM_WHO( 3 ), memHandle );
#else
        p = strdup( str );
#endif
        if( p == NULL ) {
            no_mem();
        } else {
            UnFreeMem++;
        }
    } else {
        UnFreeMem++;
    }
    return( p );
}

TRMEMAPI( FMemFree )
void    FMemFree( void *p ) {
//===========================

#ifdef TRMEM
    _trmem_free( p, _TRMEM_WHO( 4 ), memHandle );
#else
    free( p );
#endif
    UnFreeMem--;
}

TRMEMAPI( wres_free )
void    wres_free( void *p )
//==========================
{
#ifdef TRMEM
    _trmem_free( p, _TRMEM_WHO( 5 ), memHandle );
#else
    free( p );
#endif
}
