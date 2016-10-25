/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Alpha AXP instruction encoding.
*
****************************************************************************/


extern void         GenCallLabelReg( pointer label, uint reg );
extern void         GenOPINS( uint_8 opcode, uint_8 function, uint_8 reg_a, uint_8 reg_b, uint_8 reg_c );
extern void         GenLOADS32( signed_32 value, uint_8 reg );
extern void         GenMEMINSRELOC( uint_8 opcode, uint_8 a, uint_8 b, signed_16 displacement, pointer lbl, owl_reloc_type type );
extern void         GenMEMINS( uint_8 opcode, uint_8 a, uint_8 b, signed_16 displacement );
extern void         GenLOAD( hw_reg_set dst, hw_reg_set src, signed_16 displacement );
extern void         GenFLOAD( hw_reg_set dst, hw_reg_set src, signed_16 displacement );
extern void         GenSTORE( hw_reg_set dst, signed_16 displacement, hw_reg_set src );
extern void         GenFSTORE( hw_reg_set dst, signed_16 displacement, hw_reg_set src );
extern void         GenRET( void );
#if 0
extern void         GenJumpIf( instruction *ins, pointer label );
#endif
