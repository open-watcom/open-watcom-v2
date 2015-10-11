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
* The input file is structured as follows:
* :userdoc
*   :title
*   :docprof
*   :ctrldef
*       :pbutton
*       :ctrl
*   :ectrldef
*
*   :fn
*       block
*       inline
*   :efn
*   :h1
*       :h2
*           :h3
*               :h4
*                   :h5
*                       :h6
*       :acviewport         (block)
*       :artwork            (inline)
*       :artlink            (block)
*           :link
*       :eartlink
*       :caution            (block)
*           inline
*       :ecaution
*       :cgraphic           (block)
*           inline
*       :ecgraphic
*       :color              (format)
*       :dl                 (block)
*           :dthd           (block)
*               inline
*           :ddhd           (block)
*               inline
*           :dt             (block)
*               inline
*           :dd             (block)
*               inline
*               block
*               list-block
*       :edl
*       :fig                (block)
*           inline
*           :figcap         (block)
*               text
*       :efig
*       :font               (format)
*       :hdref              (block)
*           text
*       :hide
*           block | inline
*       :ehide
*       :hpn                (format, n = 1-9)
*           block | inline
*       :ehpn
*       :i1                 (block)
*           text
*           :icmd
*       :i2                 (block)
*           text
*           :icmd
*       :icmd               (block)
*           text
*       :isyn               (block)
*           text
*       :lines              (block)
*           format
*           text
*       :elines
*       :link               (block)
*           text
*       :elink
*       :lm                 (format)
*       :note               (block)
*           inline
*       :nt                 (block)
*           inline
*           block
*       :ent
*       :ol                 (list-block)
*           :li             (block)
*               inline
*           :lp             (block)
*               inline
*           list-block
*           format
*       :eol
*       :p                  (block)
*           inline
*       :parml              (list-block)
*           :pt             (block)
*               inline
*           :pd             (block)
*               inline
*           list-block
*       :eparml
*       :rm                 (format)
*       :sl                 (block)
*           :li
*               inline
*           :lp
*               inline
*           list-block
*           format
*       :esl
*       :table              (block)
*           :row            (block)
*               :c          (block)
*                   inline
*                   text
*       :etable
*       :ul                 (list-block)
*           :li             (block)
*               inline
*           :lp             (block)
*               inline
*           list-block
*       :eul
*       :warning            (block)
*           inline
*       :ewarning
*       :xmp                (block)
*           text
*       :exmp
* :euserdoc
*
****************************************************************************/

#if defined( __unix__ ) && !defined( __UNIX__ )
    #define __UNIX__ __unix__
#endif
#include <algorithm>
#include <functional>
#include <cstdio>
#include <cstdlib>
#include "document.hpp"
#include "bitmap.hpp"
#include "brcmd.hpp"
#include "cecmd.hpp"
#include "cell.hpp"
#include "ctrldef.hpp"
#include "docprof.hpp"
#include "env.hpp"
#include "fn.hpp"
#include "hn.hpp"
#include "i1.hpp"
#include "icmd.hpp"
#include "page.hpp"
#include "ptrops.hpp"
#include "synonym.hpp"
#include "title.hpp"
#include "util.hpp"
#include "clibext.h"

extern Env Environment;

Document::Document( Compiler& c, const char* loc ) :
    compiler( c ),
    nls( new Nls( loc ) ),
    hdr( new IpfHeader() ),
    eHdr( new IpfExtHeader() ),
    strings( new StringTable() ),
    extfiles( new ExternalFiles() ),
    controls( new Controls() ),
    gnames( new GNames() ),
    dict( new GlobalDictionary() ),
    lastPrintableItem( 0 ),
    maxHeaderLevel( 3 ),
    headerLevel( 1 ),
    currentLeftMargin( 1 ),
    lastPrintableToken( Lexer::END ),
    inDoc( false ),
    spacing( true )
{
    fonts.reset( new FontCollection( codePage() ) );
    FontEntry fnt;
    char buffer[ sizeof( fnt.faceName ) ];
    size_t size( std::wcstombs( buffer, cgraphicFontFaceName().c_str(), sizeof( fnt.faceName ) ) );
    if( size == static_cast< size_t>( -1 ) )
        throw FatalError( ERR_T_CONV );
    std::strncpy( fnt.faceName, buffer, sizeof( fnt.faceName ) );
    fnt.height = cgraphicFontHeight();
    fnt.width = cgraphicFontWidth();
    fnt.codePage = codePage();
    addFont( fnt );

}
/***************************************************************************/
Document::~Document()
{
    for( CellIter itr = cells.begin(); itr != cells.end(); ++itr )
        delete *itr;
    for( PageIter itr = pages.begin(); itr != pages.end(); ++itr )
        delete *itr;
}
/***************************************************************************/
// Reads the input file and builds the DOM tree
void Document::parse( Lexer* lexer )
{
    Lexer::Token tok( getNextToken() );
    while( tok != Lexer::END && !inDoc ) {
        //only comments and whitespace are allowed before :userdoc tag
        if( tok == Lexer::TAG && lexer->tagId() == Lexer::USERDOC ) {
            inDoc = true;
            tok = getNextToken();
            while( tok != Lexer::TAGEND ) {
                if( tok == Lexer::ATTRIBUTE )
                    printError( ERR1_ATTRNOTDEF );
                else if( tok == Lexer::FLAG )
                    printError( ERR1_ATTRNOTDEF );
                else if( tok == Lexer::ERROR_TAG )
                    throw FatalError( ERR_SYNTAX );
                else if( tok == Lexer::END )
                    throw FatalError( ERR_EOF );
                else
                    printError( ERR1_TAGSYNTAX );
                tok = getNextToken();
            }
        }
        else if( tok == Lexer::COMMAND ) {
            if( lexer->cmdId() == Lexer::NAMEIT || lexer->cmdId() == Lexer::COMMENT ) {
                tok = processCommand( lexer, 0 );
                continue;
            }
            else
                printError( ERR1_TAGCONTEXT );
        }
        else if( tok != Lexer::WHITESPACE )
            printError( ERR1_HEADTEXT );
        tok = getNextToken();
    }
    if( tok == Lexer::END )                 //:userdoc tag not found
        throw FatalError( ERR_DOCBODY );
    //process tokens until first :h1 or :fn tag
    while( tok != Lexer::END ) {
        //instructions for the document
        if( tok == Lexer::TAG) {
            if( lexer->tagId() == Lexer::TITLE ) {
                Title title( this );
                tok = title.parse( lexer, hdr.get() );
            }
            else if( lexer->tagId() == Lexer::DOCPROF ) {
                DocProf dp( this );
                tok = dp.parse( lexer );
                dp.build( controls.get(), strings.get() );
            }
            else if( lexer->tagId() == Lexer::CTRLDEF ) {
                CtrlDef cd( this );
                tok = cd.parse( lexer );
                cd.build( controls.get() );
            }
            else if( lexer->tagId() == Lexer::H1 || lexer->tagId() == Lexer::FN )
                break;
            else if( lexer->tagId() == Lexer::EUSERDOC ) {
                throw FatalError( ERR_DOCSMALL );
            }
            else {
                printError( ERR1_TAGCONTEXT );
                tok = getNextToken();
            }
        }
        else if( tok == Lexer::COMMAND ) {
            if( lexer->cmdId() == Lexer::BREAK ) {
                printError( ERR1_TAGCONTEXT );
                tok = getNextToken();
            }
            else
                tok = processCommand( lexer, NULL );
        }
        else {
            if( tok != Lexer::WHITESPACE )
                printError( ERR1_HEADTEXT );
            tok = getNextToken();
        }
    }
    if( tok == Lexer::END )
        throw FatalError( ERR_EOF );
    while( tok != Lexer::END ) {
        //Should only ever see :h1, :fn :euserdoc, .*, and .imbed here
        if( tok == Lexer::TAG ) {
            if( lexer->tagId() == Lexer::H1 ) {
                Hn* h1( new Hn( this, NULL, dataName(), lexerLine(), lexerCol(), 1 ) );
                Page* pg( new Page( this, h1 ) );
                addPage( pg );
                tok = h1->parse( lexer );
            }
            else if( lexer->tagId() == Lexer::FN ) {
                Fn* fn( new Fn( this, NULL, dataName(), lexerLine(), lexerCol() ) );
                Page* pg( new Page( this, fn ) );
                addPage( pg );
                tok = fn->parse( lexer );
            }
            else if( lexer->tagId() == Lexer::EUSERDOC ) {
                inDoc = false;
                tok = getNextToken();   //should be Lexer::TAGEND
                while( tok != Lexer::TAGEND ) {
                    if( tok == Lexer::ATTRIBUTE )
                        printError( ERR1_ATTRNOTDEF );
                    else if( tok == Lexer::FLAG )
                        printError( ERR1_ATTRNOTDEF );
                    else if( tok == Lexer::ERROR_TAG )
                        throw FatalError( ERR_SYNTAX );
                    else if( tok == Lexer::END )
                        throw FatalError( ERR_EOF );
                    else
                        printError( ERR1_TAGSYNTAX );
                    tok = getNextToken();
                }
                if( tok == Lexer::TAGEND )
                    tok = getNextToken();   //should be Lexer::WHITESPACE
                break;
            }
            else {
                printError( ERR1_TAGCONTEXT );
                tok = getNextToken();
            }
        }
        else if( tok == Lexer::COMMAND ) {
            if( lexer->cmdId() == Lexer::COMMENT || lexer->cmdId() == Lexer::IMBED )
                tok = processCommand( lexer, NULL );
            else {
                printError( ERR1_TAGCONTEXT );
                tok = getNextToken();
            }
        }
        else {
            if( tok != Lexer::WHITESPACE )
                printError( ERR1_HEADTEXT );
            tok = getNextToken();
        }
    }
    if( inDoc )
        throw FatalError( ERR_DOCBODY );
    while( tok != Lexer::END ) {
        if( tok != Lexer::WHITESPACE )
            printError( ERR1_TAILTEXT );
        tok = getNextToken();
    }
    if( pages.size() > 65535 )
        throw FatalError( ERR_LARGETOC );
    if( dict->size() > 64000 )
        throw FatalError( ERR_DOCLARGE );
    else if ( dict->size() == 0 )
        throw FatalError( ERR_DOCSMALL );
}
/***************************************************************************/
// Iterate through the DOM tree to build output data
void Document::build()
{
    //build the TOC
    unsigned int visiblePages = 0;
    for( PageIter itr = pages.begin(); itr != pages.end(); ++itr ) {
        ( *itr )->buildTOC();
        if( ( *itr )->isVisible() )
            ++visiblePages;
    }
    if( !visiblePages )
        throw FatalError( ERR_INVISIBLETOC );
    tocOffsets.reserve( pages.size() );
    dict->convert( pages.size() );  //number each page
    extfiles->convert();            //number each external file
    makeBitmaps();                  //process images
    //for each page, linearize the DOM tree it contains and build local dictionary
    std::for_each( pages.begin(), pages.end(), std::mem_fun( &Page::linearize ) );
    makeIndexes();
    std::for_each( pages.begin(), pages.end(), std::mem_fun( &Page::buildLocalDictionary ) );
    std::for_each( cells.begin(), cells.end(), std::mem_fun( &Cell::build ) );
    if( compiler.searchable() && dict->buildFTS() ) //build FTS from GlobalDictionary
        hdr->recSize = true;
}
/***************************************************************************/
// Write the file
void Document::write( std::FILE *out )
{
    hdr->write( out );   //write the header
    hdr->panelCount = static_cast< STD1::uint16_t>( resMap.size() );
    hdr->panelOffset = writeResMap( out );
    hdr->nameCount = !isInf() ? static_cast< STD1::uint16_t >( nameMap.size() ) : 0;
    hdr->nameOffset = writeNameMap( out );
    eHdr->gNameOffset = gnames->write( out );
    eHdr->gNameCount = gnames->size();
    hdr->imageOffset = writeBitmaps( out );
    hdr->tocCount = static_cast< STD1::uint16_t >( pages.size() );
    hdr->tocOffset = writeTOCs( out );
    hdr->tocOffsetOffset = writeTOCOffsets( out );
    writeSynonyms( out );
    hdr->indexOffset = writeIndex( out );
    hdr->icmdOffset = writeICmd( out );
    hdr->nlsOffset = nls->write( out );
    hdr->nlsSize = nls->length();
    eHdr->stringsOffset = strings->write( out );
    eHdr->stringsSize = static_cast< STD1::uint16_t >( strings->length() );
    eHdr->dbOffset = extfiles->write( out );
    eHdr->dbCount = static_cast< STD1::uint16_t >( extfiles->size() );
    eHdr->dbSize = extfiles->length();
    eHdr->fontOffset = fonts->write( out );
    eHdr->fontCount = static_cast< STD1::uint16_t >( fonts->size() );
    eHdr->ctrlOffset = controls->write( out );
    eHdr->ctrlSize = controls->length();
    hdr->dictOffset = dict->write( out );
    hdr->dictSize = dict->length();
    hdr->dictCount = dict->size();
    writeCells( out );
    hdr->cellCount = static_cast< STD1::uint16_t >( cells.size() );
    hdr->cellOffsetOffset = writeCellOffsets( out );
    eHdr->childPagesOffset = writeChildWindows( out );
    if( compiler.searchable() ) {
        hdr->searchOffset = dict->writeFTS( out, hdr->recSize );
        hdr->searchSize = dict->ftsLength();
    }
    hdr->extOffset = eHdr->write( out );
    hdr->write( out );   //rewrite the header to update the offsets
}
/***************************************************************************/
void Document::summary( std::FILE* out )
{
    //TODO: use ostream when streams and strings mature
    std::fprintf( out, "Number of pages:          %u\n", static_cast< unsigned int >( pages.size() ) );
    std::fprintf( out, "Pages defined by name:    %u\n", static_cast< unsigned int >( nameMap.size() ) );
    std::fprintf( out, "Pages defined by number:  %u\n", static_cast< unsigned int >( resMap.size() ) );
    std::fprintf( out, "Words in dictionary:      %u\n", dict->size() );
    std::fprintf( out, "Number of index entries:  %u\n", static_cast< unsigned int >( index.size() ) ); 
    std::fprintf( out, "Global index entries:     %u\n", eHdr->gIndexCount );
    std::fprintf( out, "Number of fonts used:     %u\n", static_cast< unsigned int >( fonts->size() ) );
    std::fprintf( out, "Number of External files: %u\n", extfiles->size() );
    std::fprintf( out, "\nIdentifier Cross-reference\n" );
    for( ConstResMapIter itr( resMap.begin() ); itr != resMap.end(); ++itr ) {
        std::fprintf( out, "%5u:", itr->first );
        itr->second.write( out );
    }
    for( ConstNameMapIter itr( nameMap.begin() ); itr != nameMap.end(); ++itr ) {
        std::fprintf( out, "%ls:\n      ", itr->first->getText().c_str() );
        itr->second.write( out );
    }
}
/***************************************************************************/
void Document::addBitmap( std::wstring& bmn )
{
    bitmapNames.insert( std::map< std::wstring, unsigned long int >::value_type( bmn, 0 ) );
}
/***************************************************************************/
STD1::uint32_t Document::bitmapByName( std::wstring& bmn )
{
    BitmapNameIter itr( bitmapNames.find( bmn ) );
    if( itr == bitmapNames.end() )
        return 0;
    return itr->second;
}
/***************************************************************************/
void Document::addRes( STD1::uint16_t key, TocRef& value )
{
    if( resMap.find( key ) == resMap.end() )    //add it to the list
        resMap.insert( std::map< STD1::uint16_t, TocRef >::value_type( key, value ) );
    else
        throw Class3Error( ERR3_DUPRES );
}
/***************************************************************************/
void Document::addNameOrId( GlobalDictionaryWord* key, TocRef& value )
{
    if( nameMap.find( key ) == nameMap.end() )  //add it to the list
        nameMap.insert( std::map< GlobalDictionaryWord*, TocRef, ptrLess< GlobalDictionaryWord* > >::value_type( key, value ) );
    else
        throw Class3Error( ERR3_DUPID );
}
/***************************************************************************/
void Document::addXRef( STD1::uint16_t res, XRef& xref )
{
    ResMapIter itr( resMap.find( res ) );
    if( itr == resMap.end() )
        throw Class1Error( ERR1_NORES );
    itr->second.addXRef( xref );
}
/***************************************************************************/
void Document::addXRef( GlobalDictionaryWord* id, XRef& xref )
{
    NameMapIter itr( nameMap.find( id ) );
    if( itr == nameMap.end() )
        throw Class1Error( ERR1_NOID );
    itr->second.addXRef( xref );
}
/***************************************************************************/
void Document::addPage( Page* page )
{
    page->setIndex( static_cast< STD1::uint16_t >( pages.size() ) );
    pages.push_back( page );
}
/***************************************************************************/
void Document::addCell( Cell* cell )
{
    cell->setIndex( cells.size() );
    cells.push_back( cell );
}
/***************************************************************************/
void Document::makeIndexes()
{
    std::sort( index.begin(), index.end(), ptrLess< I1* >() );
    std::sort( icmd.begin(), icmd.end(), ptrLess< ICmd* >() );
    std::for_each( pages.begin(), pages.end(), std::mem_fun( &Page::buildIndex ) );
}
/***************************************************************************/
void Document::makeBitmaps()
{
    if( !bitmapNames.empty() ) {
        //could use tmpfile...
        tmpName = Environment.value( "TMP" );
        tmpName += std::tmpnam( NULL );
        std::FILE* tmp( std::fopen( tmpName.c_str(), "wb" ) );
        if( !tmp )
            throw FatalIOError( ERR_OPEN, L"(temporary file for bitmaps)" );
        //get IPFCARTWORK from env
        std::string env( Environment.value( "IPFCARTWORK" ) );
        std::vector< std::string > paths;
        std::string cwd;    //empty string for current directory
        paths.push_back( cwd );
#ifdef __UNIX__
        std::string separators( ":;" );
        char slash( '/' );
#else
        std::string separators( ";" );
        char slash( '\\' );
#endif
        std::string::size_type idx1( 0 );
        std::string::size_type idx2( env.find_first_of( separators, idx1 ) );
        paths.push_back( env.substr( idx1, idx2 - idx1 ) );
        while( idx2 != std::string::npos ) {
            idx1 = idx2 + 1;
            idx2 = env.find_first_of( separators, idx1 );
            paths.push_back( env.substr( idx1, idx2 - idx1 ) );
        }
        try {
            for( BitmapNameIter itr = bitmapNames.begin(); itr != bitmapNames.end(); ++itr ) {
                std::string fname;
                wtombstring( itr->first, fname );
                for( size_t count = 0; count < paths.size(); ++count ) {
                    std::string fullname( paths[ count ] );
                    if( !fullname.empty() )
                        fullname += slash;
                    fullname += fname;
#ifndef __UNIX__
                    if( fullname.size() > PATH_MAX ) {
                        throw FatalError( ERR_PATH_MAX );
                    }
#endif
                    try {
#ifdef CHECKCOMP
                        std::printf( "Processing bitmap %s\n", fullname.c_str() );
#endif
                        Bitmap bm( fullname );
                        itr->second = bm.write( tmp );
                        break;
                    }
                    catch( FatalError& e ) {
                        if( count == paths.size() - 1 )
                            throw FatalIOError( e.code, itr->first );
                    }
                    catch( Class1Error& e ) {
                        printError( e.code, itr->first );
                    }
                }
            }
        }
        catch( FatalError& e ) {
            std::fclose( tmp );
            std::remove( tmpName.c_str() );
            throw e;
        }
        catch( FatalIOError& e ) {
            std::fclose( tmp );
            std::remove( tmpName.c_str() );
            throw e;
        }
        std::fclose( tmp );
    }
}
/***************************************************************************/
STD1::uint32_t Document::writeBitmaps( std::FILE* out )
{
    STD1::uint32_t offset( 0 );
    if( !bitmapNames.empty() ) {
        offset = std::ftell( out );
        std::FILE* tmp( std::fopen( tmpName.c_str(), "rb" ) );
        if( !tmp )
            throw FatalIOError( ERR_OPEN, L"(temporary file for bitmaps)" );
        std::fseek( tmp, 0L, SEEK_END );
        STD1::uint32_t length( std::ftell( tmp ) );
        std::fseek( tmp, 0L, SEEK_SET );
        std::vector< STD1::uint8_t > buffer( BUFSIZ );
        //copy the temporary file into this one
        try {
            while( length > BUFSIZ ) {
                if( std::fread( &buffer[0], sizeof( STD1::uint8_t ), BUFSIZ, tmp ) != BUFSIZ )
                    throw FatalIOError( ERR_READ, L"(temporary file for bitmaps)" );
                if( std::fwrite( &buffer[0], sizeof( STD1::uint8_t ), BUFSIZ, out ) != BUFSIZ )
                    throw FatalError( ERR_WRITE );
                length -= BUFSIZ;
            }
            if( length ) {
                if( std::fread( &buffer[0], sizeof( STD1::uint8_t ), length, tmp ) != length )
                    throw FatalIOError( ERR_READ, L"(temporary file for bitmaps)" );
                if( std::fwrite( &buffer[0], sizeof( STD1::uint8_t ), length, out ) != length )
                    throw FatalError( ERR_WRITE );
            }
        }
        catch( FatalError& e ) {
            std::fclose( tmp );
            std::remove( tmpName.c_str() );
            throw e;
        }
        catch( FatalIOError& e ) {
            std::fclose( tmp );
            std::remove( tmpName.c_str() );
            throw e;
        }
        std::fclose( tmp );
        std::remove( tmpName.c_str() );
    }
    return offset;
}
/***************************************************************************/
STD1::uint32_t Document::writeResMap( std::FILE* out )
{
    STD1::uint32_t offset( 0 );
    if( !resMap.empty() ) {
        offset = std::ftell( out );
        ConstResMapIter itr;
        for( itr = resMap.begin(); itr != resMap.end(); ++itr ) {
            if( std::fwrite( &itr->first, sizeof( STD1::uint16_t ), 1, out ) != 1 )
                throw FatalError( ERR_WRITE );
        }
        for( itr = resMap.begin(); itr != resMap.end(); ++itr ) {
            STD1::uint16_t idx( itr->second.index() );
            if( std::fwrite( &idx, sizeof( STD1::uint16_t ), 1, out ) != 1 )
                throw FatalError( ERR_WRITE );
        }
    }
    return offset;
}
/***************************************************************************/
STD1::uint32_t Document::writeNameMap( std::FILE* out )
{
    STD1::uint32_t offset( 0 );
    if( !isInf() && !nameMap.empty() ) {
        offset = std::ftell( out );
        ConstNameMapIter itr;
        for( itr = nameMap.begin(); itr != nameMap.end(); ++itr ) {
            STD1::uint16_t index( itr->first->index() );
            if( std::fwrite( &index, sizeof( STD1::uint16_t ), 1, out ) != 1 )
                throw FatalError( ERR_WRITE );
        }
        for( itr = nameMap.begin(); itr != nameMap.end(); ++itr ) {
            STD1::uint16_t idx( itr->second.index() );
            if( std::fwrite( &idx, sizeof( STD1::uint16_t ), 1, out ) != 1 )
                throw FatalError( ERR_WRITE );
        }
    }
    return offset;
}
/***************************************************************************/
STD1::uint32_t Document::writeTOCs( std::FILE* out )
{
    STD1::uint32_t offset( std::ftell( out ) );
    hdr->tocSize = 0;
    for( PageIter itr = pages.begin(); itr != pages.end(); ++itr ) {
        addTOCOffset( ( *itr )->write( out ) );
        hdr->tocSize += ( *itr )->tocSize();
    }
    return offset;
}
/***************************************************************************/
STD1::uint32_t Document::writeTOCOffsets( std::FILE* out )
{
    STD1::uint32_t offset( 0 );
    if( !tocOffsets.empty() ) {
        offset = std::ftell( out );
        if( std::fwrite( &tocOffsets[0], sizeof( STD1::uint32_t ),
            tocOffsets.size(), out ) != tocOffsets.size() )
            throw FatalError( ERR_WRITE );
    }
    return offset;
}
/**************************************************************************/
void Document::writeCells( std::FILE* out )
{
    if( cells.size() > UINT16_MAX )
        throw FatalError( ERR_LARGETOC );
    for( CellIter itr = cells.begin(); itr != cells.end(); ++itr )
        addCellOffset( (*itr)->write( out ) );
}
/***************************************************************************/
STD1::uint32_t Document::writeCellOffsets( std::FILE* out )
{
    STD1::uint32_t offset( 0 );
    if( !cellOffsets.empty() ) {
        offset = std::ftell( out );
        if( std::fwrite( &cellOffsets[0], sizeof( STD1::uint32_t ),
            cellOffsets.size(), out ) != cellOffsets.size() )
            throw FatalError( ERR_WRITE );
    }
    return offset;
}
/***************************************************************************/
STD1::uint32_t Document::writeChildWindows( std::FILE* out )
{
    STD1::uint32_t offset( std::ftell( out ) );
    eHdr->childPagesSize = 0;
    for( PageIter itr = pages.begin(); itr != pages.end(); ++itr )
        eHdr->childPagesSize += ( *itr )->writeChildren( out );
    return eHdr->childPagesSize ? offset : 0;
}
/***************************************************************************/
void Document::writeSynonyms( std::FILE* out )
{
    for( SynIter itr = synonyms.begin(); itr != synonyms.end(); ++itr )
        itr->second->write( out );
}
/***************************************************************************/
STD1::uint32_t Document::writeIndex( std::FILE* out )
{
    STD1::uint32_t offset( 0 );
    hdr->indexCount = 0;
    hdr->indexSize = 0;
    eHdr->gIndexCount = 0;
    if( !index.empty() ) {
        offset = std::ftell( out );
        STD1::uint32_t count( 0 );
        STD1::uint32_t gcount( 0 );
        STD1::uint32_t size( 0 );
        for( IndexIter itr = index.begin(); itr != index.end(); ++itr ) {
            size += ( *itr )->write( out );
            count += ( *itr )->secondaryCount() + 1;
            if( ( *itr )->isGlobal() )
                gcount += 1;
        }
        if( count > UINT16_MAX )
            throw FatalError( ERR_LARGEINDEX );
        hdr->indexCount = static_cast< STD1::uint16_t >( count );
        hdr->indexSize = size;
        eHdr->gIndexCount = gcount;
    }
    return offset;
}
/***************************************************************************/
STD1::uint32_t Document::writeICmd( std::FILE* out )
{
    STD1::uint32_t offset( 0 );
    hdr->icmdCount = 0;
    hdr->icmdSize = 0;
    if( !icmd.empty() ) {
        if( icmd.size() > UINT16_MAX )
            throw FatalError( ERR_LARGEINDEX );
        offset = std::ftell( out );
        STD1::uint32_t size( 0 );
        for( ICmdIter itr = icmd.begin(); itr != icmd.end(); ++itr )
            size += ( *itr )->write( out );
        hdr->icmdCount = static_cast< STD1::uint16_t >( icmd.size() );
        hdr->icmdSize = size;
    }
    return offset;
}
/*****************************************************************************
* First character of command id should be second char of line
* Lexer returns a full line of data, needs to be parsed if .ce
* In version 1.0, valid commands are:
* .* (comment) followed by text to '\n'
* .br (new line), nothing else allowed on line
* .im (include file) 'filename'
* .nameit (define text macro) symbol=[a-zA-Z0-9]+ (10 max) text='text string'
*     text may contain entity references, nameit references and tags
* .ce (center) no tags, but text and both entity types
* Version 2.0 only supports .*, .br, .im
*/
Lexer::Token Document::processCommand( Lexer* lexer, Tag* parent )
{
    if( lexer->cmdId() == Lexer::COMMENT )
        ;//do nothing
    else if( lexer->cmdId() == Lexer::BREAK )
        parent->appendChild( new BrCmd( this, parent, dataName(), dataLine(), dataCol() ) );
    else if( lexer->cmdId() == Lexer::CENTER ) {
        CeCmd* cecmd( new CeCmd( this, parent, dataName(), dataLine(), dataCol() ) );
        parent->appendChild( cecmd );
        return cecmd->parse( lexer );
    }
    else if( lexer->cmdId() == Lexer::IMBED ) {
        std::string env( Environment.value( "IPFCIMBED" ) );
        std::vector< std::wstring > paths;
        std::wstring cwd;   //empty string for current directory
        paths.push_back( cwd );
#ifdef __UNIX__
        std::string separators( ":;" );
        char slash( '/' );
#else
        std::string separators( ";" );
        char slash( '\\' );
#endif
        std::string::size_type idx1( 0 );
        std::string::size_type idx2( env.find_first_of( separators, idx1 ) );
        std::wstring fbuffer;
        mbtowstring( env.substr( idx1, idx2 - idx1 ), fbuffer );
        paths.push_back( fbuffer );
        while( idx2 != std::string::npos ) {
            idx1 = idx2 + 1;
            idx2 = env.find_first_of( separators, idx1 );
            fbuffer.clear();
            mbtowstring( env.substr( idx1, idx2 - idx1 ), fbuffer );
            paths.push_back( fbuffer );
        }
        for( size_t count = 0; count < paths.size(); ++count ) {
            std::wstring* fname( new std::wstring( paths[ count ] ) );
            if( !fname->empty() )
                *fname += slash;
            *fname += lexer->text();
#ifndef __UNIX__
            if( fname->size() > PATH_MAX ) {
                throw FatalError( ERR_PATH_MAX );
            }
#endif
            try {
                IpfFile* ipff( new IpfFile( fname ) );
                fname = addFileName( fname );
                pushInput( ipff );
                break;
            }
            catch( FatalError& e ) {
                delete fname;
                if( count == paths.size() - 1 )
                    throw e;
            }
            catch( FatalIOError& e ) {
                delete fname;
                if( count == paths.size() - 1 )
                    throw e;
            }
        }
    }
    else if( lexer->cmdId() == Lexer::NAMEIT ) {
        std::wstring::size_type idx1( lexer->text().find( L"symbol=" ) );
        std::wstring::size_type idx2( lexer->text().find( L' ', idx1 ) );
        std::wstring sym( lexer->text().substr( idx1 + 7, idx2 - idx1 - 7 ) );
        killQuotes( sym );
        sym.insert( sym.begin(), L'&' );
        sym += L'.';
        std::wstring::size_type idx3( lexer->text().find( L"text=" ) );
        //check for single quotes
        std::wstring::size_type idx4( lexer->text()[ idx3 + 5 ] == L'\'' ? \
            lexer->text().find( L'\'', idx3  + 6 ) : \
            lexer->text().find( L' ', idx3 + 5 ) );
        std::wstring txt( lexer->text().substr( idx3 + 5, idx4 - idx3 - 5 ) );
        killQuotes( txt );
        if( !nls->isEntity( sym ) && nameIts.find( sym ) == nameIts.end() ) //add it to the list
            nameIts.insert( std::map< std::wstring, std::wstring >::value_type( sym, txt ) );
        else
            printError( ERR3_DUPSYMBOL );
    }
    else
        printError( ERR1_CMDNOTDEF );
    return getNextToken();
}
/***************************************************************************/
//get a TOC index from the resource number to TOC index map
STD1::uint16_t Document::tocIndexByRes( STD1::uint16_t res )
{
    ResMapIter itr( resMap.find( res ) );
    if( itr == resMap.end() )
        throw Class1Error( ERR1_NORES );
    return itr->second.index();
}
/***************************************************************************/
//get a TOC index from the id or name to TOC index map
STD1::uint16_t Document::tocIndexById( GlobalDictionaryWord* id )
{
    NameMapIter itr( nameMap.find( id ) );
    if( itr == nameMap.end() )
        throw Class1Error( ERR1_NOID );
    return itr->second.index();
}
/***************************************************************************/
void Document::addSynonym( std::wstring& key, Synonym* value )
{
    if( synonyms.find( key ) == synonyms.end() )    //add it to the list
        synonyms.insert( std::map< std::wstring, Synonym* >::value_type( key, value ) );
    else
        throw Class3Error( ERR3_DUPSYN );
}
/***************************************************************************/
Synonym* Document::synonym( const std::wstring& key )
{
    SynIter itr( synonyms.find( key ) );
    if( itr == synonyms.end() )
        throw Class3Error( ERR3_NOSYN );
    return itr->second;
}
/***************************************************************************/
void Document::addIndexId( std::wstring& key, I1* value )
{
    if( indexMap.find( key ) == indexMap.end() )    //add it to the list
        indexMap.insert( std::map< std::wstring, I1* >::value_type( key, value ) );
    else
        throw Class3Error( ERR3_DUPID );
}
/***************************************************************************/
I1* Document::indexById( const std::wstring& key )
{
    IndexMapIter itr( indexMap.find( key ) );
    if( itr == indexMap.end() )
        throw Class1Error( ERR1_NOID );
    return itr->second;
}
/***************************************************************************/
const std::wstring* Document::nameit( const std::wstring& key )
{
    NameItIter itr( nameIts.find( key ) );
    if( itr == nameIts.end() )
        return 0;
    return &(itr->second);
}
/***************************************************************************/
std::wstring* Document::prepNameitName( const std::wstring& key )
{
    std::wstring* name( new std::wstring( L"Expansion of .nameit " ) );
    name->append( key );
    name = addFileName( name );
    return name;
}
/***************************************************************************/
STD1::uint16_t Document::getGroupById( const std::wstring& i )
{
    ControlGroup* grp( controls->getGroupById( i ) );
    if( !grp ) {
        compiler.printError( ERR1_NOID, i );
        return 0;
    }
    else
        return grp->index() + 1;
}
