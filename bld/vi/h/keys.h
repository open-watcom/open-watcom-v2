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


#ifndef _KEYS_INCLUDED
#define _KEYS_INCLUDED

#define VI_KEY( a )     __VIKEY__##a

/*
 * regular keystrokes
 */
//#define MOUSE_EVENT 0
enum {
        VI_KEY( CTRL_A ) = 1,
        VI_KEY( CTRL_B ),
        VI_KEY( CTRL_C ),
        VI_KEY( CTRL_D ),
        VI_KEY( CTRL_E ),
        VI_KEY( CTRL_F ),
        VI_KEY( CTRL_G ),
        VI_KEY( CTRL_H ),
        VI_KEY( CTRL_I ),
        VI_KEY( CTRL_J ),
        VI_KEY( CTRL_K ),
        VI_KEY( CTRL_L ),
        VI_KEY( CTRL_M ),
        VI_KEY( CTRL_N ),
        VI_KEY( CTRL_O ),
        VI_KEY( CTRL_P ),
        VI_KEY( CTRL_Q ),
        VI_KEY( CTRL_R ),
        VI_KEY( CTRL_S ),
        VI_KEY( CTRL_T ),
        VI_KEY( CTRL_U ),
        VI_KEY( CTRL_V ),
        VI_KEY( CTRL_W ),
        VI_KEY( CTRL_X ),
        VI_KEY( CTRL_Y ),
        VI_KEY( CTRL_Z ),
        VI_KEY( CTRL_OSB ),

#ifdef __WIN__
        VI_KEY( F1 ) = 257,
#else
        VI_KEY( F1 ) = 129,
#endif
        VI_KEY( F2 ),
        VI_KEY( F3 ),
        VI_KEY( F4 ),
        VI_KEY( F5 ),
        VI_KEY( F6 ),
        VI_KEY( F7 ),
        VI_KEY( F8 ),
        VI_KEY( F9 ),
        VI_KEY( F10 ),

#ifdef __WIN__
        VI_KEY( UP ) = 267,
#else
        VI_KEY( UP ) = 139,
#endif
        VI_KEY( DOWN ),
        VI_KEY( LEFT ),
        VI_KEY( RIGHT ),
        VI_KEY( PAGEUP ),
        VI_KEY( PAGEDOWN ),
        VI_KEY( INS ),
        VI_KEY( DEL ),
        VI_KEY( BS ),
        VI_KEY( SHIFT_TAB ),
        VI_KEY( ENTER ),
        VI_KEY( TAB ),
        VI_KEY( HOME ),
        VI_KEY( END ),
        VI_KEY( ESC ),

#ifdef __WIN__
        VI_KEY( SHIFT_F1 ) = 282,
#else
        VI_KEY( SHIFT_F1 ) = 154,
#endif
        VI_KEY( SHIFT_F2 ),
        VI_KEY( SHIFT_F3 ),
        VI_KEY( SHIFT_F4 ),
        VI_KEY( SHIFT_F5 ),
        VI_KEY( SHIFT_F6 ),
        VI_KEY( SHIFT_F7 ),
        VI_KEY( SHIFT_F8 ),
        VI_KEY( SHIFT_F9 ),
        VI_KEY( SHIFT_F10 ),

#ifdef __WIN__
        VI_KEY( ALT_F1 ) = 292,
#else
        VI_KEY( ALT_F1 ) = 164,
#endif
        VI_KEY( ALT_F2 ),
#ifdef __WIN__
        VI_KEY( CS_LEFT ),
        VI_KEY( CS_RIGHT ),
        VI_KEY( CS_UP ),
        VI_KEY( CS_DOWN ),
        VI_KEY( CS_PAGEUP ),
        VI_KEY( CS_PAGEDOWN ),
        VI_KEY( CS_HOME ),
        VI_KEY( CS_END ),
#else
        VI_KEY( ALT_F3 ),
        VI_KEY( ALT_F4 ),
        VI_KEY( ALT_F5 ),
        VI_KEY( ALT_F6 ),
        VI_KEY( ALT_F7 ),
        VI_KEY( ALT_F8 ),
        VI_KEY( ALT_F9 ),
        VI_KEY( ALT_F10 ),
#endif

#ifdef __WIN__
        VI_KEY( CTRL_F1 ) = 302,
#else
        VI_KEY( CTRL_F1 ) = 174,
#endif
        VI_KEY( CTRL_F2 ),
        VI_KEY( CTRL_F3 ),
        VI_KEY( CTRL_F4 ),
        VI_KEY( CTRL_F5 ),
        VI_KEY( CTRL_F6 ),
        VI_KEY( CTRL_F7 ),
        VI_KEY( CTRL_F8 ),
        VI_KEY( CTRL_F9 ),
        VI_KEY( CTRL_F10 ),

#ifdef __WIN__
        VI_KEY( ALT_A ) = 312,
#else
        VI_KEY( ALT_A ) = 184,
#endif
        VI_KEY( ALT_B ),
        VI_KEY( ALT_C ),
        VI_KEY( ALT_D ),
        VI_KEY( ALT_E ),
        VI_KEY( ALT_F ),
        VI_KEY( ALT_G ),
        VI_KEY( ALT_H ),
        VI_KEY( ALT_I ),
        VI_KEY( ALT_J ),
        VI_KEY( ALT_K ),
        VI_KEY( ALT_L ),
        VI_KEY( ALT_M ),
        VI_KEY( ALT_N ),
        VI_KEY( ALT_O ),
        VI_KEY( ALT_P ),
        VI_KEY( ALT_Q ),
        VI_KEY( ALT_R ),
        VI_KEY( ALT_S ),
        VI_KEY( ALT_T ),
        VI_KEY( ALT_U ),
        VI_KEY( ALT_V ),
        VI_KEY( ALT_W ),
        VI_KEY( ALT_X ),
        VI_KEY( ALT_Y ),
        VI_KEY( ALT_Z ),

#ifdef __WIN__
        VI_KEY( F11 ) = 338,
#else
        VI_KEY( F11 ) = 210,
#endif
        VI_KEY( F12 ),
        VI_KEY( SHIFT_F11 ),
        VI_KEY( SHIFT_F12 ),
        VI_KEY( CTRL_F11 ),
        VI_KEY( CTRL_F12 ),
        VI_KEY( ALT_F11 ),
        VI_KEY( ALT_F12 ),

#ifdef __WIN__
        VI_KEY( CTRL_END ) = 346,
#else
        VI_KEY( CTRL_END ) = 218,
#endif
        VI_KEY( CTRL_DEL ),
        VI_KEY( CTRL_INS ),
        VI_KEY( CTRL_HOME ),
        VI_KEY( CTRL_PAGEUP ),
        VI_KEY( CTRL_PAGEDOWN ),
        VI_KEY( CTRL_LEFT ),
        VI_KEY( CTRL_RIGHT ),
        VI_KEY( CTRL_UP ),
        VI_KEY( CTRL_DOWN ),
        VI_KEY( ALT_END ),
        VI_KEY( ALT_DEL ),
        VI_KEY( ALT_INS ),
        VI_KEY( ALT_HOME ),
        VI_KEY( ALT_PAGEUP ),
        VI_KEY( ALT_PAGEDOWN ),
        VI_KEY( ALT_LEFT ),
        VI_KEY( ALT_RIGHT ),
        VI_KEY( ALT_UP ),
        VI_KEY( ALT_DOWN ),
        VI_KEY( CTRL_TAB ),
        VI_KEY( ALT_TAB ),

#ifdef __WIN__
        VI_KEY( MOUSEEVENT ) = 368,
#else
        VI_KEY( MOUSEEVENT ) = 240,
#endif

#ifdef __WIN__
        VI_KEY( SHIFT_UP ) = 369,
#else
        VI_KEY( SHIFT_UP ) = 241,
#endif
        VI_KEY( SHIFT_DOWN ),
        VI_KEY( SHIFT_LEFT ),
        VI_KEY( SHIFT_RIGHT ),
        VI_KEY( SHIFT_DEL ),
        VI_KEY( SHIFT_INS ),
        VI_KEY( SHIFT_HOME ),
        VI_KEY( SHIFT_END ),
        VI_KEY( SHIFT_PAGEUP ),
        VI_KEY( SHIFT_PAGEDOWN ),

#ifdef __WIN__
        VI_KEY( FAKEMOUSE ) = 379,
        VI_KEY( IDLE ) = 380,

        VI_KEY( ALT_BS ) = 381,
        VI_KEY( CTRL_BS ),

        VI_KEY( ALT_HYPHEN ) = 383,

        VI_KEY( TERMINATE )
#else
        VI_KEY( FAKEMOUSE ) = 251,
        VI_KEY( IDLE ) = 252,

        VI_KEY( ALT_BS ) = 253,
        VI_KEY( CTRL_BS ),

        VI_KEY( ALT_HYPHEN ) = 255,

        VI_KEY( TERMINATE )
#endif
};

#endif
