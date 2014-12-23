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
* Description:  Prototypes for front end callbacks.
*
****************************************************************************/


CGCALLBACKDEF( FESegID,         segment_id,     (cg_sym_handle) )
CGCALLBACKDEF( FEBack,          back_handle,    (cg_sym_handle) )
CGCALLBACKDEF( FEAttr,          fe_attr,        (cg_sym_handle) )
CGCALLBACKDEF( FEAuxInfo,       pointer,        (pointer,int) )
CGCALLBACKDEF( FEName,          cchar_ptr,      (cg_sym_handle) )
CGCALLBACKDEF( FEModuleName,    cchar_ptr,      (void) )
CGCALLBACKDEF( FEMessage,       void,           (int,pointer) )
CGCALLBACKDEF( FEMoreMem,       int,            (unsigned) )
CGCALLBACKDEF( FETrue,          int,            (void) )
CGCALLBACKDEF( FEGenProc,       void,           (cg_sym_handle,call_handle) )
CGCALLBACKDEF( FELexLevel,      int,            (cg_sym_handle) )
CGCALLBACKDEF( FEDbgType,       dbg_type,       (cg_sym_handle) )
CGCALLBACKDEF( FEDbgRetType,    dbg_type,       (cg_sym_handle) )
CGCALLBACKDEF( FEStackChk,      int,            (cg_sym_handle) )
CGCALLBACKDEF( FEGetEnv,        cchar_ptr,      (cchar_ptr) )
CGCALLBACKDEF( FEExtName,       cchar_ptr,      (cg_sym_handle,int) )
CGCALLBACKDEF( FEParmType,      cg_type,        (cg_sym_handle,cg_sym_handle,cg_type) )
