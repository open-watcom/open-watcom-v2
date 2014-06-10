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


#include "wfilenam.hpp"
#include "wobjfile.hpp"
#include "mconfig.hpp"

Define( MCommand )

MCommand::MCommand( WTokenFile& fil, WString& tok )
{
    fil.token( *this );
    if( !fil.eol() ) {
        fil.token( _mask );
    }
    fil.token( tok );
}

MCommand::MCommand( const char* str )
    : WString( str )
{
}

MCommand& WEXPORT MCommand::operator=( const MCommand& x )
{
    *(WString*)this = (WString&)x;
    return( *this );
}

bool MCommand::expandSwitches( WString& v, WVList& list, WVList* stateList, WString* browseSwitch, SwMode mode ) const
{
    bool browse = false;
    int icount = list.count();
    for( int i=0; i<icount; i++ ) {
        MSwitch* sw = (MSwitch*)list[i];
        WString swText;
        sw->getText( swText, stateList, mode );
        if( swText.size() > 0 ) {
            if( v.size() > 0 ) {
                v.concat( ' ' );
            }
            v.concat( swText );
            if( browseSwitch && swText == *browseSwitch ) {
                browse = true;
            }
        }
    }
    return( browse );
}

int WEXPORT MCommand::expand( WString& command, WFileName* target, MTool* tool, const WString& mask, WVList* stateList, SwMode mode, WString* browseSwitch ) const
{
    bool browse = false;
    const char* cmd = gets();
    int location = EXECUTE_NORMAL;
    size_t i=0;
    if( strnicmp( &cmd[i], "!Batch ", 7 ) == 0 ) {
        i += 7;
        location = EXECUTE_BATCH;
    } else if( strnicmp( &cmd[i], "!Help ", 6 ) == 0 ) {
        i += 6;
        location = EXECUTE_HELP;
    } else if( strnicmp( &cmd[i], "!Make ", 6 ) == 0 ) {
        i += 6;
        location = EXECUTE_MAKE;
    } else if( strnicmp( &cmd[i], "!Editor ", 8 ) == 0 ) {
        i += 8;
        location = EXECUTE_EDITOR;
    } else if( strnicmp( &cmd[i], "!Browse ", 8 ) == 0 ) {
        i += 8;
        location = EXECUTE_BROWSE;
    } else if( strnicmp( &cmd[i], "!RemakeAll", 10 ) == 0 ) {
        i += 10;
        location = EXECUTE_TOUCH_ALL;
    }
    WString com;
    size_t len = strlen( cmd );
    for( ; i<len; ) {
        if( strncmp( &cmd[i], BMACRO, 2 ) == 0 ) {
            size_t l = 2;
            WString m;
            for(; i+l<len; l++ ) {
                if( i+l >= len ) {
                    m.concat( cmd[i++] );
                    break;
                } else if( cmd[i+l] == EMACRO ) {
                    l += 1;
                    WString v;
                    if( m.size() > 0 && m[0] == '%' ) {
                        m.deleteChar( 0 );
                        v.puts( getenv( m ) );
                    } else {
                        WVList list;
                        if( m[0] == '#' ) {
                            m.deleteChar( 0 );
                            MTool* t = _config->findTool( m );
                            if( !t ) {
                                t = tool;
                            }
                            t->addSwitches( list, mask, false );
                        }
                        browse = browse | expandSwitches( v, list, stateList, browseSwitch, mode );
                    }
                    com.concat( v );
                    i += l;
                    break;
//              } else if( cmd[i+l] == '#' ) {
//                  m.concat( mask );
                } else {
                    m.concat( cmd[i+l] );
                }
            }
        } else if( cmd[i] == '\r' ) {
            // skip \r
                        i++;
        } else {
            com.concat( cmd[i++ ] );
        }
    }
    if( !browse && browseSwitch ) {
        *browseSwitch = "";
    }
    if( target ) {
        size_t icount = com.size();
        for( size_t i=0; i<icount; ) {
            WFileName f;
            if( strncmp( &com[i], "$@", 2 ) == 0 ) {
                i += 2;
                if( target->needQuotes() ) {
                    target->addQuotes();
                }
                command.concat( *target );
                target->removeQuotes();
            } else if( strncmp( &com[i], "$*", 2 ) == 0 ) {
                i += 2;
                target->noExt( f );
                if( com[i] == '.' ) {
                    for( int j=1; j<=4; j++ ) {
                        if( com[i] != ' ' ) f.concat( com[i++] );
                    }
                }
                if( f.needQuotes() ) {
                    f.addQuotes();
                }
                command.concat( f );
                f.removeQuotes();
            } else if( strncmp( &com[i], "$'", 2 ) == 0 ) {
                i += 2;
                target->noExt( f );
                if( com[i] == '.' ) {
                    for( int j=1; j<=4; j++ ) {
                        if( com[i] != ' ' ) f.concat( com[i++] );
                    }
                }
                if( f.needQuotes( '\'' ) ) {
                    f.addQuotes( '\'' );
                }
                command.concat( f );
                f.removeQuotes( '\'' );
            } else if( strncmp( &com[i], "$&", 2 ) == 0 ) {
                i += 2;
                target->noPathNoExt( f );
                if( com[i] == '.' ) {
                    for( int j=1; j<=4; j++ ) {
                        if( com[i] != ' ' ) f.concat( com[i++] );
                    }
                }
                if( f.needQuotes() ) {
                    f.addQuotes();
                }
                command.concat( f );
                f.removeQuotes();
            } else if( strncmp( &com[i], "$.", 2 ) == 0 ) {
                i += 2;
                target->noPath( f );
                if( f.needQuotes() ) {
                    f.addQuotes();
                }
                command.concat( f );
                f.removeQuotes();
            } else if( strncmp( &com[i], "$:", 2 ) == 0 ) {
                i += 2;
                target->path( f );
                command.concat( f );
            } else if( strncmp( &com[i], "$!", 2 ) == 0 ) {
                i += 2;
                f = *target;
                f.relativeTo();
                if( f.needQuotes() ) {
                    f.addQuotes();
                }
                command.concat( f );
                f.removeQuotes();
            } else if( strncmp( &com[i], "$$", 2 ) == 0 ) {
                i += 2;
                command.concat( '$' );
            } else if( strncmp( &com[i], "$(", 2 ) == 0 ) {
                size_t l = i+2;
                WString m;
                for(; l<icount; l++ ) {
                    if( l >= icount ) {
                        m.concat( com[l++] );
                        break;
                    } else if( com[l] == ')' ) {
                        l += 1;
                        WString v;
                        if( m.size() > 0 && m[0] == '%' ) {
                            m.deleteChar( 0 );
                            v.puts( getenv( m ) );
                        } else {
                            //lookup *v in makeinit
                            //command.concat( "value" )
                        }
                        if( v.size() > 0 ) {
                            command.concat( v );
                            i = l;
                        } else {
                            command.concat( com[i++] );
                        }
                        break;
                    } else {
                        m.concat( com[l] );
                    }
                }
            } else {
                command.concat( com[i++] );
            }
        }
    } else {
        command.concat( com );
    }
    return( location );
}

#ifndef NOPERSIST
MCommand* WEXPORT MCommand::createSelf( WObjectFile& )
{
    return( new MCommand() );
}

void WEXPORT MCommand::readSelf( WObjectFile& p )
{
    WString::readSelf( p );
}

void WEXPORT MCommand::writeSelf( WObjectFile& p )
{
    WString::writeSelf( p );
}
#endif
