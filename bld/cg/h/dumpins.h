/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Dump instruction.
*
****************************************************************************/


extern void     DumpInsOffsets( void );
extern void     DumpInOut( instruction *ins );
extern void     DumpITab( instruction *ins );
extern void     DumpClass( type_class_def type_class );
extern void     DumpOperand( name *operand );
extern void     DoDumpIInfo( instruction *ins, bool fp );
extern void     DumpFPInfo( instruction *ins );
extern void     DumpIInfo( instruction *ins );
extern void     DumpInsOnly( instruction *ins );
extern void     DumpLineNum( instruction *ins );
extern void     DumpInsNoNL( instruction *ins );
extern void     DumpIns( instruction *ins );
extern void     DumpInstrsOnly( block *blk );
extern void     DumpCond( instruction *ins, block *blk );
extern void     DumpVUsage( name *v );
extern void     DumpSym( name *sym );
extern void     DumpTempWId( int id );
extern void     DumpSymList( name *sym );
extern void     DumpNTemp( void );
extern void     DumpNMemory( void );
extern void     DumpNIndexed( void );
extern void     DumpNConst( void );
extern void     DumpNRegister( void );
extern void     DumpInsList( block *blk );
