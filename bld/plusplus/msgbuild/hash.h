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


#ifndef __HASH_H__
#define __HASH_H__

// Hash -- hash table

#include "Ring.h"

const unsigned HashModulus = 147;

class HashEntry                 // HASH ENTRY
    : public Ring
{
    void const* _entry;         // - entry

public:

    HashEntry                   // - CONSTRUCTOR
        ( void const * );

friend class HashTable;
};


class HashTable                 // HASH TABLE
{
    RingHdr _table[ HashModulus ]; // - hash table
    unsigned (*_hashfun)        // - hash function
        ( void const * )        // - - comparand
    ;
    int (*_hashcmp)             // - comparison function
        ( void const *          // - - entry
        , void const * )        // - - comparand
    ;

public:

    HashTable                   // CONSTRUCTOR
        ( unsigned (*)          // - hash function
            ( void const * )    // - - comparand
        , int (*)               // - comparison function
            ( void const *      // - - element
            , void const * ) )  // - - comparand
    ;
    void add                    // ADD AN ENTRY
        ( void const *          // - - element
        , void const * )        // - - comparand
    ;
    void const * find           // FIND AN ENTRY
        ( void const * );       // - comparand
    ;
};

#endif
