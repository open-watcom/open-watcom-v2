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
* Description:  Master and extended header
*
****************************************************************************/

#ifndef IPFHEADER_INCLUDED
#define IPFHEADER_INCLUDED

#include "config.hpp"
#include <cstdio>

#pragma pack(push, 1)

struct IpfHeader {
    STD1::uint8_t   id[ 3 ];            // "HSP"
    STD1::uint8_t   flags;              // 0x01 if INF style file, 0x10 if HLP style file
    STD1::uint16_t  size;               // total size of header, in bytes
    STD1::uint8_t   version_hi;
    STD1::uint8_t   version_lo;
    STD1::uint16_t  tocCount;           // number of toc items
    STD1::uint32_t  tocOffset;          // file offset to start of TocEntry elements
    STD1::uint32_t  tocSize;            // size of all TocEntry data
    STD1::uint32_t  tocOffsetOffset;    // file offset to array of file offsets to TocEntry
    STD1::uint16_t  panelCount;         // number of panels with resource numbers
    STD1::uint32_t  panelOffset;        // file offset to panel number table
                                        // 2 consecutive arrays: panel number, TocEntry index
    STD1::uint16_t  nameCount;          // number of named panels
    STD1::uint32_t  nameOffset;         // file offset to panel name table
                                        // 2 consecutive arrays: panel name index, TocEntry index
    STD1::uint16_t  indexCount;         // number of index entries
    STD1::uint32_t  indexOffset;        // file offset to index table
    STD1::uint32_t  indexSize;          // size of index table
    STD1::uint16_t  icmdCount;          // number of icmd index items
    STD1::uint32_t  icmdOffset;         // file offset to icmd index items
    STD1::uint32_t  icmdSize;           // size of icmd index table
    STD1::uint32_t  searchOffset:31;    // file offset to full text search table
    STD1::uint32_t  recSize:1;          // if high bit set, size of search record size is 16-bit
    STD1::uint32_t  searchSize;         // size of full text search table
    STD1::uint16_t  cellCount;          // number of cells
    STD1::uint32_t  cellOffsetOffset;   // file offset to array of file offsets to cells
    STD1::uint32_t  dictSize;           // number of bytes occupied by the dictionary
    STD1::uint16_t  dictCount;          // number of entries in the dictionary
    STD1::uint32_t  dictOffset;         // file offset array of DictString
                                        // string table is built from this
    STD1::uint32_t  imageOffset;        // file offset of image data
    STD1::uint8_t   maxLocalIndex;      // highest index inside panel's local dictionary,
    STD1::uint32_t  nlsOffset;          // file offset to NLS table
    STD1::uint32_t  nlsSize;            // size of NLS table
    STD1::uint32_t  extOffset;          // file offset to extended data block
    STD1::uint8_t   reserved[ 12 ];     // reserved for future use
    char            title[ 48 ];        // title of database
    IpfHeader();
    void write( std::FILE *out ) const;
};

// Extended header info
struct IpfExtHeader {
    STD1::uint16_t  fontCount;          // number of font entries
    STD1::uint32_t  fontOffset;         // file offset in file
    STD1::uint16_t  dbCount;            // number of external files
    STD1::uint32_t  dbOffset;           // file offset of external files table
    STD1::uint32_t  dbSize;             // size of external files table
    STD1::uint16_t  gNameCount;         // number of global panel name entries
    STD1::uint32_t  gNameOffset;        // file offset of global panel names
    STD1::uint32_t  stringsOffset;      // file offset of strings
    STD1::uint16_t  stringsSize;        // size of string data
    STD1::uint32_t  childPagesOffset;   // file offset of child pages table
    STD1::uint32_t  childPagesSize;     // size of child pages
    STD1::uint32_t  gIndexCount;        // number of global index items
    STD1::uint32_t  ctrlOffset;         // file offset to button control data
    STD1::uint32_t  ctrlSize;           // size of button control data
    STD1::uint32_t  reserved[ 4 ];      // reserved for future use
    IpfExtHeader() { std::memset( this, 0, sizeof( IpfExtHeader) ); };
    STD1::uint32_t write( std::FILE *out ) const;
};

#pragma pack(pop)

#endif //IPFHEADER_INCLUDED

