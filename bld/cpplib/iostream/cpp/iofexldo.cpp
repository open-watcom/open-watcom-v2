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
#include <errno.h>
#include <iostream>
#endif
#include "ioutil.h"
#include "lock.h"
#include "iofhdr.h"
#include "fltsupp.h"


namespace std {

  // Extract a "long double" from the stream.

  istream &istream::operator >> ( long double &f ) {

    char         buf[DEFAULT_MAINBUF_SIZE];
    ios::iostate state;

    state = ios::goodbit;
    __lock_it( __i_lock );
    if( ipfx() ) {
        state = __GetLDFloat( rdbuf(), buf );
        if( !state ) {
            errno = 0;
            __EFG_cnvs2d( buf, (double *)&f );
            if( errno == ERANGE ) {
                state |= ios::failbit;
            }
        }
        setstate( state );
        isfx();
    }
    return( *this );
  }

}
