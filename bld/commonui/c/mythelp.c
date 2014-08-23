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
#include "bool.h"
#include "mythelp.h"

/*
 * MyModuleFindHandle - wrapper for ModuleFindHandle
 */
bool MyModuleFindHandle( MODULEENTRY FAR *me, HMODULE h )
{
    bool        rc;

    me->dwSize = sizeof( MODULEENTRY );
    rc = (ModuleFindHandle( me, h ) != NULL);
    me->szModule[MAX_MODULE_NAME] = 0;
    return( rc );

} /* MyModuleFindHandle */

/*
 * MyStackTraceNext - wrapper for StackTraceNext
 */
bool MyStackTraceNext( STACKTRACEENTRY FAR *se )
{
    se->dwSize = sizeof( STACKTRACEENTRY );
    return( StackTraceNext( se ) != 0 );

} /* MyStackTraceNext */

/*
 * MyTaskFindHandle - wrapper for TaskFindHandle
 */
bool MyTaskFindHandle( TASKENTRY FAR *te, HTASK h )
{
    te->dwSize = sizeof( TASKENTRY );
    return( TaskFindHandle( te, h ) != 0 );

} /* MyTaskFindHandle */

/*
 * MyTaskFirst - wrapper for TaskFirst
 */
bool MyTaskFirst( TASKENTRY FAR *te )
{
    te->dwSize = sizeof( TASKENTRY );
    return( TaskFirst( te ) != 0 );

} /* MyTaskFirst */

/*
 * MyTaskNext - wrapper for TaskNext
 */
bool MyTaskNext( TASKENTRY FAR *te )
{
    te->dwSize = sizeof( TASKENTRY );
    return( TaskNext( te ) != 0 );

} /* MyTaskNext */

/*
 * MyStackTraceCSIPFirst - wrapper for StackTraceCSIPFirst
 */
bool MyStackTraceCSIPFirst( STACKTRACEENTRY FAR *se, WORD ss, WORD cs, WORD ip, WORD bp )
{
    se->dwSize = sizeof( STACKTRACEENTRY );
    return( StackTraceCSIPFirst( se, ss, cs, ip, bp ) != 0 );

} /* MyStackTraceCSIPFirst */

/*
 * MyStackTraceFirst - wrapper for StackTraceFirst
 */
bool MyStackTraceFirst( STACKTRACEENTRY FAR *se, HANDLE h )
{
    se->dwSize = sizeof( STACKTRACEENTRY );
    return( StackTraceFirst( se, h ) != 0 );

} /* MyStackTraceFirst */

/*
 * MyGlobalFirst - wrapper for GlobalFirst
 */
bool MyGlobalFirst( GLOBALENTRY FAR *ge, WORD flags )
{
    ge->dwSize = sizeof( GLOBALENTRY );
    return( GlobalFirst( ge, flags ) != 0 );

} /* MyGlobalFirst */

/*
 * MyGlobalNext - wrapper for GlobalNext
 */
bool MyGlobalNext( GLOBALENTRY FAR *ge, WORD flags )
{
    ge->dwSize = sizeof( GLOBALENTRY );
    return( GlobalNext( ge, flags ) != 0 );

} /* MyGlobalNext */

/*
 * MyGlobalEntryModule - wrapper for GlobalEntryModule
 */
bool MyGlobalEntryModule( GLOBALENTRY FAR *ge, HMODULE hmod, WORD seg )
{
    ge->dwSize = sizeof( GLOBALENTRY );
    return( GlobalEntryModule( ge, hmod, seg ) != 0 );

} /* MyGlobalEntryModule */

/*
 * MyGlobalEntryHandle - wrapper for GlobalEntryHandle
 */
bool MyGlobalEntryHandle( GLOBALENTRY FAR *ge, HMODULE hmem )
{
    ge->dwSize = sizeof( GLOBALENTRY );
    return( GlobalEntryHandle( ge, hmem ) != 0 );

} /* MyGlobalEntryHandle */
