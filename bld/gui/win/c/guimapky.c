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
* Description:  GUI library key mapping for OS/2 and Windows.
*
****************************************************************************/


#include "guiwind.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "guimapky.h"
#include "guixutil.h"
#include "guimkey.h"

extern  gui_window      *GUICurrWnd;
extern  bool            EditControlHasFocus;
extern  gui_keystate    KeyState;
#ifndef __OS2_PM__
static  bool            RetTrue = false; /* set to true of app returned
                                            true to last WM_SYSKEYDOWN or
                                            WM_SYSKEYUP message */
#endif

typedef struct {
    unsigned char   value;
    char            regular;
    char            shifted;
    gui_key         alt;
    gui_key         ctrl;
} keytable;

static keytable vk_table[] = {
    { 0xbc,     ',',    '<', GUI_KEY_ALT_COMMA,         0       },
    { 0xbe,     '.',    '>', GUI_KEY_ALT_PERIOD,        0       },
    { 0xbf,     '/',    '?', GUI_KEY_ALT_SLASH,         0       },
    { 0xba,     ';',    ':', GUI_KEY_ALT_SEMICOLON,     0       },
    { 0xde,     '\'',   '"', GUI_KEY_ALT_QUOTE,         0       },
    { 0xdb,     '[',    '{', GUI_KEY_ALT_LEFT_BRACKET,  GUI_KEY_CTRL_LEFT_BRACKET       },
    { 0xdd,     ']',    '}', GUI_KEY_ALT_RIGHT_BRACKET, GUI_KEY_CTRL_RIGHT_BRACKET      },
    { 0xdc,     '\\',   '|', GUI_KEY_ALT_BACKSLASH,     GUI_KEY_CTRL_BACKSLASH          },
    { 0xc0,     '`',    '~', GUI_KEY_ALT_BACKQUOTE,     0       },
    { 0xbd,     '-',    '_', GUI_KEY_ALT_MINUS,         GUI_KEY_CTRL_MINUS              },
    { 0xbb,     '=',    '+', GUI_KEY_ALT_EQUAL,         0       }
};

static char num_regular[] = "1234567890";
static char num_shifted[] = "!@#$%^&*()";

#ifndef __OS2_PM__
static char c_regular[]   = "1234567890`-=[]\\;',./";
static char c_shifted[]   = "!@#$%^&*()~_+{}|:\"<>?";
#else
static char c_regular[]   = "1234567890`-=[]\\;',./*+";
static char c_shifted[]   = "!@#$%^&*()~_+{}|:\"<>?*+";
#endif

static gui_key AltFunc[] =
{
    GUI_KEY_ALT_A,
    GUI_KEY_ALT_B,
    GUI_KEY_ALT_C,
    GUI_KEY_ALT_D,
    GUI_KEY_ALT_E,
    GUI_KEY_ALT_F,
    GUI_KEY_ALT_G,
    GUI_KEY_ALT_H,
    GUI_KEY_ALT_I,
    GUI_KEY_ALT_J,
    GUI_KEY_ALT_K,
    GUI_KEY_ALT_L,
    GUI_KEY_ALT_M,
    GUI_KEY_ALT_N,
    GUI_KEY_ALT_O,
    GUI_KEY_ALT_P,
    GUI_KEY_ALT_K,
    GUI_KEY_ALT_R,
    GUI_KEY_ALT_S,
    GUI_KEY_ALT_T,
    GUI_KEY_ALT_U,
    GUI_KEY_ALT_V,
    GUI_KEY_ALT_W,
    GUI_KEY_ALT_X,
    GUI_KEY_ALT_Y,
    GUI_KEY_ALT_Z
};

static bool convert_shiftkeys( WORD vk, gui_key *key,
                               char *regular, char *shifted )
{
    char        *str;

    str = strchr( regular, vk );
    if( CHK_KS_SHIFT ) {
        if( str != NULL ) {
            *key = *( shifted + ( str - regular ) );
            return( true );
        } else {
            str = strchr( shifted, vk );
            if( str != NULL ) {
                *key = *str;
                return( true );
            }
        }
    } else if( !CHK_KS_CTRL && !CHK_KS_ALT ) {
        if( str != NULL ) {
            *key = *str;
            return( true );
        }
    }
    return( false );
}

static bool discard_this_vk( WORD vk )
{
    bool        discard;

    discard = false;

    switch( vk ) {
    case VK_SHIFT:
    case VK_CONTROL:
    case VK_CAPITAL :
#ifndef __OS2_PM__
    case VK_MENU:
#endif
    case VK_NUMLOCK :
    case VK_PAUSE :
    case VK_LBUTTON :
    case VK_RBUTTON :
    case VK_MBUTTON :
    case VK_SNAPSHOT :
#ifdef __OS2_PM__
    case VK_ALT :
#else
    case VK_CLEAR :
    case VK_SELECT :
    case VK_CANCEL :
    case VK_EXECUTE :
    case VK_HELP :
    case VK_SEPARATOR :
#endif
        discard = true;
        break;
    }

    return( discard );
}

static bool convert_numeric( WORD ch, gui_key *key )
{
    int         t;

    if( isdigit( ch ) ) {
        if( CHK_KS_SHIFT ) {
            if( convert_shiftkeys( ch, key, num_regular, num_shifted ) ) return( true );
            *key = ch;
            return( true );
        } else if( CHK_KS_ALT ) {
            t = ch - '1';
            if( t == -1 ) {
                t = 9;
            }
            *key = GUI_KEY_ALT_1 + t;
            return( true );
        } else if( CHK_KS_CTRL ) {
            if( ch == '2' ) {
                *key = GUI_KEY_CTRL_2;
                return( true );
            } else if( ch == '6' ) {
                *key = GUI_KEY_CTRL_6;
                return( true );
            }
        } else {
            *key = ch;
            return( true );
        }
    }

    return( false );
}

static bool convert_alpha( WORD ch, gui_key *key )
{
    WORD        t;

    if( isalpha( ch ) ) {
        t = toupper( ch ) - 'A';
        if( CHK_KS_ALT ) {
            *key = AltFunc[t];
        } else if( CHK_KS_CTRL ) {
            *key = t + 1;
        } else if ( CHK_KS_SHIFT ) {
            *key = 'A'+ t;
        } else {
            *key = 'a' + t;
        }
        return( true );
    }

    return( false );
}

static bool convert_otherkeys( WORD vk, gui_key *key )
{
    return( convert_shiftkeys( vk, key, c_regular, c_shifted ) );
}

static bool convert_ascii( WORD ch, gui_key *key )
{
    if( convert_alpha( ch, key ) )
        return( true );
    if( convert_numeric( ch, key ) )
        return( true );
    if( convert_otherkeys( ch, key ) )
        return( true );
    return( false );
}

static bool convert_keytable( WORD vk, gui_key *key )
{
    int         i;

    for( i = 0; i < ( sizeof( vk_table ) / sizeof( vk_table[0] ) ); i++ ) {
        if( vk == vk_table[i].value ) {
            if( CHK_KS_SHIFT ) {
                *key = vk_table[i].shifted;
            } else if( CHK_KS_ALT ) {
                *key = vk_table[i].alt;
            } else if( CHK_KS_CTRL ) {
                *key = vk_table[i].ctrl;
            } else {
                *key = vk_table[i].regular;
            }
            return( true );
        }
    }
    return( false );
}

#ifndef __OS2_PM__
static bool convert_numpad( WORD vk, gui_key *key )
{
    if( ( vk >= VK_NUMPAD0 ) && ( vk <= VK_NUMPAD9 ) ) {
        *key = '0' + ( vk - VK_NUMPAD0 );
        return( true );
    }
    return( false );
}
#endif

static bool GUIConvertVirtKeyToGUIKey( WORD vk, gui_key *key )
{
    if( key == NULL ) {
        return( false );
    }

    *key = 0;

    if( discard_this_vk( vk ) ) {
        return( false );
#ifndef __OS2_PM__
    } else if( convert_numpad( vk, key ) ) {
        // do nothing
#endif
    } else {
        switch( vk ) {
        case VK_TAB:
            *key = GUIMapKey( GUI_KEY_TAB );
            break;
        case VK_BACK:
            *key= GUIMapKey( GUI_KEY_BACKSPACE );
            break;
        case VK_ESCAPE:
            *key= GUIMapKey( GUI_KEY_ESCAPE );
            break;
#ifdef __OS2_PM__
        case VK_ENTER:
#endif
        case VK_RETURN:
            *key= GUIMapKey( GUI_KEY_ENTER );
            break;
        case VK_HOME:
            *key= GUIMapKey( GUI_KEY_HOME );
            break;
        case VK_END:
            *key= GUIMapKey( GUI_KEY_END );
            break;
        case VK_UP:
            *key= GUIMapKey( GUI_KEY_UP );
            break;
        case VK_DOWN:
            *key= GUIMapKey( GUI_KEY_DOWN );
            break;
        case VK_LEFT:
            *key= GUIMapKey( GUI_KEY_LEFT );
            break;
        case VK_RIGHT:
            *key= GUIMapKey( GUI_KEY_RIGHT );
            break;
        case VK_INSERT:
            *key= GUIMapKey( GUI_KEY_INSERT );
            break;
        case VK_DELETE:
            *key= GUIMapKey( GUI_KEY_DELETE );
            break;
        case VK_PRIOR:
            *key= GUIMapKey( GUI_KEY_PAGEUP );
            break;
        case VK_NEXT:
            *key= GUIMapKey( GUI_KEY_PAGEDOWN );
            break;
        case VK_SPACE :
            *key = GUIMapKey( GUI_KEY_SPACE );
            break;
        case VK_F1 :
            *key = GUIMapKey( GUI_KEY_F1 );
            break;
        case VK_F2 :
            *key = GUIMapKey( GUI_KEY_F2 );
            break;
        case VK_F3 :
            *key = GUIMapKey( GUI_KEY_F3 );
            break;
        case VK_F4 :
            *key = GUIMapKey( GUI_KEY_F4 );
            break;
        case VK_F5 :
            *key = GUIMapKey( GUI_KEY_F5 );
            break;
        case VK_F6 :
            *key = GUIMapKey( GUI_KEY_F6 );
            break;
        case VK_F7 :
            *key = GUIMapKey( GUI_KEY_F7 );
            break;
        case VK_F8 :
            *key = GUIMapKey( GUI_KEY_F8 );
            break;
        case VK_F9 :
            *key = GUIMapKey( GUI_KEY_F9 );
            break;
        case VK_F10 :
            *key = GUIMapKey( GUI_KEY_F10 );
            break;
        case VK_F11 :
            *key = GUIMapKey( GUI_KEY_F11 );
            break;
        case VK_F12 :
            *key = GUIMapKey( GUI_KEY_F12 );
            break;
#ifdef __OS2_PM__
        case VK_BACKTAB:
            *key= GUI_KEY_SHIFT_TAB;
            break;
#else
        case VK_DECIMAL:
            *key = GUIMapKey( GUI_KEY_KP_PERIOD );
            break;
        case VK_ADD :
            *key = GUIMapKey( GUI_KEY_KP_PLUS );
            break;
        case VK_SUBTRACT :
            *key = GUIMapKey( GUI_KEY_KP_MINUS );
            break;
        case VK_DIVIDE :
            *key = GUIMapKey( GUI_KEY_KP_SLASH );
            break;
        case VK_MULTIPLY :
            *key = GUIMapKey( GUI_KEY_KP_ASTERISK );
            break;
#endif
        default:
            if( convert_keytable( vk, key ) ) {
                return( *key != 0 );
            }
            // the rest of this case assumes that vk is ascii
            if( !convert_ascii( vk, key ) )
                return( false );
            break;
        }
    }

    return( true );
}

/*
 *  GUISetKeyState
 */

void GUISetKeyState( void )
{
    int         vk_cap;
    bool        shift;
    bool        caplock;

    KeyState = GUI_KS_NONE;
    shift = _wpi_getkeystate( VK_SHIFT ) < 0;

    vk_cap = _wpi_getkeystate( VK_CAPITAL );
    /* vk_cap & 0x80 is caplock pressed.
     * vk_cap & 0x01 indicates and odd number of toggles since the system
     * was started.
     */
    caplock = ( ( vk_cap & 0x8000 ) && !( vk_cap & 0x0001 ) ) ||
              ( ( vk_cap & 0x0001 ) && !( vk_cap & 0x8000 ) );
    if( ( shift && !caplock ) || ( caplock && !shift ) ) {
        SET_KS_SHIFT;
    }
    if( _wpi_getkeystate( VK_CONTROL ) < 0 ) {
        SET_KS_CTRL;
    }
#ifndef __OS2_PM__
    if( _wpi_getkeystate( VK_MENU ) < 0 ) {
#else
    if( _wpi_getkeystate( VK_ALT ) < 0 ) {
#endif
        SET_KS_ALT;
    }
}

#ifndef __OS2_PM__
bool GUIWindowsMapKey( WPI_PARAM1 vk, WPI_PARAM2 data, gui_key *scan )
{
    if( scan == NULL ) {
        return( false );
    }
    GUISetKeyState();
    *scan = HIWORD( LOBYTE( data ) );
    return( GUIConvertVirtKeyToGUIKey( (WORD)vk, scan ) );
}

WPI_MRESULT GUIProcesskey( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam,
                           WPI_PARAM2 lparam )
{
    gui_window          *wnd = NULL;
    gui_key_state       key_state;
    gui_event           gui_ev;
    HWND                low_l;

    switch( msg ) {
    case WM_MENUCHAR :
        low_l = GET_WM_COMMAND_HWND( wparam, lparam );
        if( low_l == 0 && RetTrue ) {
            /* app used last WM_SYSMENU key and there are no menus open,
               so tell windows not to beep */
            return( MAKELONG( 0, 1 ) );
        } else {
#ifndef __OS2_PM__
            // if the last syskey was not recognized as a macro key
            // then lets check to see of the key press could have
            // activated the main menu
            wnd = GUIGetWindow( hwnd );
            if( wnd && wnd->parent == GUIGetRootWindow() ) {
                PostMessage(wnd->parent->root, WM_SYSCOMMAND, SC_KEYMENU, wparam);
                return( MAKELONG( 0, 1 ) );
            } else {
                return( _wpi_defwindowproc( hwnd, msg, wparam, lparam ) );
            }
#else
            return( _wpi_defwindowproc( hwnd, msg, wparam, lparam ) );
#endif
        }
    case WM_SYSKEYDOWN :
    case WM_SYSKEYUP :
        if( GUICurrWnd != NULL ) {
            GUIGetKeyState( &key_state.state );
            if( GUIWindowsMapKey( wparam, 0, &key_state.key ) ) {
                if( msg == WM_SYSKEYDOWN  ) {
                    gui_ev = GUI_KEYDOWN;
                } else {
                    gui_ev = GUI_KEYUP;
                }
                RetTrue = GUIEVENTWND( GUICurrWnd, gui_ev, &key_state );
                if( RetTrue ) {
                    return( 0l ); // app used key, don't send to windows
                }
            }
        }
        return( _wpi_defwindowproc( hwnd, msg, wparam, lparam ) );
    case WM_KEYUP :
        if( ( GUICurrWnd != NULL ) && !EditControlHasFocus ) {
            GUIGetKeyState( &key_state.state );
            if( GUIWindowsMapKey( wparam, lparam, &key_state.key ) ) {
                GUIEVENTWND( GUICurrWnd, GUI_KEYUP, &key_state );
            }
        }
        break;
    case WM_KEYDOWN :
        if( ( GUICurrWnd != NULL ) && !EditControlHasFocus ) {
            GUIGetKeyState( &key_state.state );
            if( GUIWindowsMapKey( wparam, lparam, &key_state.key ) ) {
                GUIEVENTWND( GUICurrWnd, GUI_KEYDOWN, &key_state );
            }
        }
        break;
    }
    return( (WPI_MRESULT)MAKELONG( 0, 1 ) );
}
#else

typedef struct {
    gui_key     key;
    char        scan;
} ctrlkey;

static ctrlkey ctrl_table[] = {
    GUI_KEY_CTRL_BACKSLASH,             43,
    GUI_KEY_CTRL_RIGHT_BRACKET,         27,
    GUI_KEY_CTRL_LEFT_BRACKET,          26,
    GUI_KEY_CTRL_6,                     7,
    GUI_KEY_CTRL_MINUS,                 12,
    GUI_KEY_CTRL_BACKSPACE,             14,
    GUI_KEY_CTRL_ENTER,                 28,
    GUI_KEY_CTRL_MINUS,                 12,
    GUI_KEY_CTRL_2,                     3,
    GUI_KEY_CTRL_KP_SLASH,              92,
    GUI_KEY_CTRL_KP_ASTERISK,           55,
    GUI_KEY_CTRL_KP_MINUS,              74,
    GUI_KEY_CTRL_KP_PLUS,               78,
    GUI_KEY_CTRL_8,                     9
};

static ctrlkey alt_table[] = {
    GUI_KEY_ALT_1,      '1',
    GUI_KEY_ALT_2,      '2',
    GUI_KEY_ALT_3,      '3',
    GUI_KEY_ALT_4,      '4',
    GUI_KEY_ALT_5,      '5',
    GUI_KEY_ALT_6,      '6',
    GUI_KEY_ALT_7,      '7',
    GUI_KEY_ALT_8,      '8',
    GUI_KEY_ALT_9,      '9',
    GUI_KEY_ALT_0,      '0',
    GUI_KEY_ALT_MINUS,  '-',
    GUI_KEY_ALT_EQUAL,  '=',
};

static bool convert_table( WORD vk, gui_key *key, ctrlkey *table, int size )
{
    int         i;

    for( i=0; i < size; i++ ) {
        if( vk == table[i].scan ) {
            *key = table[i].key;
            return( true );
        }
    }
    return( false );
}

bool GUIWindowsMapKey( WPI_PARAM1 p1, WPI_PARAM2 p2, gui_key *key )
{
    WORD            flags;
    WORD            vk;
    unsigned char   ch;
    char            scan;
    char            pm_scan;

    flags       = SHORT1FROMMP( p1 );
    vk          = SHORT2FROMMP( p2 );
    ch          = CHAR1FROMMP( p2 );
    scan        = CHAR2FROMMP( p2 );
    pm_scan     = CHAR4FROMMP( p1 );

    if( flags & ( KC_DEADKEY | KC_COMPOSITE ) ) {
        return( false );
    } else if( flags & KC_VIRTUALKEY ) {
        return( GUIConvertVirtKeyToGUIKey( vk, key ) );
    } else if( ( flags & KC_CHAR ) && ( ch != 0 ) && ( ch != 0xe0 ) ) {
        return( convert_ascii( ch, key ) );
    } else if( ( flags & KC_LONEKEY ) && ( ch != 0 ) && ( ch != 0xe0 ) ) {
        return( convert_ascii( ch, key ) );
    } else if( flags & KC_SCANCODE ) {
        if( CHK_KS_CTRL ) {
            if( convert_table( pm_scan, key, ctrl_table,
                               sizeof(ctrl_table) / sizeof(ctrl_table[0]) ) ) {
                return( true );
            }
            if( convert_alpha( ch, key ) ) return( true );
            return( false );
        } else {
            if( convert_table( ch, key, alt_table,
                               sizeof(alt_table) / sizeof(alt_table[0]) ) ) {
                return( true );
            }
            *key = (gui_key ) ( pm_scan + GUI_SCAN_OFFSET ) ;
            return( true );
        }
    }
    return( false );
}

WPI_MRESULT GUIProcesskey( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam,
                           WPI_PARAM2 lparam )
{
    WORD                key_flags;
    gui_key_state       key_state;
    gui_event           gui_ev;

    hwnd = hwnd;
    key_flags = SHORT1FROMMP( wparam );

    if( msg == WM_TRANSLATEACCEL ) {
        // Don't let OS/2 process F10 as an accelerator
        // Note: similar code exists in guixwind.c but we need to
        // take different default action
        PQMSG   pqmsg = wparam;
        USHORT  flags = SHORT1FROMMP(pqmsg->mp1);
        USHORT  vkey  = SHORT2FROMMP(pqmsg->mp2);

        if( (flags & KC_VIRTUALKEY) && (vkey == VK_F10) )
            return( (WPI_MRESULT)false );

        return( _wpi_defwindowproc( hwnd, msg, wparam, lparam ) );
    }
    if( ( GUICurrWnd != NULL ) && !EditControlHasFocus ) {
        if( msg == WM_CHAR ) {
            if( key_flags & KC_KEYUP ) {
                gui_ev = GUI_KEYUP;
            } else {
                gui_ev = GUI_KEYDOWN;
            }
            GUIGetKeyState( &key_state.state );
            if( GUIWindowsMapKey( wparam, lparam, &key_state.key ) ) {
                return( (WPI_MRESULT)(GUIEVENTWND( GUICurrWnd, gui_ev, &key_state ) ) );
            }
        }
    }
    return( (WPI_MRESULT)false );
}
#endif
