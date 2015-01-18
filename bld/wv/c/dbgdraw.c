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


#include <string.h>
#include "dbgdraw.h"
#include "stdui.h"

/* Assume IBM character set */
unsigned char DbgDraw[] = {
    '\x1b', /*     DRAW_BP_IND             */
    '*',    /* *    DRAW_ACT_CNTRL_TRANS    */
    '\x10', /*     DRAW_PNT_ACT            */
    '\xba', /* º    DRAW_VERT_FRAME         */
    '\xcd', /* Í    DRAW_HOR_FRAME          */
    '\x1f', /*     DRAW_TOP_IND            */
    '\x1e', /*     DRAW_BOT_IND            */
    '\xb5', /* µ    DRAW_LEFT_TITLE_MARK    */
    '\xc6', /* Æ    DRAW_RITE_TITLE_MARK    */
    '\x18', /*     DRAW_SCROLL_UP          */
    '\x19', /*     DRAW_SCROLL_DOWN        */
    '\x12', /*     DRAW_RESIZE_V           */
    '\x1d', /*     DRAW_RESIZE_H           */
    '\x1b', /*     DRAW_SCROLL_LEFT        */
    '\x1a', /* ->   DRAW_SCROLL_RIGHT       */
    '\x1e', /*     DRAW_PAGE_UP            */
    '\x1f', /*     DRAW_PAGE_DOWN          */
    '\x11', /*     DRAW_PAGE_LEFT          */
    '\x10', /*     DRAW_PAGE_RIGHT         */
    '\xb0', /* °    DRAW_BLOCK_CHAR         */
    '\xc9', /* É    DRAW_UL_CORNER          */
    '\xbb', /* »    DRAW_UR_CORNER          */
    '\xc8', /* È    DRAW_LL_CORNER          */
    '\xbc', /* ¼    DRAW_LR_CORNER          */
    '\x0f', /*     DRAW_ZOOMER             */
    '\xf0'  /* ð    DRAW_CLOSER             */
};

unsigned char DbgDrawDBCS[] = {
    '\x0f', /*     DRAW_BP_IND             */
    '*',    /* *    DRAW_ACT_CNTRL_TRANS    */
    '>',    /*     DRAW_PNT_ACT            */
    '\x05', /* º    DRAW_VERT_FRAME         */
    '\x06', /* Í    DRAW_HOR_FRAME          */
    '\x1f', /*     DRAW_TOP_IND   ??       */
    '\x1e', /*     DRAW_BOT_IND   ??       */
    '\x17', /* µ    DRAW_LEFT_TITLE_MARK    */
    '\x19', /* Æ    DRAW_RITE_TITLE_MARK    */
    '^',    /*     DRAW_SCROLL_UP          */
    'v',    /*     DRAW_SCROLL_DOWN        */
    '\x12', /*     DRAW_RESIZE_V           */
    '\x1b', /*     DRAW_RESIZE_H           */
    '<',    /*     DRAW_SCROLL_LEFT        */
    '>',    /* ->   DRAW_SCROLL_RIGHT       */
    162,    /*     DRAW_PAGE_UP            */
    163,    /*     DRAW_PAGE_DOWN          */
    162,    /*     DRAW_PAGE_LEFT          */
    163,    /*     DRAW_PAGE_RIGHT         */
    '\x1a', /* °    DRAW_BLOCK_CHAR         */
    '\x01', /* É    DRAW_UL_CORNER          */
    '\x02', /* »    DRAW_UR_CORNER          */
    '\x03', /* È    DRAW_LL_CORNER          */
    '\x04', /* ¼    DRAW_LR_CORNER          */
    '\x0f', /*     DRAW_ZOOMER             */
    '\x0e'  /* ð    DRAW_CLOSER             */
};


void DBCSCheck( void )
{
    if( UIData->dbcs )
        memcpy( DbgDraw, DbgDrawDBCS, sizeof( DbgDraw ) );
}
