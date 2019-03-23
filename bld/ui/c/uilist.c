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


static ui_event_list EventList = { 0 };


void intern initeventlists( void )
/********************************/
{
    UIData->events = &EventList;
    UIData->events->num_lists = 0;
}


ui_event_list * UIAPI uigetlist( void )
/*************************************/
{
    return( UIData->events );
}


void UIAPI uiputlist( ui_event_list *eventlist )
/**********************************************/
{
    UIData->events = eventlist;
}


void UIAPI uipushlist( ui_event *list )
/*************************************/
{
    if( UIData->events->num_lists < MAX_EVENT_LISTS ) {
        UIData->events->events[UIData->events->num_lists++] = list;
    }
}


bool UIAPI uiinlist( ui_event ui_ev, ui_event *ui_ev_list )
/*********************************************************/
{
    for( ; *ui_ev_list != __rend__; ui_ev_list += 2 ) {
        if( ( ui_ev >= *ui_ev_list ) && ( ui_ev <= *( ui_ev_list + 1 ) ) ) {
            return( true );
        }
    }
    for( ui_ev_list++; *ui_ev_list != __end__; ui_ev_list++ ) {
        if( ui_ev == *ui_ev_list ) {
            return( true );
        }
    }
    return( false );
}


ui_event * UIAPI uipoplist( void )
/********************************/
{
    ui_event    *list;

    list = NULL;
    if( UIData->events->num_lists > 0 ) {
        UIData->events->num_lists--;
        list = UIData->events->events[UIData->events->num_lists];
        UIData->events->events[UIData->events->num_lists] = NULL;
    }
    return( list );
}


bool UIAPI uiinlists( ui_event ui_ev )
/************************************/
{
    int         index;

    // EV_KILL_UI is implicitly pushed as part of every list
    if( ui_ev == EV_KILL_UI )
        return( true );
    if( UIData->events->num_lists > 0 ) {
        for( index = UIData->events->num_lists - 1; index >= 0; --index ) {
            if( UIData->events->events[index] == NULL )
                break;
            if( uiinlist( ui_ev, UIData->events->events[index] ) ) {
                return( true );
            }
        }
    }
    return( false );
}


bool UIAPI uiintoplist( ui_event ui_ev )
/**************************************/
{
    if( UIData->events->num_lists > 0 )
        return( uiinlist( ui_ev, UIData->events->events[UIData->events->num_lists - 1] ) );
    return( false );
}
