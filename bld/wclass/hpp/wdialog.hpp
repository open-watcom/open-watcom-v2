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


#ifndef wdialog_class
#define wdialog_class

#include "wwindow.hpp"

WCLASS WDialog : public WWindow {
    public:
        WEXPORT WDialog( WWindow *parent=NULL );
        WEXPORT WDialog( WWindow *parent, const char *text );
        WEXPORT WDialog( const char *text, const WRect& r );
        WEXPORT WDialog( const WRect& r, const char *text=NULL );
        WEXPORT WDialog( WWindow* parent, const char *text,
                         const WRect& r );
        WEXPORT WDialog( WWindow* parent, const WRect& r,
                         const char *text=NULL );
        WEXPORT ~WDialog();
        virtual void WEXPORT initialize() {}
        virtual void cancelButton( WWindow * ) { quit( 0 ); }
        virtual void okButton( WWindow * ) { quit( 1 ); }
        virtual bool WEXPORT keyDown( WKeyCode, WKeyState );
        int WEXPORT process();
        int WEXPORT process( WWindow *parent );
        virtual WWindow * WEXPORT switchChild( WWindow *win, bool forward );
        void WEXPORT quit( int code );
        void WEXPORT setDefaultQuitCode( int quit ) { _quitCode = quit; }
        bool WEXPORT processMsg( gui_event msg, void *parm );
        virtual WOrdinal WEXPORT frameWidth( void ) {
            return( WSystemMetrics::dialogFrameWidth() );
        }
        virtual WOrdinal WEXPORT frameHeight( void ) {
            return( WSystemMetrics::dialogFrameHeight() );
        }
        /* right mouse button should not display a popup for dialogs */
        virtual bool WEXPORT rightBttnUp( int, int, WMouseKeyFlags ) { return( FALSE ); }
    protected:
        virtual void doDialog( WWindow *parent );
    protected:
        WString         _text;
    private:
        int             _quitCode;
};

#endif
