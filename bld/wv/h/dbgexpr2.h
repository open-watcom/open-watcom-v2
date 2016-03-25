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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


extern bool ClassifyType( location_context *lc, type_handle *th, dip_type_info *info );
extern void ClassifyEntry( stack_entry *stk, dip_type_info *info );
extern sym_list *ExprGetSymList( stack_entry *entry, bool source_only );
extern void ExprSymbol( stack_entry *entry, sym_handle *sh );
extern bool NameResolve( stack_entry *entry, bool source_only );
extern void SymResolve( stack_entry *entry );
extern void LValue( stack_entry *entry );
extern void ClassNum( stack_entry *entry );
extern void ExprResolve( stack_entry *entry );
extern void LRValue( stack_entry *entry );
extern void RValue( stack_entry *entry );
extern void ExprValue( stack_entry *entry );
