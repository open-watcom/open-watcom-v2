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

  // Open the named file and attach it to this filebuf. If the filebuf
  // is already attached, then return NULL. If the open fails for any
  // reason, then return NULL. Otherwise, return a pointer to the
  // filebuf.

  filebuf *filebuf::open( char const *name, ios::openmode p_mode, int prot ) {

    __lock_it( __b_lock );

    if( __file_handle != EOF ) {
        return( NULL );
    }

    if( p_mode & ios::app ) {
        p_mode |= ios::out;
    }

    // If ios::out is specified, and none of ios::in, ios::append or
    // ios::atend are specified, then ios::truncate is implied:
    if( p_mode & ios::out ) {
        if( (p_mode & (ios::in | ios::append | ios::atend)) == 0 ) {
            p_mode |= ios::truncate;
        }
    }

    // If both ios::binary and ios::text are specified, give an error.
    if( p_mode & ios::binary ) {
        if( p_mode & ios::text ) {
            return( NULL );
        }
    }

    // If both ios::noreplace and ios::nocreate are specified, give an error.
    if( p_mode & ios::noreplace) {
        if( p_mode & ios::nocreate) {
            return( NULL);
        }
    }

    // Go ahead and try to open the file:
    __file_handle = ::__plusplus_open( name, &p_mode, prot );
    if( __file_handle < 0 ) {
        __file_handle = EOF;
        return( NULL );
    }
    __file_mode = p_mode;
    __attached = 0;

    // If "atend" was specified, then seek to the end:
    if( p_mode & ios::atend ) {
        seekoff( 0, ios::end, p_mode );
    }
    return( this );
  }

}

