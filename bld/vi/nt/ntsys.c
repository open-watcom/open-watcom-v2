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
* Description:  OS/2 specific system interface functions.:
*
****************************************************************************/


#include "vi.h"
#include <dos.h>
#include "win.h"
#include "dosx.h"
#include "vibios.h"

HANDLE  InputHandle = INVALID_HANDLE_VALUE;
HANDLE  OutputHandle = INVALID_HANDLE_VALUE;
COORD   BSize = {0, 0};

extern int PageCnt;

static char oldDir[_MAX_PATH];

int FileSysNeedsCR( int handle )
{
    return( TRUE );
}

/*
 * PushDirectory
 */
void PushDirectory( char *orig )
{
    orig = orig;
    oldDir[0] = 0;
    GetCWD2( oldDir, _MAX_PATH );

} /* PushDirectory */

/*
 * PopDirectory
 */
void PopDirectory( void )
{
    if( oldDir[0] != 0 ) {
        ChangeDirectory( oldDir );
    }
    ChangeDirectory( CurrentDirectory );

} /* PopDirectory */

/*
 * NewCursor - change cursor to insert mode type
 */
void NewCursor( window_id id, cursor_type ct )
{
    CONSOLE_CURSOR_INFO ci;

    id = id;
    ci.dwSize = ct.height;
    ci.dwSize += 18;
    if( ci.dwSize > 100 ) {
        ci.dwSize = 100;
    }
    ci.bVisible = TRUE;
    SetConsoleCursorInfo( OutputHandle, &ci );

} /* NewCursor */

/*
 * MyBeep - ring beeper
 */
void MyBeep( void )
{
    if( EditFlags.BeepFlag ) {
        Beep( 300, 75 );
    }

} /* MyBeep */

static char *oldConTitle;

/*
 * ScreenInit - get screen info
 */
void ScreenInit( void )
{
    CONSOLE_SCREEN_BUFFER_INFO  sbi;
    char                        tmp[256];

    InputHandle = CreateFile( "CONIN$", GENERIC_READ | GENERIC_WRITE,
                              FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                              OPEN_EXISTING, 0, NULL );
    SetConsoleMode( InputHandle, ENABLE_MOUSE_INPUT | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT );

    OutputHandle = CreateConsoleScreenBuffer( GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL );
    SetConsoleMode( OutputHandle, 0 );
    // SetConsoleActiveScreenBuffer( OutputHandle );

    GetConsoleScreenBufferInfo( OutputHandle, &sbi );
    EditVars.WindMaxWidth = sbi.dwMaximumWindowSize.X;
    EditVars.WindMaxHeight = sbi.dwMaximumWindowSize.Y;
    BSize.X = EditVars.WindMaxWidth;
    BSize.Y = EditVars.WindMaxHeight;

    EditFlags.Color = TRUE;

    Scrn = malloc( EditVars.WindMaxWidth * EditVars.WindMaxHeight * sizeof( char_info ) );
    ScreenPage( 0 );

    tmp[0] = 0;
    GetConsoleTitle( tmp, sizeof( tmp ) );
    AddString( &oldConTitle, tmp );
    if( !EditFlags.Quiet ) {
        SetConsoleTitle( "Open Watcom vi" );
    }

} /* ScreenInit */

/*
 * ScreenFini - finished with console
 */
void ScreenFini( void )
{
    CloseHandle( OutputHandle );
    CloseHandle( InputHandle );
    SetConsoleTitle( oldConTitle );

} /* ScreenFini */

/*
 * ChkExtendedKbd - look for extended keyboard type
 */
void ChkExtendedKbd( void )
{
    EditFlags.ExtendedKeyboard = 0x10;

} /* ChkExtendedKbd */

/*
 * MemSize - return amount of dos memory left (in 16 byte paragraphs)
 */
long MemSize( void )
{
    // this value is not used for anything important.
    return( 0 );

} /* MemSize */

/*
 * ScreenPage - set the screen page to active/inactive
 */
void ScreenPage( int page )
{
    PageCnt += page;

} /* ScreenPage */

/*
 * ChangeDrive - change the working drive
 */
vi_rc ChangeDrive( int drive )
{
    char        dir[4];

    dir[0] = drive;
    dir[1] = ':';
    dir[2] = '.';
    dir[3] = 0;

    if( !SetCurrentDirectory( dir ) ) {
        return( ERR_NO_SUCH_DRIVE );
    }
    return( ERR_NO_ERR );

}/* ChangeDrive */

/*
 * ShiftDown - test if shift key is down
 */
bool ShiftDown( void )
{
    // This is technically correct but this function is not
    // actually used for anything so why bother.

    // BYTE kbstate[256];
    // GetKeyboardState( &kbstate );
    // return( kbstate[VK_SHIFT] & 0x80 );

    return( FALSE );

} /* ShiftDown */

static bool hadCapsLock;

/*
 * TurnOffCapsLock - switch off caps lock
 */
void TurnOffCapsLock( void )
{
    hadCapsLock = FALSE;

} /* TurnOffCapsLock */

/*
 * DoGetDriveType - get the type of drive A-Z
 */
drive_type DoGetDriveType( int drv )
{
    char        path[4];
    DWORD       type;

    path[0] = drv;
    path[1] = ':';
    path[2] = '\\';
    path[3] = 0;
    type = GetDriveType( path );
    if( type == 1 ) {
        return( DRIVE_NONE );
    }
    if( type == DRIVE_REMOVABLE ) {
        return( DRIVE_IS_REMOVABLE );
    }
    return( DRIVE_IS_FIXED );

} /* DoGetDriveType */

/*
 * MyDelay - delay a specified number of milliseconds
 */
void MyDelay( int ms )
{
    Sleep( ms );

} /* MyDelay */

/*
 * SetCursorBlinkRate - set the current blink rate for the cursor
 */
void SetCursorBlinkRate( int cbr )
{
    EditVars.CursorBlinkRate = cbr;

} /* SetCursorBlinkRate */

vi_key GetKeyboard( void )
{
    return( GetVIKey( BIOSGetKeyboard( NULL ), 0, FALSE ) );
}

bool KeyboardHit( void )
{
    return( BIOSKeyboardHit() );
}

void MyVioShowBuf( unsigned offset, unsigned nchars )
{
    BIOSUpdateScreen( offset, nchars );
}

