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


#include <wwindow.hpp>
#include "gtfnopt.h"

GTFunctionOpts::GTFunctionOpts()
//------------------------------
{
    int i;
    const Color ColorYellow = CUSTOM_RGB(255,255,0);

    PaintInfo DefaultNumCalls[ 5 ] = {
        PaintInfo( ColorBlack,  1,      LS_PEN_SOLID ),
/* line thickness not supported under PM
        PaintInfo( ColorBlack,  2,      LS_PEN_SOLID ),
        PaintInfo( ColorBlack,  3,      LS_PEN_SOLID ),
        PaintInfo( ColorBlack,  4,      LS_PEN_SOLID ),
        PaintInfo( ColorBlack,  5,      LS_PEN_SOLID ),
*/
        PaintInfo( ColorBlack,  1,      LS_PEN_SOLID ),
        PaintInfo( ColorBlack,  1,      LS_PEN_SOLID ),
        PaintInfo( ColorBlack,  1,      LS_PEN_SOLID ),
        PaintInfo( ColorBlack,  1,      LS_PEN_SOLID ),
    };

    PaintInfo DefaultRepetition[ 2 ] = {
        PaintInfo( ColorBlack,  1,      LS_PEN_SOLID ),
/* line thickness not supported under PM
        PaintInfo( ColorRed,    2,      LS_PEN_DASH ),
*/
        PaintInfo( ColorRed,    1,      LS_PEN_DASH ),
    };

    for( i = 0; i < 5; i += 1 ) {
        _numCalls[ i ] = DefaultNumCalls[ i ];
    }

    for( i = 0; i < 2; i += 1 ) {
        _repetition[ i ] = DefaultRepetition[ i ];
    }
}

GTFunctionOpts::GTFunctionOpts( const GTFunctionOpts & o )
//--------------------------------------------------------
{
    int i;

    for( i = 0; i < 5; i += 1 ) {
        _numCalls[ i ] = o._numCalls[ i ];
    }

    for( i = 0; i < 2; i += 1 ) {
        _repetition[ i ] = o._repetition[ i ];
    }
}

const PaintInfo & GTFunctionOpts::numCalls( uint num ) const
//----------------------------------------------------------
{
    if( num < 5 ) {
        return _numCalls[ num - 1 ];
    } else {
        return _numCalls[ 4 ];
    }
}

const PaintInfo & GTFunctionOpts::repeated( bool rep ) const
//----------------------------------------------------------
{
    if( rep ) {
        return _repetition[ 1 ];
    } else {
        return _repetition[ 0 ];
    }
}

PaintInfo & GTFunctionOpts::numCalls( uint num )
//----------------------------------------------
{
    if( num < 5 ) {
        return _numCalls[ num - 1 ];
    } else {
        return _numCalls[ 4 ];
    }
}

PaintInfo & GTFunctionOpts::repeated( bool rep )
//----------------------------------------------
{
    if( rep ) {
        return _repetition[ 1 ];
    } else {
        return _repetition[ 0 ];
    }
}
