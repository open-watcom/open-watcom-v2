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


#ifndef __FILEEDIT_H__
#define __FILEEDIT_H__

#include <wdialog.hpp>

#include "dgfileed.gh"

class WPushButton;
class WDefPushButton;
class WGroupBox;
class WCommandList;
class ModuleList;
class FileFilter;

class FileEdit : public FileEditDlg, public WDialog {
public:
                                FileEdit( WWindow * parent );
                                ~FileEdit();

                int             edit( FileFilter * f );

        virtual void            cancelButton( WWindow * );
        virtual void            initialize();

                void            okButton( WWindow * );
                void            helpButton( WWindow * );
                bool            contextHelp( bool );

                void            includeButton( WWindow * );
                void            excludeButton( WWindow * );

                void            setAllButton( WWindow * );
                void            clearAllButton( WWindow * );

private:
                void            update();

        FileFilter *        _fileFilter;
        FileFilter *        _tmpFilter;
        WVList *            _searchPatterns;

        ModuleList *        _fileBox;

        WGroupBox *         _patternGroup;
        WCommandList *      _patternText;
        WPushButton *       _includeButton;
        WPushButton *       _excludeButton;

        WPushButton *       _setAllButton;
        WPushButton *       _clearAllButton;

        WDefPushButton *    _okButton;
        WPushButton *       _cancelButton;
        WPushButton *       _helpButton;
};


#endif //__FILEEDIT_H__
