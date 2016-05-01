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


#ifndef DWLOC_H_INCLUDED
#define DWLOC_H_INCLUDED


#define InitDebugLoc            DW_InitDebugLoc
#define FiniDebugLoc            DW_FiniDebugLoc
#define IsLocExpr               DW_IsLocExpr
#define EmitLocExpr             DW_EmitLocExpr
#define EmitLocExprNull         DW_EmitLocExprNull
#define EmitLocList             DW_EmitLocList
#define EmitLoc                 DW_EmitLoc
#define EmitLocNull             DW_EmitLocNull

void            InitDebugLoc( dw_client );
void            FiniDebugLoc( dw_client );
int             IsLocExpr( dw_client, dw_loc_handle );
uint_32         EmitLocExpr( dw_client, uint, size_t, dw_loc_handle );
void            EmitLocExprNull( dw_client, uint, size_t );
uint_32         EmitLoc( dw_client, uint, dw_loc_handle );
uint_32         EmitLocList( dw_client, uint, dw_loc_handle );
uint_32         EmitLocNull( dw_client cli, uint sect);

#endif
