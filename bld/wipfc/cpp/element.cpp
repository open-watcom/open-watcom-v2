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
* Description:  An abstract class for any element (tag, command, text)
*
****************************************************************************/


#include "wipfc.hpp"
#include "element.hpp"
#include "document.hpp"
#include "page.hpp"

void Element::printError( ErrCode c ) const
{
    _document->printError( c, _fileName, _row, _col );
}
/***************************************************************************/
void Element::printError( ErrCode c, const std::wstring& text ) const
{
    _document->printError( c, _fileName, _row, _col, text );
}
/***************************************************************************/
void Element::linearize( Page* page )
{
    page->addElement( this );
}
/***************************************************************************/
Element* Element::rootElement()
{
    Element* root( this );
    while( root->_parent ) //find root element
        root = root->_parent;
    return root;
}

