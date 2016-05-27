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


#include <windows.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "restest.h"
#include "res.h"


void Error( char *heading, char *msg ) {
    MessageBox( NULL, msg, heading, MB_OK );
}

/*
 * doLBPrintf - printf to a list box
 */
void LBPrintf( HWND lb, char *str, ... )
{
    char        tmp[256];
    va_list     al;

    va_start( al, str );
    vsprintf( tmp, str, al );
    SendMessage( lb, LB_ADDSTRING, 0, (LONG)(LPSTR)tmp );
    va_end( al );
} /* LBPrintf */

static char mkHexDigit( char ch ) {
    if( ch < 0xA ) return( '0' + ch );
    return( 'A' + ch - 0xA );
} /* MkHexDigit */

#define WIDTH           16
void LBDump( HWND hwnd, char *str, WORD len ) {
    char        buf[128];
    char        *cur;
    WORD        digit;
    WORD        cnt;

    cur = str;
    cnt = 0;
    digit = 0;
    memset( buf, ' ', sizeof( buf ) );
    while( cnt < len ) {
        buf[ 3 * digit ] = mkHexDigit( *cur >> 4 );
        buf[ 3 * digit + 1 ]  = mkHexDigit( *cur & 0xF );
        buf[ 3 * digit + 2 ]  = ' ';
        if( isprint( *cur ) ) {
            buf[ 3 * WIDTH + 1 + digit ] = *cur;
        } else {
            buf[ 3 * WIDTH + 1 + digit ] = '.';
        }
        digit ++;
        cur ++;
        cnt ++;
        if( digit == WIDTH || cnt == len ) {
            buf[ 4 * WIDTH + 1 ] = '\0';
            SendMessage( hwnd, LB_ADDSTRING, 0, (LPARAM)(LPCSTR) buf );
            memset( buf, ' ', sizeof( buf ) );
            digit = 0;
        }
    }
}


LRESULT CALLBACK MainWndProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    WORD        cmd;

    switch( msg ) {
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
        case TMENU_EXIT:
            SendMessage( hwnd, WM_DESTROY, 0, 0 );
            break;
        case TMENU_VERINFO:
            DisplayVerInfo();
            break;
        case TMENU_STRINGTBL:
            DisplayStringTable();
            break;
        case TMENU_MENU:
            DisplayMenu();
            break;
        case TMENU_DLG:
            DisplayDialog();
            break;
        case TMENU_RCDATA:
            DisplayData( true );
            break;
        case TMENU_USERDEF:
            DisplayData( false );
            break;
        case TMENU_CURSOR:
            DisplayCursor( hwnd );
            break;
        case TMENU_ICON:
            DisplayIcon( hwnd );
            break;
        case TMENU_ACCEL:
            DisplayAccel();
            break;
        case TMENU_BITMAP:
            DisplayBitmap();
            break;
#ifdef __NT__
        case TMENU_MSGTBL:
            DisplayMessageTable();
            break;
#endif
        case TMENU_FONT:
        default:
            MessageBox( hwnd, "NYI", "", MB_OK );
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( FALSE );
}
