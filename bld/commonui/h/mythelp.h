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
* Description:  ToolHelp wrappers.
*
****************************************************************************/


#ifndef _MYTHELP_H_INCLUDED
#define _MYTHELP_H_INCLUDED

BOOL    MyModuleFindHandle( MODULEENTRY *me, HMODULE h );
BOOL    MyStackTraceNext( STACKTRACEENTRY *se );
BOOL    MyTaskFindHandle( TASKENTRY *te, HTASK h );
BOOL    MyTaskFirst( TASKENTRY *te );
BOOL    MyTaskNext( TASKENTRY *te );
BOOL    MyStackTraceCSIPFirst( STACKTRACEENTRY *se, WORD ss, WORD cs, WORD ip, WORD bp );
BOOL    MyStackTraceFirst( STACKTRACEENTRY *se, HANDLE h );
BOOL    MyGlobalFirst( GLOBALENTRY *ge, WORD flags );
BOOL    MyGlobalNext( GLOBALENTRY *ge, WORD flags );
BOOL    MyGlobalEntryModule( GLOBALENTRY *ge, HMODULE hmod, WORD seg );
BOOL    MyGlobalEntryHandle( GLOBALENTRY *ge, HMODULE hmem );

#endif /* _MYTHELP_H_INCLUDED */
