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
#include "ioutil.h"
#include "lock.h"

namespace std {

  // Set up a new reserve area using the specified buffer and length. If
  // a reserve area is already present, then ignore the offered buffer.
  // If buffer is NULL or len is 0, then the streambuf is unbuffered.

  streambuf *streambuf::setbuf( char *buf, int len ) {

    __lock_it( __b_lock );
    if( base() != NULL ) {
        return( NULL );
    }
    if( (buf == NULL) || (len <= 0) ) {
        setb( NULL, NULL, FALSE );
    } else {
        if( len <= DEFAULT_PUTBACK_SIZE ) {
            setb( NULL, NULL, FALSE );
            return( NULL );
        }
        setb( buf, buf + len, FALSE );
    }
    return( this );
  }

}
