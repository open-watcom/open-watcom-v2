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
* Description:  Public interface to echoapi module.
*
****************************************************************************/


#ifndef __ECHOAPI__H__
#define __ECHOAPI__H__

#ifndef NDEBUG

#include "cgechoap.h"
#include "seldef.h"
#include "tree.h"

void        EchoAPI( const char *text, ... );
cg_name     EchoAPICgnameReturn( cg_name retn );
int         EchoAPIHexReturn( int retn );
int         EchoAPIIntReturn( int retn );
call_handle EchoAPICallHandleReturn( call_handle retn );
select_node *EchoAPISelHandleReturn( select_node *retn );
call_handle EchoAPITempHandleReturn( temp_handle retn );
cg_type     EchoAPICgtypeReturn( cg_type retn );

void        EchoAPIInit( void );
void        EchoAPIFini( void );

void        EchoAPICallBack( tn node, cg_callback rtn, callback_handle param, char *start_end );

void        hdlAddUnary( handle_type hdltype, tn ffihandle, tn old );
void        hdlAddBinary( handle_type hdltype, tn handle, tn old_l, tn old_r );
void        hdlAddTernary( handle_type hdltype, tn handle, tn old_t, tn old_l, tn old_r );

void        handleUseOnce( handle_type hdltype, use_info *useinfo );
void        hdlAllUsed( handle_type hdltype );
void        verifyNotUserType( cg_type type );

void        handleAdd( handle_type hdltype, use_info *useinfo );
void        handleExists( handle_type hdltype, use_info *useinfo );

#define hdlUseOnce( type, handle ) ( handleUseOnce( type, (use_info *)handle ) )
#define hdlAdd( type, handle ) ( handleAdd( type, (use_info *)handle ) )
#define hdlExists( type, handle ) ( handleExists( type, (use_info *)handle ) )

#endif // NDEBUG

#endif // __ECHOAPI__H__
