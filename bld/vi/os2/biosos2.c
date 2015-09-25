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
* Description:  BIOS emulation routines for OS/2.
*
****************************************************************************/


#include "vi.h"
#define INCL_BASE
#define INCL_VIO
#include <os2.h>
#include "win.h"
#include "vibios.h"

#ifdef _M_I86
    #define SEG16
#else
    #define SEG16 _Seg16
#endif

typedef void * SEG16 ptr_16;
typedef struct {
   USHORT  cb;
   USHORT  type;
   USHORT  firstcolorreg;
   USHORT  numcolorregs;
   ptr_16  colorregaddr;
} THUNKEDVIO;

extern int  PageCnt;

long BIOSGetColorRegister( short reg )
{
    THUNKEDVIO  vcr;
    struct { char r; char g; char b; } data;

    vcr.cb = sizeof( vcr );
    vcr.type = 3;
    vcr.firstcolorreg = reg;
    vcr.numcolorregs = 1;
    vcr.colorregaddr = (ptr_16)&data;
    VioGetState( &vcr, 0 );
    return( ((long)data.r << 8) | ((long)data.g << 24) | ((long)data.b << 16) );
}

void BIOSSetColorRegister( short reg, char r, char g, char b )
{
    THUNKEDVIO  vcr;
    struct { char r; char g; char b; } data;

    data.r = r;
    data.g = g;
    data.b = b;
    vcr.cb = sizeof( vcr );
    vcr.type = 3;
    vcr.firstcolorreg = reg;
    vcr.numcolorregs = 1;
    vcr.colorregaddr = (ptr_16)&data;
    VioSetState( &vcr, 0 );
}

void BIOSGetColorPalette( void _FAR *palette )
{
    VIOPALSTATE         *pal_state;
    USHORT              size, i;
    char _FAR           *pal = palette;

    size = sizeof( VIOPALSTATE ) + sizeof( USHORT ) * (MAX_COLOR_REGISTERS - 1);
    pal_state = MemAlloc( size );
    pal_state->cb = size;
    pal_state->type = 0;
    pal_state->iFirst = 0;
    VioGetState( pal_state, 0 );
    for( i = 0; i <= MAX_COLOR_REGISTERS; i++ ) {
        pal[i] = pal_state->acolor[i];
    }
    MemFree( pal_state );
}

static void setIntensity( int value )
{
    VIOINTENSITY        vio_int;

    vio_int.cb = sizeof( vio_int );
    vio_int.type = 2;
    vio_int.fs = value;
    VioSetState( &vio_int, 0 );
}

void BIOSSetNoBlinkAttr( void )
{
    setIntensity( 0 );
}

void BIOSSetBlinkAttr( void )
{
    setIntensity( 1 );
}

void BIOSSetCursor( char page, char row, char col )
{
    page = page;
    VioSetCurPos( row, col, 0);

} /* BIOSSetCursor */

short BIOSGetCursor( char page )
{
    USHORT      r, c;
    short       res;

    page = page;
    VioGetCurPos( &r, &c, 0 );
    res = (r << 8) + c;
    return( res );

} /* BIOSGetCursor */

void BIOSNewCursor( char ch, char notused )
{
    VIOCURSORINFO       vioCursor;

    notused = notused;
    VioGetCurType( &vioCursor, 0 );
    vioCursor.yStart = ch;
    VioSetCurType( &vioCursor, 0);

} /* BIOSNewCursor */

/*
 * BIOSGetKeyboard - get a keyboard char
 */
unsigned BIOSGetKeyboard( unsigned *scan )
{
    KBDKEYINFO      info;

    KbdCharIn( &info, 0, 0 );
    if( scan != NULL ) {
        *scan = info.chScan;
    }
    if( info.chChar == 0xe0 && info.chScan != 0 ) {
        return( 0 );
    }
    return( info.chChar );

} /* BIOSGetKeyboard */

/*
 * BIOSKeyboardHit - test for keyboard hit
 */
bool BIOSKeyboardHit( void )
{
    KBDKEYINFO  info;

    KbdPeek( &info, 0 );
    return( (info.fbStatus & 0xe0) != 0 );

} /* BIOSKeyboardHit */

/*
 * BIOSUpdateScreen - update the screen
 */
void  BIOSUpdateScreen( unsigned offset, unsigned nchars )
{
    if( PageCnt > 0 ) {
        return;
    }
    VioShowBuf( offset * sizeof( char_info ), nchars * sizeof( char_info ), 0 );

} /* BIOSUpdateScreen */

