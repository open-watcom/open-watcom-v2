/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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


#ifdef MAPREGCLASS
#define pick0(id,idx,cls)
#define pick1(id,idx,cls,s1)        pick_start(s1) = pick_item(id),
#define pick2(id,idx,cls,e1,s1)     pick_end(e1) = pick_item(id), pick_start(s1) = pick_item(id),
#define pick3(id,e1)                pick_end(e1) = pick_item(id),
#else
#define pick0(id,idx,cls)           pick(id,idx,cls)
#define pick1(id,idx,cls,s1)        pick(id,idx,cls)
#define pick2(id,idx,cls,e1,s1)     pick(id,idx,cls)
#define pick3(id,e1)
#endif

/*    id  idx  cls */
pick1( R0,  0,  GPR, GPR )
pick0( R1,  1,  GPR )
pick0( R2,  2,  GPR )
pick0( R3,  3,  GPR )
pick0( R4,  4,  GPR )
pick0( R5,  5,  GPR )
pick0( R6,  6,  GPR )
pick0( R7,  7,  GPR )
pick0( R8,  8,  GPR )
pick0( R9,  9,  GPR )
pick0( R10, 10, GPR )
pick0( R11, 11, GPR )
pick0( R12, 12, GPR )
pick0( R13, 13, GPR )
pick0( R14, 14, GPR )
pick0( R15, 15, GPR )
pick0( R16, 16, GPR )
pick0( R17, 17, GPR )
pick0( R18, 18, GPR )
pick0( R19, 19, GPR )
pick0( R20, 20, GPR )
pick0( R21, 21, GPR )
pick0( R22, 22, GPR )
pick0( R23, 23, GPR )
pick0( R24, 24, GPR )
pick0( R25, 25, GPR )
pick0( R26, 26, GPR )
pick0( R27, 27, GPR )
pick0( R28, 28, GPR )
pick0( R29, 29, GPR )
pick0( R30, 30, GPR )
pick0( R31, 31, GPR )
pick2( F0,  0,  FPR, GPR, FPR )
pick0( F1,  1,  FPR )
pick0( F2,  2,  FPR )
pick0( F3,  3,  FPR )
pick0( F4,  4,  FPR )
pick0( F5,  5,  FPR )
pick0( F6,  6,  FPR )
pick0( F7,  7,  FPR )
pick0( F8,  8,  FPR )
pick0( F9,  9,  FPR )
pick0( F10, 10, FPR )
pick0( F11, 11, FPR )
pick0( F12, 12, FPR )
pick0( F13, 13, FPR )
pick0( F14, 14, FPR )
pick0( F15, 15, FPR )
pick0( F16, 16, FPR )
pick0( F17, 17, FPR )
pick0( F18, 18, FPR )
pick0( F19, 19, FPR )
pick0( F20, 20, FPR )
pick0( F21, 21, FPR )
pick0( F22, 22, FPR )
pick0( F23, 23, FPR )
pick0( F24, 24, FPR )
pick0( F25, 25, FPR )
pick0( F26, 26, FPR )
pick0( F27, 27, FPR )
pick0( F28, 28, FPR )
pick0( F29, 29, FPR )
pick0( F30, 30, FPR )
pick0( F31, 31, FPR )
pick3( END, FPR )

#undef pick0
#undef pick1
#undef pick2
#undef pick3
