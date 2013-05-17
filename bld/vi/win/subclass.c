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


#include "vi.h"
#include "subclass.h"
#include <assert.h>

proc_entry  *procHead;
proc_entry  *procTail;

void SubclassGenericAdd( HWND hwnd, WNDPROC proc )
{
    proc_entry *newProc;

    newProc = MemAlloc( sizeof( proc_entry ) );

    newProc->hwnd = hwnd;
    newProc->oldProc = (WNDPROC)GET_WNDPROC( hwnd );
    newProc->newProc = (WNDPROC)MakeProcInstance( (FARPROC)proc, InstanceHandle );
    SET_WNDPROC( hwnd, (LONG_PTR)newProc->newProc );

    AddLLItemAtEnd( (ss **)&procHead, (ss **)&procTail, (ss *)newProc );
}

void SubclassGenericRemove( HWND hwnd )
{
    proc_entry  *findProc;

    findProc = procHead;
    while( findProc && findProc->hwnd != hwnd ) {
        findProc = findProc->next;
    }
    assert( findProc != NULL );

    SET_WNDPROC( hwnd, (LONG_PTR)findProc->oldProc );
    (void)FreeProcInstance( (FARPROC)findProc->newProc );

    DeleteLLItem( (ss **)&procHead, (ss **)&procTail, (ss *)findProc );
    MemFree( findProc );
}

void SubclassGenericInit( void )
{
    procHead = NULL;
    procTail = NULL;
}

void SubclassGenericFini( void )
{
    assert( procHead == NULL );
}

WNDPROC SubclassGenericFindOldProc( HWND hwnd )
{
    // doing a search on every message is a huge time hit, but this is
    // by no means time-critical.  Perhaps automatically registering a
    // new class based on the hwnd parameter with an extra few bytes
    // for the old window... /*BW*/
    proc_entry  *findProc;

    findProc = procHead;
    while( findProc && findProc->hwnd != hwnd ) {
        findProc = findProc->next;
    }
    assert( findProc != NULL );
    return( findProc->oldProc );
}
