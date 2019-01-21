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
* Description:  Search data for a single word in the Dictionary (vocabulary)
*               data
*
****************************************************************************/

#ifndef FTSELEMENT_INCLUDED
#define FTSELEMENT_INCLUDED

#include <cstdio>
#include <string>
#include <vector>


class OutFile;

class FTSElement {
public:
    FTSElement() : _pageCount( 0 ), _dataSize( 0 ), _firstPage( 0 ), _comp( NONE ) { };
    //set the number of pages on which the word might occur
    void setPages( std::size_t count );
    //the word appears on page i
    void onPage( std::size_t i );
    void build( OutFile* out );
    //Only valid after build is run
    bool isBigFTS() { return _dataSize + 2 > UINT8_MAX; };
    dword write( OutFile* out, bool big ) const;
private:
    std::size_t getPages( std::vector< word >& pg, bool absent ) const;
    FTSElement( const FTSElement& rhs );            //no copy
    FTSElement& operator=( const FTSElement& rhs ); //no assignment
    void encode( std::vector< byte >& rle );

    enum CompressionCode {
        NONE,                       // word is in no panel, no bitstring
        ALL,                        // word is in every panel, no bitsring
        RLE,                        // run length encoded bitstring
        PRESENT,                    // list of panel numbers (unsigned short) word is in
        ABSENT,                     // list of panel numbers (unsigned short) word is not in
        TRUNC,                      // no empty bytes after last set bit
        DBL_TRUNC                   // first panel number, then bitstring with no empty bytes
    };                              //   after last set bit
    std::size_t         _pageCount; // number of pages the word is in
    std::size_t         _maxPage;   // the highest page number
    std::size_t         _dataSize;  // the size of the compressed bitstring
    word                _firstPage; // the first page the word is in
    CompressionCode     _comp;
    std::vector< byte > _pages;     // bitstring of what pages its found on (for FTS)
    typedef std::vector< byte >::iterator PageIter;
    typedef std::vector< byte >::const_iterator ConstPageIter;
    typedef std::vector< byte >::reverse_iterator RPageIter;
    typedef std::vector< byte >::const_reverse_iterator ConstRPageIter;
};

#endif //FTSELEMENT_INCLUDED
