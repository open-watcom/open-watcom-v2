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
* Description:  A collection of fonts
*
****************************************************************************/

#ifndef FNTCOL_INCLUDED
#define FNTCOL_INCLUDED

#include "config.hpp"
#include <cstdio>
#include <vector>
#include "fnt.hpp"
#include "errors.hpp"

class FontCollection
{
public:
    FontCollection( int cp );
    //returns the index of the font in the list
    size_t add( FontEntry& fnt );
    //the number of bytes written by the collection
    STD1::uint32_t length() { return bytes; };
    //the number of fonts in the collection
    size_t size() { return fonts.size(); };
    STD1::uint32_t write( std::FILE * out );
private:
    FontCollection( const FontCollection& rhs );            //no copy
    FontCollection& operator=( const FontCollection& rhs ); //no assigment
    std::vector< FontEntry > fonts;
    typedef std::vector< FontEntry >::iterator FontIter;
    typedef std::vector< FontEntry >::const_iterator ConstFontIter;
    STD1::uint32_t bytes;
};

#endif

