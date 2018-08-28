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
* Description:  Process i1 tags
*
*   :i1
*       id=[a-zA-z][a-zA-z0-9]*
*       global
*       roots='' (space separated list of words defined by :isyn)
*       sortkey='key-text'.index-text (place index-text where key-text would sort in index)
*   Cannot be child of :fn
*   Text must be on same line as tag
*
****************************************************************************/


#include "wipfc.hpp"
#include <algorithm>
#include "i1.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "gdword.hpp"
#include "ptrops.hpp"
#include "synonym.hpp"
#include "util.hpp"

I1::I1( Document* d, Element* p, const std::wstring* f, unsigned int r, unsigned int c ) :
    Element( d, p, f, r, c ), _primary( new IndexItem( IndexItem::PRIMARY ) ),
            _parentId( 0 ), _parentRes( 0 )
{
    _document->addIndex( this );
}
/*****************************************************************************/
Lexer::Token I1::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    std::wstring text;
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC)) {
        if( tok == Lexer::WORD ) {
            text += lexer->text();
        } else if( tok == Lexer::ENTITY ) {
            const std::wstring* exp( _document->nameit( lexer->text() ) );
            if( exp ) {
                text += *exp;
            } else {
                try {
                    wchar_t entityChar( _document->entityChar( lexer->text() ) );
                    text += entityChar;
                }
                catch( Class2Error& e ) {
                    _document->printError( e._code );
                }
            }
        } else if( tok == Lexer::PUNCTUATION ) {
            text += lexer->text();
        } else if( tok == Lexer::WHITESPACE ) {
            if( lexer->text()[0] == L'\n' ) {
                tok = _document->getNextToken();
                break;
            }
            text += lexer->text();
        } else {
            break;
        }
        tok = _document->getNextToken();
    }
    if( text.empty() ) {
        _document->printError( ERR2_INOTEXT );
    } else if( text.size() > 255 ) {
        _document->printError( ERR2_TEXTTOOLONG );
    }
    _primary->setText( text );
    return tok;
}
/*****************************************************************************/
Lexer::Token I1::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok;

    while( (tok = _document->getNextToken()) != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"id" ) {
                _id = value;
                try {
                    _document->addIndexId( _id, this );
                }
                catch( Class3Error& e ) {
                    _document->printError( e._code );
                }
            } else if( key == L"roots" ) {
                std::wstring::size_type idx1( 0 );
                std::wstring::size_type idx2( value.find( L' ' ) );
                while( idx1 != std::wstring::npos ) { //split value on ' '
                    _synRoots.push_back( value.substr( idx1, idx2 - idx1 ) );
                    idx1 = idx2 == std::wstring::npos ? std::wstring::npos : idx2 + 1;
                    idx2 = value.find( L' ', idx1 );
                }
            } else if( key == L"sortkey" ) {
                _primary->setSortKey( value );
            } else {
                _document->printError( ERR1_ATTRNOTDEF );
            }
        } else if( tok == Lexer::FLAG ) {
            if( lexer->text() == L"global" ) {
                if( !_document->isInf() )    //only for hlp files
                    _primary->setGlobal();
            } else {
                _document->printError( ERR1_ATTRNOTDEF );
            }
        } else if( tok == Lexer::ERROR_TAG ) {
            throw FatalError( ERR_SYNTAX );
        } else if( tok == Lexer::END ) {
            throw FatalError( ERR_EOF );
        } else {
            _document->printError( ERR1_TAGSYNTAX );
        }
    }
    return _document->getNextToken(); //consume TAGEND
}
/*****************************************************************************/
void I1::buildIndex()
{
    try {
        XRef xref( _fileName, _row );
        if( _parentRes ) {
            _primary->setTOC( _document->tocIndexByRes( _parentRes ) );
            _document->addXRef( _parentRes, xref );
        } else if( _parentId ) {
            _primary->setTOC( _document->tocIndexById( _parentId ) );
            _document->addXRef( _parentId, xref );
        }
    }
    catch( Class1Error& e ) {
        printError( e._code );
    }
}
/*****************************************************************************/
dword I1::write( OutFile* out )
{
    for( ConstSynIter itr = _synRoots.begin(); itr != _synRoots.end(); ++itr ) {
        //convert roots into offsets
        try {
            Synonym* syn( _document->synonym( *itr ) );
            _primary->addSynonym( syn->location() );
        }
        catch( Class3Error& e ) {
            printError( e._code );
        }
    }
    dword written = _primary->write( out );
    std::sort( _secondary.begin(), _secondary.end(), ptrLess< IndexItem* >() );
    for( IndexIter itr = _secondary.begin(); itr != _secondary.end(); ++itr )
        written += ( *itr )->write( out );
    return( written );
}
