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
#include "aui.h"
#include "guimem.h"
#include "memfuncs.h"
#include "memutil.h"
#include "wpmutil.h"
#include "dumpmem.h"
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

static FILE         *TrFile = NULL;    /* stream to put output on */

static void WPMemPrintLine( void *parm, const char *buff, size_t len )
/********************************************************************/
{
    /* unused parameters */ (void)parm; (void)len;

    if( TrFile != NULL ) {
        fprintf( TrFile, "%s\n", buff );
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
    _trmem_prt_usage( TrHdl );
#endif
}
void GUIMemPrtUsage( void )
/*************************/
{
}

void GUIMemOpen( void )
/********************/
{
#ifdef TRMEM
    char * tmpdir;

    if( TrHdl == _TRMEM_HDL_NONE ) {
        TrFile = stderr;
        TrHdl = _trmem_open( malloc, free, realloc, strdup,
            NULL, WPMemPrintLine, _TRMEM_DEF );

        tmpdir = getenv( "TRMEMFILE" );
        if( tmpdir != NULL ) {
            TrFile = fopen( tmpdir, "w" );
        }
    }
#endif
}

void GUIMemClose( void )
/**********************/
{
#ifdef TRMEM
    _trmem_prt_list( TrHdl );
    _trmem_close( TrHdl );
    if( TrFile != stderr ) {
        fclose( TrFile );
        TrFile = NULL;
    }
#endif
}

#ifdef TRMEM
static void profMemCheck( char *msg )
/***********************************/
{
    if( !WalkMem() ) {
        fatal( LIT( Assertion_Failed ), msg );
    }
}
#endif

static void *check_nomem( void *ptr )
{
    if( ptr == NULL ) {
        fatal( LIT( Memfull ) );
    }
    return( ptr );
}

/*
 *  Alloc functions
 */

#ifdef TRMEM
static void *doAlloc( size_t size, _trmem_who who )
#else
static void *doAlloc( size_t size )
#endif
{
    void    *ptr;

    for( ;; ) {
#ifdef TRMEM
        profMemCheck( "ProfTryAlloc" );
        ptr = _trmem_alloc( size, who, TrHdl );
#else
        ptr = malloc( size );
#endif
        if( ptr != NULL )
            break;
        if( DIPMoreMem( size ) == DS_FAIL ) {
            break;
        }
    }
    return( ptr );
}

TRMEMAPI( MemAlloc )
void    *MemAlloc( size_t size )
{
#ifdef TRMEM
    return( doAlloc( size, _TRMEM_WHO( 1 ) ) );
#else
    return( doAlloc( size ) );
#endif
}

TRMEMAPI( MemAllocSafe )
void *MemAllocSafe( size_t size )
/***************************/
{
#ifdef TRMEM
    return( check_nomem( doAlloc( size, _TRMEM_WHO( 1 ) ) ) );
#else
    return( check_nomem( doAlloc( size ) ) );
#endif
}

/*
 *  Strdup functions
 */

#ifdef TRMEM
static void *doStrdup( const char *str, _trmem_who who )
#else
static void *doStrdup( const char *str )
#endif
{
    char    *ptr;
    size_t  size;

    size = strlen( str ) + 1 ;
    for( ;; ) {
#ifdef TRMEM
        profMemCheck( "ProfTryAlloc" );
        ptr = _trmem_strdup( str, who, TrHdl );
#else
        ptr = strdup( str );
#endif
        if( ptr != NULL )
            break;
        if( DIPMoreMem( size ) == DS_FAIL ) {
            break;
        }
    }
    return( ptr );
}

TRMEMAPI( MemStrdup )
char *MemStrdup( const char *str )
/********************************/
{
#ifdef TRMEM
    return( doStrdup( str, _TRMEM_WHO( 7 ) ) );
#else
    return( doStrdup( str ) );
#endif
}

TRMEMAPI( MemStrdupSafe )
char *MemStrdupSafe( const char *str )
/********************************/
{
#ifdef TRMEM
    return( check_nomem( doStrdup( str, _TRMEM_WHO( 7 ) ) ) );
#else
    return( check_nomem( doStrdup( str ) ) );
#endif
}

/*
 *  Free functions
 */

TRMEMAPI( MemFree )
void MemFree( void *ptr )
/************************/
{
#ifdef TRMEM
    profMemCheck( "MemFree" );
    _trmem_free( ptr, _TRMEM_WHO( 8 ), TrHdl );
#else
    free( ptr );
#endif
}

/*
 *  Realloc functions
 */

#ifdef TRMEM
static void *doRealloc( void *ptr, size_t new_size, _trmem_who who )
#else
static void *doRealloc( void *ptr, size_t new_size )
#endif
{
    void    *new;

    for( ;; ) {
#ifdef TRMEM
        profMemCheck( "ProfTryRealloc" );
        new = _trmem_realloc( ptr, new_size, who, TrHdl );
#else
        new = realloc( ptr, new_size );
#endif
        if( new != NULL )
            break;
        if( DIPMoreMem( new_size ) == DS_FAIL ) {
            break;
        }
    }
    return( new );
}

TRMEMAPI( MemRealloc )
void *MemRealloc( void *ptr, size_t new_size )
/*********************************************/
{
#ifdef TRMEM
    return( doRealloc( ptr, new_size, _TRMEM_WHO( 14 ), TrHdl ) );
#else
    return( doRealloc( ptr, new_size ) );
#endif
}

/*
 *  Other functions WP specific
 */
TRMEMAPI( ProfCAlloc )
void *ProfCAlloc( size_t size )
/*****************************/
{
    void    *ptr;

#ifdef TRMEM
    ptr = check_nomem( doAlloc( size, _TRMEM_WHO( 1 ) ) );
#else
    ptr = check_nomem( doAlloc( size ) );
#endif
    return( memset( ptr, 0, size ) );
}

#if 0
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
