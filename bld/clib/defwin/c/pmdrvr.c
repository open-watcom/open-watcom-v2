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
* Description:  OS/2 default windowing support main window procedure.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include "win.h"
#include "pmmenu.h"

static char  DefaultAboutTitle[] = "About Open Watcom Default Windowing System";
static char  DefaultAboutMsg[] = "   Open Watcom Default Window System\n\n                Version 1.0\n\n Portions Copyright (c) 1991-2002 Sybase, Inc.";
static char  *AboutTitle = DefaultAboutTitle;
static char  *AboutMsg = DefaultAboutMsg;

extern HWND _GetWinMenuHandle( void );
extern void _ResizeWindows( void );

#define DISPLAY(x)      WinMessageBox( HWND_DESKTOP, NULL, x, "Error", 0, MB_APPLMODAL | MB_NOICON | MB_OK | MB_MOVEABLE );
#define CTRL_C          0x03
#define CTRL_CONST      ( 'A' - 1 )

int     _SetAboutDlg( char *title, char *text ) {
//===============================================

        if( title ) {
            if( DefaultAboutTitle != AboutTitle ) {
                _MemFree( AboutTitle );
            }
            AboutTitle = _MemAlloc( FARstrlen( title ) + 1 );
            if( !AboutTitle ) return( 0 );
            FARstrcpy( AboutTitle, title );
        }
        if( text ) {
            if( DefaultAboutMsg != AboutMsg ) {
                _MemFree( AboutMsg );
            }
            AboutMsg = _MemAlloc( FARstrlen( text ) + 1 );
            if( !AboutMsg ) return( 0 );
            FARstrcpy( AboutMsg, text );
        }
        return( 1 );
}

static  USHORT  _VirtualKey( MPARAM mp1, MPARAM mp2 ) {
//====================================================

    unsigned short      vk;

    if( SHORT1FROMMP( mp1 ) & KC_VIRTUALKEY ) {
        if( SHORT2FROMMP( mp2 ) == VK_SPACE ) {
            return( CHAR1FROMMP( mp2 ) );
        } else {
            return( SHORT2FROMMP( mp2 ) );
        }
    } else {
        #ifdef _MBCS
            if( SHORT1FROMMP(mp1) == KC_CHAR ) {    /* double-byte char */
                vk = SHORT1FROMMP( mp2 );
            } else {                                /* single-byte char */
                vk = CHAR1FROMMP( mp2 );
                /* Check for control characters and map them appropriately */
                if((SHORT1FROMMP(mp1) & KC_CTRL) && iscntrl(toupper(vk) - CTRL_CONST))
                    vk = toupper(vk) - CTRL_CONST;
            }
            return( vk );
        #else
            vk = CHAR1FROMMP( mp2 );
            /* Check for control characters and map them appropriately */
            if((SHORT1FROMMP(mp1) & KC_CTRL) && iscntrl(toupper(vk) - CTRL_CONST))
                vk = toupper(vk) - CTRL_CONST;
            return( vk );
        #endif
    }
}

/*
 * _MainWindowProc - message handler for the frame window
 */
static MRESULT _MainWindowProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
    LPWDATA     w;
    USHORT      vk;
    HPS         hps;
    RECTL       rcl;
    USHORT      dlg_id;
    SWP         swps;
    char        scan;

    switch( msg ) {
    case WM_PAINT:
        hps = WinBeginPaint( hwnd, NULL, &rcl );
        WinFillRect( hps, &rcl, CLR_DARKGRAY );
        WinEndPaint( hps );
        break;
    case WM_WINDOWPOSCHANGED:
        if ( ( ( SWP *)( mp1 ) )->fl & ( SWP_SIZE | SWP_NOADJUST |
                                         SWP_MINIMIZE | SWP_MAXIMIZE |
                                         SWP_RESTORE ) ) {
            _ResizeWindows();
        }
        break;
    case WM_COMMAND:
        dlg_id = SHORT1FROMMP( mp1 );

        if ( dlg_id >= DID_WIND_STDIO ) {
            w = _GetActiveWindowData();
            w = _IsWindowedHandle( dlg_id - DID_WIND_STDIO );
            if( w != NULL ) {
                _MakeWindowActive( w );
                WinQueryWindowPos( w->frame, &swps );
                if ( swps.fl & SWP_MINIMIZE ) {
                    WinSetWindowPos( w->frame, swps.hwndInsertBehind, swps.x,
                                swps.y, swps.cx, swps.cy, SWP_RESTORE );
                }
            }
            break;
        }
        switch( dlg_id ) {
        case DID_FILE_SAVE:
            w = _GetActiveWindowData();
            if( w != NULL ) {
                _SaveAllLines( w );
            }
            break;
        case DID_FILE_CLEAR:
            _GetClearInterval();
            break;
        case DID_FILE_EXIT:
            WinSendMsg( _MainFrameWindow, WM_CLOSE, 0, 0 );
            break;
        case DID_EDIT_CLEAR:
            w = _GetActiveWindowData();
            if( w != NULL && !w->InputMode ) {
                if( w != NULL && !w->gphwin ) {
                    _FreeAllLines( w );
                    _ClearWindow( w );
                }
            }
            break;
        case DID_EDIT_COPY:
            w = _GetActiveWindowData();
            if( w != NULL && !w->gphwin ) {
                _CopyAllLines( w );
            }
            break;
        case DID_HELP_ABOUT:
            WinMessageBox( HWND_DESKTOP, hwnd, AboutMsg, AboutTitle, 0, MB_APPLMODAL | MB_INFORMATION | MB_OK | MB_MOVEABLE );
            break;
        }
        break;
    case WM_CHAR:
        w = _GetActiveWindowData();
        if( w == NULL )  break;
        vk = _VirtualKey( mp1, mp2 );
        if( SHORT1FROMMP( mp1 ) & KC_KEYUP ) {
            _WindowsKeyUp( vk, 0 );
        } else {
            if( (SHORT1FROMMP(mp1) & KC_VIRTUALKEY) &&
                (SHORT2FROMMP(mp2) != VK_SPACE) ) {
                scan = 0xff;
                if( SHORT2FROMMP( mp2 ) == VK_BREAK ) {
                    raise( SIGBREAK );
                    break;
                }
            } else {
                scan = CHAR4FROMMP( mp1 );
                if( ( SHORT1FROMMP( mp1 ) & KC_CTRL ) && ( vk == CTRL_C ) ) {
                    raise( SIGINT );
                    break;
                }
            }

            if( !(SHORT1FROMMP(mp1) & KC_VIRTUALKEY)  &&
                    (w==NULL || w->InputMode) ) {
                #ifdef _MBCS
                    if( vk & 0xFF00 ) {             /* double-byte char */
                        _WindowsKeyPush( vk&0x00FF, scan );
                        _WindowsKeyPush( (vk&0xFF00)>>8, scan );
                    } else                          /* single-byte char */
                        _WindowsKeyPush( vk, scan );
                #else
                    _WindowsKeyPush( vk, scan );
                #endif
                break;
            } else {
                WinShowPointer( HWND_DESKTOP, FALSE );
                switch( SHORT2FROMMP( mp2 ) ) {
                case VK_DOWN:
                    _MoveLineDown( w );
                    break;
                case VK_UP:
                    _MoveLineUp( w );
                    break;
                case VK_PAGEUP:
                    _MovePageUp( w );
                    break;
                case VK_PAGEDOWN:
                    _MovePageDown( w );
                    break;
                case VK_HOME:
                    _MoveToLine( w, 1, TRUE );
                    break;
                case VK_END:
                    _MoveToLine( w, _GetLastLineNumber( w ), TRUE );
                    break;
                default:
                    _WindowsVirtualKeyPush( vk, scan );
                    break;
                }
                WinShowPointer( HWND_DESKTOP, TRUE );
            }
        }
        return( (MRESULT)TRUE );

    case WM_SETFOCUS:
        w = _GetActiveWindowData();
        if( w != NULL ) {
            if( SHORT1FROMMP( mp2 ) ) {
                _ShowWindowActive( w, NULL );
            } else {
                _ShowWindowActive( NULL, w );
            }
            WinSetFocus( w->frame, FALSE );
            WinSetFocus( _MainWindow, TRUE );
        }
        return( 0 );

    default:
        return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
    }
    return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
}


MRESULT EXPENTRY _MainDriver( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 ) {
//============================================================================

    LPWDATA     w;
    LONG        height;
    LONG        width;
    RECT        rect;
    RECTL       rcl;
    HPS         hps;

    if( hwnd == _MainWindow ) {
        return( _MainWindowProc( hwnd, msg, mp1, mp2 ) );
    }
    w = _GetWindowData( hwnd );
    if( w == NULL ) {
        return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
    }

    switch( msg ) {
    case WM_COMMAND:
        break;
    case WM_FOCUSCHANGE:
        _MakeWindowActive( w );
        return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
    case WM_CLOSE:
        WinDestroyWindow( w->frame );
        return( 0 );
    case WM_DESTROY:
        WinSendMsg( _GetWinMenuHandle(), ( ULONG )MM_DELETEITEM,
                MPFROM2SHORT( ( w->handles[0] + DID_WIND_STDIO ), FALSE ), 0 );
        _DestroyAWindow( w );
        return( 0 );
        break;

    case WM_PAINT:
        hps = WinBeginPaint( hwnd, NULL, &rcl );
        _SelectFont( hps );
        rect.left = rcl.xLeft;
        rect.top = w->y1 - rcl.yTop;
        rect.bottom = w->y1 - rcl.yBottom;
        rect.right = rcl.xRight;
        WinFillRect( hps, &rcl, CLR_WHITE );
        _RepaintWindow( w, &rect, hps );
        WinEndPaint( hps );
        break;

    case WM_SIZE:
        if( w->resizing ) {
            w->resizing = FALSE;
            break;
        }
        WinQueryWindowRect( hwnd, &rcl );
        width = SHORT1FROMMP( mp2 );
        height = SHORT2FROMMP( mp2 );
        _ResizeWin( w, rcl.xLeft, rcl.yTop, rcl.xLeft+width, rcl.yTop+height );
        _DisplayAllLines( w, FALSE );
        break;

    case WM_VSCROLL:
        WinShowPointer( HWND_DESKTOP, FALSE );
        switch( SHORT2FROMMP( mp2 ) ) {
        case SB_SLIDERPOSITION:
            _MoveToLine( w, _GetLineFromThumbPosition( w,
                            SHORT1FROMMP( mp2 ) ), TRUE  );
            break;
        case SB_PAGEDOWN:
            _MovePageDown( w );
            break;
        case SB_PAGEUP:
            _MovePageUp( w );
            break;
        case SB_LINEDOWN:
            _MoveLineDown( w );
            break;
        case SB_LINEUP:
            _MoveLineUp( w );
            break;
        }
        WinShowPointer( HWND_DESKTOP, TRUE );
        break;

    default:
        return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
    }
    return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
}
