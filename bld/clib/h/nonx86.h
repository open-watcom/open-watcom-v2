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
* Description:  Macros to ensure that x86-specific tokens are never seen
*               when compiling libraries for non-x86 platforms.
*
****************************************************************************/


#ifndef _NONX86_H_INCLUDED
#define _NONX86_H_INCLUDED
#undef __far
#define __far fubar
#undef __far16
#define __far16 fubar
#undef __seg16
#define __seg16 fubar
#undef __huge
#define __huge fubar
#undef __loadds
#define __loadds fubar
#undef __near
#define __near fubar
#undef __packed
#define __packed fubar
#undef __pascal
#define __pascal fubar
#undef __segment
#define __segment fubar
#endif
