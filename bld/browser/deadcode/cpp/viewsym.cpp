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


#include <wtext.hpp>
#include <wpshbttn.hpp>
#include <wcheckbx.hpp>

#include "assure.h"
#include "browse.h"
#include "descrip.h"
#include "opicklst.h"
#include "symbol.h"
#include "referenc.h"
#include "vwcreate.h"
#include "viewsym.h"
#include "vwfunc.h"
#include "vwenum.h"
#include "vwclass.h"
#include "vwmacro.h"
#include "infobttn.h"
#include "editsess.h"
#include "mem.h"

enum {
    vertGap = 3,
    editStart = 38,
    minViewWidth = 260
};

bool ViewSymbol::_usersOn=FALSE;

createFn ViewSymbol::_createTbl[] = {
                (createFn) &ViewFunction::create,
                (createFn) &ViewClassType::create,
                (createFn) &ViewEnum::create,
                (createFn) &ViewSymbol::create,
                (createFn) &ViewSymbol::create,
                (createFn) &ViewSymbol::create,
                (createFn) &ViewMacro::create,
                (createFn) &ViewSymbol::create
                };

const char * StringDefinedIn = "Defined In:";
const char * StringNotDefined = "[ Not Defined ]";

ViewSymbol::ViewSymbol()
    :_showUsers(NULL)
    ,_usersList(NULL)
    ,_minWidth(0)
    ,_minHeight(0)
{
}

ViewSymbol::ViewSymbol( int x, int y, WBRWindow * prnt, const Symbol * info,
                        bool derived )
    :WBRWindow( prnt, WRect(x,y,0,0), "" )
    ,_showUsers(NULL)
    ,_usersList(NULL)
    ,_isderived(derived)
    ,_minWidth(0)
    ,_minHeight(0)
{
    _model = Symbol::defineSymbol( info ); // my own copy
    if( !_isderived ) {
        refresh();
        setup();
    } // else derived classes will call my setup if they want, and refresh
}

ViewSymbol::~ViewSymbol()
{
    ViewCreator::viewDying( CreateDetailView, this );
    if( _model ) {
        delete _model;
    }
    _users.deleteContents();
}

bool ViewSymbol::isEqual( WObject * o )
/*************************************/
//if equivalent Symbol; also matches Symbol
{
    if( o ) {
        return *_model == *((Symbol *)o);
    }
    return FALSE;
}

void ViewSymbol::setup()
/**********************/
{
    WVList & kids = children();
    ulong startIndex = kids.count();

    WPushButton * edit;
    WText * text;
    int w,h;

    w = getTextExtentX( StringDefinedIn );
    text = new WText( this, WRect( viewLeft, editStart, w, 20 ), (char *) StringDefinedIn,
                      TextStyleLeftNoWrap|TextStyleAmpersands);
    text->show();

    if( _model->isDefined() ) {
        char * temp;

        temp = _model->defSourceFile();
        if( temp == NULL ) {
            temp = "Unknown File";
        }
        w = getTextExtentX( temp );
        h = getTextExtentY( temp );
        text = new WText( this, WRect( viewLeft, 0, w, 20 ), temp,
                          TextStyleLeftNoWrap|TextStyleAmpersands);
        text->show();

    } else {
        w = getTextExtentX( StringNotDefined );
        h = getTextExtentY( StringNotDefined );
        text = new WText( this, WRect( viewLeft, 0, w, 20 ), (char *) StringNotDefined,
                          TextStyleLeftNoWrap|TextStyleAmpersands);
        text->show();

    }
    edit = new WPushButton( this, WRect( viewLeft, 0, 130, viewHeight ),
                            "&Edit Definition");
//  edit->setFont(stdFont);
    edit->onClick( this, (cbw)&ViewSymbol::editDefinition );
    if( _model->isDefined() ) {
        edit->show();
    } else {
        edit->show( WWinStateHide );
    }

    _showUsers = new WCheckBox( this, WRect(viewLeft,editStart,100,viewHeight),
                                "&List Users" );
//  _showUsers->setFont(stdFont);
    _showUsers->onClick( this, (cbw)&ViewSymbol::showUsers );
    _showUsers->setCheck( _usersOn );
    _showUsers->show();
    showUsers( this );
    show();
    if( !_isderived ) {
        arrangeKids( startIndex );
    }
}

WBRWindow * ViewSymbol::createView( int x, int y, WBRWindow *parent,
                                    const Symbol *info )
/*******************************************************************/
{
    return (_createTbl[ info->symtype() ])( x, y, parent, info );
}

WBRWindow * ViewSymbol::create( int x, int y, WBRWindow *parent,
                                const Symbol *info )
/***************************************************************/
{
    return new ViewSymbol( x, y, parent, info );
}

void ViewSymbol::refresh()
/************************/
{
    erase();
    char * name = _model->scopedName();

    if( name != NULL ) {
        setText( name );
    } else {
        setText( "scoped name" );
    }
    displayDescription();
    if( _usersOn ) {
        loadUsers();
    }
}

void ViewSymbol::displayDescription()
/***********************************/
{
    char          namebuf[ MAX_DISP_NAME_LEN ];
    int           x;
    int           y;
    int           w;
    int           h;
    InfoButton *  userType = NULL;
    WText *       fundType = NULL;
    WString       buf;
    Description * entry;

    WVList desc;
    _model->description( desc );

    x = y = 5;
    buf = "";
    for( int i = 0; i < desc.count(); i++ ) {
        entry = (Description *) desc[i];
        if( entry->_isUserDefined ) {
            if( _model->isEqual( entry->u.sym ) ) {

                // don't put the symbol we're describing in a button
                buf.concat( entry->u.sym->copyBaseName( namebuf ) );

                delete entry->u.sym;

            } else {

                if( buf != "" ) { // flush buf
                    w = getTextExtentX( buf );
                    h = getTextExtentY( buf );
                    fundType = new WText( this, WRect(x,y+4,w,h), buf,
                                          TextStyleLeftNoWrap|TextStyleAmpersands);
    //              fundType->setFont(courierFont);
                    fundType->show();
                    buf="";
                    x+=w;
                }
                userType = new InfoButton( entry->u.sym, this, x, y, w, h );
                userType->setInfoRqstCB( (InfoRqstCB)&ViewSymbol::popSymbolDetail,
                                         this );
                x+=w;
            }
        } else if( entry->_nameGoesHere ) {
            buf.concat( entry->u.text );
        }
    }
    desc.deleteContents();
    if( buf != "" ) { // flush buf
        w = getTextExtentX( buf );
        h = getTextExtentY( buf );
        fundType = new WText( this, WRect(x,y+4,w,h), buf,
                                          TextStyleLeftNoWrap|TextStyleAmpersands);
//      fundType->setFont(courierFont);
        fundType->show();
        buf="";
        x+=w + 7;
    }
    x += 5;
    if( x < minViewWidth ) {
        x = minViewWidth;
    }
    _minWidth = x;
    WRect rect;
    getRectangle( rect, TRUE );
    rect.w( x );
    move( rect );
}

void ViewSymbol::popSymbolDetail( WObject * info )
/***********************************************/
{
    if( info ) {
        static int x = 200;
        static int y = 200;
        x+=20;
        y+=20;
        if( x > 350 ) {
            x=200;
            y=200;
        }
        ViewCreator::findOrCreate( CreateDetailView, x, y, browseTop,
                                   (Symbol *) info );
    }
}

void ViewSymbol::popUserReference( WObject * obj )
/************************************************/
{
    Reference *ref = (Reference *) obj;
    if( ref ) {
        char * file = ref->sourceFile();
        if( file ) {
            browseTop->positionEditor( file, ref->line(), ref->column() );

            #if DEBUG
            // NYI - to be removed -- just for testing.
            new EditSession( this, 200, 200, ref );
            #endif
        }
    }
}

void ViewSymbol::arrangeKids( int startIndex )
/********************************************/
// start index is first child that was changed or created
// adjust children to account for hides, shows
// could later handle scaling of list boxes for resizes
// maybe even Sash-style re-arrangements
{
    WVList & kids = children();
    if( !kids.count() ) return;
    int lastKid = kids.count() - 1;
    if( startIndex > lastKid ) return;

    WRect rect;
    WWindow * child = NULL;
    int endY = 0;

    // establish y offset from last visible child before startIndex
    if( startIndex ) {
        int firstVisibleChild = startIndex-1;
        for(;;) {
            if( firstVisibleChild < 0 ) break;
            child = (WWindow *) kids[firstVisibleChild];
            if( child == NULL ) break;
            if( child->isVisible() ) {
                child->getRectangle( rect );
                if( rect.y() + rect.h() > endY ) {
                    endY = rect.y() + rect.h();
                }
            }
            --firstVisibleChild;
        }
    }
    endY += vertGap;

    for( int i = startIndex; i <= lastKid; i++ ) {
        child = (WWindow *) kids[i];
        if( child->isVisible() ) {
            rect = child->getAutosize();
            if( rect.y() != endY ) {
                rect.y( endY );
                child->move( rect );
            }
            if( rect.x() == viewLeft ) {
                endY += rect.h() + vertGap;
            }
        }
    }
    _minHeight = endY + 30;
    getRectangle( rect, TRUE );
    rect.h( endY + 30 );
    move( rect );
}

bool ViewSymbol::setMinimumSize( short *w, short *h )
/***************************************************/
{
    *w = (short) _minWidth;
    *h = (short) _minHeight;
    return TRUE;
}

void ViewSymbol::showUsers( WWindow * )
/*************************************/
{
    showList( &_usersList, NULL, _showUsers, (cb)&ViewSymbol::loadUsers,
              _usersOn, (pickCallback)&ViewSymbol::selectUser,
              (pickCallback)&ViewSymbol::popUserReference );
}

void ViewSymbol::editDefinition( WWindow * )
/******************************************/
{
    char        buff[ MAX_DISP_NAME_LEN ];
    bool        status = FALSE;
    ulong       line;
    uint        col;

    _model->filePosition(line,col);
    char * file = _model->defSourceFile();
    if( file ) {
        int size = strlen( _model->copyBaseName(buff) );
        browseTop->positionEditor( file, line, col, size );

        #if DEBUG
        // NYI - to be removed -- just for testing.
        new EditSession( 200, 200, *_model, this );
        #endif
    }
}

void ViewSymbol::loadUsers()
/**************************/
// _usersList must already exist
{
    if( _usersList ) {
        _users.deleteContents();
        _model->loadUsers(_users);
        _usersList->setObjectList( _users, (nameMethod)&Reference::name);
    }
}

void ViewSymbol::selectUser( WObject * )
/**************************************/
// could refresh attached local view
{
}


void ViewSymbol::showList( ObjectPickList ** picklist,
                           WText ** description,
                           WCheckBox * showCheckBox,
                           cb loadMethod, bool & show,
                           pickCallback listSelectMethod,
                           pickCallback dblSelectMethod )
/******************************************************************************/
{
    bool changed = FALSE;
    WVList & kids = children();
    long listBoxIndex = kids.indexOfSame( showCheckBox ) + 1;

    show = (bool)showCheckBox->checked();
    if( show ) {
        if( *picklist == NULL ) {
            *picklist = new ObjectPickList( WRect(viewLeft,0,-5,64), this );

            //move new list into correct order; arrange depends on it
            kids.removeAt( kids.count() - 1 );
            kids.insertAt( listBoxIndex, *picklist );

//          (*picklist)->setFont(stdFont);
            (*picklist)->setCallbacks(*this, listSelectMethod, dblSelectMethod);

            if( description ) {
                REQUIRE( *description == NULL, "viewsym::showlist -- description initialized, but not list!" );

                *description = new WText( this, WRect(viewLeft,0,-5,18), "",
                                      TextStyleLeftNoWrap|TextStyleAmpersands );
                (*description)->show();
                kids.removeAt( kids.count() -1 );
                kids.insertAt( listBoxIndex + 1, *description );
            }

            (this->*loadMethod)();
            (*picklist)->setFocus();
            (*picklist)->select(0);
            (*picklist)->chosen(*picklist); // for some reason select doesn't send call chosen ...
            changed = TRUE;
        } else if( !(*picklist)->isVisible() ) {
            (*picklist)->show();
            if( description ) {
                (*description)->show();
            }
            changed = TRUE;
        }
    } else {
        if( *picklist && (*picklist)->isVisible() ) {
            (*picklist)->show( WWinStateHide );
            if( description ){
                (*description)->show( WWinStateHide );
            }
            changed = TRUE;
        }
    }
    if( changed ) arrangeKids( listBoxIndex );
}
