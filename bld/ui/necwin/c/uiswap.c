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


/*
 * UISWAP.C : Code to allow swapping between windows graphical and character
 *            modes.
 *
 * Note: This code should really be a part of the UI library.  Thus, it
 *       goes in here.  But the calling order of the functions to
 *       initialize everything has to be just right.
 */

#include <windows.h>
#include <dos.h>
#include <string.h>

/*
 * Here are a couple of fun undocumented windows functions.
 * They are documented in the self-defeatingly titled Undocumented Windows,
 * where it is explained that these form part of the mechanism for
 * disabling the OEM layer when doing things like going to a DOS box.
 * By calling these lower level functions directly, we can simply disable
 * the screen driver, returning to text mode, without actually being in
 * DOS - we can still access windows.  Clever.
 */
extern void FAR PASCAL Death( HDC );
extern void FAR PASCAL Resurrection( HDC, WORD w1, WORD w2, WORD w3, WORD w4,
                                WORD w5, WORD w6 );
extern void far pascal SetEventHook( LPVOID );

extern void far HookRtn( unsigned event, unsigned info );

static int              screenX;
static int              screenY;
static HBITMAP          screenBitmap;
static HDC              screenDC;
static HDC              screenMemDC;

BOOL                    isGraphical = TRUE;
//BOOL                  HaveDeviceContexts = FALSE;

#pragma aux CRTsense = \
 "mov ah, 0Bh"\
 "int 18h"\
 value [al] modify [ax];
extern unsigned char CRTsense();
#pragma aux CRTset = \
 "mov ah, 0Ah"\
 "int 18h"\
 parm [al] modify [ax];
extern void CRTset( unsigned char a );
#pragma aux CRTstart = \
 "mov ah, 0Ch"\
 "int 18h"\
 modify [ah];
extern void CRTstart();
#pragma aux CRThalt = \
 "mov ah, 0Dh"\
 "int 18h"\
 modify [ah];
extern void CRThalt();
#pragma aux CRTpage = \
 "mov ah, 0Eh"\
 "int 18h"\
 parm [dx] modify [ah dx];
extern void CRTpage( unsigned short int d );

void YoureGoingDown()
{
    unsigned char mode;
    mode = CRTsense();
    mode &= 0xF3;
    CRTset( mode );
    CRTpage( 0 );
    CRTstart();
}

void WelcomeBack()
{
    CRThalt();
}

/*
 * For a semi description of what all these functions do, see ToCharacter.
 */
void ToGraphical( void )
{
    HBITMAP     old;
    if( isGraphical ) {
        return;
    }
    WelcomeBack();
    Resurrection( screenDC, 0, 0, 0, 0, 0, 0 );

    old = SelectObject( screenMemDC, screenBitmap );
    BitBlt( screenDC, 0, 0, screenX, screenY, screenMemDC, 0, 0, SRCCOPY );
    SelectObject( screenMemDC, old );

        /*
         * It would seem to me that we don't want to allocate and deallocate
         * the memory each time we switch modes.  We would rather keep the
         * device allocated and keep writing the screen to it.
         */
    DeleteObject( screenBitmap );
    DeleteDC( screenMemDC );
    ReleaseDC( NULL, screenDC );
    isGraphical = TRUE;
    SetEventHook( NULL );
}

void ToCharacter( void )
{
    HBITMAP     old;
    if( !isGraphical ) {
        return;
    }
    SetEventHook( &HookRtn );
    /*
     * GetDC( NULL ) returns an HDC handle which points to the device
     * context of the entire screen.
     */
    screenDC = GetDC( NULL );
//    if( !HaveDeviceContexts ) {
//      HaveDeviceContexts = TRUE;
        /*
         * CreateCompatibleDC( NULL ) created a memory-based virtual device
         * Identical to the system screen.  You can write to this device the way
         * you would to the screen, then display it all at once.
         */
        screenMemDC = CreateCompatibleDC( NULL );
        /*
         * CreateCompatibleBitmap here is creating a bitmap the same size as
         * the screen, compatible with the screen device itself.
         */
        screenBitmap = CreateCompatibleBitmap( screenDC, screenX, screenY );

//    }
    /*
     * I think this call copies the screen bitmap into a bitmap which is
     * pointed to by screenMemDC.  We then want to get rid of the old one.
     */
    old = SelectObject( screenMemDC, screenBitmap );
    /*
     * Here, we copy the bitmap information in screenDC directly into
     * the device screenMemDC.
     */
    BitBlt( screenMemDC, 0, 0, screenX, screenY, screenDC, 0, 0, SRCCOPY );

    SelectObject( screenMemDC, old );
    Death( screenDC );
    YoureGoingDown();
    isGraphical = FALSE;
}

static BOOL uiInitSwapper( )
{
    isGraphical = TRUE;
    screenX = GetSystemMetrics( SM_CXSCREEN );
    screenY = GetSystemMetrics( SM_CYSCREEN );
    return( FALSE );
}

static void uiFiniSwapper( void )
{
    if( !isGraphical ) {
        ToGraphical();
    }
//    if( HaveDeviceContexts ) {
//      DeleteObject( screenBitmap );
//      DeleteDC( screenMemDC );
//    }
}

int InitSwapper()
{
    if( uiInitSwapper() ) return( 1 );
    return( 0 );
}

int FiniSwapper()
{
    uiFiniSwapper();
    return( 0 );
}
