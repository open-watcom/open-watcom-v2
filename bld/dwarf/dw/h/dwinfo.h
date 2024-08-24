/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef DWINFO_H_INCLUDED
#define DWINFO_H_INCLUDED

extern void         DW_InitDebugInfo( dw_client );
extern void         DW_FiniDebugInfo( dw_client );

extern void         DW_InfoReloc( dw_client, uint );
extern void         DW_Info8( dw_client, uint_8 );
extern void         DW_Info16( dw_client, uint_16 );
extern void         DW_Info32( dw_client, uint_32 );
extern void         DW_InfoSLEB128( dw_client, dw_sconst );
extern void         DW_InfoULEB128( dw_client, dw_uconst );
extern void         DW_InfoBytes( dw_client, const void *, size_t );
extern void         DW_InfoString( dw_client, const char * );
extern uint_32      DW_InfoEmitLocExpr( dw_client, size_t, dw_loc_handle );
extern void         DW_InfoEmitLocExprNull( dw_client, size_t );
extern void         DW_InfoHandleWriteOffset( dw_client cli, dw_handle hdl );
extern void         DW_InfoHandleReference( dw_client cli, dw_handle hdl );

#endif
