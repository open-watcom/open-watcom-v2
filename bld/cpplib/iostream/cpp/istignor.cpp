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

namespace std {

  // Ignore "n" characters, or until the specified delimiter is found,
  // whichever comes first. If "delim" is EOF, don't look for a
  // delimiter. As an extension, specifying a negative "n" value will
  // not count ignored characters and will continue ignoring until the
  // delimiter is found.

  istream &istream::ignore( int n, int delim ) {

    int c;

    __lock_it( __i_lock );
    if( !ipfx( 1 ) ) {
        __last_read_length = 0;
        return( *this );
    }
    __lock_it( rdbuf()->__b_lock );
    for( ;; ) {
        if( n >= 0 ) {
            if( --n < 0 ) break;
        }
        c = rdbuf()->sgetchar();
        if( c == EOF ) break;
        if( (delim != EOF) && (c == delim) ) break;
    }
    isfx();
    return( *this );
  }

}
