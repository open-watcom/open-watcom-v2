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
* Description:  Top layer of expression evaluator.
*
****************************************************************************/


extern void     InitLC( location_context *new, bool use_real_regs );
extern void     CreateLC( stack_entry *entry );
extern void     FreeLC( stack_entry *entry );
extern void     MoveLC( stack_entry *src, stack_entry *dst );
extern void     DupLC( stack_entry *entry );
extern void     CreateEntry( void );
extern bool     AllocatedString( stack_entry *stk );
extern void     DeleteEntry( stack_entry *old );
extern void     MoveSP( int amount );
extern void     SwapStack( int entry );
extern void     FreezeStack( void );
extern void     UnFreezeStack( bool nuke_top );
extern char     *DupStringVal( stack_entry *stk );
extern void     DupStack( void );
extern void     PushName( lookup_item *li );
extern void     PushNum( long val );
extern void     PushNum64( unsigned_64 val );
extern void     PushRealNum( xreal val );
extern void     PushSymHandle( sym_handle *sh );
extern void     ExprSetAddrInfo( stack_entry *stk, bool trunc );
extern void     PushAddr( address addr );
extern void     PushLocation( location_list *ll, dip_type_info *ti );
extern void     CombineEntries( stack_entry *dest, stack_entry *l, stack_entry *r );
extern void     MoveTH( stack_entry *old, stack_entry *new );
extern void     PushType( type_handle *th );
extern void     PushInt( int val );
extern void     PushString( void );
extern void     PopEntry( void );
extern unsigned TstEQ( unsigned true_value );
extern unsigned TstLT( unsigned true_value );
extern unsigned TstTrue( unsigned true_value );
extern unsigned TstExist( unsigned true_value );
extern void     MakeAddr( void );
extern void     FreePgmStack( bool freeall );
extern void     ExprPurge( void );
extern void     MarkArrayOrder( bool column_major );
extern void     StartSubscript( void );
extern void     AddSubscript( void );
extern void     EndSubscript( void );
