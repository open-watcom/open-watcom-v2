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


#ifndef wpickdialog_class
#define wpickdialog_class

#include "wdialog.hpp"
#include "wpicklst.hpp"
#include "wpickbox.hpp"
#include "wstring.hpp"

WCLASS WText;
WCLASS WPickDialog  : public WDialog {
    public:
        WEXPORT WPickDialog( WPickList& list, cbs getname, WWindow *parent,
                             const char *text );
        WEXPORT ~WPickDialog();
        void WEXPORT picked( WWindow * );
        void WEXPORT okButton( WWindow * );
        void WEXPORT cancelButton( WWindow * );
        int WEXPORT pickOne( const char *prompt="Prompt" );
    private:
        void initialize( void );
        void updateExtents( const char *, int *, int * );
    private:
        const char      *_promptText;
        WText*          _prompt;
        WPickBox*       _input;
        WPickList*      _list;
        cbs             _getname;
};

#endif
