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


/*
    Implements an array abstract data type.  The arrays have no upper bound
    on their size.  Best performance if elements defined in order.
*/

#include <stdlib.h>
#include <string.h>
#include "womp.h"
#include "memutil.h"
#include "array.h"
#include "myassert.h"

#define BLOCK_SIZE  64      /* number of elements in a block */

typedef struct array_blk array_blk;
struct array_blk {
    array_blk   *next;
    char        data[ 1 ];  /* variable size */
};

/*
    The rover code is not very efficient with the block size of 64.  If the
    block size needs to be lowered (because of memory constraints perhaps)
    then USE_ROVER may be appropriate.
*/

struct array_hdr {
    array_blk   *block;     /* block of first element (elm num 0) */
#ifdef USE_ROVER
    array_blk   *rov_blk;   /* rover block */
    size_t      rov_num;    /* element number of first element in rover */
#endif
    size_t      elm_size;   /* size of each element */
    size_t      num_elms;   /* number of elements in the array */
    void        *def;       /* elm_size default data */
};

extern void ArrInit( void ) {
/*************************/
}

extern void ArrFini( void ) {
/*************************/
}

extern array_hdr *ArrCreate( size_t elm_size, void *def ) {
/*******************************************************/
    array_hdr   *new;

/**/myassert( elm_size > 0 );
    new = MemAlloc( sizeof( *new ) + elm_size );
    new->block = NULL;
#ifdef USE_ROVER
    new->rov_blk = NULL;
    new->rov_num = ~(size_t)0;
#endif
    new->elm_size = elm_size;
    new->num_elms = 0;
    new->def = def;
    return( new );
}

extern void ArrDestroy( array_hdr *arr ) {
/**************************************/
    array_blk   *cur;
    array_blk   *next;

/**/myassert( arr != NULL );
    cur = arr->block;
    while( cur != NULL ) {
        next = cur->next;
        MemFree( cur );
        cur = next;
    }
    MemFree( arr );
}

STATIC array_blk *newBlock( array_hdr *arr ) {

    array_blk   *new;
    char        *elm_ptr;
    char        *stop_ptr;
    size_t      elm_size;
    void        *def;

/**/myassert( arr != NULL );
    new = MemAlloc( sizeof( array_blk ) + ( arr->elm_size * BLOCK_SIZE ) );
    new->next = NULL;
    def = arr->def;
    if( def != NULL ) {
        elm_ptr = new->data;
        elm_size = arr->elm_size;
        stop_ptr = elm_ptr + elm_size * BLOCK_SIZE;
        while( elm_ptr < stop_ptr ) {
            memcpy( elm_ptr, def, elm_size );
            elm_ptr += elm_size;
        }
    }
    return( new );
}

void *ArrNewElm( array_hdr *arr, size_t elm_num ) {
/***********************************************/
    array_blk   *walk;
    size_t      walk_num;

/**/myassert( arr != NULL );
    if( elm_num >= arr->num_elms ) {
        arr->num_elms = elm_num + 1;
    }
#ifdef USE_ROVER
    if( elm_num >= arr->rov_num ) {
        walk_num = elm_num - arr->rov_num;
        walk = arr->rov_blk;
    } else {
        walk_num = elm_num;
        walk = arr->block;
        if( walk == NULL ) {
            walk = arr->block = newBlock( arr );
        }
    }
#else
    walk_num = elm_num;
    walk = arr->block;
    if( walk == NULL ) {
        walk = arr->block = newBlock( arr );
    }
#endif
/**/myassert( walk != NULL );
    while( walk_num >= BLOCK_SIZE ) {
        walk_num -= BLOCK_SIZE;
        if( walk->next == NULL ) {
            walk->next = newBlock( arr );
        }
        walk = walk->next;
    }
/**/myassert( walk != NULL );
#ifdef USE_ROVER
/**/myassert( walk_num < BLOCK_SIZE && walk_num == elm_num % BLOCK_SIZE );
    arr->rov_num = elm_num - walk_num;
    arr->rov_blk = walk;
#endif
    return( walk->data + walk_num * arr->elm_size );
}

void *ArrAccess( array_hdr *arr, size_t elm_num ) {
/***********************************************/
    array_blk   *walk;
    size_t      walk_num;

/**/myassert( arr != NULL );
/**/myassert( elm_num < arr->num_elms );
#ifdef USE_ROVER
    if( elm_num >= arr->rov_num ) {
        walk_num = elm_num - arr->rov_num;
        walk = arr->rov_blk;
    } else {
        walk_num = elm_num;
        walk = arr->block;
    }
#else
    walk_num = elm_num;
    walk = arr->block;
#endif
/**/myassert( walk != NULL );
    while( walk_num >= BLOCK_SIZE ) {
        walk_num -= BLOCK_SIZE;
        walk = walk->next;
/**/    myassert( walk != NULL );
    }
/**/myassert( walk != NULL );
#ifdef USE_ROVER
/**/myassert( walk_num < BLOCK_SIZE && walk_num == elm_num % BLOCK_SIZE );
    arr->rov_num = elm_num - walk_num;
    arr->rov_blk = walk;
#endif
    return( walk->data + walk_num * arr->elm_size );
}

int
ArrWalk( array_hdr *arr, void *parm, int (*func)( void *elm, void *parm ) ) {
/*************************************************************************/
    array_blk   *cur;
    char        *elm_ptr;
    char        *stop_ptr;
    size_t      elm_size;
    div_t       res;

/**/myassert( arr != NULL );

    cur = arr->block;
    if( cur == NULL ) {
        return( 0 );
    }
    elm_size = arr->elm_size;
    res = div( arr->num_elms, BLOCK_SIZE );
    while( res.quot > 0 ) {
/**/    myassert( cur != NULL );
        elm_ptr = cur->data;
        cur = cur->next;
        stop_ptr = elm_ptr + elm_size * BLOCK_SIZE;
        while( elm_ptr < stop_ptr ) {
            if( func( (void *)elm_ptr, parm ) == -1 ) {
                return( -1 );
            }
            elm_ptr += elm_size;
        }
        --res.quot;
    }
    if( res.rem > 0 ) {
/**/    myassert( cur != NULL );
        elm_ptr = cur->data;
        stop_ptr = elm_ptr + elm_size * res.rem;
        while( elm_ptr < stop_ptr ) {
            if( func( (void *)elm_ptr, parm ) == -1 ) {
                return( -1 );
            }
            elm_ptr += elm_size;
        }
    }
    return( 0 );
}
