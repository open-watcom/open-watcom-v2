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
#include "stfhdr.h"
#include "liballoc.h"

namespace std {

  // Set up a new reserve area. Delete the old one if necessary. Turn
  // buffering on.

  void streambuf::setb( char *buf, char *ebuf, int autodelete ) {

    __lock_it( __b_lock );
    if( __delete_reserve ) {
        _plib_free( __reserve_base );
    }
    if( (buf == NULL) || (ebuf <= buf) ) {
        __reserve_base     = NULL;
        __reserve_end      = NULL;
        __delete_reserve   = 0;
        __unbuffered_state = 1;
    } else {
        __reserve_base     = buf;
        __reserve_end      = ebuf;
        __delete_reserve   = (char)(autodelete ? 1 : 0);
        __unbuffered_state = 0;
    }
  }

}
