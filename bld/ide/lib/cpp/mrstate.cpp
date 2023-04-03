/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include "idecfg.h"
#include "wobjfile.hpp"
#include "mconfig.hpp"
#include "mrstate.hpp"
#include "mrswitch.hpp"

Define( MRState )

MRState::MRState( MTool* tool, SwMode mode, MRSwitch* sw, bool state )
    : MState( tool, mode, sw, state )
{
}

#ifndef NOPERSIST
MRState* WEXPORT MRState::createSelf( WObjectFile& )
{
    return( new MRState( _config->nilTool() ) );
}

void WEXPORT MRState::readSelf( WObjectFile& p )
{
    MState::readSelf( p );
    if( p.version() < 41 ) {
        MState::readState( p );
    }
}

void WEXPORT MRState::writeSelf( WObjectFile& p )
{
    MState::writeSelf( p );
}
#endif
