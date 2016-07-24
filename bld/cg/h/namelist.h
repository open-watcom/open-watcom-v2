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
* Description:  Prototypes for namelist.c
*
****************************************************************************/


extern name             *AllocConst( float_handle value );
extern name             *AllocAddrConst( name *value, int seg, constant_class class, type_class_def name_class );
extern name             *FindIntValue( signed_32 value );
extern name             *AllocIntConst( int value );
extern name             *AllocUIntConst( uint value );
extern name             *AllocS32Const( signed_32 value );
extern name             *AllocU32Const( unsigned_32 value );
extern name             *AllocS64Const( unsigned_32 low, unsigned_32 high );
extern name             *AllocU64Const( unsigned_32 low, unsigned_32 high );
extern constant_defn    *GetFloat( name *cons, type_class_def class );
extern memory_name      *SAllocMemory( pointer symbol, type_length offset, cg_class class, type_class_def nclass, type_length size );
extern name             *AllocMemory( pointer symbol, type_length offset, cg_class class, type_class_def type_class );
extern name             *STempOffset( name *temp, type_length offset, type_class_def class, type_length size );
extern name             *SAllocTemp( type_class_def class, type_length size );
extern name             *AllocTemp( type_class_def class );
extern name             *TempOffset( name *temp, type_length offset, type_class_def class );
extern name             *SAllocUserTemp( pointer symbol, type_class_def class, type_length size );
extern name             *AllocUserTemp( pointer symbol, type_class_def class );
extern name             *DeAlias( name *temp );
extern name             *AllocRegName( hw_reg_set regs );
extern name             *ScaleIndex( name *index, name *base, type_length offset, type_class_def class, type_length size, int scale, i_flags flags );
extern name             *SAllocIndex( name *index, name *base, type_length offset, type_class_def class, type_length size );
extern name             *AllocIndex( name *index, name *base, type_length offset, type_class_def class );
extern void             InitNames( void );
extern void             ReInitNames( void );
extern void             FreeNames( void );
extern void             FreeAName( name *op );
extern bool             NameFrlFree( void );
extern i_flags          AlignmentToFlags( type_length alignment );
extern type_length      FlagsToAlignment( i_flags flags );
extern void             SetArchIndex( name *new_r, hw_reg_set regs );
