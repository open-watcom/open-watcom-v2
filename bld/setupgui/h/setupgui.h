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


#include <stdlib.h>
#include <string.h>
enum {
    MENU_FIRST          = 2400,
    MENU_SEARCH,
    MENU_NEXT,
    MENU_PREV,
    MENU_QUIT,
    MENU_COMMAND,
    MENU_PICK,
    MENU_STATUS,
    MENU_MATCH,
    MENU_SCRAMBLE_MENUS,
    MENU_BUG,
    MENU_OPTIONS,

    MENU_SECOND         = 2500,
    MENU_OPEN1,
    MENU_OPEN1A,
    MENU_OPEN2,
    MENU_OPEN3,
    MENU_OPEN4,
    MENU_OPEN5,
    MENU_GET_FILE,
    MENU_TOOLS,
    MENU_OPEN4B,
    MENU_OPEN6,
    MENU_OPEN7,
    MENU_MORE,


    MENU_THIRD          = 2600,
    MENU_POPUP          = 2700,

    MENU_W1_ALIGN       = 500,
    MENU_W1_UNALIGN,
    MENU_W1_SAY,
    MENU_W1_NEWWORD,
    MENU_W1_MORE,

    MENU_W2_SAY         = 600,
    MENU_W2_TOP,
    MENU_W2_BOTTOM,
    MENU_W2_TITLE,
    MENU_W2_OPEN1,

    MENU_W3_POPUP       = 700,
    MENU_W3_BUG

};

enum {
    WND_PLAIN = WND_FIRST_UNUSED,
    WND_TABSTOP,
    WND_SELECTED,
    WND_HOTSPOT,
    WND_CENSORED,
    WND_BASEBALL,
    WND_NUMBER_OF_COLOURS
};

enum {
    EV_UPDATE_1         = 0x0001,
    EV_UPDATE_2         = 0x0002,
    EV_UPDATE_ALL       = 0xFFFF,
};

enum {
    CLASS_W1,
    CLASS_W2
};

extern int RandNum( int );

#define MAX_WORD 14
#define WORD_SIZE 50

extern char *Word[WORD_SIZE];
