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

#include "gtclsopt.h"

GTClassOpts::GTClassOpts()
//------------------------
{
    int i;
    PaintInfo DefaultClassOpt[ 6 ] = {
        PaintInfo( ColorBlack,  1,      LS_PEN_SOLID ),
        PaintInfo( ColorBlue,   1,      LS_PEN_SOLID ),
        PaintInfo( ColorRed,    1,      LS_PEN_SOLID ),
        PaintInfo( ColorBlack,  1,      LS_PEN_DOT ),
        PaintInfo( ColorBlue,   1,      LS_PEN_DOT ),
        PaintInfo( ColorRed,    1,      LS_PEN_DOT ),
    };

    for( i = 0; i < 6; i += 1 ) {
        _values[ i ] = DefaultClassOpt[ i ];
    }
}

GTClassOpts::GTClassOpts( const GTClassOpts & o )
//-----------------------------------------------
{
    int i;

    for( i = 0; i < 6; i += 1 ) {
        _values[ i ] = o._values[ i ];
    }
}

const PaintInfo & GTClassOpts::value( uint access, uint virt ) const
//------------------------------------------------------------------
{
    int vmult = virt ? 1 : 0;

    return _values[ access + vmult * 3 - 1 ];
}

PaintInfo & GTClassOpts::value( uint access, uint virt )
//------------------------------------------------------
{
    int vmult = virt ? 1 : 0;

    return _values[ access + vmult * 3 - 1 ];
}

