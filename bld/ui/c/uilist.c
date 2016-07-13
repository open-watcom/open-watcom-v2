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


static EVENTLIST EventList = { 0 };


void initeventlists( void )
/*************************/
{
    UIData->events = &EventList;
    UIData->events->num_lists = 0;
}


EVENTLIST* UIAPI uigetlist( void )
/*********************************/
{
    return( UIData->events );
}


void UIAPI uiputlist( EVENTLIST *eventlist )
/*******************************************/
{
    UIData->events = eventlist;
}


void UIAPI uipushlist( EVENT *list )
/***********************************/
{
    if( UIData->events->num_lists < MAX_EVENT_LISTS ) {
        UIData->events->events[UIData->events->num_lists++] = list;
    }
}


bool UIAPI uiinlist( EVENT ev, EVENT *eptr )
/******************************************/
{
    while( *eptr != EV_NO_EVENT ) {
        if( ( ev >= *eptr ) && ( ev <= *( eptr + 1 ) ) ) {
            return( true );
        }
        eptr += 2;
    }
    ++eptr;
    while( *eptr != EV_NO_EVENT ) {
        if( ev == *eptr ) {
            return( true );
        }
        ++eptr;
    }
    return( false );
}


EVENT *UIAPI uipoplist( void )
/*****************************/
{
    register EVENT      *list;

    list = NULL;
    if( UIData->events->num_lists > 0 ) {
        UIData->events->num_lists--;
        list = UIData->events->events[UIData->events->num_lists];
        UIData->events->events[UIData->events->num_lists] = NULL;
    }
    return( list );
}


bool UIAPI uiinlists( EVENT ev )
/******************************/
{
    register int    index;

    // EV_KILL_UI is implicitly pushed as part of every list
    if( ev == EV_KILL_UI )
        return( true );
    if( UIData->events->num_lists > 0 ) {
        for( index = UIData->events->num_lists - 1; index >= 0; --index ) {
            if( UIData->events->events[index] == NULL )
                break;
            if( uiinlist( ev, UIData->events->events[index] ) ) {
                return( true );
            }
        }
    }
    return( false );
}


bool UIAPI uiintoplist( EVENT ev )
/*********************************/
{
    if( UIData->events->num_lists > 0 )
        return( uiinlist( ev, UIData->events->events[UIData->events->num_lists - 1] ) );
    return( false );
}
