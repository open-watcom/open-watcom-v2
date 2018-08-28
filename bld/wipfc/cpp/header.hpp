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
* Description:  Master and extended header
*
****************************************************************************/

#ifndef IPFHEADER_INCLUDED
#define IPFHEADER_INCLUDED

#include <cstdio>
#include <cstring>


class OutFile;

#define TITLE_SIZE      48

struct IpfHeader {
    IpfHeader();
    void write( OutFile* out ) const;
    bool isBigFTS();
    void setBigFTS( bool );
    std::size_t size() { return( ( 3 + 12 + TITLE_SIZE + 4 ) * sizeof( byte ) + 8 * sizeof( word ) + 18 * sizeof( dword ) ); };

    byte            id[3];              // "HSP"
    byte            flags;              // 0x01 if INF style file, 0x10 if HLP style file
    word            hdrsize;            // total size of header, in bytes
    byte            version_hi;
    byte            version_lo;
    word            tocCount;           // number of toc items
    dword           tocOffset;          // file offset to start of TocEntry elements
    dword           tocSize;            // size of all TocEntry data
    dword           tocOffsetOffset;    // file offset to array of file offsets to TocEntry
    word            panelCount;         // number of panels with resource numbers
    dword           panelOffset;        // file offset to panel number table
                                        // 2 consecutive arrays: panel number, TocEntry index
    word            nameCount;          // number of named panels
    dword           nameOffset;         // file offset to panel name table
                                        // 2 consecutive arrays: panel name index, TocEntry index
    word            indexCount;         // number of index entries
    dword           indexOffset;        // file offset to index table
    dword           indexSize;          // size of index table
    word            icmdCount;          // number of icmd index items
    dword           icmdOffset;         // file offset to icmd index items
    dword           icmdSize;           // size of icmd index table
    dword           searchOffset;       // file offset to full text search table; if high bit set, size of search record size is 16-bit
    dword           searchSize;         // size of full text search table
    word            cellCount;          // number of cells
    dword           cellOffsetOffset;   // file offset to array of file offsets to cells
    dword           dictSize;           // number of bytes occupied by the dictionary
    word            dictCount;          // number of entries in the dictionary
    dword           dictOffset;         // file offset array of DictString
                                        // string table is built from this
    dword           imageOffset;        // file offset of image data
    byte            maxLocalIndex;      // highest index inside panel's local dictionary,
    dword           nlsOffset;          // file offset to NLS table
    dword           nlsSize;            // size of NLS table
    dword           extOffset;          // file offset to extended data block
    byte            reserved[12];       // reserved for future use
    char            title[TITLE_SIZE];  // title of database
};

// Extended header info
struct IpfExtHeader {
    IpfExtHeader() { std::memset( this, 0, sizeof( IpfExtHeader) ); };
    dword write( OutFile* out ) const;
    std::size_t size() { return( 4 * sizeof( word ) + 14 * sizeof( dword ) ); };

    word            fontCount;          // number of font entries
    dword           fontOffset;         // file offset in file
    word            dbCount;            // number of external files
    dword           dbOffset;           // file offset of external files table
    dword           dbSize;             // size of external files table
    word            gNameCount;         // number of global panel name entries
    dword           gNameOffset;        // file offset of global panel names
    dword           stringsOffset;      // file offset of strings
    word            stringsSize;        // size of string data
    dword           childPagesOffset;   // file offset of child pages table
    dword           childPagesSize;     // size of child pages
    dword           gIndexCount;        // number of global index items
    dword           ctrlOffset;         // file offset to button control data
    dword           ctrlSize;           // size of button control data
    dword           reserved[4];        // reserved for future use
};

#endif //IPFHEADER_INCLUDED

