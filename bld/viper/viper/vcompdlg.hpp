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


#ifndef vcompdlg_class
#define vcompdlg_class

#include "wdialog.hpp"
#include "wfilenam.hpp"
#include "wpicklst.hpp"
#include "wlistbox.hpp"

WCLASS WEditBox;
WCLASS WPickCBox;

WCLASS MRule;
WCLASS MComponent;
WCLASS MProject;
WCLASS WFileDialog;
WCLASS VCompDialog : public WDialog
{
        public:
                VCompDialog( WWindow* parent, const char* text, MProject* project, const char* filter );
                ~VCompDialog();
                void okButton( WWindow* );
                void cancelButton( WWindow* );
                void browseButton( WWindow* );
                void initialize();
                bool process( WFileName& fn, MRule** rulep, WString& mask, MComponent* comp );
        private:
                MProject*       _project;
                WFileDialog     *_browseDialog;
                WFileName*      _fn;
                MRule**         _rulep;
                WString*        _mask;
                MComponent*     _comp;
                WPickList       _tgList;
                WString         _cur_os;
                bool legalExt();
                bool    findRule();

                WEditBox*       _eName;
                WListBox*       _imagelist;
                bool checkName();
                void osButton( WWindow *button );
                void updateCurOs( void );
};

#endif
