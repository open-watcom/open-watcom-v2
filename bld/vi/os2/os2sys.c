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
* Description:  OS/2 specific system interface functions.
*
****************************************************************************/


#include "vi.h"
#include "win.h"
#include "dosx.h"
#include "vibios.h"
#include <stddef.h>

#ifdef __OS2V2__
    #define SEG16   _Seg16
    #define STUPID_UINT     unsigned long
#else
    #define SEG16
    #define STUPID_UINT     unsigned short
#endif

extern int      PageCnt;

static char     oldPath[_MAX_PATH];
static char     oldDisk;

int FileSysNeedsCR( int handle )
{
    return( true );
}

/*
 * PushDirectory - save the current directory
 */
void PushDirectory( char *orig )
{
    STUPID_UINT         c;
    unsigned long       map;

    oldPath[0] = 0;
    DosQCurDisk( &c, &map );
    oldDisk = (char) c;
    if( orig[1] == ':' ) {
        ChangeDrive( orig[0] );
    }
    GetCWD2( oldPath, sizeof( oldPath ) );

} /* PushDirectory */

/*
 * PopDirectory - restore the last directory
 */
void PopDirectory( void )
{
    if( oldPath[0] != 0 ) {
        ChangeDirectory( oldPath );
    }
    DosSelectDisk( oldDisk );
    ChangeDirectory( CurrentDirectory );

} /* PopDirectory */

/*
 * NewCursor - change cursor to insert mode type
 */
void NewCursor( window_id id, cursor_type ct )
{
    int                 base,nbase;
    VIOCURSORINFO       vioCursor;

    id = id;
    VioGetCurType( &vioCursor, 0 );
    base = vioCursor.cEnd;
    nbase = (base * (int)(100 - ct.height)) / 100;
    BIOSNewCursor( (char) nbase, base - 1 );

} /* NewCursor */

/*
 * MyBeep - ring beeper
 */
void MyBeep( void )
{
    if( EditFlags.BeepFlag ) {
    }

} /* MyBeep */

/*
 * ScreenInit - get screen info
 */
void ScreenInit( void )
{
    unsigned short              solvb;
    struct _VIOCONFIGINFO       config;
    struct _VIOMODEINFO         vioMode;
    void * SEG16                ptr;

    /* Set the cb member of VIOMODEINFO/VIOCONFIGINFO to smaller values
     * in order to be backward compatible with old OS/2 versions.
     */
    vioMode.cb = offsetof( VIOMODEINFO, buf_addr );
    if( VioGetMode( &vioMode, 0 ) != 0 ) {
        StartupError( ERR_WIND_INVALID );
    }
    EditVars.WindMaxWidth = vioMode.col;
    EditVars.WindMaxHeight = vioMode.row;

    config.cb = offsetof( VIOCONFIGINFO, Configuration );
    if( VioGetConfig( 0, &config, 0 ) != 0 ) {
        StartupError( ERR_WIND_INVALID );
    }
    if( config.display == 3 ) {
        EditFlags.BlackAndWhite = true;
    } else {
        if( config.adapter == 0 ) {
            EditFlags.Monocolor = true;
        } else {
            EditFlags.Color = true;
        }
    }

    VioGetBuf( (PULONG) &ptr, (PUSHORT) &solvb, 0);
    Scrn = ptr;
    ScreenPage( 0 );

} /* ScreenInit */

/*
 * ScreenFini
 */
void ScreenFini( void )
{
} /* ScreenFini */

/*
 * ChkExtendedKbd - look for extended keyboard type
 */
void ChkExtendedKbd( void )
{
    EditVars.ExtendedKeyboard = 0x10;

} /* ChkExtendedKbd */

/*
 * MemSize - return amount of dos memory left (in 16 byte paragraphs)
 */
long MemSize( void )
{
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
    char        a;
    unsigned    b;
    a = (char) tolower( drive ) - (char) 'a';
    b = a + 1;
    if( DosSelectDisk( b ) ) {
        return( ERR_NO_SUCH_DRIVE );
    }
    return( ERR_NO_ERR );

}/* ChangeDrive */

/*
 * ShiftDown - test if shift key is down
 */
bool ShiftDown( void )
{
    KBDINFO     ki;

    ki.cb = sizeof( KBDINFO );
    KbdGetStatus( &ki, 0 );
    if( ki.fsState & KEY_SHIFT ) {
        return( true );
    }
    return( false );

} /* ShiftDown */

static bool hadCapsLock;

/*
 * TurnOffCapsLock - switch off caps lock
 */
void TurnOffCapsLock( void )
{
    KBDINFO     ki;

    ki.cb = sizeof( KBDINFO );
    KbdGetStatus( &ki, 0 );
    if( ki.fsState & KEY_CAPS_LOCK ) {
        hadCapsLock = true;
        ki.fsMask |= 0x10;
        ki.fsState &= ~KEY_CAPS_LOCK;
        KbdSetStatus( &ki, 0 );  /* OS/2 2.0 FUCKS UP IF YOU DO THIS */
    } else {
        hadCapsLock = false;
    }

} /* TurnOffCapsLock */

/*
 * DoGetDriveType - get the type of drive A-Z
 */
drive_type DoGetDriveType( int drv )
{
    STUPID_UINT disk;
    ULONG       map;
    int         i;

    DosQCurDisk( &disk, &map );
    for( i = 'A'; i <= 'Z'; i++ ) {
        if( drv == i ) {
            if( map & 1 ) {
                return( DRIVE_IS_FIXED );
            } else {
                return( DRIVE_NONE );
            }
        }
        map >>= 1;
    }
    return( DRIVE_NONE ); // to quiet the compiler

} /* DoGetDriveType */

/*
 * MyDelay - delay a specified number of milliseconds
 */
void MyDelay( int ms )
{
    DosSleep( ms );

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
    unsigned short  key;
    int             scan;
    bool            shift;

    key = BIOSGetKeyboard( &scan );
    shift = ShiftDown();
    key &= 0xff;
    if( key == 0xE0 && scan != 0 ) {
        key = 0;
    }
    return( GetVIKey( key, scan, shift ) );
}

bool KeyboardHit( void )
{
    return( BIOSKeyboardHit() );
}

void MyVioShowBuf( unsigned offset, unsigned nchars )
{
    BIOSUpdateScreen( offset, nchars );
}

