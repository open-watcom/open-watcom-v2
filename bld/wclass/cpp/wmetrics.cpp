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


#include "wmetrics.hpp"
#include "wrect.hpp"

WRect                    WSystemMetrics::_initDefRect;
WRect                    WSystemMetrics::_defRect;
WRect                    WSystemMetrics::_screenCoords;
gui_system_metrics       WSystemMetrics::_metrics;
gui_rect                 WSystemMetrics::_screen;

void    InitWClass( gui_rect *screen ) {
/**************************************/

    WSystemMetrics::_screenCoords.x( screen->x );
    WSystemMetrics::_screenCoords.y( screen->y );
    WSystemMetrics::_screenCoords.w( screen->width );
    WSystemMetrics::_screenCoords.h( screen->height );
    WSystemMetrics::_initDefRect.x( screen->x );
    WSystemMetrics::_initDefRect.y( screen->y );
    WSystemMetrics::_initDefRect.w( ( 3 * screen->width ) / 4 );
    WSystemMetrics::_initDefRect.h( ( 2 * screen->height ) / 3 );
    WSystemMetrics::_defRect = WSystemMetrics::_initDefRect;
    GUIGetSystemMetrics( &WSystemMetrics::_metrics );
    GUIGetScreen( &WSystemMetrics::_screen );
}


void WEXPORT WSystemMetrics::screenSize( WRect& r ) {
/***************************************************/

    r.x( 0 );
    r.y( 0 );
    r.w( _screen.width );
    r.h( _screen.height );
}


void WEXPORT WSystemMetrics::screenCoordinates( WRect& r ) {
/**********************************************************/

    r = _screenCoords;
}


void WEXPORT WSystemMetrics::defaultRectangle( WRect& r ) {
/*********************************************************/

    r = _defRect;
    _defRect.x( _defRect.x() + WSystemMetrics::captionSize() );
    _defRect.y( _defRect.y() + WSystemMetrics::captionSize() );
    if( (_defRect.x() > _initDefRect.w()/3) ||
        (_defRect.y() > _initDefRect.h()/3) ) {
        _defRect = _initDefRect;
    }
}
