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


#ifndef __OUTDEV_H__
#define __OUTDEV_H__

#include <wcolour.hpp>
#include <wrect.hpp>
#include <wpoint.hpp>
#include "paintinf.h"

class OutputDevice
{
public:
                        OutputDevice() : _pinf(NULL), _background(ColorWhite){}

    virtual void        rectangle( const WRect & ) = 0;
    virtual void        drawText( const WPoint &, const char * ) = 0;
    virtual void        drawText( const WPoint &, const char *, Color, Color ) = 0;
    virtual void        moveTo( long x, long y ) = 0;
    virtual void        lineTo( long x, long y ) = 0;
    virtual int         getTextExtentX( const char * ) = 0;
    virtual int         getTextExtentY( const char * ) = 0;

    void        setPaintInfo( PaintInfo *p ) {_pinf = p;}
    PaintInfo * getPaintInfo( void ) {return _pinf;}
    void        setBackground( Color c ) {_background = c;}
    Color       getBackground( void ) {return _background;}

protected:
    PaintInfo * _pinf;
    Color       _background;
};

#endif
