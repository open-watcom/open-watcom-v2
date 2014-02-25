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
#include "mrule.hpp"
#include "maction.hpp"
#include "msymbol.hpp"
#include "mfamily.hpp"
#include "mstate.hpp"
#include "mworkfil.hpp"

Define( MRule )

MRule::MRule( WTokenFile& fil, WString& tok )
    : _tagMask( "*" )
    , _autodepend( FALSE )
{
    fil.token( _tag );
    _tool = _config->nilTool();
    if( !fil.eol() ) {
        MTool* tool = _config->findTool( fil.token( tok ) );
        if( tool ) _tool = tool;
        if( !fil.eol() ) {
            fil.token( _tagMask );
        }
    }
    fil.token( tok );
    for(;;) {
        if( tok == "Command" ) {
            _commands.add( new MCommand( fil, tok ) );
        } else if( tok == "Autodepend" ) {
            _autodepend = TRUE;
            fil.token( tok );
        } else if( tok == "BrowseSwitch" ) {
            fil.token( _browseSwitch );
            fil.token( tok );
        } else if( tok == "Target" ) {
            while( !fil.eol() ) {
                _targets.add( new WFileName( fil.token( tok ) ) );
            }
            fil.token( tok );
        } else if( tok == "Source" ) {
            while( !fil.eol() ) {
                _sources.add( new WFileName( fil.token( tok ) ) );
            }
            fil.token( tok );
        } else if( tok == "Autotrack" ) {
            while( !fil.eol() ) {
                _autotracks.add( new WFileName( fil.token( tok ) ) );
            }
            fil.token( tok );
        } else if( tok == "Action" ) {
            _actions.add( new MAction( fil, tok ) );
        } else if( tok == "Symbol" ) {
            _symbols.add( new MSymbol( fil, tok ) );
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

MRule::MRule( const char* tag, MTool* tool )
    : _tool( tool )
    , _tag( tag )
    , _autodepend( FALSE )
{
}

MRule::~MRule()
{
    _actions.deleteContents();
    _symbols.deleteContents();
    _targets.deleteContents();
    _sources.deleteContents();
    _commands.deleteContents();
}

#ifndef NOPERSIST
MRule* WEXPORT MRule::createSelf( WObjectFile& )
{
    return new MRule( NULL, NULL );
}

void WEXPORT MRule::readSelf( WObjectFile& p )
{
    WObject::readSelf( p );
    _tool = (MTool*)p.readObject();
    p.readObject( &_tag );
    p.readObject( &_tagMask );
    p.readObject( &_targets );
    p.readObject( &_sources );
    p.readObject( &_autodepend );
    p.readObject( &_browseSwitch );
    p.readObject( &_commands );
    p.readObject( &_actions );
    p.readObject( &_symbols );
}

void WEXPORT MRule::writeSelf( WObjectFile& p )
{
    WObject::writeSelf( p );
    p.writeObject( _tool );
    p.writeObject( &_tag );
    p.writeObject( &_tagMask );
    p.writeObject( &_targets );
    p.writeObject( &_sources );
    p.writeObject( _autodepend );
    p.writeObject( &_browseSwitch );
    p.writeObject( &_commands );
    p.writeObject( &_actions );
    p.writeObject( &_symbols );
}
#endif

const char* MRule::resultExt()
{
    if( _targets.count() > 0 ) {
        WFileName* t = (WFileName*)_targets[0];
        return t->ext();
    }
    return ".xxx";
}

bool MRule::matchTarget( WFileName& tgt )
{
    for( int i=0; i<_targets.count(); i++ ) {
        WFileName* t = (WFileName*)_targets[i];
        if( tgt.match( *t ) ) {
            return TRUE;
        }
    }
    return FALSE;
}

bool MRule::matchSource( WFileName& src )
{
    for( int i=0; i<_sources.count(); i++ ) {
        WFileName* s = (WFileName*)_sources[i];
        if( src.match( *s ) ) {
            return TRUE;
        }
    }
    return FALSE;
}

bool MRule::match( WString& tag, WFileName& src, WFileName& tgt )
{
    if( tag.match( _tagMask ) && matchSource( src ) && matchTarget( tgt ) ) {
        return TRUE;
    }
    return FALSE;
}

bool MRule::match( WString& tag, WFileName& tgt )
{
    if( !_sources.count() && tag.match( _tagMask ) && matchTarget( tgt ) ) {
        return TRUE;
    }
    return FALSE;
}

MSymbol* MRule::expandSymbol( WString& v, const char* s, WVList* workFiles )
{
    bool found = FALSE;
    int len = 0;
    MSymbol* o = NULL;
    WString oName;
    int icount = _symbols.count();
    for( int i=0; i<icount; i++ ) {
        o = (MSymbol*)_symbols[i];
        o->name( oName );
        len = oName.size();
        if( strnicmp( s, oName, len ) == 0 ) {
            found = TRUE;
            break;
        }
    }
    if( !found ) o = NULL;
    if( found && workFiles ) {
        bool incName = TRUE;
        char afterName = ' ';
        char sep[2]; strcpy( sep, " " );
        for( bool done=FALSE; !done; ) {
            char ch = s[len];
            switch( ch ) {
            case '!':
                incName = FALSE;
                len += 1;
                break;
            case '=':
                afterName = ch;
                len += 1;
                break;
            case ',':
            case ';':
            case '+':
                sep[0] = ch;
                len += 1;
                break;
            default:
                done = TRUE;
            }
        }
        WString vv;
        int jcount = workFiles->count();
        for( int j=0; j<jcount; j++ ) {
            MWorkFile* f = (MWorkFile*)(*workFiles)[j];
            f->removeQuotes();
            if( !f->isMask() && f->match( o->mask() ) ) {
                char ch = ( oName == "FIL" || oName == "LIBR" ) ? '\'' : '\"';

                if( vv.size() > 0 )
                    vv.concat( sep );
                if( f->needQuotes( ch ) ) {
                    f->addQuotes( ch );
                }
                vv.concat( *f );
                f->removeQuotes( ch );
            }
        }
        if( vv.size() > 0 ) {
            if( incName ) {
                v.concat( oName );
                v.concat( afterName );
            }
            v.concat( vv.gets() );
        }
    }
    return o;
}

void MRule::expandCommand( WString& cmd, MCommand& s, WFileName* target, WVList* workFiles, WString& mask, WVList* stateList, SwMode mode, bool* browse )
{
    MCommand c;
    if( s.size() > 0 ) {
        for( int i=0; s[i] != '\0'; ) {
            WString v;
            int len = 0;
            if( strnicmp( &s[i], BMACRO, 2 ) == 0 ) {
                len = 2;
                WString m;
                for(; s[i+len] != '\0';) {
                    if( s[i+len] == EMACRO ) break;
                    m.concat( s[i+len] );
                    len += 1;
                }
                MSymbol* o = expandSymbol( v, m, workFiles );
                if( s[i+len] == EMACRO ) {
                    len += 1;
                    if( o ) {
                        c.concat( v );
                        i += len;
                    } else {
                        c.concat( s[i] );
                        i += 1;
                    }
                } else {
                    c.concat( s[i] );
                    i += 1;
                }
            } else {
                c.concat( s[i] );
                i += 1;
            }
        }
    }
    WString browseSwitch( _browseSwitch );
    c.expand( cmd, target, _tool, mask, stateList, mode, &browseSwitch );
    if( browse ) {
        *browse = (bool)(browseSwitch.size() > 0 );
    }
}

void MRule::makeCommand( WString& s, WFileName* target, WVList* workFiles, WString& mask, WVList* stateList, SwMode mode, bool* browse )
{
    MCommand cmd;
    for( int j=0; j<_commands.count(); j++ ) {
        MCommand* x = (MCommand*)_commands[j];
        const WString& xm = x->mask();
        if( xm.size() == 0 || xm.match( mask ) ) {
            cmd.concat( *x );
            cmd.concat( "\n" );
        }
    }
    expandCommand( s, cmd, target, workFiles, mask, stateList, mode, browse );
}

bool MRule::enumAccel( WObject *obj, bcbk fn ) {

    int         icount;
    WKeyCode    key;
    bool        rc;

    icount = _actions.count();
    rc = FALSE;
    for( int i=0; i < icount; i++ ) {
        MAction *action;
        action = (MAction *)_actions[i];
        key = action->menuAccel();
        if( key != WKeyNone ) {
            rc = (obj->*fn)( key );
            if( rc ) break;
        }
    }
    return( rc );
}
