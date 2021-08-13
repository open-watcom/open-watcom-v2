/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#ifdef TRMEM
#include <sys/types.h>
#include "trmem.h"
#endif
#include "wresmem.h"


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

STATIC _trmem_hdl   Handle;
STATIC int          trmemCode;
STATIC FILE         *trkfile = NULL;

STATIC void printLine( void *h, const char *buf, size_t size )
/************************************************************/
{
    /* unused parameters */ (void)h; (void)size;

    if( trkfile == NULL ) {
        trkfile = fopen( "mem.trk", "w" );
    }
    if( trkfile != NULL ) {
        fprintf( trkfile, "%s\n", buf );
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
 *          guaranteed safe routine is FreeSafe().
 */
{
    struct scarce *new;

    assert( func != NULL );

    new = MallocSafe( sizeof( *new ) );
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
#endif

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
        FreeSafe( cur );
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
        if( TRMEM_IGNORE_ERROR & trmemCode ) {
            _trmem_prt_list( Handle );
        } else {
            if( _trmem_prt_list( Handle ) > 0 ) {
                PrtMsg( ERR | ERROR_TRMEM );
            }
        }

        _trmem_close( Handle ); /* Report any memory errors. */
    }
    MemCheck();
    if( trkfile != NULL ) {
        fclose( trkfile );
        trkfile = NULL;
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
#else
#if !defined( __NT__ ) && !defined( __UNIX__ )
    _heapgrow();
#endif
#endif
#endif
}


void MemInit( void )
/*************************/
{
    memGrow();
#ifdef TRMEM
    Handle = _trmem_open( malloc, free, _TRMEM_NO_REALLOC, NULL,
                          NULL, printLine, _TRMEM_CLOSE_CHECK_FREE );
    if( Handle == NULL ) {
        PrtMsg( FTL | PRNTSTR, "Unable to track memory!" );
        ExitFatal();
        // never return
    }
#endif
}


#ifdef TRMEM
STATIC void *doAlloc( size_t size, void (* ra)(void) )
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

#ifdef USE_SCARCE

    for( ;; ) {
#ifdef TRMEM
        ptr = _trmem_alloc( size, ra, Handle );
#else
        ptr = malloc( size );
#endif
        if( ptr != NULL ) {
            break;
        }
        if( !tryScarce() ) {
            break;
        }
    }

#else

#ifdef TRMEM
    ptr = _trmem_alloc( size, ra, Handle );
#else
    ptr = malloc( size );
#endif

#endif
    return( ptr );
}

#ifndef BOOTSTRAP
void *wres_alloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), Handle ) );
#else
    return( malloc( size ) );
#endif
}
#endif


void *MallocUnSafe( size_t size )
/**************************************/
{
    void *ptr;
#ifdef TRMEM
    ptr = doAlloc( size, _trmem_guess_who() );
#else
    ptr = doAlloc( size );
#endif
    return( ptr );
}


void *MallocSafe( size_t size )
/*************************************
 * post:    A scarce routine may be called
 * returns: A pointer to a block of memory of size size.
 * aborts:  If not enough memory to satisfy request.
 */
{
    void    *ptr;

#ifdef TRMEM
    ptr = doAlloc( size, _trmem_guess_who() );
#else
    ptr = doAlloc( size );
#endif
    if( ptr == NULL ) {
        PrtMsg( FTL | OUT_OF_MEMORY );
        ExitFatal();
        // never return
    }
    return( ptr );
}


void *CallocSafe( size_t size )
/*************************************
 * post:    A scarce routine may be called
 * returns: A pointer to a block of memory of size size
 * aborts:  If not enough memory to satisfy request
 */
{
    void    *ptr;

#ifdef TRMEM        /* so we can track ret address */
    ptr = doAlloc( size, _trmem_guess_who() );

    if( ptr == NULL ) {
        PrtMsg( FTL | OUT_OF_MEMORY );
        ExitFatal();
        // never return
    }
#else
    ptr = MallocSafe( size );
#endif

    memset( ptr, NULLCHAR, size );

    return( ptr );
}


void FreeSafe( void *ptr )
/********************************
 * post:    The block pointed to by ptr is freed if it was allocated by
 *          MallocSafe.
 * remarks: Guaranteed to work in low memory situations (scarce).
 */
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), Handle );
#else
    free( ptr );
#endif
}

#ifndef BOOTSTRAP
void wres_free( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), Handle );
#else
    free( ptr );
#endif
}
#endif

char *StrDupSafe( const char *str )
/*****************************************
 * returns: Pointer to a duplicate of str in a new block of memory.
 * aborts:  If not enough memory to make a duplicate.
 */
{
    size_t  len;
    char    *p;

    len = strlen( str ) + 1;

#ifdef TRMEM
    p = doAlloc( len, _trmem_guess_who() );
    if( p == NULL ) {
        PrtMsg( FTL | OUT_OF_MEMORY );
        ExitFatal();
        // never return
    }
#else
    p = MallocSafe( len );
#endif

    memcpy( p, str, len );
    return( p );
}


char *CharToStrSafe( char c )
/*****************************************
 * returns: Pointer to a string with one character in a new block of memory.
 * aborts:  If not enough memory to make a string.
 */
{
    char    *p;

#ifdef TRMEM
    p = doAlloc( 2, _trmem_guess_who() );
    if( p == NULL ) {
        PrtMsg( FTL | OUT_OF_MEMORY );
        ExitFatal();
        // never return
    }
    p[0] = c;
    p[1] = '\0';
#else
    p = MallocSafe( 2 );
    if( p != NULL ) {
        p[0] = c;
        p[1] = '\0';
    }
#endif
    return( p );
}


void MemShrink( void )
/***************************/
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

void FAR *FarMallocUnSafe( size_t size )
/**************************************/
{
    if( !largeNearSeg ) {
        memGrow();
    }
    return( _fmalloc( size ) );
}


void FAR *FarMallocSafe( size_t size )
/************************************/
{
    void FAR *p;

    p = FarMallocUnSafe( size );
    if( p == NULL ) {
        PrtMsg( FTL | OUT_OF_MEMORY );
        ExitFatal();
        // never return
    }

    return( p );
}


void FarFreeSafe( void FAR *p )
/*****************************/
{
    _ffree( p );
}
#endif /* USE_FAR */
