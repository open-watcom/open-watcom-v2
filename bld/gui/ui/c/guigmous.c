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


#include "guiwind.h"
#include "guigmous.h"
#include "guixinit.h"

static  bool    GraphicsMouse   = FALSE;
static  bool    GraphicsDlg     = FALSE;
static  bool    GMouseOn        = FALSE;
static  int     Param           = 0;

void GUIInitGraphicsMouse( gui_window_styles style )
{
#ifdef __DOS__
    if( style & GUI_CHARMAP_MOUSE ) {
        GraphicsMouse = TRUE;
    }
    if( style & GUI_CHARMAP_DLG ) {
        GraphicsDlg = TRUE;
    }
#else
    style = style;
#endif
}

int GUIInitMouse( int param )
{
    int init;

    Param = param;
    if( GraphicsMouse ) {
#ifdef __DOS__
        init = uiinitgmouse( param );
        if( GraphicsDlg ) FlipCharacterMap();
#endif
    } else {
        init = initmouse( param );
    }
    GMouseOn = TRUE;
    return( init );
}

void GUIFiniMouse( void )
{
    if( GraphicsMouse ) {
#ifdef __DOS__
        if( GraphicsDlg ) FlipCharacterMap();
        uifinigmouse();
#endif
    } else {
        finimouse();
    }
    GMouseOn = FALSE;
}

void GUIGMouseOn( void )
{
    ORD row;
    ORD col;

    if( !GMouseOn && GraphicsMouse ) {
        uivmousepos( NULL, &row, &col );
        GUIInitMouse( Param );
        uisetmouseposn( row, col );
    }
}

void GUIGMouseOff( void )
{
    if( GMouseOn && GraphicsMouse ) {
        GUIFiniMouse();
    }
}

bool GUIHasCharRemap( void )
{
    return( GraphicsDlg );
}
