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


#ifndef wmetrics_class
#define wmetrics_class

#include "_windows.hpp"
#include "wobject.hpp"

WCLASS          WRect;

WCLASS WSystemMetrics {
    public:
        static bool WEXPORT mousePresent() { return( _metrics.mouse ); }
        static int WEXPORT captionSize( void ) {
            return( _metrics.caption_size );
        }
        static int WEXPORT frameWidth( void ) {
            return( _metrics.resize_border.x );
        }
        static int WEXPORT frameHeight( void ) {
            return( _metrics.resize_border.y );
        }
        static int WEXPORT dialogFrameWidth( void ) {
            return( _metrics.dialog_bottom_right_size.x );
        }
        static int WEXPORT dialogFrameHeight( void ) {
            return( _metrics.dialog_bottom_right_size.y );
        }
        static int WEXPORT vScrollBarWidth( void ) {
            return( _metrics.scrollbar_size.x );
        }
        static int WEXPORT hScrollBarHeight( void ) {
            return( _metrics.scrollbar_size.y );
        }
        static void WEXPORT screenSize( WRect& r );
        static void WEXPORT screenCoordinates( WRect& r );
        static void WEXPORT defaultRectangle( WRect& r );

    public:
        static WRect                    _initDefRect;
        static WRect                    _defRect;
        static WRect                    _screenCoords;
        static gui_system_metrics       _metrics;
        static gui_rect                 _screen;
};

#endif
