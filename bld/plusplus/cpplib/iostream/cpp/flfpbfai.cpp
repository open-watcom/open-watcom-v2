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
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#endif

namespace std {

  // Handle attempt to put back off the begining of the get area.

  int filebuf::pbackfail( int c ) {

    char *ptr;
    int  offset;

    __lock_it( __b_lock );
    if( base() == NULL ) {
        if( allocate() == EOF ) {
            return( EOF );
        }
        if( base() == NULL ) {
            // unbuffered special case
            ptr = __unbuffered_get_area + DEFAULT_PUTBACK_SIZE;
            setg( __unbuffered_get_area, ptr, ptr );
        } else {
            ptr = base() + DEFAULT_PUTBACK_SIZE;
            setg( base(), ptr, ptr );
        }
        // note: this function will NOT call filebuf::pbackfail() again
        //       because we have ensured that __get_ptr > __get_base
        return( sputbackc( (char)c ) );
    }
    if( sync() == EOF ) {
        return( EOF );
    }
    if( (c == '\n') && (__file_mode & ios::text) ) {
        offset = -2;
    } else {
        offset = -1;
    }
    if( lseek( fd(), offset, ios::cur ) < 0 ) {
        return( EOF );
    }
    return( c );
  }

}

