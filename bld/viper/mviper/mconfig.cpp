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


#include "mconfig.hpp"
#include "mtool.hpp"
#include "mrule.hpp"
#include "mtarget.hpp"
#include "mtoolitm.hpp"
#include "maction.hpp"
#include "msymbol.hpp"
#include "wtokfile.hpp"
#include "wobjfile.hpp"
#include "mtypo.hpp"

#include <mbctype.h>

extern "C" {
    #include <malloc.h>
    #if defined( __WINDOWS__ ) || defined( __NT__ )
        #include <windows.h>    //temporary ?
    #endif
};

#define MALLOC(s) (char*)malloc(s)
#define REALLOC(p,s) (char*)realloc(p,s)
#define FREE(p) if( p != NULL ) free(p)

#define CUR_CFG_VERSION         4

Define( MConfig )

MConfig* MConfig::_configPtr;

MConfig::MConfig( WFileName& filename, bool debug, HostType host )
    : _filename( filename )
    , _fileFilters( NULL )
    , _fileFilterSize( 0 )
    , _ok( FALSE )
    , _dirty( FALSE )
    , _debug( debug )
    , _version( 0 )
    , _kludge( 0 )
    , _hostType( HOST_UNDEFINED )
{
    struct {
        char    *batserv;
        char    *editor;
        bool    editorIsDLL;
        char    *editorParms;
    } host_info[] = {
        #undef pick
        #define pick(enum,type,batchserv,editor,DLL,parms,descr) { batchserv, editor, DLL, parms },
        #include "hosttype.h"
    };

    _configPtr = this;
    if( host < 0 || host >= HOST_UNDEFINED ) {
#ifdef __WINDOWS__
        _hostType = HOST_WINDOWS;
        if( __IsDBCS ) {
            /* japanese windows on an IBM */
            _hostType = HOST_J_WIN;
        } else {
            /* assume no DBCS win-os/2 */
            union {
                DWORD dVersion;
                struct {
                    char winMajor;
                    char winMinor;
                    char dosMinor;
                    char dosMajor;
                } v;
            } v;
            v.dVersion = GetVersion();
            if( v.v.dosMajor >= 20 ) {
                _hostType = HOST_WINOS2;
            }
        }
#elif defined( __NT__ )
    /* -------------------------------------------------------- */
    #ifdef __AXP__
        _hostType = HOST_NT_AXP;
    #else
        DWORD ver;
        bool is_chicago = FALSE;

        _hostType = HOST_NT;
        ver = GetVersion();
        if( !( ver < 0x80000000 ) && !( LOBYTE( LOWORD( ver ) ) < 4 ) ) {
            _hostType = HOST_WIN95;
        }
    #endif
    /* -------------------------------------------------------- */
#elif defined( __OS2__ )
        _hostType = HOST_PM;
#elif defined( __DOS__ )
        _hostType = HOST_DOS;
#elif defined( __LINUX__ )
        _hostType = HOST_LINUX;
#endif
    } else {
        _hostType = host;
    }

    if( _hostType == HOST_UNDEFINED ) {
        return;
    }
    _batserv = host_info[ _hostType ].batserv;
    // set editor to default values
    _editor = host_info[ _hostType ].editor;
    _editorIsDLL = host_info[ _hostType ].editorIsDLL;
    _editorParms = host_info[ _hostType ].editorParms;

    if( _filename.size() == 0 ) {
        _filename = "ide.cfg";
    }
    readConfig();
}

void MConfig::writeConfig()
{
    _filename.setExt( ".cfc" );
    WObjectFile of( 9 );
    if( of.open( _filename, OStyleWrite ) ) {
        of.writeObject( this );
        of.close();
    }
}

bool MConfig::readConfig()
{
    _nilTool = new MTool( "NilTool", "NIL" );
    _tools.add( _nilTool );
    _nilRule = new MRule( "NIL", _nilTool );
    _rules.add( _nilRule );
    _ok = TRUE;
    if( readFile( _filename ) ) {
        zapTargetMasks();
        return TRUE;
    }
    return FALSE;
}

void MConfig::zapTargetMasks()
{
    static char hostChars[] = {
        #undef pick
        #define pick(enum,type,batchserv,editor,DLL,parms,descr) type,
        #include "hosttype.h"
    };
    int     i;

    for( i=0; i<_hostMask.size(); i++ ) {
        if( _hostMask[i] == '@' ) {
            _hostMask.setChar( i, hostChars[ _hostType ] );
        }
    }
    for( i=0; i<_targets.count(); i++ ) {
        MTarget* tar = (MTarget*)_targets[i];
        zapMask( tar->mask() );
    }
}

bool MConfig::readFile( const WFileName& filename, bool reqd )
{
    WTokenFile fil;
    if( !fil.open( filename, "PATH" ) ) {
        if( reqd ) {
            _ok = FALSE;
            _errMsg.printf( "Unable to open '%s'", (const char*)filename );
        }
    } else {
        _filename = fil.filename();
        WString tok;
        fil.token( tok );
        for(; !fil.eof(); ) {
            if( tok == "Tool" ) {
                _tools.add( new MTool( fil, tok ) );
            } else if( tok == "Rule" ) {
                _rules.add( new MRule( fil, tok ) );
            } else if( tok == "Project" ) {
                configProject( fil, tok );
            } else if( tok == "MsgLog" ) {
                configMsgLog( fil, tok );
            } else if( tok == "IncludeFile" ) {
                WFileName fn;
                fil.token( fn );
                if( !readFile( fn, FALSE ) ) break;
                fil.token( tok );
            } else if( tok == "HostMask" ) {
                fil.token( _hostMask );
                fil.token( tok );
            } else if( tok == "Version" ) {
                _version = (int)fil.token( tok );
                if( _version > CUR_CFG_VERSION ) {
                    _ok = FALSE;
                    _errMsg.printf( "Configuration file '%s' format is too new.  "
                                    "you must use a newer version of the IDE.",
                                    (const char*)filename );
                    break;
                }
                fil.token( tok );
            } else if( tok == "rem" ) {
                fil.flushLine( tok );
                fil.token( tok );
            } else if( tok == "Compat" ) {
                WString good;
                WString bad;
                fil.token( good );
                fil.token( bad );
                AddTypo( good, bad );
                fil.token( tok );
            } else {
                _ok = FALSE;
                _errMsg.printf( "Error in '%s', line %d, at '%s'", (const char*)filename, fil.lineCount(), (const char*)tok );
                break;
            }
        }
        fil.close();
    }
    buildTargetOSList();
    return _ok;
}

MConfig::~MConfig()
{
    _tools.deleteContents();
    _rules.deleteContents();
    _targets.deleteContents();
    _toolItems.deleteContents();
    _actions.deleteContents();
    _helpactions.deleteContents();
    _logScanPatterns.deleteContents();
    _logHelpFiles.deleteContents();
    _logHtmlHelpFiles.deleteContents();
    _targetOSs.deleteContents();
    FREE( _fileFilters );
}

#ifndef NOPERSIST
MConfig* WEXPORT MConfig::createSelf( WObjectFile& )
{
    return NULL;        //should never be called!
}

void WEXPORT MConfig::readSelf( WObjectFile& p )
{
    WObject::readSelf( p );
    p.readObject( &_tools );
    p.readObject( &_rules );
    p.readObject( &_actions );
    p.readObject( &_editor );
    p.readObject( &_browse );
    p.readObject( &_browseMerge );
}

void WEXPORT MConfig::writeSelf( WObjectFile& p )
{
    WObject::writeSelf( p );
    p.writeObject( &_tools );
    p.writeObject( &_rules );
    p.writeObject( &_actions );
    p.writeObject( &_editor );
    p.writeObject( &_browse );
    p.writeObject( &_browseMerge );
}
#endif

void MConfig::configMsgLog( WTokenFile& fil, WString& tok )
{
    fil.token( tok );
    for(;;) {
        if( tok == "Scan" ) {
            _logScanPatterns.add( new WString( fil.token( tok ) ) );
            fil.token( tok );
        } else if( tok == "BatchServer" ) {
            fil.token( _batserv );
            fil.token( tok );
        } else if( tok == "Help" ) {
            _logHelpFiles.add( new WString( fil.token( tok ) ) );
            _logHelpFiles.add( new WString( fil.token( tok ) ) );
            if( !fil.eol() ) {
                _logHelpFiles.add( new WString( fil.token( tok ) ) );
            } else {
                _logHelpFiles.add( new WString( "0" ) );
            }
            fil.token( tok );
        } else if( tok == "HtmlHelp" ) {
            _logHtmlHelpFiles.add( new WString( fil.token( tok ) ) );
            _logHtmlHelpFiles.add( new WString( fil.token( tok ) ) );
            if( !fil.eol() ) {
                _logHtmlHelpFiles.add( new WString( fil.token( tok ) ) );
            } else {
                _logHtmlHelpFiles.add( new WString( "0" ) );
            }
            fil.token( tok );
        } else if( tok == "rem" ) {
            fil.flushLine( tok );
            fil.token( tok );
        } else {
            break;
        }
    }
}

void MConfig::expandMacroes( WString &str ) {
    WString     tok( str );
    WString     envvar;
    unsigned    i;

    str = "";
    for( i=0; tok[i] != '\0'; i++ ) {
        if( tok[i] == '$' && tok[i+1] == '(' && tok[i+2] == '%' ) {
            envvar = "";
            i += 3;
            for( ;; i++ ) {
                if( tok[i] == '\0' ) {
                    str.concat( "$%(" );
                    str.concat( envvar );
                    str.concat( tok[i] );
                    i--; // don't let the outside loop go past the NULLCHAR
                    break;
                } else if( tok[i] == ')' ) {
                    str.concat( getenv( envvar ) );
                    break;
                } else {
                    envvar.concat( tok[i] );
                }
            }
        } else {
            str.concat( tok[i] );
        }
    }
}

void MConfig::configProject( WTokenFile& fil, WString& tok )
{
    WString     target_os( "" );

    fil.token( tok );
    for(;;) {
        if( tok == "Editor" ) {
            // just ignore this and set things to the defaults
            // read all the stuff
            fil.flushLine( tok );
            fil.token( tok );
        } else if( tok == "Browse" ) {
            fil.token( _browseMerge );
            expandMacroes( _browseMerge );
            fil.token( _browse );
            expandMacroes( _browse );
            fil.token( tok );
        } else if( tok == "Help" ) {
            fil.token( _helpFile );
            fil.token( tok );
        } else if( tok == "HtmlHelp" ) {
            fil.token( _htmlHelpFile );
            fil.token( tok );
        } else if( tok == "TargetGroup" ) {
            fil.token( target_os );
            fil.token( tok );
        } else if( tok == "Target" ) {
            _targets.add( new MTarget( fil, tok, target_os ) );
        } else if( tok == "ToolItem" ) {
            _toolItems.add( new MToolItem( fil, tok ) );
        } else if( tok == "Action" ) {
            _actions.add( new MAction( fil, tok ) );
        } else if( tok == "HelpAction" ) {
            _helpactions.add( new MAction( fil, tok ) );
        } else if( tok == "Before" ) {
            fil.token( tok );
            while( tok == "Command" ) {
                _before.concat( fil.token( tok ) );
                _before.concat( "\n" );
                fil.token( tok );
            }
        } else if( tok == "After" ) {
            fil.token( tok );
            while( tok == "Command" ) {
                _after.concat( fil.token( tok ) );
                _after.concat( "\n" );
            }
        } else if( tok == "Filter" ) {
            fil.token( tok );
            int size = _fileFilterSize;
            for( int i=0; i<2; i++ ) {
                _fileFilters = REALLOC( _fileFilters, size + (tok.size() + 1) + 1 );
                if( _fileFilters ) {
                    strcpy( &_fileFilters[ size ], tok );
                    size += tok.size() + 1;
                    _fileFilters[ size ] = '\0';
                }
                fil.token( tok );
            }
            _fileFilterSize = size;
        } else if( tok == "rem" ) {
            fil.flushLine( tok );
            fil.token( tok );
        } else {
            break;
        }
    }
}

MTool* MConfig::findTool( WString& tooltag )
{
    int icount = _tools.count();
    for( int i=0; i<icount; i++ ) {
        if( tooltag == ((MTool*)_tools[i])->tag() ) {
            return (MTool*)_tools[i];
        }
    }
    return NULL;
}

void MConfig::addRules( WFileName& srcMask, WFileName& tgtMask, WVList& list, WString& tagMask )
{
    int icount = _rules.count();
    for( int i=0; i<icount; i++ ) {
        MRule* rule = (MRule*)_rules[i];
        if( rule->match( tagMask, srcMask, tgtMask ) ) {
            list.add( rule );
        }
    }
}

MRule* MConfig::findRule( WString& ruletag )
{
    int icount = _rules.count();
    for( int i=0; i<icount; i++ ) {
        MRule* rule = (MRule*)_rules[i];
        if( ruletag == rule->tag() ) {
            return rule;
        }
    }
    return NULL;
}

MRule* MConfig::findMatchingRule( WFileName& fn, MRule* tgtRule, WString& mask )
{
    WVList list;
    WVList& symbols = tgtRule->symbols();
    int icount = symbols.count();
    for( int i=0; i<icount; i++ ) {
        MSymbol& sym = *(MSymbol*)symbols[i];
        addRules( fn, sym.mask(), list, mask );
        if( list.count() > 0 ) {
            return (MRule*)list[0];
        }
    }
    return nilRule();
}

int MConfig::findMatchingRules( WFileName& fn, WString& mask, WVList& list )
{
    int icount = _rules.count();
    for( int i=0; i<icount; i++ ) {
        MRule* rule = (MRule*)_rules[i];
        if( rule->match( mask, fn ) ) {
            list.add( rule );
        }
    }
    return list.count();
}

MRule* MConfig::findMatchingRule( WFileName& fn, WString& mask )
{
    int icount = _rules.count();
    for( int i=0; i<icount; i++ ) {
        MRule* rule = (MRule*)_rules[i];
        if( rule->match( mask, fn ) ) {
            return rule;
        }
    }
    return nilRule();
}

void MConfig::zapMask( WString& mask )
{
    for( int i=0; i<_hostMask.size(); i++ ) {
        if( _hostMask[i] != '?' ) {
            mask.setChar( i, _hostMask[i] );
        }
    }
}

void MConfig::kludgeString( WString& str )
{
    if( _kludge ) {
        WString temp;
        for( int i=0; i<str.size(); i++ ) {
            temp.concat( str[ i ] );
            if( _kludge == 3 && i == 0 ) temp.concat( '?' );
            if( _version > 1 && _kludge == 3 && i == 2 ) temp.concat( '?' );
            if( _version > 1 && _kludge == 4 && i == 3 ) temp.concat( '?' );
        }
        str = temp;
    }
}

void MConfig::buildTargetOSList() {

    int         icount;
    MTarget     *target;
    WString     *os;

    icount = _targets.count();
    os = new WString;
    for( int i=0; i < icount; i++ ) {
        target = (MTarget *)_targets[i];
        *os = target->targetOS();
        if( _targetOSs.find( (WObject *)os ) == NULL ) {
            _targetOSs.add( (WObject *)os );
            os = new WString;
        }
    }
    delete os;
}

void MConfig::enumAccel( WObject *obj, bcbk fn )
{
    int         icount;
    WKeyCode    key;
    int         i;

    icount = _actions.count();
    for( i=0; i < icount; i++ ) {
        MAction *action;
        action = (MAction *)_actions[i];
        key = action->menuAccel();
        if( key != WKeyNone ) {
            if( (obj->*fn)( key ) ) return;
        }
    }
    icount = _rules.count();
    for( i=0; i < icount; i++ ) {
        MRule   *rule;
        rule = (MRule *)_rules[i];
        if( rule->enumAccel( obj, fn ) ) return;
    }
}
