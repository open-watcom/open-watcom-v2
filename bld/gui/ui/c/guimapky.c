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


#include "guiwind.h"
#include "guikey.h"
#include "guimkey.h"
#include "uishift.h"
#include "guix.h"

extern  gui_window      *GUICurrWnd;
extern  gui_keystate    KeyState;

void GUISetKeyState( void )
{
    unsigned char       shiftstate;

    shiftstate = uicheckshift();

    KeyState = GUI_KS_NONE;
    if( shiftstate & S_SHIFT ) {
        SET_SHIFT;
    }
    if( shiftstate & S_ALT ) {
        SET_ALT;
    }
    if( shiftstate & S_CTRL ) {
        SET_CTRL;
    }
}

EVENT GUIMapKeys( EVENT ev )
{
    gui_key_state       key_state;
    EVENT               key;

    KeyState = GUI_KS_NONE;
    switch( ev ) {
    case EV_TAB_FORWARD :
        key = GUI_KEY_TAB;
        break;
    case EV_CTRL_TAB :
        key = GUI_KEY_TAB;
        SET_CTRL;
        break;
    case EV_TAB_BACKWARD :
        key = GUI_KEY_TAB;
        SET_SHIFT;
        break;
    case EV_RUB_OUT :
        key = GUI_KEY_BACKSPACE;
        break;
    case EV_ESCAPE :
        key = GUI_KEY_ESCAPE;
        break;
    case EV_ENTER : /* Same as EV_RETURN */
        key = GUI_KEY_ENTER;
        break;
    case EV_ALT_HOME :
        key = GUI_KEY_HOME;
        SET_ALT;
        break;
    case EV_CTRL_HOME :
        key = GUI_KEY_HOME;
        SET_CTRL;
        break;
    case EV_SHIFT_HOME :
        key = GUI_KEY_HOME;
        SET_SHIFT;
        break;
    case EV_HOME :
        key = GUI_KEY_HOME;
        break;
    case EV_ALT_END :
        key = GUI_KEY_END;
        SET_ALT;
        break;
    case EV_CTRL_END :
        key = GUI_KEY_END;
        SET_CTRL;
        break;
    case EV_SHIFT_END :
        key = GUI_KEY_END;
        SET_SHIFT;
        break;
    case EV_END :
        key = GUI_KEY_END;
        break;
    case EV_CTRL_UP :
        key = GUI_KEY_UP;
        SET_CTRL;
        break;
    case EV_SHIFT_CURSOR_UP :
        key = GUI_KEY_UP;
        SET_SHIFT;
        break;
    case EV_CURSOR_UP :
        key = GUI_KEY_UP;
        break;
    case EV_CTRL_DOWN :
        key = GUI_KEY_DOWN;
        SET_CTRL;
        break;
    case EV_CURSOR_DOWN :
        key = GUI_KEY_DOWN;
        break;
    case EV_SHIFT_CURSOR_DOWN :
        key = GUI_KEY_DOWN;
        SET_SHIFT;
        break;
    case EV_CTRL_LEFT :
        key = GUI_KEY_LEFT;
        SET_CTRL;
        break;
    case EV_SHIFT_CURSOR_LEFT :
        key = GUI_KEY_LEFT;
        SET_SHIFT;
        break;
    case EV_CURSOR_LEFT :
        key = GUI_KEY_LEFT;
        break;
    case EV_CTRL_RIGHT :
        key = GUI_KEY_RIGHT;
        SET_CTRL;
        break;
    case EV_SHIFT_CURSOR_RIGHT :
        key = GUI_KEY_RIGHT;
        SET_SHIFT;
        break;
    case EV_CURSOR_RIGHT :
        key = GUI_KEY_RIGHT;
        break;
    case EV_INSERT :
        key = GUI_KEY_INSERT;
        break;
    case EV_CTRL_INSERT :
        key = GUI_KEY_INSERT;
        SET_CTRL;
        break;
    case EV_ALT_INSERT :
        key = GUI_KEY_INSERT;
        SET_ALT;
        break;
    case EV_CTRL_DELETE :
        key = GUI_KEY_DELETE;
        SET_CTRL;
        break;
    case EV_ALT_DELETE :
        key = GUI_KEY_DELETE;
        SET_ALT;
        break;
    case EV_DELETE :
        key = GUI_KEY_DELETE;
        break;
    case EV_ALT_PAGE_UP :
        key = GUI_KEY_PAGEUP;
        SET_ALT;
        break;
    case EV_CTRL_PAGE_UP :
        key = GUI_KEY_PAGEUP;
        SET_CTRL;
        break;
    case EV_SHIFT_PAGE_UP :
        key = GUI_KEY_PAGEUP;
        SET_SHIFT;
        break;
    case EV_PAGE_UP :
        key = GUI_KEY_PAGEUP;
        break;
    case EV_CTRL_PAGE_DOWN :
        key = GUI_KEY_PAGEDOWN;
        SET_CTRL;
        break;
    case EV_ALT_PAGE_DOWN :
        key = GUI_KEY_PAGEDOWN;
        SET_ALT;
        break;
    case EV_SHIFT_PAGE_DOWN :
        key = GUI_KEY_PAGEDOWN;
        SET_SHIFT;
        break;
    case EV_PAGE_DOWN :
        key = GUI_KEY_PAGEDOWN;
        break;
    case EV_ALT_SPACE :
        key = GUI_KEY_SPACE;
        SET_ALT;
        break;
    case ' ' :
        key = GUI_KEY_SPACE;
        break;
    case EV_FUNC_11 :
        key = GUI_KEY_F11;
        break;
    case EV_ALT_FUNC_11 :
        key = GUI_KEY_F11;
        SET_ALT;
        break;
    case EV_CTRL_FUNC_11 :
        key = GUI_KEY_F11;
        SET_CTRL;
        break;
    case EV_SHIFT_FUNC_11 :
        key = GUI_KEY_F11;
        SET_SHIFT;
        break;
    case EV_FUNC_12 :
        key = GUI_KEY_F12;
        break;
    case EV_ALT_FUNC_12 :
        key = GUI_KEY_F12;
        SET_ALT;
        break;
    case EV_CTRL_FUNC_12 :
        key = GUI_KEY_F12;
        SET_CTRL;
        break;
    case EV_SHIFT_FUNC_12 :
        key = GUI_KEY_F12;
        SET_SHIFT;
        break;
    default :
        if( ev < EV_LAST_KEYBOARD ) {
            key = (gui_key)ev;
        } else {
            return( ev );
        }
    }
    key_state.key = GUIMapKey( key );
    GUIGetKeyState( &key_state.state );
    if( GUICurrWnd != NULL ) {
        GUIEVENTWND( GUICurrWnd, GUI_KEYDOWN, &key_state );
    }
    if( GUICurrWnd != NULL ) {
        GUIEVENTWND( GUICurrWnd, GUI_KEYUP, &key_state );
    }
    return( EV_NO_EVENT );
}
