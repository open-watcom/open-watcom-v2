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
* Description:  Process docprof tag
*
****************************************************************************/

#include "docprof.hpp"
#include "document.hpp"
#include "controls.hpp"
#include "strings.hpp"
#include "util.hpp"

Lexer::Token DocProf::parse( Lexer* lexer )
{
    Lexer::Token tok( document->getNextToken() );
    while( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if ( key == L"toc" ) {
                wchar_t ch[ 2 ];
                ch[ 0 ] = value[ value.size() - 1 ];    //last number is critical value
                ch[ 1 ] = L'\0';
                int tmp( static_cast<int>( std::wcstol( ch, 0, 10 ) ) );
                if( tmp < 1 || tmp > 6 )
                    document->printError( ERR2_VALUE );
                else
                    headerCutOff = static_cast< unsigned int >( tmp );
            }
            else if( key == L"objectname" )
                objName = value;
            else if( key == L"dll" )
                dll = value;
            else if ( key == L"objectinfo" )
                objInfo = value ;
            else if( key == L"ctrlarea" ) {
                if( value == L"none" )
                    area = NONE;
                else if( value == L"coverpage" )
                    area = COVERPAGE;
                else if( value == L"page" )
                    area = PAGE;
                else if( value == L"both" )
                    area = BOTH;
                else
                    document->printError( ERR2_VALUE );
            }
            else
                document->printError( ERR1_ATTRNOTDEF );
        }
        else if ( tok == Lexer::FLAG )
            document->printError( ERR1_ATTRNOTDEF );
        else if( tok == Lexer::END )
            throw FatalError( ERR_EOF );
        else
            document->printError( ERR1_TAGSYNTAX );
        tok = document->getNextToken();
    }
    return document->getNextToken();
}
/***************************************************************************/
void DocProf::build( Controls* ctrls, StringTable* strs )
{
    document->setHeaderCutOff( headerCutOff );
    if( area == NONE || area == PAGE )
        ctrls->setCover( 0xFFFF );
    else
        ctrls->setCover( 0 );   //may be modified by :ctrl. tag later
    if( !objName.empty() )
        strs->addString( objName );
    if( !dll.empty() )
        strs->addString( dll );
    if( !objInfo.empty() )
        strs->addString( objInfo );
}
