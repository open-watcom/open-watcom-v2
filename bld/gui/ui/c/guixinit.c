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
#include "uiattrs.h"
#include "guiscale.h"
#include "guiclock.h"
#include "guisysin.h"
#include "guigmous.h"
#include "uigchar.h"
#include "guixinit.h"
#include "guidraw.h"

static gui_window_styles        GUIStyle =      GUI_PLAIN;
static bool                     GUIInitialized = FALSE;

gui_window_styles GUIGetWindowStyles( void )
{
    return( GUIStyle );
}

/*
 * GUIWndInit -- initialize display
 */

bool GUIWndInit( unsigned DClickInterval, gui_window_styles style )
{
    gui_colour_set      background = { GUI_WHITE, GUI_BLUE };

    GUIMemOpen();
    GUIStyle = style;
    GUIInitGraphicsMouse( style );
    if( !GUISysInit( 2 ) ) {
        return( FALSE );
    }
    uiattrs();
    UIData->f10menus = FALSE;
    UIData->mouse_clk_delay = uiclockdelay( DClickInterval );
    UIData->tick_delay = uiclockdelay( TICK_DELAY );
    UIData->mouse_rpt_delay = 0;
    UIData->mouse_acc_delay = uiclockdelay( 250 );
    GUISetScreen( XMIN, YMIN, XMAX-XMIN, YMAX-YMIN );
    GUISetBackgroundColour( &background );
    GUIInitDrawingChars( uiisdbcs() );
    if( !GUIHasCharRemap() ) {
        UiGChar[UI_BOX_TOP_LEFT] = ' ';
        UiGChar[UI_BOX_TOP_RIGHT] = ' ';
        UiGChar[UI_BOX_BOTTOM_RIGHT] = ' ';
        UiGChar[UI_BOX_BOTTOM_LEFT] = ' ';
        UiGChar[UI_BOX_TOP_LINE] = ' ';
        UiGChar[UI_BOX_RIGHT_LINE] = DRAW( LEFT_HALF );
        UiGChar[UI_BOX_BOTTOM_LINE] = ' ';
        UiGChar[UI_BOX_LEFT_LINE] = DRAW( RIGHT_HALF );
    }
    GUIInitialized = TRUE;
    return( TRUE );
}

void GUISetF10Menus( bool setting )
{
    UIData->f10menus = setting;
}

bool GUIIsInit( void )
{
    return( GUIInitialized );
}

