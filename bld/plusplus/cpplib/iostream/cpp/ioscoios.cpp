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

  // Construct an "ios" and initialize it. New "ios" gets all fields of
  // "is" except streambuf.

  ios::ios( const ios &is ) {

    __lock_it( is.__i_lock );
    #ifdef __MT__
        __i_lock =  __get_next_ios_lock();
    #else
        __i_lock = NULL;
    #endif
    __error_state        = is.__error_state;
    __format_flags       = is.__format_flags;
    __enabled_exceptions = is.__enabled_exceptions;
    __float_precision    = is.__float_precision;
    __field_width        = is.__field_width;
    __fill_character     = is.__fill_character;
    __tied_stream        = is.__tied_stream;
    __xalloc_list        = NULL;
    __strmbuf            = NULL;
  }

}
