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
#include <iostream>
#include <streambu>
#endif

namespace std {

  void streambuf::dbp() {
    printf( "STREAMBUF Debug Info:\n"
            "this  = %p, unbuffered = %d, delete_reserve = %d\n"
            "base  = %p, ebuf = %p\n"
            "eback = %p, gptr = %p, egptr = %p\n"
            "pbase = %p, pptr = %p, epptr = %p\n"
          , this
          , __unbuffered_state
          , __delete_reserve
          , __reserve_base
          , __reserve_end
          , __get_base
          , __get_ptr
          , __get_end
          , __put_base
          , __put_ptr
          , __put_end
     );
  }

}
