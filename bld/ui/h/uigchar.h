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


#ifndef _UIGCHAR_H_
#define _UIGCHAR_H_

enum    MappedChars {
    UI_RADIO_LEFT,
    UI_RADIO_RIGHT,
    UI_RADIO_FULL,
    UI_RADIO_EMPTY,
    UI_CHECKBOX_LEFT,
    UI_CHECKBOX_RIGHT,
    UI_CHECKBOX_FULL,
    UI_CHECKBOX_EMPTY,
    UI_BOX_TOP_LEFT,
    UI_BOX_TOP_RIGHT,
    UI_BOX_BOTTOM_RIGHT,
    UI_BOX_BOTTOM_LEFT,
    UI_BOX_TOP_LINE,
    UI_BOX_RIGHT_LINE,
    UI_BOX_BOTTOM_LINE,
    UI_BOX_LEFT_LINE,
    UI_SHADOW_BOTTOM,
    UI_SHADOW_B_LEFT,
    UI_SHADOW_RIGHT,
    UI_SHADOW_B_RIGHT,
    UI_SBOX_TOP_LEFT,
    UI_SBOX_TOP_RIGHT,
    UI_SBOX_BOTTOM_RIGHT,
    UI_SBOX_BOTTOM_LEFT,
    UI_SBOX_TOP_LINE,
    UI_SBOX_RIGHT_LINE,
    UI_SBOX_BOTTOM_LINE,
    UI_SBOX_LEFT_LINE,
    UI_SBOX_LEFT_TACK,
    UI_SBOX_RIGHT_TACK,
    UI_SBOX_HORIZ_LINE,
    UI_ARROW_DOWN,
    UI_POPUP_MARK,
    UI_CHECK_MARK,
    UI_DBOX_TOP_LEFT,
    UI_DBOX_TOP_RIGHT,
    UI_DBOX_BOTTOM_RIGHT,
    UI_DBOX_BOTTOM_LEFT,
    UI_DBOX_TOP_LINE,
    UI_DBOX_RIGHT_LINE,
    UI_DBOX_BOTTOM_LINE,
    UI_DBOX_LEFT_LINE,
    UI_LAST_CHAR
};

extern unsigned char    UiGChar[];
extern void             DBCSCharacterMap( void );

#endif
