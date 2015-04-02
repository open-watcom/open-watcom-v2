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


#ifndef wstatdialog_class
#define wstatdialog_class

#include "wdialog.hpp"

WCLASS WStatDialog : public WDialog {
    public:
        WStatDialog( WWindow * parent, WResourceNameOrId dialog_name );
        WStatDialog( WWindow * parent, WResourceId dialog_id );

        void addControl( WControl * control );

        virtual WControl * WEXPORT getControl( WControlId id );

        void WEXPORT getCtrlText( WControlId id, WString & str );
        void WEXPORT getCtrlText( WControlId id, char* buff, size_t len );
        size_t WEXPORT getCtrlTextLength( WControlId id );
        void WEXPORT setCtrlText( WControlId id, const char *text );
        void WEXPORT getCtrlRect( WControlId id, WRect & r );
        bool WEXPORT isCtrlEnabled( WControlId id );
        void WEXPORT enableCtrl( WControlId id, bool state );

        virtual void doDialog( WWindow *parent );   // override WDialog

    private:
        WResourceNameOrId   _dialog_id;     // resource name or id of the dialog
        WVList              _controls;      // registered controls
};

#endif // wstatdialog_class
