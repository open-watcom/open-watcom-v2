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


#ifndef wbutton_class
#define wbutton_class

#include "wcontrol.hpp"

WCLASS WButton : public WControl
{
    public:
        WEXPORT WButton( WWindow* parent, gui_control_class control_class,
                         const WRect& r, const char *text, WStyle wstyle );
        WEXPORT WButton( WStatDialog* parent, unsigned id, WStyle wstyle );
        WEXPORT ~WButton();
        void WEXPORT onClick( WObject* obj, cbw click );
        void WEXPORT onDblClick( WObject* obj, cbw dblClick );
        bool WEXPORT hotKey( WKeyCode );
        virtual bool processMsg( gui_event );
        virtual bool WEXPORT processMsg( gui_event msg, void *parm ) {
            return( WWindow::processMsg( msg, parm ) );
        };
        void setTagPtr( void* tagPtr ) { _tagPtr = tagPtr; }
        void* tagPtr() { return _tagPtr; }
    private:
        WObject*        _clickClient;
        cbw             _click;
        WObject*        _dblClickClient;
        cbw             _dblClick;
        void*           _tagPtr;
};

#endif
