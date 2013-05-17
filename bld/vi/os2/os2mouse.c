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
* Description:  OS/2 mouse interface functions.
*
****************************************************************************/


#include "vi.h"
#include "mouse.h"
#define INCL_BASE
#include <os2.h>

static bool mouseHasTwoButtons = FALSE;
static HMOU mouseHandle;
static int  lastStatus;
static int  lastRow;
static int  lastCol;

/*
 * SetMouseSpeed - set mouse movement speed
 */
void SetMouseSpeed( int speed )
{
    speed = speed;

} /* SetMouseSpeed */

/*
 * SetMousePosition - set the mouse position
 */
void SetMousePosition( int row, int col )
{
    lastRow = MouseRow = row;
    lastCol = MouseCol = col;

} /* SetMousePosition */

/*
 * PollMouse - poll the mouse for it's state
 */
void PollMouse( int *status, int *row, int *col )
{
    struct      _MOUEVENTINFO   meinfo;
    struct      _MOUQUEINFO     mqinfo;
    USHORT                      readtype = 0;

    if( !MouGetNumQueEl( &mqinfo, mouseHandle ) ) {
        if( mqinfo.cEvents != 0 ) {
            if( !MouReadEventQue( &meinfo, &readtype, mouseHandle ) ) {
                lastStatus = 0;
                if( meinfo.fs & 0x0006 ) {
                    lastStatus |= MOUSE_LEFT_BUTTON_DOWN;
                }
                if( mouseHasTwoButtons ) {
                    if( meinfo.fs & 0x0078 ) {
                        lastStatus |= MOUSE_RIGHT_BUTTON_DOWN;
                    }
                } else {
                    if( meinfo.fs & 0x0018 ) {
                        lastStatus |= MOUSE_RIGHT_BUTTON_DOWN;
                    }
                    if( meinfo.fs & 0x0060 ) {
                        lastStatus |= MOUSE_MIDDLE_BUTTON_DOWN;
                    }
                }
                lastRow  = meinfo.row;
                lastCol  = meinfo.col;
            }
        }
    }

    *status = lastStatus;
    *col = lastCol;
    *row = lastRow;

} /* PollMouse */

/*
 * InitMouse - initialize the mouse
 */
void InitMouse( void )
{
    USHORT          events;

    if( !EditFlags.UseMouse ) {
        return;
    }

    if( MouOpen( 0L, &mouseHandle ) != 0 ) {
        EditFlags.UseMouse = FALSE;
        return;
    }
    events = 0x007f;
    if( MouSetEventMask( &events, mouseHandle ) != 0 ){
        events = 0x001f;
        if( MouSetEventMask( &events, mouseHandle ) == 0 ){
            mouseHasTwoButtons = TRUE;
        }
    }

    SetMousePosition( EditVars.WindMaxWidth / 2 - 1, EditVars.WindMaxHeight / 2 - 1 );
    SetMouseSpeed( EditVars.MouseSpeed );
    PollMouse( &MouseStatus, &MouseRow, &MouseCol );

} /* InitMouse */

/*
 * FiniMouse - done with the mouse
 */
void FiniMouse( void )
{
    if( EditFlags.UseMouse ) {
        MouClose( mouseHandle );
    }

} /* FiniMouse */
