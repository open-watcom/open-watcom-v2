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

#include <algorithm>
#include "i1.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "gdword.hpp"
#include "ptrops.hpp"
#include "synonym.hpp"
#include "util.hpp"

I1::I1( Document* d, Element* p, const std::wstring* f, unsigned int r, unsigned int c ) :
    Element( d, p, f, r, c ), primary( new IndexItem( IndexItem::PRIMARY ) ),
            parentId( 0 ), parentRes( 0 )
{
    d->addIndex( this );
}
/*****************************************************************************/
Lexer::Token I1::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    std::wstring txt;
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC)) {
        if( tok == Lexer::WORD )
            txt += lexer->text();
        else if( tok == Lexer::ENTITY ) {
            const std::wstring* exp( document->nameit( lexer->text() ) );
            if( exp )
                txt += *exp;
            else {
                try {
                    wchar_t ch( document->entity( lexer->text() ) );
                    txt += ch;
                }
                catch( Class2Error& e ) {
                    document->printError( e.code );
                }
            }
        }
        else if( tok == Lexer::PUNCTUATION )
            txt += lexer->text();
        else if( tok == Lexer::WHITESPACE ) {
            if( lexer->text()[0] == L'\n' ) {
                tok = document->getNextToken();
                break;
            }
            txt+= lexer->text();
        }
        else
            break;
        tok = document->getNextToken();
    }
    if( txt.empty() )
        document->printError( ERR2_INOTEXT );
    else if( txt.size() > 255 )
        document->printError( ERR2_TEXTTOOLONG );
    primary->setText( txt );
    return tok;
}
/*****************************************************************************/
Lexer::Token I1::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok( document->getNextToken() );
    while( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"id" ) {
                id = value;
                try {
                    document->addIndexId( id, this );
                }
                catch( Class3Error& e ) {
                    document->printError( e.code );
                }
            }
            else if( key == L"roots" ) {
                std::wstring::size_type idx1( 0 );
                std::wstring::size_type idx2( value.find( L' ' ) );
                while( idx1 != std::wstring::npos ) { //split value on ' '
                    synRoots.push_back( value.substr( idx1, idx2 - idx1 ) );
                    idx1 = idx2 == std::wstring::npos ? std::wstring::npos : idx2 + 1;
                    idx2 = value.find( L' ', idx1 );
                }
            }
            else if( key == L"sortkey" )
                primary->setSortKey( value );
            else
                document->printError( ERR1_ATTRNOTDEF );
        }
        else if( tok == Lexer::FLAG ) {
            if( lexer->text() == L"global" ) {
                if( !document->isInf() )    //only for hlp files
                    primary->setGlobal();
            }
            else
                document->printError( ERR1_ATTRNOTDEF );
        }
        else if( tok == Lexer::ERROR_TAG )
            throw FatalError( ERR_SYNTAX );
        else if( tok == Lexer::END )
            throw FatalError( ERR_EOF );
        else
            document->printError( ERR1_TAGSYNTAX );
        tok = document->getNextToken();
    }
    return document->getNextToken(); //consume TAGEND
}
/*****************************************************************************/
void I1::buildIndex()
{
    try {
        XRef xref( fileName, row );
        if( parentRes ) {
            primary->setTOC( document->tocIndexByRes( parentRes ) );
            document->addXRef( parentRes, xref );
        }
        else if( parentId ) {
            primary->setTOC( document->tocIndexById( parentId ) );
            document->addXRef( parentId, xref );
        }
    }
    catch( Class1Error& e ) {
        printError( e.code );
    }
}
/*****************************************************************************/
size_t I1::write( std::FILE* out )
{
    for( ConstSynIter itr = synRoots.begin(); itr != synRoots.end(); ++itr ) {
        //convert roots into offsets
        try {
            Synonym* syn( document->synonym( *itr ) );
            primary->addSynonym( syn->location() );
        }
        catch( Class1Error& e ) {
            printError( e.code );
        }
    }
    size_t written( primary->write( out ) );
    std::sort( secondary.begin(), secondary.end(), ptrLess< IndexItem* >() );
    for( IndexIter itr = secondary.begin(); itr != secondary.end(); ++itr )
        written += ( *itr )->write( out );
    return written;
}
