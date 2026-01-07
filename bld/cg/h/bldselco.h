/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023-2025 The Open Watcom Contributors. All Rights Reserved.
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


#if _TARGET_INTEL
#define MAX_COST        0x7FFFFFFF
#else
#define MAX_COST        0x7FFF
#endif

typedef int_32          cost_val;

extern cost_val         IfCost( sel_handle s_node, uint_32 );
extern name             *ScanCall( tbl_control *, name *, type_class_def );
extern cost_val         JumpCost( sel_handle s_node );
extern cost_val         ScanCost( sel_handle s_node );
extern tbl_control      *MakeScanTab( sel_handle s_node, cg_type value_type, cg_type real_type );
extern tbl_control      *MakeJmpTab( sel_handle s_node );
extern name             *SelIdx( tbl_control *, an );
extern const type_def   *SelNodeType( an, bool );
extern void             MkSelOp( name *idx, type_class_def type_class );
