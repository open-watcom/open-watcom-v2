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
* Description:  Wrappers for ToolHelp DLL routines.
*
****************************************************************************/


#include "precomp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * MyModuleFindHandle - wrapper for ModuleFindHandle
 */
BOOL MyModuleFindHandle( MODULEENTRY FAR* me, HMODULE h )
{
    BOOL        rc;

    me->dwSize = sizeof( MODULEENTRY );
    rc = (ModuleFindHandle( me, h ) != NULL);
    me->szModule[MAX_MODULE_NAME] = 0;
    return( rc );

} /* MyModuleFindHandle */

/*
 * MyStackTraceNext - wrapper for StackTraceNext
 */
BOOL MyStackTraceNext( STACKTRACEENTRY *se )
{
    se->dwSize = sizeof( STACKTRACEENTRY );
    return( StackTraceNext( se ) );

} /* MyStackTraceNext */

/*
 * MyTaskFindHandle - wrapper for TaskFindHandle
 */
BOOL MyTaskFindHandle( TASKENTRY *te, HTASK h )
{
    te->dwSize = sizeof( TASKENTRY );
    return( TaskFindHandle( te, h ) );

} /* MyTaskFindHandle */

/*
 * MyTaskFirst - wrapper for TaskFirst
 */
BOOL MyTaskFirst( TASKENTRY *te )
{
    te->dwSize = sizeof( TASKENTRY );
    return( TaskFirst( te ) );

} /* MyTaskFirst */

/*
 * MyTaskNext - wrapper for TaskNext
 */
BOOL MyTaskNext( TASKENTRY *te )
{
    te->dwSize = sizeof( TASKENTRY );
    return( TaskNext( te ) );

} /* MyTaskNext */

/*
 * MyStackTraceCSIPFirst - wrapper for StackTraceCSIPFirst
 */
BOOL MyStackTraceCSIPFirst( STACKTRACEENTRY *se, WORD ss, WORD cs, WORD ip, WORD bp )
{
    se->dwSize = sizeof( STACKTRACEENTRY );
    return( StackTraceCSIPFirst( se, ss, cs, ip, bp ) );

} /* MyStackTraceCSIPFirst */

/*
 * MyStackTraceFirst - wrapper for StackTraceFirst
 */
BOOL MyStackTraceFirst( STACKTRACEENTRY *se, HANDLE h )
{
    se->dwSize = sizeof( STACKTRACEENTRY );
    return( StackTraceFirst( se, h ) );

} /* MyStackTraceFirst */

/*
 * MyGlobalFirst - wrapper for GlobalFirst
 */
BOOL MyGlobalFirst( GLOBALENTRY *ge, WORD flags )
{
    ge->dwSize = sizeof( GLOBALENTRY );
    return( GlobalFirst( ge, flags ) );

} /* MyGlobalFirst */

/*
 * MyGlobalNext - wrapper for GlobalNext
 */
BOOL MyGlobalNext( GLOBALENTRY *ge, WORD flags )
{
    ge->dwSize = sizeof( GLOBALENTRY );
    return( GlobalNext( ge, flags ) );

} /* MyGlobalNext */

/*
 * MyGlobalEntryModule - wrapper for GlobalEntryModule
 */
BOOL MyGlobalEntryModule( GLOBALENTRY *ge, HMODULE hmod, WORD seg )
{
    ge->dwSize = sizeof( GLOBALENTRY );
    return( GlobalEntryModule( ge, hmod, seg ) );

} /* MyGlobalEntryModule */

/*
 * MyGlobalEntryHandle - wrapper for GlobalEntryHandle
 */
BOOL MyGlobalEntryHandle( GLOBALENTRY *ge, HMODULE hmem )
{
    ge->dwSize = sizeof( GLOBALENTRY );
    return( GlobalEntryHandle( ge, hmem ) );

} /* MyGlobalEntryHandle */
