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


#ifndef __HOTLIST_H__
#define __HOTLIST_H__

// disable "integral value may be truncated" warning
#pragma warning 389 9

#include "wbrwin.h"

const WStyle HotSpotListStyleDefault = WStyleVScrollAll
                                | WStyleVRows
                                | WStyleVScrollEvents
                                | WStyleHScrollAll;

class HotSpotList
{
public:
                                HotSpotList( WWindow * win, bool infinite );
        virtual                 ~HotSpotList();

                bool            HLPaint();
                bool            HLMouseMove( int x, int y );
                bool            HLLeftBttnDn( int x, int y );
                bool            HLLeftBttnUp( int x, int y );
                bool            HLLeftBttnDbl( int x, int y );
                bool            HLScrollNotify( WScrollNotification, int );
                bool            HLKeyDown( WKeyCode, WKeyState );

                void            onChanged( WObject * obj, cbw changed );
                void            onDblClick( WObject * obj, cbw dblClick );
                void            onHotPress( WObject * obj, cbw hotPress );
                int             selected();
                void            select( int index );
                void            reset();
                void            resetScrollRange();

                // --------- members supplied by derived --------- //
        virtual int             count() = 0;
        virtual const char *    getString( int index ) = 0;
        virtual int             getHotSpot( int index, bool pressed ) = 0;
        virtual int             getHotOffset( int ) { return 0; }
        virtual bool            full() { return TRUE; }

protected:
        void            scrollToSelected();

        int             _topIndex;      // index of top of list
        int             _selected;      // index of selected item
        WPaintAttr      _selectedAttr;  // attribute of selected item
        WWindow *       _win;

private:
        void            changed();
        void            performScroll( long diff, bool absolute = FALSE );

        bool            _leftDown;
        int             _hotPressIdx;
        bool            _inHotZone;
        bool            _infinite;
        int             _width;

        WObject*        _changedClient;
        cbw             _changed;
        WObject*        _dblClickClient;
        cbw             _dblClick;
        WObject*        _hotPressClient;
        cbw             _hotPress;
};

class HotControlList : public WWindow, public HotSpotList
{
public:
                                HotControlList( WWindow * prt,
                                                const WRect & r,
                                                WStyle stl = HotSpotListStyleDefault );

        virtual bool            gettingFocus( WWindow * );
        virtual bool            losingFocus( WWindow * );
        virtual bool            paint() { return HLPaint(); }
        virtual bool            mouseMove( int x, int y, WMouseKeyFlags mf ) {
            WWindow::mouseMove( x, y, mf );
            return HLMouseMove( x, y );
        }
        virtual bool            leftBttnDn( int x, int y, WMouseKeyFlags ) { return HLLeftBttnDn( x, y ); }
        virtual bool            leftBttnUp( int x, int y, WMouseKeyFlags ) { return HLLeftBttnUp( x, y ); }
        virtual bool            leftBttnDbl( int x, int y, WMouseKeyFlags ) { return HLLeftBttnDbl( x, y ); }
        virtual bool            scrollNotify( WScrollNotification sn, int dif ) { return HLScrollNotify( sn, dif ); }
        virtual bool            keyDown( WKeyCode kc, WKeyState ks ) { return HLKeyDown( kc, ks ); }
};

class HotWindowList : public WBRWindow, public HotSpotList
{
public:
                                HotWindowList( const char * text,
                                               bool infinite = FALSE,
                                               WStyle stl =
                                               WBRWinStyleDefault |
                                               HotSpotListStyleDefault );

        virtual bool            paint() { return HLPaint(); }
        virtual bool            mouseMove( int x, int y, WMouseKeyFlags mf ) {
            WWindow::mouseMove( x, y, mf );
            return HLMouseMove( x, y );
        }
        virtual bool            leftBttnDn( int x, int y, WMouseKeyFlags ) { return HLLeftBttnDn( x, y ); }
        virtual bool            leftBttnUp( int x, int y, WMouseKeyFlags ) { return HLLeftBttnUp( x, y ); }
        virtual bool            leftBttnDbl( int x, int y, WMouseKeyFlags ) { return HLLeftBttnDbl( x, y ); }
        virtual bool            scrollNotify( WScrollNotification sn, int dif ) { return HLScrollNotify( sn, dif ); }
        virtual bool            keyDown( WKeyCode kc, WKeyState ks ) { return HLKeyDown( kc, ks ); }
        virtual void            resized( WOrdinal width, WOrdinal height );
};

#endif // __HOTLIST_H__
