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
* Description:  Graphic character definitions
*
****************************************************************************/


#include <string.h>
#include "uibox.h"
#include "uichars.h"
#include "uidef.h"
#include "uigchar.h"

#ifdef __LINUX__

void intern DBCSCharacterMap( void ) {}

unsigned char UiGChar[] = {
    '(',                /* UI_RADIO_LEFT        */
    ')',                /* UI_RADIO_RIGHT       */
    '*',                /* UI_RADIO_FULL        */
    ' ',                /* UI_RADIO_EMPTY       */
    '[',                /* UI_CHECKBOX_LEFT     */
    ']',                /* UI_CHECKBOX_RIGHT    */
    'X',                /* UI_CHECKBOX_FULL     */
    ' ',                /* UI_CHECKBOX_EMPTY    */
    UI_ULCORNER,        /* UI_BOX_TOP_LEFT      */
    UI_URCORNER,        /* UI_BOX_TOP_RIGHT     */
    UI_LRCORNER,        /* UI_BOX_BOTTOM_RIGHT  */
    UI_LLCORNER,        /* UI_BOX_BOTTOM_LEFT   */
    UI_DHLINE,          /* UI_BOX_TOP_LINE      */
    UI_DVLINE,          /* UI_BOX_RIGHT_LINE    */
    UI_DHLINE,          /* UI_BOX_BOTTOM_LINE   */
    UI_DVLINE,          /* UI_BOX_LEFT_LINE     */
    UI_UBLOCK,          /* UI_SHADOW_BOTTOM     */
    ' ',                /* UI_SHADOW_B_LEFT     */
    UI_DBLOCK,          /* UI_SHADOW_RIGHT      */
    UI_UBLOCK,          /* UI_SHADOW_B_RIGHT    */
    UI_ULCORNER,        /* UI_SBOX_TOP_LEFT     */
    UI_URCORNER,        /* UI_SBOX_TOP_RIGHT    */
    UI_LRCORNER,        /* UI_SBOX_BOTTOM_RIGHT */
    UI_LLCORNER,        /* UI_SBOX_BOTTOM_LEFT  */
    UI_HLINE,           /* UI_SBOX_TOP_LINE     */
    UI_VLINE,           /* UI_SBOX_RIGHT_LINE   */
    UI_HLINE,           /* UI_SBOX_BOTTOM_LINE  */
    UI_VLINE,           /* UI_SBOX_LEFT_LINE    */
    UI_LTEE,            /* UI_SBOX_LEFT_TACK    */
    UI_RTEE,            /* UI_SBOX_RIGHT_TACK   */
    UI_HLINE,           /* UI_SBOX_HORIZ_LINE   */
    UI_DARROW,          /* UI_ARROW_DOWN        */
    UI_RPOINT,          /* UI_POPUP_MARK        */
    UI_ROOT,            /* UI_CHECK_MARK        */
};

#else

/*
    The order of items in this table is position dependent.
    The first "n" entries in this table are remapped on EGA/VGA devices
    supporting character remapping (see UIMapCharacters in UIMAPCH.C).
*/

unsigned char UiGChar[] = {
    '(',                /* UI_RADIO_LEFT        */
    ')',                /* UI_RADIO_RIGHT       */
    '*',                /* UI_RADIO_FULL        */
    ' ',                /* UI_RADIO_EMPTY       */
    '[',                /* UI_CHECKBOX_LEFT     */
    ']',                /* UI_CHECKBOX_RIGHT    */
    'X',                /* UI_CHECKBOX_FULL     */
    ' ',                /* UI_CHECKBOX_EMPTY    */
    0xc9,               /* UI_BOX_TOP_LEFT      */
    0xbb,               /* UI_BOX_TOP_RIGHT     */
    0xbc,               /* UI_BOX_BOTTOM_RIGHT  */
    0xc8,               /* UI_BOX_BOTTOM_LEFT   */
    0xcd,               /* UI_BOX_TOP_LINE      */
    0xba,               /* UI_BOX_RIGHT_LINE    */
    0xcd,               /* UI_BOX_BOTTOM_LINE   */
    0xba,               /* UI_BOX_LEFT_LINE     */
    0xdf,               /* UI_SHADOW_BOTTOM     */
    ' ',                /* UI_SHADOW_B_LEFT     */
    0xdc,               /* UI_SHADOW_RIGHT      */
    0xdf,               /* UI_SHADOW_B_RIGHT    */
    0xda,               /* UI_SBOX_TOP_LEFT     */
    0xbf,               /* UI_SBOX_TOP_RIGHT    */
    0xd9,               /* UI_SBOX_BOTTOM_RIGHT */
    0xc0,               /* UI_SBOX_BOTTOM_LEFT  */
    0xc4,               /* UI_SBOX_TOP_LINE     */
    0xb3,               /* UI_SBOX_RIGHT_LINE   */
    0xc4,               /* UI_SBOX_BOTTOM_LINE  */
    0xb3,               /* UI_SBOX_LEFT_LINE    */
    0xc3,               /* UI_SBOX_LEFT_TACK    */
    0xb4,               /* UI_SBOX_RIGHT_TACK   */
    0xc4,               /* UI_SBOX_HORIZ_LINE   */
    PC_arrowdown,       /* UI_ARROW_DOWN        */
    PC_triangright,     /* UI_POPUP_MARK        */
    PC_radical,         /* UI_CHECK_MARK        */
};

extern char VertScrollFrame[2];
extern char HorzScrollFrame[2];
extern char SliderChar[2];
extern char LeftPoint[2];
extern char RightPoint[2];
extern char UpPoint[2];
extern char DownPoint[2];

unsigned char UiDBCSChar[] = {
    '(',        /* UI_RADIO_LEFT        */
    ')',        /* UI_RADIO_RIGHT       */
    '*',        /* UI_RADIO_FULL        */
    ' ',        /* UI_RADIO_EMPTY       */
    '[',        /* UI_CHECKBOX_LEFT     */
    ']',        /* UI_CHECKBOX_RIGHT    */
    'X',        /* UI_CHECKBOX_FULL     */
    ' ',        /* UI_CHECKBOX_EMPTY    */
    0x01,       /* UI_BOX_TOP_LEFT      */
    0x02,       /* UI_BOX_TOP_RIGHT     */
    0x04,       /* UI_BOX_BOTTOM_RIGHT  */
    0x03,       /* UI_BOX_BOTTOM_LEFT   */
    0x06,       /* UI_BOX_TOP_LINE      */
    0x05,       /* UI_BOX_RIGHT_LINE    */
    0x06,       /* UI_BOX_BOTTOM_LINE   */
    0x05,       /* UI_BOX_LEFT_LINE     */
    ' ',        /* UI_SHADOW_BOTTOM     */
    ' ',        /* UI_SHADOW_B_LEFT     */
    ' ',        /* UI_SHADOW_RIGHT      */
    ' ',        /* UI_SHADOW_B_RIGHT    */
    0x01,       /* UI_SBOX_TOP_LEFT     */
    0x02,       /* UI_SBOX_TOP_RIGHT    */
    0x04,       /* UI_SBOX_BOTTOM_RIGHT */
    0x03,       /* UI_SBOX_BOTTOM_LEFT  */
    0x06,       /* UI_SBOX_TOP_LINE     */
    0x05,       /* UI_SBOX_RIGHT_LINE   */
    0x06,       /* UI_SBOX_BOTTOM_LINE  */
    0x05,       /* UI_SBOX_LEFT_LINE    */
    0x19,       /* UI_SBOX_LEFT_TACK    */
    0x17,       /* UI_SBOX_RIGHT_TACK   */
    0x06,       /* UI_SBOX_HORIZ_LINE   */
    0x07,       /* UI_ARROW_DOWN        */
    '>',        /* UI_POPUP_MARK        */
    'X',        /* UI_CHECK_MARK        */
};

void intern DBCSCharacterMap( void )
{
    if ( uiisdbcs() ) {
        memcpy( UiGChar, UiDBCSChar, sizeof( UiDBCSChar ) );
        VertScrollFrame[0]  = 0x1A;
        HorzScrollFrame[0]  = 0x1A;
        SliderChar[0]       = 0x14;
        LeftPoint[0]        = '<';
        RightPoint[0]       = '>';
        UpPoint[0]          = '^';
        DownPoint[0]        = 'v';
    }
}

#endif
