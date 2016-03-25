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


#define BOX_CHARS()         ((unsigned char *)&UiGChar[UI_BOX_TOP_LEFT])
#define SBOX_CHARS()        ((unsigned char *)&UiGChar[UI_SBOX_TOP_LEFT])

#define BOX_CHAR(x,y)       (x)[UI_SBOX_##y - UI_SBOX_TOP_LEFT]
#define CHECKBOX_CHAR(x)    UiGChar[UI_CHECKBOX_##x]
#define RADIO_CHAR(x)       UiGChar[UI_RADIO_##x]

enum    MappedChars {
    #define pick(e,linux,others,dbcs,charmap,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9,da,db,dc,dd,de,df) e,
    #include "_mapchar.h"
    #undef pick
};

extern unsigned char    UiGChar[];
extern void             DBCSCharacterMap( void );

#endif
