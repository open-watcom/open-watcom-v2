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
#include <wvlist.hpp>
#include "assure.h"
#include "opickdlg.h"
#include "opicklst.h"


ObjectPickDlg::ObjectPickDlg( int x, int y, int w, int h,
                              WVList & objs, nameMethod nmMthd,
                              WObject & clnt, pickCallback select,
                              pickCallback dblClickCb, char * title )
    : WBRWindow( WRect(x,y,w,h), title )
    ,_objList(new ObjectPickList( WRect(x,y,w,h-40) ))
//-------------------------------------------------------------------------
{
//    _objList->setFont(stdFont);  NYI
    _objList->setCallbacks( clnt, select, dblClickCb );
    _objList->setObjectList( objs, nmMthd );

    // NYI -- this used to have a BStyleDefPushButton style
    WPushButton * bOk = new WPushButton( this, WRect(10,h-30,100,22), "Select" );
    bOk->onClick( this, (cbw)&ObjectPickDlg::okEdit );
    bOk->show();

    _cancelBttn = new WPushButton( this, WRect(120,h-30,100,22), "Cancel" );
    _cancelBttn->onClick( this, (cbw)&ObjectPickDlg::cancelEdit );
    _cancelBttn->show();
    _objList->setFocus();
    show();
}

void ObjectPickDlg::okEdit( WWindow * )
//-------------------------------------
{
    _objList->dblClicked( NULL );
}

void ObjectPickDlg::cancelEdit( WWindow * )
//-----------------------------------------
{
    delete this;
}

bool ObjectPickDlg::enterHit()   // NYI
//----------------------------
{
#if 0
    id focus = getChildFocus();
    if( focus == _objList ) {
        dblClicked( _objList->selectedItem() );
    } else if (focus != _cancelBttn) {
        okEdit();
    } else {  // must be ok button
        cancelEdit();
    }
#endif
    return TRUE;
}
