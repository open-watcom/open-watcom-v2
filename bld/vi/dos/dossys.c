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
* Description:  General DOS system helper routines for VI
*
****************************************************************************/


#include "vi.h"
#include <dos.h>
#include "win.h"
#include "dosx.h"
#include "vibios.h"
#include "pragmas.h"

extern void UpdateDOSClock( void );

#define PHAR_SCRN_SEL   0x34
extern int PageCnt;

static char oldPath[_MAX_PATH];
static char oldDrive;

int FileSysNeedsCR( int handle )
{
    return( true );
}

/*
 * PushDirectory
 */
void PushDirectory( const char *orig )
{
    unsigned    c;

    oldPath[0] = 0;
    _dos_getdrive( &c );
    oldDrive = (char)c;
    if( orig[1] == ':' ) {
        ChangeDrive( orig[0] );
    }
    GetCWD2( oldPath, _MAX_PATH );

} /* PushDirectory */

/*
 * PopDirectory
 */
void PopDirectory( void )
{
    unsigned    total;

    if( oldPath[0] != 0 ) {
        ChangeDirectory( oldPath );
    }
    _dos_setdrive( oldDrive, &total );
    ChangeDirectory( CurrentDirectory );

} /* PopDirectory */

/*
 * NewCursor - change cursor to insert mode type
 */
void NewCursor( window_id id, cursor_type ct )
{
    int base, nbase;

    id = id;
    if( EditFlags.Monocolor ) {
        base = 14;
    } else {
        base = 16;
    }
    nbase = (base * (int)(100 - ct.height)) / 100;
    BIOSNewCursor( (char) nbase, base - 1 );

} /* NewCursor */

#if 0

/*
 * noteOn - turn beeper on to a specific frequency
 */
static void noteOn( int freq )
{
    unsigned char   pbstate;

    /*
     * beeper on
     */
    pbstate = In61();
    pbstate |= 3;
    Out61( pbstate );

    /*
     * set note - lsb, then msb
     */
    Out43( 0xb6 );
    Out42( freq & 0xFF );
    Out42( freq / 256 );

} /* noteOn */

/*
 * noteOff - turn beeper off
 */
static void noteOff( void )
{
    unsigned char   pbstate;

    pbstate = In61();
    pbstate &= 0xFC;
    Out61( pbstate );

} /* noteOff */

#endif

/*
 * MyBeep - ring beeper
 */
void MyBeep( void )
{
    int             i;
    int             j = 0;
    unsigned char   pbstate;

    if( EditFlags.BeepFlag ) {
        pbstate = In61();
        pbstate |= 3;
        Out61( pbstate );
        Out43( 0xb6 );
        Out42( 3000 & 0xFF );
        Out42( 3000 / 256 );

        for( i = 1; i < 15000; i++ ) {
            j++;
        }

        pbstate = In61();
        pbstate &= 0xFC;
        Out61( pbstate );
    }

} /* MyBeep */

static void getExitAttr( void )
{
    short       cursor;
    short       x, y;

    cursor = BIOSGetCursor( VideoPage );
    x = cursor >> 8;
    y = cursor & 0xff;
    EditVars.ExitAttr = Scrn[y * EditVars.WindMaxWidth + x].cinfo_attr;
}

/*
 * ScreenInit - get screen info
 */
void ScreenInit( void )
{
    unsigned long       x;
    U_INT               y;

    x = BIOSGetVideoMode();
    y = (U_INT) x;
    EditVars.WindMaxWidth = (y >> 8);
    y &= 0xff;
    VideoPage = (U_INT) (x >> 24);

    /*
     * mode _ get apropos screen ptr
     */
    if( y == 0x07 ) {
        EditFlags.Monocolor = true;
    } else if( y == 0x00 || y == 0x02 ) {
        EditFlags.BlackAndWhite = true;
    } else {
        EditFlags.Color = true;
    }
    ScreenPage( 0 );
    EditVars.WindMaxHeight = BIOSGetRowCount() + 1;
    getExitAttr();

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
#define RSH( x )    (((x) & 0x0002) != 0)
#define LSH( x )    (((x) & 0x0002) != 0)
#define CT( x )     (((x) & 0x0004) != 0)
#define AL( x )     (((x) & 0x0008) != 0)
#define LCT( x )    (((x) & 0x0100) != 0)
#define LAL( x )    (((x) & 0x0200) != 0)
#define RCT( x )    (((x) & 0x0400) != 0)
#define RAL( x )    (((x) & 0x0800) != 0)

    unsigned    x;

    EditVars.ExtendedKeyboard = 0;

    x = BIOSTestKeyboard();
    if( (x & 0xff) == 0xff ) {
        return; /* too many damn keys pressed! */
    }

    if( AL( x ) != (RAL( x ) || LAL( x )) ) {
        return;
    }
    if( CT( x ) != (RCT( x ) || LCT( x )) ) {
        return;
    }
    EditVars.ExtendedKeyboard = 0x10;

} /* ChkExtendedKbd */

/*
 * MemSize - return amount of dos memory left (in 16 byte paragraphs)
 */
long MemSize( void )
{
    short       x;

    x = DosMaxAlloc();
#ifdef _M_I86
    return( 16L * (long) x );
#else
    return( 4096L * (long) x );
#endif

} /* MemSize */

/*
 * ScreenPage - set the screen page to active/inactive
 */
void ScreenPage( int page )
{
#if defined( _M_I86 )
    unsigned long       a;
    unsigned long       b;

    if( !EditFlags.Monocolor ) {
        Scrn = (char_info *)0xb8000000;
    } else {
        Scrn = (char_info *)0xb0000000;
    }
    a = *(unsigned short *)MK_FP( 0x40,0x4e ) / sizeof( char_info );
    Scrn += a;
    PageCnt += page;
    if( PageCnt > 0 ) {
        b = (unsigned long)( ( EditVars.WindMaxWidth + 1 ) * ( EditVars.WindMaxHeight + 1 ) );
        if( a + b < 0x8000L / sizeof( char_info ) ) {
            Scrn += b;
        }
        EditFlags.NoSetCursor = true;
    } else {
        EditFlags.NoSetCursor = false;
    }
#elif defined( __4G__ )
    unsigned long       a;
    unsigned long       b;

    if( !EditFlags.Monocolor ) {
        Scrn = (char_info *)0xb8000;
    } else {
        Scrn = (char_info *)0xb0000;
    }
    a = *(unsigned short *)0x44e / sizeof( char_info );
    Scrn += a;
    PageCnt += page;
    if( PageCnt > 0 ) {
        b = (unsigned long)( ( EditVars.WindMaxWidth + 1 ) * ( EditVars.WindMaxHeight + 1 ) );
        if( a + b < 0x8000L / sizeof( char_info ) ) {
            Scrn += b;
        }
        EditFlags.NoSetCursor = true;
    } else {
        EditFlags.NoSetCursor = false;
    }
#else
    unsigned long       a;
    unsigned long       b;
    unsigned long       c;

    if( !EditFlags.Monocolor ) {
        c = 0xb8000;
    } else {
        c = 0xb0000;
    }
    a = *(unsigned short _FAR *)MK_FP( PHAR_SCRN_SEL, 0x44e );
    c += a;
    PageCnt += page;
    if( PageCnt > 0 ) {
        b = (unsigned long)( ( EditVars.WindMaxWidth + 1 ) * ( EditVars.WindMaxHeight + 1 ) * sizeof( char_info ) );
        if( a + b < 0x8000L ) {
            c += b;
        }
        EditFlags.NoSetCursor = true;
    } else {
        EditFlags.NoSetCursor = false;
    } /* if */
    Scrn = MK_FP( PHAR_SCRN_SEL, c );
#endif

} /* ScreenPage */

/*
 * ChangeDrive - change the working drive
 */
vi_rc ChangeDrive( int drive )
{
    char        a;
    unsigned    b;
    unsigned    total, c;

    a = (char) tolower( drive ) - (char) 'a';
    b = a + 1;
    _dos_setdrive( b, &total );
    _dos_getdrive( &c );
    if( b != c ) {
        return( ERR_NO_SUCH_DRIVE );
    }
    return( ERR_NO_ERR );

}/* ChangeDrive */

#if defined( _M_I86 ) || defined( __4G__ )
    #define KEY_PTR (char *) 0x00400017;
#else
    #define KEY_PTR MK_FP( PHAR_SCRN_SEL, 0x417 );
#endif

/*
 * ShiftDown - test if shift key is down
 */
bool ShiftDown( void )
{
    char _FAR   *kptr;

    kptr = KEY_PTR;
    if( kptr[0] & KEY_SHIFT ) {
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
    char _FAR   *kptr;

    kptr = KEY_PTR;

    if( kptr[0] & KEY_CAPS_LOCK ) {
        hadCapsLock = true;
        kptr[0] &= ~KEY_CAPS_LOCK;
    } else {
        hadCapsLock = false;
    }

} /* TurnOffCapsLock */

extern short CheckRemovable( char );
#pragma aux CheckRemovable = \
        "mov    ax, 04408h" \
        "int    021h" \
        "cmp    ax, 0fh" \
        "jne    ok" \
        "mov    ax, 0" \
        "jmp    done" \
        "ok:    inc ax" \
        "done:" \
    parm [bl] value[ax];

/*
 * DoGetDriveType - get the type of drive A-Z
 */
drive_type DoGetDriveType( int drv )
{
    return( CheckRemovable( drv - 'A' + 1 ) );

} /* DoGetDriveType */

/*
 * MyDelay - delay a specified number of milliseconds
 */
void MyDelay( int ms )
{
    int         final_ticks;

    final_ticks = ClockTicks + ((ms * 182L + 5000L) / 10000L);
    while( ClockTicks < final_ticks );

} /* MyDelay */

/*
 * SetCursorBlinkRate - set the current blink rate for the cursor
 */
void SetCursorBlinkRate( int cbr )
{
    EditVars.CursorBlinkRate = cbr;

} /* SetCursorBlinkRate */

/*
 * KeyboardHit - test for keyboard hit
 */
bool KeyboardHit( void )
{
    bool        rc;

    rc = _BIOSKeyboardHit( EditVars.ExtendedKeyboard + 1 );
    if( !rc ) {
#if !( defined( _M_I86 ) || defined( __4G__ ) )
        UpdateDOSClock();
#endif
        JustAnInt28();
    }
    return( rc );

} /* KeyboardHit */

/*
 * GetKeyboard - get a keyboard char
 */
vi_key GetKeyboard( void )
{
    unsigned    code;
    unsigned    scan;
    bool        shift;

    code = _BIOSGetKeyboard( EditVars.ExtendedKeyboard );
    shift = ShiftDown();
    scan = code >> 8;
    code &= 0xff;
    if( code == 0xE0 && scan != 0 ) {
        code = 0;
    }
    return( GetVIKey( code, scan, shift ) );

} /* GetKeyboard */
