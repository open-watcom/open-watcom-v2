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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include <wmsgdlg.hpp>

#include "mem.h"
#include "util.h"
#include "browse.h"
#include "opterms.h"
#include "opscan.h"
#include "opstream.h"
#include "opgram.h"

ScannerList  Scanner::ActiveScanners;
int          Scanner::CurrentScanner = -1;
StringList   Scanner::_stringTable;
const char * SearchEnvVar = "Path";

Scanner::Scanner( char * cmdLine, int len )
            : _scanStream( new ScanStreamMem( cmdLine, len ))
            , _stutter( FALSE )
            , _lastToken( -1 )
            , _lookFor( LF_Directive )
            , _recovering( FALSE )
{
    ActiveScanners.add( this );
    CurrentScanner += 1;
}

Scanner::Scanner( const char * fileName )
            : _scanStream( new ScanStreamFile( fileName ))
            , _stutter( FALSE )
            , _lastToken( -1 )
            , _lookFor( LF_Directive )
            , _recovering( FALSE )
{
    ActiveScanners.add( this );
    CurrentScanner += 1;
}

Scanner::~Scanner( )
{
    ActiveScanners.removeAt( ActiveScanners.count() - 1 );
    CurrentScanner -= 1;

    delete _scanStream;
}

int Scanner::getToken( YYSTYPE & lval )
/*************************************/
{
    int         c;
    int         count;
    int         fnd;
    bool        flush = FALSE;
    bool        inquoted = FALSE;
    bool        escaped = FALSE;

    if( _stutter ) {
        _stutter = FALSE;
        return _lastToken;
    }

    c = _scanStream->get();
    for(;;) {
        if( isspace( c ) ) {
            c = _scanStream->get();
        } else if( strchr( StartComments, c ) ) {
            do {
                c = _scanStream->get();
            } while( c != EOF && !strchr( EndComments, c ) );
            c = _scanStream->get();
        } else {
            break;
        }
    }
    if( c == '"' ) {
        inquoted = TRUE;
        c = _scanStream->get();         // skip the quote
    }
    for( count = 0; ; count += 1 ) {

        if(( c == '\\' ) && ( _lookFor == LF_MagicString )) {
            c = _scanStream->get();
            if( isspace( c )) {
                _scanStream->unget( c );
                c = '\\';
            } else {
                escaped = TRUE;
            }
        }

        if( c == EOF ) {
            if( count == 0 ) {
                if( CurrentScanner > 0 ) {
                    if( _lastToken == T_FileSync || _lastToken == EOF ) {
                        delete this;
                        return yylex();
                    } else {
                        _lastToken = T_FileSync;
                        return T_FileSync;
                    }
                } else {
                    return 0;       // done
                }
            } else {
                flush = TRUE;
            }
        } else if( inquoted && !escaped ) {
            if( c == '"' ) {
                c = _scanStream->get();         // skip the quote
                flush = TRUE;
            }
        } else if( isspace( c ) || strchr( StartComments, c ) ) {
            flush = TRUE;
        } else if( strchr( Special, c ) && !escaped ) {
            if( count == 0 ) {
                _lastToken = c;
                return c;
            } else {
                flush = TRUE;
            }
        }

        if( count >= BufSize - 1 ) {
            flush = TRUE;
        }

        if( flush ) {
            _scanStream->unget( c );
            _buffer[ count ] = '\0';

            if(( _lookFor != LF_String ) && ( _lookFor != LF_MagicString )) {
                fnd = tryToken( _lookFor, _buffer );

                if( fnd > 0 ) {
                    lval = fnd;
                    return fnd;
                } else {
                    if( _recovering ) {
                        flush = FALSE;
                        count = -1;
                    } else {
                        WString msg;
                        char * ambigs;
                        ambigs = tryAmbig( _lookFor, _buffer );

                        if( ambigs != NULL ) {
                            msg.printf( "[%s] is ambiguous -- try %s --",
                                        _buffer, ambigs );
                            WBRFree( ambigs );
                        } else {
                            msg.printf( "Unrecognized symbol [%s]", _buffer );
                        }
                        if( error( msg ) ) {
                            return 0;
                        }

                        _recovering = TRUE;
                        flush = FALSE;
                        count = -1;
                    }
                }
            } else {
                _stringTable.add( strdup( _buffer ));
                lval = _stringTable.count() - 1;
                if( _lookFor == LF_String ) {
                    _lastToken = T_String;
                } else {
                    _lastToken = T_MagicString;
                }
                return _lastToken;
            }
        } else {
            _buffer[ count ] = (char) c;
        }
        escaped = FALSE;
        c = _scanStream->get();
    }
}

void Scanner::recoverTo( LookForType lf )
/***************************************/
{
    YYSTYPE val;

    _lookFor = lf;
    _recovering = TRUE;

    if( _stutter &&
        _lastToken > 0 &&
        ! (_lastToken == T_String && lf != LF_String )
       ) return;

    while( 1 ) {
        int tok = getToken( val );
        if( tok > 0 && ! (tok == T_String && lf != LF_String ) ) break;
    }

    _recovering = FALSE;
    stutterOn();
}

void Scanner::stutterOn( void )
/*****************************/
{
    _stutter = TRUE;
}

void Scanner::setLookFor( LookForType lf )
/****************************************/
{
    _lookFor = lf;
}

char * Scanner::getString( int index )
/************************************/
{
    assert( index < _stringTable.count() );

    return _stringTable[ index ];
}

static Scanner * Scanner::CurrScanner( void )
/*******************************************/
{
    assert( CurrentScanner < ActiveScanners.count() );

    return ActiveScanners[ CurrentScanner ];
}

static bool Scanner::NewFileScanner( char * fname )
/*************************************************/
{
    Scanner * scn = new Scanner( fname );

    if( scn->_scanStream->error() ) {
        delete scn;
        return TRUE;
    }

    return FALSE;
}

void Scanner::scanInit( void )
/****************************/
{
    char * path = getenv( SearchEnvVar );
    ScanStreamBase::streamInit( path );

    terminalSetup();
}

bool Scanner::error( const char * err )
/*************************************/
{
    WString msg;
    WString title;
    char *  name = _scanStream->name();
    msgRetType ret;

    msg.printf( "%s near '%s'\n\nContinue?", err, _buffer );
    title.printf( "Syntax error in %s", name );

    ret = WMessageDialog::message( browseTop, MsgError, MsgOkCancel,
                                    msg, title );

    WBRFree( name );

    if( ret == MsgRetCancel ) {
        return TRUE;
    } else {
        return FALSE;
    }
}

///////////////////////// Scanner calls ///////////////////////

void init( void )
/***************/
{
    Scanner::scanInit();
}

char * getString( int index )
/***************************/
{
    return Scanner::CurrScanner()->getString( index );
}

bool yyerror( char const * err )
/*****************************/
{
    return Scanner::CurrScanner()->error( err );
}

int yylex()
/*********/
{
    int     token;
    YYSTYPE val;

    token = Scanner::CurrScanner()->getToken( val );
    yylval = val;
    return token;
}

void setLookFor( LookForType lf )
/*******************************/
{
    Scanner::CurrScanner()->setLookFor( lf );
}

void stutterOn( void )
/********************/
{
    Scanner::CurrScanner()->stutterOn();
}

void recoverTo( LookForType lf )
/******************************/
{
    Scanner::CurrScanner()->recoverTo( lf );
}

bool includeFile( char * fname )
/******************************/
{
    return Scanner::NewFileScanner( fname );
}
