/****************************************************************************
*
*                            Open Watcom Project
*
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
#ifdef __WATCOMC__
    #include <malloc.h>     /* for _expand() */
#endif
#ifdef TRMEM
    #include "trmem.h"
#endif
#include "linkstd.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "library.h"
#include "virtmem.h"
#include "reloc.h"
#include "objcache.h"
#include "alloc.h"
#if defined( __QNX__ )
    #include <sys/seginfo.h>

    unsigned LastChanceSeg;
#endif

#ifdef _INT_DEBUG
    static  int     Chunks;
#endif

#ifdef TRMEM
#include "ideentry.h"

void    *TrHdl;

static void         PrintAllMem( void );
#endif

static bool         CacheRelease( void );

#ifdef TRMEM

static void PrintLine( void *bogus, const char *buff, unsigned len )
{
    bogus = bogus;      /* to avoid a warning */
    len = len;
    WriteStdOut( (void *)buff );
}
#endif

void LnkMemInit( void )
/*********************/
{
#if defined( __QNX__ )
    /* allocate some memory we can give back to the system if it runs low */
    LastChanceSeg = qnx_segment_alloc( 65000 );
#endif
#ifdef _INT_DEBUG
    Chunks = 0;
#endif
#ifdef TRMEM
    TrHdl = _trmem_open( malloc, free, realloc, _expand,
            NULL, PrintLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 | _TRMEM_REALLOC_NULL |
            _TRMEM_FREE_NULL | _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
#endif
}


void LnkMemFini( void )
/*********************/
{
#ifdef _INT_DEBUG
    if( Chunks != 0 ) {
        DEBUG( (DBG_ALWAYS, "%d Chunks unfreed", Chunks ) );
    }
#endif
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
        if( !FreeUpMemory() ) break;
    }
#ifdef _INT_DEBUG
    if( p != NULL ) ++Chunks;
#endif
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


void LFree( void *p )
/**************************/
{
    if( p == NULL ) return;
#ifdef TRMEM
    _trmem_free( p, _trmem_guess_who(), TrHdl );
#else
    free( p );
#endif
#ifdef _INT_DEBUG
    --Chunks;
#endif
}

void *LnkExpand( void *src, size_t size )
/***************************************/
// try to expand a block of memory
{
#ifdef _ZDOS
    src = src; size = size;
    return ( NULL );
#else
  #ifdef TRMEM
    return( _trmem_expand( src, size, _trmem_guess_who(), TrHdl ) );
  #else
    return( _expand( src, size ) );
  #endif
#endif
}

void *LnkReAlloc( void *src, size_t size )
/****************************************/
// reallocate a block of memory.
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
        if( dest != NULL ) break;
        if( !CacheRelease() && !SwapOutVirt() && !SwapOutRelocs() ) {
            LnkMsg( FTL + MSG_NO_DYN_MEM, NULL );       // see note 1 below
        }
    }
    return( dest );
}
/* Notes for LnkReAlloc
 * NOTE 1: we don't want to call FreeUpMemory, since that does a permshrink
 * and this function is called from permshrink
*/

#ifdef TRMEM
int ValidateMem( void )
/*********************/
{
    return( _trmem_validate_all( TrHdl ) );
}

void PrintAllMem( void )
/**********************/
{
    if( _trmem_prt_list( TrHdl ) == 0 ) {
        _trmem_prt_usage( TrHdl );
    }
}
#endif

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

bool FreeUpMemory( void )
/***********************/
// make sure LnkReAlloc is kept up to date with what is put in here.
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

int __nmemneed( size_t amount )
/*****************************/
{
    amount = amount;
    return( FreeUpMemory() );
}

#ifdef _M_I86
int __fmemneed( size_t amount )
/*****************************/
{
    amount = amount;
    return( FreeUpMemory() );
}
#endif
