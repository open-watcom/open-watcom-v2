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


#ifndef opickdlg_h
#define opickdlg_h

// opickdlg.cpp - implementation of class ObjectPickDlg
// puts up dialog listing names of objects in OrdCollect passed into
// constructor, and calls back with address of the selected object, if any

/*
                            Behavior

        Enter hit, with focus on...
                Ok button:
                     dblClick callback called, window closes
                Cancel button :
                    window closes, no callback
                List box :
                    dblClick callback called,
                    if persistence is FALSE, window closes

        cursor up and down, or mouse select:
            select callback called for current item
        double click on list item:
            dblClick callback called for current item,
            if persistence is FALSE, window closes
*/


#include "wbrdefs.h"
#include "opicklst.h"
#include "wbrwin.h"
#include "callback.h"

class WPushButton;

class ObjectPickDlg : public WBRWindow
{
public:
                ObjectPickDlg( int x, int y, int w, int h,
                               WVList & objs, nameMethod objName,
                               WObject & client, pickCallback selected,
                               pickCallback dblClick, char * title=NULL );
                ~ObjectPickDlg() {}
  protected:
// event handlers
    void        okEdit( WWindow * );
    void        cancelEdit( WWindow * );
    bool        enterHit();

    ObjectPickList *    _objList;
    WPushButton *       _cancelBttn;
};
#endif /*opickdlg_h*/
