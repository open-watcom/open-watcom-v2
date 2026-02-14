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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#define USE_CG_MEMMGT

#include "plusplus.h"
#include <stddef.h>
#include "memmgr.h"
#include "ring.h"
#include "initdefs.h"
#include "pragdefn.h"
#include "codegen.h"
#ifdef TRMEM
    #include "trmem.h"
#endif
#ifdef DEVBUILD
    #include "togglesd.h"
#endif


#if !defined( USE_CG_MEMMGT ) && defined( TRMEM ) && defined( _M_IX86 ) && ( __WATCOMC__ > 1290 )
#define _XSTR(s)    # s
#define TRMEMAPI(x) _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
#endif

#if defined( TRMEM ) && !defined( USE_CG_MEMMGT ) && defined( _M_IX86 )
    #define alloc_mem(s,n)  _trmem_alloc( s, who, trackerHdl )
    #define _doFree(p,n)    _trmem_free( p, _trmem_guess_who(), trackerHdl )
    #define _MemAlloc(p,n)  _CMemAlloc( p, _trmem_guess_who() )
    #define _MemAllocW(p)   _CMemAlloc( p, who )
    #define _addPerm(p,n)   addPerm( p, _trmem_guess_who() )
#else
  #ifdef USE_CG_MEMMGT
    #define alloc_mem(s,n)  BEMemAlloc( s )
    #define _doFree(p,n)    BEMemFree( p )
  #elif defined( TRMEM )
    #define alloc_mem(s,n)  _trmem_alloc( s, _TRMEM_ROUTINE(n), trackerHdl )
    #define _doFree(p,n)    _trmem_free( p, _TRMEM_ROUTINE(n), trackerHdl )
  #else
    #define alloc_mem(s,n)  malloc( s )
    #define _doFree(p,n)    free( p )
  #endif
    #define _MemAlloc(p,n)  CMemAlloc( p )
    #define _MemAllocW(p)   CMemAlloc( p )
    #define _addPerm(p,n)   addPerm( p )
#endif

typedef struct cleanup *CLEANPTR;
struct cleanup {
    CLEANPTR            next;
    void                (*rtn)( void );
};

typedef struct perm_blk *PERMPTR;
typedef struct perm_blk {
    PERMPTR             next;
    size_t              amt_left;
    size_t              size;
    char                mem[1];
} perm_blk;
#define PERM_MAX_ALLOC  (1024)
#define PERM_MIN_ALLOC  (128)

static CLEANPTR cleanupList;
static PERMPTR permList;

#ifdef DEVBUILD
static void *deferredFreeList;
#endif

#ifdef TRMEM

static _trmem_hdl   trackerHdl;

static void printLine( void *dummy, const char *buf, size_t len )
/***************************************************************/
{
    /* unused parameters */ (void)dummy; (void)len;

    fprintf( stdout, "%s\n", buf );
}

#endif /* TRMEM */

void CMemRegisterCleanup( void (*cleanup)( void ) )
/*************************************************/
{
    CLEANPTR new_cleanup;

    new_cleanup = (CLEANPTR)RingAlloc( &cleanupList, sizeof( *new_cleanup ) );
    new_cleanup->rtn = cleanup;
}

#if defined( TRMEM ) && !defined( USE_CG_MEMMGT ) && defined( _M_IX86 )
static void *_CMemAlloc( size_t size, pointer who )
#else
void *CMemAlloc( size_t size )
#endif
/*************************************************/
{
    PERMPTR p;
    CLEANPTR curr;

    if( size == 0 ) {
        return( NULL );
    }
#ifdef DEVBUILD
    if( !TOGGLEDBG( no_mem_cleanup ) ) {
        static unsigned test_cleanup;
        static unsigned test_inc = 1;

        test_cleanup += test_inc;
        if( test_cleanup > 1000 ) {
            test_cleanup = 0;
            RingIterBeg( cleanupList, curr ) {
                curr->rtn();
            } RingIterEnd( curr )
        }
    }
#endif
    p = alloc_mem( size, 1 );
    if( p != NULL ) {
        return( p );
    }
    RingIterBeg( cleanupList, curr ) {
        curr->rtn();
        p = alloc_mem( size, 1 );
        if( p != NULL ) {
            return( p );
        }
    } RingIterEnd( curr )
    CErr1( ERR_OUT_OF_MEMORY );
    CSuicide();
    return( NULL );
}


#if defined( TRMEM ) && !defined( USE_CG_MEMMGT ) && defined( _M_IX86 )
TRMEMAPI( CMemAlloc )
void *CMemAlloc( size_t size )
/****************************/
{
    return( _CMemAlloc( size, _trmem_guess_who() ) );
}
#endif

TRMEMAPI( CMemStrDup )
char *CMemStrDup( const char *str )
/*********************************/
{
    if( str != NULL ) {
        return( strcpy( _MemAlloc( strlen( str ) + 1, 7 ), str ) );
    }
    return( NULL );
}

TRMEMAPI( CMemFree )
void CMemFree( void *p )
/**********************/
{
    if( p != NULL ) {
        _doFree( p, 3 );
    }
}

TRMEMAPI( CMemFreePtr )
void CMemFreePtr( void *pp )
/**************************/
{
    void *p;

    p = *(void **)pp;
    if( p != NULL ) {
        _doFree( p, 4 );
        *(void **)pp = NULL;
    }
}

#ifdef DEVBUILD
void CMemDeferredFree( void *p )
/******************************/
{
    if( p != NULL ) {
        RingPush( &deferredFreeList, p );
    }
}
#endif

static void linkPerm( PERMPTR p, size_t amt )
{
    p->size = amt;
    p->amt_left = amt;
    RingPush( &permList, p );
}

#if defined( TRMEM ) && !defined( USE_CG_MEMMGT ) && defined( _M_IX86 )
static void addPerm( size_t size, pointer who )
#else
static void addPerm( size_t size )
#endif
{
    PERMPTR p;
    CLEANPTR curr;
    size_t amt;

    if( size > PERM_MAX_ALLOC ) {
        p = _MemAllocW( offsetof( perm_blk, mem ) + size );
        linkPerm( p, size );
        return;
    }
    size = PERM_MAX_ALLOC;
    for(;;) {
        amt = offsetof( perm_blk, mem ) + size;
        p = alloc_mem( amt, 2 );
        if( p != NULL ) {
            linkPerm( p, size );
            return;
        }
        RingIterBeg( cleanupList, curr ) {
            curr->rtn();
            p = alloc_mem( amt, 2 );
            if( p != NULL ) {
                linkPerm( p, size );
                return;
            }
        } RingIterEnd( curr )
        if( size == PERM_MIN_ALLOC )
            break;
        size >>= 1;
    }
}

static void *cutPerm( PERMPTR find, size_t size )
{
    void *p;

    if( size <= find->amt_left ) {
        p = &(find->mem[find->size - find->amt_left]);
        find->amt_left -= size;
        return( p );
    }
    return( NULL );
}

TRMEMAPI( CPermAlloc )
void *CPermAlloc( size_t size )
/*****************************/
{
    void *p;
    PERMPTR find;

    size = _RoundUp( size, sizeof( int ) );
    RingIterBeg( permList, find ) {
        p = cutPerm( find, size );
        if( p != NULL ) {
            return( p );
        }
    } RingIterEnd( find )
    _addPerm( size, 5 );
    RingIterBeg( permList, find ) {
        p = cutPerm( find, size );
        if( p != NULL ) {
            return( p );
        }
    } RingIterEnd( find )
    CErr1( ERR_OUT_OF_MEMORY );
    CSuicide();
    return( NULL );
}


TRMEMAPI( cmemInit )
static void cmemInit(           // INITIALIZATION
    INITFINI* defn )            // - definition
{
#ifdef TRMEM
    unsigned trmem_flags;
#endif

    /* unused parameters */ (void)defn;
#ifdef TRMEM
    trmem_flags = _TRMEM_ALLOC_SIZE_0 | _TRMEM_OUT_OF_MEMORY;
    if( CppGetEnv( "TRQUIET" ) == NULL ) {
        trmem_flags |= _TRMEM_CLOSE_CHECK_FREE;
    }
    trackerHdl = _trmem_open( malloc, free, NULL, NULL, NULL, printLine, trmem_flags );
#endif
#if defined( USE_CG_MEMMGT )
  #ifdef TRMEM
    BEMemInit( trackerHdl );
  #else
    BEMemInit( NULL );
  #endif
#endif
#ifdef DEVBUILD
    deferredFreeList = NULL;
#endif
    cleanupList = NULL;
    permList = NULL;
    _addPerm( 0, 6 );
}

static void cmemFini(           // COMPLETION
    INITFINI* defn )            // - definition
{
    /* unused parameters */ (void)defn;

    RingFree( &permList );
#ifdef DEVBUILD
    RingFree( &deferredFreeList );
#endif
#if defined( USE_CG_MEMMGT )
    BEMemFini();
#endif
#ifdef TRMEM
    if( TOGGLEDBG( dump_memory ) ) {
        _trmem_prt_list_ex( trackerHdl, 100 );
    }
    if( _trmem_close( trackerHdl ) != 0 && !CompFlags.compile_failed ) {
        // we can't print an error message since we have no more memory
  #if defined( __WATCOMC__ )
        EnterDebugger();
  #endif
    }
#endif
}


INITDEFN( memmgr, cmemInit, cmemFini )


static void cleanupInit(        // INITIALIZATION
    INITFINI* defn )            // - definition
{
    /* unused parameters */ (void)defn;

    cleanupList = NULL;
#if 0
    BEMemInit();
#endif
}


static void cleanupFini(        // COMPLETION
    INITFINI* defn )            // - definition
{
    /* unused parameters */ (void)defn;

    RingFree( &cleanupList );
}

INITDEFN( mem_cleanup, cleanupInit, cleanupFini )
