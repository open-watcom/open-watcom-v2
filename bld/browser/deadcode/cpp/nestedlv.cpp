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


#include <wstring.hpp>
#include <wpshbttn.hpp>

#include "assure.h"
#include "nestedlv.h"

#define nestedlvWidth   200
#define nestedlvHeight  300

Item::Item( void * pData, int numChildren, int indentation, char * pChildName )
        : _pData( pData )
        , _numChildren( numChildren )
        , _indentation( indentation )
        , _pChildName( pChildName )
        , _isExpanded( FALSE )
//-----------------------------------------------------------------------------
{
}

/*-------------------------------------------------------------------------
  The items displayed in the list box have a parallel data structure
  called _itemList which keeps track of :
        - a pointer to the data item associated with the list box item
        - whether or not the list box item has any children
        - whether or not the list box item's children are displayed
        - the number of spaces the item is indented by

  In the constructor, the roots are retrieved from the ForestListInterface
  and are stored in the parallel data structure _itemList and displayed
  in the list box.

  As list items are opened, their children are retrieved from
  ForestListInterface and added to the list box and _itemList.

  As list items are closed, their children are deleted from the two
  objects.
  -------------------------------------------------------------------------*/

NestedListView::NestedListView( int x, int y, char * title,
                                ForestListInterface * pFLI )
    : WBRWindow( WRect( x, y, nestedlvWidth, nestedlvHeight ), "" )
    , _listBox( new VerboseListBox( WRect( 5, 30, -5, -25 ), this ))
    , _fli( pFLI )
{
    setup( title );
}

NestedListView::~NestedListView()
//-------------------------------
{
    int i;

    for( i = 0; i < _itemList.count(); i += 1 ) {
        delete _itemList[ i ];
    }

    delete _fli;
}

void NestedListView::setup( char * title )
//----------------------------------------
{
    if( title != NULL ) {
        setText( title );
    }

    /*
     * Set up controls
     */
    _listBox->setCallbacks( this, NULL, (bcbwi) keyPressed,
                        (cbw) itemDblClicked );
    _listBox->select( 0 );
    _listBox->show();

    WPushButton * bttn;
    bttn = new WPushButton( this, WRect( 5, 2, 25, 25 ), "+" );
    bttn->onClick( this, (cbw) plusSelected );
    bttn->show();

    bttn = new WPushButton( this, WRect( 40, 2, 25, 25 ), "-" );
    bttn->onClick( this, (cbw) minusSelected );
    bttn->show();

    bttn = new WPushButton( this, WRect( 75, 2, 25, 25 ), "*" );
    bttn->onClick( this, (cbw) starSelected );
    bttn->show();

    /*
     * Get roots and display them
     */
    fillRoots();

    _listBox->setFocus();

    show();
}

void NestedListView::fillRoots()
//------------------------------
{
    int i;

    void * pRootData;
    char * pRootName;
    WString strTmp;
    int numChildren;

    for( i = 0; i < _fli->getNumChildren( NULL ); i += 1 ) {
        _fli->getChildData( NULL, i, pRootData, pRootName,
                       numChildren );
        if( pRootData != NULL ) {
            Item * pRootItem;

            pRootItem = new Item(  pRootData, numChildren, 0, pRootName );

            buildString( strTmp, pRootItem, FALSE );

            _listBox->insertString( strTmp.gets() );
            _itemList.add( pRootItem );
        }
    }
}

bool NestedListView::keyPressed( WObject *, int key )
//---------------------------------------------------
{
    if( key == WKeyPlus || key == WKeyMinus || key == WKeyAsterisk ){
        switch( (WKeyCode) key ) {
        case WKeyPlus:
            plusSelected( this );
            break;
        case WKeyAsterisk:
            starSelected( this );
            break;
        case WKeyMinus:
            minusSelected( this );
            break;
        default:
            break;
        }
        return TRUE;
    } else {
        return FALSE;
    }
}

void NestedListView::plusSelected( WObject * )
//--------------------------------------------
{
    int index = _listBox->selected();
    if( index >= 0 ) {
        expandNode( index );
        _listBox->select( index );
    }
}

void NestedListView::minusSelected( WObject * )
//---------------------------------------------
{
    int index = _listBox->selected();
    if( index >= 0 ) {
        collapseNode( index );
        _listBox->select( index );
    }
}

void NestedListView::starSelected( WObject * )
//--------------------------------------------
{
    int index = _listBox->selected();
    if( index >= 0 ) {
        expandAllNode( index );
        _listBox->select( index );
    }
}

void NestedListView::itemDblClicked( WObject * )
//----------------------------------------------
{
    int index = _listBox->selected();
    Item * pItem;
    pItem = (Item *) _itemList[ index ];

    if( pItem->_isExpanded ) {
        collapseNode( index );
    } else {
        expandNode( index );
    }
    _listBox->select( index );
}

void NestedListView::expandNode( int index )
//------------------------------------------
{
    Item * pItem;

    pItem = (Item *) _itemList[ index ];

    if( pItem == NULL ) {
        return;
    }
    if( pItem->_numChildren == 0 || pItem->_isExpanded ) {
        return;
    }

    char * pChildName;
    void * pChildData;
    int numChildren;
    int i;
    WString strTmp;

    for( i = 0; i < pItem->_numChildren; i += 1 ) {
        _fli->getChildData( pItem->_pData, i, pChildData, pChildName,
                            numChildren );
        if( pChildData != NULL ) {
            Item * pChildItem;

            pChildItem = new Item( pChildData, numChildren,
                                   pItem->_indentation + 1,
                                   pChildName );

            buildString( strTmp, pChildItem, FALSE );

            _listBox->insertString( strTmp.gets(), index + i + 1 );
            _itemList.insertAt( index + i + 1, pChildItem );
        }
    }

    pItem->_isExpanded = TRUE;

    buildString( strTmp, pItem, TRUE );

    _listBox->deleteString( index );
    _listBox->insertString( strTmp.gets(), index );
}

void NestedListView::buildString( WString & str, Item * pItem,
                                  bool isExpanded )
//------------------------------------------------------------
{
    str.printf( "" );

    if( pItem->_numChildren > 0 ) {
        if( isExpanded ) {
            str.concat( '-' );
        } else {
            str.concat( '+' );
        }
    } else {
        str.concat( ' ' );
    }

    for( int j = 0; j < pItem->_indentation; j += 1 ) {
        str.concat( ' ' );
    }

    str.concat( pItem->_pChildName );
}

void NestedListView::collapseNode( int index )
//--------------------------------------------
{
    REQUIRE( _itemList.count() == _listBox->count(), "Number of items != List box items" );

    int i;
    int maxIndent;
    int count;
    Item * pItem;
    Item * collapsedItem;
    WString strTmp;

    collapsedItem = _itemList[ index ];

    if( !collapsedItem->_isExpanded ) {
        return;
    }

    maxIndent = collapsedItem->_indentation;
    count = _listBox->count();

    for( i = index + 1; i < count; i += 1 ){
        if( _itemList[ index + 1 ]->_indentation > maxIndent ) {
            REQUIRE( (index+1) < _listBox->count(), "!!!" );
            _listBox->deleteString( index + 1 );
            pItem = _itemList.removeAt( index + 1 );
            delete pItem;
        } else {
            break;
        }
    }
    _listBox->deleteString( index );

    buildString( strTmp, collapsedItem, FALSE );

    _listBox->insertString( strTmp.gets(), index );
    collapsedItem->_isExpanded = FALSE;
}

void NestedListView::expandAllNode( int index )
//---------------------------------------------
{
    int minIndent;
    int i;

    if( _itemList[ index ]->_isExpanded ) {
        return;
    }

    minIndent = _itemList[ index ]->_indentation;

    if( _itemList[ index ]->_numChildren > 0 ) {
        expandNode( index );
    }

    /*
     * Note that the size of _itemList changes when calling expandNode.
     */
    for( i = index + 1;; i += 1 ) {

        if( i >= _itemList.count() ) {
            break;
        }

        if( _itemList[ i ]->_indentation <= minIndent ) {
            break;
        }

        if( _itemList[ i ]->_numChildren > 0 ) {
            expandNode( i );
        }
    }
}
