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
* Description:  An abstract text element (WHITESPACE, WORD, ENTITY,
*               or PUNCTUATION).
*
****************************************************************************/


#include "wipfc.hpp"
#include <cwctype>
#include "text.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "gdword.hpp"
#include "page.hpp"

Text::Text( Document* d, Element* p, const std::wstring* f, unsigned int r,
            unsigned int c, const std::wstring& text, bool ts ) :
            Element( d, p, f, r, c ), _whiteSpace( Tag::NONE), _toggleSpacing( ts )
{
    _text = _document->addTextToGD( new GlobalDictionaryWord( text ) );   //insert into global dictionary
}
/***************************************************************************/
std::pair< bool, bool > Text::buildLocalDict( Page* page )
{
    std::pair< bool, bool > retval( false, false );
    if( _text ) {
        retval.first = page->addTextToLD( _text );
        retval.second = _toggleSpacing;
    }
    return retval;
}
/***************************************************************************/
void Text::buildText( Cell* cell )
{
    if( _text ) {
        if( _toggleSpacing )
            cell->addByte( Cell::TOGGLE_SPACING );
        cell->add( cell->LDIndex( _text->index() ) );
        if( cell->textFull() ) {
            printError( ERR1_LARGEPAGE );
        }
    }
}
