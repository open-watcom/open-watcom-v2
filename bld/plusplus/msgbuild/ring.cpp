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


// Ring Class
//
// 95/01/31 -- J.W.Welch        -- defined

#include <malloc.h>
#include <stdlib.h>

#include "Ring.h"


//---------------------------------------------------------------------
// RingHdr Implementation
//---------------------------------------------------------------------


RingHdr::RingHdr                // Constructor
    ( void )
    : _hdr( 0 )
{
}


Ring* RingHdr::append           // Add to end of ring
    ( Ring* element )
{
    if( 0 == _hdr ) {
        element->_next = element;
    } else {
        element->_next = _hdr->_next;
        _hdr->_next = element;
    }
    _hdr = element;
    return element;
}


unsigned RingHdr::countElements // count the # elements in the ring
    ( void ) const
{
    RingIter iter( *this );
    unsigned count = 0;
    for( ; ; ++count ) {
        Ring const* curr = iter.next();
        if( 0 == curr ) break;
    }
    return count;
}


void RingHdr::sort              // Sort a ring
    ( int (*comp_rtn)           // - comparator
        ( void const*           // - - operand 1
        , void const* ) )       // - - operand 2
{
    if( 0 != _hdr ) {
        unsigned count = countElements();
        Ring ** vect = (Ring**)alloca( count * sizeof( Ring* ) );
        RingIter iter( *this );
        Ring ** curr = vect;
        for( ; ; ) {
            Ring* old = iter.next();
            if( 0 == old ) break;
            *curr = old;
            ++curr;
        }
        ::qsort( vect, count, sizeof( Ring* ), comp_rtn );
        _hdr = 0;
        for( curr = vect; count > 0; --count, ++curr ) {
            append( *curr );
        }
    }
}


Ring* RingHdr::pop              // POP THE FIRST ENTRY
    ( void )
{
    Ring* entry;                // - entry

    entry = _hdr;
    if( 0 != entry ) {
        entry = entry->_next;
        if( _hdr == entry ) {
            _hdr = 0;
        } else {
            _hdr->_next = entry->_next;
        }
    }
    return entry;
}


Ring* RingHdr::push             // PUSH AN ENTRY
    ( Ring* element )           // - entry
{
    if( 0 == _hdr ) {
        element->_next = element;
        _hdr = element;
    } else {
        element->_next = _hdr->_next;
        _hdr->_next = element;
    }
    return element;
}
