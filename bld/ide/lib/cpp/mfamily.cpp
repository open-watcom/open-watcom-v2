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
#include "mconfig.hpp"
#include "mvswitch.hpp"
#include "mcswitch.hpp"
#include "mrswitch.hpp"
#include "mc2swtch.hpp"
#include "mfamily.hpp"
#include "wobjfile.hpp"

Define( MFamily )

MFamily::MFamily( WTokenFile& fil, WString& tok )
{
    if( !fil.eol() ) {
        fil.token( _name );
    }
    WString rGroup;
    for( ;; ) {
        fil.token( tok );
        if( tok == "RGroup" ) {
            fil.token( rGroup );
        } else if( tok == "VSwitch" ) {
            _switches.add( new MVSwitch( fil, tok ) );
        } else if( tok == "C2Switch" ) {
            _switches.add( new MC2Switch( fil, tok ) );
        } else if( tok == "CSwitch" ) {
            _switches.add( new MCSwitch( fil, tok ) );
        } else if( tok == "RSwitch" ) {
            _switches.add( new MRSwitch( fil, tok, rGroup ) );
#if IDE_CFG_VERSION_MAJOR > 4
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
#endif
        } else if( tok == "rem" ) {
            fil.flushLine( tok );
        } else {
            break;
        }
    }
}

MFamily::~MFamily()
{
    _switches.deleteContents();
#if IDE_CFG_VERSION_MAJOR > 4
    _switchesTexts.deleteContents();
    _switchesIds.deleteContents();
#endif
}

#ifndef NOPERSIST
MFamily* WEXPORT MFamily::createSelf( WObjectFile& )
{
    return( new MFamily() );
}

void WEXPORT MFamily::readSelf( WObjectFile& p )
{
    WObject::readSelf( p );
    p.readObject( &_name );
    p.readObject( &_switches );
}

void WEXPORT MFamily::writeSelf( WObjectFile& p )
{
    WObject::writeSelf( p );
    p.writeObject( &_name );
    p.writeObject( &_switches );
}
#endif

bool WEXPORT MFamily::hasSwitches( bool setable )
{
    int icount = _switches.count();
    for( int i=0; i<icount; i++ ) {
        MSwitch* sw = (MSwitch*)_switches[i];
        if( !setable || sw->isSetable() ) {
            return( true );
        }
    }
    return( false );
}

MSwitch* WEXPORT MFamily::findSwitch( WString& swtag, long fixed_version, int kludge )
{
    //
    // Open Watcom IDE configuration/project files are buggy
    // There are many switch ID's which were changed by incompatible way
    // IDE uses various hacks to fix it later instead of proper solution
    // It is very hard to detect what was broken in each OW version because
    // there vere no change to version number of project files
    //
    // type of non-exact search is defined by kludge parameter
    //
    int icount = _switches.count();
    if( fixed_version == 0 || !isSetable( swtag ) ) {
        for( int i = 0; i < icount; i++ ) {
            MSwitch* sw = (MSwitch*)_switches[i];
            if( sw->isTagEqual( swtag, kludge ) ) {
                return( sw );
            }
        }
    } else {
        for( int i = 0; i < icount; i++ ) {
            MSwitch* sw = (MSwitch*)_switches[i];
            if( !sw->isSetable() )
                continue;
            if( sw->isTagEqual( swtag, kludge ) ) {
                return( sw );
            }
        }
    }
    return( NULL );
}

void MFamily::addSwitches( WVList& list, const char* mask, bool setable )
{
    MSwitch* lastSw = NULL;
    int icount = _switches.count();
    for( int i = 0; i < icount; i++ ) {
        MSwitch* sw = (MSwitch*)_switches[i];
        if( !setable || sw->hasText() ) {
            if( !sw->isTextEqual( lastSw ) ) {
                if( sw->addSwitch( list, mask ) ) {
                    lastSw = sw;
                }
            }
        }
    }
}

#if IDE_CFG_VERSION_MAJOR > 4
WString *WEXPORT MFamily::translateID( WString* id, WString& text )
{
    if( _config->version() > 4 ) {
        WString *swtext = (WString *)_switchesTexts.findThis( id );
        if( swtext != NULL ) {
            text = *swtext;
            return( &text );
        }
    }
    return( NULL );
}

WString* WEXPORT MFamily::findSwitchByText( WString& id, WString& text, int kludge )
{
    if( kludge == 0 ) {         // check current text
        if( text.isEqual( (WString *)_switchesTexts.findThis( &id ) ) ) {
            return( &id );
        }
    } else if( kludge == 1 ) {  // check old text
        if( id.isEqual( (WString *)_switchesIds.findThis( &text, &id ) ) ) {
            return( &id );
        }
    }
    return( NULL );
}
#endif

WString *WEXPORT MFamily::displayText( MSwitch *sw, WString& text )
{
    text = sw->text();
  #if IDE_CFG_VERSION_MAJOR > 4
    translateID( &text, text );
  #endif
    sw->concatOptText( text );
    return( &text );
}
