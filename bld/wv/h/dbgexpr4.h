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
* Description:  Debugger expression handling, Part IV (Arithmetic).
*
****************************************************************************/


extern void DoPlus( void );
extern void DoMinus( void );
extern void DoMul( void );
extern void DoDiv( void );
extern void DoMod( void );
extern void DoAnd( void );
extern void DoOr( void );
extern void DoXor( void );
extern void DoShift( void );
extern void DoAddr( void );
extern void DoAPoints( stack_entry *stk, type_kind def );
extern void DoPoints( type_kind def );
extern void DoConvert( void );
extern void DoLConvert( void );
extern void DoMakeComplex( void );
extern void DoStringConcat( void );
extern void DoGivenField( sym_handle *member_hdl );
extern void DoField( void );
extern void DoScope( void );
extern void DoAssign( void );
extern void DoCall( unsigned num_parms, bool build_scbs );
#if 0
extern void InitReturnInfo( sym_handle *f, return_info *ri );
extern void PrepReturnInfo( sym_handle *f, return_info *ri );
extern void PushReturnInfo( sym_handle *f, return_info *ri );
#endif
