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
* Description:  Search data for a single word in the Dictionary (vocabulary)
*               data
*
****************************************************************************/

#ifndef FTSELEMENT_INCLUDED
#define FTSELEMENT_INCLUDED

#include "config.hpp"
#include <cstdio>
#include <string>
#include <vector>

class FTSElement {
public:
    FTSElement() : pageCount( 0 ), dataSize( 0 ), firstPage( 0 ), comp( NONE ) { };
    //set the number of pages on which the word might occur
    void setPages( size_t count );
    //the word appears on page i
    void onPage( size_t i );
    void build();
    //Only valid after build is run
    bool bigFTS() { return dataSize + 2 > UINT8_MAX; };
    size_t write( std::FILE* out, bool big ) const;
private:
    FTSElement( const FTSElement& rhs );            //no copy
    FTSElement& operator=( const FTSElement& rhs ); //no assignment
    enum CompressionCode {
        NONE,               //word is in no panel, no bitstring
        ALL,                //word is in every panel, no bitsring
        RLE,                //run length encoded bitstring
        PRESENT,            //list of panel numbers (unsigned short) word is in
        ABSENT,             //list of panel numbers (unsigned short) word is not in
        TRUNC,              //no empty bytes after last set bit
        DBL_TRUNC           //first panel number, then bitstring with no empty bytes
    };                      //  after last set bit
    size_t pageCount;       //number of pages the word is in
    size_t maxPage;         //the highest page number
    size_t dataSize;        //the size of the compressed bitstring
    STD1::uint16_t firstPage;   //the first page the word is in
    CompressionCode comp;
    std::vector< STD1::uint8_t > pages; //bitstring of what pages its found on (for FTS)
    typedef std::vector< STD1::uint8_t >::iterator PageIter;
    typedef std::vector< STD1::uint8_t >::const_iterator ConstPageIter;
    typedef std::vector< STD1::uint8_t >::reverse_iterator RPageIter;
    typedef std::vector< STD1::uint8_t >::const_reverse_iterator ConstRPageIter;
    void encode( std::vector< STD1::uint8_t >& rle );
};

#endif //FTSELEMENT_INCLUDED
