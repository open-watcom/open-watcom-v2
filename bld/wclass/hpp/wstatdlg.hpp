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

typedef res_name_or_id WDialogId;

WCLASS WStatDialog : public WDialog {
    public:
        WStatDialog( WWindow * parent, WDialogId dialog_name );
        WStatDialog( WWindow * parent, int dialog_id );

        void addControl( WControl * control );

        virtual WControl * WEXPORT getControl( gui_ctl_id id );

        void WEXPORT getCtrlText( gui_ctl_id id, WString & str );
        void WEXPORT getCtrlText( gui_ctl_id id, char* buff, size_t len );
        size_t WEXPORT getCtrlTextLength( gui_ctl_id id );
        void WEXPORT setCtrlText( gui_ctl_id id, const char *text );
        void WEXPORT getCtrlRect( gui_ctl_id id, WRect & r );
        bool WEXPORT isCtrlEnabled( gui_ctl_id id );
        void WEXPORT enableCtrl( gui_ctl_id id, bool state );

        virtual void doDialog( WWindow *parent );   // override WDialog

    private:
        WDialogId   _dialog_id;             // resource id of the dialog
        WVList      _controls;              // registered controls
};

#endif // wstatdialog_class
