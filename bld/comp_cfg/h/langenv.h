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
* Description:  Establish common stuff for a target environment for
*               language-related processors.
*
****************************************************************************/


#ifndef __LANGENV_H__
#define __LANGENV_H__

// This file uses __TGT_SYS to figure out the required target.
//
//      langenvd.h contains the possible values.
//
//      __TGT_SYS_X86 is the default
//

#include "langenvd.h"

#ifndef __TGT_SYS
    #define __TGT_SYS __TGT_SYS_X86
#endif

#if __TGT_SYS == __TGT_SYS_X86

    #define TS_SEG_CODE     "_TEXT"
    #define TS_SEG_CONST    "CONST"
    #define TS_SEG_CONST2   "CONST2"
    #define TS_SEG_DATA     "_DATA"
    #define TS_SEG_TIB      "TIB"
    #define TS_SEG_TI       "TI"
    #define TS_SEG_TIE      "TIE"
    #define TS_SEG_XIB      "XIB"
    #define TS_SEG_XI       "XI"
    #define TS_SEG_XIE      "XIE"
    #define TS_SEG_YIB      "YIB"
    #define TS_SEG_YI       "YI"
    #define TS_SEG_YIE      "YIE"
    #define TS_SEG_YC       "YC"
    #define TS_SEG_BSS      "_BSS"
    #define TS_SEG_STACK    "STACK"
    #define TS_SEG_DEPENDS  "not used"
    #define TS_SEG_TLSB     ".tls"
    #define TS_SEG_TLS      ".tls$"
    #define TS_SEG_TLSE     ".tls$ZZZ"
    #define TS_SEG_TLS_CLASS        "TLS"

    #define TS_MAX_OBJNAME      256
    #define TS_DATA_MANGLE      "_*"
    #define TS_CODE_MANGLE      "*_"

#elif __TGT_SYS == __TGT_SYS_AXP_NT || __TGT_SYS == __TGT_SYS_PPC_NT || __TGT_SYS == __TGT_SYS_MIPS

    #define TS_SEG_CODE     ".text"
    #define TS_SEG_CONST    ".const"
    #define TS_SEG_CONST2   ".const2"
    #define TS_SEG_DATA     ".data"
    #define TS_SEG_TIB      ".rtl$tib"
    #define TS_SEG_TI       ".rtl$tid"
    #define TS_SEG_TIE      ".rtl$tie"
    #define TS_SEG_XIB      ".rtl$xib"
    #define TS_SEG_XI       ".rtl$xid"
    #define TS_SEG_XIE      ".rtl$xie"
    #define TS_SEG_YIB      ".rtl$yib"
    #define TS_SEG_YI       ".rtl$yid"
    #define TS_SEG_YIE      ".rtl$yie"
    #define TS_SEG_YC       ".rtl$yc"
    #define TS_SEG_BSS      ".bss"
    #define TS_SEG_STACK    ".stack"
    #define TS_SEG_DEPENDS  ".depend"
    #define TS_SEG_TLSB     ".tls"
    #define TS_SEG_TLS      ".tls$"
    #define TS_SEG_TLSE     ".tls$ZZZ"
    #define TS_SEG_TLS_CLASS        NULL

    #define TS_MAX_OBJNAME      1024
    #define TS_DATA_MANGLE      "*"
    #define TS_CODE_MANGLE      "*"

#else

    #error Invalid target system

#endif

#undef __TGT_SYS

#endif
