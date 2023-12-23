/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Build code for a selection (switch) statement.
*
****************************************************************************/


extern sel_handle   BGSelInit( void );
extern void         BGSelCase( sel_handle s_node, label_handle label, int_32 value );
extern void         BGSelRange( sel_handle s_node, int_32 lo, int_32 hi, label_handle label );
extern void         BGSelOther( sel_handle s_node, label_handle other );
extern void         BGSelect( sel_handle s_node, an node, cg_switch_type allowed );
extern int          SelCompare( int_32 lo1, int_32 lo2 );
extern cg_type      SelType( uint_32 value_range );
extern int_32       NumValues( select_list *list, int_32 hi );
extern void         FreeTable( tbl_control *table );
