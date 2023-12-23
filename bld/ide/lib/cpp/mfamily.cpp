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


WCLASS StrMapItem : public WObject {
    public:
        StrMapItem( WString& id, WString& text, bool id2text );
        virtual bool isEqual( const WObject* obj ) const;
        bool isIdEqual( const char* id ) const
            { return( strcmp( _id, id ) == 0 && _id2text ); }
        bool isTextEqual( const char* text, int kludge ) const;

        WString     _id;
        WString     _text;
        int         _textlen;
        bool        _id2text;
};

StrMapItem::StrMapItem( WString& id, WString& text, bool id2text )
{
    _id = id;
    _text = text;
    _id2text = id2text;
    _textlen = strlen( text );
}

bool StrMapItem::isEqual( const WObject* o ) const
{
    return( ((StrMapItem*)o)->_id2text == _id2text
            && ((StrMapItem*)o)->_text.isEqual( &_text )
            && ((StrMapItem*)o)->_id.isEqual( &_id ) );
}

bool StrMapItem::isTextEqual( const char* text, int kludge ) const
{
    for( int i = 0; i < _textlen; i++ ) {
        int ct = (unsigned char)_text[i];
        int cs = (unsigned char)*text++;
        if( cs == '\0' )
            return( false );
        if( ct == cs )
            continue;
        if( kludge == 1 ) {
            // ignore dash/space mismatch
            if( cs == '-' && ct == ' ' || cs == ' ' && ct == '-' )
                continue;
            // ignore upper/lower case mismatch
            if( toupper( cs ) == toupper( ct ) ) {
                continue;
            }
        }
        return( false );
    }
    return( *text == '\0' );
}


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
        } else if( tok == "SwitchText" ) {
            WString id;
            fil.token( id );
            if( id.size() > 0 ) {
                fil.token( tok );
                if( tok.size() > 0 ) {
                    _switchesIds.add( new StrMapItem( id, tok, true ) );
                }
                // define map "text -> id" for older versions of project files
                while( !fil.eol() ) {
                    fil.token( tok );
                    if( tok.size() > 0 ) {
                        _switchesIds.add( new StrMapItem( id, tok, false ) );
                    }
                }
            }
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
    _switchesIds.deleteContents();
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

MSwitch* WEXPORT MFamily::findSwitch( const char* swtag, int kludge )
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
    for( int i = 0; i < icount; i++ ) {
        MSwitch* sw = (MSwitch*)_switches[i];
        if( sw->isTagEqual( swtag, kludge ) ) {
            return( sw );
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
        if( !setable || sw->isSetable() ) {
            /*
             * Important notes how switch definitions are selected
             * into the list.
             *
             * 1. switch definition without ID
                    every active definition (by mask) is included
             * 2. switch definition with ID
             *      only first active definition (by mask) is included
             */
            if(  /* first switch */ lastSw == NULL
              || /* no ID switch */ sw->id()[0] == '\0'
              || /* new switch */   strcmp( sw->id(), lastSw->id() ) != 0 ) {
                if( sw->addSwitch( list, mask ) ) {
                    /*
                     * save first active switch definition
                     * for switch change comparision
                     */
                    lastSw = sw;
                }
            }
        }
    }
}

WString* WEXPORT MFamily::translateID( const char* id, WString& text )
{
    int icount = _switchesIds.count();
    for( int i = 0; i < icount; i++ ) {
        StrMapItem* x = (StrMapItem*)_switchesIds[i];
        if( x->isIdEqual( id ) ) {
            text = x->_text;
            return( &text );
        }
    }
    return( NULL );
}

WString* WEXPORT MFamily::findSwitchIdByText( const char* text, int kludge )
{
    int icount = _switchesIds.count();
    for( int i = 0; i < icount; i++ ) {
        StrMapItem* x = (StrMapItem*)_switchesIds[i];
        if( x->isTextEqual( text, kludge ) ) {
            return( &(x->_id) );
        }
    }
    return( NULL );
}

WString* WEXPORT MFamily::displayText( MSwitch *sw, WString& text )
{
    sw->getId( text );
    translateID( text, text );
    sw->addOptText( text );
    return( &text );
}
