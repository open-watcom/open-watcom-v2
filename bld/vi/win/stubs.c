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


#include "vi.h"
#include <dos.h>

#pragma off( unreferenced );

void RedrawMouse( int p1, int p2 ) {}
bool DisplayMouse( bool p1 ) { return( FALSE ); }
void TurnOffCapsLock( void ) {}
vi_rc HandleMouseEvent( void ) { return( ERR_NO_ERR ); }
void SwapAllWindows( void ) {}
void SetMouseSpeed( int i ) {}
void GetClockStart( void ) {}
void GetSpinStart( void ) {}
void WindowAuxUpdate( window_id id, int x, int y ) {}
void DrawBorder( window_id id ) {}
void PushMouseEventHandler( mouse_callback cb ) {}
void PopMouseEventHandler( void ) {}
void WindowBorderData( window_id id, char *c, int x ) {}
void ResizeWindowRelative( window_id id, int p1, int p2, int p3, int p4, int flags ) {}
void ResizeWindow( window_id id, int p1, int p2, int p3, int p4, int flags ) {}
void RestoreInterrupts( void ) {}
void WindowSwapFileClose( void ) {}
void FiniMouse( void ) {}
void ScreenFini( void ) {}
vi_rc ResizeCurrentWindowWithKeys( void ) { return( ERR_NO_ERR ); }
vi_rc MoveCurrentWindowWithKeys( void ) { return( ERR_NO_ERR ); }
drive_type DoGetDriveType( int i ) { return( DRIVE_NONE ); }
void ClearScreen( void ) {}
vi_rc ResetWindow( window_id *id ) { return( ERR_NO_ERR ); }
bool WindowIsVisible( window_id id ) { return( TRUE ); }
void ScreenInit( void ) {}
void SetInterrupts( void ) {}
void ChkExtendedKbd( void ) {}
void InitMouse( void ) {}
void SetBorderGadgets( window_id id, bool how ) {}
void NewWindow( void ) {}
int GetNewValueDialog( char *value ) { return( 0 ); }
void DisplayCrossLineInWindow( window_id id, int line ) {}
int SetCharInWindowWithColor( window_id wn, int line, int col, char text,
    type_style *style ) { return( ERR_NO_ERR ); }
void DisplayLineWithHilite( window_id id, int line, char *text, int start, int end,
    int ignore ) {}
void SetPosToMessageLine( void ) {}
void HideCursor( void ) {}

