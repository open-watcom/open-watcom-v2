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


#ifndef _UISHIFT_H_
#define _UISHIFT_H_
#define         S_SHIFT_R               (unsigned char) 0x01
#define         S_SHIFT_L               (unsigned char) 0x02
#define         S_SHIFT                 (unsigned char) 0x03
#define         S_CTRL                  (unsigned char) 0x04
#define         S_ALT                   (unsigned char) 0x08
#define         S_SCROLL                (unsigned char) 0x10
#define         S_NUM                   (unsigned char) 0x20
#define         S_CAPS                  (unsigned char) 0x40
#define         S_INSERT                (unsigned char) 0x80
// OS2 Additions:
#define         S_CTRL_L                (unsigned char) 0x100
#define         S_ALT_L                 (unsigned char) 0x200
#define         S_CTRL_R                (unsigned char) 0x400
#define         S_ALT_R                 (unsigned char) 0x800
#define         S_SCRL                  (unsigned char) 0x1000
#define         S_NUM_D                 (unsigned char) 0x2000
#define         S_CAPS_D                (unsigned char) 0x4000
#define         S_SYSRQ                 (unsigned char) 0x8000
// Additions for NEC windows:
#define         S_KANJI_LOCK            (unsigned int) 0x100
/* Shift Status Support

 AL - Shift status
 Bit 7 - Insert locked
     6 - Caps locked
     5 - Nums locked
     4 - Scroll locked
     3 - Alt key pressed
     2 - Ctrl key pressed
     1 - Left shift key pressed
     0 - Right shift key pressed

 AH - Extended shift status
 Bit 7 - System request key pressed
     6 - Caps lock key pressed
     5 - Num lock key pressed
     4 - Scroll lock key pressed
     3 - Right Alt key pressed
     2 - Right Ctrl key pressed
     1 - Left Alt key pressed
     0 - Left Ctrl key pressed
*/
#endif
