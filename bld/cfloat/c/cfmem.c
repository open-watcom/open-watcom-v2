/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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
#include <stddef.h>
#include "watcom.h"
#include "cfloati.h"

#define DOUBLE_DIGITS   17
#define FRLSIZE         ( DOUBLE_DIGITS + offsetof( cfloat, mant ) + 1 )
#define NEXT_BLOCK(x)   (((mem_blk *)(x))->next)

typedef char *pointer;

typedef struct {
    pointer     head;
} mini_frl;

typedef struct {
    pointer     next;
} mem_blk;

static cf_callbacks     cfRtns;
static mini_frl         cfFrlList;

static  void    miniFrlInit( void )
/*********************************/
{
    cfFrlList.head = NULL;
}

static  pointer miniFrlAlloc( void )
/**********************************/
{
    pointer     ptr;

    if( cfFrlList.head != NULL ) {
        /*
         * unhook it and return the first element
         */
        ptr = cfFrlList.head;
        cfFrlList.head = NEXT_BLOCK( ptr );
        return( ptr );
    }
    return( cfRtns.alloc( FRLSIZE ) );
}

static  void    miniFrlFree( pointer ptr )
/****************************************/
{
    NEXT_BLOCK( ptr ) = cfFrlList.head;
    cfFrlList.head = ptr;
}

static  void    miniFrlFini( void )
/*********************************/
{
    pointer     ptr;

    while( (ptr = cfFrlList.head) != NULL ) {
        cfFrlList.head = NEXT_BLOCK( ptr );
        cfRtns.free( ptr );
    }
}

void    CFInit( cf_callbacks *table )
/***********************************/
{
    cfRtns = *table;
    miniFrlInit();
}


cfloat  *CFAlloc( size_t size )
/*****************************/
{
    cfloat      *number;

    if( size <= DOUBLE_DIGITS ) {
        size = FRLSIZE;
        number = (cfloat *)miniFrlAlloc();
    } else {
        size += offsetof( cfloat, mant ) + 1;
        number = cfRtns.alloc( size );
    }
    number->sign = 0;
    number->exp = 1;
    number->len = 1;
    number->alloc = size;
    *number->mant = '0';
    *(number->mant + 1) = NULLCHAR;
    return( number );
}


void    CFFree( cfloat *f )
/*************************/
{
    if( f->alloc == FRLSIZE ) {
        miniFrlFree( (pointer)f );
    } else {
        cfRtns.free( f );
    }
}


bool CFFrlFree( void )
/********************/
{
    if( cfFrlList.head != NULL ) {
        miniFrlFini();
        return( true );
    }
    return( false );
}

void    CFFini( void )
/********************/
{
    miniFrlFini();
}
