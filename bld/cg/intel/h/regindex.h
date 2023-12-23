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
#define pick1(id,idx,cls,s1,s2)     pick_start(s1) = pick_item(id), pick_start(s2) = pick_item(id),
#define pick2(id,idx,cls,e1,s1)     pick_end(e1) = pick_item(id), pick_start(s1) = pick_item(id),
#define pick3(id,idx,cls,e1,e2,s1)  pick_end(e1) = pick_item(id), pick_end(e2) = pick_item(id), pick_start(s1) = pick_item(id),
#define pick4(id,e1)                pick_end(e1) = pick_item(id),
#else
#define pick0(id,idx,cls)           pick(id,idx,cls)
#define pick1(id,idx,cls,s1,s2)     pick(id,idx,cls)
#define pick2(id,idx,cls,e1,s1)     pick(id,idx,cls)
#define pick3(id,idx,cls,e1,e2,s1)  pick(id,idx,cls)
#define pick4(id,e1)
#endif

/*    id  idx  cls */
pick1( AL,  0,  GPR, GPR, BYTE )
pick0( CL,  1,  GPR )
pick0( DL,  2,  GPR )
pick0( BL,  3,  GPR )
pick0( AH,  4,  GPR )
pick0( CH,  5,  GPR )
pick0( DH,  6,  GPR )
pick0( BH,  7,  GPR )
pick2( AX,  0,  GPR, BYTE, WORD )
pick0( CX,  1,  GPR )
pick0( DX,  2,  GPR )
pick0( BX,  3,  GPR )
pick0( SP,  4,  GPR )
pick0( BP,  5,  GPR )
pick0( SI,  6,  GPR )
pick0( DI,  7,  GPR )
pick2( EAX, 0,  GPR, WORD, DWORD )
pick0( ECX, 1,  GPR )
pick0( EDX, 2,  GPR )
pick0( EBX, 3,  GPR )
pick0( ESP, 4,  GPR )
pick0( EBP, 5,  GPR )
pick0( ESI, 6,  GPR )
pick0( EDI, 7,  GPR )
pick3( ST0, 0,  FPR, DWORD, GPR, FPR )
pick0( ST1, 1,  FPR )
pick0( ST2, 2,  FPR )
pick0( ST3, 3,  FPR )
pick0( ST4, 4,  FPR )
pick0( ST5, 5,  FPR )
pick0( ST6, 6,  FPR )
pick0( ST7, 7,  FPR )
pick2( ES,  0,  SEG, FPR, SEG )
pick0( CS,  1,  SEG )
pick0( SS,  2,  SEG )
pick0( DS,  3,  SEG )
pick0( FS,  4,  SEG )
pick0( GS,  5,  SEG )
pick4( END, SEG )

#undef pick0
#undef pick1
#undef pick2
#undef pick3
#undef pick4
