/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*    Portions Copyright (c) 1989-2002 Novell, Inc.  All Rights Reserved.
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
*   Any problems or updates required, please either contact the author or
*   the OpenWatcom contributors forums.
*       http://www.openwatcom.com/
*
* Description:
*
****************************************************************************/

#define DONT_CHECK_CTRL_CHARS 0x10 /* turns off ^C and ^S processing        */
#define AUTO_DESTROY_SCREEN   0x20 /* prevents "Press any key to close" msg */

#define RIGHT_SHIFT_KEY     0x01
#define LEFT_SHIFT_KEY      0x02
#define CONTROL_KEY         0x04
#define ALT_KEY             0x08
#define SCROLL_LOCK_IS_ON   0x10
#define NUM_LOCK_IS_ON      0x20
#define CAPS_LOCK_IS_ON     0x40

#define NORMAL_KEY          0x00
#define FUNCTION_KEY        0x01
#define ENTER_KEY           0x02
#define ESCAPE_KEY          0x03
#define BACKSPACE_KEY       0x04
#define DELETE_KEY          0x05
#define INSERT_KEY          0x06
#define CURSOR_UP_KEY       0x07
#define CURSOR_DOWN_KEY     0x08
#define CURSOR_RIGHT_KEY    0x09
#define CURSOR_LEFT_KEY     0x0A
#define CURSOR_HOME_KEY     0x0B
#define CURSOR_END_KEY      0x0C
#define CURSOR_PUP_KEY      0x0D
#define CURSOR_PDOWN_KEY    0x0E

typedef unsigned char       BYTE;
typedef unsigned short      WORD;

/* From conio.h since conio.h conflicts with procdef.h */
extern int __GetScreenID( int __screenHandle );

extern int CreateScreen( char *screenName, BYTE attributes );
extern void CopyToScreenMemory( WORD height, WORD width, BYTE *Rect, WORD beg_x, WORD beg_y );
extern int DestroyScreen( int screenHandle );
extern int DisplayInputCursor( void );
extern int HideInputCursor( void );
extern int SetPositionOfInputCursor( WORD row, WORD column );
extern int DisplayScreen( int screenHandle );
extern int SetCurrentScreen( int screenHandle );
extern int GetSizeOfScreen( WORD *heightP, WORD *widthP );
extern int IsColorMonitor( void );
extern WORD SetCursorShape( BYTE startline, BYTE endline );
extern WORD GetCursorShape( BYTE *startline, BYTE *endline );
extern WORD wherex( void );
extern WORD wherey( void );


extern int CheckKeyStatus( struct ScreenStruct *scrID );
extern void GetKey( struct ScreenStruct *scrID, BYTE *type, BYTE *value, BYTE *status, BYTE *scancode, size_t linesToProtect );
extern void UngetKey( struct ScreenStruct *scrID, BYTE type, BYTE value, BYTE status, BYTE scancode );
