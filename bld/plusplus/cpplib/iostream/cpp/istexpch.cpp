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
#include <ctype.h>
#include <iostream>
#endif
#include "lock.h"

namespace std {

  // Input a string of non-whitespace characters into the buffer. If the
  // width is set, read a maximum of that many characters, less one for
  // the NULLCHAR on the end. Otherwise, keep reading until EOF or a
  // whitespace character.

  istream &istream::operator >> ( char *buf ) {
    int c;
    int offset;
    int last_offset;

    offset = 0;
    __lock_it( __i_lock );
    if( ipfx() ) {
        last_offset = width() - 2;
        __lock_it( rdbuf()->__b_lock );
        for( ;; ) {
            if( (width() != 0) && (offset > last_offset) ) {
                break;
            }
            c = rdbuf()->speekc();
            if( c == EOF ) {
                if( offset == 0 ) {
                    setstate( ios::eofbit );
                }
                break;
            }
            if( isspace( c ) || c == 0 ) {
                break;
            }
            buf[offset++] = (unsigned char)c;
            rdbuf()->sbumpc();
        }
        isfx();
    }
#if 1
    // Borland's implementation does not set failbit if no
    // characters are read.
    if( offset == 0 ) {
        setstate( ios::failbit );
    }
#endif
    width( 0 );
    buf[offset] = '\0';
    return( *this );
  }

}
