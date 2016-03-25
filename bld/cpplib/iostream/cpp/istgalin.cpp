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
#include <iostream>
#include <streambu>
#endif
#include "lock.h"
#include "isthdr.h"

// Read characters into buffer "buf". At most "len - 1" characters are
// read, and a 0 is added at the end. If "delim" is encountered, it is
// left in the stream and the read is terminated (and the NULLCHAR is
// added).
//
// Used by:
//    get( char *buf, int len, char delim )
//    getline( char *buf, int len, char delim )
//
// NOTE: Borland sets eofbit only. A full buffer just stops reading. If
//       something has been read, set eofbit anyway.
//
//       The proposed standard says to set failbit if the buffer is
//       filled without finding the delimiter (if doing a "getline"), or
//       if a read fails and no characters are extracted. It says
//       nothing about eofbit.
//
//       Currently we set eofbit only if eof occurs on first character
//       read. failbit is set if no characters were read.
//
//       Failbit was being set (prior to Nov15,95) when len-1 characters
//       were read in the buffer. At that time we discerned that no one
//       else was setting failbit in that case, so we stopped doing it.

ios::iostate __getaline( std::istream &istrm,
                         char *buf,
                         int len,
                         char delim,
                         int is_get,
                         int &chars_read ) {
    int           c;
    int           offset;
    ios::iostate  state = 0;
    streambuf    *sb;

    offset = 0;
    __lock_it( istrm.__i_lock );
    if( len > 1 && istrm.ipfx( 1 ) ) {
        sb = istrm.rdbuf();
        __lock_it( sb->__b_lock );
        len--;  // leave a space for the NULLCHAR
        while( offset < len ) {
            c = sb->speekc();
            if( c == EOF ) {
                if( offset == 0 ) {
                    state |= ios::eofbit;
                }
                break;
            }
            if( c == (int)(unsigned char)delim ) {
                if( !is_get ) {
                    sb->sbumpc();
                }
                break;
            }
            buf[offset++] = (char)c;
            sb->sbumpc();
        }
        istrm.isfx();
    }
    buf[offset]  = '\0';
    chars_read = offset;

    // the draft standard says that no characters is an error if using
    // get() or getline(). the IOStreams Handbook suggests that no
    // characters is not an error if the delim was seen, this seems to
    // be what our competitors do.
    // 
    if( (offset == 0) && (c != (int)(unsigned char)delim) ) {
        state |= ios::failbit;
    }

    // the draft standard says that len-1 characters is an error if
    // using getline() our competitors don't agree with the draft, we
    // won't follow the draft either.
    // 
    #if 0
    if( offset == len && !is_get ) {
        state |= ios::failbit;
    }
    #endif
    return( state );
}
