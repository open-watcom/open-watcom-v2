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

#if defined(_NEC_PC)        /* NEC character set dependent */
unsigned char DbgDraw[] = {
    '\x1c', /*      DRAW_BP_IND             */
    '*',    /*      DRAW_ACT_CNTRL_TRANS    */
    '\x3e', /*      DRAW_PNT_ACT            */
    '\x04', /*      DRAW_VERT_FRAME         */
    '\x02', /*      DRAW_HOR_FRAME          */
    '\x20', /*      DRAW_TOP_IND            */
    '\x20', /*      DRAW_BOT_IND            */
    '\x13', /*      DRAW_LEFT_TITLE_MARK    */
    '\x11', /*      DRAW_RITE_TITLE_MARK    */
    '\x1e', /*      DRAW_SCROLL_UP          */
    '\x1f', /*      DRAW_SCROLL_DOWN        */
    '\x2a', /*      DRAW_RESIZE_V           */
    '\x2a', /*      DRAW_RESIZE_H           */
    '\x1d', /*      DRAW_SCROLL_LEFT        */
    '\x1c', /*      DRAW_SCROLL_RIGHT       */
    '\x20', /*      DRAW_PAGE_UP            */
    '\x20', /*      DRAW_PAGE_DOWN          */
    '\x3c', /*      DRAW_PAGE_LEFT          */
    '\x3e', /*      DRAW_PAGE_RIGHT         */
    '\x23', /*      DRAW_BLOCK_CHAR         */
    '\x07', /*      DRAW_UL_CORNER          */
    '\x09', /*      DRAW_UR_CORNER          */
    '\x0c', /*      DRAW_LL_CORNER          */
    '\x0f', /*      DRAW_LR_CORNER          */
    '\x2a', /*      DRAW_ZOOMER             */
    '\x3d'  /*      DRAW_CLOSER             */
};

void DBCSCheck()
{
}

#elif defined(_FMR_PC)      /* FMR character set dependent */
unsigned char DbgDraw[] = {
    '\x1d', /*     DRAW_BP_IND             */
    '*',    /* *    DRAW_ACT_CNTRL_TRANS    */
    '\x18', /*     DRAW_PNT_ACT            */
    '\x04', /* º    DRAW_VERT_FRAME         */
    '\x02', /* Í    DRAW_HOR_FRAME          */
    '\x1f', /*     DRAW_TOP_IND            */
    '\x1e', /*     DRAW_BOT_IND            */
    '\x13', /* µ    DRAW_LEFT_TITLE_MARK    */
    '\x11', /* Æ    DRAW_RITE_TITLE_MARK    */
    '\x1e', /*     DRAW_SCROLL_UP          */
    '\x1f', /*     DRAW_SCROLL_DOWN        */
    '\x1b', /*     DRAW_RESIZE_V           */
    '\x1b', /*     DRAW_RESIZE_H           */
    '\x1d', /*     DRAW_SCROLL_LEFT        */
    '\x1c', /* ->   DRAW_SCROLL_RIGHT       */
    '\x18', /*     DRAW_PAGE_UP            */
    '\x18', /*     DRAW_PAGE_DOWN          */
    '\x18', /*     DRAW_PAGE_LEFT          */
    '\x18', /*     DRAW_PAGE_RIGHT         */
    '\xfe', /* °    DRAW_BLOCK_CHAR         */
    '\x07', /* É    DRAW_UL_CORNER          */
    '\x09', /* »    DRAW_UR_CORNER          */
    '\x0c', /* È    DRAW_LL_CORNER          */
    '\x0f', /* ¼    DRAW_LR_CORNER          */
    '\x1a', /*     DRAW_ZOOMER             */
    '\x19'  /* ð    DRAW_CLOSER             */
};

void DBCSCheck()
{
}

#else                       /* IBM character set dependent */
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


void DBCSCheck()
{
    if( UIData->dbcs )
        memcpy( DbgDraw, DbgDrawDBCS, sizeof( DbgDraw ) );
}

#endif
