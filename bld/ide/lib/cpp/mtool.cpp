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


#include "mtool.hpp"
#include "mconfig.hpp"
#include "mfamily.hpp"
#include "wobjfile.hpp"
#include "mtypo.hpp"

Define( MTool )

MTool::MTool( WTokenFile& fil, WString& tok )
{
    fil.token( _tag );
    fil.token( _name );
    fil.token( tok );
    for(;;) {
        if( tok == "Family" ) {
            _families.add( new MFamily( fil, tok ) );
        } else if( tok == "IncludeTool" ) {
            MTool* tool = _config->findTool( fil.token( tok ) );
            if( tool ) {
                _incTools.add( tool );
            }
            fil.token( tok );
        } else if( tok == "Help" ) {
            fil.token( _help );
            fil.token( tok );
        } else if( tok == "rem" ) {
            fil.flushLine( tok );
            fil.token( tok );
        } else {
            break;
        }
    }
}

MTool::MTool( const char* name, const char* tag )
    : _tag( tag )
    , _name( name )
{
}

MTool::~MTool()
{
    _families.deleteContents();
}

#ifndef NOPERSIST
MTool* WEXPORT MTool::createSelf( WObjectFile& )
{
    return new MTool( NULL, NULL );
}

void WEXPORT MTool::readSelf( WObjectFile& p )
{
    WObject::readSelf( p );
    p.readObject( &_tag );
    p.readObject( &_name );
    p.readObject( &_families );
    p.readObject( &_incTools );
}

void WEXPORT MTool::writeSelf( WObjectFile& p )
{
    WObject::writeSelf( p );
    p.writeObject( &_tag );
    p.writeObject( &_name );
    p.writeObject( &_families );
    p.writeObject( &_incTools );
}
#endif

MSwitch* WEXPORT MTool::findSwitch( WString& switchtag, bool fix )
{
    //
    // Open Watcom IDE configuration/project files are buggy
    // There are many switch ID's which were changed by incompatible way
    // IDE uses various hacks to fix it later instead of proper solution
    // It is very hard to detect what was broken in each OW version because
    // there vere no change to version number of project files
    //
    if( fix && _config->version() == 4 ) {
        //
        // hack for buggy version of configuration/project files
        //
        FixTypo( switchtag );
    }
    int icount = _families.count();
    for( int i = 0; i < icount; i++ ) {
        MFamily* family = (MFamily*)_families[i];
        MSwitch* sw = family->findSwitch( switchtag, fix );
        if( sw != NULL ) {
            return sw;
        }
    }
    icount = _incTools.count();
    for( int i = 0; i < icount; i++ ) {
        MTool* tool = (MTool*)_incTools[i];
        MSwitch* sw = tool->findSwitch( switchtag, fix );
        if( sw != NULL ) {
            return sw;
        }
    }
    return NULL;
}

bool MTool::hasSwitches( bool setable )
{
    int icount = _families.count();
    for( int i = 0; i < icount; i++ ) {
        MFamily* family = (MFamily*)_families[i];
        if( family->hasSwitches( setable ) ) {
            return TRUE;
        }
    }
    icount = _incTools.count();
    for( int i = 0; i < icount; i++ ) {
        MTool* tool = (MTool*)_incTools[i];
        if( tool->hasSwitches( setable ) ) {
            return TRUE;
        }
    }
    return FALSE;
}

void MTool::addSwitches( WVList& list, const char* mask, bool setable )
{
    int icount = _families.count();
    for( int i = 0; i < icount; i++ ) {
        MFamily* family = (MFamily*)_families[i];
        family->addSwitches( list, mask, setable );
    }
    icount = _incTools.count();
    for( int i = 0; i < icount; i++ ) {
        MTool* tool = (MTool*)_incTools[i];
        tool->addSwitches( list, mask, setable );
    }
}

void MTool::addFamilies( WVList& list )
{
    int icount = _families.count();
    for( int i = 0; i < icount; i++ ) {
        list.add( _families[i] );
    }
    icount = _incTools.count();
    for( int i = 0; i < icount; i++ ) {
        MTool* tool = (MTool*)_incTools[i];
        tool->addFamilies( list );
    }
}
