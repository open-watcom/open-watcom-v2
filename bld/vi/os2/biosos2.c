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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vi.h"
#include "pragmas.h"
#define INCL_BASE
#define INCL_VIO
#include "os2.h"

#pragma off (unreferenced);
char In61( void ) { return( 0 ); }
void Out61( char a ) {}
void Out43( char a ) {}
void Out42( char a ) {}
#pragma on (unreferenced);

#ifdef __386__
    #define SEG16 _Seg16
#else
    #define SEG16
#endif

typedef void * SEG16 ptr_16;
typedef struct {
   USHORT  cb;
   USHORT  type;
   USHORT  firstcolorreg;
   USHORT  numcolorregs;
   ptr_16  colorregaddr;
} THUNKEDVIO;

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
    return( ( (long)data.r << 8 ) | ( (long)data.g << 24 ) | ( (long)data.b << 16 ) );
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

void BIOSGetColorPalette( char _FAR *palette )
{
    VIOPALSTATE         *pal_state;
    USHORT              size, i;

    size = sizeof( VIOPALSTATE ) + sizeof( USHORT )*(MAX_COLOR_REGISTERS - 1);
    pal_state = MemAlloc( size );
    pal_state->cb = size;
    pal_state->type = 0;
    pal_state->iFirst = 0;
    VioGetState( pal_state, 0 );
    for( i = 0; i <= MAX_COLOR_REGISTERS; i++ ) {
        palette[ i ] = pal_state->acolor[ i ];
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

void BIOSSetNoBlinkAttr()
{
    setIntensity( 0 );
}

void BIOSSetBlinkAttr()
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
    USHORT      r,c;
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

extern short BIOSGetKeyboard( char x)
{
    KBDKEYINFO  info;
    short       res;

    x = x;
    KbdCharIn( &info, 0, 0 );
    res = (info.chScan << 8) + info.chChar;
    return( res );

} /* BIOSGetKeyboard */

extern short BIOSKeyboardHit( char x )
{
    KBDKEYINFO  info;

    x = x;
    KbdPeek( &info, 0 );
    return( ( info.fbStatus & 0xe0 ) != 0 );

} /* BIOSKeyboardHit */

void MyVioShowBuf( unsigned short offset, unsigned short length )
{
    extern int  PageCnt;

    if( PageCnt > 0 ) {
        return;
    }
    VioShowBuf( offset, length*2, 0 );

} /* MyVioShowBuf */

extern long DosGetFullPath( char *old, char *full )
{
#ifdef __386__
    DosQueryPathInfo( old, FIL_QUERYFULLNAME, full, _MAX_PATH );
#else
    strcpy( full, old );
#endif
    return( 0L );
} /* DosGetFullPath */
