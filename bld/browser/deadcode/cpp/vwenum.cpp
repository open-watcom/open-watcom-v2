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


#include <wpshbttn.hpp>
#include <wlistbox.hpp>
#include <wcheckbx.hpp>
#include <wcombox.hpp>
#include <wtext.hpp>
#include <wstring.hpp>
#include <wvlist.hpp>
#include "wbrdefs.h"
#include "assure.h"
#include "vwcreate.h"
#include "vwenum.h"
#include "enumtype.h"
#include "opicklst.h"
#include "browse.h"

bool ViewEnum::_elementsOn=FALSE;

static char * styleOptions[] = {
    "Hex",
    "Octal",
    "Decimal",
    NULL
};

ViewEnum::ViewEnum()
    :_showElements(NULL)
    ,_elementsList(NULL)
    ,_elementDesc(NULL)
{
}

ViewEnum::ViewEnum( int x, int y, WBRWindow *prnt, const Symbol *info,
                    bool derived )
    :ViewSymbol( x, y, prnt, info, TRUE /*derived call*/ )
    ,_showElements(NULL)
    ,_elementsList(NULL)
    ,_elementDesc(NULL)
    ,_comboBoxFormatStyle(NULL)
    ,_projectModel(( (Browse *) prnt )->project() )
{
    if( !derived ) {
        refresh();
        setup();
    } // else derived class will call these
}

ViewEnum::~ViewEnum()
{
    _elements.deleteContents();
}

void ViewEnum::setup()
/********************/
{
    ulong startIndex = children().count();

    ViewSymbol::setup();

    /*
     * CheckBox for "Show Elements"
     */
    _showElements=new WCheckBox( this, WRect(viewLeft,0,140,viewHeight),
                                 "Show &Elements" );
//  _showElements->setFont(stdFont);
    _showElements->onClick( this, (cbw)&ViewEnum::showElements );
    _showElements->setCheck( _elementsOn );
    _showElements->show();

    showElements( NULL );
    arrangeKids( startIndex );
}

void ViewEnum::refresh()
/**********************/
{
    ViewSymbol::refresh();
    if( _elementsOn ) {
        loadElements();
    }
}

ViewSymbol * ViewEnum::create(int x, int y, WBRWindow *prnt, const Symbol *info)
/******************************************************************************/
{
    return new ViewEnum( x, y, prnt, info );
}

void ViewEnum::showElements( WWindow * )
/**************************************/
{
    long pushButtonIndex;

    showList( &_elementsList, &_elementDesc, _showElements,
              methodOf(ViewEnum,loadElements), _elementsOn,
              pickCallback(&ViewEnum::selectElement), NULL );

    if( _elementsOn ) {
        if((_comboBoxFormatStyle != NULL ) && _comboBoxFormatStyle->isHidden()){
            _pushbuttonMakeDefault->show();
            _comboBoxFormatStyle->show();
            pushButtonIndex = children().indexOfSame( _pushbuttonMakeDefault );
            arrangeKids( pushButtonIndex );
        } else if( _comboBoxFormatStyle == NULL ) {
            /*
             * "Make default" button
             */
            _pushbuttonMakeDefault = new WPushButton( this,
                                        WRect( -119,0,114,24 ),
                                        "Make Default" );
            _pushbuttonMakeDefault->onClick( this,
                                             (cbw) ViewEnum::makeDefault );
            _pushbuttonMakeDefault->show();


            /*
             * Create combobox for enum format style
             */
            EnumViewStyle selected;
            int i;

            selected = _projectModel->getEnumStyle();
            ((EnumType *) _model)->setStyle( selected );

            _comboBoxFormatStyle = new WComboBox( this,
                                                 WRect( viewLeft,0,95,85 ),
                                                 NULL );
            for( i = 0; styleOptions[ i ] != NULL; i += 1 ) {
                _comboBoxFormatStyle->insertString( styleOptions[ i ]);
            }
            _comboBoxFormatStyle->onChanged( this,
                                            (cbw) ViewEnum::selectValueStyle );
            _comboBoxFormatStyle->show();
            _comboBoxFormatStyle->select( selected - 2 );
            pushButtonIndex = children().indexOfSame( _pushbuttonMakeDefault );

            arrangeKids( pushButtonIndex );
        }
    } else if(( _comboBoxFormatStyle != NULL )
              && ( !_comboBoxFormatStyle->isHidden() )) {
        _pushbuttonMakeDefault->show( WWinStateHide );
        _comboBoxFormatStyle->show( WWinStateHide );
        pushButtonIndex = children().indexOfSame( _pushbuttonMakeDefault );
        arrangeKids( pushButtonIndex );
    }
}

void ViewEnum::loadElements()
/***************************/
{
    if( _elementsList ) {
        _elements.deleteContents();

        ((EnumType *)_model)->loadElements( _elements );

        _elementsList->setObjectList( _elements, (nameMethod)&EnumElement::name );
    }
}

void ViewEnum::selectElement( WObject * obj )
/*******************************************/
{
    EnumElement * elem = (EnumElement *) obj;
    WString buf( elem->name() );
    WString val;
    buf.concat( " = " );
    buf.concat( elem->value( val , ((EnumType *) _model )->getStyle() ));

    _elementDesc->setText( buf );
}

void ViewEnum::selectValueStyle( WWindow * win )
/************************************************/
{
    EnumViewStyle selected;

    selected = (EnumViewStyle) (( (WComboBox *) win )->selected() + 2 );

    ((EnumType *) _model)->setStyle( selected );

    /*
     * Force refresh of text box containing enum's value
     */
    _elementsList->chosen( _elementsList );
}

void ViewEnum::makeDefault( WWindow * )
/*************************************/
{
    EnumViewStyle selected;

    selected = (EnumViewStyle) ( _comboBoxFormatStyle->selected() + 2 );
    _projectModel->setEnumStyle( selected );
}
