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
* Description:  Table of Contents data
*
****************************************************************************/

#ifndef TOC_INCLUDED
#define TOC_INCLUDED

#include "config.hpp"
#include <cstdio>
#include <cstring>

#pragma pack(push, 1)

// TocEntry: located at offset pointed to by tocOffsetOffset[i]
// There is one entry per page, stored in the order in which
// they occur in the document.
struct TocEntry {
    STD1::uint8_t size;             // size of the entry
    STD1::uint8_t nestLevel  :4;    // nesting level
    STD1::uint8_t unknown    :1;   
    STD1::uint8_t extended   :1;    // extended entry format
    STD1::uint8_t hidden     :1;    // don't show this toc entry
    STD1::uint8_t hasChildren:1;    // following nodes are numerically higher
    STD1::uint8_t cellCount;        // number of Cells occupied by the text for this toc entry
    TocEntry() { std::memset( this, 0, sizeof( TocEntry ) ); };
    STD1::uint32_t write( std::FILE* out ) const;
    //variable length data follows:
    //if extended
    // ExtTocEntry + associated stuff
    //STD1::uint16_t  cellIndex[cellCount];
    //char            title[length - 3 - 2 * cellCount {- sizes of extended TOC components, if any}];
};

struct ExtTocEntry {
    STD1::uint16_t setPos  :1;      //PanelOrigin is present
    STD1::uint16_t setSize :1;      //PanelSize is present
    STD1::uint16_t setView :1;      //force new window
    STD1::uint16_t setStyle:1;      //PanelStyle is present
    STD1::uint16_t noSearch:1;
    STD1::uint16_t noPrint :1;
    STD1::uint16_t setCtrl :1;      //PanelControls is present
    STD1::uint16_t setTutor:1;
    STD1::uint16_t clear   :1;      //erase window
    STD1::uint16_t unknown1:1;
    STD1::uint16_t setGroup:1;      //PanelGroup is present
    STD1::uint16_t isParent:1;      //has child windows
    STD1::uint16_t unknown2:4;
    ExtTocEntry() { std::memset( this, 0, sizeof( ExtTocEntry ) ); };
    void write( std::FILE* out ) const;
    enum Position {
        ABSOLUTE_CHAR = 0,
        RELATIVE_PERCENT,
        ABSOLUTE_PIXEL,
        ABSOLUTE_POINTS,
        DYNAMIC
        };
    enum DynamicPosition {
        DYNAMIC_LEFT    = 1,
        DYNAMIC_RIGHT   = 2,
        DYNAMIC_TOP     = 4,
        DYNAMIC_BOTTOM  = 8,
        DYNAMIC_CENTER  = 16
        };
};

//on disk in this order
struct PageOrigin {
    STD1::uint8_t  yPosType:4;
    STD1::uint8_t  xPosType:4;
    STD1::uint16_t xpos;
    STD1::uint16_t ypos;
    PageOrigin() { std::memset( this, 0, sizeof( PageOrigin ) ); };
    void write( std::FILE* out ) const;
};

struct PageSize {
    STD1::uint8_t   widthType :4;
    STD1::uint8_t   heightType:4;
    STD1::uint16_t  width;
    STD1::uint16_t  height;
    PageSize() { std::memset( this, 0, sizeof( PageSize ) ); };
    void write( std::FILE* out ) const;
};

//titlebar, scrollbars, and rules
struct PageStyle {
    enum Style {
        BORDER      = 0x0004,   //?
        SIZEBORDER  = 0x0008,
        HSCROLL     = 0x0010,
        VSCROLL     = 0x0020,
        TITLEBAR    = 0x0100,   //?
        SYSMENU     = 0x0200,   //?
        MINMAX      = 0x0400,
        SPLIT       = 0x1000,   //???
        AUTOCLOSE   = 0x2000,   //?
        //0x4000 ?
        //0x8000 ?
    };
    STD1::uint16_t word;
    PageStyle() : word( 0 ) { };
    void write( std::FILE* out ) const;
};

struct PageGroup {
    STD1::uint16_t id;               //a panel number
    PageGroup() : id( 0 ) { };
    void write( std::FILE* out ) const;
};

//ctrlarea and ctrlrefid?
struct PageControls {
    STD1::uint16_t word;
    PageControls() : word( 0 ) { };
    void write( std::FILE* out ) const;
};

// TOCOffset
// unsigned long TOCOffset[IpfHeader.tocCount]

#pragma pack(pop)

#endif //TOC_INCLUDED
