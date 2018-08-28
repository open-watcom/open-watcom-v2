/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Data processing output format
*
****************************************************************************/


#ifndef DATAENC_INCLUDED
#define DATAENC_INCLUDED

#include <vector>
#include <string>
#include "hotspot.hpp"


class DataEncode {
public:
    //is this data empty?
    bool empty() const { return _data.empty(); }
    //is the data block full?
    bool textFull() const {return _data.size() > 64000; };
    //reserve space in encoded text
    void reserve( std::size_t n ) { _data.reserve( _data.size() + n ); };
    //get current possition in encoded text
    std::size_t getPos( std::size_t start = 0 ) { return _data.size() - start; };
    //update byte at possition in encoded text
    void updateByte( std::size_t pos, byte data ) { _data[pos] = data; };
    //add a byte code to the encoded text
    void addByte( byte c ) { _data.push_back( c ); };
    void add( byte c ) { _data.push_back( c ); };
    //add a word code to the encoded text
    void add( word w ) {
        _data.push_back( static_cast< byte >( w ) );
        _data.push_back( static_cast< byte >( w >> 8 ) );
    };
    //add a dword code to the encoded text
    void add( dword dw ) {
        _data.push_back( static_cast< byte >( dw ) );
        _data.push_back( static_cast< byte >( dw >> 8 ) );
        _data.push_back( static_cast< byte >( dw >> 16 ) );
        _data.push_back( static_cast< byte >( dw >> 24 ) );
    };
    //add a byte array to the encoded text
    void add( const byte *array, std::size_t len ) {
        _data.reserve( _data.size() + len );
        for( std::size_t i = 0; i < len; i++ ) {
            _data.push_back( array[i] );
        }
    };
    //add a std::string to the encoded text
    void add( const std::string& text ) {
        std::size_t len = text.size();
        _data.reserve( _data.size() + len );
        for( std::size_t i = 0; i < len; i++ ) {
            _data.push_back( text[i] );
        }
    };
    //add a Hotspot to the encoded text
    void add( const Hotspot& hs ) {
        _data.push_back( static_cast< byte >( hs.x ) );
        _data.push_back( static_cast< byte >( hs.x >> 8 ) );
        _data.push_back( static_cast< byte >( hs.y ) );
        _data.push_back( static_cast< byte >( hs.y >> 8 ) );
        _data.push_back( static_cast< byte >( hs.cx ) );
        _data.push_back( static_cast< byte >( hs.cx >> 8 ) );
        _data.push_back( static_cast< byte >( hs.cy ) );
        _data.push_back( static_cast< byte >( hs.cy >> 8 ) );
    };

protected:
    std::vector< byte >     _data;              //encoded data, indexes into local dictionary
    typedef std::vector< byte >::iterator DataIter;
    typedef std::vector< byte >::const_iterator ConstDataIter;
};

#endif
