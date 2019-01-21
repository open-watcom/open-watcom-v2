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
* Description:  Process docprof tag
*   :docprof
*       toc=[1-6]+ (default: 123)
*       dll=''
*       objectname=''
*       objectinfo=''
*       ctrlarea=page|coverpage|both|none
*       Must be a child of :userdoc
* Must follow :title
*
****************************************************************************/


#include "wipfc.hpp"
#include "docprof.hpp"
#include "document.hpp"
#include "controls.hpp"
#include "strings.hpp"
#include "util.hpp"

Lexer::Token DocProf::parse( Lexer* lexer )
{
    Lexer::Token tok;

    while( (tok = _document->getNextToken()) != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"toc" ) {
                wchar_t ch[2];
                ch[0] = value[value.size() - 1];    //last number is critical value
                ch[1] = L'\0';
                int tmp( static_cast<int>( std::wcstol( ch, 0, 10 ) ) );
                if( tmp < 1 || tmp > 6 ) {
                    _document->printError( ERR2_VALUE );
                } else {
                    _headerCutOff = static_cast< unsigned int >( tmp );
                }
            } else if( key == L"objectname" ) {
                _objName = value;
            } else if( key == L"dll" ) {
                _dll = value;
            } else if( key == L"objectinfo" ) {
                _objInfo = value ;
            } else if( key == L"ctrlarea" ) {
                if( value == L"none" ) {
                    _area = NONE;
                } else if( value == L"coverpage" ) {
                    _area = COVERPAGE;
                } else if( value == L"page" ) {
                    _area = PAGE;
                } else if( value == L"both" ) {
                    _area = BOTH;
                } else {
                    _document->printError( ERR2_VALUE );
                }
            } else {
                _document->printError( ERR1_ATTRNOTDEF );
            }
        } else if( tok == Lexer::FLAG ) {
            _document->printError( ERR1_ATTRNOTDEF );
        } else if( tok == Lexer::END ) {
            throw FatalError( ERR_EOF );
        } else {
            _document->printError( ERR1_TAGSYNTAX );
        }
    }
    return _document->getNextToken();
}
/***************************************************************************/
void DocProf::build( Controls* controls, StringTable* strings )
{
    _document->setHeaderCutOff( _headerCutOff );
    if( _area == NONE || _area == PAGE ) {
        controls->setCover( 0xFFFF );
    } else {
        controls->setCover( 0 );   //may be modified by :ctrl. tag later
    }
    if( !_objName.empty() )
        strings->add( _objName );
    if( !_dll.empty() )
        strings->add( _dll );
    if( !_objInfo.empty() ) {
        strings->add( _objInfo );
    }
}
