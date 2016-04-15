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


#include <wchash.h>
#include <wpopmenu.hpp>
#include <wmenuitm.hpp>
#include <wmenusep.hpp>

#include "assure.h"
#include "browse.h"
#include "menumgr.h"
#include "menuids.h"
#include "view.h"
#include "viewmgr.h"

struct MenuInfo {
    MIMainMenu      mainId;
    const char *    menuName;
    const char *    hint;
};

MenuInfo MainMenuInfo[ MMNumMainMenus ] = {
    { MMFile,           "&File",                             "Create/open DBR files, load/save options." },
    { MMView,           "&View",                             "Open new views, change view options for current window." },
    { MMDetail,         "&Detail",                           "View extra information for this symbol." },
    { MMTree,           "&Tree",                             "Select nodes to view." },
    { MMLocate,         "&Locate",                           "Find symbols within view, reload view." },
    { MMOptions,        "&Options",                          "Change options for all windows." },
    { MMWindows,        "&Windows",                          "Arrange/show/hide/select windows." },
    { MMHelp,           "&Help",                             "View help." },
};

MenuInfo CascadeMenuInfo[ CMNumCascadeMenus ] = {
    { VMInherit,        "&Inheritance",                     "View the inheritance hierarchy." },
    { VMCall,           "&Calls",                           "View the call hierarchy." },
};

struct SubMenuInfo {
    MIMainMenu      mainId;
    MISubMenu       subId;
    const char *    menuName;
    const char *    hint;
};

static SubMenuInfo FileMenuInfo[ FMNumFileMenus ] = {
    { MMFile, FMNew,            "&New...",                   "Create a new database file." },
    { MMFile, FMOpen,           "&Open...",                  "Open an existing database file." },
    { MMFile, FMModules,        "&Modules...",               "List Browser modules." },
    { MMFile, FMSep0,           NULL,                        NULL },
    { MMFile, FMLoad,           "&Load Options...",          "Load Setup." },
    { MMFile, FMSave,           "&Save Options",             "Save current options." },
    { MMFile, FMSaveAs,         "Save Options &As...",       "Save current options, specifying a file name." },
#ifdef REPORT_IMPLEMENTED
    { MMFile, FMSep1,           NULL,                        NULL },
    { MMFile, FMReport,         "&Report...",                "Write a formatted report to a disk file." },
#endif
    { MMFile, FMSep2,           NULL,                        NULL },
    { MMFile, FMExit,           "E&xit",                     "Exit the Open Watcom Source Browser." },
};


static SubMenuInfo ViewMenuInfo[ VMNumViewMenus ] = {
    { MMView, VMList,           "&List...",                  "View a list of symbols." },
    { MMView, VMInherit,        NULL,                        NULL },
    { MMView, VMCall,           NULL,                        NULL },
};

static SubMenuInfo ViewInheritMenuInfo[ VMNumViewInheritMenus ] = {
    { CMViewInherit, VMInheritTree,    "&Graph...",                 "Show a tree view of inheritance." },
    { CMViewInherit, VMInheritOutline, "&Outline...",               "Show an outline view of inheritance." },
};

static SubMenuInfo ViewCallMenuInfo[ VMNumViewCallMenus ] = {
    { CMViewCall, VMCallTree,   "&Graph...",                 "Show a tree view of calls." },
    { CMViewCall, VMCallOutline,"&Outline...",               "Show an outline view of calls." },
};

static SubMenuInfo DetailMenuInfo[ DMNumDetailMenus ] = {
    { MMDetail, DMDetail,       "&Detail...",                "View the details of the selected symbol." },
    { MMDetail, DMDefinition,   "&Goto Definition...",       "Start editor at the definition of the current symbol." },
    { MMDetail, DMSep0,         NULL,                        NULL },
    { MMDetail, DMReferences,   "&Source References...",     "List the source lines which refer to the current symbol." },
    { MMDetail, DMUsers,        "Symbol &References...",     "List the symbols which refer to the current symbol." },
#ifdef DETAIL_STUFF_IMPLEMENTED
    { MMDetail, DMSep1,         NULL,                        NULL },
    { MMDetail, DMInheritance,  "&Inheritance...",           "Show immediate base and derived classes of the current symbol." },
    { MMDetail, DMStructure,    "&Member Layout...",         "Show the layout of member variables for the current symbol." },
    { MMDetail, DMSep2,         NULL,                        NULL },
    { MMDetail, DMCalls,        "&Calls...",                 "Show immediate calls to and from the current symbol." },
#endif
};

static SubMenuInfo TreeMenuInfo[ TMNumTreeMenus ] = {
    { MMTree, TMRoots,           "&Select Root Nodes...",    "Select which trees are displayed, which are hidden." },
    { MMTree, TMArrangeAll,      "&Arrange Graph",           "Arrange the nodes currently visible in the active graphical tree." },
    { MMTree, TMSep0,            NULL,                       NULL },
    { MMTree, TMExpandOne,       "Expand One Level\t+",      "Show the immediate child nodes of the selected node." },
    { MMTree, TMExpandBranch,    "Expand Branch\t*",         "Expand all the descendants of the selected node." },
    { MMTree, TMExpandAll,       "Expand All\tCtrl *",       "Expand all nodes in the display." },
    { MMTree, TMCollapseBranch,  "Collapse Branch\t-",       "Collapse all the descendants of the selected node." },
    { MMTree, TMCollapseAll,     "Collapse All\tCtrl -",     "Collapse all nodes, showing only root nodes." },
    #ifdef DEBUGTREE
    { MMTree, TMSep1,            NULL,                       NULL },
    { MMTree, TMDebugInfo,       "Debug Info",               "Show debugging info for selected node." },
    { MMTree, TMSibWidth,        "Sib Width",                "Show sibling width." },
    #endif
};

static SubMenuInfo LocateMenuInfo[ LMNumLocateMenus ] = {
    { MMLocate, LMFind,           "&Find...",                "Find a symbol in the current view." },
    { MMLocate, LMFindNext,       "Find &Next",              "Find the next occurence of a symbol in the current view." },
    { MMLocate, LMSep0,           NULL,                      NULL },
    { MMLocate, LMFindSelected,   "Find &Selected",          "Find the currently selected symbol in the current view." },
};


static SubMenuInfo OptionsMenuInfo[ OMNumOptionsMenus ] = {
    { MMOptions, OMRegExp,           "&Regular Expressions...", "Configure regular expression usage." },
    { MMOptions, OMQuery,            "&Query...",               "Reload the current view for the specified query." },
    { MMOptions, OMSep0,             NULL,                       NULL },
    { MMOptions, OMEnumStyles,       "&Enumeration Styles...",  "Choose display styles for enumeration values." },
    { MMOptions, OMMemberFilters,    "Member &Filters...",      "Choose which class members to display." },
#ifdef OPTIONS_IMPLEMENTED
    { MMOptions, OMMemberOrder,      "Member &Order...",        "Configure the display order of class members." },
#endif
    { MMOptions, OMSep1,             NULL,                      NULL },
    { MMOptions, OMTreeAutoArrange,  "Graph &Auto Arrange",     "Automatically rearrange graph views when nodes are collapsed." },
    { MMOptions, OMTreeSquareLines,  "Graph Square &Lines",     "Use square lines to display trees." },
    { MMOptions, OMTreeHorizontal,   "Graph &Horizontal",       "Draw graphs horizontally." },
    { MMOptions, OMSep2,             NULL,                      NULL },
    { MMOptions, OMInheritTreeLegend,"&Inheritance Graph Legend...",  "Choose colors and line styles for inheritance trees." },
    { MMOptions, OMCallTreeLegend,   "&Call Graph Legend...",   "Choose colors and line styles for call trees." },
    { MMOptions, OMSep3,             NULL,                      NULL },
    { MMOptions, OMEditorDLL,        "Set Text E&ditor...",     "Choose a text editor executable or a DLL to communicate with a text editor." },
    { MMOptions, OMAutoSave,         "&Save Options on Exit",   "Automatically save options on program exit." },
};

#if defined( __OS2__ )
#define _HelpMenuContents               "&Table of Contents"
#define _HelpMenuIndex                  "Help &Index"
#define _HelpMenuIndexHint              "View Index"
#define _HelpMenuUsing                  "&Using Help"
#define _HelpMenuInformation            "&Product Information"
#else
#define _HelpMenuContents               "&Contents"
#define _HelpMenuIndex                  "&Search for Help On..."
#define _HelpMenuIndexHint              "Search for help on a specific topic."
#define _HelpMenuUsing                  "&How to Use Help"
#define _HelpMenuInformation            "&About..."
#endif

static SubMenuInfo HelpMenuInfo[ HMNumHelpMenus ] = {
    { MMHelp, HMContents,       _HelpMenuContents,      "List Open Watcom Source Browser help topics." },
    { MMHelp, HMSearch,         _HelpMenuIndex,         _HelpMenuIndexHint },
    { MMHelp, HMSep0,           NULL,                   NULL },
    { MMHelp, HMUsingHelp,      _HelpMenuUsing,         "Display information on how to use help." },
    { MMHelp, HMSep1,           NULL,                   NULL },
    { MMHelp, HMAbout,          _HelpMenuInformation,   "Display information on how to use help." },
};

unsigned MenuHash( const MIMenuID & id )
//--------------------------------------
{
    return( id.mainID() ^ id.subID() );
};

MenuManager::MenuManager()
                : _clientWin( NULL )
                , _topMenus( NULL )
                , _receivers( NULL )
//----------------------------------
{
}

MenuManager::~MenuManager()
//-------------------------
{
    delete [] _topMenus;
    _receivers->clear();
    delete _receivers;
}

void MenuManager::registerForViewEvents( ViewManager * vmgr )
//-----------------------------------------------------------
{
    vmgr->registerForEvents( this );
}

ViewEvent MenuManager::wantEvents()
//---------------------------------
{
    return( VEGettingFocus | VELosingFocus | VEBrowseTopDying );
}

void MenuManager::event( ViewEvent ve, View * view )
//--------------------------------------------------
{
    switch( ve ) {
    case VEGettingFocus:
        view->setMenus( this );
        break;
    case VELosingFocus:
        view->unsetMenus( this );
        break;
    case VEBrowseTopDying:
        _clientWin = NULL;
        break;
    }
}

void MenuManager::enableMenu( const MIMenuID & id, bool enable )
//--------------------------------------------------------------
{
    if( _clientWin ) {
        _topMenus[ id.mainID() ]->enableItem( enable, id.subID() );
    }
}

bool MenuManager::menuEnabled( const MIMenuID & id )
//--------------------------------------------------
{
    if( _clientWin ) {
        return _topMenus[ id.mainID() ]->itemEnabled( id.subID() );
    }

    return false;
}

void MenuManager::checkMenu( const MIMenuID & id, bool enable )
//-------------------------------------------------------------
{
    if( _clientWin ) {
        _topMenus[ id.mainID() ]->checkItem( enable, id.subID() );
    }
}

void MenuManager::registerForMenu( MenuHandler * hdlr, const MIMenuID & id, bool enable )
//---------------------------------------------------------------------------------------
{
    ASSERTION( !_receivers->contains( id ) );

    (*_receivers)[ id ] = hdlr;
    enableMenu( id, enable );
}

void MenuManager::unRegister( const MIMenuID & id )
//-------------------------------------------------
{
    bool found;

    enableMenu( id, false );
    found = (bool) _receivers->remove( id );

    ASSERTION( found );
}

void MenuManager::menuSelected( WMenuItem * item )
//------------------------------------------------
{
    const SubMenuInfo * info;
    MenuHandler *       handler;

    info = (const SubMenuInfo *) item->tagPtr();
    MIMenuID id( info->mainId, info->subId );

    if( _receivers->find( id, handler ) ) {
        if( handler != NULL ) {
            handler->menuSelected( id );
        }
    }
}

void MenuManager::menuPopup( WPopupMenu * pop )
//---------------------------------------------
{
    int i;

    ASSERTION( (_clientWin != NULL) );

    for( i = 0; i < MMNumMainMenus; i += 1 ) {
        if( pop == _topMenus[ i ] ) {
            _clientWin->statusText( MainMenuInfo[ i ].hint );
            break;
        }
    }
}

void MenuManager::hintText( WMenuItem *, const char * hint )
//----------------------------------------------------------
{
    ASSERTION( (_clientWin != NULL) );

    _clientWin->statusText( hint );
}

void MenuManager::makeItem( WPopupMenu * pop, const SubMenuInfo * info,
                            int idx, bool disable )
//-------------------------------------------------------------------------------
{
    WMenuItem * item;

    if( info[ idx ].menuName != NULL ) {
        item = new WMenuItem( info[ idx ].menuName, this, (cbm) &MenuManager::menuSelected,
                                (cbh) &MenuManager::hintText, info[ idx ].hint );
        item->setTagPtr( (void *) (info + idx) );
    } else {
        item = new WMenuSeparator;
    }

    pop->insertItem( item );

    if( disable ) {
        pop->enableItem( false, idx );
    }
}

void MenuManager::setupMenus( Browse * client )
//---------------------------------------------
{
    int             i;
    WPopupMenu *    pop;
    WPopupMenu *    subPop;
    WMenu *         menu;

    _clientWin = client;
    menu = new WMenu;

    typedef WPopupMenu * popMenuStar;   // to use new []

    _topMenus = new popMenuStar[ MMNumMainMenus + CMNumCascadeMenus ];
    _receivers = new WCValHashDict< MIMenuID, MenuHandler * >( &MenuHash );

    for( i = 0; i < MMNumMainMenus; i += 1 ) {
        if( i == MMWindows ) {
            // Windows menu handled by WCLASS
            _topMenus[ i ] = NULL;
        } else {
            _topMenus[ i ] = new WPopupMenu( MainMenuInfo[ i ].menuName );
            _topMenus[ i ]->onPopup( this, (cbp) &MenuManager::menuPopup );
        }
    }

    for( i = 0; i < CMNumCascadeMenus; i += 1 ) {
        _topMenus[ i + MMNumMainMenus ] = new WPopupMenu( CascadeMenuInfo[ i ].menuName );
        _topMenus[ i + MMNumMainMenus ]->onPopup( this, (cbp) &MenuManager::menuPopup );
    }

    //------- File Menu ---------//
    pop = _topMenus[ MMFile ];
    menu->insertPopup( pop );
    for( i = 0; i < FMNumFileMenus; i += 1 ) {
        makeItem( pop, FileMenuInfo, i );
    }

    //-------  View Menu --------//
    pop = _topMenus[ MMView ];
    menu->insertPopup( pop );
    makeItem( pop, ViewMenuInfo, VMList );

    subPop = _topMenus[ CMViewInherit ];
    pop->insertPopup( subPop );
    for( i = 0; i < VMNumViewInheritMenus; i += 1 ) {
        makeItem( subPop, ViewInheritMenuInfo, i );
    }

    subPop = _topMenus[ CMViewCall ];
    pop->insertPopup( subPop );
    for( i = 0; i < VMNumViewCallMenus; i += 1 ) {
        makeItem( subPop, ViewCallMenuInfo, i );
    }

    //------- Detail Menu ---------//
    pop = _topMenus[ MMDetail ];
    menu->insertPopup( pop );
    for( i = 0; i < DMNumDetailMenus; i += 1 ) {
        makeItem( pop, DetailMenuInfo, i );
    }

    //------- Tree Menu ---------//
    pop = _topMenus[ MMTree ];
    menu->insertPopup( pop );
    for( i = 0; i < TMNumTreeMenus; i += 1 ) {
        makeItem( pop, TreeMenuInfo, i );
    }

    //------- Locate Menu ---------//
    pop = _topMenus[ MMLocate ];
    menu->insertPopup( pop );
    for( i = 0; i < LMNumLocateMenus; i += 1 ) {
        makeItem( pop, LocateMenuInfo, i );
    }

    //------- Options Menu ---------//
    pop = _topMenus[ MMOptions ];
    menu->insertPopup( pop );
    for( i = 0; i < OMNumOptionsMenus; i += 1 ) {
        makeItem( pop, OptionsMenuInfo, i );
    }

    //-------- Windows Menu ----------//
    // WCLASS handles this one for us //

    pop = _clientWin->getMdiPopup();
    menu->insertPopup( pop );

    //------- Help Menu ---------//
    pop = _topMenus[ MMHelp ];
    menu->insertPopup( pop );
    for( i = 0; i < HMNumHelpMenus; i += 1 ) {
        makeItem( pop, HelpMenuInfo, i );
    }

    _clientWin->setMenu( menu );

}

void MenuManager::trackPopup( WWindow * win, MIMainMenu pop )
//-----------------------------------------------------------
{
    if( _clientWin ) {
        win->setPopup( _topMenus[ pop ] );
    }
}

MIMenuID::MIMenuID()
            : _main( 0 )
            , _sub( 0 )
//---------------------
{
}

bool MIMenuID::operator == ( const MIMenuID & o ) const
//-----------------------------------------------------
{
    return( ( _main == o._main ) && ( _sub == o._sub ) );
}
