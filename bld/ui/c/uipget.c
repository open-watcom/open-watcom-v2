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


#include "uidef.h"
#include "uidebug.h"


ui_event        Event = EV_NO_EVENT;

ui_event intern saveevent( void )
/*******************************/
{
    register ui_event       ui_ev;

    if( Event > EV_NO_EVENT && uiinlists( Event ) ) {
        ui_ev = Event;
        Event = -Event;
    } else {
        Event = EV_NO_EVENT;
        ui_ev = EV_NO_EVENT;
    }
    return( ui_ev );
}


ui_event intern getprime( VSCREEN *vptr )
/***************************************/
{
    _uicheckuidata();
    if( Event <= EV_NO_EVENT ) {
        if( vptr != NULL ) {
//            uivsetactive( vptr );
        }
        uivsetcursor( vptr );
        uidoneevent();
        Event = uiget();
        uistartevent();
    }
    return( Event );
}


ui_event UIAPI uivgetprime( VSCREEN *vptr )
/*****************************************/
{
    getprime( vptr );
    return( saveevent() );
}


ui_event UIAPI uivgetprimevent( VSCREEN *vptr )
/*********************************************/
{
    register ui_event       ui_ev;

    ui_ev = uivgetprime( vptr );
//    switch( ui_ev ) {
//    case EV_NO_EVENT:
//    case EV_SINK:
//        uirefresh();
//    }
    return( ui_ev );
}


