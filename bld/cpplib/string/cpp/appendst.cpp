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


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %     Copyright (C) 1992, by WATCOM International Inc.  All rights    %
// %     reserved.  No part of this software may be reproduced or        %
// %     used in any form or by any means - graphic, electronic or       %
// %     mechanical, including photocopying, recording, taping or        %
// %     information storage and retrieval systems - except with the     %
// %     written permission of WATCOM International Inc.                 %
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//  Modified    By              Reason
//  ========    ==              ======
//  92/01/09    Steve McDowell  Initial simple implementation using C library
//                              string functions wherever possible.
//  92/01/20    ...             Added checking for invalid Strings, including
//                              adding the ! operator, the == and !=
//                              operators with integer arguments, and the
//                              "valid" member and friend functions.
//  92/03/11    ...             Re-implemented using StringReps (reference
//                              counts, offset and length).
//  92/10/08    Greg Bentz      Cleanup.
//  93/08/31    Greg Bentz      - make extractor skip whitespace if
//                                ios::skipws is set
//                              - correct offset computation in substring
//  93/10/07    Greg Bentz      - alloc_mult_size() not setting new value right
//  93/10/19    Raymond Tang    split into separate files
//  94/04/06    Greg Bentz      combine header files
//  95/06/13    Greg Bentz      fix bug when joining too strings

#include "strng.h"

void String::__AppendToStringRep( const String &l, const char *r,
/***************************************************************/
    size_t rlen ) {
// Append r/rlen to l, leaving *this pointing to the result.
// For any append/concatenate operator where the left side is a String,
// it might be possible to use realloc() to efficiently extend the
// left side for the stuff on the right.
// This can happen only if the left side String offset/slength ends at the
// last character in the StringRep.
// Note that the reference count of the StringRep had better be 1, otherwise
// some other String(s) might be left with bad srep pointers!
// Also, note that *this and "l" might be the same object, so be careful
// about the reference count when altering the "srep" pointer.
    size_t l_endoffset;

    l_endoffset = l.__offset + l.__slength;

    // If l.offset/l.slength go right to the end of the characters in the
    // StringRep, then we might be able to append directly to "l":
    if( l_endoffset >= l.__srep->__used_length ) {

        // If "r" is small enough to fit on the end of "l"
        // (in the unused area), then copy it there:
        if( l.__srep->__avail_length - l.__srep->__used_length >= rlen ) {
            ::memcpy( l.__srep->__value + l.__srep->__used_length, r, rlen );
            l.__srep->__used_length += rlen;
            if( this == &l ) {
                __slength += rlen;
            } else {
                __FreeStringRep();
                l.__srep->__refcount++;
                __srep    = l.__srep;
                __offset  = l.__offset;
                __slength = l.__slength + rlen;
            }
            return;
        }

        // If nobody else is pointing at the StringRep, then we can use
        // realloc (in AllocStringRep) to get a bigger string
        // (realloc might change the srep pointer, so nobody else can be
        // pointing at this StringRep):
        if( l.__srep->__refcount == 1 ) {
            String::StringRep *new_srep;

            new_srep = __AllocStringRep( l.__srep, l_endoffset + rlen );
            if( new_srep == NULL ) {
                __FreeStringRep();
            } else {
                ::memcpy( new_srep->__value + l_endoffset, r, rlen );
                if( this == &l ) {
                    __slength += rlen;
                    __srep     = new_srep;
                } else {
                    this->__FreeStringRep();
                    new_srep->__refcount++;
                    __slength = l.__slength + rlen;
                    __offset  = l.__offset;
                    {
                        // we are modifying a const parm here
                        // this is ok because the value of the parm is
                        // not changing, only a detail of the representation
                        String *p = (String *)&l;
                        p->__srep  = new_srep;
                    }
                    __srep    = new_srep;
                }
            }
            return;
        }
    }

    // Either somebody else is pointing at the StringRep, or
    // l.offset/l.length doesn't go right to the end of the
    // StringRep value, so we must allocate a new StringRep to
    // hold the result:
    __JoinStrings( l.__srep->__value + l.__offset, l.__slength, r, rlen );
}
