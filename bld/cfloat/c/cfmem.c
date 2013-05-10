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
#include <stddef.h>
#include "watcom.h"
#include "cfloat.h"

#define DOUBLE_DIGITS   17
#define FRLSIZE         ( DOUBLE_DIGITS + offsetof( cfloat, mant ) + 1 )

typedef char *pointer;

typedef struct {
    pointer     head;
} mini_frl;

static cf_callbacks     cfRtns;
static mini_frl         cfFrlList;

static  void    miniFrlInit( mini_frl *frl_ptr ) {
/************************************************/

    frl_ptr->head = NULL;
}

static  pointer miniFrlAlloc( mini_frl *frl_ptr ) {
/*************************************************/

    pointer     ptr;

    if( frl_ptr->head != NULL ) {
        // unhook it and return the first element
        ptr = frl_ptr->head;
        frl_ptr->head = *((pointer *)ptr);
        return( ptr );
    }
    return( cfRtns.alloc( FRLSIZE ) );
}

static  void    miniFrlFree( mini_frl *frl_ptr, pointer ptr ) {
/****************&********************************************/

    *(pointer *)ptr = frl_ptr->head;
    frl_ptr->head = ptr;
}

static  void    miniFrlFini( mini_frl *frl_ptr ) {
/************************************************/

    pointer     ptr;
    pointer     next;

    ptr = frl_ptr->head;
    while( ptr != NULL ) {
        next = *(pointer *)ptr;
        cfRtns.free( ptr );
        ptr = next;
    }
    frl_ptr->head = NULL;
}

extern  void    CFInit( cf_callbacks *table ) {
/*********************************************/

    cfRtns = *table;
    miniFrlInit( &cfFrlList );
}


extern  cfloat  *CFAlloc( unsigned size ) {
/*****************************************/

    cfloat      *number;

    if( size <= DOUBLE_DIGITS ) {
        size = FRLSIZE;
        number = (cfloat *)miniFrlAlloc( &cfFrlList );
    } else {
        size += offsetof( cfloat, mant ) + 1 + 1;
        number = cfRtns.alloc( size );
    }
    number->sign = 0;
    number->exp = 1;
    number->len = 1;
    number->alloc = size;
    number->mant[0] = '0';
    number->mant[1] = '\0';
    return( number );
}


extern  void    CFFree( cfloat *f ) {
/***********************************/

    if( f->alloc == FRLSIZE ) {
        miniFrlFree( &cfFrlList, (pointer)f );
    } else {
        cfRtns.free( f );
    }
}


extern  cf_bool CFFrlFree( void ) {
/***************************/

    if( cfFrlList.head != NULL ) {
        miniFrlFini( &cfFrlList );
        return( CF_TRUE );
    }
    return( CF_FALSE );
}

extern  void    CFFini( void ) {
/************************/

    miniFrlFini( &cfFrlList );
}
