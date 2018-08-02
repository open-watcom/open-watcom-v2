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
* Description:  Process acviewport tag
*
*   :acviewport
*       dll=[a-zA-z][a-zA-z0-9]*
*       objectname=[a-zA-z][a-zA-z0-9]*
*       objectinfo=[a-zA-z][a-zA-z0-9]*
*       objectid=[a-zA-z][a-zA-z0-9]*
*       vpx=([0-9]+[c|x|p|%]) | (left|center|right)
*       vpy=([0-9]+[c|x|p|%]) | (top|center|bottom)
*       vpcx=([0-9]+[c|x|p|%])
*       vpcy=([0-9]+[c|x|p|%])
*
****************************************************************************/


#include "wipfc.hpp"
#include "acvwport.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "util.hpp"

Lexer::Token AcViewport::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    if( _objectId == 0 ) {
        std::wstring txt( L"objectid" );
        _document->printError( ERR2_VALUE, txt );
    }
    if( _objectName.empty() ) {
        std::wstring txt( L"objectname" );
        _document->printError( ERR2_VALUE, txt );
    }
    return tok;
}
/***************************************************************************/
Lexer::Token AcViewport::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok( _document->getNextToken() );
    bool xorg( false );
    bool yorg( false );
    bool dx( false );
    bool dy( false );
    while( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"dll" ) {
                _dll = value;
            } else if( key == L"objectname" ) {
                _objectName = value;
            } else if( key == L"objectinfo" ) {
                _objectInfo = value;
            } else if( key == L"objectid" ) {
                _objectId = static_cast< STD1::uint16_t >( std::wcstoul( value.c_str(), 0, 10 ) );
            } else if( key == L"vpx" ) {
                _doOrigin = true;
                xorg = true;
                if( value == L"left" ) {
                    _origin.xPosType = ExtTocEntry::DYNAMIC;
                    _origin.xpos = ExtTocEntry::DYNAMIC_LEFT;
                } else if( value == L"center" ) {
                    _origin.xPosType = ExtTocEntry::DYNAMIC;
                    _origin.xpos = ExtTocEntry::DYNAMIC_CENTER;
                } else if( value == L"right" ) {
                    _origin.xPosType = ExtTocEntry::DYNAMIC;
                    _origin.xpos = ExtTocEntry::DYNAMIC_RIGHT;
                } else if( value == L"top" || value == L"bottom" ) {
                    _document->printError( ERR2_VALUE );
                } else {
                    wchar_t *end;
                    unsigned long int x( std::wcstoul( value.c_str(), &end, 10 ) );
                    _origin.xpos = static_cast< STD1::uint16_t >( x );
                    if( *end == L'c' ) {
                        _origin.xPosType = ExtTocEntry::ABSOLUTE_CHAR;
                    } else if( *end == L'%' ) {
                        _origin.xPosType = ExtTocEntry::RELATIVE_PERCENT;
                    } else if( *end == L'x' ) {
                        _origin.xPosType = ExtTocEntry::ABSOLUTE_PIXEL;
                    } else if( *end == L'p' ) {
                        _origin.xPosType = ExtTocEntry::ABSOLUTE_POINTS;
                    } else {
                        _document->printError( ERR2_VALUE );
                    }
                }
                if( dx && _origin.xPosType == ExtTocEntry::DYNAMIC && _size.widthType != ExtTocEntry::RELATIVE_PERCENT ) {
                    _document->printError( ERR3_MIXEDUNITS );
                }
            } else if( key == L"vpy" ) {
                _doOrigin = true;
                yorg = true;
                if( value == L"top" ) {
                    _origin.yPosType = ExtTocEntry::DYNAMIC;
                    _origin.ypos = ExtTocEntry::DYNAMIC_TOP;
                } else if( value == L"center" ) {
                    _origin.yPosType = ExtTocEntry::DYNAMIC;
                    _origin.ypos = ExtTocEntry::DYNAMIC_CENTER;
                } else if( value == L"bottom" ) {
                    _origin.yPosType = ExtTocEntry::DYNAMIC;
                    _origin.ypos = ExtTocEntry::DYNAMIC_BOTTOM;
                } else if( value == L"left" || value == L"right" ) {
                    _document->printError( ERR2_VALUE );
                } else {
                    wchar_t *end;
                    unsigned long int y( std::wcstoul( value.c_str(), &end, 10 ) );
                    _origin.ypos = static_cast< STD1::uint16_t >( y );
                    if( *end == L'c' ) {
                        _origin.yPosType = ExtTocEntry::ABSOLUTE_CHAR;
                    } else if( *end == L'%' ) {
                        _origin.yPosType = ExtTocEntry::RELATIVE_PERCENT;
                    } else if( *end == L'x' ) {
                        _origin.yPosType = ExtTocEntry::ABSOLUTE_PIXEL;
                    } else if( *end == L'p' ) {
                        _origin.yPosType = ExtTocEntry::ABSOLUTE_POINTS;
                    } else {
                        _document->printError( ERR2_VALUE );
                    }
                }
                if( dy && _origin.yPosType == ExtTocEntry::DYNAMIC && _size.heightType != ExtTocEntry::RELATIVE_PERCENT )
                    _document->printError( ERR3_MIXEDUNITS );
            } else if( key == L"vpcx" ) {
                _doSize = true;
                dx = true;
                if( value == L"left" ||
                    value == L"center" ||
                    value == L"right" ||
                    value == L"top" ||
                    value == L"bottom" ) {
                    _document->printError( ERR2_VALUE );
                } else {
                    wchar_t *end;
                    unsigned long int width = std::wcstoul( value.c_str(), &end, 10 );
                    _size.width = static_cast< STD1::uint16_t >( width );
                    if( *end == L'c' ) {
                        _size.widthType = ExtTocEntry::ABSOLUTE_CHAR;
                    } else if( *end == L'%' ) {
                        _size.widthType = ExtTocEntry::RELATIVE_PERCENT;
                    } else if( *end == L'x' ) {
                        _size.widthType = ExtTocEntry::ABSOLUTE_PIXEL;
                    } else if( *end == L'p' ) {
                        _size.widthType = ExtTocEntry::ABSOLUTE_POINTS;
                    } else {
                        _document->printError( ERR2_VALUE );
                    }
                }
                if( xorg && _origin.xPosType == ExtTocEntry::DYNAMIC && _size.widthType != ExtTocEntry::RELATIVE_PERCENT )
                    _document->printError( ERR3_MIXEDUNITS );
            } else if( key == L"vpcy" ) {
                _doSize = true;
                dy = true;
                if( value == L"left" ||
                    value == L"center" ||
                    value == L"right" ||
                    value == L"top" ||
                    value == L"bottom" ) {
                    _document->printError( ERR2_VALUE );
                } else {
                    wchar_t *end;
                    unsigned long int height = std::wcstoul( value.c_str(), &end, 10 );
                    _size.height = static_cast< STD1::uint16_t >( height );
                    if( *end == L'c' ) {
                        _size.heightType = ExtTocEntry::ABSOLUTE_CHAR;
                    } else if( *end == L'%' ) {
                        _size.heightType = ExtTocEntry::RELATIVE_PERCENT;
                    } else if( *end == L'x' ) {
                        _size.heightType = ExtTocEntry::ABSOLUTE_PIXEL;
                    } else if( *end == L'p' ) {
                        _size.heightType = ExtTocEntry::ABSOLUTE_POINTS;
                    } else {
                        _document->printError( ERR2_VALUE );
                    }
                }
                if( yorg && _origin.yPosType == ExtTocEntry::DYNAMIC && _size.heightType != ExtTocEntry::RELATIVE_PERCENT )
                    _document->printError( ERR3_MIXEDUNITS );
            } else {
                _document->printError( ERR1_ATTRNOTDEF );
            }
        } else if( tok == Lexer::FLAG ) {
            _document->printError( ERR1_ATTRNOTDEF );
        } else if( tok == Lexer::ERROR_TAG ) {
            throw FatalError( ERR_SYNTAX );
        } else if( tok == Lexer::END ) {
            throw FatalError( ERR_EOF );
        }
        tok = _document->getNextToken();
    }
    return _document->getNextToken(); //consume TAGEND;
}
/***************************************************************************/
void AcViewport::buildText( Cell* cell )
{
    if( _objectId && !_objectName.empty() ) {
        std::vector< STD1::uint8_t > esc;
        esc.reserve( 3 + 4 + _objectName.size() + 1 + _dll.size() + 1 +
            _objectInfo.size() + 1 + 2 + sizeof( PageOrigin ) + sizeof( PageSize ) );
        esc.push_back( 0xFF );          //ESC
        esc.push_back( 2 );             //size
        esc.push_back( 0x21 );          //type
        esc.push_back( 0 );             //reserved
        esc.push_back( static_cast< STD1::uint8_t >( _objectName.size() + 1 +
            _dll.size() + 1 + _objectInfo.size() + 1 ) );
        esc.push_back( static_cast< STD1::uint8_t >( _objectId ) );
        esc.push_back( static_cast< STD1::uint8_t >( _objectId >> 8 ) );
        esc.push_back( static_cast< STD1::uint8_t >( _objectName.size() + 1 ) );
        if( !_objectName.empty() ) {
            std::string buffer;
            _document->wtomb_string( _objectName, buffer );
            std::size_t bytes( buffer.size() );
            for( std::size_t count1 = 0; count1 < bytes; ++count1 ) {
                esc.push_back( static_cast< STD1::uint8_t >( buffer[ count1 ] ) );
            }
        }
        esc.push_back( static_cast< STD1::uint8_t >( _dll.size() + 1 ) );
        if( !_dll.empty() ) {
            std::string buffer;
            _document->wtomb_string( _dll, buffer );
            std::size_t bytes( buffer.size() );
            for( std::size_t count1 = 0; count1 < bytes; ++count1 ) {
                esc.push_back( static_cast< STD1::uint8_t >( buffer[ count1 ] ) );
            }
        }
        esc.push_back( static_cast< STD1::uint8_t >( _objectInfo.size() + 1 ) );
        if( !_objectInfo.empty() ) {
            std::string buffer;
            _document->wtomb_string( _objectInfo, buffer );
            std::size_t bytes( buffer.size() );
            for( std::size_t count1 = 0; count1 < bytes; ++count1 ) {
                esc.push_back( static_cast< STD1::uint8_t >( buffer[ count1 ] ) );
            }
        }
        if( _doOrigin || _doSize ) {
            STD1::uint8_t flag( 0xC0 );
            if( _doOrigin )
                flag |= 0x01;
            if( _doSize )
                flag |= 0x02;
            esc.push_back( flag );
            esc.push_back( 0 );
            if( _doOrigin ) {
                STD1::uint8_t* src = reinterpret_cast< STD1::uint8_t* >( &_origin );
                for( std::size_t count1 = 0; count1 < sizeof( PageOrigin ); ++count1, ++src ) {
                    esc.push_back( *src );
                }
            }
            if( _doSize ) {
                STD1::uint8_t* src = reinterpret_cast< STD1::uint8_t* >( &_size );
                for( std::size_t count1 = 0; count1 < sizeof( PageSize ); ++count1, ++src ) {
                    esc.push_back( *src );
                }
            }
        }
        esc[ 1 ] = static_cast< STD1::uint8_t >( esc.size() - 1 );
        cell->addEsc( esc );
    }
}

