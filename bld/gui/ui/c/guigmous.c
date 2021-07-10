/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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

#ifdef __DOS__
static bool         GraphicsMouse   = false;
static bool         GraphicsDlg     = false;
static bool         GMouseOn        = false;
static init_mode    GMinstall       = INIT_MOUSELESS;
#endif

void GUIInitGraphicsMouse( gui_window_styles style )
{
#ifdef __DOS__
    if( style & GUI_CHARMAP_MOUSE ) {
        GraphicsMouse = true;
    }
    if( style & GUI_CHARMAP_DLG ) {
        GraphicsDlg = true;
    }
#else
    /* unused parameters */ (void)style;
#endif
}

bool GUIInitMouse( init_mode install )
{
    bool    init;

#ifdef __DOS__
    GMinstall = install;
    if( GraphicsMouse ) {
        init = uiinitgmouse( install );
        if( GraphicsDlg ) {
            FlipCharacterMap();
        }
    } else {
        init = initmouse( install );
    }
    GMouseOn = true;
#else
    init = initmouse( install );
#endif
    return( init );
}

void GUIFiniMouse( void )
{
#ifdef __DOS__
    if( GraphicsMouse ) {
        if( GraphicsDlg ) {
            FlipCharacterMap();
        }
        uifinigmouse();
    } else {
        finimouse();
    }
    GMouseOn = false;
#else
    finimouse();
#endif
}

void GUIAPI GUIGMouseOn( void )
{
#ifdef __DOS__
    ORD row;
    ORD col;

    if( !GMouseOn && GraphicsMouse ) {
        uivmousepos( NULL, &row, &col );
        GUIInitMouse( GMinstall );
        uisetmouseposn( row, col );
    }
#endif
}

void GUIAPI GUIGMouseOff( void )
{
#ifdef __DOS__
    if( GMouseOn && GraphicsMouse ) {
        GUIFiniMouse();
    }
#endif
}

bool GUIHasCharRemap( void )
{
#ifdef __DOS__
    return( GraphicsDlg );
#else
    return( false );
#endif
}
