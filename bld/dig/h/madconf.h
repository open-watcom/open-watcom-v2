/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2022-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Target architecture related declarations (MAD).
*
****************************************************************************/


#ifndef MADCONF_H_INCLUDED
#define MADCONF_H_INCLUDED

#define MADARCH_NONE     0x00
#define MADARCH_AXP      0x01
#define MADARCH_PPC      0x02
#define MADARCH_X86      0x04
#define MADARCH_X64      0x08
#define MADARCH_MIPS     0x10
#define MADARCH_JVM      0x20

#if defined( DIGAXP )
#define MADARCH MADARCH_AXP
#elif defined( DIGPPC )
#define MADARCH MADARCH_PPC
#elif defined( DIGX86 )
#define MADARCH MADARCH_X86
#elif defined( DIGX64 )
#define MADARCH MADARCH_X64
#elif defined( DIGJVM )
#define MADARCH MADARCH_JVM
#elif defined( DIGMIPS )
#define MADARCH MADARCH_MIPS
#elif defined( DIGALL )
#define MADARCH (MADARCH_AXP | MADARCH_MIPS | MADARCH_PPC | MADARCH_X86 | MADARCH_X64)
#endif

#ifndef MADARCH
#error "MAD: undefined/missing/incorrect DIG Architecture"
#endif

#endif
