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


#ifndef wlistbox_class
#define wlistbox_class

#include "wcontrol.hpp"
#include "wstring.hpp"

#define LStyleDefault           0
#define LStyleNoIntegral        _ControlStyleToWStyle( GUI_CONTROL_NOINTEGRALHEIGHT )
#define LStyleSorted            _ControlStyleToWStyle( GUI_CONTROL_SORTED )

WCLASS WListBox : public WControl {
    public:
        WEXPORT WListBox( WWindow* parent, const WRect& r,
                          WStyle wstyle=LStyleDefault );
        WEXPORT WListBox( WStatDialog* parent, unsigned id,
                          WStyle wstyle=LStyleDefault );
        WEXPORT ~WListBox();
        bool WEXPORT gettingFocus( WWindow* );
        void WEXPORT onChanged( WObject* obj, cbw changed );
        void WEXPORT onDblClick( WObject* obj, cbw dblClick );

        void WEXPORT getString( int index, WString& str );
        int WEXPORT insertString( const char *s, int index=-1 );
        void WEXPORT deleteString( int index );
        void WEXPORT reset();
        int WEXPORT count();
        int WEXPORT selected();
        void WEXPORT select( int index );
        void WEXPORT setTopIndex( int index );
        int WEXPORT topIndex();
        void WEXPORT setExtent( WOrdinal extent );
        bool processMsg( gui_event );
        virtual bool WEXPORT processMsg( gui_event msg, void *parm ) {
            return( WWindow::processMsg( msg, parm ) );
        };
        bool keyDown( WKeyCode, WKeyState );
        void WEXPORT setTagPtr( int index, void* tagPtr );
        void* WEXPORT tagPtr( int index );
    protected:
        virtual gui_control_class controlClass() { return GUI_LISTBOX; }
    private:
        WObject*        _changedClient;
        cbw             _changed;
        WObject*        _dblClickClient;
        cbw             _dblClick;
        WString         _hotPrefix;
        int findString( int index, const char *str );
};

#endif
