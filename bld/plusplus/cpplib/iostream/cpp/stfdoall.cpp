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
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <streambu>
#endif
#include "lock.h"
#include "liballoc.h"

namespace std {

  // Do the allocation required if allocate() thinks it's needed. If the
  // allocation fails, return EOF.

  int streambuf::doallocate() {

    char *buf;

    __lock_it( __b_lock );
    buf = (char *)_plib_malloc( DEFAULT_BUF_SIZE );
    if( buf == NULL ) {
        return( EOF );
    }
    setb( buf, buf + DEFAULT_BUF_SIZE, 1 );
    return( __NOT_EOF );          // something other than EOF!
  }

}
