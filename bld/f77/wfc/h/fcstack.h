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
* Description:  Stack F-Code processor.
*
****************************************************************************/


extern void        InitStack( void );
extern cg_type     ArrayPtrType( sym_id sym );
extern cg_type     SymPtrType( sym_id sym );
extern void        XPush( cg_name cgname );
extern cg_name     SymIndex( sym_id sym, cg_name i );
extern cg_name     SymAddr( sym_id sym );
extern cg_name     SymValue( sym_id sym );
extern char        *StackBuffer( int * len );
extern void        DXPush( intstar4 val );
extern void        SymPush( sym_id val );
extern cg_name     XPop( void );
extern cg_name     XPopValue( cg_type typ );
extern cg_name     GetTypedValue( void );
extern cg_name     StkElement( int idx );
extern void        PopStkElements( int num );
extern intstar4    DXPop( void );
extern sym_id      SymPop( void );
extern cg_name     IntegerConstant( ftn_type *value, uint size );
