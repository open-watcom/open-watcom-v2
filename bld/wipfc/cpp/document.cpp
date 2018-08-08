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

#include "wipfc.hpp"
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

#ifndef HAVE_CONFIG_H
#include "clibext.h"
#endif


Document::Document( Compiler& c, const char* loc ) :
    _compiler( c ),
    _nls( new Nls( loc ) ),
    _hdr( new IpfHeader() ),
    _eHdr( new IpfExtHeader() ),
    _strings( new StringTable() ),
    _extfiles( new ExternalFiles() ),
    _controls( new Controls() ),
    _fonts( new FontCollection( MAX_FONTS ) ),
    _gnames( new GNames() ),
    _dict( new GlobalDictionary() ),
    _tmpBitmaps( NULL ),
    _lastPrintableItem( 0 ),
    _maxHeaderLevel( 3 ),
    _headerLevel( 1 ),
    _currentLeftMargin( 1 ),
    _lastPrintableToken( Lexer::END ),
    _inDoc( false ),
    _spacing( true )
{
    std::string env;
    std::string::size_type idx1;
    std::string::size_type idx2;
    std::string separators( PATH_LIST_SEPARATORS );
    std::string path;

    addFont( cgraphicFont() );
    //get IPFCARTWORK from env
    env = Environment.value( "IPFCARTWORK" );
    ipfcartwork_paths.push_back( "" );
    idx1 = 0;
    idx2 = env.find_first_of( separators, idx1 );
    path = env.substr( idx1, idx2 - idx1 );
    if( !path.empty() )
        path += PATH_SEPARATOR;
    ipfcartwork_paths.push_back( path );
    while( idx2 != std::string::npos ) {
        idx1 = idx2 + 1;
        idx2 = env.find_first_of( separators, idx1 );
        path = env.substr( idx1, idx2 - idx1 );
        if( !path.empty() )
            path += PATH_SEPARATOR;
        ipfcartwork_paths.push_back( path );
    }
    //get IPFCIMBED from env
    env = Environment.value( "IPFCIMBED" );
    ipfcimbed_paths.push_back( "" );
    idx1 = 0;
    idx2 = env.find_first_of( separators, idx1 );
    path = env.substr( idx1, idx2 - idx1 );
    if( !path.empty() )
        path += PATH_SEPARATOR;
    ipfcimbed_paths.push_back( path );
    while( idx2 != std::string::npos ) {
        idx1 = idx2 + 1;
        idx2 = env.find_first_of( separators, idx1 );
        path = env.substr( idx1, idx2 - idx1 );
        if( !path.empty() )
            path += PATH_SEPARATOR;
        ipfcimbed_paths.push_back( path );
    }
}
/***************************************************************************/
Document::~Document()
{
    if( _tmpBitmaps != NULL )
        std::fclose( _tmpBitmaps );
    for( CellIter itr = _cells.begin(); itr != _cells.end(); ++itr )
        delete *itr;
    for( PageIter itr = _pages.begin(); itr != _pages.end(); ++itr ) {
        delete *itr;
    }
    ipfcartwork_paths.resize( 0 );
    ipfcimbed_paths.resize( 0 );
}
/***************************************************************************/
// Reads the input file and builds the DOM tree
void Document::parse( Lexer* lexer )
{
    Lexer::Token tok;

    while( (tok = getNextToken()) != Lexer::END && !_inDoc ) {
        //only comments and whitespace are allowed before :userdoc tag
        if( tok == Lexer::TAG && lexer->tagId() == Lexer::USERDOC ) {
            _inDoc = true;
            while( (tok = getNextToken()) != Lexer::TAGEND ) {
                if( tok == Lexer::ATTRIBUTE ) {
                    printError( ERR1_ATTRNOTDEF );
                } else if( tok == Lexer::FLAG ) {
                    printError( ERR1_ATTRNOTDEF );
                } else if( tok == Lexer::ERROR_TAG ) {
                    throw FatalError( ERR_SYNTAX );
                } else if( tok == Lexer::END ) {
                    throw FatalError( ERR_EOF );
                } else {
                    printError( ERR1_TAGSYNTAX );
                }
            }
        } else if( tok == Lexer::COMMAND ) {
            if( lexer->cmdId() == Lexer::NAMEIT || lexer->cmdId() == Lexer::COMMENT ) {
                parseCommand( lexer, 0 );
            } else {
                printError( ERR1_TAGCONTEXT );
            }
        } else if( tok != Lexer::WHITESPACE ) {
            printError( ERR1_HEADTEXT );
        }
    }
    if( tok == Lexer::END )                 //:userdoc tag not found
        throw FatalError( ERR_DOCBODY );
    //process tokens until first :h1 or :fn tag
    while( tok != Lexer::END ) {
        //instructions for the document
        if( tok == Lexer::TAG) {
            if( lexer->tagId() == Lexer::TITLE ) {
                Title title( this );
                tok = title.parse( lexer, _hdr.get() );
            } else if( lexer->tagId() == Lexer::DOCPROF ) {
                DocProf dp( this );
                tok = dp.parse( lexer );
                dp.build( _controls.get(), _strings.get() );
            } else if( lexer->tagId() == Lexer::CTRLDEF ) {
                CtrlDef cd( this );
                tok = cd.parse( lexer );
                cd.build( _controls.get() );
            } else if( lexer->tagId() == Lexer::H1 || lexer->tagId() == Lexer::FN ) {
                break;
            } else if( lexer->tagId() == Lexer::EUSERDOC ) {
                throw FatalError( ERR_DOCSMALL );
            } else {
                printError( ERR1_TAGCONTEXT );
                tok = getNextToken();
            }
        } else if( tok == Lexer::COMMAND ) {
            if( lexer->cmdId() == Lexer::BREAK ) {
                printError( ERR1_TAGCONTEXT );
            } else {
                parseCommand( lexer, NULL );
            }
            tok = getNextToken();
        } else {
            if( tok != Lexer::WHITESPACE )
                printError( ERR1_HEADTEXT );
            tok = getNextToken();
        }
    }
    if( tok == Lexer::END )
        throw FatalError( ERR_EOF );
    while( tok != Lexer::END ) {
        //Should only ever see :hx, :fn :euserdoc, .*, and .imbed here
        if( tok == Lexer::TAG ) {
            if( lexer->tagId() == Lexer::H1 ) {
                Hn* h1( new Hn( this, NULL, dataName(), lexerLine(), lexerCol(), 1 ) );
                Page* pg( new Page( this, h1 ) );
                addPage( pg );
                tok = h1->parse( lexer );
            } else if( lexer->tagId() == Lexer::H2 ) {
                Hn* h2( new Hn( this, NULL, dataName(), lexerLine(), lexerCol(), 2 ) );
                Page* pg( new Page( this, h2 ) );
                addPage( pg );
                tok = h2->parse( lexer );
            } else if( lexer->tagId() == Lexer::H3 ) {
                Hn* h3( new Hn( this, NULL, dataName(), lexerLine(), lexerCol(), 3 ) );
                Page* pg( new Page( this, h3 ) );
                addPage( pg );
                tok = h3->parse( lexer );
            } else if( lexer->tagId() == Lexer::H4 ) {
                Hn* h4( new Hn( this, NULL, dataName(), lexerLine(), lexerCol(), 4 ) );
                Page* pg( new Page( this, h4 ) );
                addPage( pg );
                tok = h4->parse( lexer );
            } else if( lexer->tagId() == Lexer::H5 ) {
                Hn* h5( new Hn( this, NULL, dataName(), lexerLine(), lexerCol(), 5 ) );
                Page* pg( new Page( this, h5 ) );
                addPage( pg );
                tok = h5->parse( lexer );
            } else if( lexer->tagId() == Lexer::H6 ) {
                Hn* h6( new Hn( this, NULL, dataName(), lexerLine(), lexerCol(), 6 ) );
                Page* pg( new Page( this, h6 ) );
                addPage( pg );
                tok = h6->parse( lexer );
            } else if( lexer->tagId() == Lexer::FN ) {
                Fn* fn( new Fn( this, NULL, dataName(), lexerLine(), lexerCol() ) );
                Page* pg( new Page( this, fn ) );
                addPage( pg );
                tok = fn->parse( lexer );
            } else if( lexer->tagId() == Lexer::EUSERDOC ) {
                _inDoc = false;
                // should be Lexer::TAGEND
                while( (tok = getNextToken()) != Lexer::TAGEND ) {
                    if( tok == Lexer::ATTRIBUTE ) {
                        printError( ERR1_ATTRNOTDEF );
                    } else if( tok == Lexer::FLAG ) {
                        printError( ERR1_ATTRNOTDEF );
                    } else if( tok == Lexer::ERROR_TAG ) {
                        throw FatalError( ERR_SYNTAX );
                    } else if( tok == Lexer::END ) {
                        throw FatalError( ERR_EOF );
                    } else {
                        printError( ERR1_TAGSYNTAX );
                    }
                }
                if( tok == Lexer::TAGEND )
                    tok = getNextToken();   //should be Lexer::WHITESPACE
                break;
            } else {
                printError( ERR1_TAGCONTEXT );
                tok = getNextToken();
            }
        } else if( tok == Lexer::COMMAND ) {
            if( lexer->cmdId() == Lexer::COMMENT || lexer->cmdId() == Lexer::IMBED ) {
                parseCommand( lexer, NULL );
            } else {
                printError( ERR1_TAGCONTEXT );
            }
            tok = getNextToken();
        } else {
            if( tok != Lexer::WHITESPACE )
                printError( ERR1_HEADTEXT );
            tok = getNextToken();
        }
    }
    if( _inDoc )
        throw FatalError( ERR_DOCBODY );
    for( ; tok != Lexer::END; tok = getNextToken() ) {
        if( tok != Lexer::WHITESPACE ) {
            printError( ERR1_TAILTEXT );
        }
    }
    if( _pages.size() > 65535 )
        throw FatalError( ERR_LARGETOC );
    if( _dict->size() > 64000 ) {
        throw FatalError( ERR_DOCLARGE );
    } else if( _dict->size() == 0 ) {
        throw FatalError( ERR_DOCSMALL );
    }
}
/***************************************************************************/
// Iterate through the DOM tree to build output data
void Document::build()
{
    //build the TOC
    unsigned int visiblePages = 0;
    for( PageIter itr = _pages.begin(); itr != _pages.end(); ++itr ) {
        ( *itr )->buildTOC();
        if( ( *itr )->isVisible() ) {
            ++visiblePages;
        }
    }
    if( !visiblePages )
        throw FatalError( ERR_INVISIBLETOC );
    _tocOffsets.reserve( _pages.size() );
    _dict->convert( _pages.size() );  //number each page
    _extfiles->convert();            //number each external file
    makeBitmaps();                  //process images
    //for each page, linearize the DOM tree it contains and build local dictionary
    std::for_each( _pages.begin(), _pages.end(), std::mem_fun( &Page::linearize ) );
    makeIndexes();
    std::for_each( _pages.begin(), _pages.end(), std::mem_fun( &Page::buildLocalDictionary ) );
    std::for_each( _cells.begin(), _cells.end(), std::mem_fun( &Cell::build ) );
    if( _compiler.searchable() ) {
        _hdr->setBigFTS( _dict->buildFTS() ); //build FTS from GlobalDictionary
    }
}
/***************************************************************************/
// Write the file
void Document::write( std::FILE *out )
{
    _hdr->write( out );   //write the header
    _hdr->panelCount = static_cast< word >( _resMap.size() );
    _hdr->panelOffset = writeResMap( out );
    _hdr->nameCount = !isInf() ? static_cast< word >( _nameMap.size() ) : 0;
    _hdr->nameOffset = writeNameMap( out );
    _eHdr->gNameOffset = _gnames->write( out );
    _eHdr->gNameCount = _gnames->size();
    _hdr->imageOffset = writeBitmaps( out );
    _hdr->tocCount = static_cast< word >( _pages.size() );
    _hdr->tocOffset = writeTOCs( out );
    _hdr->tocOffsetOffset = writeTOCOffsets( out );
    writeSynonyms( out );
    _hdr->indexOffset = writeIndex( out );
    _hdr->icmdOffset = writeICmd( out );
    _hdr->nlsOffset = _nls->write( out );
    _hdr->nlsSize = _nls->length();
    _eHdr->stringsOffset = _strings->write( out, this );
    _eHdr->stringsSize = static_cast< word >( _strings->length() );
    _eHdr->dbOffset = _extfiles->write( out, this );
    _eHdr->dbCount = static_cast< word >( _extfiles->size() );
    _eHdr->dbSize = _extfiles->length();
    _eHdr->fontOffset = _fonts->write( out, this );
    _eHdr->fontCount = static_cast< word >( _fonts->size() );
    _eHdr->ctrlOffset = _controls->write( out, this );
    _eHdr->ctrlSize = _controls->length();
    _hdr->dictOffset = _dict->write( out, this );
    _hdr->dictSize = _dict->length();
    _hdr->dictCount = _dict->size();
    writeCells( out );
    _hdr->cellCount = static_cast< word >( _cells.size() );
    _hdr->cellOffsetOffset = writeCellOffsets( out );
    _eHdr->childPagesOffset = writeChildWindows( out );
    if( _compiler.searchable() ) {
        bool big = _hdr->isBigFTS();
        _hdr->searchOffset = _dict->writeFTS( out, big );
        _hdr->setBigFTS( big );
        _hdr->searchSize = _dict->ftsLength();
    }
    _hdr->extOffset = _eHdr->write( out );
    _hdr->write( out );   //rewrite the header to update the offsets
}
/***************************************************************************/
void Document::summary( std::FILE* out )
{
    //TODO: use ostream when streams and strings mature
    std::fprintf( out, "Number of pages:          %u\n", static_cast< unsigned int >( _pages.size() ) );
    std::fprintf( out, "Pages defined by name:    %u\n", static_cast< unsigned int >( _nameMap.size() ) );
    std::fprintf( out, "Pages defined by number:  %u\n", static_cast< unsigned int >( _resMap.size() ) );
    std::fprintf( out, "Words in dictionary:      %u\n", _dict->size() );
    std::fprintf( out, "Number of index entries:  %u\n", static_cast< unsigned int >( _index.size() ) );
    std::fprintf( out, "Global index entries:     %u\n", _eHdr->gIndexCount );
    std::fprintf( out, "Number of fonts used:     %u\n", static_cast< unsigned int >( _fonts->size() ) );
    std::fprintf( out, "Number of External files: %u\n", _extfiles->size() );
    std::fprintf( out, "\nIdentifier Cross-reference\n" );
    for( ConstResMapIter itr( _resMap.begin() ); itr != _resMap.end(); ++itr ) {
        std::fprintf( out, "%5u:", itr->first );
        itr->second.write( out );
    }
    for( ConstNameMapIter itr( _nameMap.begin() ); itr != _nameMap.end(); ++itr ) {
        std::fprintf( out, "%ls:\n      ", itr->first->getText().c_str() );
        itr->second.write( out );
    }
}
/***************************************************************************/
void Document::addBitmap( std::wstring& bmn )
{
    _bitmapNames.insert( std::map< std::wstring, unsigned long int >::value_type( bmn, 0 ) );
}
/***************************************************************************/
STD1::uint32_t Document::bitmapByName( std::wstring& bmn )
{
    BitmapNameIter itr( _bitmapNames.find( bmn ) );
    if( itr == _bitmapNames.end() )
        return 0;
    return itr->second;
}
/***************************************************************************/
void Document::addRes( word key, TocRef& value )
{
    if( !key ) {
        throw Class3Error( ERR3_MISSINGRES );
    } else if( _resMap.find( key ) == _resMap.end() ) {   //add it to the list
        _resMap.insert( std::map< word, TocRef >::value_type( key, value ) );
    } else {
        throw Class3Error( ERR3_DUPRES );
    }
}
/***************************************************************************/
void Document::addNameOrId( GlobalDictionaryWord* key, TocRef& value )
{
    if( _nameMap.find( key ) == _nameMap.end() ) {    //add it to the list
        _nameMap.insert( std::map< GlobalDictionaryWord*, TocRef, ptrLess< GlobalDictionaryWord* > >::value_type( key, value ) );
    } else {
        throw Class3Error( ERR3_DUPID );
    }
}
/***************************************************************************/
void Document::addXRef( word res, XRef& xref )
{
    ResMapIter itr( _resMap.find( res ) );
    if( itr == _resMap.end() )
        throw Class1Error( ERR1_NORES );
    itr->second.addXRef( xref );
}
/***************************************************************************/
void Document::addXRef( GlobalDictionaryWord* id, XRef& xref )
{
    NameMapIter itr( _nameMap.find( id ) );
    if( itr == _nameMap.end() )
        throw Class1Error( ERR1_NOID );
    itr->second.addXRef( xref );
}
/***************************************************************************/
void Document::addPage( Page* page )
{
    page->setIndex( static_cast< word >( _pages.size() ) );
    _pages.push_back( page );
}
/***************************************************************************/
void Document::addCell( Cell* cell )
{
    cell->setIndex( _cells.size() );
    _cells.push_back( cell );
}
/***************************************************************************/
void Document::makeIndexes()
{
    std::sort( _index.begin(), _index.end(), ptrLess< I1* >() );
    std::sort( _icmd.begin(), _icmd.end(), ptrLess< ICmd* >() );
    std::for_each( _pages.begin(), _pages.end(), std::mem_fun( &Page::buildIndex ) );
}
/***************************************************************************/
void Document::makeBitmaps()
{
    if( !_bitmapNames.empty() ) {
        //could use tmpfile...
        _tmpBitmaps = std::tmpfile();
        if( _tmpBitmaps == NULL )
            throw FatalIOError( ERR_OPEN, L"(temporary file for bitmaps)" );
        try {
            for( BitmapNameIter itr = _bitmapNames.begin(); itr != _bitmapNames.end(); ++itr ) {
                std::string fname;
                def_wtomb_string( itr->first, fname );
                for( std::size_t count = 0; count < ipfcartwork_paths.size(); ++count ) {
                    std::string fullname( ipfcartwork_paths[count] );
                    fullname += fname;
#if !defined( __UNIX__ ) && !defined( __APPLE__ )
                    if( fullname.size() > PATH_MAX ) {
                        throw FatalError( ERR_PATH_MAX );
                    }
#endif
                    try {
#ifdef CHECKCOMP
                        std::printf( "Processing bitmap %s\n", fullname.c_str() );
#endif
                        Bitmap bm( fullname );
                        itr->second = bm.write( _tmpBitmaps );
                        break;
                    }
                    catch( FatalError& e ) {
                        if( count == ipfcartwork_paths.size() - 1 ) {
                            throw FatalIOError( e.code, itr->first );
                        }
                    }
                    catch( Class1Error& e ) {
                        printError( e.code, itr->first );
                    }
                }
            }
        }
        catch( FatalError& e ) {
            std::fclose( _tmpBitmaps );
            _tmpBitmaps = NULL;
            throw e;
        }
        catch( FatalIOError& e ) {
            std::fclose( _tmpBitmaps );
            _tmpBitmaps = NULL;
            throw e;
        }
    }
}
/***************************************************************************/
STD1::uint32_t Document::writeBitmaps( std::FILE* out )
{
    dword offset = 0;
    if( !_bitmapNames.empty() && _tmpBitmaps != NULL ) {
        offset = std::ftell( out );
        std::fseek( _tmpBitmaps, 0L, SEEK_END );
        dword length;
        std::fseek( _tmpBitmaps, 0L, SEEK_SET );
        std::vector< byte > buffer( BUFSIZ );
        //copy the temporary file into this one
        try {
            for( length = std::ftell( _tmpBitmaps ); length > BUFSIZ; length -= BUFSIZ ) {
                if( std::fread( &buffer[0], sizeof( byte ), BUFSIZ, _tmpBitmaps ) != BUFSIZ )
                    throw FatalIOError( ERR_READ, L"(temporary file for bitmaps)" );
                if( std::fwrite( &buffer[0], sizeof( byte ), BUFSIZ, out ) != BUFSIZ ) {
                    throw FatalError( ERR_WRITE );
                }
            }
            if( length ) {
                if( std::fread( &buffer[0], sizeof( byte ), length, _tmpBitmaps ) != length )
                    throw FatalIOError( ERR_READ, L"(temporary file for bitmaps)" );
                if( std::fwrite( &buffer[0], sizeof( byte ), length, out ) != length ) {
                    throw FatalError( ERR_WRITE );
                }
            }
        }
        catch( FatalError& e ) {
            std::fclose( _tmpBitmaps );
            _tmpBitmaps = NULL;
            throw e;
        }
        catch( FatalIOError& e ) {
            std::fclose( _tmpBitmaps );
            _tmpBitmaps = NULL;
            throw e;
        }
        std::fclose( _tmpBitmaps );
        _tmpBitmaps = NULL;
    }
    return offset;
}
/***************************************************************************/
STD1::uint32_t Document::writeResMap( std::FILE* out )
{
    dword offset = 0;
    if( !_resMap.empty() ) {
        offset = std::ftell( out );
        ConstResMapIter itr;
        for( itr = _resMap.begin(); itr != _resMap.end(); ++itr ) {
            if( std::fwrite( &itr->first, sizeof( word ), 1, out ) != 1 ) {
                throw FatalError( ERR_WRITE );
            }
        }
        for( itr = _resMap.begin(); itr != _resMap.end(); ++itr ) {
            word idx( itr->second.index() );
            if( std::fwrite( &idx, sizeof( word ), 1, out ) != 1 ) {
                throw FatalError( ERR_WRITE );
            }
        }
    }
    return offset;
}
/***************************************************************************/
STD1::uint32_t Document::writeNameMap( std::FILE* out )
{
    dword offset = 0;
    if( !isInf() && !_nameMap.empty() ) {
        offset = std::ftell( out );
        ConstNameMapIter itr;
        for( itr = _nameMap.begin(); itr != _nameMap.end(); ++itr ) {
            word idx( itr->first->index() );
            if( std::fwrite( &idx, sizeof( word ), 1, out ) != 1 ) {
                throw FatalError( ERR_WRITE );
            }
        }
        for( itr = _nameMap.begin(); itr != _nameMap.end(); ++itr ) {
            word idx( itr->second.index() );
            if( std::fwrite( &idx, sizeof( word ), 1, out ) != 1 ) {
                throw FatalError( ERR_WRITE );
            }
        }
    }
    return offset;
}
/***************************************************************************/
STD1::uint32_t Document::writeTOCs( std::FILE* out )
{
    dword offset = std::ftell( out );
    _hdr->tocSize = 0;
    for( PageIter itr = _pages.begin(); itr != _pages.end(); ++itr ) {
        addTOCOffset( ( *itr )->write( out ) );
        _hdr->tocSize += ( *itr )->tocSize();
    }
    return offset;
}
/***************************************************************************/
STD1::uint32_t Document::writeTOCOffsets( std::FILE* out )
{
    dword offset = 0;
    if( !_tocOffsets.empty() ) {
        offset = std::ftell( out );
        if( std::fwrite( &_tocOffsets[0], sizeof( dword ),
          _tocOffsets.size(), out ) != _tocOffsets.size() ) {
            throw FatalError( ERR_WRITE );
        }
    }
    return offset;
}
/**************************************************************************/
void Document::writeCells( std::FILE* out )
{
    if( _cells.size() > UINT16_MAX )
        throw FatalError( ERR_LARGETOC );
    for( CellIter itr = _cells.begin(); itr != _cells.end(); ++itr ) {
        addCellOffset( (*itr)->write( out ) );
    }
}
/***************************************************************************/
STD1::uint32_t Document::writeCellOffsets( std::FILE* out )
{
    dword offset = 0;
    if( !_cellOffsets.empty() ) {
        offset = std::ftell( out );
        if( std::fwrite( &_cellOffsets[0], sizeof( dword ),
          _cellOffsets.size(), out ) != _cellOffsets.size() ) {
            throw FatalError( ERR_WRITE );
        }
    }
    return offset;
}
/***************************************************************************/
STD1::uint32_t Document::writeChildWindows( std::FILE* out )
{
    dword offset = std::ftell( out );
    _eHdr->childPagesSize = 0;
    for( PageIter itr = _pages.begin(); itr != _pages.end(); ++itr )
        _eHdr->childPagesSize += ( *itr )->writeChildren( out );
    return _eHdr->childPagesSize ? offset : 0;
}
/***************************************************************************/
void Document::writeSynonyms( std::FILE* out )
{
    for( SynIter itr = _synonyms.begin(); itr != _synonyms.end(); ++itr )
        itr->second->write( out );
}
/***************************************************************************/
STD1::uint32_t Document::writeIndex( std::FILE* out )
{
    dword offset = 0;
    _hdr->indexCount = 0;
    _hdr->indexSize = 0;
    _eHdr->gIndexCount = 0;
    if( !_index.empty() ) {
        offset = std::ftell( out );
        dword count = 0;
        dword gcount = 0;
        dword size = 0;
        for( IndexIter itr = _index.begin(); itr != _index.end(); ++itr ) {
            size += ( *itr )->write( out );
            count += ( *itr )->secondaryCount() + 1;
            if( ( *itr )->isGlobal() ) {
                gcount += 1;
            }
        }
        if( count > UINT16_MAX )
            throw FatalError( ERR_LARGEINDEX );
        _hdr->indexCount = static_cast< word >( count );
        _hdr->indexSize = size;
        _eHdr->gIndexCount = gcount;
    }
    return offset;
}
/***************************************************************************/
STD1::uint32_t Document::writeICmd( std::FILE* out )
{
    dword offset = 0;
    _hdr->icmdCount = 0;
    _hdr->icmdSize = 0;
    if( !_icmd.empty() ) {
        if( _icmd.size() > UINT16_MAX )
            throw FatalError( ERR_LARGEINDEX );
        offset = std::ftell( out );
        dword size = 0;
        for( ICmdIter itr = _icmd.begin(); itr != _icmd.end(); ++itr )
            size += ( *itr )->write( out );
        _hdr->icmdCount = static_cast< word >( _icmd.size() );
        _hdr->icmdSize = size;
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
void Document::parseCommand( Lexer* lexer, Tag* parent )
{
    if( lexer->cmdId() == Lexer::COMMENT ) {
        //do nothing
    } else if( lexer->cmdId() == Lexer::BREAK ) {
        parent->appendChild( new BrCmd( this, parent, dataName(), dataLine(), dataCol() ) );
    } else if( lexer->cmdId() == Lexer::CENTER ) {
        CeCmd* cecmd( new CeCmd( this, parent, dataName(), dataLine(), dataCol() ) );
        parent->appendChild( cecmd );
        cecmd->parseCommand( lexer );
    } else if( lexer->cmdId() == Lexer::IMBED ) {
        for( std::size_t count = 0; count < ipfcimbed_paths.size(); ++count ) {
            std::string sname;
            def_wtomb_string( lexer->text(), sname );
            std::string sfname( ipfcimbed_paths[count] + sname );
#if !defined( __UNIX__ ) && !defined( __APPLE__ )
            if( sfname.size() > PATH_MAX ) {
                throw FatalError( ERR_PATH_MAX );
            }
#endif
            std::wstring* wfname( new std::wstring() );
            def_mbtow_string( sfname, *wfname );
            try {
                pushFileInput( sfname, wfname );
                break;
            }
            catch( FatalError& e ) {
                delete wfname;
                if( count == ipfcimbed_paths.size() - 1 ) {
                    throw e;
                }
            }
            catch( FatalIOError& e ) {
                delete wfname;
                if( count == ipfcimbed_paths.size() - 1 ) {
                    throw e;
                }
            }
        }
    } else if( lexer->cmdId() == Lexer::NAMEIT ) {
        std::wstring::size_type idx1( lexer->text().find( L"symbol=" ) );
        std::wstring::size_type idx2( lexer->text().find( L' ', idx1 ) );
        std::wstring sym( lexer->text().substr( idx1 + 7, idx2 - idx1 - 7 ) );
        killQuotes( sym );
        sym.insert( sym.begin(), L'&' );
        sym += L'.';
        std::wstring::size_type idx3( lexer->text().find( L"text=" ) );
        //check for single quotes
        std::wstring::size_type idx4( lexer->text()[ idx3 + 5 ] == L'\'' ?
            lexer->text().find( L'\'', idx3  + 6 ) :
            lexer->text().find( L' ', idx3 + 5 ) );
        std::wstring txt( lexer->text().substr( idx3 + 5, idx4 - idx3 - 5 ) );
        killQuotes( txt );
        if( !_nls->isEntity( sym ) && _nameIts.find( sym ) == _nameIts.end() ) {   //add it to the list
            _nameIts.insert( std::map< std::wstring, std::wstring >::value_type( sym, txt ) );
        } else {
            printError( ERR3_DUPSYMBOL );
        }
    } else {
        printError( ERR1_CMDNOTDEF );
    }
}
/***************************************************************************/
//get a TOC index from the resource number to TOC index map
STD1::uint16_t Document::tocIndexByRes( word res )
{
    ResMapIter itr( _resMap.find( res ) );
    if( itr == _resMap.end() )
        throw Class1Error( ERR1_NORES );
    return itr->second.index();
}
/***************************************************************************/
//get a TOC index from the id or name to TOC index map
STD1::uint16_t Document::tocIndexById( GlobalDictionaryWord* id )
{
    NameMapIter itr( _nameMap.find( id ) );
    if( itr == _nameMap.end() )
        throw Class1Error( ERR1_NOID );
    return itr->second.index();
}
/***************************************************************************/
void Document::addSynonym( std::wstring& key, Synonym* value )
{
    if( _synonyms.find( key ) == _synonyms.end() ) {  //add it to the list
        _synonyms.insert( std::map< std::wstring, Synonym* >::value_type( key, value ) );
    } else {
        throw Class3Error( ERR3_DUPSYN );
    }
}
/***************************************************************************/
Synonym* Document::synonym( const std::wstring& key )
{
    SynIter itr( _synonyms.find( key ) );
    if( itr == _synonyms.end() )
        throw Class3Error( ERR3_NOSYN );
    return itr->second;
}
/***************************************************************************/
void Document::addIndexId( std::wstring& key, I1* value )
{
    if( _indexMap.find( key ) == _indexMap.end() ) {  //add it to the list
        _indexMap.insert( std::map< std::wstring, I1* >::value_type( key, value ) );
    } else {
        throw Class3Error( ERR3_DUPID );
    }
}
/***************************************************************************/
I1* Document::indexById( const std::wstring& key )
{
    IndexMapIter itr( _indexMap.find( key ) );
    if( itr == _indexMap.end() )
        throw Class1Error( ERR1_NOID );
    return itr->second;
}
/***************************************************************************/
const std::wstring* Document::nameit( const std::wstring& key )
{
    NameItIter itr( _nameIts.find( key ) );
    if( itr == _nameIts.end() )
        return 0;
    return &(itr->second);
}
/***************************************************************************/
std::wstring* Document::prepNameitName( const std::wstring& key )
{
    std::wstring* name( new std::wstring( L"Expansion of .nameit " ) );
    name->append( key );
    name = _compiler.addFileName( name );
    return name;
}
/***************************************************************************/
STD1::uint16_t Document::getGroupById( const std::wstring& i )
{
    ControlGroup* grp( _controls->getGroupById( i ) );
    if( !grp ) {
        _compiler.printError( ERR1_NOID, i );
        return 0;
    } else {
        return grp->index() + 1;
    }
}

std::wstring * Document::pushFileInput( std::wstring *wfname )
{
    IpfFile *ipff = new IpfFile( wfname );
    wfname = _compiler.addFileName( wfname );
    ipff->setName( wfname );
    _compiler.pushInput( ipff );
    return( wfname );
}

std::wstring * Document::pushFileInput( std::string& sfname, std::wstring *wfname )
{
    IpfFile *ipff = new IpfFile( sfname, wfname );
    wfname = _compiler.addFileName( wfname );
    ipff->setName( wfname );
    _compiler.pushInput( ipff );
    return( wfname );
}
