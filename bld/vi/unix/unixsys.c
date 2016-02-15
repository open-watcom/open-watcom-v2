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
* Description:  *nix system support functions for VI
*
****************************************************************************/


#include "vi.h"
#include "curses.h"
#include "win.h"
#include "stdui.h"
#include "vibios.h"

extern int  PageCnt;

static char oldPath[FILENAME_MAX];

int FileSysNeedsCR( int handle )
{
    return( false );
}

/*
 * PushDirectory
 */
void PushDirectory( const char *orig )
{
    orig = orig;
    oldPath[0] = 0;
    GetCWD2( oldPath, FILENAME_MAX );

} /* PushDirectory */

/*
 * PopDirectory
 */
void PopDirectory( void )
{
    if( oldPath[0] != 0 ) {
        ChangeDirectory( oldPath );
    }
    ChangeDirectory( CurrentDirectory );

} /* PopDirectory */

/*
 * NewCursor - change cursor to insert mode type
 */
void NewCursor( window_id wid, cursor_type ct )
{
    // could do a curs_set() here

} /* NewCursor */

/*
 * MyBeep - ring beeper
 */
void MyBeep( void )
{
    if( EditFlags.BeepFlag ) {
        beep();
    }

} /* MyBeep */


/*
 * ScreenInit - get screen info
 */
void ScreenInit( void )
{
    if( !uistart() )
        StartupError( ERR_WIND_NO_MORE_WINDOWS );

    BIOSKeyboardInit();

    EditVars.WindMaxHeight = LINES;
    EditVars.WindMaxWidth = COLS;
    Scrn = (void *)(UIData->screen.origin);

    EditFlags.Color = true;
    EditFlags.HasSystemMouse = true;

} /* ScreenInit */

/*
 * ScreenFini
 */
void ScreenFini( void )
{
    uistop();

} /* ScreenFini */

/*
 * ChkExtendedKbd - look for extended keyboard type
 */
void ChkExtendedKbd( void )
{
    EditVars.ExtendedKeyboard = 0;

} /* ChkExtendedKbd */

/*
 * MemSize - return amount of dos memory left (in 16 byte paragraphs)
 */
long MemSize( void )
{
    return ( 0 );

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
    drive = drive;
    return( ERR_NO_ERR );

}/* ChangeDrive */

/*
 * ShiftDown - test if shift key is down
 */
bool ShiftDown( void )
{
    return( false );

} /* ShiftDown */

static bool hadCapsLock;

/*
 * TurnOffCapsLock - switch off caps lock
 */
void TurnOffCapsLock( void )
{
    hadCapsLock = false;

} /* TurnOffCapsLock */

/*
 * DoGetDriveType - get the type of drive A-Z
 */
drive_type DoGetDriveType( int drv )
{
    drv = drv;
    return( DRIVE_TYPE_NONE );

} /* DoGetDriveType */

/*
 * MyDelay - delay a specified number of milliseconds
 */
void MyDelay( int ms )
{
    napms( ms );

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
    return( GetVIKey( BIOSGetKeyboard( NULL ), 0, false ) );
}

bool KeyboardHit( void )
{
    return( BIOSKeyboardHit() );
}

void MyVioShowBuf( size_t offset, unsigned nchars )
{
    BIOSUpdateScreen( offset, nchars );
}
