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
* Description:  code to manage free lists for a given entity
*
****************************************************************************/


#include "ftnstd.h"
#include "fmemmgr.h"
#include "frl.h"


void    *FrlAlloc( void ***head, unsigned size ) {
//================================================

// Bag an item from the free list pointer head if possible.

    void        *next;

    if( *head ) {
        next = *head;
        *head = **head;
    } else {
        next = FMemAlloc( size );
    }
    return( next );
}


void    FrlFree( void *arg_head, void *arg_item ) {
//=================================================

// Add item to the list head pointer head.

    void        **head;
    void        **item;

    head = arg_head;
    item = arg_item;
    *item = *head;
    *head = item;
}


void    FrlInit( void ***head ) {
//===============================

// Initialize for free list manipulations.

    *head = NULL;
}


void    FrlFini( void ***head ) {
//===============================

// Throw away a free list.

    void        **curr;
    void        *junk;

    curr = *head;
    *head = NULL;
    while( curr ) {
        junk = curr;
        curr = *curr;
        FMemFree( junk );
    }
}
