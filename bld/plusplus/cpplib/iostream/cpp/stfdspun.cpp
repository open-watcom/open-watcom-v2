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
#include <string.h>
#include <iostream>
#include <streambu>
#endif
#include "ioutil.h"
#include "lock.h"

namespace std {

  // Write the "len" characters starting at "buf" into the put area.
  // Return 0 if not possible. Return the number of characters
  // successfully put.

  int streambuf::do_sputn( char const *buf, int len ) {

    int waiting;
    int written;

    written = 0;
    __lock_it( __b_lock );
    while( len > 0 ) {
        if( pbase() != NULL ) {
            // # chars available in put area
            waiting = (__huge_ptr_int)(epptr() - pptr());
            if( waiting <= 0 ) {            // full or not there?
                if( overflow( EOF ) == EOF ) {
                    return( written );
                }
                waiting = (__huge_ptr_int)(epptr() - pptr());
            }
            if( waiting > len ) {           // more than we have to sputn?
                waiting = len;
            }
            ::memcpy( pptr(), buf, waiting );
            pbump( waiting );
        } else {
            if( overflow( *buf ) == EOF ) {
                return( written );
            }
            waiting = 1;
        }
        written += waiting;
        buf     += waiting;
        len     -= waiting;
    }
    return( written );
  }

}
