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
* Description:  the document
*
****************************************************************************/

#ifndef DOCUMENT_INCLUDED
#define DOCUMENT_INCLUDED

#include "config.hpp"
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

class Cell; //forward reference
class Page;
class Tag;
class I1;
class ICmd;
class Synonym;
class Text;

class Document {
public:
    Document( Compiler& c, const char * loc );
    ~Document();
    void parse( Lexer* lexer );
    void build();
    void write( std::FILE* out );
    void summary( std::FILE* out );

    //set the output file type
    void setOutputType( Compiler::OutputType t )
        { t == Compiler::INF ? hdr->flags = 0x01 : hdr->flags = 0x10; };
    bool isInf() const { return hdr->flags == 0x01; };
    //set the lowest header level for which new pages are made
    void setHeaderCutOff( unsigned int co ) { maxHeaderLevel = co; };
    unsigned int headerCutOff() const { return maxHeaderLevel; };
    //track the current left margin
    void setLeftMargin( unsigned char lm ) { currentLeftMargin = lm; } ;
    unsigned char leftMargin() const { return currentLeftMargin; };
    //store a graphics file name
    void addBitmap( std::wstring& bmn );
    STD1::uint32_t bitmapByName( std::wstring& bmn );
    //toggle automatic insertion of spaces (for parsing)
    void toggleAutoSpacing() { spacing = !spacing; };
    bool autoSpacing() const { return spacing; };
    void setLastPrintable( Lexer::Token tok, Text* t ) { lastPrintableToken = tok; lastPrintableItem = t; };
    Text* lastText() { return lastPrintableItem; };
    Lexer::Token lastToken() const { return lastPrintableToken; };
    //add a resource number to TOC index mapping
    void addRes( STD1::uint16_t key, TocRef& value );
    //add a name/id to TOC index mapping
    void addNameOrId( GlobalDictionaryWord* key, TocRef& value );
    //add a cross-reference to an identifier
    void addXRef( STD1::uint16_t res, XRef& xref );
    void addXRef( GlobalDictionaryWord* id, XRef& xref );
    //add a page to the page collection
    void addPage( Page* page );
    //add a cell to the cell collection
    void addCell( Cell* cell);
    //add a TOC file offset to the collection
    void addTOCOffset( STD1::uint32_t o ) { tocOffsets.push_back( o ); };
    //add a Cell file offset to the collection
    void addCellOffset( STD1::uint32_t o ) { cellOffsets.push_back( o ); };
    //add a synonym entry
    void addSynonym( std::wstring& key, Synonym* value );
    //get a synonym entry
    Synonym* synonym( const std::wstring& key );
    //add a primary index entry
    void addIndex( I1* i ) { index.push_back( i ); };
    void addIndexId( std::wstring& key, I1* value );
    //get a primary index entry
    I1* indexById( const std::wstring& key );
    //add an icmd entry
    void addCmdIndex( ICmd* i ) { icmd.push_back( i ); };
    //get the maximum size of a local dictionary in a cell
    size_t maxLocalDictionarySize() const { return hdr->maxLocalIndex; };
    //parse a command
    Lexer::Token processCommand( Lexer* lexer, Tag* parent );
    //get a TOC index from the resource number to TOC index map
    STD1::uint16_t tocIndexByRes( STD1::uint16_t res );
    //get a TOC index from the id or name to TOC index map
    STD1::uint16_t tocIndexById( GlobalDictionaryWord* id );
    //get a .nameit expansion
    const std::wstring* nameit( const std::wstring& key );
    std::wstring* prepNameitName( const std::wstring& key );

    //Forwarding functions

    //To Controls
    STD1::uint16_t getGroupById( const std::wstring& i );

    //To Compiler
    std::wstring* addFileName( std::wstring* name ) { return compiler.addFileName( name ); };
    void printError( ErrCode c ) const { compiler.printError( c ); };
    void printError( ErrCode c, const std::wstring& txt ) const { compiler.printError( c, txt ); };
    void printError( ErrCode c, const std::wstring* name, unsigned int row, unsigned int col ) const
        { compiler.printError( c, name, row, col ); };
    void printError( ErrCode c, const std::wstring* name, unsigned int row, unsigned int col, const std::wstring& txt ) const
        { compiler.printError( c, name, row, col, txt ); };
    Lexer::Token getNextToken() { return compiler.getNextToken(); };
    void setBlockParsing( bool yn ) { compiler.setBlockParsing( yn ); };
    bool blockParsing() { return compiler.blockParsing(); };
    void pushInput( IpfData* blk ) { compiler.pushInput( blk ); };
    void popInput() { compiler.popInput(); };
    const std::wstring* dataName() const { return compiler.dataName(); };
    unsigned int dataLine() const { return compiler.dataLine(); };
    unsigned int lexerLine() const { return compiler.lexerLine(); };
    unsigned int dataCol() const { return compiler.dataCol(); };
    unsigned int lexerCol() const { return compiler.lexerCol(); };

    //To ExternalFiles
    void addExtFile( std::wstring& str ) { extfiles->addFile( str ); };
    STD1::uint16_t extFileIndex( std::wstring& str ) { return extfiles->index( str ); };

    //To FontCollection
    size_t addFont( FontEntry& fnt ) { return fonts->add( fnt ); };

    //To GlobalDictionary
    GlobalDictionaryWord * addWord( GlobalDictionaryWord* word ) { return dict->insert( word ); };
    STD1::uint16_t findIndex( std::wstring& word ) { return dict->findIndex( word ); };

    //To GNames
    void addGNameOrId( GlobalDictionaryWord* key, STD1::uint16_t value )
        { gnames->insert( key, value ); };

    //To Nls
    int codePage() const { return nls->codePage(); };
    const std::wstring& note() const { return nls->note(); };
    const std::wstring& warning() const { return nls->warning(); };
    const std::wstring& caution() const { return nls->caution(); };
    const std::wstring& reference() const { return nls->reference(); };
    //const std::wstring& grammer() const { return nls->grammer(); };
    const std::wstring& cgraphicFontFaceName() const { return nls->cgraphicFontFaceName(); };
    int cgraphicFontWidth() const { return nls->cgraphicFontWidth(); };
    int cgraphicFontHeight() const { return nls->cgraphicFontHeight(); };
    wchar_t olChar( unsigned int level ) const { return nls->olChars()[ level % nls->olChars().size() ]; };
    const std::wstring& olClose( unsigned int level ) const { return nls->olClose()[ level % 2 ]; };
    const std::wstring& ulBullet( unsigned int level ) const { return nls->ulBullets()[ level % 3 ]; };
    wchar_t entity( const std::wstring& key ) { return nls->entity( key ); };

    //To Strings
    void addString( std::wstring& str ) { strings->addString( str ); };

private:
    Document( const Document &rhs );            //no copy constructor
    Document& operator=( const Document &rhs ); //no assignment
    Compiler& compiler;
    std::auto_ptr< Nls > nls;
    std::auto_ptr< IpfHeader > hdr;
    std::auto_ptr< IpfExtHeader > eHdr;
    std::auto_ptr< StringTable > strings;
    std::auto_ptr< ExternalFiles > extfiles;
    std::auto_ptr< Controls > controls;
    std::auto_ptr< FontCollection > fonts;
    std::auto_ptr< GNames > gnames;
    std::auto_ptr< GlobalDictionary > dict;

    std::vector< Page* > pages;
    typedef std::vector< Page* >::iterator PageIter;
    typedef std::vector< Page* >::const_iterator ConstPageIter;

    std::vector< Cell* > cells;
    typedef std::vector< Cell* >::iterator CellIter;
    typedef std::vector< Cell* >::const_iterator ConstCellIter;

    std::map< std::wstring, STD1::uint32_t > bitmapNames;
    typedef std::map< std::wstring, STD1::uint32_t >::iterator BitmapNameIter;
    typedef std::map< std::wstring, STD1::uint32_t >::const_iterator ConstBitmapNameIter;

    std::map< STD1::uint16_t, TocRef > resMap;
    typedef std::map< STD1::uint16_t, TocRef >::iterator ResMapIter;
    typedef std::map< STD1::uint16_t, TocRef >::const_iterator ConstResMapIter;

    std::map< GlobalDictionaryWord*, TocRef, ptrLess< GlobalDictionaryWord* > > nameMap;
    typedef std::map< GlobalDictionaryWord*, TocRef, ptrLess< GlobalDictionaryWord* > >::iterator NameMapIter;
    typedef std::map< GlobalDictionaryWord*, TocRef, ptrLess< GlobalDictionaryWord* > >::const_iterator ConstNameMapIter;

    std::map< std::wstring, Synonym* > synonyms;    //each Synonym is owned by an ISym tag
    typedef std::map< std::wstring, Synonym* >::iterator SynIter;
    typedef std::map< std::wstring, Synonym* >::const_iterator ConstSynIter;

    std::map< std::wstring, std::wstring > nameIts;
    typedef std::map< std::wstring, std::wstring >::iterator NameItIter;
    typedef std::map< std::wstring, std::wstring >::const_iterator ConstNameItIter;

    std::vector< I1* > index;
    typedef std::vector< I1* >::iterator IndexIter;
    typedef std::vector< I1* >::const_iterator ConstIndexIter;
    std::map< std::wstring, I1* > indexMap;
    typedef std::map< std::wstring, I1* >::iterator IndexMapIter;
    typedef std::map< std::wstring, I1* >::const_iterator ConstIndexMapIter;

    std::vector< ICmd* > icmd;
    typedef std::vector< ICmd* >::iterator ICmdIter;
    typedef std::vector< ICmd* >::const_iterator ConstICmdIter;

    std::vector< STD1::uint32_t > tocOffsets;
    typedef std::vector< STD1::uint32_t >::iterator TocOffsetIter;
    typedef std::vector< STD1::uint32_t >::const_iterator ConstTocOffsetIter;

    std::vector< STD1::uint32_t > cellOffsets;
    typedef std::vector< STD1::uint32_t >::iterator CellOffsetIter;
    typedef std::vector< STD1::uint32_t >::const_iterator ConstCellOffsetIter;

    std::string tmpName;    //temporary storage for bitmaps
    Text* lastPrintableItem;
    unsigned int maxHeaderLevel;
    unsigned int headerLevel;
    unsigned char currentLeftMargin;
    Lexer::Token lastPrintableToken;
    bool inDoc;             //true if parsing between userdoc and euserdoc
    bool spacing;           //true if automatically inserting spaces
    void makeBitmaps();
    void makeIndexes();
    STD1::uint32_t writeBitmaps( std::FILE* out );
    STD1::uint32_t writeResMap( std::FILE* out );
    STD1::uint32_t writeNameMap( std::FILE* out );
    STD1::uint32_t writeTOCs( std::FILE* out );
    STD1::uint32_t writeTOCOffsets( std::FILE* out );
    void writeCells( std::FILE* out );
    STD1::uint32_t writeCellOffsets( std::FILE* out );
    STD1::uint32_t writeChildWindows( std::FILE* out );
    void writeSynonyms( std::FILE* out );
    STD1::uint32_t writeIndex( std::FILE* out );
    STD1::uint32_t writeICmd( std::FILE* out );
};

#endif //DOCUMENT_INCLUDED
