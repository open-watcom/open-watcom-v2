/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Process all pending events.
*
****************************************************************************/


#include "guiwind.h"
#include "guixloop.h"
#include "guiuiev.h"
#include "guievent.h"


void GUIAPI GUIDrainEvents( void )
{
    ui_event    ui_ev;

    UIData->busy_wait = true;
    uipushlist( GUIAllEvents );
    for( ;; ) {
        ui_ev = uivgetevent( NULL );
        if( ui_ev == EV_SINK )
            break;
        ui_ev = GUIUIProcessEvent( ui_ev );
        if( ui_ev == EV_NO_EVENT )
            break;
        GUIProcessEvent( ui_ev );
    }
    uipoplist();
    UIData->busy_wait = false;
}
