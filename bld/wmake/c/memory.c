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
* Description:  Memory management for wmake.
*
****************************************************************************/


#include "make.h"
#include "mmemory.h"
#include "mrcmsg.h"
#include "msg.h"
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
    #include <malloc.h>
#endif

#include "clibext.h"


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
    RET_T           (*func)( void );
} *scarceHead;

#endif

#ifdef TRMEM

#include <sys/types.h>
#include "wio.h"
#include "trmem.h"
#include "mposix.h"

#include "clibext.h"

#define TRMEM_ENV_VAR   "TRMEM_CODE"

enum {
    TRMEM_DO_NOT_PRINT  = 0x0001,
    TRMEM_IGNORE_ERROR  = 0x0002
};

STATIC _trmem_hdl   Handle;
STATIC int          trmemCode;
STATIC int          trkfile = -1;     /* file handle we'll posix_write() to */

STATIC void printLine( void *h, const char *buf, size_t size )
/************************************************************/
{
    h = h;
    if( trkfile == -1 ) {
        trkfile = open( "mem.trk", O_WRONLY | O_CREAT | O_TRUNC, PMODE_RW );
    }
    if( trkfile != -1 ) {
        posix_write( trkfile, buf, size );
        posix_write( trkfile, "\n", 1 );
    }
    if( (trmemCode & TRMEM_DO_NOT_PRINT) == 0 ) {
         posix_write( STDOUT_FILENO, buf, size );
         posix_write( STDOUT_FILENO, "\n", 1 );
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
        case _HEAPBADNODE:
            PrtMsg( FTL | PRNTSTR, "Bad node in Near heap!" );
            ExitFatal();
        }
        switch( _fheapchk() ) {
        case _HEAPOK:
        case _HEAPEMPTY:
            break;
        case _HEAPBADBEGIN:
            PrtMsg( FTL | PRNTSTR, "Far heap is damaged!" );
            ExitFatal();
        case _HEAPBADNODE:
            PrtMsg( FTL | PRNTSTR, "Bad node in Far heap!" );
            ExitFatal();
        }
#else
        switch( _heapchk() ) {
        case _HEAPOK:
        case _HEAPEMPTY:
            break;
        case _HEAPBADBEGIN:
            PrtMsg( FTL | PRNTSTR, "Heap is damaged!" );
            ExitFatal();
        case _HEAPBADNODE:
            PrtMsg( FTL | PRNTSTR, "Bad node in Heap!" );
            ExitFatal();
        }
#endif
        busy = false;
    }
#endif
}
#endif  /* TRMEM */

#ifdef USE_SCARCE
void IfMemScarce( RET_T (*func)( void ) )
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


STATIC RET_T tryScarce( void )
/*****************************
 * returns: true if a scarce routine managed to deallocate memory.
 */
{
    bool            did;
    struct scarce   *cur;

    did = false;
    cur = scarceHead;
    while( cur != NULL && !did ) {
        did = (cur->func)() == RET_SUCCESS;
        cur = cur->next;
    }

    return( did ? RET_SUCCESS : RET_ERROR );
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

    while( tryScarce() == RET_SUCCESS ) /* call all scarce routines */
        ;

    while( scarceHead != NULL ) {   /* free all scarce trackers */
        cur = scarceHead;
        scarceHead = scarceHead->next;
        FreeSafe( cur );
    }
#endif

#ifdef DEVELOPMENT
    PutEnvFini();
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
            if( _trmem_prt_list ( Handle ) != 0 ) {
                PrtMsg( ERR | ERROR_TRMEM );
            }
        }

        _trmem_close( Handle ); /* Report any memory errors. */
    }
    MemCheck();
    if( trkfile != -1 ) {
        close( trkfile );
        trkfile = -1;
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
        if( tryScarce() != RET_SUCCESS ) {
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
    }
#else
    p = MallocSafe( len );
#endif

    memcpy( p, str, len );
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
    }

    return( p );
}


void FarFreeSafe( void FAR *p )
/*****************************/
{
    _ffree( p );
}
#endif /* USE_FAR */
