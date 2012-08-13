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
#include "stfhdr.h"

namespace std {

  // Construct an empty streambuf using the given buf. If buffer is NULL
  // or len is 0, then the streambuf is unbuffered.

  streambuf::streambuf( char *buf, int len ) {

    #ifdef __MT__
        __b_lock = __get_next_streambuf_lock();
    #else
        __b_lock = NULL;
    #endif
    __reserve_base     = NULL;
    __reserve_end      = NULL;
    __get_base         = NULL;
    __get_end          = NULL;
    __get_ptr          = NULL;
    __put_base         = NULL;
    __put_end          = NULL;
    __put_ptr          = NULL;
    __delete_reserve   = 0;
    __unbuffered_state = 0;
    setbuf( buf, len );
  }

}
