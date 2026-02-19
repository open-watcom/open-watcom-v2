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
* Description:  memory management routines for ms2wlink
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ms2wlink.h"
#ifdef TRMEM
//    #include "wio.h"
    #include "trmem.h"
#endif

#include "clibext.h"


#if defined( TRMEM ) && defined( _M_IX86 ) && ( __WATCOMC__ > 1290 )
#define _XSTR(s)    # s
#define TRMEMAPI(x) _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
#endif

#ifdef TRMEM

static _trmem_hdl TrHdl;

static void PrintLine( void *parm, const char *buff, size_t len )
/****************************************************************/
{
    /* unused parameters */ (void)parm; (void)len;

    QWrite( stderr, buff, len, NULL );
    QWriteNL( stderr, NULL );
}

#endif  /* TRMEM */

void MemInit( void )
/******************/
{
#ifdef TRMEM
    TrHdl = _trmem_open( malloc, free, _TRMEM_NO_REALLOC, strdup,
                            NULL, PrintLine, _TRMEM_DEF );
#endif
}


void MemFini( void )
/******************/
{
#ifdef TRMEM
    _trmem_prt_list( TrHdl );
    _trmem_close( TrHdl );
#endif
}

static void *check_nomem( void *ptr )
{
    if( ptr == NULL ) {
        ErrorExit( "Dynamic Memory Exhausted!!!" );
    }
    return( ptr );
}

TRMEMAPI( MemAlloc )
void *MemAlloc( size_t size )
/***************************/
{
#ifdef TRMEM
    return( _trmem_alloc( size, _TRMEM_WHO( 1 ), TrHdl ) );
#else
    return( malloc( size ) );
#endif
}

TRMEMAPI( MemAllocSafe )
void *MemAllocSafe( size_t size )
/*******************************/
{
#ifdef TRMEM
    return( check_nomem( _trmem_alloc( size, _TRMEM_WHO( 2 ), TrHdl ) ) );
#else
    return( check_nomem(  malloc( size ) ) );
#endif
}

TRMEMAPI( MemStrdupSafe )
char *MemStrdupSafe( const char *str )
/************************************/
{
#ifdef TRMEM
    return( check_nomem( _trmem_strdup( str, _TRMEM_WHO( 3 ), TrHdl ) ) );
#else
    return( check_nomem( strdup( str ) ) );
#endif
}

TRMEMAPI( MemFree )
void MemFree( void *p )
/*********************/
{
    if( p == NULL )
        return;
#ifdef TRMEM
    _trmem_free( p, _TRMEM_WHO( 4 ), TrHdl );
#else
    free( p );
#endif
}
