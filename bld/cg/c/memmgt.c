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
* Description:  Internal memory management with optional tracking.
*
****************************************************************************/


#include "_cgstd.h"
#include <stdio.h>
#include "memsydep.h"
#include "memcheck.h"
#include "zoiks.h"
#include "spawn.h"
#include "memout.h"
#include "cgmem.h"
#include "optmac.h"
#include "memlimit.h"
#include "onexit.h"
#include "dumpio.h"
#include "memmgt.h"
#ifdef TRMEM
    #include "trmem.h"
#endif


#if defined( TRMEM ) && defined( _M_IX86 ) && ( __WATCOMC__ > 1290 )
#define _XSTR(s)    # s
#define TRMEMAPI(x) _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
#endif

static mem_out_action   MemOut;
#ifdef _CHUNK_TRACKING
static uint             Chunks;
#endif

#ifdef TRMEM

static _trmem_hdl       Handle;
static bool             local_init;

static void PrintLine( void *handle, const char *buff, size_t len )
/*****************************************************************/
{
    /* unused parameters */ (void)handle; (void)len;

    fprintf( stderr, "%s\n", buff );
}

#endif /* TRMEM */

void    CGMemInit( pointer trmemhdl )
/***********************************/
{
    _SysReInit();
    MemOut = MO_FATAL;
#ifdef TRMEM
    if( trmemhdl == NULL ) {
        Handle = _trmem_open( _SysAlloc, _SysFree, _TRMEM_NO_REALLOC, _TRMEM_NO_STRDUP,
                              NULL, PrintLine, _TRMEM_DEF );
        local_init = true;
    } else {
        Handle = trmemhdl;
        local_init = false;
    }
#else
    /* unused parameters */ (void)trmemhdl;
#endif
#ifdef _CHUNK_TRACKING
    Chunks = 0;
#endif
    CalcMemLimit();
}

void    CGMemFini( void )
/***********************/
{
#ifdef TRMEM
    const char  *envvar;

    if( local_init ) {
        envvar = FEGetEnv( "TRQUIET" );
        if( envvar == NULL ) {
            _trmem_prt_list( Handle );
        }
        _trmem_close( Handle );
    }
#endif
#ifdef _CHUNK_TRACKING
    if( Chunks != 0 ) {
        _Zoiks( ZOIKS_002 );
    }
#endif
    MemFini();
}


mem_out_action    SetMemOut( mem_out_action what )
/************************************************/
{
    mem_out_action      old;

    old = MemOut;
    MemOut = what;
    return( old );
}

#if defined( TRMEM ) && defined( _M_IX86 )
pointer _CGAlloc( size_t size, _trmem_who who )
#else
pointer CGAlloc( size_t size )
#endif
/******************************************/
{
    pointer     chunk;

    _MemLow;
    for( ;; ) {
#if defined( TRMEM ) && defined( _M_IX86 )
        chunk = _trmem_alloc( size, who, Handle );
#elif defined( TRMEM )
        chunk = _trmem_alloc( size, NULL, Handle );
#else
        chunk = _SysAlloc( size );
#endif
        if( chunk != NULL ) {
#ifdef _CHUNK_TRACKING
            ++Chunks;
#endif
            _AlignmentCheck( chunk, 8 );
            return( chunk );
        }
        if( !_MemCheck( size ) ) {
            break;
        }
    }
    if( ( MemOut == MO_FATAL ) || ( InOptimizer != 0 ) ) {
        FatalError( "Out of memory" );
    } else if( MemOut == MO_SUICIDE ) {
        Suicide();
    }
    return( NULL );
}

#if defined( TRMEM )
TRMEMAPI( CGAlloc )
pointer CGAlloc( size_t size )
/****************************/
{
    return( _CGAlloc( size, _TRMEM_WHO( 3 ) ) );
}
#endif

#if defined( TRMEM ) && defined( _M_IX86 )
void    _CGFree( pointer chunk, _trmem_who who )
#else
void    CGFree( pointer chunk )
#endif
/*******************************************/
{
#ifdef _CHUNK_TRACKING
    --Chunks;
#endif
#if defined( TRMEM ) && defined( _M_IX86 )
    _trmem_free( chunk, who, Handle );
#elif defined( TRMEM )
    _trmem_free( chunk, NULL, Handle );
#else
    _SysFree( chunk );
#endif
}

#if defined( TRMEM )
TRMEMAPI( CGFree )
void    CGFree( pointer chunk )
/*****************************/
{
    _CGFree( chunk, _TRMEM_WHO( 4 ) );
}
#endif

#ifdef TRMEM
void    DumpMem( void )
/*********************/
{
    _trmem_prt_usage( Handle );
}
#endif
