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


#include "womp.h"
#include "myassert.h"
#include "canaddr.h"
#include "cansymb.h"
#include "cantype.h"
#include "namemgr.h"
#include "memutil.h"
#include "carve.h"

STATIC symb_handle  cansHead;
STATIC carve_t      myCarver;

void CanSInit( void ) {
/*******************/
    cansHead = CANS_NULL;
    myCarver = CarveCreate( sizeof( struct cansymb ), 16 );
}

void CanSFini( void ) {
/*******************/
    symb_handle cur;
    symb_handle next;

    cur = cansHead;
    if( cur != NULL ) {
        do {
            next = CanSFwd( cur );
            switch( cur->class ) {
            case CANS_CHANGE_SEG:
                CanADestroyHdl( cur->d.cseg.seg );
                break;
            case CANS_MEM_LOC:
                CanADestroyHdl( cur->d.memloc.mem_hdl );
                break;
            }
            cur = next;
        } while( cur != cansHead );
        cansHead = CANS_NULL;
    }
    CarveDestroy( myCarver );
}

symb_handle CanSGetHead( void ) {
/*****************************/
    return( cansHead );
}

void CanSSetHead( symb_handle head ) {
/**********************************/
    cansHead = head;
}

#if 0
symb_handle CanSFwd( symb_handle cursor ) {
/***************************************/
    return( cursor->fwd );
}

symb_handle CanSBwd( symb_handle cursor ) {
/***************************************/
    return( cursor->bwd );
}
#endif

symb_handle CanSIAfter( symb_handle cursor, symb_handle insert ) {
/**************************************************************/
/*
    Split the insert ring so that insert becomes the head, then place this
    ring right after cursor.
*/
    if( insert == CANS_NULL ) {
        return( cursor );
    }
    if( cursor == CANS_NULL ) {
        return( insert );
    }
    /* inserting an entire ring */
    insert->bwd->fwd = cursor->fwd;
    cursor->fwd->bwd = insert->bwd;
    cursor->fwd = insert;
    insert->bwd = cursor;
    return( cursor );
}

symb_handle CanSIBefore( symb_handle cursor, symb_handle insert ) {
/***************************************************************/
    symb_handle temp;

    if( insert == CANS_NULL ) {
        return( cursor );
    }
    if( cursor == CANS_NULL ) {
        return( insert );
    }
    /* inserting an entire ring */
    insert->bwd->fwd = cursor;
    cursor->bwd->fwd = insert;
    temp = insert->bwd;
    insert->bwd = cursor->bwd;
    cursor->bwd = temp;
    return( cursor );
}

symb_handle CanSNew( uint_8 class ) {
/*********************************/
    symb_handle new;

    new = CarveAlloc( myCarver );
    new->fwd = new;
    new->bwd = new;
    new->class = class;
    new->d.nat.name_hdl = NAME_NULL;
    new->d.nat.type_hdl = CANT_NULL;
    return( new );
}
