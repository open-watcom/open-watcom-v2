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
* Description:  Describes a font
*
****************************************************************************/

#ifndef FNT_INCLUDED
#define FNT_INCLUDED

#include "config.hpp"
#include <cstring>
#include <cstdio>

#pragma pack(push, 1)

// Font Entry
struct FontEntry {
    char            faceName[ 33 ]; //null terminated
    STD1::uint16_t  height;         //reversed from docs
    STD1::uint16_t  width;
    STD1::uint16_t  codePage;
    FontEntry() { std::memset( this, 0, sizeof( FontEntry ) ); };
    STD1::uint32_t write( std::FILE *out ) const;
    bool operator==( const FontEntry &rhs ) const
    {
        return std::strncmp(faceName, rhs.faceName, 33) == 0 &&
            height == rhs.height &&
            width == rhs.width &&
            codePage == rhs.codePage;
    };
};

#pragma pack(pop)

#endif //FNT_INCLUDED

