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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "global.h"
#include "error.h"
#include "mem.h"

#ifdef TRMEM
#include "trmemcvr.h"
#define malloc  TRMemAlloc
#define realloc TRMemRealloc
#define free    TRMemFree
#endif

void    InitMem( void ) {
/************************/
#ifdef TRMEM
    TRMemOpen();
    TRMemRedirect( stdout->_handle );
#endif
}

void    FiniMem( void ) {
/************************/
#ifdef TRMEM
    TRMemPrtList();
    TRMemClose();
#endif
}

void    *MemMalloc( int size ) {
/******************************/

    void        *tmp;

    tmp = malloc( size );
#ifdef MEM_TRACK
    printf( "ALLOC %lX          size %d\n", tmp, size );
#endif
    if( !tmp ) {
        Error( OUT_OF_MEMORY );
    };
    return( tmp );
}


void    *MemRealloc( void *old, int size ) {
/******************************************/

    void        *tmp;

    tmp = realloc( old, size );
#ifdef MEM_TRACK
    printf( "FREE %lX\n", old );
    printf( "REALLOC %lX                size %d\n", tmp, size );
#endif
    if( !tmp ) {
        Error( OUT_OF_MEMORY );
    };
    return( tmp );
}


void    MemFree( void *blck ) {
/*****************************/

     if( blck ) {
         free( blck );
     }
#ifdef MEM_TRACK
     printf( "FREE %lX\n", blck );
#endif
}


char    *MemStrDup( const char *src ) {
/*************************************/

    char                *dst;

    if( !src ) {
        return( NULL );
    }

    dst = MemMalloc( strlen( src ) + 1 );
    strcpy( dst, src );
    return( dst );
}
