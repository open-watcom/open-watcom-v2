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


#ifndef __OPICKLST_H__
#define __OPICKLST_H__

/*
                    Behavior
                    ========
        Enter hit:
            dblClick callback called

        cursor up and down, or mouse select:
            select callback called for current item
        double click on list item:
            dblClick callback called for current item,
*/


#include <wlistbox.hpp>
#include "wbrdefs.h"
#include "wbrwin.h"
#include "callback.h"

class WVList;

class ObjectPickList : public WListBox
{
public:
                ObjectPickList( WRect & );
                ~ObjectPickList(){}

    void        setObjectList( WVList & objs, nameMethod objName);
    void        setCallbacks( WObject & client
                            ,pickCallback selected
                            ,pickCallback dblClick
                            );
    void        chosen( WWindow * );
    void        dblClicked( WWindow * );

protected:
    bool        enterHit();

    WVList *    _objects;  // not owned
    WObject *   _client;

    pickCallback _clntCallback;
    pickCallback _clntDblCallback;
};

#endif // __OPICKLST_H__
