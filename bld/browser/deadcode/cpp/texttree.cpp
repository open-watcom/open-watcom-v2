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


#include <wlistbox.hpp>
#include <wtext.hpp>
#include <wkeydefs.hpp>
#include "assure.h"
#include "texttree.h"
#include "symbol.h"
#include "clstxttr.h"
#include "fnctxttr.h"
#include "mem.h"

enum {
    DOWN = 'd',
    UP = 'u',
    START = 's'
};

const int treeWidth = 336;
const int treeHeight = 201;
const int titleHeight = 18;

inline WRect & ParentRect( WRect &r, int w, int h, bool bottom );
inline WRect & HistoryRect( WRect &r, int w, int h, bool bottom );
inline WRect & KidRect( WRect &r, int w, int h, bool bottom );

inline WRect & ParentTitleRect( WRect &r, int w, int h );
inline WRect & HistoryTitleRect( WRect &r, int w, int h );
inline WRect & KidTitleRect( WRect &r, int w, int h );

inline WRect & ParentDescRect( WRect &r, int w, int h );
inline WRect & KidDescRect( WRect &r, int w, int h );

// a version of listbox with extra control over keys
//
class ListBoxKeyCtrl : public WListBox
{
    public:
        ListBoxKeyCtrl( WWindow* parent, const WRect& r )
            : WListBox( parent, r, LStyleDefault|LStyleNoIntegral ) {}

        bool        keyDown( WKeyCode, WKeyState );

        void        setFocii( WWindow * left, WWindow * right ) {
                        _focusLeft = left;
                        _focusRight = right;
                    }
    private:
        WWindow * _focusLeft;
        WWindow * _focusRight;
};

TextTree::TextTree()
    : WBRWindow( WRect(), NULL )
    , _historyList(NULL)
    , _parentList(NULL)
    , _kidList(NULL)
    , _historyTitle(NULL)
    , _parentTitle(NULL)
    , _kidTitle(NULL)
    , _currentHistoryItem(0)
{
}

TextTree::TextTree( int x, int y, char * prntListName,
                    char * kidListName, Symbol * sym, bool bottom )
    : WBRWindow( WRect(x,y,treeWidth,treeHeight), NULL )
    ,_destroyListContents(FALSE)
    ,_currentHistoryItem(0)
    ,_info( Symbol::defineSymbol(sym))
    ,_bottom( bottom )
{
    WRect r;
    getClientRect( r );
    int w = r.w();
    int h = r.h();

    #if 0   // ITB test styles -- TextStyleCentre broken?
    _parentTitle = new WText( this, ParentTitleRect( r, w, h ),
                              prntListName, TextStyleCentre );
    _parentTitle->show();

    _kidTitle = new WText( this, KidTitleRect( r, w, h ),
                           kidListName, TextStyleCentre );
    _kidTitle->show();

    _historyTitle = new WText( this, HistoryTitleRect( r, w, h ),
                               "Search Path", TextStyleLeftNoWrap );
    _historyTitle->show();
    #else
    _parentTitle = new WText( this, ParentTitleRect( r, w, h ),
                              prntListName, TextStyleLeftNoWrap );
    _parentTitle->show();

    _kidTitle = new WText( this, KidTitleRect( r, w, h ),
                           kidListName, TextStyleLeftNoWrap );
    _kidTitle->show();

    _historyTitle = new WText( this, HistoryTitleRect( r, w, h ),
                               "Search Path", TextStyleLeftNoWrap );
    _historyTitle->show();
    #endif

    if( _bottom ) {
        _parentDesc = new WText( this, ParentDescRect( r, w, h ),
                                  NULL, TextStyleLeftNoWrap|TextStyleAmpersands );
        _parentDesc->show();

        _kidDesc = new WText( this, KidDescRect( r, w, h ),
                               NULL, TextStyleLeftNoWrap|TextStyleAmpersands );
        _kidDesc->show();
    }

    _parentList = new ListBoxKeyCtrl( this, ParentRect( r, w, h, _bottom ) );
    _parentList->onChanged( this, (cbw)&TextTree::doSelected );
    _parentList->onDblClick( this, (cbw)&TextTree::parentOrKidSelected );

    _historyList = new ListBoxKeyCtrl( this, HistoryRect( r, w, h, _bottom ) );
    _historyList->onChanged( this, (cbw)&TextTree::historySelected );
    _historyList->onDblClick( this, (cbw)&TextTree::doDoubleClick );

    _kidList = new ListBoxKeyCtrl( this, KidRect( r, w, h, _bottom ) );
    _kidList->onChanged( this, (cbw)&TextTree::doSelected );
    _kidList->onDblClick( this, (cbw)&TextTree::parentOrKidSelected );

    _parentList->setFocii( NULL, _historyList );
    _historyList->setFocii( _parentList, _kidList );
    _kidList->setFocii( _historyList, NULL );

     _historyList->show();
     _parentList->show();
     _kidList->show();

     resized( w, h );
     show();
     _historyList->setFocus();
}

TextTree::~TextTree()
{
    // NYI -- detailview ViewCreator::viewDying( CreateInheritView, this );
    reset();
}


WBRWindow * TextTree::createView( int x, int y, Symbol * sym )
//------------------------------------------------------------
{
    dr_sym_type stype = sym->symtype();

    switch( stype ) {
    case DR_SYM_CLASS:
        return new ClassTextTree( x, y, sym );
    case DR_SYM_FUNCTION:
        return new FuncTextTree( x, y, sym );
    default:
        REQUIRE( 0, "TextTree::createView with invalid type" );
    }
    return NULL;
}

bool TextTree::isEqual( WObject * sym )
//-------------------------------------
{
    return _info->isEqual( sym );
}

void TextTree::reset()
//--------------------
{
    if( _destroyListContents ) {
        _parents.deleteContents();
        _kids.deleteContents();
        _history.deleteContents();
    } else {
        _history.reset();
        _parents.reset();
        _kids.reset();
    }
}

void TextTree::resized( WORD w, WORD h )
//--------------------------------------
{
    WRect r;

    _parentList->move( ParentRect( r, w, h, _bottom ) );
    _historyList->move( HistoryRect( r, w, h, _bottom ) );
    _kidList->move( KidRect( r, w, h, _bottom ) );

    _parentTitle->move( ParentTitleRect( r, w, h ) );
    _historyTitle->move( HistoryTitleRect( r, w, h ) );
    _kidTitle->move( KidTitleRect( r, w, h ) );

    if( _bottom ) {
        _parentDesc->move( ParentDescRect( r, w, h ) );
        _kidDesc->move( KidDescRect( r, w, h ) );
    }
}

void TextTree::loadMyObj( WObject & startObj, bool destroyListContents )
//----------------------------------------------------------------------
// if destroyListContents, I own results of getParents & getKids
{
    reset(); // cleans collections

    _destroyListContents = destroyListContents;
    _currentHistoryItem=0;
    _historyList->reset();
    _history.add( &startObj );
    _historyPath = "";
    _historyPath.concat( START );
    loadHistoryList();
    _historyList->select( 0 );
    loadParentKidLists( startObj );
}

void TextTree::loadParentKidLists( WObject & currentObj )
//-------------------------------------------------------
{
    char buff[MAX_DISP_NAME_LEN];

    if (_destroyListContents ) {
         _parents.deleteContents();
         _kids.deleteContents();
    } else {
        _parents.reset();
        _kids.reset();
    }
    _parentList->reset();
    _kidList->reset();
    getParents( currentObj, _parents );
    for( int i=0; i < _parents.count(); i++ ) {
        _parentList->insertString( getName( *_parents[i], buff ) );
    }
    getKids( currentObj, _kids );
    for( i=0; i < _kids.count(); i++ ) {
        _kidList->insertString( getName( *_kids[i], buff ) );
    }

    doSelected( _kidList );
    doSelected( _parentList );
}

void TextTree::loadHistoryList()
//------------------------------
{
    char buff[ MAX_DISP_NAME_LEN ];

    _historyList->reset();
    const char * path = _historyPath; // fixup path

    // find farthest left part of history
    int indent = 0;
    int baseIndent = 0;
    while( *path ) {
        switch( *path ) {
        case DOWN:
            ++indent;
            break;
        case UP:
            --indent;
            break;
        default:
            REQUIRE( *path == START, "TextTree::loadHistoryList() -- *path out of range!" );
        }
        if( indent < baseIndent ) baseIndent=indent;
        path++;
    }
    indent = (-1)*baseIndent;
    for( int i = 0; i < _history.count(); i++ ) {
        WObject * item = _history[i];
        switch( _historyPath[i] ) {
        case DOWN:
            ++indent;
            break;
        case UP:
            --indent;
            break;
        default:
            REQUIRE( _historyPath[ i ] == START, "TextTree::loadHistoryList() -- _historyPath[i] out of range!" );
        }
        WString histLine;
// NYI inefficient without this:        histLine.expandby( indent * 2 );
        for( int j=indent; j; j-- ) {
            histLine.concat( "  " );
        }
        histLine.concat( getName( *item, buff ) );
        _historyList->insertString( histLine );
    }
    _historyList->select( _currentHistoryItem );
}

/* rules for selection:
    parent selected: if parent is just next to currentHistoryItem in tree
            select that item in historyList, else clear list from current item
            on down, add parent to list
    child selected: as above
    history selected:
            just loadList
*/

void TextTree::parentOrKidSelected( WWindow * choice )
//----------------------------------------------------
{
    WListBox *list = (WListBox *) choice;
    bool kidPicked = (bool) (list == _kidList);
    int index = list->selected();
    WObject * newFocus = kidPicked
                                ? _kids.removeAt( index )
                                : _parents.removeAt( index );

    if( newFocus ) { // find if newFocus is next to current item
        bool historyRepeats = FALSE;
        if( (_currentHistoryItem > 0) &&
            newFocus->isEqual(_history[_currentHistoryItem - 1]) ) {
            // parent precedes current item in history
            --_currentHistoryItem;
            historyRepeats = TRUE;
        } else if( (_history.count() > (_currentHistoryItem + 1)) &&
                 (newFocus->isEqual(_history[_currentHistoryItem + 1])) ) {
            // parent follows current item in history
            ++_currentHistoryItem;
            historyRepeats = TRUE;
        }
        if( historyRepeats ) {
            _historyList->select( _currentHistoryItem );
            if( _destroyListContents ) {
                delete newFocus;
                newFocus = _history[ _currentHistoryItem ];
            }
        } else {
            // need to blow away history below current item, rebuild it
            for( int i = _history.count() - 1; i > _currentHistoryItem; i-- ) {
                WObject* item = _history.removeAt( i );
                REQUIRE( item != NULL, "Textree -- tried to remove past end!" );
                if( _destroyListContents ) {
                    delete item;
                }
            }
            _history.add( newFocus );
            _currentHistoryItem++;
            #if 0 // ITB DAGLO
            _historyPath.truncate( _currentHistoryItem - _historyPath.size() );
            #else
            _historyPath.truncate( _currentHistoryItem );
            #endif
            _historyPath.concat( kidPicked ? DOWN : UP );
            loadHistoryList();
        }
        loadParentKidLists( *newFocus );
    }
}

void TextTree::historySelected( WWindow * choice )
//------------------------------------------------
{
    int index = ((WListBox *)choice)->selected();
    WObject * newFocus = _history[index];
    if( newFocus ) {
        _currentHistoryItem = index;
        loadParentKidLists( *newFocus );
    }
}

WObject * TextTree::getHistoryObj( void )
//---------------------------------------
{
    return _history[ _currentHistoryItem ];
}

void TextTree::doubleClick( WObject * sym )
//-----------------------------------------
{
    popDetail( (const Symbol *) sym );
}

void TextTree::doDoubleClick( WWindow * )
//---------------------------------------
{
    doubleClick( _history[ _historyList->selected() ] );
}

char * TextTree::selected( WObject * )
//------------------------------------
{
    return NULL;
}

void TextTree::doSelected( WWindow * choice )
//-------------------------------------------
{
    char *      str;
    WListBox *  listbox = (WListBox *) choice;
    bool        kidPicked = (bool) (listbox == _kidList);
    WVList &    list = kidPicked ? _kids : _parents;
    int         index = listbox->selected();

    if( index < 0 ) {
        index = 0;
    }
    if( index >= list.count() ) {
        if( _bottom ) {
            if( kidPicked ) {
                _kidDesc->setText( NULL );
            } else {
                _parentDesc->setText( NULL );
            }
        }
        return;
    }

    if( _bottom ) {
        WObject * sel =  list[ index ];
        bool      freestr = TRUE;

        str = selected( sel );
        if( str == NULL ) {
            freestr = FALSE;
            str = "";
        }
        if( kidPicked ) {
            _kidDesc->setText( str );
        } else {
            _parentDesc->setText( str );
        }
        if( freestr ) {
            WBRFree( str );
        }
    }
}


bool TextTree::setMinimumSize( short *w, short *h )
//-------------------------------------------------
{
    *w = treeWidth;
    *h = treeHeight;
    return TRUE;
}

bool ListBoxKeyCtrl::keyDown( WKeyCode key, WKeyState state )
//-----------------------------------------------------------
{
    bool result = TRUE;

    switch( key ) {
    case WKeyLeft:
        if( _focusLeft ) {
            _focusLeft->setFocus();
        }
        break;
    case WKeyRight:
        if( _focusRight ) {
            _focusRight->setFocus();
        }
        break;
    default:
        result = WListBox::keyDown( key, state );
    }

    return result;
}

inline WRect & ParentRect( WRect &r, int w, int h, bool bottom )
//--------------------------------------------------------------
{
    r.x( 0 );
    r.y( titleHeight );
    r.w( w / 3 );

    int height = bottom ? h - 2 * titleHeight : h - titleHeight;
    r.h( height );

    return r;
}

inline WRect & HistoryRect( WRect &r, int w, int h, bool bottom )
//---------------------------------------------------------------
{
    r.x( w / 3 );
    r.y( titleHeight );
    r.w( w / 3 );

    int height = bottom ? h - 2 * titleHeight : h - titleHeight;
    r.h( height );

    return r;
}

inline WRect & KidRect( WRect &r, int w, int h, bool bottom )
//-----------------------------------------------------------
{
    r.x( 2 * ( w / 3 ) );
    r.y( titleHeight );
    r.w( w - r.x() );

    int height = bottom ? h - 2 * titleHeight : h - titleHeight;
    r.h( height );

    return r;
}

inline WRect & ParentTitleRect( WRect &r, int w, int )
//----------------------------------------------------
{
    r.x( 0 );
    r.y( 0 );
    r.w( w / 3 );
    r.h( titleHeight );

    return r;
}

inline WRect & HistoryTitleRect( WRect &r, int w, int )
//-----------------------------------------------------
{
    r.x( w / 3 );
    r.y( 0 );
    r.w( w / 3 );
    r.h( titleHeight );

    return r;
}

inline WRect & KidTitleRect( WRect &r, int w, int )
//-------------------------------------------------
{
    r.x( 2 * ( w / 3 ) );
    r.y( 0 );
    r.w( w - r.x() );
    r.h( titleHeight );

    return r;
}

inline WRect & ParentDescRect( WRect &r, int w, int h )
//-----------------------------------------------------
{
    r.x( 2 );
    r.y( h - titleHeight );
    r.w( w / 3 );
    r.h( titleHeight );

    return r;
}

inline WRect & KidDescRect( WRect &r, int w, int h )
//--------------------------------------------------
{
    r.x( 2 * ( w / 3 ) );
    r.y( h - titleHeight );
    r.w( w - r.x() - 2 );
    r.h( titleHeight );

    return r;
}

