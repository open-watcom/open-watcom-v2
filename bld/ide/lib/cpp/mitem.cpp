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
#include "mproject.hpp"
#include "mitem.hpp"
#include "mcompon.hpp"
#include "mstate.hpp"
#include "mrswitch.hpp"
#include "maction.hpp"
#include "mactset.hpp"

extern "C" {
#ifndef __UNIX__
    #include <dos.h>
#endif
};

Define( MItem )

WEXPORT MItem::MItem( const char* name, MComponent* comp, MRule* rule, bool isTarget )
    : WFileName( name, _config->getPathSep() )
    , _component( comp )
    , _ruleTag( rule->tag() )
    , _rule( rule )
    , _parent( NULL )
    , _attribs( 0 )
    , _exists( true )
    , _isTarget( isTarget )
    , _owner( MITEM_OWNER_IDE )
    , _expanded( true )
{
    updateAttribs();
}

WEXPORT MItem::~MItem()
{
    _states.deleteContents();
    _actionStates.deleteContents();
}

#ifndef NOPERSIST
MItem* WEXPORT MItem::createSelf( WObjectFile& )
{
    return( new MItem( "", NULL, _config->nilRule() ) );
}

void WEXPORT MItem::readSelf( WObjectFile& p )
{
    WFileName::readSelf( p );
    p.readObject( &_ruleTag );
    _rule = _config->findRule( _ruleTag );
    if( !_rule ) {
        _rule = _config->nilRule();
    }
    p.readObject( &_states );
    if( p.version() > 24 ) {
        p.readObject( &_actionStates );
    }
    _parent = (MItem*)p.readObject();
    _component = (MComponent*)p.readObject();
    updateAttribs();
    if( p.version() > 30 ) {
        p.readObject( &_expanded );
    }
    if( p.version() > 37 ) {
        p.readObject( &_owner );
    } else {
        _owner = MITEM_OWNER_IDE;
    }
    if( p.version() < 28 ) {
        bool temp;
        p.readObject( &temp );  //include
        p.readObject( &temp );  //exclude
        p.readObject( &temp );  //shadow
    }
    if( p.version() < 24 ) {
        WString mask;
        p.readObject( &mask );
        _component->mask() = mask;
    }

    pruneStates( _states );

    //this part needs to be done after the item is completely constructed!
    int icount = _actionStates.count();
    if( icount ) {
        WVList actions;
        addActions( actions );
        for( int i=icount; i>0; ) {
            i--;
            ActionStates* astates = (ActionStates*)_actionStates[i];
            if( astates->linkup( actions ) ) {
                pruneStates( astates->states() );
            } else {
                delete _actionStates.removeAt( i );
            }
        }
    }
}

void WEXPORT MItem::writeSelf( WObjectFile& p )
{
    WFileName::writeSelf( p );
    p.writeObject( &_ruleTag );
    p.writeObject( &_states );
    p.writeObject( &_actionStates );
    p.writeObject( _parent );
    p.writeObject( _component );
    p.writeObject( _expanded );
    p.writeObject( _owner );
}
#endif

int MItem::compare( const WObject* obj ) const
{
    int comp = 0;
    MItem* robj = (MItem*)obj;
    if( _parent ) {
        if( robj->_parent ) {
            comp = stricmp( *_parent, *robj->_parent );
            if( comp == 0 ) comp = stricmp( *this, *robj );
        } else {
            comp = stricmp( *_parent, *robj );
            if( comp == 0 ) comp = 1;
        }
    } else {
        if( robj->_parent ) {
            comp = stricmp( *this, *robj->_parent );
            if( comp == 0 ) comp = -1;
        } else {
            comp = stricmp( *this, *robj );
        }
    }
    return( comp );
}

bool MItem::ismakeable()
{
    return( rule()->ismakeable() );
}

void MItem::absName( WFileName& fn )
{
    fn.puts( *this );
    fn.absoluteTo( _component->filename() );
}

bool MItem::result( WFileName& fn )
{
    if( ismakeable() ) {
        fn.setFName( fName() );
        if( _isTarget ) {
            fn.setExt( ext() );
        } else {
            fn.setExt( rule()->resultExt() );
        }
        return( true );
    }
    return( false );
}

bool MItem::absResult( WFileName& fn )
{
    if( result( fn ) ) {
        fn.absoluteTo( _component->filename() );
        return( true );
    }
    return( false );
}

bool MItem::touchResult()
{
    WFileName t;
    if( absResult( t ) ) {
        t.touch();
        return( true );
    }
    return( false );
}

void MItem::updateAttribs()
{
    if( _component && size() > 0 ) {
        WFileName fn;
        absName( fn );
        _exists = fn.attribs( &_attribs );
    }
}

void MItem::addDecorators( WString& n )
{
    if( _config->debug() ) {
        n.concat( " '" );
        n.concat( _ruleTag );
        n.concat( "'" );
    }
    if( _states.count() > 0 ) {
        n.concat( " [sw]" );
    }
    if( !isMask() ) {
        if( _exists ) {
#ifndef __UNIX__
            if( (_attribs & _A_RDONLY) ) {
                n.concat( " [r/o]" );
            }
#endif
        } else {
            n.concat( " [n/a]" );
        }
    }
}

void MItem::name( WString& n )
{
    n = "";
    if( !isMask() ) {
        n.concat( *this );
    } else {
        n.concat( "( " );
        n.concat( this->ext() );
        n.concat( " )" );
    }
    addDecorators( n );
}

void MItem::setRule( MRule* rule )
{
    _rule = rule;
    _ruleTag = _rule->tag();
    resetRuleRefs();
}

bool MItem::rename( WFileName& name, MRule* rule )
{
    puts( name );
    updateAttribs();
    setRule( rule );
    return( true );
}

void MItem::addStates( WVList& list, SwMode mode )
{
    int icount = _states.count();
    for( int i=0; i<icount; i++ ) {
        MState* st = (MState*)_states[i];
        if( st->mode() == mode ) {
            list.add( _states[i] );
        }
    }
}

void MItem::addActions( WFileName& fn, WVList& list )
{
    WVList rules;
    if( _config->findMatchingRules( fn, _component->mask(), rules ) ) {
        for( int i=0; i<rules.count(); i++ ) {
            MRule* r = (MRule*)rules[i];
            for( int j=0; j<r->actions().count(); j++ ) {
                list.add( r->actions()[j] );
            }
        }
    }
}

int MItem::addActions( WVList& list )
{
    if( !_isTarget ) {
        //add actions for Source
        addActions( *this, list );
    }
    //add actions for result
    int ocount = list.count();
    int icount = _rule->actions().count();
    for( int i=0; i<icount; i++ ) {
        list.add( _rule->actions()[i] );
    }
    WFileName fn;
    if( result( fn ) ) {
        addActions( fn, list );
    }
    return( ocount );
}

WVList& MItem::getActionStates( MAction* action )
{
    int icount = _actionStates.count();
    for( int i=0; i<icount; i++ ) {
        ActionStates* as = (ActionStates*)_actionStates[i];
        if( as->action() == action ) {
            return( as->states() );
        }
    }
    ActionStates* as = new ActionStates( action );
    _actionStates.add( as );
    return( as->states() );
}

WVList* MItem::findActionStates( MAction* action )
{
    int icount = _actionStates.count();
    for( int i=0; i<icount; i++ ) {
        ActionStates* as = (ActionStates*)_actionStates[i];
        if( as->action() == action ) {
            return( &as->states() );
        }
    }
    return( NULL );
}

int MItem::expandAction( const WString& actionName, WString& command )
{
    WVList actions;
    addActions( actions );
    for( int i=0; i<actions.count(); i++ ) {
        MAction* action = (MAction*)actions[i];
        if( action->name().match( actionName ) ) {
            return( expandAction( action, command ) );
        }
    }
    return( -1 );
}

int MItem::expandAction( MAction* action, WString& command )
{
    WFileName fn;
    if( _isTarget ) {
        absName( fn );
    } else {
        WVList actions;
        int ocount = addActions( actions );
        if( !action ) {
            int icount = actions.count();
            if( icount > 0 ) {
                for( int i=0; i<icount; i++ ) {
                    MAction* act = (MAction*)actions[i];
                    if( act->defAction() ) {
                        if( act->okForMask() || !isMask() ) {
                            action = act;
                            break;
                        }
                    }
                }
            }
        }
        if( action ) {
            int ix = actions.indexOfSame( action );
            if( ix < 0 ) {
                action = NULL;
            } else if( ix >= ocount ) {
                absResult( fn );
            } else {
                absName( fn );
            }
        }
    }
    if( action ) {
        WVList* states = findActionStates( action );
        return( action->expand( command, &fn, _component->mask(), states, _component->mode() ) );
    }
    return( -1 );
}

void MItem::resetRuleRefs()
{
    if( _ruleTag != _rule->tag() ) {
        _rule = _config->findRule( _ruleTag );
    }
    int icount = _states.count();
    for( int i=0; i<icount; i++ ) {
        MState* s = (MState*)_states[i];
        s->resetRuleRefs();
    }
    pruneStates( _states );
}

void MItem::pruneStates( WVList& states )
{
    for( int i=states.count(); i>0; ) {
        i--;
        MState* s = (MState*)states[i];
        if( !s->legal() ) {
            delete states.removeAt( i );
        }
    }
}

int MItem::type()
{
    if( _parent ) {
        return( 3 );
    }
    if( !_expanded ) {
        return( 2 );
    }
    return( 1 );
}

bool MItem::included()
{
    if( _parent ) {
        return( _parent->_expanded );
    }
    return( true );
}
