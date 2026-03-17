/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
#ifdef __WINDOWS__
    #include "wclbproc.h"
#endif


proc_entry  *procHead;
proc_entry  *procTail;

#ifdef __WINDOWS__
void SubclassGenericAdd( HWND hwnd, WNDPROCx wndproc, HINSTANCE inst )
#else
void SubclassGenericAdd( HWND hwnd, WNDPROC wndproc, HINSTANCE inst )
#endif
{
    proc_entry  *newProc;

#ifdef __WINDOWS__
#else
    /* unused parameters */ (void)inst;
#endif
    newProc = MemAllocSafe( sizeof( proc_entry ) );

    newProc->hwnd = hwnd;
    newProc->oldProc = (WNDPROC)GET_WNDPROC( hwnd );
#ifdef __WINDOWS__
    newProc->newProc = MakeProcInstance_WND( wndproc, inst );
#else
    newProc->newProc = wndproc;
#endif
    SET_WNDPROC( hwnd, (LONG_PTR)newProc->newProc );

    AddLLItemAtEnd( (ss **)&procHead, (ss **)&procTail, (ss *)newProc );
}

void SubclassGenericRemove( HWND hwnd )
{
    proc_entry  *findProc;

    for( findProc = procHead; findProc != NULL; findProc = findProc->next ) {
        if( findProc->hwnd == hwnd ) {
            break;
        }
    }
    assert( findProc != NULL );

    SET_WNDPROC( hwnd, (LONG_PTR)findProc->oldProc );
#ifdef __WINDOWS__
    FreeProcInstance_WND( findProc->newProc );
#endif
    MemFree( DeleteLLItem( (ss **)&procHead, (ss **)&procTail, (ss *)findProc ) );
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

    for( findProc = procHead; findProc != NULL; findProc = findProc->next ) {
        if( findProc->hwnd == hwnd ) {
            break;
        }
    }
    assert( findProc != NULL );
    return( findProc->oldProc );
}
