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

  // Seek to a new position in the file.

  streampos filebuf::seekoff( streamoff offset,
                              ios::seekdir dir,
                              ios::openmode ) {

    int       origin;
    streampos newpos;

    __lock_it( __b_lock );
    if( fd() == EOF ) {
        return( EOF );
    }
    if( out_waiting() || in_avail() ) {
        if( sync() == EOF ) {           // flush buffers
            return( EOF );
        }
    }
    switch( dir ) {
    case ios::beg:
        origin = SEEK_SET;
        break;
    case ios::cur:
        origin = SEEK_CUR;
        break;
    case ios::end:
        origin = SEEK_END;
        break;
    default:
        return( EOF );
    }
    newpos = lseek( fd(), offset, origin );
    if( newpos == -1 ) {
        return( EOF );
    }
    return( newpos );
  }

}
