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
* Description:  Memory allocation routines for profiler.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>

#include "wio.h"
#include "common.h"
#include "dip.h"
#include "msg.h"
#include "memutil.h"
#ifdef TRMEM
#include "trmemcvr.h"
#endif


extern int WalkMem( void ) ;

#ifdef TRMEM
STATIC void profMemCheck( char *msg )
/***********************************/
{
    if( !WalkMem() ) {
        fatal( LIT( Assertion_Failed ), msg );
    }
}
#endif

void *ProfAlloc( size_t size )
/****************************/
{
    void    *mem;

    for( ;; ) {
#ifdef TRMEM
        profMemCheck( "ProfTryAlloc" );
        mem = TRMemAlloc( size );
#else
        mem = _MALLOC( size );
#endif
        if( mem != NULL ) 
            break;
        if( DIPMoreMem( size ) == DS_FAIL ) {
            break;
        }
    }

    if( mem == NULL ) {
        fatal( LIT( Memfull ) );
    }
    return( mem );
}

void ProfFree( void *ptr )
/************************/
{
#ifdef TRMEM
    profMemCheck( "ProfFree" );
    TRMemFree( ptr );
#else
    _FREE( ptr );
#endif
}

void *ProfRealloc( void *p, size_t new_size )
/*******************************************/
{
    void    *new;

    for( ;; ) {
#ifdef TRMEM
        profMemCheck( "ProfTryRealloc" );
        new = TRMemRealloc( p, new_size );
#else
        new = _REALLOC( p, new_size );
#endif
        if( new != NULL )
            break;
        if( DIPMoreMem( new_size ) == DS_FAIL ) {
            break;
        }
    }
    if( new == NULL ) {
        fatal( LIT( Memfull_Realloc  ));
    }
    return( new );
}

void *ProfCAlloc( size_t size )
/*****************************/
{
    void    *new;

    new = ProfAlloc( size );
    memset( new, 0, size );
    return( new );
}

#if 0
extern void *WndAlloc( unsigned size )
/************************************/
{
    return( ProfAlloc( size ) );
}

extern void *WndRealloc( void *chunk, unsigned size )
/***************************************************/
{
    return( ProfRealloc( chunk, size ) );
}

extern void WndFree( void *chunk )
/********************************/
{
    ProfFree( chunk );
}

extern void WndNoMemory( void )
/*****************************/
{
    fatal( LIT( Memfull  ));
}

extern void WndMemInit( void )
/****************************/
{
}

extern void WndMemFini( void )
/****************************/
{
}
#endif

void WPMemOpen( void )
/********************/
{
#ifdef TRMEM
    TRMemOpen();
    TRMemRedirect( STDOUT_FILENO );
#endif
}

void WPMemClose( void )
/*********************/
{
#ifdef TRMEM
    TRMemClose();
#endif
}

void WPMemPrtUsage( void )
/************************/
{
#ifdef TRMEM
    TRMemPrtUsage();
#endif
}
