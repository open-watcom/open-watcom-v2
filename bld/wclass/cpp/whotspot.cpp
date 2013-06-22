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


#include "whotspot.hpp"
#include "wstring.hpp"


WEXPORT WHotSpots::WHotSpots( int num_hot_spots )
        : _numHotSpots( num_hot_spots )
        , _hotSpotIndex( 0 )
/***************************************/
{

    _hotSpotItems = new gui_resource[ num_hot_spots ];
}


WEXPORT WHotSpots::~WHotSpots() {
/*******************************/

    if( _hotSpotItems ) {
        delete []_hotSpotItems;
    }
}


void WEXPORT WHotSpots::addHotSpot( WResource bitmap, char *ch_mode ) {
/*********************************************************************/

    if( _hotSpotIndex < _numHotSpots ) {
        _hotSpotItems[_hotSpotIndex].res = bitmap;
        _hotSpotItems[_hotSpotIndex].chars = ch_mode;
        ++_hotSpotIndex;
    }
}


void WEXPORT WHotSpots::hotSpotSize( int hot_spot, WPoint &size ) {
/*****************************************************************/

    gui_coord   s;

    GUIGetHotSpotSize( hot_spot, &s );
    size.x( s.x );
    size.y( s.y );
}


void WHotSpots::attach() {
/************************/

    if( _hotSpotItems != NULL ) {
        GUIInitHotSpots( _numHotSpots, _hotSpotItems );
    }
}
