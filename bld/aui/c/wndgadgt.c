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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "auipvt.h"

extern void WndGetGadgetSize( wnd_gadget_type type, gui_coord *size )
{
    GUIGetHotSpotSize( type+1, size );
}

extern void WndSetGadgetLine( a_window *wnd, wnd_line_piece *line, wnd_gadget_type type, unsigned length )
{
    static char text[2] = " ";

    line->hint = WndGadgetHint[ type ];
    if( WndSwitchOn( wnd, WSW_ALTERNATE_BIT ) ) type += WndGadgetSecondary;
    text[0] = type+1;
    line->text = text;
    line->bitmap = TRUE;
    line->length = length;
    line->hot = TRUE;
    line->attr = WndGadgetAttr;
    line->tabstop = FALSE;
}

extern void WndGadgetInit( void )
{
    GUIInitHotSpots( WndGadgetArraySize, WndGadgetArray );
}
