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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <conio.h>
#include <malloc.h>
#include "vi.h"
#include "win.h"
#include "dosx.h"
#include "pragmas.h"
#ifdef __CURSES__
#include "curses.h"
#endif

static int saveRow,saveCol;
int PageCnt=0;

/*
 * getCursor - get cursor position
 */
static void getCursor( int *row, int *col )
{
    int x;

    x = BIOSGetCursor( VideoPage );
    *row = (x >> 8);
    *col = x & 0xff;
} /* getCursor */

/*
 * SetCursorOnScreen - set cursor position
 */
void SetCursorOnScreen( int row, int col )
{
    if( EditFlags.Quiet || EditFlags.NoSetCursor ) {
        return;
    }
    BIOSSetCursor( VideoPage, row, col );
} /* SetCursorOnScreen */

/*
 * KillCursor - do just that, get rid of it
 */
void KillCursor( void )
{
    getCursor( &saveRow,&saveCol );
    SetCursorOnScreen( WindMaxHeight,0 );

} /* KillCursor */

#if 0
/*
 * TurnOffCursor - as it sounds
 */
void TurnOffCursor( void )
{
    SetCursorOnScreen( WindMaxHeight,0 );

} /* TurnOffCursor */
#endif

/*
 * RestoreCursor - bring back a killed cursor
 */
void RestoreCursor( void )
{
    SetCursorOnScreen( saveRow,saveCol );

} /* RestoreCursor */

#if !defined(__QNX__) && !defined(__OS2__) && !defined(__NT__)
    extern void JustAnInt28( void );
    #pragma aux JustAnInt28 = 0xcd 0x28;
#endif

/*
 * KeyboardHit - test for keyboard hit
 */
bool KeyboardHit( void )
{
    bool        rc;

    rc = BIOSKeyboardHit( EditFlags.ExtendedKeyboard + 1 );
#if !defined(__QNX__) && !defined(__OS2__) && !defined(__NT__)
    if( !rc ) {
        #ifdef __386__
            extern void UpdateDOSClock( void );
            UpdateDOSClock();
        #endif
        JustAnInt28();
    }
#endif
    return( rc );
} /* KeyboardHit */

/*
 * GetKeyboard - get a keyboard char
 */
int GetKeyboard( int *scan )
{
    U_INT       key;

    key = BIOSGetKeyboard( EditFlags.ExtendedKeyboard );
    if( scan != NULL ) {
        *scan = (key>>8);
    }
    key &= 0xff;
    if( key == 0xe0 ) {
        return( 0 );
    }
    return( key );

} /* GetKeyboard */

#if defined( __386__ ) && !defined( __4G__ )
static char     colorPalette[MAX_COLOR_REGISTERS+1] = {0,1,2,3,4,5,0x14,7,
                        0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x00 };
#else
static char     colorPalette[MAX_COLOR_REGISTERS+1];
#endif
static rgb      oldColors[MAX_COLOR_REGISTERS];
static rgb      newColors[MAX_COLOR_REGISTERS];
static bool     colorChanged[MAX_COLOR_REGISTERS];

/*
 * getColorRegister - as it sounds
 */
static void getColorRegister( int reg, rgb *c )
{
    long        res;

    res = BIOSGetColorRegister( colorPalette[reg] );
    c->red = ((char *)(&res))[1] << 2;
    c->blue = ((char *)(&res))[2] << 2;
    c->green = ((char *)(&res))[3] << 2;

} /* getColorRegister */

/*
 * setColorRegister - as it sounds
 */
static void setColorRegister( int reg, rgb *c )
{
    BIOSSetColorRegister( colorPalette[reg], c->red >> 2, c->green >> 2, c->blue >> 2 );

} /* setColorRegister */

/*
 * getColorPalette - as it sounds
 */
static void getColorPalette( char *p )
{
#if !defined( __386__ ) || defined( __4G__ ) || defined( __OS2__ )
    BIOSGetColorPalette( p );
#else
    p = p;
#endif

} /* getColorPalette */

/*
 * InitColors - set up default colors
 */
void InitColors( void )
{
    /*
     * set color palette (if in color mode)
     */
    if( EditFlags.Color && !EditFlags.Quiet ) {

        BIOSSetNoBlinkAttr();
        getColorPalette( colorPalette );

    }

} /* InitColors */

void ResetColors()
{
    int i;

    if( EditFlags.Color && !EditFlags.Quiet ) {
        BIOSSetNoBlinkAttr();
        for( i=0;i<MAX_COLOR_REGISTERS;i++ ) {
            if( colorChanged[i] ) {
                setColorRegister( i, &newColors[i] );
            }
        }
    }
}

/*
 * FiniColors - reset colors on exit
 */
void FiniColors( void )
{
    int i;

    if( EditFlags.Color && !EditFlags.Quiet ) {
        for( i=0;i<MAX_COLOR_REGISTERS;i++ ) {
            if( colorChanged[i] ) {
                setColorRegister( i, &oldColors[i] );
            }
        }
        BIOSSetBlinkAttr();
    }

} /* FiniColors */

int SetFont( char *data )
{
    data = data;
    return( ERR_NO_ERR );
}

/*
 * SetAColor - perform a setcolor command
 */
int SetAColor( void *data )
{
    rgb         c;
    int         clr;
    char        token[MAX_STR];

    if( NextWord1( data,token ) <= 0 ) {
        return( ERR_INVALID_SETCOLOR );
    }
    clr = atoi( token );
    if( NextWord1( data,token ) <= 0 ) {
        return( ERR_INVALID_SETCOLOR );
    }
    c.red = atoi( token );
    if( NextWord1( data,token ) <= 0 ) {
        return( ERR_INVALID_SETCOLOR );
    }
    c.green = atoi( token );
    if( NextWord1( data,token ) <= 0 ) {
        return( ERR_INVALID_SETCOLOR );
    }
    c.blue = atoi( token );
    if( !EditFlags.Quiet ) {
        if( !colorChanged[ clr ] ) {
            getColorRegister( clr, &oldColors[ clr ] );
            colorChanged[ clr ] = TRUE;
        }
        setColorRegister( clr, &c );
        newColors[ clr ] = c;
    }
    return( ERR_NO_ERR );

} /* SetAColor */

/*
 * GetNumColors - get # colors stored (in array newColors)
 */
int GetNumColors( void )
{
    return( MAX_COLOR_REGISTERS );
}

/*
 * GetColorSetting - get a specific color setting
 */
bool GetColorSetting( int clr, rgb *c  )
{
    if( !colorChanged[ clr ] ) {
        return( FALSE );
    }
    *c = newColors[ clr ];
    return( TRUE );

} /* GetColorSetting */

/*
 * ClearScreen - clear the screen
 */
void ClearScreen( void )
{
#ifdef __CURSES__
    clear();
#else
    int                 i;
    char_info           what;
    char_info           _FAR *foo;

    if( EditFlags.Quiet ) {
        return;
    }
    foo = (char_info _FAR *) Scrn;
    what.ch = ' ';
    what.attr = ExitAttr;
    for( i=WindMaxWidth*WindMaxHeight-1;i>=0;i-- ) {
        WRITE_SCREEN( *foo, what );
        foo++;
    }
#ifdef __VIO__
    MyVioShowBuf( 0, WindMaxWidth*WindMaxHeight );
#endif
#endif
    SetCursorOnScreen( 0,0 );
} /* ClearScreen */

/*
 * GetClockStart - get clock start position
 */
void GetClockStart( void )
{
    ClockStart = &Scrn[ sizeof(char_info)*(ClockX+ClockY*WindMaxWidth)];

} /* GetClockStart */

/*
 * GetSpinStart - get spinner start position
 */
void GetSpinStart( void )
{
    SpinLoc = &Scrn[ sizeof(char_info)*(SpinX+SpinY*WindMaxWidth)];

} /* GetSpinStart */
