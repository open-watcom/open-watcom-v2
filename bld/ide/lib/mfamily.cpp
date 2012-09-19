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


#include "mfamily.hpp"
#include "mvswitch.hpp"
#include "mcswitch.hpp"
#include "mrswitch.hpp"
#include "mc2swtch.hpp"
#include "wobjfile.hpp"

Define( MFamily )

MFamily::MFamily( WTokenFile& fil, WString& tok )
{
    if( !fil.eol() ) {
        fil.token( _name );
    }
    WString rGroup;
    for(;;) {
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
}

#ifndef NOPERSIST
MFamily* WEXPORT MFamily::createSelf( WObjectFile& )
{
    return new MFamily();
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
        if( !setable || ( sw->text().size() > 0 && sw->text()[0] != ' ' ) ) {
            return TRUE;
        }
    }
    return FALSE;
}

MSwitch* WEXPORT MFamily::findSwitch( WString& switchtag )
{
    int icount = _switches.count();
    for( int i=0; i<icount; i++ ) {
        MSwitch* sw = (MSwitch*)_switches[i];
        WString tag;
        sw->getTag( tag );
        if( tag == switchtag ) {
            return sw;
        }
        sw->getCompatibleTag( tag );
        if( !stricmp( tag, switchtag ) ) {
            return sw;
        }
    }
    return NULL;
}

void MFamily::addSwitches( WVList& list, const char* mask, bool setable )
{
    MSwitch* lastSw = NULL;
    int icount = _switches.count();
    for( int i=0; i<icount; i++ ) {
        MSwitch* sw = (MSwitch*)_switches[i];
        bool hasText = (sw->text().size() > 0 );
        if( !setable || hasText ) {
            if( !lastSw || !hasText || lastSw->text() != sw->text() ) {
                if( sw->addSwitch( list, mask ) ) {
                    lastSw = sw;
                }
            }
        }
    }
}
