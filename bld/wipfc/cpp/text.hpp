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
* Description:  An abstract text element (WHITESPACE, WORD, ENTITY, or PUNCTUATION)
*
****************************************************************************/

#ifndef TEXT_INCLUDED
#define TEXT_INCLUDED

#include "element.hpp"
#include "tag.hpp"

class GlobalDictionaryWord; //forward reference

class Text : public Element {
public:
    Text( Document* d, Element* p, const std::wstring* f, unsigned int r,
        unsigned int c, Tag::WsHandling w = Tag::NONE, bool ts = false) :
        Element( d, p, f, r, c ), _text( 0 ), _whiteSpace( w ),
        _toggleSpacing( ts ) { };
    Text( Document* d, Element* p, const std::wstring* f, unsigned int r,
        unsigned int c, const std::wstring& text, bool ts );
    ~Text() { };
    std::pair< bool, bool > buildLocalDict( Page* page );
    void buildText( Cell* cell );
    void setToggleSpacing() { _toggleSpacing = true; };
    void clearToggleSpacing() { _toggleSpacing = false; };
private:
    Text( const Text& rhs );            //no copy
    Text& operator=( const Text& rhs ); //no assignment

protected:
    GlobalDictionaryWord*   _text;
    Tag::WsHandling         _whiteSpace;
    bool                    _toggleSpacing;
};

#endif //TEXT_INCLUDED
