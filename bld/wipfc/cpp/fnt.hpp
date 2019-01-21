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
* Description:  Describes a font
*
****************************************************************************/

#ifndef FNT_INCLUDED
#define FNT_INCLUDED

#include <cstdio>
#include <string>


#define MAX_FACENAME_SIZE   33
#define DEFAULT_CODEPAGE    static_cast< word >( -1 )

class OutFile;      // forward reference

// Font Entry
struct FontEntry {
private:
public:
    FontEntry() : _faceName(), _width( 0 ), _height( 0 ), _codePage( DEFAULT_CODEPAGE ) { };
    FontEntry( const std::wstring& faceName, word width, word height, word codePage = DEFAULT_CODEPAGE )
        : _faceName( faceName ), _width( width ), _height( height ), _codePage( codePage ) {}
    void setFaceName( const std::wstring& faceName ) { _faceName = faceName; }
    void setHeight( word height ) { _height = height; }
    void setWidth( word width ) { _width = width; }
    void setCodePage( word codePage ) { _codePage = codePage; }
    dword write( OutFile* out ) const;
    bool operator==( const FontEntry &rhs ) const;

private:
    std::wstring    _faceName;
    word            _width;
    word            _height;        //reversed from docs
    word            _codePage;
};

#endif //FNT_INCLUDED
