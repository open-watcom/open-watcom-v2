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


#include <stdio.h>      // printf debugging

#include "dialog.h"
#include "control.h"

Dialog::Dialog( const char * name, Rect rect )
                : _name( name )
                , _rect( rect )
                , _caption( NULL )
                , _fontSize( 0 )
                , _fontName( NULL )
//--------------------------------------------
{
    _rectangles = new std::vector<Rect>;
    _controls = new std::vector<Control>;
}

Dialog::~Dialog()
//---------------
{
    delete _rectangles;
    _controls->clear();
    delete _controls;
}

int Dialog::addRect( const Rect & r )
//-----------------------------------
{
    _rectangles->push_back( r );
    return( _rectangles->size() - 1 );
}

void Dialog::setCaption( const char * text )
//------------------------------------------
{
    _caption = text;
}

void Dialog::setFont( int ptsize, const char * text )
//---------------------------------------------------
{
    _fontSize = ptsize;
    _fontName = text;
}

void Dialog::setStyle( long style )
//---------------------------------
{
    _style = style;
}

void Dialog::addControl( const char * text, const char * id, const char *, long, int rect )
//-----------------------------------------------------------------------------------------
{
    Rect &       r( (*_rectangles)[ rect ] );

    _controls->push_back( Control( text, id, r ) );
}

