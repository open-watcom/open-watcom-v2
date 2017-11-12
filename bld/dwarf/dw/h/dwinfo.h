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


#ifndef DWINFO_H_INCLUDED
#define DWINFO_H_INCLUDED

#define InitDebugInfo           DW_InitDebugInfo
#define FiniDebugInfo           DW_FiniDebugInfo
#define InfoAllocate            DW_InfoAllocate
#define InfoReloc               DW_InfoReloc
#define Info8                   DW_Info8
#define Info16                  DW_Info16
#define Info32                  DW_Info32
#define InfoLEB128              DW_InfoLEB128
#define InfoULEB128             DW_InfoULEB128
#define InfoBytes               DW_InfoBytes
#define InfoString              DW_InfoString
#define InfoEmitLocExpr         DW_InfoEmitLocExpr
#define InfoEmitLocExprNull     DW_InfoEmitLocExprNull
#define InfoHandleWriteOffset   DW_InfoHandleWriteOffset
#define InfoHandleReference     DW_InfoHandleReference

extern void         InitDebugInfo( dw_client );
extern void         FiniDebugInfo( dw_client );

extern void         InfoReloc( dw_client, uint );
extern void         Info8( dw_client, uint_8 );
extern void         Info16( dw_client, uint_16 );
extern void         Info32( dw_client, uint_32 );
extern void         InfoLEB128( dw_client, dw_sconst );
extern void         InfoULEB128( dw_client, dw_uconst );
extern void         InfoBytes( dw_client, const void *, size_t );
extern void         InfoString( dw_client, const char * );
extern uint_32      InfoEmitLocExpr( dw_client, size_t, dw_loc_handle );
extern void         InfoEmitLocExprNull( dw_client, size_t );
extern void         InfoHandleWriteOffset( dw_client cli, dw_handle hdl );
extern void         InfoHandleReference( dw_client cli, dw_handle hdl );

#endif
