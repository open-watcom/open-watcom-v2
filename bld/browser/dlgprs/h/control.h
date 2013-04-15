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


#ifndef __CONTROL_H__

#include <string.h>

class Control {
public:
                                Control( const char * text, const char * id, Rect& r )
                                                : _text( text )
                                                , _id( id )
                                                , _rect( r ) {}
                                Control( const Control & o )
                                                : _text( o._text )
                                                , _id( o._id )
                                                , _rect( o._rect ) {}
                                Control()
                                                : _text( NULL )
                                                , _id( NULL )
                                                , _rect( Rect( 0, 0, 0, 0 ) ) {}
                                ~Control(){}

    bool                        operator <( const Control & o ) const {
                                    return( strcmp( _id, o._id ) < 0 );
                                }
    bool                        operator ==( const Control & o ) const {
                                    return( strcmp( _id, o._id ) == 0 );
                                }

    Control &                   operator =( const Control & o ) {
                                                _text = o._text;
                                                _id = o._id;
                                                _rect = o._rect;
                                    return *this;
                                }

    const char *                getText() const { return _text; }
    const char *                getId() const { return _id; }
    const Rect &                getRect() const { return _rect; }

private:
    const char *                _text;
    const char *                _id;
    Rect                        _rect;
};

#define __CONTROL_H__
#endif

