/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Inspect a symbol.
*
****************************************************************************/


extern void     WndTmpFileInspect( const char *file );
extern void     WndFileInspect( const char *file, bool binary );
extern void     WndFuncInspect( mod_handle mod );
extern void     WndGblFuncInspect( mod_handle mod );
extern void     WndGblVarInspect( mod_handle mod );
extern void     WndMemInspect( address addr, char *next, unsigned len, mad_type_handle mth );
extern void     WndIOInspect( address *addr, mad_type_handle mth );
extern void     WndAddrInspect( address addr );
extern void     WndModListInspect( mod_handle mod );
extern void     WndInspectExprSP( const char *item );
extern void     WndInspect( const char *item );
extern void     WndVarInspect( const char *buff );
extern void     WndAsmInspect( address addr );
extern void     WndSrcInspect( address addr );
extern void     WndSrcOrAsmInspect( address addr );
extern void     WndNewSrcInspect( address addr );
