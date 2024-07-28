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
#define FRLSIZE         (CFLOAT_SIZE + DOUBLE_DIGITS)
#define NEXT_BLOCK(x)   (((mem_blk *)(x))->next)

typedef struct {
    void        *next;
} mem_blk;

void    CFInit( cfhandle h )
/**************************/
{
    h->head = NULL;
}


cfloat  *CFAlloc( cfhandle h, size_t size )
/*****************************************/
{
    cfloat      *result;

    if( size <= DOUBLE_DIGITS ) {
        size = FRLSIZE;
        if( h->head != NULL ) {
            /*
             * unhook it and return the first element
             */
            result = h->head;
            h->head = NEXT_BLOCK( result );
        } else {
            result = h->alloc( size );
        }
    } else {
        size += CFLOAT_SIZE;
        result = h->alloc( size );
    }
    /*
     * Init result to 0 (zero)
     */
    result->sign = 0;
    result->exp = 1;
    result->len = 1;
    result->alloc = size;
    result->mant[0] = '0';
    result->mant[1] = NULLCHAR;
    return( result );
}


void    CFFree( cfhandle h, cfloat *f )
/*************************************/
{
    if( f->alloc == FRLSIZE ) {
        NEXT_BLOCK( f ) = h->head;
        h->head = f;
    } else {
        h->free( f );
    }
}

void    CFFini( cfhandle h )
/**************************/
{
    void    *ptr;

    while( (ptr = h->head) != NULL ) {
        h->head = NEXT_BLOCK( ptr );
        h->free( ptr );
    }
}

bool CFFrlFree( cfhandle h )
/**************************/
{
    if( h->head != NULL ) {
        CFFini( h );
        return( true );
    }
    return( false );
}
