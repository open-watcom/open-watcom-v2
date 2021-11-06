/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Take the instruction stream from the code generator and
*               writes the instructions into the object file.
*
****************************************************************************/


extern void             ReFormat( oc_class class );
extern void             AddByte( byte b );
extern void             AddToTemp( byte b );
extern void             EmitByte( byte b );
extern void             EmitPtr( pointer p );
extern void             EmitSegId( segment_id segid );
extern void             EmitOffset( offset i );
extern void             LayOpbyte( gen_opcode op );
extern void             LayOpword( gen_opcode op );
extern void             LayW( type_class_def type_class );
extern void             LayReg( hw_reg_set r );
extern void             LayRMRegOp( name *r );
extern void             LayRegRM( hw_reg_set r );
extern void             LayRegAC( hw_reg_set r );
extern void             GenSeg( hw_reg_set regs );
extern type_class_def   OpndSize( hw_reg_set reg );
extern void             QuickSave( hw_reg_set reg, opcode_defs op );
extern void             GenRegXor( hw_reg_set src, hw_reg_set dst );
extern void             GenRegNeg( hw_reg_set src );
extern void             GenRegMove( hw_reg_set src, hw_reg_set dst );
extern void             AddWData( signed_32 value, type_class_def type_class );
extern void             AddWCons( name *op, type_class_def type_class );
extern void             AddSData( signed_32 value, type_class_def type_class );
extern void             GenRegAdd( hw_reg_set dst, type_length value );
extern void             GenRegSub( hw_reg_set dst, type_length value );
extern void             GenRegAnd( hw_reg_set dst, type_length value );
extern void             GFldMorC( name *what );
extern void             GFldM( pointer what );
extern void             GFstpM( pointer what );
extern void             GFstp( int i );
extern void             GFxch( int i );
extern void             GFldz( void );
extern void             GFld( int i );
extern void             GCondFwait( void );
extern void             GFwait( void );
extern void             Gpusha( void );
extern void             Gpopa( void );
extern void             Gcld( void );
extern void             GenLeave( void );
extern void             GenTouchStack( bool sp_might_point_at_something );
extern void             GenEnter( int size, int level );
extern void             GenPushOffset( byte offset );
extern void             GenUnkSub( hw_reg_set dst, pointer value );
extern void             GenUnkMov( hw_reg_set dst, pointer value );
extern void             GenUnkEnter( pointer value, int level );
extern void             GenWindowsProlog( void );
extern void             GenCypWindowsProlog( void );
extern void             GenWindowsEpilog( void );
extern void             GenCypWindowsEpilog( void );
extern void             GenRdosdevProlog( void );
extern void             GenRdosdevEpilog( void );
extern void             GenLoadDS( void );

extern void             DoRepOp( instruction *ins );
extern void             Do4CXShift( instruction *ins, void (*rtn)(instruction *) );
extern void             LayLeaRegOp( instruction *ins );
extern void             DoMAddr( name *op );
extern byte             DoMDisp( name *op, bool alt_encoding );
extern void             LayModRM( name *op );
extern byte             Displacement( signed_32 val, hw_reg_set regs );
extern byte             DoIndex( hw_reg_set regs );
extern void             DoRelocConst( name *op, type_class_def type_class );
extern void             Do4Shift( instruction *ins );
extern void             Do4RShift( instruction *ins );
extern void             Gen4RNeg( instruction *ins );
extern void             Gen4Neg( instruction *ins );
extern void             By2Div( instruction *ins );
extern void             Pow2Div( instruction *ins );
extern void             GFstp10( type_length where );
extern void             GFld10( type_length where );

#if _TARGET & _TARG_8086
extern void             Pow2Div286(instruction*);
#else
extern type_length      TmpLoc( name *base, name *op );
extern void             GenUnkPush( pointer value );
extern void             GenPushC( signed_32 value );
extern pointer          GenFar16Thunk( pointer label, unsigned_16 parms_size, bool remove_parms );
extern void             GenP5ProfilingProlog( label_handle label );
extern void             GenP5ProfilingEpilog( label_handle label );
#if 0
extern void             GenProfilingCode( char *fe_name, label_handle *data, bool prolog );
#endif
#endif
