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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef paintinfo_h_included
#define paintinfo_h_included

#include<wlines.hpp>
#include<wcolour.hpp>

class PaintInfo
{
public:
                PaintInfo();
                PaintInfo( Color, unsigned, WLineStyle );

    void        setStyle( WLineStyle s ) { _style = s; }
    void        setThickness( unsigned t ) { _thickness = t; }
    void        setColour( Color c ) { _colour = c;}
    WLineStyle  getStyle( void ) { return _style; }
    unsigned    getThickness( void ) { return _thickness; }
    Color       getColour( void ) { return _colour; }


/* NOTE: we use the default (bitwise) copy constructor in several places. if
 * this becomes invalid, make sure we define one!
*/
private:
    WLineStyle  _style;
    unsigned    _thickness;
    Color       _colour;
};

#endif
