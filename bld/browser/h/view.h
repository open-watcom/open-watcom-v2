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


#ifndef __VIEW_H__
#define __VIEW_H__

class MenuManager;
class View;

enum ViewEventVals {
    VENoEvent               = 0x0000,       /* want no events */
    VECreate                = 0x0001,       /* view being created */
    VEClose                 = 0x0002,       /* view being closed */
    VEGettingFocus          = 0x0004,       /* view gets focus */
    VELosingFocus           = 0x0008,       /* view loses focus */
    VEBrowseFileChange      = 0x0010,       /* new .dbr loaded -- no view */
    VEOptionFileChange      = 0x0020,       /* new .obr loaded -- no view */
    VEOptionValChange       = 0x0040,       /* options changed -- no view */
    VEQueryFiltChange       = 0x0080,       /* query performed -- no view */
    VEBrowseTopDying        = 0x0100,       /* end of the program -- no view */
};

typedef long ViewEvent;     // allow |'ing values together

class ViewEventReceiver {
public:
    virtual void        event( ViewEvent ve, View * view ) = 0;
    virtual ViewEvent   wantEvents() = 0;
};

enum ViewIdentity {
    VIDetailView,
    VIListView,
    VIInheritTree,
    VIInheritOutline,
    VICallTree,
    VICallOutline
};

class View : public ViewEventReceiver {
public:
    virtual                 ~View() {}
    virtual void            setMenus( MenuManager * ) = 0;
    virtual void            unsetMenus( MenuManager * ) = 0;
    virtual ViewIdentity    identity() = 0;
};

#endif // __VIEW_H__

