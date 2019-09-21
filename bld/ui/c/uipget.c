/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
    ui_event        ui_ev;

    if( Event > EV_NO_EVENT && uiinlists( Event ) ) {
        ui_ev = Event;
        Event = -Event;
    } else {
        Event = EV_NO_EVENT;
        ui_ev = EV_NO_EVENT;
    }
    return( ui_ev );
}


ui_event intern getprime( VSCREEN *vs )
/*************************************/
{
    _uicheckuidata();
    if( Event <= EV_NO_EVENT ) {
        if( vs != NULL ) {
//            uivsetactive( vs );
        }
        uivsetcursor( vs );
        uidoneevent();
        Event = uiget();
        uistartevent();
    }
    return( Event );
}


ui_event UIAPI uivgetprime( VSCREEN *vs )
/***************************************/
{
    getprime( vs );
    return( saveevent() );
}


ui_event UIAPI uivgetprimevent( VSCREEN *vs )
/*******************************************/
{
    ui_event        ui_ev;

    ui_ev = uivgetprime( vs );
//    switch( ui_ev ) {
//    case EV_NO_EVENT:
//    case EV_SINK:
//        uirefresh();
//    }
    return( ui_ev );
}


