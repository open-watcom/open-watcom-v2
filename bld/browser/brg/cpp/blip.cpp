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


#include <stdio.h>
#include "blip.h"

#ifndef STANDALONE_MERGER
static char BlipCount::_Buffer[ MAXBLIPBUFFER ];
#endif

BlipCount::BlipCount( const char * str )
                : _str( str )
                , _count( 0 )
                , _blipCount( 0 )
                , _spinState( 0 )
                , _spinPos( 0 )
                , _spinDelta( +1 )
                , MaxSpin( 40 )
//----------------------------------
{
    start( str );
}

BlipCount::~BlipCount()
//---------------------
{
}

void BlipCount::start( const char * startMessage )
//------------------------------------------------
{
    _str = startMessage;

#ifndef STANDALONE_MERGER
    strcpy( _Buffer, _str );
    browseTop->statusText( _Buffer );
#else
    printf( "%s", _str );
    fflush( stdout );
#endif
}

void BlipCount::doBlip()
//----------------------
{
    const int   MaxBlip = 50;       // if blipCount == MaxBlip, change spinner
    const char  Spins[] = { '-', '\\', '|', '/' };

    _count ++;
    _blipCount ++;

    if( _blipCount == MaxBlip ) {
        _spinPos += _spinDelta;
        if( _spinPos == 1 ) {
            _spinDelta = 1;
        }
        if( _spinPos == MaxSpin ) {
            _spinDelta = -1;
        }

#ifndef STANDALONE_MERGER
        sprintf( _Buffer, "%s: %*c%*c", _str, _spinPos, Spins[ _spinState ],
                                        MaxSpin - _spinPos, ' ' );
        browseTop->statusText( _Buffer );
#else
        printf( "\r%s: %*c%*c", _str, _spinPos, Spins[ _spinState ],
                                        MaxSpin - _spinPos, ' ' );
        fflush( stdout );
#endif

        _blipCount = 0;

        _spinState += _spinDelta;
        _spinState &= 0x03;     // 0, 1, 2, or 3
    }
}


void BlipCount::end( const char * endMsg )
//----------------------------------------
{
    endMsg = endMsg;

    // NYI -- this has to be re-implemented nicely for standalone and GUI

#ifndef STANDALONE_MERGER
    strcat( _Buffer, endMsg );
    browseTop->statusText( _Buffer );
#else
    puts( endMsg );
    fflush( stdout );
#endif
}
