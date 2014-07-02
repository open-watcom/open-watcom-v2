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

#ifndef __PDEFN2_H
#define __PDEFN2_H

#if _CPU == 370
global  linkage_regs    DefaultLinkage;
global  linkage_regs    OSLinkage;
#endif
global  hw_reg_set      DefaultParms[];
global  hw_reg_set      CdeclParms[];
global  hw_reg_set      PascalParms[];
global  hw_reg_set      DefaultVarParms[];
global  hw_reg_set      RegBits[];
global  char            Registers[];
global  unsigned char   RegMap[];
global  aux_info        InlineInfo;
global  inline_funcs    SInline_Functions[];
global  inline_funcs    Inline_Functions[];
#if _CPU == 8086
global  inline_funcs    ZF_Data_Functions[];
global  inline_funcs    ZP_Data_Functions[];
global  inline_funcs    DF_Data_Functions[];
global  inline_funcs    DP_Data_Functions[];
#else
global  inline_funcs    Flat_Functions[];
global  inline_funcs    BigData_Functions[];
global  inline_funcs    BigDataNoDs_Functions[];
global  inline_funcs    SBigData_Functions[];
global  inline_funcs    SBigDataNoDs_Functions[];
#endif
global  inline_funcs    Common_Functions[];
global  inline_funcs    _8087_Functions[];

#endif
