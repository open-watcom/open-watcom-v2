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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


/*
  Mem : memory management routines for linker

*/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#ifdef TRACKER
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
#if _OS == _QNX
    #include <sys/seginfo.h>

    unsigned LastChanceSeg;
#endif

#ifdef _INT_DEBUG
    static  int     Chunks;
#endif

#ifdef TRACKER
#include "fileio.h"

void    *TrHdl;
#endif

static bool         CacheRelease( void );

#ifdef TRACKER

void PrintLine( void * bogus, const char *buff, unsigned len )
{
    bogus = bogus;      /* to avoid a warning */
    len = len;
    WriteStdOut( (void *) buff );
}
#endif

extern void LnkMemInit( void )
/****************************/
{
#if  _OS == _OS2HOST

    if( _osmode == OS2_MODE ) {
        LinkState |= RUNNING_OS2_FLAG;
    }
#elif _OS == _QNX
    /* allocate some memory we can give back to the system if it runs low */
    LastChanceSeg = qnx_segment_alloc( 65000 );
#endif
#ifdef _INT_DEBUG
    Chunks = 0;
#endif
#ifdef TRACKER
    TrHdl = _trmem_open( malloc, free, realloc, _expand,
            NULL, PrintLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 | _TRMEM_REALLOC_NULL |
            _TRMEM_FREE_NULL | _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
#endif
}


extern void LnkMemFini( void )
/****************************/
{
#ifdef _INT_DEBUG
    if( Chunks != 0 ) {
        DEBUG( (DBG_ALWAYS, "%d Chunks unfreed", Chunks ) );
    }
#endif
#ifdef TRACKER
    PrintAllMem();
    _trmem_close( TrHdl );
#endif
}

#ifdef TRACKER
extern void *LAlloc( unsigned size )
/**********************************/
{
    extern void *DoLAlloc( unsigned, void (*)() );
    void        (*ra)();

    ra = _trmem_guess_who();

    return( DoLAlloc( size, ra ) );
}

void *DoLAlloc( unsigned size, void (*ra)() )
#else
void *LAlloc( unsigned size )
#endif
{
    void    *p;

    for( ;; ) {
#ifdef TRACKER
        p = _trmem_alloc( size, ra, TrHdl );
#else
        p = malloc( size );
#endif
        if( p != NULL ) break;
        if( !FreeUpMemory() ) break;
    }
#ifdef _INT_DEBUG
    if( p != NULL ) ++Chunks;
#endif
    return( p );
}

extern void * ChkLAlloc( unsigned size )
/**************************************/
{
    void                *ptr;
#ifdef TRACKER
    void                (*ra)();

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

#if 0   // not used anymore
#ifdef TRACKER
void *TryAlloc( unsigned size )
/*****************************/
{
    static void *DoTryAlloc( unsigned, void (*)() );
    void        (*ra)();

    ra = _trmem_guess_who();

    return( DoTryAlloc( size, ra ) );
}

static void *DoTryAlloc( unsigned size, void (*ra)() )
#else
void *TryAlloc( unsigned size )
#endif
{
    void    *p;

#ifdef TRACKER
    p = _trmem_alloc( size, ra, TrHdl );
#else
    p = malloc( size );
#endif
#ifdef _INT_DEBUG
    if( p != NULL ) ++Chunks;
#endif
    return( p );
}
#endif


extern void LFree( void *p )
/**************************/
{
    if( p == NULL ) return;
#ifdef TRACKER
    _trmem_free( p, _trmem_guess_who(), TrHdl );
#else
    free( p );
#endif
#ifdef _INT_DEBUG
    --Chunks;
#endif
}

extern void * LnkExpand( void *src, unsigned size )
/*************************************************/
// try to expand a block of memory
{
#ifdef TRACKER
    return( _trmem_expand( src, size, _trmem_guess_who(), TrHdl ) );
#else
    return( _expand( src, size ) );
#endif
}

extern void * LnkReAlloc( void *src, unsigned size )
/**************************************************/
// reallocate a block of memory.
{
    void *  dest;
#ifdef TRACKER
    void        (*ra)();

    ra = _trmem_guess_who(); /* must be first thing */
#endif
    for(;;) {
#ifdef TRACKER
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

#ifdef TRACKER
extern int ValidateMem( void )
/*****************************/
{
    return _trmem_validate_all( TrHdl );
}

extern void PrintAllMem( void )
/*****************************/
{
    if( _trmem_prt_list( TrHdl ) == 0 ) {
        _trmem_prt_usage( TrHdl );
    }
}
#endif

#ifndef NDEBUG
extern void DbgZapAlloc( void *tgt, unsigned size )
/*************************************************/
{
    memset( tgt, 0xA5, size );
}

extern void DbgZapFreed( void *tgt, unsigned size )
/*************************************************/
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

extern bool FreeUpMemory( void )
/******************************/
// make sure LnkReAlloc is kept up to date with what is put in here.
{
#if _OS == _QNX
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
    return PermShrink() || CacheRelease() || SwapOutVirt() || SwapOutRelocs();
}

extern int __nmemneed( size_t amount )
/************************************/
{
    amount = amount;
    return FreeUpMemory();
}

#ifdef M_I86
extern int __fmemneed( size_t amount )
/************************************/
{
    amount = amount;
    return FreeUpMemory();
}
#endif
