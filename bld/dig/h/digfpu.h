/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef DIGFPU_H_INCLUDED
#define DIGFPU_H_INCLUDED

typedef enum {
    X86_NOFPU,
    X86_87,
    X86_287,
    X86_387,
    X86_487,
    X86_587,
    X86_687,
    X86_P47         = 15,
    X86_EMU         = 255
} x86_fputypes;

typedef enum {
    X64_FPU1        = 1
} x64_fputypes;

typedef union dig_fputypes {
    unsigned char   byte;
    x86_fputypes    x86;
    x64_fputypes    x64;
//    axp_fputypes    axp;
//    ppc_fputypes    ppc;
//    mips_fputypes   mips;
//    jvm_fputypes    jvm;
} dig_fputypes;

#endif
