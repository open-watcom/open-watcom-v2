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


#ifndef __CARVE_H__
#define __CARVE_H__

// Carve -- support class for operator new, new[]

#include "Ring.h"



// Carve - object controlling carving

class Carve
{
    RingHdr _blks;              // - allocated blocks
    RingHdr _freed;             // - freed entries
    unsigned _size_entry;       // - size of an entry
    unsigned _size_block;       // - size of a block

    class CarveBlock            // class: block of elements
        : public Ring
    {
        char* _data;            // - - block of data
    friend class Carve;
    };

    class FreeElement           // class: free elements
        : public Ring
    {
    };

public:

    Carve                       // CONSTRUCTOR
        ( unsigned              // - size of an entry
        , unsigned )            // - # entries / block
    ;
    ~Carve                      // DESTRUCTOR
        ( void )
    ;
    void* alloc                 // ALLOCATE AN ENTRY
        ( void )
    ;
    void free                   // FREE AN ENTRY
        ( void* )               // - the entry
    ;
};

#endif
