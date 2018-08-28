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
* Description:  Hypergraphic hotspot data
*
****************************************************************************/


#ifndef HOTSPOT_INCLUDED
#define HOTSPOT_INCLUDED

struct Hotspot {                        //hypergraphic hotspot
    Hotspot( word x, word y, word cx, word cy ) {
        this->x = x;
        this->y = y;
        this->cx = cx;
        this->cy = cy;
    };
    bool isDef( bool flag ) { return( flag && (x || y || cx || cy) ); };
    std::size_t size( bool flag ) { return( ( isDef( flag ) ) ? 4 * sizeof( word ) : 0 ); };

    word                x;
    word                y;
    word                cx;
    word                cy;
};

#endif
