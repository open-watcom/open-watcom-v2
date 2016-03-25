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
* Description:  Get actual CS:IP of a stopped Windows task.
*
****************************************************************************/


#include "commonui.h"
#include "getcsip.h"

#define MAKECSIP( cs, ip ) (((DWORD)(cs) << 16L) + (DWORD)(ip))

/*
 * GetRealCSIP - get the CS:IP of a stopped task, cuz microsoft only tells
 *               you that the task is stopped in the kernel (gee, that's
 *               useful!!)
 */
DWORD GetRealCSIP( HTASK htask, HMODULE *mod )
{
    DWORD               csip;
    STACKTRACEENTRY     se;
    GLOBALENTRY         ge;
    TASKENTRY           te;

    te.dwSize = sizeof( te );
    if( TaskFindHandle( &te, htask ) == NULL ) {
        return( 0L );
    }
    if( mod != NULL ) {
        *mod = te.hModule;
    }

    csip = TaskGetCSIP( htask );
    if( csip == 0L ) {
        return( 0L );
    }
    se.dwSize = sizeof( se );
    if( !StackTraceFirst( &se, htask ) ) {
        return( csip );
    }
    csip = MAKECSIP( se.wCS, se.wIP );
    while( 1 ) {
        se.dwSize = sizeof( se );
        if( !StackTraceNext( &se ) ) {
            break;
        }
        csip = MAKECSIP( se.wCS, se.wIP );
        ge.dwSize = sizeof( ge );
        if( GlobalEntryHandle( &ge, (HGLOBAL)se.wCS ) ) {
            if( ge.hOwner == te.hModule ) {
                break;
            }
        }
    }
    return( csip );

} /* GetRealCSIP */
