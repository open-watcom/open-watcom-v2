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
* Description:  MIPS instruction encoding.
*
****************************************************************************/


extern void         GenCallLabelReg( pointer label, uint reg );
#if 0
extern void         EmitInsReloc( mips_ins ins, pointer sym, owl_reloc_type type );
extern void         GenLOADS32( signed_32 value, uint_8 reg );
extern void         GenMEMINSRELOC( uint_8 opcode, uint_8 rt, uint_8 rs, signed_16 displacement, pointer lbl, owl_reloc_type type );
extern void         GenMEMINS( uint_8 opcode, uint_8 a, uint_8 b, signed_16 displacement );
extern void         GenIType( uint_8 opcode, uint_8 rt, uint_8 rs, signed_16 immed );
extern void         GenRType( uint_8 opcode, uint_8 fc, uint_8 rd, uint_8 rs, uint_8 rt );
extern void         GenIShift( uint_8 fc, uint_8 rd, uint_8 rt, uint_8 sa );
extern void         GenJType( uint_8 opcode, pointer label );
extern void         GenRET( void );
extern type_length  TempLocation( name *temp );
extern void         GenObjCode( instruction *ins );
extern void         GenLabelReturn( void );
extern byte         ReverseCondition( byte cond );
extern byte         CondCode( instruction *ins );
#endif
