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


#ifndef __RING_H__
#define __RING_H__

class RingHdr;
class RingIter;

class Ring                      // Ring class
{                               // **********
    Ring* _next;                // next in ring

public:

    inline
    Ring                        // constructor
        ( void )
    ;
    inline
    Ring* next                  // get next
        ( void );
    inline
    Ring const * next           // get next
        ( void ) const;

friend class RingHdr;
friend class RingIter;
};

//---------------------------------------------------------------------
// Ring Implementation
//---------------------------------------------------------------------

Ring::Ring                      // Constructor
    ( void )
    : _next( 0 )
{
}

Ring* Ring::next                // GET NEXT
    ( void )
{
    return _next;
}

Ring const* Ring::next          // GET NEXT
    ( void ) const
{
    return _next;
}


class RingHdr                   // Ring Header
{                               // ***********
    Ring* _hdr;                 // header

public:

    RingHdr                     // constructor
        ( void )
    ;
    Ring* append                // insert at end of ring
        ( Ring * )              // - element
    ;
    unsigned countElements      // count elements
        ( void ) const
    ;
    inline
    Ring* last                  // get last element
        ( void ) const
    ;
    Ring* pop                   // POP AN ENTRY
        ( void )
    ;
    Ring* push                  // PUSH AN ENTRY
        ( Ring* )               // - entry
    ;
    void sort                   // sort ring
        ( int (*rtn)            // - comparor
            ( void const *      // - - operand 1
            , void const * ) )  // - - operand 2
    ;

friend class RingIter;
};


Ring* RingHdr::last             // GET LAST ELEMENT
    ( void ) const
{
    return _hdr;
}


class RingIter                  // Ring Iterator
{                               // *************
    Ring* _hdr;                 // - original header
    Ring* _cur;                 // - current element

public:

    inline
    RingIter                    // CONSTRUCTOR
        ( RingHdr const & )
    ;
    inline
    Ring* next                  // GET NEXT
        ( void )
    ;
};


//---------------------------------------------------------------------
// RingIter Implementation
//---------------------------------------------------------------------


RingIter::RingIter              // CONSTRUCTOR
    ( RingHdr const & rh )      // - header
    : _hdr( rh._hdr )
    , _cur( 0 )
{
}


Ring* RingIter::next            // GET NEXT
    ( void )
{
    Ring* retn;
    if( _cur == _hdr ) {
        retn = 0;
    } else if( _cur == 0 ) {
        retn = _hdr->_next;
    } else {
        retn = _cur->_next;
    }
    _cur = retn;
    return retn;
}

#endif
