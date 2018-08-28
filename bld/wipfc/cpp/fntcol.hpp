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
* Description:  A collection of fonts
*
****************************************************************************/

#ifndef FNTCOL_INCLUDED
#define FNTCOL_INCLUDED

#include <cstdio>
#include <vector>
#include "fnt.hpp"
#include "errors.hpp"


#define MAX_FONTS       14

class FontCollection
{
public:
    FontCollection( word maxFontCount );
    //returns the index of the font in the list
    std::size_t add( const FontEntry& fnt );
    //the number of bytes written by the collection
    dword length() { return _bytes; };
    //the number of fonts in the collection
    std::size_t size() { return _fonts.size(); };
    dword write( OutFile* out );
private:
    FontCollection( const FontCollection& rhs );            //no copy
    FontCollection& operator=( const FontCollection& rhs ); //no assigment

    std::vector< FontEntry >    _fonts;
    typedef std::vector< FontEntry >::iterator FontIter;
    typedef std::vector< FontEntry >::const_iterator ConstFontIter;
    dword                       _bytes;
    word                        _maxFontCount;
};

#endif

