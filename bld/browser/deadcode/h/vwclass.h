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


#ifndef vwclass_h
#define vwclass_h

#include "wbrdefs.h"
#include "viewsym.h"
#include "filtflag.h"

class ObjectPickList;
class WText;

class ViewClassType : public ViewSymbol
{
public:
                    ViewClassType();
                    ViewClassType( int x, int y, WBRWindow * parent,
                                const Symbol *info, bool derived=FALSE );
                    ~ViewClassType();
    static ViewSymbol * create( int x, int y, WBRWindow *parent,
                                const Symbol *info );
  protected:
    virtual void    setup();
    virtual void    refresh();

    void            selectDataMember( WObject * );
    void            selectMethod( WObject * );
    void            selectFriend( WObject * );

//  callbacks
    void            showDataMembers( WWindow * );
    void            showMethods( WWindow * );
    void            showFriends( WWindow * );
    void            dataFilterProc( WWindow * );
    void            methodFilterProc( WWindow * );

  private:
    void            loadDataMembers();
    void            loadMethods();
    void            loadFriends();
    void            createInherit( WWindow * );
    void            createStructure( WWindow * );

    static bool     _dataMembersOn;
    static bool     _methodsOn;
    static bool     _friendsOn;

    ObjectPickList *_dataMembersList;
    ObjectPickList *_methodsList;
    ObjectPickList *_friendsList;

    WCheckBox *     _showDataMembers;
    WCheckBox *     _showMethods;
    WCheckBox *     _showFriends;

    WText *         _dataMembersDesc;
    WText *         _methodsDesc;
    WText *         _friendsDesc;

    WVList          _dataMembers;
    WVList          _methods;
    WVList          _friends;

    FilterFlags     _dataFlags;
    FilterFlags     _methodFlags;
};
#endif // vwclass_h
