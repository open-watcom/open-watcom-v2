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
* Description:  Memory management for wmake.
*
****************************************************************************/


#include "make.h"
#include "mmemory.h"
#include "mrcmsg.h"
#include "msg.h"
#if defined( __WATCOMC__ )
    #include <malloc.h>         /* necessary for heap checking functions */
#endif
#include "memfuncs.h"
#ifdef TRMEM
    #include <sys/types.h>
    #include "trmem.h"
#endif

#include "clibext.h"


#if defined( TRMEM ) && defined( _M_IX86 ) && ( __WATCOMC__ > 1290 )
#define _XSTR(s)    # s
#define TRMEMAPI(x) _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
#endif

#ifdef USE_FAR
STATIC bool     largeNearSeg;    /* have we done a _nheapgrow() ? */
#endif

#ifdef USE_SCARCE

/*
 * A word on "scarce memory". We maintain a list of functions to call if
 * memory gets too tight to do a malloc. A function returns RET_SUCCESS if
 * it could dealloc some memory. Several wmake modules register "scarce"
 * function that will attempt to free up memory that's not immediately
 * needed.
 * Scarce memory management is naturally next to useless on virtual memory
 * systems where malloc() isn't likely to fail, hence we only use it on
 * memory constrained platforms (ie. 16-bit DOS).
 */

STATIC struct scarce {
    struct scarce   *next;
    bool            (*func)( void );
} *scarceHead;

#endif

#ifdef TRMEM

#define TRMEM_ENV_VAR   "TRMEM_CODE"

enum {
    TRMEM_DO_NOT_PRINT  = 0x0001,
    TRMEM_IGNORE_ERROR  = 0x0002
};

STATIC _trmem_hdl   TrHdl = _TRMEM_HDL_NONE;
STATIC int          trmemCode;
STATIC FILE         *TrFile = NULL;

STATIC void printLine( void *h, const char *buf, size_t size )
/************************************************************/
{
    /* unused parameters */ (void)h; (void)size;

    if( TrFile == NULL ) {
        TrFile = fopen( "mem.trk", "w" );
    }
    if( TrFile != NULL ) {
        fprintf( TrFile, "%s\n", buf );
    }
    if( (trmemCode & TRMEM_DO_NOT_PRINT) == 0 ) {
        fprintf( stdout, "%s\n", buf );
    }
}

STATIC void MemCheck( void )
/**************************/
{
    static bool     busy = false;   /* protect against recursion thru PrtMsg */

#if defined( __WATCOMC__ )
    if( !busy ) {
        busy = true;
  #ifdef USE_FAR
        switch( _nheapchk() ) {
        case _HEAPOK:
        case _HEAPEMPTY:
            break;
        case _HEAPBADBEGIN:
            PrtMsg( FTL | PRNTSTR, "Near heap is damaged!" );
            ExitFatal();
            // never return
        case _HEAPBADNODE:
            PrtMsg( FTL | PRNTSTR, "Bad node in Near heap!" );
            ExitFatal();
            // never return
        }
        switch( _fheapchk() ) {
        case _HEAPOK:
        case _HEAPEMPTY:
            break;
        case _HEAPBADBEGIN:
            PrtMsg( FTL | PRNTSTR, "Far heap is damaged!" );
            ExitFatal();
            // never return
        case _HEAPBADNODE:
            PrtMsg( FTL | PRNTSTR, "Bad node in Far heap!" );
            ExitFatal();
            // never return
        }
  #else
        switch( _heapchk() ) {
        case _HEAPOK:
        case _HEAPEMPTY:
            break;
        case _HEAPBADBEGIN:
            PrtMsg( FTL | PRNTSTR, "Heap is damaged!" );
            ExitFatal();
            // never return
        case _HEAPBADNODE:
            PrtMsg( FTL | PRNTSTR, "Bad node in Heap!" );
            ExitFatal();
            // never return
        }
  #endif
        busy = false;
    }
#endif
}

#endif  /* TRMEM */

#ifdef USE_SCARCE

void IfMemScarce( bool (*func)( void ) )
/***********************************************
 * post:    function registered in scarce list
 * remarks: The function *func must return SUCCESS if it manages to deallocate
 *          some memory.  Otherwise ERROR.  Be careful to make sure that
 *          *func can do its work without being too obtrusive - make no
 *          assumptions about what routines can be called.  The only
 *          guaranteed safe routine is MemFree().
 */
{
    struct scarce *new;

    assert( func != NULL );

    new = MemAllocSafe( sizeof( *new ) );
    new->func = func;
    new->next = scarceHead;
    scarceHead = new;
}

STATIC bool tryScarce( void )
/****************************
 * returns: true if a scarce routine managed to deallocate memory.
 */
{
    bool            did;
    struct scarce   *cur;

    did = false;
    cur = scarceHead;
    while( cur != NULL && !did ) {
        did = (cur->func)();
        cur = cur->next;
    }

    return( did );
}

#endif /* USE_SCARCE */

void MemFini( void )
/**************************
 * post:    As much memory as possible is freed.
 */
{
#ifdef TRMEM
    char    *trmemCodeStr;
#endif
#if defined( DEVELOPMENT ) || defined( TRMEM )

  #ifdef USE_SCARCE
    struct scarce *cur;

    while( tryScarce() ) /* call all scarce routines */
        ;

    while( scarceHead != NULL ) {   /* free all scarce trackers */
        cur = scarceHead;
        scarceHead = scarceHead->next;
        MemFree( cur );
    }
  #endif

  #ifdef DEVELOPMENT
    SetEnvFini();
  #endif
  #ifdef TRMEM
    if( !Glob.erroryet ) { /* No error diagnostics yet? */
        trmemCodeStr = getenv( TRMEM_ENV_VAR );
        if( trmemCodeStr == NULL ) {
            trmemCode = 0;
        } else {
            trmemCode = atoi( trmemCodeStr );
        }
        if( trmemCode & TRMEM_IGNORE_ERROR ) {
            _trmem_prt_list( TrHdl );
        } else {
            if( _trmem_prt_list( TrHdl ) > 0 ) {
                PrtMsg( ERR | ERROR_TRMEM );
            }
        }

        _trmem_close( TrHdl ); /* Report any memory errors. */
    }
    MemCheck();
    if( TrFile != NULL ) {
        fclose( TrFile );
        TrFile = NULL;
    }
  #endif
#endif
}


STATIC void memGrow( void )
/*************************/
{
#if defined( __WATCOMC__ )
  #ifdef USE_FAR
    _nheapgrow();
    _fheapgrow();
    largeNearSeg = true;
  #elif !defined( __NT__ ) && !defined( __UNIX__ )
    _heapgrow();
  #endif
#endif
}


void MemInit( void )
/******************/
{
    memGrow();
#ifdef TRMEM
    TrHdl = _trmem_open( malloc, free, _TRMEM_NO_REALLOC, strdup,
                          NULL, printLine, _TRMEM_CLOSE_CHECK_FREE );
    if( TrHdl == _TRMEM_HDL_NONE ) {
        PrtMsg( FTL | PRNTSTR, "Unable to track memory!" );
        ExitFatal();
        // never return
    }
#endif
}

static void *check_nomem( void *ptr )
{
    if( ptr == NULL ) {
        PrtMsg( FTL | OUT_OF_MEMORY );
        ExitFatal();
        // never return
    }
    return( ptr );
}

#ifdef USE_SCARCE

#ifdef TRMEM
STATIC void *doAlloc( size_t size, _trmem_who who )
#else
STATIC void *doAlloc( size_t size )
#endif
/******************************************************
 * post:    A scarce routine may have been called.
 * returns: A pointer to a block of memory of size size, or NULL if no
 *          such block exists.
 */
{
    void   *ptr;

    for( ;; ) {
  #ifdef TRMEM
        ptr = _trmem_alloc( size, who, TrHdl );
  #else
        ptr = malloc( size );
  #endif
        if( ptr != NULL || !tryScarce() ) {
            break;
        }
    }
    return( ptr );
}

#endif /* USE_SCARCE */

TRMEMAPI( MemAlloc )
void *MemAlloc( size_t size )
/***************************/
{
#ifdef TRMEM
  #ifdef USE_SCARCE
    return( doAlloc( size, _TRMEM_WHO( 1 ) ) );
  #else
    return( _trmem_alloc( size, _TRMEM_WHO( 1 ), TrHdl ) );
  #endif
#else
  #ifdef USE_SCARCE
    return( doAlloc( size ) );
  #else
    return( malloc( size ) );
  #endif
#endif
}

TRMEMAPI( MemAllocSafe )
void *MemAllocSafe( size_t size )
/******************************
 * post:    A scarce routine may be called
 * returns: A pointer to a block of memory of size size.
 * aborts:  If not enough memory to satisfy request.
 */
{
#ifdef TRMEM
  #ifdef USE_SCARCE
    return( check_nomem( doAlloc( size, _TRMEM_WHO( 2 ) ) ) );
  #else
    return( check_nomem( _trmem_alloc( size, _TRMEM_WHO( 2 ), TrHdl ) ) );
  #endif
#else
  #ifdef USE_SCARCE
    return( check_nomem( doAlloc( size ) ) );
  #else
    return( check_nomem( malloc( size ) ) );
  #endif
#endif
}

TRMEMAPI( MemCAllocSafe )
void *MemCAllocSafe( size_t size )
/*********************************
 * post:    A scarce routine may be called
 * returns: A pointer to a block of memory of size size
 * aborts:  If not enough memory to satisfy request
 */
{
    void    *ptr;

#ifdef TRMEM
  #ifdef USE_SCARCE
    ptr = check_nomem( doAlloc( size, _TRMEM_WHO( 3 ) ) );
  #else
    ptr = check_nomem( _trmem_alloc( size, _TRMEM_WHO( 3 ), TrHdl ) );
  #endif
#else
  #ifdef USE_SCARCE
    ptr = check_nomem( doAlloc( size ) );
  #else
    ptr = check_nomem( malloc( size ) );
  #endif
#endif
    return( memset( ptr, NULLCHAR, size ) );
}

#ifdef USE_SCARCE

#ifdef TRMEM
STATIC char *doStrdup( const char *str, _trmem_who who )
#else
STATIC char *doStrdup( const char *str )
#endif
/******************************************************
 * post:    A scarce routine may have been called.
 * returns: A pointer to a block of memory of size size, or NULL if no
 *          such block exists.
 */
{
    void   *ptr;

    for( ;; ) {
  #ifdef TRMEM
        ptr = _trmem_strdup( str, who, TrHdl );
  #else
        ptr = strdup( str );
  #endif
        if( ptr != NULL || !tryScarce() ) {
            break;
        }
    }
    return( ptr );
}

#endif /* USE_SCARCE */

TRMEMAPI( MemStrdupSafe )
char *MemStrdupSafe( const char *str )
/**********************************
 * returns: Pointer to a duplicate of str in a new block of memory.
 * aborts:  If not enough memory to make a duplicate.
 */
{
#ifdef TRMEM
  #ifdef USE_SCARCE
    return( check_nomem( doStrdup( str, _TRMEM_WHO( 4 ) ) ) );
  #else
    return( _trmem_strdup( str, _TRMEM_WHO( 4 ), TrHdl ) );
  #endif
#else
  #ifdef USE_SCARCE
    return( check_nomem( doStrdup( str ) ) );
  #else
    return( strdup( str ) );
  #endif
#endif
}

TRMEMAPI( MemFree )
void MemFree( void *ptr )
/*************************
 * post:    The block pointed to by ptr is freed if it was allocated by
 *          MemAlloc...
 * remarks: Guaranteed to work in low memory situations (scarce).
 */
{
#ifdef TRMEM
    _trmem_free( ptr, _TRMEM_WHO( 5 ), TrHdl );
#else
    free( ptr );
#endif
}


TRMEMAPI( CharToStringSafe )
char *CharToStringSafe( char c )
/****************************
 * returns: Pointer to a string with one character in a new block of memory.
 * aborts:  If not enough memory to make a string.
 */
{
    char    *p;

#ifdef TRMEM
  #ifdef USE_SCARCE
    p = check_nomem( doAlloc( 2, _TRMEM_WHO( 6 ) ) );
  #else
    p = check_nomem( _trmem_alloc( 2, _TRMEM_WHO( 6 ), TrHdl ) );
  #endif
#else
  #ifdef USE_SCARCE
    p = check_nomem( doAlloc( 2 ) );
  #else
    p = check_nomem( malloc( 2 ) );
  #endif
#endif
    p[0] = c;
    p[1] = NULLCHAR;
    return( p );
}


void MemShrink( void )
/********************/
{
#if defined( __WATCOMC__ )
  #ifdef USE_FAR
    _nheapshrink();
    _fheapshrink();
    largeNearSeg = false;
  #elif !defined( __UNIX__ )
    _heapshrink();
  #endif
#endif
}


#ifdef USE_FAR

void FAR *FarMemAlloc( size_t size )
/**********************************/
{
    if( !largeNearSeg ) {
        memGrow();
    }
    return( _fmalloc( size ) );
}


void FAR *FarMemAllocSafe( size_t size )
/************************************/
{
    void FAR *ptr;

    ptr = FarMemAlloc( size );
    if( ptr == NULL ) {
        PrtMsg( FTL | OUT_OF_MEMORY );
        ExitFatal();
        // never return
    }
    return( ptr );
}


void FarMemFree( void FAR *p )
/*****************************/
{
    _ffree( p );
}

#endif /* USE_FAR */
