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
#endif
#include "lock.h"
#include "ioshdr.h"

namespace std {

  // Initialize the ios, including a streambuf pointer. If the streambuf
  // pointer is NULL, mark the stream as currently invalid.

  void ios::init( streambuf *sb ) {

#ifdef __SW_BM
    __i_lock = __get_next_ios_lock();
#else
    __i_lock = NULL;
#endif
    __error_state = 0;
    __format_flags = 0;
    __enabled_exceptions = 0;
    __float_precision = 6;
    __field_width = 0;
    __fill_character = ' ';
    __tied_stream = NULL;
    __xalloc_list = NULL;

    if( sb == NULL ) {
        __error_state = ios::badbit;
    }
    __strmbuf = sb;
  }

}
