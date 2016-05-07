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


#ifndef __DTVIEW_H__
#define __DTVIEW_H__

#include "view.h"
#include "wbrwin.h"
#include "wbrdefs.h"
#include "symbol.h"

class DetailView : public View, public virtual WBRWinBase
{
public:
    enum ViewType {
        ViewDetail,
        ViewInherit,
        ViewCalls,
        ViewSourceRefs,
        ViewSymbolRefs,
        ViewLast
    };

                            DetailView();
    virtual                 ~DetailView();

            //----- view ------//
    virtual void            event( ViewEvent, View * ){};
    virtual ViewEvent       wantEvents(){ return VENoEvent; };
    virtual void            setMenus( MenuManager * ){};
    virtual void            unsetMenus( MenuManager * ){};
    virtual ViewIdentity    identity(){ return VIDetailView; }

            //--- WBRWindow overrides ---//
            bool            focusChange( bool gettingFocus );
            bool            aboutToClose();

    virtual void            showWin( WWindowState state=WWinStateShow ) = 0;
    virtual void            setFocusWin() = 0;

    virtual drmem_hdl       symHandle() const = 0;
    virtual ViewType        viewType() const = 0;

            bool            operator == ( const DetailView & other ) const {
                                return( symHandle() == other.symHandle()
                                        && viewType() == other.viewType() );
                            }

    static  DetailView *    createView( const Symbol * sym, ViewType vt );

};

#endif // __DTVIEW_H__
