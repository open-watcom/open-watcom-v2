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
#include <string.h>
#include <strstrea>
#endif
#include "ioutil.h"

namespace std {

  // Freeze the strstreambuf, and return a pointer to the contents. This
  // effectively terminates modification of the buffer, and the buffer
  // is now the responsibility of the user. We also realloc() the
  // reserve area down to the exact required size. This involves some
  // fancy pointer computations to set the get area and put area up
  // correctly in the new reserve areay.

  char *strstreambuf::str() {

    char    *newbuf;
    char    *oldbuf;
    int     size;
    __lock_it( __b_lock );

    if( (__dynamic) && (base() != NULL) && (gptr() == NULL) ) {
        oldbuf = base();
        size = (__huge_ptr_int)(pptr() - pbase());
        if( __minbuf_size > size ) {
            size = __minbuf_size;
        }
        if( __alloc_fn == NULL ) {
            newbuf = new char [size];
        } else {
            newbuf = (char *) __alloc_fn( size );
        }
        if( newbuf != NULL ) {
            ::memcpy( newbuf, oldbuf, size );
            setb( newbuf, newbuf + size, FALSE );

            if( pbase() != NULL ) {
                size_t base_offset = (__huge_ptr_int)(pbase() - oldbuf);
                size_t ptr_offset  = (__huge_ptr_int)(pptr()  - oldbuf);
                setp( newbuf+base_offset, newbuf+size );
                pbump( ptr_offset - base_offset );
            }
            if( __free_fn == NULL ) {
                delete [] oldbuf;
            } else {
                __free_fn( oldbuf );
            }
        }
    }
    __frozen = 1;
    return( base() );
  }

}
