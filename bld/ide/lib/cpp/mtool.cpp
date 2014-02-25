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


#include "wobjfile.hpp"
#include "mconfig.hpp"
#include "mfamily.hpp"
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
            if( tool != NULL ) {
                _incTools.add( tool );
            }
            fil.token( tok );
#if CUR_CFG_VERSION > 4
        } else if( _config->version() > 4 && tok == "SwitchText" ) {
            WString id;
            fil.token( id );
            fil.token( tok );
            if( tok.size() > 0 ) {
                _switchesTexts.setThis( new WString( tok ), new WString( id ) );
            }
            // define map "text -> id" for older versions of project files
            while( !fil.eol() ) {
                fil.token( tok );
                if( tok.size() > 0 ) {
                    // define new switch text for map
                    _switchesIds.setThis( new WString( id ), new WString( tok ) );
                }
            }
            fil.token( tok );
#endif
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
#if CUR_CFG_VERSION > 4
    _switchesTexts.deleteContents();
    _switchesIds.deleteContents();
#endif
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

MSwitch* WEXPORT MTool::findSwitch( WString& switchtag, long fixed_version )
{
    //
    // Open Watcom IDE configuration/project files are buggy
    // There are many switch ID's which were changed by incompatible way
    // IDE uses various hacks to fix it later instead of proper solution
    // It is very hard to detect what was broken in each OW version because
    // there vere no change to version number of project files
    //
    if( fixed_version != 0 && fixed_version < 41 && _config->version() < 5 ) {
        //
        // hack for buggy version of configuration/project files
        //
        FixTypo( switchtag );
    }
    int icount = _families.count();
    for( int i = 0; i < icount; i++ ) {
        MFamily* family = (MFamily*)_families[i];
        MSwitch* sw = family->findSwitch( this, switchtag, fixed_version );
        if( sw != NULL ) {
            return sw;
        }
    }
    icount = _incTools.count();
    for( int i = 0; i < icount; i++ ) {
        MTool* tool = (MTool*)_incTools[i];
        MSwitch* sw = tool->findSwitch( switchtag, fixed_version );
        if( sw != NULL ) {
            return sw;
        }
    }
    return NULL;
}

WString *WEXPORT MTool::displayText( MSwitch *sw, WString& text, bool first )
{
#if CUR_CFG_VERSION > 4
    WString *switchid = &sw->text();
    WString *switchtext;
    int icount;

    if( _config->version() > 4 ) {
        switchtext = (WString *)_switchesTexts.findThis( switchid );
        if( switchtext != NULL ) {
            text = *switchtext;
            sw->displayText( text );
            return &text;
        }
        icount = _incTools.count();
        for( int i = 0; i < icount; i++ ) {
            MTool* tool = (MTool*)_incTools[i];
            if( tool->displayText( sw, text, false ) != NULL ) {
                return &text;
            }
        }
        if( !first ) {
            return NULL;
        }
    }
    text = *switchid;
#else
    first = first;
    text = sw->text();
#endif
    sw->displayText( text );
    return &text;
}

#if CUR_CFG_VERSION > 4
WString* WEXPORT MTool::findSwitchByText( WString& id, WString& text, int kludge )
{
    int icount;

    if( kludge == 0 ) {         // check current text
        if( text.isEqual( (WString *)_switchesTexts.findThis( &id ) ) ) {
            return &id;
        }
    } else if( kludge == 1 ) {  // check old text
        if( id.isEqual( (WString *)_switchesIds.findThis( &text, &id ) ) ) {
            return &id;
        }
    }
    icount = _incTools.count();
    for( int i = 0; i < icount; i++ ) {
        MTool* tool = (MTool*)_incTools[i];
        if( tool->findSwitchByText( id, text, kludge ) != NULL ) {
            return &id;
        }
    }
    return NULL;
}
#endif

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
