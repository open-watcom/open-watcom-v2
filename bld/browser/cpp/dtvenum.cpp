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


#include <string.h>

#include <wstring.hpp>
#include <wlistbox.hpp>
#include <wvlist.hpp>

#include "dtvenum.h"
#include "enumtype.h"
#include "util.h"
#include "viewmgr.h"

DTViewEnum::DTViewEnum( const Symbol * sym )
                : DTViewSymbol( sym, TRUE )
//------------------------------------------
{
    _values = new WVList;
    _stl = optManager()->getEnumStyle();
    viewManager()->registerForEvents( this );
    setup();
}


DTViewEnum::~DTViewEnum()
//-----------------------
{
    _values->deleteContents();
    delete _values;
}

void DTViewEnum::setup()
//----------------------
{
    int             i;
    WString         str;
    WString         val;
    EnumType *      enumSym = (EnumType *) _symbol;
    EnumElement *   element;

    _values->deleteContents();
    _symbolBox->reset();

    enumSym->loadElements( *_values );

    for( i = 0; i < _values->count(); i += 1 ) {
        element = (EnumElement *) (*_values)[ i ];
        element->value( val );

        str.printf( "%s = %s", element->name(), val.gets() );
        _symbolBox->insertString( str.gets() );
    }
}

void DTViewEnum::event( ViewEvent ve, View * )
//--------------------------------------------
{
    switch( ve ) {
    case VEOptionValChange:
        if( _stl != optManager()->getEnumStyle() ) {
            _stl = optManager()->getEnumStyle();
            setup();
        }
        break;
    }
}

ViewEvent DTViewEnum::wantEvents()
//--------------------------------
{
    return VEOptionValChange;
}
