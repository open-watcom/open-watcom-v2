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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <errno.h>
#include <iostream>
#endif
#include <stdlib.h>
#include "rtinit.h"
#include "iofhdr.h"
#include "fltsupp.h"

extern "C" {

static void __setiofEFGfmt( void ) {
#ifndef IN_SLIB
 #ifdef __SW_BR
    __EFG_cnvs2d  = __get_EFG_cnvs2d();
    __EFG_cnvd2f  = __get_EFG_cnvd2f();
    __EFG_cvtld   = __get_EFG_cvtld();
  #ifdef _LONG_DOUBLE_
    __EFG_cnvd2ld = __get_EFG_cnvd2ld();
  #endif
 #else
    __EFG_cnvs2d  = __cnvs2d;
    __EFG_cnvd2f  = __cnvd2f;
    __EFG_cvtld   = __cvtld;
  #ifdef _LONG_DOUBLE_
    __EFG_cnvd2ld = __cnvd2ld;
  #endif
 #endif
#endif
}

    XI( __ppfltused_, __setiofEFGfmt, INIT_PRIORITY_LIBRARY )
#if !defined(__AXP__)
    #pragma aux __ppfltused_ "*";
#endif
}
