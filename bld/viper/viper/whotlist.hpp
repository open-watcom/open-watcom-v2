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


#ifndef __HOTLIST_HPP__
#define __HOTLIST_HPP__

#include "wwindow.hpp"
#include "whotspot.hpp"

const WStyle    HotSpotListStyleDefault     = WStyleVScrollAll | WStyleVRows |
                                              WStyleVScrollEvents | WStyleHScrollAll;
const WExStyle  HotSpotListExStyleDefault   = WExStyle3DBorder;

class WHotSpotList : public WWindow
{
    public:
        WHotSpotList( WWindow * prt, const WRect & r, const char * text, WHotSpots* hs,
                      WStyle wstyle = HotSpotListStyleDefault,
                      WExStyle wexstyle = HotSpotListExStyleDefault );
        virtual ~WHotSpotList();

                /* members from WWindow */
        virtual bool            gettingFocus( WWindow* );
        virtual bool            paint();
        virtual bool            mouseMove( int x, int y, WMouseKeyFlags );
        virtual bool            leftBttnDn( int x, int y, WMouseKeyFlags );
        virtual bool            leftBttnUp( int x, int y, WMouseKeyFlags );
        virtual bool            leftBttnDbl( int x, int y, WMouseKeyFlags );
        virtual bool            rightBttnDn( int x, int y, WMouseKeyFlags );
        virtual bool            scrollNotify( WScrollNotification, int );
        virtual bool            keyDown( WKeyCode, WKeyState );

                void            onChanged( WObject * obj, cbw changed );
                void            onDblClick( WObject * obj, cbw dblClick );
                void            onHotPress( WObject * obj, cbw hotPress );
                int             selected();
                void            setSelected( int index );
                void            select( int index );
                void            reset();

        virtual int             count() = 0;
        virtual int             width() = 0;
        virtual const char *    getString( int index ) = 0;
        virtual int             getHotSpot( int index, bool pressed ) = 0;
        virtual int             getHotOffset( int ) { return 0; }
        virtual void            resized( WOrdinal width, WOrdinal height );

    protected:
        void            scrollToSelected();

        int             _topIndex;  // index of top of list
        int             _selected;  // index of selected item
        void            performScroll( int diff, bool absolute = FALSE );
        void            adjustScrollBars();

    private:
        void            changed();

        bool            _leftDown;
        int             _hotPressIdx;
        bool            _inHotZone;

        WObject*        _changedClient;
        cbw             _changed;
        WObject*        _dblClickClient;
        cbw             _dblClick;
        WObject*        _hotPressClient;
        cbw             _hotPress;
        WHotSpots*      _hs;
};

#endif
