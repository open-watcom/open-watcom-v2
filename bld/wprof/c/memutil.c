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
* Description:  Memory allocation routines for profiler.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dip.h"
#include "msg.h"
#include "guimem.h"
#if defined( GUI_IS_GUI )
    #include "wpimem.h"
#else
    #include "stdui.h"
    #include "helpmem.h"
#endif
#include "memutil.h"
#include "wpmutil.h"
#include "dumpmem.h"
#include "wresmem.h"
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

static _trmem_hdl   WPMemHandle;

static FILE         *WPMemFP = NULL;    /* stream to put output on */
static int          WPMemOpened = 0;

static void WPMemPrintLine( void *parm, const char *buff, size_t len )
/********************************************************************/
{
    /* unused parameters */ (void)parm; (void)len;

    if( WPMemFP != NULL ) {
        fprintf( WPMemFP, "%s\n", buff );
    }
}
#if 0
static void GUIMemPrintLine( void *parm, const char *buff, size_t len )
{
    /* unused parameters */ (void)parm; (void)buff; (void)len;
}
#endif

#endif  /* TRMEM */

void GUIMemRedirect( FILE *fp )
/*****************************/
{
    /* unused parameters */ (void)fp;
}


void WPMemPrtUsage( void )
/************************/
{
#ifdef TRMEM
    _trmem_prt_usage( WPMemHandle );
#endif
}
void GUIMemPrtUsage( void )
/*************************/
{
}

void WPMemOpen( void ) {}
void GUIMemOpen( void )
/********************/
{
#ifdef TRMEM
    char * tmpdir;

    if( !WPMemOpened ) {
        WPMemFP = stderr;
        WPMemHandle = _trmem_open( malloc, free, realloc, NULL,
            NULL, WPMemPrintLine, _TRMEM_DEF );

        tmpdir = getenv( "TRMEMFILE" );
        if( tmpdir != NULL ) {
            WPMemFP = fopen( tmpdir, "w" );
        }
        WPMemOpened = 1;
    }
#endif
}
#if !defined( GUI_IS_GUI )
void UIAPI UIMemOpen( void ) {}
#endif


void WPMemClose( void ) {}
void GUIMemClose( void )
/**********************/
{
#ifdef TRMEM
    _trmem_prt_list( WPMemHandle );
    _trmem_close( WPMemHandle );
    if( WPMemFP != stderr ) {
        fclose( WPMemFP );
        WPMemFP = NULL;
    }
#endif
}
#if !defined( GUI_IS_GUI )
void UIAPI UIMemClose( void ) {}
#endif

#ifdef TRMEM
static void profMemCheck( char *msg )
/***********************************/
{
    if( !WalkMem() ) {
        fatal( LIT( Assertion_Failed ), msg );
    }
}
#endif


/*
 *  Alloc functions
 */

TRMEMAPI( ProfAlloc )
void *ProfAlloc( size_t size )
/****************************/
{
    void    *mem;

    for( ;; ) {
#ifdef TRMEM
        profMemCheck( "ProfTryAlloc" );
        mem = _trmem_alloc( size, _TRMEM_WHO( 1 ), WPMemHandle );
#else
        mem = malloc( size );
#endif
        if( mem != NULL )
            break;
        if( DIPMoreMem( size ) == DS_FAIL ) {
            break;
        }
    }

    if( mem == NULL ) {
        fatal( LIT( Memfull ) );
    }
    return( mem );
}
TRMEMAPI( GUIMemAlloc )
void *GUIMemAlloc( size_t size )
/******************************/
{
    void    *mem;

    for( ;; ) {
#ifdef TRMEM
        profMemCheck( "ProfTryAlloc" );
        mem = _trmem_alloc( size, _TRMEM_WHO( 2 ), WPMemHandle );
#else
        mem = malloc( size );
#endif
        if( mem != NULL )
            break;
        if( DIPMoreMem( size ) == DS_FAIL ) {
            break;
        }
    }

    if( mem == NULL ) {
        fatal( LIT( Memfull ) );
    }
    return( mem );
}
#if defined( GUI_IS_GUI )
TRMEMAPI( _wpi_malloc )
void * _wpi_malloc( size_t size )
{
    void    *mem;

    for( ;; ) {
#ifdef TRMEM
        profMemCheck( "ProfTryAlloc" );
        mem = _trmem_alloc( size, _TRMEM_WHO( 3 ), WPMemHandle );
#else
        mem = malloc( size );
#endif
        if( mem != NULL )
            break;
        if( DIPMoreMem( size ) == DS_FAIL ) {
            break;
        }
    }

    if( mem == NULL ) {
        fatal( LIT( Memfull ) );
    }
    return( mem );
}
#else
TRMEMAPI( uimalloc )
void * UIAPI uimalloc( size_t size )
{
    void    *mem;

    for( ;; ) {
#ifdef TRMEM
        profMemCheck( "ProfTryAlloc" );
        mem = _trmem_alloc( size, _TRMEM_WHO( 4 ), WPMemHandle );
#else
        mem = malloc( size );
#endif
        if( mem != NULL )
            break;
        if( DIPMoreMem( size ) == DS_FAIL ) {
            break;
        }
    }

    if( mem == NULL ) {
        fatal( LIT( Memfull ) );
    }
    return( mem );
}
TRMEMAPI( HelpMemAlloc )
void *HelpMemAlloc( size_t size )
{
    void    *mem;

    for( ;; ) {
#ifdef TRMEM
        profMemCheck( "ProfTryAlloc" );
        mem = _trmem_alloc( size, _TRMEM_WHO( 5 ), WPMemHandle );
#else
        mem = malloc( size );
#endif
        if( mem != NULL )
            break;
        if( DIPMoreMem( size ) == DS_FAIL ) {
            break;
        }
    }

    if( mem == NULL ) {
        fatal( LIT( Memfull ) );
    }
    return( mem );
}
#endif
TRMEMAPI( wres_alloc )
void *wres_alloc( size_t size )
{
    void    *mem;

    for( ;; ) {
#ifdef TRMEM
        profMemCheck( "ProfTryAlloc" );
        mem = _trmem_alloc( size, _TRMEM_WHO( 6 ), WPMemHandle );
#else
        mem = malloc( size );
#endif
        if( mem != NULL )
            break;
        if( DIPMoreMem( size ) == DS_FAIL ) {
            break;
        }
    }

    if( mem == NULL ) {
        fatal( LIT( Memfull ) );
    }
    return( mem );
}

/*
 *  Free functions
 */

TRMEMAPI( ProfFree )
void ProfFree( void *ptr )
/************************/
{
#ifdef TRMEM
    profMemCheck( "ProfFree" );
    _trmem_free( ptr, _TRMEM_WHO( 7 ), WPMemHandle );
#else
    free( ptr );
#endif
}
TRMEMAPI( GUIMemFree )
void GUIMemFree( void *ptr )
/**************************/
{
#ifdef TRMEM
    profMemCheck( "ProfFree" );
    _trmem_free( ptr, _TRMEM_WHO( 8 ), WPMemHandle );
#else
    free( ptr );
#endif
}
#if defined( GUI_IS_GUI )
TRMEMAPI( _wpi_free )
void _wpi_free( void *ptr )
{
#ifdef TRMEM
    profMemCheck( "ProfFree" );
    _trmem_free( ptr, _TRMEM_WHO( 9 ), WPMemHandle );
#else
    free( ptr );
#endif
}
#else
TRMEMAPI( uifree )
void UIAPI uifree( void *ptr )
{
#ifdef TRMEM
    profMemCheck( "ProfFree" );
    _trmem_free( ptr, _TRMEM_WHO( 10 ), WPMemHandle );
#else
    free( ptr );
#endif
}
TRMEMAPI( HelpMemFree )
void HelpMemFree( void *ptr )
{
#ifdef TRMEM
    profMemCheck( "ProfFree" );
    _trmem_free( ptr, _TRMEM_WHO( 11 ), WPMemHandle );
#else
    free( ptr );
#endif
}
#endif
TRMEMAPI( wres_free )
void wres_free( void *ptr )
{
#ifdef TRMEM
    profMemCheck( "ProfFree" );
    _trmem_free( ptr, _TRMEM_WHO( 12 ), WPMemHandle );
#else
    free( ptr );
#endif
}


/*
 *  Realloc functions
 */

TRMEMAPI( ProfRealloc )
void *ProfRealloc( void *ptr, size_t new_size )
/*********************************************/
{
    void    *new;

    for( ;; ) {
#ifdef TRMEM
        profMemCheck( "ProfTryRealloc" );
        new = _trmem_realloc( ptr, new_size, _TRMEM_WHO( 13 ), WPMemHandle );
#else
        new = realloc( ptr, new_size );
#endif
        if( new != NULL )
            break;
        if( DIPMoreMem( new_size ) == DS_FAIL ) {
            break;
        }
    }
    if( new == NULL ) {
        fatal( LIT( Memfull_Realloc  ));
    }
    return( new );
}
TRMEMAPI( GUIMemRealloc )
void *GUIMemRealloc( void *ptr, size_t new_size )
/***********************************************/
{
    void    *new;

    for( ;; ) {
#ifdef TRMEM
        profMemCheck( "ProfTryRealloc" );
        new = _trmem_realloc( ptr, new_size, _TRMEM_WHO( 14 ), WPMemHandle );
#else
        new = realloc( ptr, new_size );
#endif
        if( new != NULL )
            break;
        if( DIPMoreMem( new_size ) == DS_FAIL ) {
            break;
        }
    }
    if( new == NULL ) {
        fatal( LIT( Memfull_Realloc  ));
    }
    return( new );
}
#if defined( GUI_IS_GUI )
TRMEMAPI( _wpi_realloc )
void * _wpi_realloc( void *ptr, size_t new_size )
{
    void    *new;

    for( ;; ) {
#ifdef TRMEM
        profMemCheck( "ProfTryRealloc" );
        new = _trmem_realloc( ptr, new_size, _TRMEM_WHO( 15 ), WPMemHandle );
#else
        new = realloc( ptr, new_size );
#endif
        if( new != NULL )
            break;
        if( DIPMoreMem( new_size ) == DS_FAIL ) {
            break;
        }
    }
    if( new == NULL ) {
        fatal( LIT( Memfull_Realloc  ));
    }
    return( new );
}
#else
TRMEMAPI( uirealloc )
void * UIAPI uirealloc( void *ptr, size_t new_size )
{
    void    *new;

    for( ;; ) {
#ifdef TRMEM
        profMemCheck( "ProfTryRealloc" );
        new = _trmem_realloc( ptr, new_size, _TRMEM_WHO( 16 ), WPMemHandle );
#else
        new = realloc( ptr, new_size );
#endif
        if( new != NULL )
            break;
        if( DIPMoreMem( new_size ) == DS_FAIL ) {
            break;
        }
    }
    if( new == NULL ) {
        fatal( LIT( Memfull_Realloc  ));
    }
    return( new );
}
TRMEMAPI( HelpMemRealloc )
void *HelpMemRealloc( void *ptr, size_t new_size )
{
    void    *new;

    for( ;; ) {
#ifdef TRMEM
        profMemCheck( "ProfTryRealloc" );
        new = _trmem_realloc( ptr, new_size, _TRMEM_WHO( 17 ), WPMemHandle );
#else
        new = realloc( ptr, new_size );
#endif
        if( new != NULL )
            break;
        if( DIPMoreMem( new_size ) == DS_FAIL ) {
            break;
        }
    }
    if( new == NULL ) {
        fatal( LIT( Memfull_Realloc  ));
    }
    return( new );
}
#endif


/*
 *  Other functions WP specific
 */
TRMEMAPI( ProfCAlloc )
void *ProfCAlloc( size_t size )
/*****************************/
{
    void    *mem;

    for( ;; ) {
#ifdef TRMEM
        profMemCheck( "ProfTryAlloc" );
        mem = _trmem_alloc( size, _TRMEM_WHO( 18 ), WPMemHandle );
#else
        mem = malloc( size );
#endif
        if( mem != NULL )
            break;
        if( DIPMoreMem( size ) == DS_FAIL ) {
            break;
        }
    }

    if( mem == NULL ) {
        fatal( LIT( Memfull ) );
    }
    memset( mem, 0, size );
    return( mem );
}

#if 0
void *WndAlloc( unsigned size )
/*****************************/
{
    return( ProfAlloc( size ) );
}

void *WndRealloc( void *chunk, unsigned size )
/********************************************/
{
    return( ProfRealloc( chunk, size ) );
}

void WndFree( void *chunk )
/*************************/
{
    ProfFree( chunk );
}

void WndNoMemory( void )
/**********************/
{
    fatal( LIT( Memfull  ));
}

void WndMemInit( void )
/*********************/
{
}

void WndMemFini( void )
/*********************/
{
}
#endif
