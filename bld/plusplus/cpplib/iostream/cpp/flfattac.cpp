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
#include <fstream.h>
#endif
#include "ioutil.h"

namespace std {

  // Attach an open file (described by fd) to this filebuf. If the
  // filebuf is already attached, return NULL, otherwise return a
  // pointer to the filebuf. Note that we must extract the "mode"
  // information via the file's handle.

  filebuf *filebuf::attach( filedesc fd ) {
    __lock_it( __b_lock );
    if( __file_handle != EOF ) {
        return( NULL );
    }

    // Derive the value of "mode" from the file handle:
    if( ::__plusplus_fstat( fd, &__file_mode ) != 0 ) {
        return( NULL );
    }
    __file_handle = fd;
    __attached    = 1;
    return( this );
  }

}
