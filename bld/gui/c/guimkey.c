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
#include "guix.h"


        gui_keystate    KeyState                = GUI_KS_NONE;

#define ALT_CTRL( in, key ) {                           \
        if( ALT ) key = GUI_KEY_ALT_##in;               \
        else if( CTRL ) key = GUI_KEY_CTRL_##in;        \
        else key = GUI_KEY_##in;                        \
}

#define ALT_CTRL_SHIFT( in, key ) {                     \
        if( ALT ) key = GUI_KEY_ALT_##in;               \
        else if( CTRL ) key = GUI_KEY_CTRL_##in;        \
        else if( SHIFT ) key = GUI_KEY_SHIFT_##in;      \
        else key = GUI_KEY_##in;                        \
}

#define EXTALT_CTRL( in, key ) {                        \
        if( ALT ) key = GUI_KEY_ALT_EXT_##in;           \
        else if( CTRL ) key = GUI_KEY_CTRL_##in;        \
        else key = GUI_KEY_##in;                        \
}

#define ALTorCTRL( in, key ) {                          \
    if( ALT || CTRL ) key = GUI_KEY_ALT_##in;           \
    else key = GUI_KEY_##in;                            \
}

gui_key GUIMapKey( gui_key key )
{
    switch( key ) {
    case GUI_KEY_TAB :
        ALT_CTRL_SHIFT( TAB, key );
        break;
    case GUI_KEY_BACKSPACE :
        ALT_CTRL( BACKSPACE, key );
        break;
    case GUI_KEY_ESCAPE :
        ALTorCTRL( ESCAPE, key );
        break;
    case GUI_KEY_ENTER :
        ALT_CTRL( ENTER, key );
        break;
    case GUI_KEY_HOME :
        EXTALT_CTRL( HOME, key );
        break;
    case GUI_KEY_END :
        EXTALT_CTRL( END, key );
        break;
    case GUI_KEY_UP :
        EXTALT_CTRL( UP, key );
        break;
    case GUI_KEY_DOWN :
        EXTALT_CTRL( DOWN, key );
        break;
    case GUI_KEY_LEFT :
        EXTALT_CTRL( LEFT, key );
        break;
    case GUI_KEY_RIGHT :
        EXTALT_CTRL( RIGHT, key );
        break;
    case GUI_KEY_INSERT :
        EXTALT_CTRL( INSERT, key );
        break;
    case GUI_KEY_DELETE :
        EXTALT_CTRL( DELETE, key );
        break;
    case GUI_KEY_PAGEUP :
        EXTALT_CTRL( PAGEUP, key );
        break;
    case GUI_KEY_PAGEDOWN :
        EXTALT_CTRL( PAGEDOWN, key );
        break;
    case GUI_KEY_F11 :
        ALT_CTRL_SHIFT( F11, key );
        break;
    case GUI_KEY_F12 :
        ALT_CTRL_SHIFT( F12, key );
        break;
    case GUI_KEY_KP_MINUS :
        ALT_CTRL( KP_MINUS, key );
        break;
    case GUI_KEY_KP_PLUS :
        ALT_CTRL( KP_PLUS, key );
        break;
    default :
        break;  // keeps GCC happy
    }
    return( key );
}

void GUIGetKeyState( gui_keystate *state )
{
    if( state != NULL ) {
        GUISetKeyState();
        *state = KeyState;
    }
}
