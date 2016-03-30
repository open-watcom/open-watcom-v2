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
* Description:  chain processing
*
****************************************************************************/


#include "ftnstd.h"
#include "fmemmgr.h"
#include "chain.h"


void    FreeChain( void *_head ) {
//================================

// Free a chain.
// The "link" field in the chain must be first in the structure.

    void        **head = _head;
    void        *next;
    void        **chain;

    chain = *head;
    while( chain != NULL ) {
        next = *chain;
        FMemFree( chain );
        chain = next;
    }
    *head = NULL;
}


void    *FreeLink( void **link ) {
//================================

// Free a link in the chain.
// The "link" field in the chain must be first in the structure.

    void        *next;

    next = *link;
    FMemFree( link );
    return( next );
}


void    ReverseList( void **head ) {
//==================================

// Reverse a linked list.

    void        *rev_list;
    void        *next;
    void        **entry;

    rev_list = NULL;
    entry = *head;
    while( entry != NULL ) {
        next = *entry;
        *entry = rev_list;
        rev_list = entry;
        entry = next;
    }
    *head = rev_list;
}
