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
* Description:  Inform CG what registers I/O return type touches
*
****************************************************************************/


#ifndef _IOTYPE_H_INCLUDED
#define _IOTYPE_H_INCLUDED

typedef PTYPE (io_type_rtn)(void);

#if defined( _M_IX86 )
#ifdef __386__
  #pragma aux io_type_decl modify [eax ebx ecx edx esi edi];
#else
  #if defined( __SMALL__ ) || defined( __MEDIUM__ ) || defined( __WINDOWS__ )
    #pragma aux io_type_decl modify [ax bx cx dx si di es];
  #else
    #pragma aux io_type_decl modify [ax bx cx dx si di ds es];
  #endif
#endif

#pragma aux ( io_type_rtn, io_type_decl )
#endif

extern  io_type_rtn             IOType;         // for optimizing compiler
extern  io_type_rtn             FC_IOType;      // for load'n go compiler

gbl_defn io_type_rtn            *IOTypeRtn;

#endif
