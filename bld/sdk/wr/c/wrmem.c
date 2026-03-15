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
* Description:  WR memory manipulation routines with optional tracking.
*
****************************************************************************/


#include "wrglbl.h"
#include "memfuncs.h"
#include "wrmemi.h"


#if defined( _M_IX86 ) && defined( __NT__ )
#define _XSTR(s)    # s
#define TRMEMAPI(x)     _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
#endif

static _trmem_hdl   TrHdl = _TRMEM_HDL_NONE;
static FILE         *TrFile = NULL;
static wr_nomem_cb  *nomem_cb = NULL;

static void TRPrintLine( void *parm, const char *buff, size_t len )
/*****************************************************************/
{
    /* unused parameters */ (void)parm; (void)len;

    if( TrFile != NULL ) {
        fprintf( TrFile, "%s\n", buff );
    }
}

void WRMemOpen( bool trace, wr_nomem_cb *nomemcb )
{
    char    *tmpdir;

    nomem_cb = nomemcb;
    if( trace ) {
        TrHdl = _trmem_open( malloc, free, realloc, _TRMEM_NO_STRDUP,
                                   NULL, TRPrintLine, _TRMEM_DEF );
        tmpdir = getenv( "TRMEMFILE" );
        if( tmpdir != NULL ) {
            TrFile = fopen( tmpdir, "w" );
        }
    } else {
        TrHdl = _TRMEM_HDL_NONE;
    }
}

void WRMemClose( void )
{
    if( TrHdl != _TRMEM_HDL_NONE ) {
        _trmem_prt_list( TrHdl );
        _trmem_close( TrHdl );
        if( TrFile != NULL ) {
            fclose( TrFile );
            TrFile = NULL;
        }
        TrHdl = _TRMEM_HDL_NONE;
    }
}

static void *check_nomem( void *ptr )
{
    if( ptr == NULL ) {
        if( nomem_cb != NULL ) {
            nomem_cb();
        }
    }
    return( ptr );
}

void * WRAPI WRMemAlloc( size_t size, _trmem_who who )
{
    if( TrHdl != _TRMEM_HDL_NONE ) {
        return( _trmem_alloc( size, who, TrHdl ) );
    } else {
        return( malloc( size ) );
    }
}

void * WRAPI WRMemAllocSafe( size_t size, _trmem_who who )
{
    if( TrHdl != _TRMEM_HDL_NONE ) {
        return( check_nomem( _trmem_alloc( size, who, TrHdl ) ) );
    } else {
        return( check_nomem( malloc( size ) ) );
    }
}

char * WRAPI WRMemStrdup( const char *str, _trmem_who who )
/*********************************************************/
{
    if( TrHdl != _TRMEM_HDL_NONE ) {
        return( _trmem_strdup( str, who, TrHdl ) );
    } else {
        return( strdup( str ) );
    }
}

char * WRAPI WRMemStrdupSafe( const char *str, _trmem_who who )
/*************************************************************/
{
    if( TrHdl != _TRMEM_HDL_NONE ) {
        return( check_nomem( _trmem_strdup( str, who, TrHdl ) ) );
    } else {
        return( check_nomem( strdup( str ) ) );
    }
}

void WRAPI WRMemFree( void *ptr, _trmem_who who )
{
    if( TrHdl != _TRMEM_HDL_NONE ) {
        _trmem_free( ptr, who, TrHdl );
    } else {
        free( ptr );
    }
}

void * WRAPI WRMemRealloc( void *ptr, size_t size, _trmem_who who )
{
    if( TrHdl != _TRMEM_HDL_NONE ) {
        return( _trmem_realloc( ptr, size, who, TrHdl ) );
    } else {
        return( realloc( ptr, size ) );
    }
}

int WRAPI WRMemValidate( void *ptr, _trmem_who who )
{
    if( TrHdl != _TRMEM_HDL_NONE ) {
        return( _trmem_validate( ptr, who, TrHdl ) );
    } else {
        return( TRUE );
    }
}

int WRAPI WRMemChkRange( void *start, size_t len, _trmem_who who )
{
    if( TrHdl != _TRMEM_HDL_NONE ) {
        return( _trmem_chk_range( start, len, who, TrHdl ) );
    } else {
        return( TRUE );
    }
}

void WRAPI WRMemPrtUsage( void )
{
    if( TrHdl != _TRMEM_HDL_NONE ) {
        _trmem_prt_usage( TrHdl );
    }
}

/* function to replace this in mem.c in commonui */

TRMEMAPI( MemAlloc )
void *MemAlloc( size_t size )
{
    void *p;

    if( TrHdl != _TRMEM_HDL_NONE ) {
        p = _trmem_alloc( size, _TRMEM_WHO( 1 ), TrHdl );
    } else {
        p = malloc( size );
    }
    if( p != NULL ) {
        memset( p, 0, size );
    }
    return( p );
}

TRMEMAPI( MemAllocSafe )
void *MemAllocSafe( size_t size )
{
    void *p;

    if( TrHdl != _TRMEM_HDL_NONE ) {
        p = check_nomem( _trmem_alloc( size, _TRMEM_WHO( 1 ), TrHdl ) );
    } else {
        p = check_nomem( malloc( size ) );
    }

    if( p != NULL ) {
        memset( p, 0, size );
    }

    return( p );
}

TRMEMAPI( MemStrdup )
char *MemStrdup( const char *str )
{
    if( TrHdl != _TRMEM_HDL_NONE ) {
        return( _trmem_strdup( str, _TRMEM_WHO( 1 ), TrHdl ) );
    } else {
        return( strdup( str ) );
    }
}

TRMEMAPI( MemStrdupSafe )
char *MemStrdupSafe( const char *str )
{
    if( TrHdl != _TRMEM_HDL_NONE ) {
        return( check_nomem( _trmem_strdup( str, _TRMEM_WHO( 1 ), TrHdl ) ) );
    } else {
        return( check_nomem( strdup( str ) ) );
    }
}

/* function to replace this in mem.c in commonui */

TRMEMAPI( MemRealloc )
void *MemRealloc( void *ptr, size_t size )
{
    if( TrHdl != _TRMEM_HDL_NONE ) {
        return( _trmem_realloc( ptr, size, _TRMEM_WHO( 3 ), TrHdl ) );
    } else {
        return( realloc( ptr, size ) );
    }
}

TRMEMAPI( MemReallocSafe )
void *MemReallocSafe( void *ptr, size_t size )
{
    if( TrHdl != _TRMEM_HDL_NONE ) {
        return( check_nomem( _trmem_realloc( ptr, size, _TRMEM_WHO( 3 ), TrHdl ) ) );
    } else {
        return( check_nomem( realloc( ptr, size ) ) );
    }
}

/* function to replace this in mem.c in commonui */

TRMEMAPI( MemFree )
void MemFree( void *ptr )
{
    if( TrHdl != _TRMEM_HDL_NONE ) {
        _trmem_free( ptr, _TRMEM_WHO( 5 ), TrHdl );
    } else {
        free( ptr );
    }
}
