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


#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "make.h"
#include "massert.h"
#include "mtypes.h"
#include "memory.h"
#include "mrcmsg.h"
#include "msg.h"


STATIC BOOLEAN largeNearSeg;    /* have we done a _nheapgrow() ? */

/*
 * We maintain a list of functions to call if memory gets too tight to do
 * a malloc.  A function returns RET_SUCCESS if it could dealloc some memory.
 */
STATIC struct scarce {
    struct scarce   *next;
    RET_T           (*func)( void );
} *scarceHead;


#if defined(TRACK)
    STATIC int trmemCode;
#endif

#ifdef  TRACK
#   include <malloc.h>
#   include <io.h>
#   include <sys/types.h>
#   include <sys/stat.h>
#   include <fcntl.h>
#   include "trmem.h"

    STATIC _trmem_hdl Handle;

    STATIC int trkfile;     /* file handle we'll write() to */

    STATIC void printLine( void *h, const char *buf, unsigned size )
    {
        h = h;
        write( trkfile, buf, size );
        write( trkfile, "\n", 1 );
        if (!(trmemCode & TRMEM_DO_NOT_PRINT)) {
             write( STDOUT_FILENO, buf, size );
             write( STDOUT_FILENO, "\n", 1 );
        }
    }

    STATIC void MemCheck( void )
    /**************************/
    {
        static busy = FALSE;    /* protect against recursion thru PrtMsg */

        if( !busy ) {
            busy = TRUE;
#ifdef USE_FAR
            switch( _nheapchk() ) {
            case _HEAPOK:
            case _HEAPEMPTY:
                break;
            case _HEAPBADBEGIN:
                PrtMsg( FTL| HEAP_IS_DAMAGED, "NEAR" );
            case _HEAPBADNODE:
                PrtMsg( FTL| BAD_NODE_IN_HEAP, "NEAR" );
            }
            switch( _fheapchk() ) {
            case _HEAPOK:
            case _HEAPEMPTY:
                break;
            case _HEAPBADBEGIN:
                PrtMsg( FTL| HEAP_IS_DAMAGED, "FAR" );
            case _HEAPBADNODE:
                PrtMsg( FTL| BAD_NODE_IN_HEAP, "FAR" );
            }
#else
            switch( _heapchk() ) {
            case _HEAPOK:
            case _HEAPEMPTY:
                break;
            case _HEAPBADBEGIN:
                PrtMsg( FTL| HEAP_IS_DAMAGED, "" );
            case _HEAPBADNODE:
                PrtMsg( FTL| BAD_NODE_IN_HEAP, "" );
            }
#endif
            busy = FALSE;
        }
    }
#endif  /* TRACK */


extern void IfMemScarce( RET_T (*func)( void ) )
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
 * returns: TRUE if a scarce routine managed to deallocate memory.
 */
{
    BOOLEAN did;
    struct scarce *cur;

    did = FALSE;
    cur = scarceHead;
    while( cur != NULL && !did ) {
        did = (cur->func)() == RET_SUCCESS;
        cur = cur->next;
    }

    return( did ? RET_SUCCESS : RET_ERROR );
}


extern void MemFini( void )
/**************************
 * post:    As much memory as possible is freed.
 */
{
#if defined(TRACK)
    char* trmemCodeStr;
#endif
#if defined(DEVELOPMENT) || defined(TRACK)
    struct scarce *cur;

    while( tryScarce() == RET_SUCCESS ) /* call all scarce routines */
        ;

    while( scarceHead != NULL ) {   /* free all scarce trackers */
        cur = scarceHead;
        scarceHead = scarceHead->next;
        FreeSafe( cur );
    }

#ifdef DEVELOPMENT
    PutEnvFini();
#endif
#ifdef TRACK
    if( ! Glob.erroryet ) {
        trmemCodeStr = getenv(TRMEM_ENV_VAR);
        if (trmemCodeStr == NULL) {
            trmemCode = 0;
        } else {
            trmemCode = atoi(trmemCodeStr);
        }
        if (TRMEM_IGNORE_ERROR & trmemCode) {
            _trmem_prt_list( Handle );
        } else {
            if (_trmem_prt_list ( Handle ) != 0) {
                PrtMsg( ERR| ERROR_TRMEM );
            }
        }
    }

    _trmem_close( Handle );
    MemCheck();
    close( trkfile );
#endif
#endif
}


STATIC void memGrow( void )
/*************************/
{
#ifdef USE_FAR
    _nheapgrow();
    _fheapgrow();
#else
#ifndef __NT__
    _heapgrow();
#endif
#endif
    largeNearSeg = TRUE;
}


extern void MemInit( void )
/*************************/
{
    largeNearSeg = FALSE;
    memGrow();
#ifdef TRACK
    Handle = _trmem_open( malloc
                        , free
                        , _TRMEM_NO_REALLOC
                        , _expand
                        , NULL
                        , printLine
                        , _TRMEM_CLOSE_CHECK_FREE );
    if( Handle == NULL ) PrtMsg( FTL| UNABLE_TO_TRACK );
    trkfile = open( "mem.trk", O_WRONLY | O_CREAT | O_TRUNC, 0 );
#endif
}


#ifdef TRACK
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

    for(;;) {
#ifdef TRACK
        ptr = _trmem_alloc( size, ra, Handle );
#else
        ptr = malloc( size );
#endif
        if( ptr != NULL ) break;
        if( tryScarce() != RET_SUCCESS ) break;
    }
    return( ptr );
}


extern void *MallocUnSafe( size_t size )
/**************************************/
{
    void *ptr;
#ifdef TRACK
    ptr = doAlloc( size, _trmem_guess_who() );
#else
    ptr = doAlloc( size );
#endif
    return( ptr );
}


extern void *MallocSafe( size_t size )
/*************************************
 * post:    A scarce routine may be called
 * returns: A pointer to a block of memory of size size.
 * aborts:  If not enough memory to satisfy request.
 */
{
    void *ptr;

#ifdef TRACK
    ptr = doAlloc( size, _trmem_guess_who() );
#else
    ptr = doAlloc( size );
#endif
    if( ptr != NULL ) {
        return( ptr );
    }

    PrtMsg( FTL| OUT_OF_MEMORY );
    return( NULL );
}


extern void *CallocSafe( size_t size )
/*************************************
 * post:    A scarce routine may be called
 * returns: A pointer to a block of memory of size size
 * aborts:  If not enough memory to satisfy request
 */
{
    void *ptr;

#ifdef TRACK        /* so we can track ret address */
    ptr = doAlloc( size, _trmem_guess_who() );

    if( ptr == NULL ) {
        PrtMsg( FTL| OUT_OF_MEMORY );
    }
#else
    ptr = MallocSafe( size );
#endif

    memset( ptr, NULLCHAR, size );

    return( ptr );
}


extern void FreeSafe( void *ptr )
/********************************
 * post:    The block pointed to by ptr is freed if it was allocated by
 *          MallocSafe.
 * remarks: Guaranteed to work in low memory situations (scarce).
 */
{
#ifdef TRACK
    _trmem_free( ptr, _trmem_guess_who(), Handle );
#else
    free( ptr );
#endif
}


extern char *StrDupSafe( const char *str )
/*****************************************
 * returns: Pointer to a duplicate of str in a new block of memory.
 * aborts:  If not enough memory to make a duplicate.
 */
{
    size_t  len;
    char    *p;

    len = strlen( str ) + 1;

#ifdef TRACK
    p = doAlloc( len, _trmem_guess_who() );
    if( p == NULL ) {
        PrtMsg( FTL| OUT_OF_MEMORY );
    }
#else
    p = MallocSafe( len );
#endif

    memcpy( p, str, len );
    return( p );
}


extern void MemShrink( void )
/***************************/
{
#ifdef USE_FAR
    _nheapshrink();
    _fheapshrink();
#else
    _heapshrink();
#endif
    largeNearSeg = FALSE;
}


extern void MemDecreaseSize( void *ptr, size_t new_size )
/*******************************************************/
{
#ifdef TRACK
    _trmem_expand( ptr, new_size, _trmem_guess_who(), Handle );
#else
    _expand( ptr, new_size );
#endif
}


#ifdef USE_FAR

extern void FAR *FarMaybeMalloc( size_t size )
/********************************************/
{
    if( !largeNearSeg ) {
        memGrow();
    }
    return( _fmalloc( size ) );
}


extern void FAR *FarMalloc( size_t size )
/***************************************/
{
    void FAR *p;

    p = FarMaybeMalloc( size );
    if( p == NULL ) {
        PrtMsg( FTL| OUT_OF_MEMORY );
    }

    return( p );
}


extern void FarFree( void FAR *p )
/********************************/
{
    _ffree( p );
}
#endif /* USE_FAR */
