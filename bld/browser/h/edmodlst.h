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


#ifndef __EDMODLST_H__
#define __EDMODLST_H__

#include <wdialog.hpp>

#include "modlist.h"
#include "dgmodlst.gh"

class Browse;

class WPushButton;
class WDefPushButton;
class WString;
class HotList;

class EditModuleList : public EditModuleListDlg, public WDialog
{
public:
                        EditModuleList( WWindow * parent, const char * text,
                                        bool addRemove = TRUE );
                        ~EditModuleList();

    virtual void        initialize();

    virtual bool        losingFocus( WWindow* ){ setFocus(); return TRUE; }
    virtual void        cancelButton( WWindow * ){ quit( 0 ); }
    virtual void        okButton( WWindow * ){ quit( 1 ); }
    virtual void        addButton( WWindow * ){}
            void        removeButton( WWindow * );
            void        disableButton( WWindow * );
            void        enableButton( WWindow * );
    virtual void        helpButton( WWindow * ) = 0;

    virtual void        loadBox() {};
protected:

    bool                _addRemove;
    WCPtrOrderedVector<ModuleItem> *    _removedModuleItems;

    ModuleList *        _moduleBox;
    WDefPushButton *    _okButton;
    WPushButton *       _cancelButton;
    WPushButton *       _addButton;
    WPushButton *       _removeButton;
    WPushButton *       _disableButton;
    WPushButton *       _enableButton;
    WPushButton *       _helpButton;
};

#endif //__EDMODLST_H__
