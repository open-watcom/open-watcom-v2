/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Target processor platform related declarations.
*
****************************************************************************/


#ifndef DISCONF_H_INCLUDED
#define DISCONF_H_INCLUDED

#define DISCPU_NONE     0x00
#define DISCPU_AXP      0x01
#define DISCPU_PPC      0x02
#define DISCPU_X86      0x04
#define DISCPU_JVM      0x08
#define DISCPU_SPARC    0x10
#define DISCPU_MIPS     0x20
#define DISCPU_X64      0x40

#if defined( DIGAXP )
#define DISCPU DISCPU_AXP
#elif defined( DIGPPC )
#define DISCPU DISCPU_PPC
#elif defined( DIGX86 )
#define DISCPU DISCPU_X86
#elif defined( DIGX64 )
#define DISCPU DISCPU_X64
#elif defined( DIGJVM )
#define DISCPU DISCPU_JVM
#elif defined( DIGSPARC )
#define DISCPU DISCPU_SPARC
#elif defined( DIGMIPS )
#define DISCPU DISCPU_MIPS
#elif defined( DIGTEST )
#define DISCPU ( DISCPU_AXP | DISCPU_PPC | DISCPU_X86 | DISCPU_JVM | DISCPU_SPARC | DISCPU_MIPS | DISCPU_X64 )
#elif defined( DIGALL )
#define DISCPU ( DISCPU_AXP | DISCPU_PPC | DISCPU_X86 | DISCPU_SPARC | DISCPU_MIPS | DISCPU_X64 )
#endif

#ifndef DISCPU
#error "DIS: undefined/missing/incorrect DIG Architecture"
#endif

#endif

