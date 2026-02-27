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
* Description:  Memory management routines for linker.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined( __WATCOMC__ )
    #include <malloc.h>     /* necessary for __(n/f)memneed */
#endif
#if defined( __QNX__ )
    #include <sys/seginfo.h>
#endif
#include "linkstd.h"
#include "library.h"
#include "virtmem.h"
#include "reloc.h"
#include "objcache.h"
#include "memfuncs.h"
#ifdef TRMEM
    #include "trmem.h"
    #include "ideentry.h"
#endif


#if defined( TRMEM ) && defined( _M_IX86 ) && ( __WATCOMC__ > 1290 )
#define _XSTR(s)    # s
#define TRMEMAPI(x) _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
#endif

#if defined( __QNX__ )
unsigned LastChanceSeg;
#endif

#ifdef TRMEM

void    *TrHdl;

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

#endif  /* TRMEM */

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
    TrHdl = _trmem_open( malloc, free, realloc, strdup,
            NULL, PrintLine, _TRMEM_DEF );
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

static void *check_nomem( void *ptr )
{
    if( ptr == NULL ) {
        LnkMsg( FTL + MSG_NO_DYN_MEM, NULL );       // see note 1 below
    }
    return( ptr );
}

#ifdef TRMEM
static void *doAlloc( size_t size, _trmem_who who )
#else
static void *doAlloc( size_t size )
#endif
{
    void    *ptr;

    for( ;; ) {
#ifdef TRMEM
        ptr = _trmem_alloc( size, who, TrHdl );
#else
        ptr = malloc( size );
#endif
        if( ptr != NULL || !FreeUpMemory( false ) ) {
            break;
        }
    }
    return( ptr );
}

TRMEMAPI( MemAlloc )
void *MemAlloc( size_t size )
/***************************/
{
    void    *ptr;

#ifdef TRMEM
    ptr = doAlloc( size, _TRMEM_WHO( 1 ) );
#else
    ptr = doAlloc( size );
#endif
    if( ptr != NULL )
        memset( ptr, 0, size );
    return( ptr );
}

TRMEMAPI( MemAllocSafe )
void *MemAllocSafe( size_t size )
/*******************************/
{
    void            *ptr;

#ifdef TRMEM
    ptr = check_nomem( doAlloc( size, _TRMEM_WHO( 1 ) ) );
#else
    ptr = check_nomem( doAlloc( size ) );
#endif
    return( memset( ptr, 0, size ) );
}

#ifdef TRMEM
static char *doStrdup( const char *str, _trmem_who who )
#else
static char *doStrdup( const char *str )
#endif
/**************************************/
{
    char            *ptr;

    for( ;; ) {
#ifdef TRMEM
        ptr = _trmem_strdup( str, who, TrHdl );
#else
        ptr = strdup( str );
#endif
        if( ptr != NULL || !FreeUpMemory( false ) ) {
            break;
        }
    }
    return( ptr );
}

TRMEMAPI( MemStrdupSafe )
char *MemStrdupSafe( const char *str )
/************************************/
{
#ifdef TRMEM
    return( check_nomem( doStrdup( str, _TRMEM_WHO( 3 ) ) ) );
#else
    return( check_nomem( doStrdup( str ) ) );
#endif
}

#ifdef TRMEM
static void *doRealloc( void *src, size_t size, _trmem_who who )
#else
static void *doRealloc( void *src, size_t size )
#endif
/***********************************************
 * reallocate a block of memory.
 * Notes for MemRealloc
 * NOTE 1: we don't want to call FreeUpMemory, since that does a permshrink
 * and this function is called from permshrink
 */
{
    void    *ptr;

    for( ;; ) {
#ifdef TRMEM
        ptr = _trmem_realloc( src, size, who, TrHdl );
#else
        ptr = realloc( src, size );
#endif
        if( ptr != NULL || !FreeUpMemory( true ) ) {
            break;
        }
    }
    return( ptr );
}

TRMEMAPI( MemRealloc )
void *MemRealloc( void *src, size_t size )
/****************************************/
{
#ifdef TRMEM
    return( doRealloc( src, size, _TRMEM_WHO( 4 ) ) );
#else
    return( doRealloc( src, size ) );
#endif
}

TRMEMAPI( MemReallocSafe )
void *MemReallocSafe( void *src, size_t size )
/********************************************/
{
#ifdef TRMEM
    return( check_nomem( doRealloc( src, size, _TRMEM_WHO( 4 ) ) ) );
#else
    return( check_nomem( doRealloc( src, size ) ) );
#endif
}

TRMEMAPI( MemFree )
void MemFree( void *p )
/************************/
{
#ifdef TRMEM
    _trmem_free( p, _TRMEM_WHO( 6 ), TrHdl );
#else
    free( p );
#endif
}

TRMEMAPI( MemToStringSafe )
char *MemToStringSafe( const void *mem, size_t size )
/***************************************************/
{
    char            *ptr;

#ifdef TRMEM
    ptr = check_nomem( doAlloc( size + 1, _TRMEM_WHO( 1 ) ) );
#else
    ptr = check_nomem( doAlloc( size + 1 ) );
#endif
    memcpy( ptr, mem, size );
    ptr[size] = '\0';
    return( ptr );
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

#ifdef DEVBUILD
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

bool FreeUpMemory( bool skip )
/*****************************
 * The MemRealloc.. function uses skip = true to skip PermShrink, which
 * calls the MemRealloc function, which would cause "infinite" recursion.
 */
{
    if( !skip ) {
#if defined( __QNX__ )
        if( LastChanceSeg != (unsigned)-1 ) {
            /*
             * If we're low on memory, the system is low on memory. Give
             * something back to the OS so it can do it's job, and don't
             * ever ask it for anything more.
             */
            qnx_segment_free( LastChanceSeg );
            LastChanceSeg = -1;
            _heapenable( 0 );
        }
#endif
        if( PermShrink() ) {
            return( true );
        }
    }
    return( CacheRelease() || SwapOutVirt() || SwapOutRelocs() );
}

#if defined( __WATCOMC__ )
int __nmemneed( size_t amount )
/*****************************/
{
    /* unused parameters */ (void)amount;

    return( FreeUpMemory( false ) );
}
#endif
