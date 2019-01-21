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
#include "win.h"
#include "mouse.h"

void RedrawMouse( windim p1, windim p2 ) { (void)p1;(void)p2;}
bool DisplayMouse( bool p1 ) { (void)p1; return( false ); }
void TurnOffCapsLock( void ) {}
vi_rc HandleMouseEvent( void ) { return( ERR_NO_ERR ); }
void SwapAllWindows( void ) {}
void SetMouseSpeed( int i ) { (void)i; }
void GetClockStart( void ) {}
void GetSpinStart( void ) {}
void WindowAuxUpdate( window_id wid, int x, int y ) { (void)wid;(void)x;(void)y; }
void DrawBorder( window_id wid ) { (void)wid; }
void PushMouseEventHandler( mouse_callback cb ) { (void)cb; }
void PopMouseEventHandler( void ) {}
void WindowBorderData( window_id wid, const char *c, int x ) { (void)wid;(void)c;(void)x; }
vi_rc ResizeWindowRelative( window_id wid, windim p1, windim p2, windim p3, windim p4, bool flags ) {
    (void)wid;(void)p1;(void)p2;(void)p3;(void)p4;(void)flags;
    return( ERR_NO_ERR );
}
vi_rc ResizeWindow( window_id wid, windim p1, windim p2, windim p3, windim p4, bool flags ) {
    (void)wid;(void)p1;(void)p2;(void)p3;(void)p4;(void)flags;
    return( ERR_NO_ERR );
}
void RestoreInterrupts( void ) {}
void WindowSwapFileClose( void ) {}
void FiniMouse( void ) {}
void ScreenFini( void ) {}
vi_rc ResizeCurrentWindowWithKeys( void ) { return( ERR_NO_ERR ); }
vi_rc MoveCurrentWindowWithKeys( void ) { return( ERR_NO_ERR ); }
drive_type DoGetDriveType( int i ) { (void)i; return( DRIVE_TYPE_NONE ); }
void ClearScreen( void ) {}
vi_rc ResetWindow( window_id *wid ) { (void)wid; return( ERR_NO_ERR ); }
bool WindowIsVisible( window_id wid ) { (void)wid; return( true ); }
void ScreenInit( void ) {}
void SetInterrupts( void ) {}
void ChkExtendedKbd( void ) {}
void InitMouse( void ) {}
void SetBorderGadgets( window_id wid, bool how ) { (void)wid;(void)how; }
vi_rc GetNewValueDialog( char *value ) { (void)value; return( ERR_NO_ERR ); }
void DisplayCrossLineInWindow( window_id wid, int line ) { (void)wid;(void)line; }
int SetCharInWindowWithColor( window_id wid, windim line, windim col, char text, type_style *style ) {
    (void)wid;(void)line;(void)col;(void)text;(void)style;
    return( ERR_NO_ERR );
}
void DisplayLineWithHilite( window_id wid, int line, char *text, int start, int end, int ignore ) {
    (void)wid;(void)line;(void)text;(void)start;(void)end;(void)ignore;
}
void SetPosToMessageLine( void ) {}
void HideCursor( void ) {}
