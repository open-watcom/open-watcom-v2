/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2009-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  The OS/2 help compiler.
*
****************************************************************************/


#include "wipfc.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include "compiler.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "ipfbuff.hpp"
#include "ipffile.hpp"
#include "lexer.hpp"
#include "util.hpp"
#include "outfile.hpp"


Compiler::Compiler():
    _lexer( new Lexer() ),
    _loc( "en_US" ),
    _warningLevel( 3 ),
    _outType( HLP ),
    _parseContinuously( true ),
    _printBanner( true ),
    _search( true ),
    _xref( false ),
    _inFileNameW( NULL )
{
}
/*****************************************************************************/
Compiler::~Compiler()
{
    while( _inFiles.size() )
        popInput();
    for( FileNameIter iter = _fileNames.begin(); iter != _fileNames.end(); ++iter ) {
        delete *iter;
    }
}
/*****************************************************************************/
void Compiler::setInputFile( const std::string& sfname )
{
    _inFileNameW = new std::wstring();
    _inFileName = sfname;
    def_mbtow_string( _inFileName, *_inFileNameW );
}
/*****************************************************************************/
std::wstring* Compiler::addFileName( std::wstring* name )
{
    std::pair< FileNameIter, bool > status( _fileNames.insert( name ) );
    if( !status.second )
        delete name;
    return *status.first;
}
void Compiler::setOutputFile( const std::string& sfname )
{
    _outFileName = sfname;
}
/*****************************************************************************/
int Compiler::compile()
{
    int retval( EXIT_SUCCESS );
    // init document and set locale for input/output data
    std::auto_ptr< Document > doc( new Document( *this, _outType, _loc ) );
    _inFileNameW = doc->pushFileInput( _inFileName, _inFileNameW );
    doc->setOutFile( _outFileName );
    doc->parse( _lexer.get() );
    doc->build();
    try {
        doc->write();
    }
    catch( FatalError& e ) {
        retval = EXIT_FAILURE;
        printError( e._code );
    }
    catch( FatalIOError& e ) {
        retval = EXIT_FAILURE;
        printError( e._code, e._fname );
    }
    if( _xref ) {
        //TODO: convert to ostream when streams and strings mature
        std::string logfname( _outFileName );
        logfname.erase( logfname.rfind( '.' ) );
        logfname += ".log";
        std::FILE *logfp = std::fopen( logfname.c_str(), "w" );
        if( !logfp )
            throw FatalIOError( ERR_OPEN, L"for log output" );
        try {
            std::fprintf( logfp, "Summary for %s\n\n", _outFileName.c_str() );
            doc->summary( logfp );
        } catch( FatalError& e ) {
            retval = EXIT_FAILURE;
            printError( e._code );
        } catch( FatalIOError& e ) {
            retval = EXIT_FAILURE;
            printError( e._code, e._fname );
        }
        std::fclose( logfp );
    }
    return retval;
}

/*****************************************************************************/
static unsigned levelCode( ErrCode c )
{
    if( c > ERR2_LAST )
        return c - ERR2_LAST + 300;
    if( c > ERR1_LAST )
        return c - ERR1_LAST + 200;
    if( c > ERR_LAST )
        return c - ERR_LAST + 100;
    return c;
}

//Error message format is <fullfilename:line:col> errnum: text [optional info]
void Compiler::printError( ErrCode c ) const
{
    if( c <= ERR_LAST || _warningLevel > 2 ||
       ( c <= ERR1_LAST && _warningLevel > 0 ) ||
       ( c <= ERR2_LAST && _warningLevel > 1 )) {
/*
           std::cout << '<';
           std::cout << dataName();
           std::cout << ':';
           std::cout << lexerLine();
           std::cout << ':';
           std::cout << lexerCol();
           std::cout << "> " << ( c > ERR_LAST ? "Warning" : "Fatal Error" );
           std::cout << std::setw( 2 ) << std::setfill( '0' );
           std::cout << static_cast< unsigned int >( c );
           std::cout << ': ' << ErrText[c] << std::endl;
*/
        std::fprintf( stdout, "<%ls:%u:%u> %s %02u: %s\n",
            _inFiles.size() ? dataName()->c_str() : L"(no current file)",
            lexerLine(), lexerCol(),
            c > ERR_LAST ? "Warning" : "Fatal Error",
            levelCode( c ),
            ErrText[c] );
    }
}
/*****************************************************************************/
//Error message format is <fullfilename:line:col> errnum: text [optional info]
void Compiler::printError( ErrCode c, const std::wstring& text ) const
{
    if( c <= ERR_LAST || _warningLevel > 2 ||
       ( c <= ERR1_LAST && _warningLevel > 0 ) ||
       ( c <= ERR2_LAST && _warningLevel > 1 )) {
/*
           std::cout << '<';
           std::cout << dataName();
           std::cout << ':';
           std::cout << lexerLine();
           std::cout << ':';
           std::cout << lexerCol();
           std::cout << "> " << ( c > ERR_LAST ? "Warning" : "Fatal Error" );
           std::cout << std::setw( 2 ) << std::setfill( '0' );
           std::cout << static_cast< unsigned int >( c );
           std::cout << ': ' << ErrText[c] << text << std::endl;
*/
        std::fprintf( stdout, "<%ls:%u:%u> %s %02u: %s %ls\n",
            _inFiles.size() ? dataName()->c_str() : L"(no current file)",
            lexerLine(), lexerCol(),
            c > ERR_LAST ? "Warning" : "Fatal Error",
            levelCode( c ),
            ErrText[c], text.c_str() );
    }
}
/*****************************************************************************/
//Error message format is <fullfilename:line:col> errnum: text [optional info]
void Compiler::printError( ErrCode c, const std::wstring* name, unsigned int row,
    unsigned int col ) const
{
    if( c <= ERR_LAST || _warningLevel > 2 ||
       ( c <= ERR1_LAST && _warningLevel > 0 ) ||
       ( c <= ERR2_LAST && _warningLevel > 1 )) {
/*
           std::cout << '<';
           std::cout << _inFiles[_inFiles.size() - 1]->name();
           std::cout << ':';
           std::cout << row;
           std::cout << ':';
           std::cout << col;
           std::cout << "> " ( c > ERR_LAST ? "Warning" : "Fatal Error" );
           std::cout << std::setw( 2 ) << std::setfill( '0' );
           std::cout << static_cast< unsigned int >( c );
           std::cout << ' ' << ErrText[c] << std::endl;
*/
        std::fprintf( stdout, "<%ls:%u:%u> %s %02u: %s\n",
            name->c_str(), row, col,
            c > ERR_LAST ? "Warning" : "Fatal Error",
            levelCode( c ),
            ErrText[c] );
    }
}
/*****************************************************************************/
//Error message format is <fullfilename:line:col> errnum: text [optional info]
void Compiler::printError( ErrCode c, const std::wstring* name, unsigned int row,
    unsigned int col, const std::wstring& text ) const
{
    if( c <= ERR_LAST || _warningLevel > 2 ||
       ( c <= ERR1_LAST && _warningLevel > 0 ) ||
       ( c <= ERR2_LAST && _warningLevel > 1 )) {
/*
           std::cout << '<';
           std::cout << _inFiles[_inFiles.size() - 1]->name();
           std::cout << ':';
           std::cout << row;
           std::cout << ':';
           std::cout << col;
           std::cout << "> " ( c > ERR_LAST ? "Warning" : "Fatal Error" );
           std::cout << std::setw( 2 ) << std::setfill( '0' );
           std::cout << static_cast< unsigned int >( c );
           std::cout << ' ' << ErrText[c] << text << std::endl;
*/
        std::fprintf( stdout, "<%ls:%u:%u> %s %02u: %s %ls\n",
            name->c_str(), row, col,
            c > ERR_LAST ? "Warning" : "Fatal Error",
            levelCode( c ),
            ErrText[c], text.c_str() );
    }
}
/*****************************************************************************/
Lexer::Token Compiler::getNextToken()
{
    Lexer::Token tok( _lexer->lex( _inFiles[_inFiles.size() - 1] ));
    if( _parseContinuously ) {
        while( tok == Lexer::END ) {
            popInput();
            if( _inFiles.empty() )
                break;
            tok = _lexer->lex( _inFiles[_inFiles.size() - 1] );
        }
    }
    return tok;
}
/*****************************************************************************/
void Compiler::popInput( )
{
    IpfData* f( _inFiles[_inFiles.size() - 1] );
    _inFiles.pop_back();
    delete f;
}
