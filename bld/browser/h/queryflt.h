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


#ifndef __QUERYFLT_H__
#define __QUERYFLT_H__

#include <wdialog.hpp>

#include "dgqrfilt.gh"

class WText;
class WCommandList;
class WRadioButton;
class WCheckBox;
class WPushButton;
class WDefPushButton;
class WGroupBox;
class WEditBox;
class KeySymbol;
class FileEdit;

#define NUMQFLTCHECKBOXES 5

class QueryFilter : public QueryFilterDlg, public WDialog
{
public:
                        QueryFilter( WWindow * prt, const char * txt );
                        ~QueryFilter();

    virtual void        initialize();
    virtual void        cancelButton( WWindow * );

            int         editFilter( KeySymbol * filter );

            void        okButton( WWindow * );
            void        fileButton( WWindow * );
            void        helpButton( WWindow * );
            bool        contextHelp( bool );

            void        searchAll( WWindow * );

            void        setValues();
private:
    KeySymbol *         _filter;
    FileEdit *          _fileEdit;

    WGroupBox *         _searchForGroup;
    union {
        struct {
            WCheckBox * _searchClasses;
            WCheckBox * _searchEnums;
            WCheckBox * _searchFunctions;
            WCheckBox * _searchTypedefs;
            WCheckBox * _searchVariables;
        } _chbx;
        WCheckBox *     _chkbxArray[ NUMQFLTCHECKBOXES ];
    };
    WPushButton *       _searchAll;

    WGroupBox *         _searchInGroup;
    WEditBox *          _classEdit;
    WText *             _classText;
    WEditBox *          _functionEdit;
    WText *             _functionText;

    WGroupBox *         _symAttribs;
    WCheckBox *         _attribAnon;
    WCheckBox *         _attribArt;
    WCheckBox *         _attribDecl;

    WDefPushButton *    _okButton;
    WPushButton *       _cancelButton;
    WPushButton *       _fileButton;
    WPushButton *       _helpButton;
};

#endif // __QUERYFLT_H__
