/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Memory management routines for linker.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined( __WATCOMC__ )
    #include <malloc.h>     /* necessary for __(n/f)memneed */
#endif
#include "trmem.h"
#include "linkstd.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "library.h"
#include "virtmem.h"
#include "reloc.h"
#include "objcache.h"
#include "alloc.h"
#include "wresmem.h"
#if defined( __QNX__ )
    #include <sys/seginfo.h>

    unsigned LastChanceSeg;
#endif

#ifdef TRMEM
#include "ideentry.h"

void    *TrHdl;
#endif

#ifdef TRMEM
static void PrintLine( void *bogus, const char *buff, size_t len )
{
    /* unused parameters */ (void)bogus; (void)len;

    WriteStdOutWithNL( (void *)buff );
}

static void PrintAllMem( void )
/*****************************/
{
    if( _trmem_prt_list( TrHdl ) == 0 ) {
        _trmem_prt_usage( TrHdl );
    }
}
#endif

static bool CacheRelease( void )
/******************************/
{
    bool   freed;

    freed = DumpObjCache();
    if( !freed ) {
        freed = DiscardDicts();      /* .. discard dictionarys */
    }
    return( freed );
}

void LnkMemInit( void )
/*********************/
{
#if defined( __QNX__ )
    /* allocate some memory we can give back to the system if it runs low */
    LastChanceSeg = qnx_segment_alloc( 65000 );
#endif
#ifdef TRMEM
    TrHdl = _trmem_open( malloc, free, realloc, NULL, NULL, PrintLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 | _TRMEM_REALLOC_NULL |
            _TRMEM_FREE_NULL | _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
#endif
}


void LnkMemFini( void )
/*********************/
{
#ifdef TRMEM
    PrintAllMem();
    _trmem_close( TrHdl );
#endif
}

#ifdef TRMEM
static void *DoLAlloc( size_t size, void (*ra)( void ) )
#else
void *LAlloc( size_t size )
#endif
{
    void    *p;

    for( ;; ) {
#ifdef TRMEM
        p = _trmem_alloc( size, ra, TrHdl );
#else
        p = malloc( size );
#endif
        if( p != NULL ) {
            memset( p, 0, size );
            break;
        }
        if( !FreeUpMemory() ) {
            break;
        }
    }
    return( p );
}

#ifdef TRMEM
 void *LAlloc( size_t size )
/**************************/
{
    void        (*ra)( void );

    ra = _trmem_guess_who();

    return( DoLAlloc( size, ra ) );
}
#endif

void *ChkLAlloc( size_t size )
/****************************/
{
    void                *ptr;
#ifdef TRMEM
    void                (*ra)( void );

    ra = _trmem_guess_who();

    ptr = DoLAlloc( size, ra );
#else
    ptr = LAlloc( size );
#endif
    if( ptr == NULL ) {
        LnkMsg( FTL + MSG_NO_DYN_MEM, NULL );
    }
    return( ptr );
}

void *wres_alloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), TrHdl ) );
#else
    return( malloc( size ) );
#endif
}


void LFree( void *p )
/**************************/
{
    if( p == NULL )
        return;
#ifdef TRMEM
    _trmem_free( p, _trmem_guess_who(), TrHdl );
#else
    free( p );
#endif
}

void wres_free( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), TrHdl );
#else
    free( ptr );
#endif
}

void *LnkRealloc( void *src, size_t size )
/****************************************/
/*
 * reallocate a block of memory.
 * Notes for LnkRealloc
 * NOTE 1: we don't want to call FreeUpMemory, since that does a permshrink
 * and this function is called from permshrink
*/
{
    void    *dest;
#ifdef TRMEM
    void        (*ra)( void );

    ra = _trmem_guess_who(); /* must be first thing */
#endif
    for( ;; ) {
#ifdef TRMEM
        dest = _trmem_realloc( src, size, ra, TrHdl );
#else
        dest = realloc( src, size );
#endif
        if( dest != NULL )
            break;
        if( !CacheRelease() && !SwapOutVirt() && !SwapOutRelocs() ) {
            LnkMsg( FTL + MSG_NO_DYN_MEM, NULL );       // see note 1 below
        }
    }
    return( dest );
}

int ValidateMem( void )
/*********************/
{
#ifdef TRMEM
    return( _trmem_validate_all( TrHdl ) );
#else
    return( true );
#endif
}

#ifndef NDEBUG
void DbgZapAlloc( void *tgt, size_t size )
/****************************************/
{
    memset( tgt, 0xA5, size );
}

void DbgZapFreed( void *tgt, size_t size )
/****************************************/
{
    memset( tgt, 0xBD, size );
}
#endif

bool FreeUpMemory( void )
/***********************/
// make sure LnkRealloc is kept up to date with what is put in here.
{
#if defined( __QNX__ )
    if( LastChanceSeg != (unsigned)-1 ) {
        /*
            If we're low on memory, the system is low on memory. Give
            something back to the OS so it can do it's job, and don't
            ever ask it for anything more.
        */
        qnx_segment_free( LastChanceSeg );
        LastChanceSeg = -1;
        _heapenable( 0 );
    }
#endif
    return( PermShrink() || CacheRelease() || SwapOutVirt() || SwapOutRelocs() );
}

#if defined( __WATCOMC__ )
int __nmemneed( size_t amount )
/*****************************/
{
    /* unused parameters */ (void)amount;

    return( FreeUpMemory() );
}

#ifdef _M_I86
int __fmemneed( size_t amount )
/*****************************/
{
    /* unused parameters */ (void)amount;

    return( FreeUpMemory() );
}
#endif
#endif
