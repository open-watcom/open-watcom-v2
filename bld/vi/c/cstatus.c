/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
#include <time.h>
#include "win.h"

#define MAX_CSTATUS_STRLEN      10
static char *currStatus[] =
{
    "change",
    "delete",
    "yank",
    "case",
    "insert",
    "overstrike",
    "rep char",
    "command",
    "writing",
    "reading",
    "hide",
    "shift >",
    "shift <",
    "autosave"
};

static int      lastStatus;

/*
 * GetModeString - get the string corresponding to current status
 */
void GetModeString( char *st )
{
    strcpy( st, currStatus[lastStatus] );

} /* GetModeString */

/*
 * UpdateCurrentStatus - maintain current status info
 */
status_type UpdateCurrentStatus( status_type st )
{
    status_type ost;

    ost = lastStatus;
    lastStatus = st;
#ifndef __WIN__
    if( EditFlags.Menus ) {
        char    str[MAX_CSTATUS_STRLEN + 1];
        int     i;

        if( EditFlags.CurrentStatus ) {
            memset( str, ' ', MAX_CSTATUS_STRLEN );
            str[MAX_CSTATUS_STRLEN] = 0;
            i = strlen( currStatus[lastStatus] );
            if( i > MAX_CSTATUS_STRLEN )
                i = MAX_CSTATUS_STRLEN;
            memcpy( str, currStatus[lastStatus], i );
            for( i = 0; i < MAX_CSTATUS_STRLEN; i++ ) {
                SetCharInWindowWithColor( menu_window_id, 1, EditVars.CurrentStatusColumn + i, str[i], &menubarw_info.text_style );
            }
        } else {
#if 0
            /* this is real lame - no time to fix it now */
            memset( str, ' ', 15 );
            str[15] = 0;
            for( i = 0; i < 14; i++ ) {
                SetCharInWindowWithColor( menu_window_id, 1, CurrentStatusColumn + i - 6, str[i], &menubarw_info.text_style );
            }
#endif
        }
    }
#endif
    if( EditFlags.ModeInStatusLine ) {
        UpdateStatusWindow();
    }
    return( ost );

} /* UpdateCurrentStatus */
