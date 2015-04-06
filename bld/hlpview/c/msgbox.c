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
* Description:  Message box routine for help viewer.
*
****************************************************************************/


#include "uidef.h"
#include "uidialog.h"

static a_hot_spot okButton[] = {
    {"OK",          EV_ENTER,       3,      20,     11,     HOT_DEFAULT},
};

static char     *message;
VFIELD helpMsgBox[] = {
    {{1,       4,      1,      49},    FLD_TEXT,       &message},
    {{3,       20,     1,      11},    FLD_HOT,        &okButton[0]},
    {{5,       0,      0,      0},     FLD_VOID,       NULL}
};

void ShowMsgBox( char *caption, char *msg )
{
    a_dialog            *msgbox;
    bool                done;
    EVENT               event;

    static EVENT        events[] = {
        EV_NO_EVENT,
        EV_ENTER,
        EV_ESCAPE,
        EV_MOUSE_PRESS,
        EV_LIST_BOX_DCLICK,
        EV_NO_EVENT
    };
    done = false;

    uipushlist( NULL );     /* modal barrier */
    uipushlist( events );
    helpMsgBox[0].u.str = msg;
    msgbox = uibegdialog( caption, helpMsgBox, 5, 50, 0, 0 );
    while( !done ) {
        event = uidialog( msgbox );
        switch( event ) {
        case EV_ENTER:
            done = true;
            break;
        }
    }
    uipoplist();
    uipoplist();
    uienddialog( msgbox );
}
