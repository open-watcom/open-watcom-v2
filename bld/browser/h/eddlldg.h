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


#ifndef __EDDLLDG_H__
#define __EDDLLDG_H__

#include <wdialog.hpp>

#include "dgeddll.gh"

class WText;
class WPushButton;
class WRadioButton;
class WDefPushButton;
class WEditBox;
class WGroupBox;

class NewEditDLL : public EditDLLDlg, public WDialog {
public:
                                NewEditDLL( WWindow * parent,
                                            const char * curName,
                                            bool curIsDLL,
                                            const char *curParms,
                                            const char * def,
                                            bool defIsDLL,
                                            const char * defParms );
                                ~NewEditDLL();

        virtual void            cancelButton( WWindow * );
        virtual void            initialize();

                void            okButton( WWindow * );
                void            defaultButton( WWindow * );
                void            filesButton( WWindow * );
                void            helpButton( WWindow * );
                void            exeButton( WWindow * );
                void            dllButton( WWindow * );
                bool            contextHelp( bool );

                const char *    getEditor()     { return _newDLLName.gets(); };
                const char *    getEditorParms(){ return _parms.gets(); };
                bool            isEditorDLL()   { return _isDLL; };
private:
        WString                 _default;
        bool                    _defaultIsDLL;
        WString                 _defaultParms;

        WString                 _newDLLName;
        WString                 _parms;
        bool                    _isDLL;

        WText *                 _dllText;
        WText *                 _parmBoxText;
        WEditBox *              _dllEdit;
        WEditBox *              _editorParms;

        WDefPushButton *        _okButton;
        WPushButton *           _defaultButton;
        WPushButton *           _filesButton;
        WPushButton *           _cancelButton;
        WPushButton *           _helpButton;
        WRadioButton *          _dllButton;
        WRadioButton *          _exeButton;
        WGroupBox *             _gbox;

        WText *                 _parmMsg;
        WText *                 _macroMsg;
        WText *                 _fMsg;
        WText *                 _rMsg;
        WText *                 _cMsg;
};


#endif //__EDDLLDG_H__
