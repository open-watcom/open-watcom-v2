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
* Description:  Table of Contents data
*
****************************************************************************/

#ifndef TOC_INCLUDED
#define TOC_INCLUDED

#include <cstdio>
#include <cstring>


class Cell;
class OutFile;

// TocEntry: located at offset pointed to by tocOffsetOffset[i]
// There is one entry per page, stored in the order in which
// they occur in the document.
struct TocEntry {
    TocEntry() { std::memset( this, 0, sizeof( TocEntry ) ); };
    dword write( OutFile* out ) const;
    void buildText( Cell *cell ) const;
    std::size_t size() const { return( 3 * sizeof( byte ) ); };

    struct _TocFlags {
        byte            nestLevel   :4;     // nesting level
        byte            unknown     :1;
        byte            extended    :1;     // extended entry format
        byte            hidden      :1;     // don't show this toc entry
        byte            hasChildren :1;     // following nodes are numerically higher
    };

    union TocFlags {
        _TocFlags       s;
        byte            data;
    };

    byte            hdrsize;            // size of the entry
    TocFlags        flags;
    byte            cellCount;          // number of Cells occupied by the text for this toc entry
    //variable length data follows:
    //if extended
    // ExtTocEntry + associated stuff
    //word            cellIndex[cellCount];
    //char            title[length - 3 - 2 * cellCount {- sizes of extended TOC components, if any}];
};

struct ExtTocEntry {
    ExtTocEntry() { std::memset( this, 0, sizeof( ExtTocEntry ) ); };
    void write( OutFile* out ) const;
    void buildText( Cell *cell ) const;
    std::size_t size() const { return( sizeof( word ) ); };

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

    struct _ExtTocFlags {
        word            setPos      :1;     //PanelOrigin is present
        word            setSize     :1;     //PanelSize is present
        word            setView     :1;     //force new window
        word            setStyle    :1;     //PanelStyle is present
        word            noSearch    :1;
        word            noPrint     :1;
        word            setCtrl     :1;     //PanelControls is present
        word            setTutor    :1;
        word            clear       :1;     //erase window
        word            unknown1    :1;
        word            setGroup    :1;     //PanelGroup is present
        word            isParent    :1;     //has child windows
        word            unknown2    :4;
    };

    union ExtTocFlags {
        _ExtTocFlags    s;
        word            data;
    };

    ExtTocFlags     flags;
};

//on disk in this order
struct PageOrigin {
    PageOrigin() { std::memset( this, 0, sizeof( PageOrigin ) ); };
    void write( OutFile* out ) const;
    void buildText( Cell *cell ) const;
    std::size_t size() const { return( sizeof( byte ) + 2 * sizeof( word ) ); };

    ExtTocEntry::Position   yPosType;
    ExtTocEntry::Position   xPosType;
    word                    xpos;
    word                    ypos;
};

struct PageSize {
    PageSize() { std::memset( this, 0, sizeof( PageSize ) ); };
    void write( OutFile* out ) const;
    void buildText( Cell *cell ) const;
    std::size_t size() const { return( sizeof( byte ) + 2 * sizeof( word ) ); };

    ExtTocEntry::Position   widthType;
    ExtTocEntry::Position   heightType;
    word                    width;
    word                    height;
};

//titlebar, scrollbars, and rules
struct PageStyle {
    PageStyle() : attrs( NONE ) { };
    void write( OutFile* out ) const;
    void buildText( Cell *cell ) const;
    std::size_t size() const { return( sizeof( word ) ); };

    enum Style {
        NONE        = 0,
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
    word            attrs;
};

struct PageGroup {
    PageGroup() : id( 0 ) { };
    void write( OutFile* out ) const;
    void buildText( Cell *cell ) const;
    std::size_t size() const { return( sizeof( word ) ); };

    word            id;             //a panel number
};

//ctrlarea and ctrlrefid?
struct PageControl {
    PageControl() : refid( 0 ) { };
    void write( OutFile* out ) const;
    void buildText( Cell *cell ) const;
    std::size_t size() const { return( sizeof( word ) ); };

    word            refid;
};

// TOCOffset
// unsigned long TOCOffset[IpfHeader.tocCount]

#endif //TOC_INCLUDED
