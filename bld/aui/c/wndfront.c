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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "auipvt.h"


extern  void    WndToFront( a_window *wnd )
{
    GUIBringToFront( wnd->gui );
}


extern  void    WndRestoreToFront( a_window *wnd )
{
    if( _Is( wnd, WSW_ICONIFIED ) ) {
        GUIRestoreWindow( wnd->gui );
    }
    WndToFront( wnd );
}


void    WndNextNonIconToFront( a_window *wnd )
{
    while( wnd != NULL ) {
        if( _Isnt( wnd, WSW_ICONIFIED ) && wnd != WndMain ) {
            WndToFront( wnd );
            return;
        }
        wnd = WndNext( wnd );
    }
}


extern  void    WndChooseNew()
{
    a_window    *wnd;
    a_window    *lastok;

    lastok = NULL;
    for( wnd = WndNext( NULL ); wnd != NULL; wnd = WndNext( wnd ) ) {
        if( _Is( wnd, WSW_ICONIFIED ) ) continue;
        if( wnd == WndMain ) continue;
        lastok = wnd;
    }
    if( lastok != NULL ) {
        WndToFront( lastok );
    }
}
