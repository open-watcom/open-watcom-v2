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


#ifndef __REPORTDG_H__
#define __REPORTDG_H__

#include <wdialog.hpp>

#include "dgreport.gh"

class WGroupBox;
class WCheckBox;
class WText;
class WEditBox;
class WRadioButton;
class WDefPushButton;
class WPushButton;

class ReportDialog : public ReportDlg, public WDialog {
public:
                                ReportDialog( WWindow * parent );
                                ~ReportDialog();

        virtual void            cancelButton( WWindow * );
        virtual void            initialize();

                void            saveButton( WWindow * );
                void            layoutButton( WWindow * );
                void            helpButton( WWindow * );
                void            filesButton( WWindow * );

private:
    WGroupBox *         _destinationGroup;
    WEditBox *          _destinationEdit;
    WPushButton *       _filesButton;

    WGroupBox *         _contentsGroup;
    WCheckBox *         _classes;
    WCheckBox *         _variables;
    WCheckBox *         _typedefs;
    WCheckBox *         _functions;
    WCheckBox *         _enums;

    WDefPushButton *    _saveButton;
    WPushButton *       _cancelButton;
    WPushButton *       _layoutButton;
    WPushButton *       _helpButton;
};

#endif // __REPORTDG_H__
