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



#ifndef __NEWDBRDG_H__
#define __NEWDBRDG_H__

#include <wdialog.hpp>

#include "dgnewdbr.gh"

class WPushButton;
class WDefPushButton;
class WGroupBox;
class WEditBox;

class NewDBRFile : public NewDBRDlg, public WDialog {
public:
                                NewDBRFile( WWindow * parent,
                                            const char * filter );
                                ~NewDBRFile();

        virtual void            cancelButton( WWindow * );
        virtual void            initialize();

                bool            contextHelp( bool );

                void            okButton( WWindow * );
                void            filesButton( WWindow * );
                const char *    getFileName() { return _fileName.gets(); }
private:
        WString                 _fileName;
        const char *            _filter;

        WGroupBox *             _browsefileGroup;
        WEditBox *              _fileEdit;
        WDefPushButton *        _okButton;
        WPushButton *           _browseFilesButton;
        WPushButton *           _cancelButton;
};


#endif //__NEWDBRDG_H__
