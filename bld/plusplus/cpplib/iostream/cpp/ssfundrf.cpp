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

namespace std {

  // The streambuf has decided that it needs more characters for input.
  // If a put area is present, extend the get area into it. Otherwise,
  // fail.

  int strstreambuf::underflow() {
    __lock_it( __b_lock );
    if( __unlimited ) {
        // adjust end pointer
        setg( eback(), gptr(), egptr() + DEFAULT_MAINBUF_SIZE );
        return( *gptr() );
    }
    if( pptr() > egptr() ) {
        if( gptr() == NULL ) {              // currently no get area?
            setg( pbase(), pbase(), pptr() );
        } else {
            setg( eback(), gptr(), pptr() );
        }
        return( *gptr() );
    }
    return( EOF );
  }

}
