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


#ifndef wkeydefs_h
#define wkeydefs_h

#include "guikey.h"

typedef gui_key WKeyCode;

#define WKeyNone                ((gui_key)0)
#define WKeyCtrlKPEnter         GUI_KEY_CTRL_KP_ENTER
#define WKeyKPEnter             GUI_KEY_KP_ENTER
#define WKeyKPAsterisk          GUI_KEY_KP_ASTERISK
#define WKeyKPPlus              GUI_KEY_KP_PLUS
#define WKeyKPMinus             GUI_KEY_KP_MINUS
#define WKeyKPPeriod            GUI_KEY_KP_PERIOD
#define WKeyKPSlash             GUI_KEY_KP_SLASH
#define WKeyCtrlA               GUI_KEY_CTRL_A
#define WKeyCtrlB               GUI_KEY_CTRL_B
#define WKeyCtrlC               GUI_KEY_CTRL_C
#define WKeyCtrlD               GUI_KEY_CTRL_D
#define WKeyCtrlE               GUI_KEY_CTRL_E
#define WKeyCtrlF               GUI_KEY_CTRL_F
#define WKeyCtrlG               GUI_KEY_CTRL_G
#define WKeyCtrlH               GUI_KEY_CTRL_H
#define WKeyCtrlI               GUI_KEY_CTRL_I
#define WKeyTab                 GUI_KEY_TAB
#define WKeyCtrlJ               GUI_KEY_CTRL_J
#define WKeyCtrlK               GUI_KEY_CTRL_K
#define WKeyCtrlL               GUI_KEY_CTRL_L
#define WKeyCtrlM               GUI_KEY_CTRL_M
#define WKeyCtrlN               GUI_KEY_CTRL_N
#define WKeyCtrlO               GUI_KEY_CTRL_O
#define WKeyCtrlP               GUI_KEY_CTRL_P
#define WKeyCtrlQ               GUI_KEY_CTRL_Q
#define WKeyCtrlR               GUI_KEY_CTRL_R
#define WKeyCtrlS               GUI_KEY_CTRL_S
#define WKeyCtrlT               GUI_KEY_CTRL_T
#define WKeyCtrlU               GUI_KEY_CTRL_U
#define WKeyCtrlV               GUI_KEY_CTRL_V
#define WKeyCtrlW               GUI_KEY_CTRL_W
#define WKeyCtrlX               GUI_KEY_CTRL_X
#define WKeyCtrlY               GUI_KEY_CTRL_Y
#define WKeyCtrlZ               GUI_KEY_CTRL_Z
#define WKeyCtrlBackslash       GUI_KEY_CTRL_BACKSLASH
#define WKeyCtrlRightBracket    GUI_KEY_CTRL_RIGHT_BRACKET
#define WKeyCtrlLeftBracket     GUI_KEY_CTRL_LEFT_BRACKET
#define WKeyCtrl6               GUI_KEY_CTRL_6
#define WKeyCtrlMinus           GUI_KEY_CTRL_MINUS
#define WKeyCtrlBackspace       GUI_KEY_CTRL_BACKSPACE
#define WKeyCtrlEnter           GUI_KEY_CTRL_ENTER
#define WKeyBackquote           GUI_KEY_BACKQUOTE
#define WKey1                   GUI_KEY_1
#define WKey2                   GUI_KEY_2
#define WKey3                   GUI_KEY_3
#define WKey4                   GUI_KEY_4
#define WKey5                   GUI_KEY_5
#define WKey6                   GUI_KEY_6
#define WKey7                   GUI_KEY_7
#define WKey8                   GUI_KEY_8
#define WKey9                   GUI_KEY_9
#define WKey0                   GUI_KEY_0
#define WKeyMinus               GUI_KEY_MINUS
#define WKeyEqual               GUI_KEY_EQUAL
#define WKeyq                   GUI_KEY_q
#define WKeyw                   GUI_KEY_w
#define WKeye                   GUI_KEY_e
#define WKeyr                   GUI_KEY_r
#define WKeyt                   GUI_KEY_t
#define WKeyy                   GUI_KEY_y
#define WKeyu                   GUI_KEY_u
#define WKeyi                   GUI_KEY_i
#define WKeyo                   GUI_KEY_o
#define WKeyp                   GUI_KEY_p
#define WKeyLeftBracket         GUI_KEY_LEFT_BRACKET
#define WKeyRightBracket        GUI_KEY_RIGHT_BRACKET
#define WKeyBackslash           GUI_KEY_BACKSLASH
#define WKeya                   GUI_KEY_a
#define WKeys                   GUI_KEY_s
#define WKeyd                   GUI_KEY_d
#define WKeyf                   GUI_KEY_f
#define WKeyg                   GUI_KEY_g
#define WKeyh                   GUI_KEY_h
#define WKeyj                   GUI_KEY_j
#define WKeyk                   GUI_KEY_k
#define WKeyl                   GUI_KEY_l
#define WKeySemicolon           GUI_KEY_SEMICOLON
#define WKeyQuote               GUI_KEY_QUOTE
#define WKeyz                   GUI_KEY_z
#define WKeyx                   GUI_KEY_x
#define WKeyc                   GUI_KEY_c
#define WKeyv                   GUI_KEY_v
#define WKeyb                   GUI_KEY_b
#define WKeyn                   GUI_KEY_n
#define WKeym                   GUI_KEY_m
#define WKeyComma               GUI_KEY_COMMA
#define WKeyPeriod              GUI_KEY_PERIOD
#define WKeySlash               GUI_KEY_SLASH
#define WKeyTilde               GUI_KEY_TILDE
#define WKeyExclamation         GUI_KEY_EXCLAMATION
#define WKeyAt                  GUI_KEY_AT
#define WKeyOctathorpe          GUI_KEY_OCTATHORPE
#define WKeyDollar              GUI_KEY_DOLLAR
#define WKeyPercent             GUI_KEY_PERCENT
#define WKeyCaret               GUI_KEY_CARET
#define WKeyAmpersand           GUI_KEY_AMPERSAND
#define WKeyAsterisk            GUI_KEY_ASTERISK
#define WKeyLeftParen           GUI_KEY_LEFT_PAREN
#define WKeyRightParen          GUI_KEY_RIGHT_PAREN
#define WKeyUnderscore          GUI_KEY_UNDERSCORE
#define WKeyPlus                GUI_KEY_PLUS
#define WKeyQ                   GUI_KEY_Q
#define WKeyW                   GUI_KEY_W
#define WKeyE                   GUI_KEY_E
#define WKeyR                   GUI_KEY_R
#define WKeyT                   GUI_KEY_T
#define WKeyY                   GUI_KEY_Y
#define WKeyU                   GUI_KEY_U
#define WKeyI                   GUI_KEY_I
#define WKeyO                   GUI_KEY_O
#define WKeyP                   GUI_KEY_P
#define WKeyLeftBrace           GUI_KEY_LEFT_BRACE
#define WKeyRightBrace          GUI_KEY_RIGHT_BRACE
#define WKeyBar                 GUI_KEY_BAR
#define WKeyA                   GUI_KEY_A
#define WKeyS                   GUI_KEY_S
#define WKeyD                   GUI_KEY_D
#define WKeyF                   GUI_KEY_F
#define WKeyG                   GUI_KEY_G
#define WKeyH                   GUI_KEY_H
#define WKeyJ                   GUI_KEY_J
#define WKeyK                   GUI_KEY_K
#define WKeyL                   GUI_KEY_L
#define WKeyColon               GUI_KEY_COLON
#define WKeyDoublequote         GUI_KEY_DOUBLEQUOTE
#define WKeyZ                   GUI_KEY_Z
#define WKeyX                   GUI_KEY_X
#define WKeyC                   GUI_KEY_C
#define WKeyV                   GUI_KEY_V
#define WKeyB                   GUI_KEY_B
#define WKeyN                   GUI_KEY_N
#define WKeyM                   GUI_KEY_M
#define WKeyLess                GUI_KEY_LESS
#define WKeyGreater             GUI_KEY_GREATER
#define WKeyQuestionmark        GUI_KEY_QUESTIONMARK
#define WKeyBackspace           GUI_KEY_BACKSPACE
#define WKeySpace               GUI_KEY_SPACE
#define WKeyEnter               GUI_KEY_ENTER
#define WKeyEscape              GUI_KEY_ESCAPE
#define WKeyKP0                 GUI_KEY_KP_0
#define WKeyKP1                 GUI_KEY_KP_1
#define WKeyKP2                 GUI_KEY_KP_2
#define WKeyKP3                 GUI_KEY_KP_3
#define WKeyKP4                 GUI_KEY_KP_4
#define WKeyKP5                 GUI_KEY_KP_5
#define WKeyKP6                 GUI_KEY_KP_6
#define WKeyKP7                 GUI_KEY_KP_7
#define WKeyKP8                 GUI_KEY_KP_8
#define WKeyKP9                 GUI_KEY_KP_9
#define WKeyHome                GUI_KEY_HOME
#define WKeyEnd                 GUI_KEY_END
#define WKeyUp                  GUI_KEY_UP
#define WKeyDown                GUI_KEY_DOWN
#define WKeyPageup              GUI_KEY_PAGEUP
#define WKeyPagedown            GUI_KEY_PAGEDOWN
#define WKeyLeft                GUI_KEY_LEFT
#define WKeyRight               GUI_KEY_RIGHT
#define WKeyBlank               GUI_KEY_BLANK
#define WKeyInsert              GUI_KEY_INSERT
#define WKeyDelete              GUI_KEY_DELETE
#define WKeyCtrlHome            GUI_KEY_CTRL_HOME
#define WKeyCtrlEnd             GUI_KEY_CTRL_END
#define WKeyCtrlUp              GUI_KEY_CTRL_UP
#define WKeyCtrlDown            GUI_KEY_CTRL_DOWN
#define WKeyCtrlPageup          GUI_KEY_CTRL_PAGEUP
#define WKeyCtrlPagedown        GUI_KEY_CTRL_PAGEDOWN
#define WKeyCtrlLeft            GUI_KEY_CTRL_LEFT
#define WKeyCtrlRight           GUI_KEY_CTRL_RIGHT
#define WKeyCtrlBlank           GUI_KEY_CTRL_BLANK
#define WKeyCtrlInsert          GUI_KEY_CTRL_INSERT
#define WKeyCtrlDelete          GUI_KEY_CTRL_DELETE
#define WKeyCtrlKPPlus          GUI_KEY_CTRL_KP_PLUS
#define WKeyCtrlKPMinus         GUI_KEY_CTRL_KP_MINUS
#define WKeyCtrlKPAsterisk      GUI_KEY_CTRL_KP_ASTERISK
#define WKeyCtrlKPSlash         GUI_KEY_CTRL_KP_SLASH
#define WKeyAltKPPlus           GUI_KEY_ALT_KP_PLUS
#define WKeyAltKPMinus          GUI_KEY_ALT_KP_MINUS
#define WKeyAltKPAsterisk       GUI_KEY_ALT_KP_ASTERISK
#define WKeyAltKPSlash          GUI_KEY_ALT_KP_SLASH
#define WKeyAltKPEnter          GUI_KEY_ALT_KP_ENTER
#define WKeyAltExtInsert        GUI_KEY_ALT_EXT_INSERT
#define WKeyAltExtDelete        GUI_KEY_ALT_EXT_DELETE
#define WKeyAltExtHome          GUI_KEY_ALT_EXT_HOME
#define WKeyAltExtEnd           GUI_KEY_ALT_EXT_END
#define WKeyAltExtPageup        GUI_KEY_ALT_EXT_PAGEUP
#define WKeyAltExtPagedown      GUI_KEY_ALT_EXT_PAGEDOWN
#define WKeyAltExtLeft          GUI_KEY_ALT_EXT_LEFT
#define WKeyAltExtRight         GUI_KEY_ALT_EXT_RIGHT
#define WKeyAltExtUp            GUI_KEY_ALT_EXT_UP
#define WKeyAltExtDown          GUI_KEY_ALT_EXT_DOWN
#define WKeyF1                  GUI_KEY_F1
#define WKeyF2                  GUI_KEY_F2
#define WKeyF3                  GUI_KEY_F3
#define WKeyF4                  GUI_KEY_F4
#define WKeyF5                  GUI_KEY_F5
#define WKeyF6                  GUI_KEY_F6
#define WKeyF7                  GUI_KEY_F7
#define WKeyF8                  GUI_KEY_F8
#define WKeyF9                  GUI_KEY_F9
#define WKeyF10                 GUI_KEY_F10
#define WKeyF11                 GUI_KEY_F11
#define WKeyF12                 GUI_KEY_F12
#define WKeyCtrlF1              GUI_KEY_CTRL_F1
#define WKeyCtrlF2              GUI_KEY_CTRL_F2
#define WKeyCtrlF3              GUI_KEY_CTRL_F3
#define WKeyCtrlF4              GUI_KEY_CTRL_F4
#define WKeyCtrlF5              GUI_KEY_CTRL_F5
#define WKeyCtrlF6              GUI_KEY_CTRL_F6
#define WKeyCtrlF7              GUI_KEY_CTRL_F7
#define WKeyCtrlF8              GUI_KEY_CTRL_F8
#define WKeyCtrlF9              GUI_KEY_CTRL_F9
#define WKeyCtrlF10             GUI_KEY_CTRL_F10
#define WKeyCtrlF11             GUI_KEY_CTRL_F11
#define WKeyCtrlF12             GUI_KEY_CTRL_F12
#define WKeyAltF1               GUI_KEY_ALT_F1
#define WKeyAltF2               GUI_KEY_ALT_F2
#define WKeyAltF3               GUI_KEY_ALT_F3
#define WKeyAltF4               GUI_KEY_ALT_F4
#define WKeyAltF5               GUI_KEY_ALT_F5
#define WKeyAltF6               GUI_KEY_ALT_F6
#define WKeyAltF7               GUI_KEY_ALT_F7
#define WKeyAltF8               GUI_KEY_ALT_F8
#define WKeyAltF9               GUI_KEY_ALT_F9
#define WKeyAltF10              GUI_KEY_ALT_F10
#define WKeyAltF11              GUI_KEY_ALT_F11
#define WKeyAltF12              GUI_KEY_ALT_F12
#define WKeyShiftF1             GUI_KEY_SHIFT_F1
#define WKeyShiftF2             GUI_KEY_SHIFT_F2
#define WKeyShiftF3             GUI_KEY_SHIFT_F3
#define WKeyShiftF4             GUI_KEY_SHIFT_F4
#define WKeyShiftF5             GUI_KEY_SHIFT_F5
#define WKeyShiftF6             GUI_KEY_SHIFT_F6
#define WKeyShiftF7             GUI_KEY_SHIFT_F7
#define WKeyShiftF8             GUI_KEY_SHIFT_F8
#define WKeyShiftF9             GUI_KEY_SHIFT_F9
#define WKeyShiftF10            GUI_KEY_SHIFT_F10
#define WKeyShiftF11            GUI_KEY_SHIFT_F11
#define WKeyShiftF12            GUI_KEY_SHIFT_F12
#define WKeyShiftTab            GUI_KEY_SHIFT_TAB
#define WKeyAltBackquote        GUI_KEY_ALT_BACKQUOTE
#define WKeyAlt1                GUI_KEY_ALT_1
#define WKeyAlt2                GUI_KEY_ALT_2
#define WKeyAlt3                GUI_KEY_ALT_3
#define WKeyAlt4                GUI_KEY_ALT_4
#define WKeyAlt5                GUI_KEY_ALT_5
#define WKeyAlt6                GUI_KEY_ALT_6
#define WKeyAlt7                GUI_KEY_ALT_7
#define WKeyAlt8                GUI_KEY_ALT_8
#define WKeyAlt9                GUI_KEY_ALT_9
#define WKeyAlt0                GUI_KEY_ALT_0
#define WKeyAltMinus            GUI_KEY_ALT_MINUS
#define WKeyAltEqual            GUI_KEY_ALT_EQUAL
#define WKeyAltQ                GUI_KEY_ALT_Q
#define WKeyAltW                GUI_KEY_ALT_W
#define WKeyAltE                GUI_KEY_ALT_E
#define WKeyAltR                GUI_KEY_ALT_R
#define WKeyAltT                GUI_KEY_ALT_T
#define WKeyAltY                GUI_KEY_ALT_Y
#define WKeyAltU                GUI_KEY_ALT_U
#define WKeyAltI                GUI_KEY_ALT_I
#define WKeyAltO                GUI_KEY_ALT_O
#define WKeyAltP                GUI_KEY_ALT_P
#define WKeyAltLeftBracket      GUI_KEY_ALT_LEFT_BRACKET
#define WKeyAltRightBracket     GUI_KEY_ALT_RIGHT_BRACKET
#define WKeyAltBackslash        GUI_KEY_ALT_BACKSLASH
#define WKeyAltA                GUI_KEY_ALT_A
#define WKeyAltS                GUI_KEY_ALT_S
#define WKeyAltD                GUI_KEY_ALT_D
#define WKeyAltF                GUI_KEY_ALT_F
#define WKeyAltG                GUI_KEY_ALT_G
#define WKeyAltH                GUI_KEY_ALT_H
#define WKeyAltJ                GUI_KEY_ALT_J
#define WKeyAltK                GUI_KEY_ALT_K
#define WKeyAltL                GUI_KEY_ALT_L
#define WKeyAltSemicolon        GUI_KEY_ALT_SEMICOLON
#define WKeyAltQuote            GUI_KEY_ALT_QUOTE
#define WKeyAltZ                GUI_KEY_ALT_Z
#define WKeyAltX                GUI_KEY_ALT_X
#define WKeyAltC                GUI_KEY_ALT_C
#define WKeyAltV                GUI_KEY_ALT_V
#define WKeyAltB                GUI_KEY_ALT_B
#define WKeyAltN                GUI_KEY_ALT_N
#define WKeyAltM                GUI_KEY_ALT_M
#define WKeyAltComma            GUI_KEY_ALT_COMMA
#define WKeyAltPeriod           GUI_KEY_ALT_PERIOD
#define WKeyAltSlash            GUI_KEY_ALT_SLASH
#define WKeyAltBackspace        GUI_KEY_ALT_BACKSPACE
#define WKeyAltEnter            GUI_KEY_ALT_ENTER
#define WKeyAltTab              GUI_KEY_ALT_TAB
#define WKeyAltEscape           GUI_KEY_ALT_ESCAPE
#define WKeyCtrl2               GUI_KEY_CTRL_2
#define WKeyCtrl8               GUI_KEY_CTRL_8
#define WKeyCtrlTab             GUI_KEY_CTRL_TAB

typedef gui_keystate    WKeyState;

#define WKeyStateNone   GUI_KS_NONE
#define WKeyStateAlt    GUI_KS_ALT
#define WKeyStateShift  GUI_KS_SHIFT
#define WKeyStateCtrl   GUI_KS_CTRL

#endif
