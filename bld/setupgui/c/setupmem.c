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
* Description:  Memory Management functions with included trmem memory tracker
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "guimem.h"
#include "memfuncs.h"
#ifdef TRMEM
    #include "trmem.h"
#endif
#ifdef USE_WRESLIB
    #include "wresmem.h"
#endif


#if defined( TRMEM ) && defined( _M_IX86 ) && ( __WATCOMC__ > 1290 )
#define _XSTR(s)    # s
#define TRMEMAPI(x) _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
#endif

#ifdef TRMEM

static _trmem_hdl  GUIMemHandle;

static FILE *GUIMemFP = NULL;           /* stream to put output on */
static bool GUIMemOpened = false;

static void GUIMemPrintLine( void *parm, const char *buff, size_t len )
/*********************************************************************/
{
    /* unused parameters */ (void)parm; (void)len;

    if( GUIMemFP != NULL ) {
        fprintf( GUIMemFP, "%s\n", buff );
    }
}
#if 0
static void UIMemPrintLine( void *parm, const char *buff, size_t len )
{
    /* unused parameters */ (void)parm; (void)buff; (void)len;
}
#endif

#endif  /* TRMEM */

void GUIMemPrtUsage( void )
/*************************/
{
#ifdef TRMEM
    _trmem_prt_usage( GUIMemHandle );
#endif
}

void GUIMemRedirect( FILE *fp )
/*****************************/
{
#ifdef TRMEM
    GUIMemFP = fp;
#else
    /* unused parameters */ (void)fp;
#endif
}
#if 0
void UIMemRedirect( FILE *fp )
{
#ifdef TRMEM
    GUIMemFP = fp;
#else
    /* unused parameters */ (void)fp;
#endif
}
#endif


void GUIMemOpen( void )
/*********************/
{
#ifdef TRMEM
    char * tmpdir;

    if( !GUIMemOpened ) {
        GUIMemFP = stderr;
        GUIMemHandle = _trmem_open( malloc, free, realloc, strdup,
            NULL, GUIMemPrintLine, _TRMEM_DEF );

        tmpdir = getenv( "TRMEMFILE" );
        if( tmpdir != NULL ) {
            GUIMemFP = fopen( tmpdir, "w" );
        }
        GUIMemOpened = true;
    }
#endif
}

void GUIMemClose( void )
/**********************/
{
#ifdef TRMEM
    _trmem_prt_list( GUIMemHandle );
    _trmem_close( GUIMemHandle );
    if( GUIMemFP != stderr ) {
        fclose( GUIMemFP );
        GUIMemFP = NULL;
    }
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
    return( _trmem_alloc( size, _TRMEM_WHO( 1 ), GUIMemHandle ) );
#else
    return( malloc( size ) );
#endif
}
TRMEMAPI( MemAlloc )
void *MemAlloc( size_t size )
/***************************/
{
#ifdef TRMEM
    return( _trmem_alloc( size, _TRMEM_WHO( 1 ), GUIMemHandle ) );
#else
    return( malloc( size ) );
#endif
}

#if defined( GUI_IS_GUI )
TRMEMAPI( MemAllocSafe )
void *MemAllocSafe( size_t size )
/*******************************/
{
#ifdef TRMEM
    return( _trmem_alloc( size, _TRMEM_WHO( 1 ), GUIMemHandle ) );
#else
    return( malloc( size ) );
#endif
}
#else
#endif
#ifdef USE_WRESLIB
TRMEMAPI( wres_alloc )
void *wres_alloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _TRMEM_WHO( 6 ), GUIMemHandle ) );
#else
    return( malloc( size ) );
#endif
}
#endif

/*
 * Strdup function
 */

TRMEMAPI( GUIMemStrdup )
char *GUIMemStrdup( const char *str )
/***********************************/
{
#ifdef TRMEM
    return( _trmem_strdup( str, _TRMEM_WHO( 1 ), GUIMemHandle ) );
#else
    return( strdup( str ) );
#endif
}

TRMEMAPI( MemStrdup )
char *MemStrdup( const char *str )
/***********************************/
{
#ifdef TRMEM
    return( _trmem_strdup( str, _TRMEM_WHO( 1 ), GUIMemHandle ) );
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
    _trmem_free( ptr, _TRMEM_WHO( 7 ), GUIMemHandle );
#else
    free( ptr );
#endif
}
TRMEMAPI( MemFree )
void MemFree( void *ptr )
/***********************/
{
#ifdef TRMEM
    _trmem_free( ptr, _TRMEM_WHO( 7 ), GUIMemHandle );
#else
    free( ptr );
#endif
}

#ifdef USE_WRESLIB
TRMEMAPI( wres_free )
void wres_free( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _TRMEM_WHO( 12 ), GUIMemHandle );
#else
    free( ptr );
#endif
}
#endif


/*
 * Realloc functions
 */

TRMEMAPI( GUIMemRealloc )
void *GUIMemRealloc( void *ptr, size_t size )
/*******************************************/
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _TRMEM_WHO( 13 ), GUIMemHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}
TRMEMAPI( MemRealloc )
void *MemRealloc( void *ptr, size_t size )
/*******************************************/
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _TRMEM_WHO( 13 ), GUIMemHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}
