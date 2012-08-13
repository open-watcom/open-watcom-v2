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

  // Read all characters from the istream and write them to the streambuf.

  istream &istream::operator >> ( streambuf *tgt_sb ) {
    char buf[DEFAULT_MAINBUF_SIZE];
    int  len;

    __lock_it( __i_lock );
    if( ipfx( 1 ) ) {
        streambuf *src_sb;
        src_sb = rdbuf();
        __lock_it( __lock_first( src_sb->__b_lock, tgt_sb->__b_lock ) );
        __lock_it( __lock_second( src_sb->__b_lock, tgt_sb->__b_lock ) );
        for(;;) {
            len = src_sb->sgetn( buf, sizeof( buf ) );
            if( len == 0 ) break;
            if( tgt_sb->sputn( buf, len ) != len ) break;
        }
        isfx();
    }
    return( *this );
  }

}
