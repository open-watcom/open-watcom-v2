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


#ifndef __SCREENDV_H__
#define __SCREENDV_H__

#include "wbrdefs.h"
#include "outdev.h"

class ScreenDev : public OutputDevice
{
public:
                        ScreenDev() : _window(NULL) {}

            void        open( WWindow *w );
            void        close( void ) {}

    virtual int         getTextExtentX( const char * );
    virtual int         getTextExtentY( const char * );
    virtual void        rectangle( const WRect & );
    virtual void        drawText( const WPoint &, const char * );
    virtual void        drawText( const WPoint &, const char *, Color, Color );
    virtual void        moveTo( long x, long y ) {_currx = x; _curry = y;}
    virtual void        lineTo( long x, long y );
private:
    WWindow *           _window;
    WRect               _clipRect;
    long                _currx;
    long                _curry;
};

#endif
