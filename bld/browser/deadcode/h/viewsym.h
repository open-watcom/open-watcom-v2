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


#ifndef viewsym_h
#define viewsym_h

#include <wvlist.hpp>
#include "callback.h"
#include "wbrwin.h"
#include "vwcreate.h"

class WCheckBox;
class WPushButton;
class Browse;
class WText;
class Reference;
class Symbol;
class ObjectPickList;

enum {
    viewLeft = 4,
    viewHeight = 25
};

// in ctor, derived flag disables call of virtual setup; derived will do it:

class ViewSymbol : public WBRWindow
{
public:
                    ViewSymbol();
                    ViewSymbol( int x, int y, WBRWindow * parent,
                                const Symbol * info, bool derived=FALSE);
                    ~ViewSymbol();

    bool            setMinimumSize( short *w, short *h );

    static WBRWindow * createView( int, int, WBRWindow *, const Symbol * );
    static WBRWindow * create( int x, int y, WBRWindow *parent,
                                const Symbol *info );
//  callbacks
    void            showUsers( WWindow * );
    void            popSymbolDetail( WObject * info );
    void            popUserReference( WObject * ref );

    void            showList(
                            ObjectPickList ** pickList,
                            WText ** description,
                            WCheckBox * showCheckBox,
                            cb loadMethod,
                            bool & defaultListShow,
                            pickCallback listSelectMethod,
                            pickCallback DblSelect =
                                   (pickCallback)&ViewSymbol::popSymbolDetail
                            );


protected:
    bool            isEqual( WObject * ); //if equivalent Symbol; also matches Symbol

    virtual void    setup();
    virtual void    refresh();

        // adjust children with index>startIndex to account for hides, sizing:
    void            arrangeKids( int startIndex=3 );//3 is _usersList list box

    Symbol *        _model;

private:
    void            displayDescription();
    void            editDefinition( WWindow * );
    void            selectUser( WObject * );
    void            loadUsers();

    ObjectPickList *_usersList;
    WVList          _users;
    WCheckBox *     _showUsers;
    int             _minWidth;
    int             _minHeight;
    bool            _isderived;
    static bool     _usersOn;
    static createFn _createTbl[];
};
#endif // viewsym_h
