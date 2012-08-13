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
* Description:
*
****************************************************************************/

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <strstrea>
#endif
#include "ssfhdr.h"

// For "put" buffers, "extended_end" equals "end". For "get" buffers,
// "extended_end" may be greater than "end" if there are characters in
// the put area, allowing the seek to go beyond the end of the get
// area. The get area must then be extended.

streampos __get_position( streamoff offset,
                          ios::seekdir direction,
                          char *base,
                          char *ptr,
                          char *end,
                          char *extended_end ) {

    streampos size;

    size = extended_end - base;
    if( direction == ios::beg ) {
        return( offset <= size ? offset : EOF );
    }
    if( direction == ios::end ) {
        size   = end - base;        // use real end, not extended_end
        offset = -offset;
        return( offset <= size ? size - offset : EOF );
    }
    if( direction == ios::cur ) {
        streampos cur_offset = ptr - base;
        streampos remaining  = extended_end - ptr;

        if( offset < 0 ) {
            offset = -offset;
            return( offset <= cur_offset ? cur_offset - offset : EOF );
        }
        if( offset > 0 ) {
            return( offset <= remaining ? cur_offset + offset : EOF );
        }
        return( cur_offset );
    }
    return( EOF );
}
