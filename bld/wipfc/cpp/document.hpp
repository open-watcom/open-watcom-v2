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
* Description:  the document
*
****************************************************************************/

#ifndef DOCUMENT_INCLUDED
#define DOCUMENT_INCLUDED

#include <cstdio>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "compiler.hpp"
#include "controls.hpp"
#include "errors.hpp"
#include "extfiles.hpp"
#include "fntcol.hpp"
#include "gdict.hpp"
#include "gdword.hpp"
#include "gnames.hpp"
#include "header.hpp"
#include "lexer.hpp"
#include "nls.hpp"
#include "strings.hpp"
#include "tocref.hpp"
#include "title.hpp"


class Cell; //forward reference
class Page;
class Tag;
class I1;
class ICmd;
class Synonym;
class Text;
class OutFile;

class Document {
public:
    enum DocumentType {
        INF = 0x01,
        HLP = 0x10
    };

    Document( Compiler& c, Compiler::OutputType t, const char * loc );
    ~Document();
    void parse( Lexer* lexer );
    void build();
    void write();
    void summary( std::FILE* logfp );

    //set the output file
    void setOutFile( const std::string& fileName );
    OutFile* out() { return _out; };
    bool isInf() const { return _hdr->flags == INF; };
    //set the lowest header level for which new pages are made
    void setHeaderCutOff( unsigned int co ) { _maxHeaderLevel = co; };
    unsigned int headerCutOff() const { return _maxHeaderLevel; };
    //track the current left margin
    void setLeftMargin( byte lm ) { _currentLeftMargin = lm; } ;
    byte leftMargin() const { return _currentLeftMargin; };
    //store a graphics file name
    void addBitmap( std::wstring& bmn );
    dword bitmapByName( std::wstring& bmn );
    //toggle automatic insertion of spaces (for parsing)
    void toggleAutoSpacing() { _spacing = !_spacing; };
    bool autoSpacing() const { return _spacing; };
    void setLastPrintable( Lexer::Token tok, Text* t ) { _lastPrintableToken = tok; _lastPrintableItem = t; };
    Text* lastText() { return _lastPrintableItem; };
    Lexer::Token lastToken() const { return _lastPrintableToken; };
    //add a resource number to TOC index mapping
    void addRes( word key, TocRef& value );
    //add a name/id to TOC index mapping
    void addNameOrId( GlobalDictionaryWord* key, TocRef& value );
    //add a cross-reference to an identifier
    void addXRef( word res, XRef& xref );
    void addXRef( GlobalDictionaryWord* id, XRef& xref );
    //add a page to the page collection
    void addPage( Page* page );
    //add a cell to the cell collection
    void addCell( Cell* cell);
    //add a TOC file offset to the collection
    void addTOCOffset( dword o ) { _tocOffsets.push_back( o ); };
    //add a Cell file offset to the collection
    void addCellOffset( dword o ) { _cellOffsets.push_back( o ); };
    //add a synonym entry
    void addSynonym( std::wstring& key, Synonym* value );
    //get a synonym entry
    Synonym* synonym( const std::wstring& key );
    //add a primary index entry
    void addIndex( I1* i ) { _index.push_back( i ); };
    void addIndexId( std::wstring& key, I1* value );
    //get a primary index entry
    I1* indexById( const std::wstring& key );
    //add an icmd entry
    void addCmdIndex( ICmd* i ) { _icmd.push_back( i ); };
    //get the maximum size of a local dictionary in a cell
    std::size_t maxLocalDictionarySize() const { return _hdr->maxLocalIndex; };
    //parse a command
    void parseCommand( Lexer* lexer, Tag* parent );
    //get a TOC index from the resource number to TOC index map
    word tocIndexByRes( word res );
    //get a TOC index from the id or name to TOC index map
    word tocIndexById( GlobalDictionaryWord* id );
    //get a .nameit expansion
    const std::wstring* nameit( const std::wstring& key );
    std::wstring* prepNameitName( const std::wstring& key );

    std::wstring * pushFileInput( std::wstring *wfname );
    std::wstring * pushFileInput( std::string& sfname, std::wstring *wfname );
    void setTitle( std::string& buffer );

    //Forwarding functions

    //To Controls
    word getGroupIndexById( const std::wstring& id );

    //To Compiler
    std::wstring* addFileName( std::wstring* name ) { return _compiler.addFileName( name ); };
    void printError( ErrCode c ) const { _compiler.printError( c ); };
    void printError( ErrCode c, const std::wstring& text ) const { _compiler.printError( c, text ); };
    void printError( ErrCode c, const std::wstring* name, unsigned int row, unsigned int col ) const
        { _compiler.printError( c, name, row, col ); };
    void printError( ErrCode c, const std::wstring* name, unsigned int row, unsigned int col, const std::wstring& text ) const
        { _compiler.printError( c, name, row, col, text ); };
    Lexer::Token getNextToken() { return _compiler.getNextToken(); };
    void setBlockParsing( bool yn ) { _compiler.setBlockParsing( yn ); };
    bool blockParsing() { return _compiler.blockParsing(); };
    void pushInput( IpfData* blk ) { _compiler.pushInput( blk ); };
    void popInput() { _compiler.popInput(); };
    const std::wstring* dataName() const { return _compiler.dataName(); };
    unsigned int dataLine() const { return _compiler.dataLine(); };
    unsigned int lexerLine() const { return _compiler.lexerLine(); };
    unsigned int dataCol() const { return _compiler.dataCol(); };
    unsigned int lexerCol() const { return _compiler.lexerCol(); };

    //To ExternalFiles
    void addExtFile( std::wstring& str ) { _extfiles->addFile( str ); };
    byte extFileIndex( std::wstring& str ) { return _extfiles->index( str ); };

    //To FontCollection
    std::size_t addFont( const FontEntry& fnt ) { return _fonts->add( fnt ); };

    //To GlobalDictionary
    GlobalDictionaryWord * addTextToGD( GlobalDictionaryWord* gdentry ) { return _dict->insert( gdentry ); };
    word findIndex( const std::wstring& text ) { return _dict->findIndex( text ); };

    //To GNames
    void addGNameOrId( GlobalDictionaryWord* key, word value )
        { _gnames->insert( key, value ); };

    //To Nls
    const std::wstring& note() const { return _nls->note(); };
    const std::wstring& warning() const { return _nls->warning(); };
    const std::wstring& caution() const { return _nls->caution(); };
    const std::wstring& reference() const { return _nls->reference(); };
    //const std::wstring& grammar() const { return _nls->grammar(); };
    const FontEntry& cgraphicFont() const { return _nls->cgraphicFont(); };
    wchar_t olChar( unsigned int level ) const { return _nls->olChars()[level % _nls->olChars().size()]; };
    const std::wstring& olClose( unsigned int level ) const { return _nls->olClose()[level % 2]; };
    const std::wstring& ulBullet( unsigned int level ) const { return _nls->ulBullets()[level % 3]; };
    wchar_t entityChar( const std::wstring& key ) { return _nls->entityChar( key ); };

private:
    Document( const Document &rhs );            //no copy constructor
    Document& operator=( const Document &rhs ); //no assignment
    void makeBitmaps();
    void makeIndexes();
    dword writeBitmaps();
    dword writeResMap();
    dword writeNameMap();
    dword writeTOCs();
    dword writeTOCOffsets();
    void writeCells();
    dword writeCellOffsets();
    dword writeChildWindows();
    void writeSynonyms();
    dword writeIndex();
    dword writeICmd();

    Compiler&                           _compiler;
    std::auto_ptr< Nls >                _nls;
    std::auto_ptr< IpfHeader >          _hdr;
    std::auto_ptr< IpfExtHeader >       _eHdr;
    std::auto_ptr< StringTable >        _strings;
    std::auto_ptr< ExternalFiles >      _extfiles;
    std::auto_ptr< Controls >           _controls;
    std::auto_ptr< FontCollection >     _fonts;
    std::auto_ptr< GNames >             _gnames;
    std::auto_ptr< GlobalDictionary >   _dict;

    std::vector< Page* >                _pages;
    typedef std::vector< Page* >::iterator PageIter;
    typedef std::vector< Page* >::const_iterator ConstPageIter;

    std::vector< Cell* >                _cells;
    typedef std::vector< Cell* >::iterator CellIter;
    typedef std::vector< Cell* >::const_iterator ConstCellIter;

    std::map< std::wstring, dword >     _bitmapNames;
    typedef std::map< std::wstring, dword >::iterator BitmapNameIter;
    typedef std::map< std::wstring, dword >::const_iterator ConstBitmapNameIter;

    std::map< word, TocRef >            _resMap;
    typedef std::map< word, TocRef >::iterator ResMapIter;
    typedef std::map< word, TocRef >::const_iterator ConstResMapIter;

    std::map< GlobalDictionaryWord*, TocRef, ptrLess< GlobalDictionaryWord* > > _nameMap;
    typedef std::map< GlobalDictionaryWord*, TocRef, ptrLess< GlobalDictionaryWord* > >::iterator NameMapIter;
    typedef std::map< GlobalDictionaryWord*, TocRef, ptrLess< GlobalDictionaryWord* > >::const_iterator ConstNameMapIter;

    std::map< std::wstring, Synonym* >  _synonyms;    //each Synonym is owned by an ISym tag
    typedef std::map< std::wstring, Synonym* >::iterator SynIter;
    typedef std::map< std::wstring, Synonym* >::const_iterator ConstSynIter;

    std::map< std::wstring, std::wstring > _nameIts;
    typedef std::map< std::wstring, std::wstring >::iterator NameItIter;
    typedef std::map< std::wstring, std::wstring >::const_iterator ConstNameItIter;

    std::vector< I1* >                  _index;
    typedef std::vector< I1* >::iterator IndexIter;
    typedef std::vector< I1* >::const_iterator ConstIndexIter;
    std::map< std::wstring, I1* >       _indexMap;
    typedef std::map< std::wstring, I1* >::iterator IndexMapIter;
    typedef std::map< std::wstring, I1* >::const_iterator ConstIndexMapIter;

    std::vector< ICmd* >                _icmd;
    typedef std::vector< ICmd* >::iterator ICmdIter;
    typedef std::vector< ICmd* >::const_iterator ConstICmdIter;

    std::vector< dword >                _tocOffsets;
    typedef std::vector< dword >::iterator TocOffsetIter;
    typedef std::vector< dword >::const_iterator ConstTocOffsetIter;

    std::vector< dword >                _cellOffsets;
    typedef std::vector< dword >::iterator CellOffsetIter;
    typedef std::vector< dword >::const_iterator ConstCellOffsetIter;

    std::FILE*                          _tmpBitmaps;  // temporary file for bitmaps
    Text*                               _lastPrintableItem;
    unsigned int                        _maxHeaderLevel;
    unsigned int                        _headerLevel;
    byte                                _currentLeftMargin;
    Lexer::Token                        _lastPrintableToken;
    bool                                _inDoc;             //true if parsing between userdoc and euserdoc
    bool                                _spacing;           //true if automatically inserting spaces
    std::vector< std::string >          _ipfcartwork_paths;
    std::vector< std::string >          _ipfcimbed_paths;
    OutFile*                            _out;
};

#endif //DOCUMENT_INCLUDED
