/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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


#ifndef DWLOC_H_INCLUDED
#define DWLOC_H_INCLUDED

extern void         DW_InitDebugLoc( dw_client );
extern void         DW_FiniDebugLoc( dw_client );
extern int          DW_IsLocExpr( dw_client, dw_loc_handle );
extern uint_32      DW_EmitLocExpr( dw_client, dw_sectnum, size_t, dw_loc_handle );
extern void         DW_EmitLocExprNull( dw_client, dw_sectnum, size_t );
extern uint_32      DW_EmitLoc( dw_client, dw_sectnum, dw_loc_handle );
extern uint_32      DW_EmitLocList( dw_client, dw_sectnum, dw_loc_handle );
extern uint_32      DW_EmitLocNull( dw_client cli, dw_sectnum sect );

#endif
