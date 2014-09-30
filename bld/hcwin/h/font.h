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
* Description:  Class to write the |FONT internal file in a .HLP file.
*
****************************************************************************/


#ifndef FONT_H
#define FONT_H

#include <watcom.h>     // for uint_8, etc.
#include "myfile.h"     // for class Dumpable, OutFile
#include "hlpdir.h"     // for class HFSDirectory


// Font attributes.

enum
{
    FNT_BOLD        = 0x01,
    FNT_ITALICS     = 0x02,
    FNT_UNDERLINE   = 0x04,
    FNT_STRIKEOUT   = 0x08,
    FNT_DBL_UNDER   = 0x10,
    FNT_SMALL_CAPS  = 0x20
};


// Font familes.

enum
{
    FNT_MODERN      = 0x01,
    FNT_ROMAN       = 0x02,
    FNT_SWISS       = 0x03,
    FNT_SCRIPT      = 0x04,
    FNT_DECORATIVE  = 0x05
};


//
//  FontName    --Linked-list node to store a font name.
//

class FontName
{
    char    *_name;
    FontName    *_nextName, *_prevName;
    short   _userNum;
    uint_16 _sysNum;
    uint_8  _family;

    // Assignment of FontName's is not allowed.
    FontName( FontName const & ) {};
    FontName &  operator=( FontName const & ) { return *this; };

    FontName( char const fontname[], short num1, uint_16 num2, uint_8 fam );
    ~FontName();

    int     dump( OutFile * dest );

    friend class HFFont;
};


//
//  FontDesc    --Linked-list node to store a font descriptor.
//

class FontDesc
{
    uint_8  _flags;
    uint_8  _halfPoints;
    uint_8  _family;
    uint_16 _index;
    uint_32 _rgb;
    uint_8  _supPos;
    uint_8  _subPos;
    FontDesc    *_nextDesc;
    FontDesc    *_prevDesc;

    FontDesc( uint_8 flgs, uint_8 psize, uint_8 fmily,
              uint_16 ind, uint_32 colour );
    FontDesc( FontDesc &other );

    int     dump( OutFile * dest );

    friend class HFFont;
};


//
//  FontState   --Node for a stack of states, used when compiling .RTF files.
//

class FontState
{
    uint_16 _index;
    FontState   *_next;

    FontState( unsigned short ind ) : _index(ind), _next(NULL) {};

    friend class HFFont;
};


//
//  HFFont  --Class to write the |FONT file.
//

class HFFont : public Dumpable
{
    uint_16 _numFonts;
    uint_16 _numDescs;
    uint_16 _defDesc;
    FontState   *_head;
    FontName    *_firstName;
    FontName    *_lastName;
    FontDesc    *_firstDesc;
    FontDesc    *_lastDesc;
    FontDesc    *_curDesc;
    uint_16 _curNum;

    uint_16 findDesc( FontDesc * wanted );

    // Assignment of HFFont's is not permitted.
    HFFont( HFFont const & ) {};
    HFFont &    operator=( HFFont const & ) { return *this; };

public:
    HFFont( HFSDirectory *d_file );
    ~HFFont();

    void    addFont( char const fontname[], uint_8 family, short num );

    // Clear the font numbers.  Does NOT erase the names, just
    // clears the user-assigned indices.
    void    clearFonts();

    void    setFont( uint_16 font );
    uint_16 selectFont( short index, int lnum, char const fname[] );
    uint_16 currentFont() { return _curNum; };

    uint_8  getAttribs( uint_16 font );
    uint_16 setAttribs( uint_8 attrs );
    uint_16 clearAttribs( uint_8 attrs );

    uint_16 newSize( uint_8 hpsize );
    uint_16 newSupPos( uint_8 pos );
    uint_16 newSubPos( uint_8 pos );
    void    push();
    uint_16 pop();

    uint_32 size();         // Overrides Dumpable::size
    int     dump( OutFile * dest ); // Overrides Dumpable::dump
};

#endif
