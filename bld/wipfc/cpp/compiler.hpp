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
    //set the inital source file name
    void setInputFile( std::string& name );
    //add a file name to the set of file names
    std::wstring* addFileName( std::wstring* name );
    //set the output file name
    void setOutputFile( std::string& name ) { outFileName = name; };
    //set the warning level
    void setWarningLevel( int wl ) { warningLevel = wl; };
    //set the current locale
    void setLocalization( const char *ln ) { loc = ln; };
    //set the output type (inf or hlp)
    void setOutputType( OutputType t ) { outType = t; };
    OutputType outputType( ) { return outType; };
    //suppress banner
    void noBanner() { printBanner = false; };
    bool banner() { return printBanner; };
    //Output cross-reference (not implemented)
    void noSearch() { search = false; };
    bool searchable() { return search; };
    void setXRef( bool yn ) { xref = yn; };
    //push a block of data onto the stack to be parsed
    void pushInput( IpfData *blk ) { inFiles.push_back( blk ); };
    void popInput();
    //print an error message
    void printError( ErrCode c ) const;
    void printError( ErrCode c, const std::wstring& txt ) const;
    void printError( ErrCode c, const std::wstring* name, unsigned int row, unsigned int col ) const;
    void printError( ErrCode c, const std::wstring* name, unsigned int row, unsigned int col, const std::wstring& txt ) const;
    //get the name of the data block currently being parsed
    const std::wstring* dataName() const { return inFiles[ inFiles.size() - 1 ]->name(); };
    //get the number of the line currently being parsed
    unsigned int dataLine() const { return inFiles[ inFiles.size() - 1 ]->currentLine(); };
    unsigned int lexerLine() const { return lexer->currentLine(); };
    //get the column in the line currently being parsed
    unsigned int dataCol() const { return inFiles[ inFiles.size() - 1 ]->currentCol(); };
    unsigned int lexerCol() const{ return lexer->currentCol(); };
    //get the next token
    Lexer::Token getNextToken();
    //if end of block or file reached, don't go to the next block
    void setBlockParsing( bool yn ) { parseContinuously = !yn; };
    bool blockParsing() { return !parseContinuously; };
private:
    Compiler( const Compiler &rhs );            //no copy constructor
    Compiler& operator=( const Compiler &rhs ); //no assignment
    std::string outFileName;
    std::auto_ptr< Lexer > lexer;
    std::vector< IpfData* > inFiles;            //a stack of files being parsed
    typedef std::vector< IpfData* >::iterator InFilesIter;
    typedef std::vector< IpfData* >::const_iterator ConstInFilesIter;
    std::set< std::wstring*, ptrLess< std::wstring* > > fileNames;
    typedef std::set< std::wstring*, ptrLess< std::wstring* > >::iterator FileNameIter;
    typedef std::set< std::wstring*, ptrLess< std::wstring* > >::const_iterator ConstFileNameIter;
    const char* loc;
    unsigned int warningLevel;
    OutputType outType;
    bool parseContinuously;
    bool printBanner;
    bool search;    //construct search table
    bool xref;
};

#endif //COMPILER_INCLUDED
