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


#include "wbrdefs.h"
#include "wbrwin.h"
#include "death.h"
#include "util.h"
#include "viewmgr.h"
#include "menumgr.h"
#include "optmgr.h"
#include "dbmgr.h"
#include "browse.h"

static ViewManager       VManager;
static ViewManager *     WBRWinBase::_viewManager = &VManager;

static MenuManager       MManager;
static MenuManager *     WBRWinBase::_menuManager = &MManager;

static OptionManager     OManager;
static OptionManager *   WBRWinBase::_optManager = &OManager;

static DatabaseManager   DManager;
static DatabaseManager * WBRWinBase::_dbManager = &DManager;

WBRWindow::WBRWindow( const char *t, WStyle s )
              : WMdiChild( browseTop, t, s )
//---------------------------------------------
{
}

WBRWindow::WBRWindow( const WRect &r, const char *t, WStyle s )
              : WMdiChild( browseTop, r, t, s )
//-------------------------------------------------------------
{
}


WSystemHelp * WBRWinBase::helpInfo()
//----------------------------------
{
    return( browseTop->helpInfo() );
}


void WBRWinBase::popDetail( const Symbol * sym )
//----------------------------------------------
{
    try {
        _viewManager->showDetailView( sym, DetailView::ViewDetail );
    } catch( CauseOfDeath cause ) {
        IdentifyAssassin( cause );
    }
}

ViewManager * WBRWinBase::viewManager()
//-------------------------------------
{
    return _viewManager;
}

MenuManager * WBRWinBase::menuManager()
//-------------------------------------
{
    return _menuManager;
}

OptionManager * WBRWinBase::optManager()
//--------------------------------------
{
    return _optManager;
}

DatabaseManager * WBRWinBase::dbManager()
//---------------------------------------
{
    return _dbManager;
}
