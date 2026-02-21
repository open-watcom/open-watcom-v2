/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Default memory management routines.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "aui.h"
#include "guimem.h"
#include "memfuncs.h"
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

static _trmem_hdl  memhdl;

static FILE *memfp = NULL;   /* stream to put output on */
static int  MemOpened = 0;

static void MemPrintLine( void *parm, const char *buff, size_t len )
/*********************************************************************/
{
    /* unused parameters */ (void)parm; (void)len;

    if( memfp != NULL ) {
        fprintf( memfp, "%s\n", buff );
    }
}

#endif  /* TRMEM */

/*
 * The app should replace this module
 */
#if 0
void    WndNoMemory( void )
{
    Say( "No memory for window\n" );
    exit( 1 );
}
#endif

TRMEMAPI( WndAlloc )
void    *WndAlloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _TRMEM_WHO( 1 ), memhdl ) );
#else
    return( malloc( size ) );
#endif
}


TRMEMAPI( WndRealloc )
void    *WndRealloc( void *ptr, size_t size )
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _TRMEM_WHO( 2 ), memhdl ) );
#else
    return( realloc( ptr, size ) );
#endif
}


TRMEMAPI( WndFree )
void    WndFree( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _TRMEM_WHO( 3 ), memhdl );
#else
    free( ptr );
#endif
}

void    GUIMemOpen( void )
{
#ifdef TRMEM
    char * tmpdir;

    if( !MemOpened ) {
        memfp = stderr;
        memhdl = _trmem_open( malloc, free, realloc, strdup,
            NULL, GUIMemPrintLine, _TRMEM_DEF );

        tmpdir = getenv( "TRMEMFILE" );
        if( tmpdir != NULL ) {
            memfp = fopen( tmpdir, "w" );
        }
        MemOpened = 1;
    }
#endif
}

void    GUIMemClose( void )
{
#ifdef TRMEM
    _trmem_prt_list( memhdl );
    _trmem_close( memhdl );
    if( memfp != stderr ) {
        fclose( memfp );
        memfp = NULL;
    }
#endif
}

void GUIMemPrtUsage( void )
/*************************/
{
#ifdef TRMEM
    _trmem_prt_usage( memhdl );
#endif
}

void GUIMemRedirect( FILE *fp )
/*****************************/
{
#ifdef TRMEM
    memfp = fp;
#else
    /* unused parameters */ (void)fp;
#endif
}


/*
 * Alloc functions
 */

TRMEMAPI( GUIMemAlloc )
void *GUIMemAlloc( size_t size )
/******************************/
{
#ifdef TRMEM
    return( _trmem_alloc( size, _TRMEM_WHO( 4 ), memhdl ) );
#else
    return( malloc( size ) );
#endif
}

TRMEMAPI( MemAlloc )
void *MemAlloc( size_t size )
/***************************/
{
#ifdef TRMEM
    return( _trmem_alloc( size, _TRMEM_WHO( 6 ), memhdl ) );
#else
    return( malloc( size ) );
#endif
}

#ifdef GUI_IS_GUI

TRMEMAPI( MemAllocSafe )
void *MemAllocSafe( size_t size )
/*******************************/
{
#ifdef TRMEM
    return( _trmem_alloc( size, _TRMEM_WHO( 6 ), memhdl ) );
#else
    return( malloc( size ) );
#endif
}

#else /* !GUI_IS_GUI */
#endif /* GUI_IS_GUI */

/*
 * Strdup functions
 */

TRMEMAPI( GUIMemStrdup )
char *GUIMemStrdup( const char *str )
/***********************************/
{
#ifdef TRMEM
    return( _trmem_strdup( str, _TRMEM_WHO( 9 ), memhdl ) );
#else
    return( strdup( str ) );
#endif
}

TRMEMAPI( MemStrdup )
char *MemStrdup( const char *str )
/***********************************/
{
#ifdef TRMEM
    return( _trmem_strdup( str, _TRMEM_WHO( 9 ), memhdl ) );
#else
    return( strdup( str ) );
#endif
}

/*
 * Free functions
 */

TRMEMAPI( GUIMemFree )
void GUIMemFree( void *ptr )
/**************************/
{
#ifdef TRMEM
    _trmem_free( ptr, _TRMEM_WHO( 10 ), memhdl );
#else
    free( ptr );
#endif
}

TRMEMAPI( MemFree )
void MemFree( void *ptr )
/***********************/
{
#ifdef TRMEM
    _trmem_free( ptr, _TRMEM_WHO( 12 ), memhdl );
#else
    free( ptr );
#endif
}

/*
 * Realloc functions
 */

TRMEMAPI( GUIMemRealloc )
void *GUIMemRealloc( void *ptr, size_t size )
/*******************************************/
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _TRMEM_WHO( 15 ), memhdl ) );
#else
    return( realloc( ptr, size ) );
#endif
}

TRMEMAPI( MemRealloc )
void *MemRealloc( void *ptr, size_t size )
/****************************************/
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _TRMEM_WHO( 17 ), memhdl ) );
#else
    return( realloc( ptr, size ) );
#endif
}

