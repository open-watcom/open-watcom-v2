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
* Description:  C/F77 run-time libraries stack related variables declaration.
*
****************************************************************************/


extern  unsigned            _STACKLOW;
#if !defined( _NETWARE_LIBC )
extern  unsigned            _STACKTOP;
#endif

/* alternate stack for F77 compiler */
#if !defined( _M_I86 )
extern  unsigned            __ASTACKSIZ;
extern  char                *__ASTACKPTR;
#if defined( __WATCOMC__ ) && defined( _M_IX86 )
 #pragma aux                __ASTACKPTR "*"
 #pragma aux                __ASTACKSIZ "*"
#endif
#endif

#define _RWD_stacklow       _STACKLOW
#if !defined( _NETWARE_LIBC )
#define _RWD_stacktop       _STACKTOP
#endif
