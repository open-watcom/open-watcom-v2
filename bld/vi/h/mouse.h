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



#ifndef _MOUSE_INCLUDED
#define _MOUSE_INCLUDED

/* these are DOS constants, but we need some consistancy */
#define MOUSE_LEFT_BUTTON_DOWN          0x01
#define MOUSE_RIGHT_BUTTON_DOWN         0x02
#define MOUSE_MIDDLE_BUTTON_DOWN        0x04
#define MOUSE_ANY_BUTTON_DOWN           0x07

/* mouse events */
#define MOUSE_PRESS     0
#define MOUSE_RELEASE   1
#define MOUSE_DCLICK    2
#define MOUSE_HOLD      3
#define MOUSE_DRAG      4
#define MOUSE_REPEAT    5

#define MOUSE_PRESS_R   6
#define MOUSE_RELEASE_R 7
#define MOUSE_DCLICK_R  8
#define MOUSE_HOLD_R    9
#define MOUSE_DRAG_R    10
#define MOUSE_REPEAT_R  11

#define MOUSE_PRESS_M   12
#define MOUSE_RELEASE_M 13
#define MOUSE_DCLICK_M  14
#define MOUSE_HOLD_M    15
#define MOUSE_DRAG_M    16
#define MOUSE_REPEAT_M  17

#define MOUSE_MOVE      20

typedef bool (*mouse_callback)( window_id, int, int );

typedef struct mouse_hook {
    struct mouse_hook   *next;
    mouse_callback      cb;
} mouse_hook;

/* hdlmouse.c */
int HandleMouseEvent( void );

/* mouse.c */
int GetMouseEvent( void );
void RedrawMouse( int row, int col );
int DisplayMouse( int flag );

/* mouseev.c */
window_id GetMousePosInfo( int *win_x, int *win_y );
bool TestMouseEvent( bool );
void PushMouseEventHandler( mouse_callback cb );
void PopMouseEventHandler( void );

/* system dependant, <sys>mouse.c */
void SetMouseSpeed( int speed );
void SetMousePosition( int row, int col );
void ShowMouse( int on );
void PollMouse( int *status, int *row, int *col );
void InitMouse( void );
void FiniMouse( void );

#endif
