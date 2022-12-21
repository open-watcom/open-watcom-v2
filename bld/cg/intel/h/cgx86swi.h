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
* Description:  Target dependent code generation switches for x86.
*
****************************************************************************/


typedef enum {
    #include "x86swi.h"
} cg_target_switches;

typedef enum {
    /* CPU revisions */
    CPU_86      = 0x0000,
    CPU_186     = 0x0001,
    CPU_286     = 0x0002,
    CPU_386     = 0x0003,
    CPU_486     = 0x0004,
    CPU_586     = 0x0005,
    CPU_686     = 0x0006,
    /* 8087 revisions */
    FPU_NONE    = 0x0000,
    FPU_87      = 0x0010,
    FPU_387     = 0x0020,
    FPU_586     = 0x0030,
    FPU_686     = 0x0040,
    FPU_EMU     = 0x0080,
    FPU_E87     = FPU_EMU + FPU_87,
    FPU_E387    = FPU_EMU + FPU_387,
    FPU_E586    = FPU_EMU + FPU_586,
    FPU_E686    = FPU_EMU + FPU_686,
    /* Weitek revisions */
    WTK_NONE    = 0x0000,
    WTK_1167    = 0x0100,
    WTK_3167    = 0x0200,
    WTK_4167    = 0x0300,
} proc_revision;

#define CPU_MASK        0x000f
#define FPU_MASK        0x00f0
#define WTK_MASK        0x0f00

#define GET_CPU( r )       ((r) & CPU_MASK)
#define GET_FPU( r )       ((r) & FPU_MASK)
#define GET_FPU_LEVEL( r ) (((r) & FPU_MASK)&~FPU_EMU)
#define GET_WTK( r )       ((r) & WTK_MASK)

#define SET_PROC( r, v, m ) \
    { \
        proc_revision   new; \
        new = (v); \
        (r) &= ~(m); \
        (r) |= new; \
    }
#define SET_CPU( r, v ) SET_PROC( r, v, CPU_MASK );
#define SET_FPU( r, v ) SET_PROC( r, v, FPU_MASK );
#define SET_WTK( r, v ) SET_PROC( r, v, WTK_MASK );

#define SET_FPU_LEVEL( r, v ) SET_FPU( r, (v) | ( GET_FPU( r ) & FPU_EMU ) )
#define SET_FPU_EMU( r )      SET_FPU( r, GET_FPU( r ) | FPU_EMU )
#define SET_FPU_INLINE( r )   SET_FPU( r, GET_FPU( r ) & ~FPU_EMU )

#define GET_FPU_EMU( r )      ( ( GET_FPU( r ) & FPU_EMU ) != FPU_NONE )
