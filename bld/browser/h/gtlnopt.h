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


#ifndef __GTLNOPT_H__
#define __GTLNOPT_H__

#include "dglinedt.gh"
#include "paintinfo.h"
#include "wbrwin.h"

class WDefPushButton;
class WPushButton;
class WBRWindow;
class ScreenDev;

class GTLineEditor
{
public:
        virtual void            setInfo( PaintInfo * ) = 0;
        virtual void            endEdit() = 0;
};

class GTLineOption : public GTLineOptDlg, public WDialog
{
public:
                                GTLineOption( const char * text,
                                              GTLineEditor * prt,
                                              PaintInfo * inf, bool line );
                                ~GTLineOption();

        virtual void            initialize();

                void            okButton( WWindow * );
                void            cancelButton( WWindow * );
                void            helpButton( WWindow * );
                bool            contextHelp( bool );

                void            getPinfo( PaintInfo & );
                void            updateRects( WRect * );

        virtual bool            leftBttnDn( int, int, WMouseKeyFlags );
        virtual bool            leftBttnDbl( int, int, WMouseKeyFlags );
        virtual bool            keyDown( WKeyCode, WKeyState );
        virtual bool            paint();

private:
    PaintInfo *         _info;
    GTLineEditor *      _parent;
    bool                _line;
    bool                _inStyle;

    int                 _style;
    int                 _colour;

    WRect               _styleRects[ 10 ];
    WRect               _colrRects[ 5 ];

    WDefPushButton *    _okButton;
    WPushButton *       _cancelButton;
    WPushButton *       _helpButton;
};

#endif // __GTLNOPT_H__
