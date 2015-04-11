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


#ifndef _XCONIO_H_INCLUDED
#define _XCONIO_H_INCLUDED

#include <conio.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__INLINE_FUNCTIONS__) && defined(_M_IX86)
  extern unsigned _inline_inp(unsigned __port);
  extern unsigned _inline_inpw(unsigned __port);
  extern unsigned _inline_outp(unsigned __port, unsigned __value);
  extern unsigned _inline_outpw(unsigned __port,unsigned __value);
 #ifdef __386__
  extern unsigned _inline_inpd(unsigned __port);
  extern unsigned _inline_outpd(unsigned __port,unsigned __value);
 #endif
#endif

#ifdef __cplusplus
};
#endif
#endif
