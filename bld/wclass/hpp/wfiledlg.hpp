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


#ifndef wfiledialog_class
#define wfiledialog_class

#include <stdlib.h>

#include "wdialog.hpp"
#include "wfilelst.hpp"

extern "C" {
    #include "guifdlg.h"
}


#define WMaxFiles       32      // for multi-select file dialogs

#define WFOpenNew       (fn_flags)(FN_HIDEREADONLY | FN_PATHMUSTEXIST)
#define WFOpenNewAll    (fn_flags)FN_HIDEREADONLY
#define WFOpenExisting  (fn_flags)(FN_HIDEREADONLY | FN_PATHMUSTEXIST | FN_FILEMUSTEXIST)
#define WFSaveDefault   (fn_flags)(FN_HIDEREADONLY | FN_OVERWRITEPROMPT)

WCLASS WFileDialog  : public WDialog {
    public:
        WEXPORT WFileDialog( WWindow* parent, const char *filter=NULL );
        WEXPORT WFileDialog( WWindow* parent, WResourceId filter_id );
        WEXPORT ~WFileDialog();
        const char * WEXPORT getOpenFileName( const char *fn=NULL,
                                              const char *title=NULL,
                                              fn_flags style=WFOpenExisting );
        bool WEXPORT getOpenFileName( WFileNameList &flist,
                                      const char *fn=NULL,
                                      const char *title=NULL,
                                      fn_flags style=WFOpenExisting );
        const char * WEXPORT getSaveFileName( const char *fn=NULL,
                                              const char *title=NULL,
                                              fn_flags style=WFSaveDefault );
    private:
        open_file_name  _ofn;
        WWindow         *_parent;
        char            *_fileName;
        char            _titleName[128];
        char            *_dirName;
        void            makeDialog( const char *filter );
        void            init( const char *filename, const char *title );
};

#endif
