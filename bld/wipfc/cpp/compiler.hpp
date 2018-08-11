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
* Description:  the OS/2 help compiler.
* It's major jobs are to:
*   1. manage the input file(s) being parsed
*   2. print error messages
*   3. serve as the home for the lexer
*
****************************************************************************/

#ifndef COMPILER_INCLUDED
#define COMPILER_INCLUDED

#include <cstdio>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "errors.hpp"
#include "ipffile.hpp"
#include "lexer.hpp"
#include "ptrops.hpp"


class Compiler {
public:
    enum OutputType {
        INF,
        HLP
    };
    Compiler();
    ~Compiler();
    int compile();
    //set the input source file name
    void setInputFile( const std::string& sfname );
    //add a file name to the set of file names
    std::wstring* addFileName( std::wstring* wfname );
    //set the output file name
    void setOutputFile( const std::string& sfname );
    //set the warning level
    void setWarningLevel( int wl ) { _warningLevel = wl; };
    //set the current locale
    void setLocalization( const char *loc ) { _loc = loc; };
    //set the output type (inf or hlp)
    void setOutputType( OutputType t ) { _outType = t; };
    OutputType outputType( ) { return _outType; };
    //suppress banner
    void noBanner() { _printBanner = false; };
    bool banner() { return _printBanner; };
    //Output cross-reference (not implemented)
    void noSearch() { _search = false; };
    bool searchable() { return _search; };
    void setXRef( bool yn ) { _xref = yn; };
    //push a block of data onto the stack to be parsed
    void pushInput( IpfData *blk ) { _inFiles.push_back( blk ); };
    void popInput();
    //print an error message
    void printError( ErrCode c ) const;
    void printError( ErrCode c, const std::wstring& text ) const;
    void printError( ErrCode c, const std::wstring* name, unsigned int row, unsigned int col ) const;
    void printError( ErrCode c, const std::wstring* name, unsigned int row, unsigned int col, const std::wstring& text ) const;
    //get the name of the data block currently being parsed
    const std::wstring* dataName() const { return _inFiles[_inFiles.size() - 1]->name(); };
    //get the number of the line currently being parsed
    unsigned int dataLine() const { return _inFiles[_inFiles.size() - 1]->currentLine(); };
    unsigned int lexerLine() const { return _lexer->currentLine(); };
    //get the column in the line currently being parsed
    unsigned int dataCol() const { return _inFiles[_inFiles.size() - 1]->currentCol(); };
    unsigned int lexerCol() const{ return _lexer->currentCol(); };
    //get the next token
    Lexer::Token getNextToken();
    //if end of block or file reached, don't go to the next block
    void setBlockParsing( bool yn ) { _parseContinuously = !yn; };
    bool blockParsing() { return !_parseContinuously; };
private:
    Compiler( const Compiler &rhs );            //no copy constructor
    Compiler& operator=( const Compiler &rhs ); //no assignment

    std::auto_ptr< Lexer >  _lexer;
    std::vector< IpfData* > _inFiles;            //a stack of files being parsed
    typedef std::vector< IpfData* >::iterator InFilesIter;
    typedef std::vector< IpfData* >::const_iterator ConstInFilesIter;
    std::set< std::wstring*, ptrLess< std::wstring* > > _fileNames;
    typedef std::set< std::wstring*, ptrLess< std::wstring* > >::iterator FileNameIter;
    typedef std::set< std::wstring*, ptrLess< std::wstring* > >::const_iterator ConstFileNameIter;
    const char              *_loc;
    unsigned int            _warningLevel;
    OutputType              _outType;
    bool                    _parseContinuously;
    bool                    _printBanner;
    bool                    _search;    //construct search table
    bool                    _xref;
    std::wstring            *_inFileNameW;
    std::string             _inFileName;
    std::string             _outFileName;
};

#endif //COMPILER_INCLUDED
