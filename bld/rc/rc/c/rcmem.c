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
* Description:  Resource compiler memory management routines.
*
****************************************************************************/


#include "global.h"
#include <assert.h>
#include "rcerrors.h"
#include "rcmem.h"
#include "rcalloc0.h"
#include "rcalloc1.h"
#include "rcrtns.h"
#include "preproc.h"
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

static _trmem_hdl   RcMemHandle = NULL;

static void RcPrintMemLine( void *dummy, const char *buf, size_t len )
/********************************************************************/
{
    /* unused parameters */ (void)dummy; (void)len;

    printf( "%s\n", buf );
}

#endif  /* TRMEM */

void RcMemInit( void )
/********************/
{
#ifdef TRMEM
    RcMemHandle = _trmem_open( malloc, free, realloc, _TRMEM_NO_STRDUP,
                        NULL, RcPrintMemLine, _TRMEM_DEF );
#else
    RCMemLayer1Init();
#endif
}

void RcMemShutdown( void )
/************************/
{
#ifdef TRMEM
     RcPrintMemList();
    _trmem_close( RcMemHandle );
#else
     RCMemLayer1ShutDown();
#endif
}

static void *check_nomem( void *ptr )
{
    if( ptr == NULL ) {
        RcFatalError( ERR_OUT_OF_MEMORY );
    }
    return( ptr );
}

TRMEMAPI( MemAlloc )
void *MemAlloc( size_t size )
/*****************************/
{
#ifdef TRMEM
    return( _trmem_alloc( size, _TRMEM_WHO( 1 ), RcMemHandle ) );
#else
    return( RCMemLayer1Malloc( size ) );
#endif
}

TRMEMAPI( MemAllocSafe )
void *MemAllocSafe( size_t size )
/*******************************/
{
#ifdef TRMEM
    return( check_nomem( _trmem_alloc( size, _TRMEM_WHO( 2 ), RcMemHandle ) ) );
#else
    return( check_nomem( RCMemLayer1Malloc( size ) ) );
#endif
}

TRMEMAPI( MemFree )
void MemFree( void *ptr )
/*************************/
{
#ifdef TRMEM
    _trmem_free( ptr, _TRMEM_WHO( 3 ), RcMemHandle );
#else
    RCMemLayer1Free( ptr );
#endif
}

TRMEMAPI( MemRealloc )
void *MemRealloc( void *old_ptr, size_t newsize )
/*************************************************/
{
#ifdef TRMEM
    return( _trmem_realloc( old_ptr, newsize, _TRMEM_WHO( 4 ), RcMemHandle ) );
#else
    return( RCMemLayer1Realloc( old_ptr, newsize ) );
#endif
}

TRMEMAPI( MemReallocSafe )
void *MemReallocSafe( void *old_ptr, size_t newsize )
/***************************************************/
{
    void    *ptr;

#ifdef TRMEM
    ptr = _trmem_realloc( old_ptr, newsize, _TRMEM_WHO( 5 ), RcMemHandle );
#else
    ptr = RCMemLayer1Realloc( old_ptr, newsize );
#endif
    if( newsize ) {
        return( check_nomem( ptr ) );
    }
    return( ptr );
}

TRMEMAPI( MemStrdup )
char *MemStrdup( const char *str )
/**********************************/
{
    if( str != NULL ) {
#ifdef TRMEM
        return( _trmem_strdup( str, _TRMEM_WHO( 6 ), RcMemHandle ) );
#else
        void    *ptr;
        size_t  size;

        size = strlen( str ) + 1;
        ptr = RCMemLayer1Malloc( size );
        if( ptr != NULL ) {
            return( strcpy( ptr, str ) );
        }
#endif
    }
    return( NULL );
}

TRMEMAPI( MemStrdupSafe )
char *MemStrdupSafe( const char *str )
/************************************/
{
    if( str == NULL )
        return( NULL );
#ifdef TRMEM
    return( check_nomem( _trmem_strdup( str, _TRMEM_WHO( 7 ), RcMemHandle ) ) );
#else
    return( strcpy( check_nomem( RCMemLayer1Malloc( strlen( str ) + 1 ) ), str ) );
#endif
}

#ifdef TRMEM
void RcPrintMemUsage( void )
/**************************/
{
    if( RcMemHandle != NULL ) {
        _trmem_prt_usage( RcMemHandle );
    }
}

void RcPrintMemList( void )
/*************************/
{
    if( RcMemHandle != NULL ) {
        _trmem_prt_list( RcMemHandle );
    }
}

TRMEMAPI( RcMemValidate )
int RcMemValidate( void *ptr )
/****************************/
{
    if( RcMemHandle != NULL ) {
        return( _trmem_validate( ptr, _TRMEM_WHO( 8 ), RcMemHandle ) );
    } else {
        return( 0 );
    }
}

TRMEMAPI( RcMemChkRange )
int RcMemChkRange( void *start, size_t len )
/******************************************/
{
    if( RcMemHandle != NULL ) {
        return( _trmem_chk_range( start, len, _TRMEM_WHO( 9 ), RcMemHandle ) );
    } else {
        return( 0 );
    }
}
#endif

TRMEMAPI( PPMemAlloc )
void *PPMemAlloc( size_t size )
/*****************************/
{
#ifdef TRMEM
    return( _trmem_alloc( size, _TRMEM_WHO( 10 ), RcMemHandle ) );
#else
    return( RCMemLayer1Malloc( size ) );
#endif
}

TRMEMAPI( PPMemFree )
void PPMemFree( void *ptr )
/*************************/
{
#ifdef TRMEM
    _trmem_free( ptr, _TRMEM_WHO( 11 ), RcMemHandle );
#else
    RCMemLayer1Free( ptr );
#endif
}
