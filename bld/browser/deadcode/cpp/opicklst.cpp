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
#include <wvlist.hpp>
#include <wstring.hpp>

#include "wbrdefs.h"
#include "assure.h"
#include "opicklst.h"

ObjectPickList::ObjectPickList( WRect & frame )
    : WListBox( topWindow, frame )
    ,_objects(NULL)
    ,_clntCallback(NULL)
    ,_clntDblCallback(NULL)
//---------------------------------------------------------------
{
    _client = NULL;
    show();
}

void ObjectPickList::setObjectList( WVList & objs, nameMethod objName )
//---------------------------------------------------------------------
{
    char buff[ MAX_DISP_NAME_LEN ];

    _objects = &objs;
    reset();
    for( int i = 0; i < objs.count(); i++ ) {
        insertString( (objs[i]->*objName)( buff ) );
    }
}

void ObjectPickList::setCallbacks( WObject & clnt, pickCallback selected,
                                   pickCallback dblClick )
//-----------------------------------------------------------------------
{
    _client = &clnt;
    if( selected ) {
        _clntCallback = selected;
        onChanged( this, (cbw)&ObjectPickList::chosen );
    }
    if( dblClick ) {
        _clntDblCallback = dblClick;
        onDblClick(this, (cbw)&ObjectPickList::dblClicked );
    }
}

void ObjectPickList::chosen( WWindow *)
//-------------------------------------
{
    if( selected() >= 0 ) {
        (_client->*_clntCallback)( (*_objects)[selected()]);
    }
}

void ObjectPickList::dblClicked( WWindow *)
//-----------------------------------------
{
    (_client->*_clntDblCallback)( (*_objects)[selected()]);
}

bool ObjectPickList::enterHit()
//-----------------------------
{
    dblClicked( NULL );
    return TRUE;
}
