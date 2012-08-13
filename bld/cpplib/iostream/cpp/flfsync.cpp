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
#include "ioutil.h"

namespace std {

  // Synchronize the output of this stream with the underlying C
  // standard I/O. Only one of input flushing or output flushing can
  // happen since input is flushed when output is started and vice
  // versa.

  int filebuf::sync() {

    __lock_it( __b_lock );

    // Flush any output to the file:
    if( out_waiting() ) {
        if( overflow( EOF ) == EOF ) {
            return( EOF );
        }
        return( __NOT_EOF );
    }

    // Flush any input characters. Back up the file so that a new read
    // of the actual file will start where we are now. Beware of
    // "new-line to CR/LF" translation issues when handling a text file:
    if( in_avail() ) {
        char *cptr;
        int   offset;

        if( __file_mode & ios::text ) {
            offset = 0;
            cptr   = egptr();
            if( cptr != NULL ) {
                for( ; --cptr >= gptr(); ) {
                    offset -= (*cptr == '\n') ? 2 : 1;
                }
            }
        } else {
            offset = (__huge_ptr_int)(gptr() - egptr());
        }
        if( lseek( fd(), offset, ios::cur ) < 0 ) {
            return( EOF );
        }
        setg( NULL, NULL, NULL );
    }
    return( __NOT_EOF );
  }

}

