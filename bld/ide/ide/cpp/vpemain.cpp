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
* Description:  Viper (Open Watcom IDE) mainline.
*
****************************************************************************/


#include <io.h>
#include <stdlib.h>

extern "C" {
    #include "rcdefs.h"
    #include "banner.h"
};

#include "vpemain.hpp"
#include "vmsglog.hpp"

#include "wwinmain.hpp"
#include "wmenu.hpp"
#include "wfiledlg.hpp"
#include "wmsgdlg.hpp"
#include "winpdlg.hpp"
#include "wpickdlg.hpp"
#include "wautodlg.hpp"
#include "weditdlg.hpp"
#include "wfilenam.hpp"
#include "wobjfile.hpp"
#include "wstrlist.hpp"
#include "wserver.hpp"
#include "wclient.hpp"
#include "wprocess.hpp"
#include "mconfig.hpp"

#include "vcompon.hpp"
#include "mcompon.hpp"
#include "maction.hpp"
#include "mtarget.hpp"
#include "mtoolitm.hpp"
#include "vcompdlg.hpp"

#include "wabout.hpp"
#include "wmetrics.hpp"
#include "wsystem.hpp"

#include "wstatwin.hpp"
#include "wflashp.hpp"
#include "vhelpstk.hpp"
#include "veditdlg.hpp"
#include "ide.h"
#include "autoenv.h"

/*
 * increment LATEST_SUPPORTED_VERSION macro 
 * when some change in file formats is done
 */
#define LATEST_SUPPORTED_VERSION 40
/*
 * 39 and 40 are written the same, but read in differently
 * 39 messed up .BEFORE / .AFTER containing CR/LF's
 * since this is the same as the separator char.
 */
#define OLDEST_SUPPORTED_VERSION 23

static char _projectIdent[] = { "projectIdent" };

MAINOBJECT( VpeMain, COORD_USER, 10240, 10240 )

#define _pModel ((MProject*)model())

Define( VpeMain )
static char pFilter[] = { "Project Files (*.wpj)\0*.wpj\0All Files (*.*)\0*.*\0\0" };

static WHotSpots _hotSpotList( 4 );

struct mPop {
    char*       name;
    cbp         onpop;
    MenuData*   menu;
    int         count;
};

struct mData {
    char*       name;
    cbm         callback;
    char*       hint;
    int         toolId;
    MenuPop*    pop;
    char*       tip;
};

char _viperTitle[] = { "Open Watcom IDE" };
char _viperAboutTitle[] = { "About Open Watcom IDE" };
char _viperError[] = { "IDE Error" };
char _viperRequest[] = { "IDE Request" };
char _viperInfo[] = { "IDE Information" };

#ifdef __WATCOMC__
#pragma warning 438 9
#endif

const char* _viperDesc[] = {
    "",
    banner1w1( "Integrated Development Environment" STR_BITNESS ),
    banner1w2( _VIPER_VERSION_ ),
    banner2,
    banner2a( "1993" ),
    banner3,
    banner3a,
    NULL
};

void VpeMain::loadNewEditor( WFileName &fn ) {
    WString errmsg;

    _editorDll.LoadDll( fn.gets(), &errmsg );
    if( !_editorDll.isInitialized() ) {
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, errmsg.gets() );
    }
}

WEXPORT VpeMain::VpeMain()
    : WMdiWindow( _viperTitle )
    , _project( NULL )
    , _neverSaved( FALSE )
    , _activeVComp( NULL )
    , _msgLog( NULL )
    , _batchOk( TRUE )
    , _quitAnyways( FALSE )
    , _targetPopup( NULL )
    , _itemsPopup( NULL )
    , _logPopup( NULL )
    , _toolBarActive( FALSE )
    , _statusBar( NULL )
    , _myHandle( "V1234" )
    , _help( NULL )
    , _otherhelp( NULL )
    , _rcsClient( this )
    , _refuseFileLists( FALSE )
    , _autoRefresh( TRUE )
{
    /* check and fix env vars if needed; this should perhaps be done in
     * the GUI library and not here.
     */
    watcom_setup_env();
    if( getenv( "WATCOM" ) == NULL ) {
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperError,
        "WATCOM environment variable not set.\n"
        "IDE will not function correctly" );
    }

    hookF1Key( TRUE );
#if defined( __OS2__ )
    HelpStack.push( HLP_INDEX_OF_TOPICS );
#else
    HelpStack.push( HLP_TABLE_OF_CONTENTS );
#endif
    _hotSpotList.addHotSpot( B_sourceOpen, "" );        //the order must not change
    _hotSpotList.addHotSpot( B_sourceClosed, "" );
    _hotSpotList.addHotSpot( B_sourceItem, "" );
    _hotSpotList.addHotSpot( B_flashPage, "" );
    _hotSpotList.attach();

    WRect sc;
    WSystemMetrics::screenCoordinates( sc );
    sc.w( sc.w()*3/4 );
    sc.h( sc.h()*19/20 );
    move( sc );

    WProcessInfo proc;
    WString cmd;
    proc.getCommandArgs( cmd );

    bool debug = FALSE;
    bool c_for_pb = FALSE;
    bool c_for_pbnt = FALSE;
    WFileName pj;
    WFileName cfg;
    WStringList parms( cmd );
    int jcount = parms.count();
    for( int j=0; j<jcount; j++ ) {
        if( parms.stringAt( j ) == "-c" ) {
            if( j+1 < parms.count() ) {
                j++;
                cfg = parms.cStringAt( j );
            }
        } else if( parms.stringAt( j ) == "-b" ) {
            _batchOk = FALSE;
        } else if( parms.stringAt( j ) == "-d" ) {
            debug = TRUE;
        } else if( parms.stringAt( j ) == "-pb" ) {
            c_for_pb = TRUE;
        } else if( parms.stringAt( j ) == "-pbnt" ) {
            c_for_pbnt = TRUE;
        } else {
            pj = parms.cStringAt( j );
        }
    }

    setIcon( I_Ide );

    WFlashPage* flash = new WFlashPage( NULL, &_hotSpotList, 4, _viperTitle, _viperDesc );

    new MConfig( cfg, debug );
    if( !_config->ok() ) {
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, (const char*)_config->errMsg() );
    }
    _config->enumAccel( this, (bcbk)&VpeMain::registerAccel );

    _editor = _config->editor();
    _editorIsDll = _config->editorIsDLL();

    buildMenuBar();

    createStatusBar();

    _help = new WSystemHelp( this, _viperTitle, _config->helpFile(), _config->htmlHelpFile() );

    if( !_config->debug() ) {
        _myHandle.printf( "V%04x", proc.processId() );
    }

//    _server = new WServer( _myHandle, _myHandle, this, (sbc)&VpeMain::serverNotify );
//    _editorClient = new WClient( this, (cbc)&VpeMain::editorNotify );
//    _browseClient = new WClient( this, (cbc)&VpeMain::browseNotify );

    if( pj.size() > 0 ) {
        if( c_for_pb ) {
            cForPBProject( pj, FALSE );
        } else if( c_for_pbnt ) {
            cForPBProject( pj, TRUE );
        } else {
            validateProjectName( pj );
            loadProject( pj );
        }
    }
    readIdeInit();
    if( _editorIsDll ) {
        loadNewEditor( _editor );
    }
    if( !_rcsClient.Init() ) {
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperError,
        "Unable to load the source control DLL.\n"
        "Source control functions will not be available" );
    }
    updateView();

    delete flash;
    show( WWinStateShowNormal );
}

WEXPORT VpeMain::~VpeMain()
{
    hookF1Key( FALSE );
//    delete _server;
//    _editorClient->disconnect();
//    delete _editorClient;
//    _browseClient->disconnect();
//    delete _browseClient;

    delete _project;
    delete _config;
    _compViews.deleteContents();
    _oldProjects.deleteContents();
    delete _msgLog;
    if( _editorDll.isInitialized() ) {
        _editorDll.EDITDisconnect();
    }
}

#ifndef NOPERSIST
VpeMain* WEXPORT VpeMain::createSelf( WObjectFile& )
{
    return( NULL );
}

void WEXPORT VpeMain::readSelf( WObjectFile& p )
{
    WRect r; p.readObject( &r );
    if( p.version() < 36 ) {
        r.w( (WOrdinal)((long)r.w()*10240/640) );
        r.h( (WOrdinal)((long)r.h()*10240/480) );
    }
//    move( r );
    if( isMaximized() ) {
        GUIMaximizeWindow( handle() );
    } else {
        size( r.w(), r.h() );
    }
    update();
    show();     //in case this is the first time
    setUpdates( FALSE );
    _project = (MProject*)p.readObject();
    attachModel( _project );
    p.readObject( &_compViews );
    _activeVComp = (VComponent*)p.readObject();
    if( !_activeVComp && _compViews.count() > 0 ) {
        _activeVComp = (VComponent*)_compViews[0];
    }
    if( _activeVComp ) {
        _activeVComp->setFocus();
    }
    setUpdates();
}

typedef int (WObject::*cofn)( WFileName *, WString &, WString & );
void WEXPORT VpeMain::writeSelf( WObjectFile& p )
{
    WRect r; getRectangle( r );
    p.writeObject( &r, FORCE );
    _project->setRCS( &_rcsClient, (cofn)&VRcsClient::Checkout );
    p.writeObject( _project );
    p.writeObject( &_compViews );
    p.writeObject( _activeVComp );
}
#endif

bool VpeMain::registerAccel( WKeyCode kc )
{
    removeAccelKey( kc );
    addAccelKey( kc, this, (bcbk)&VpeMain::kDynAccel );
    return( FALSE );
}


void VpeMain::setStatus( const char* msg )
{
    if( _statusBar ) {
        _statusBar->setStatus( msg );
    }
}

void VpeMain::toolPicked( WToolBarItem* tool )
{
    ToolType typ = ((VToolItem*)tool)->toolType();
    if( typ == TOOL_MENU ) {
        WMenuItem* mitem = (WMenuItem*)tool->tagPtr();
        mitem->picked();
    } else {
        MToolItem* mt = (MToolItem*)tool->tagPtr();
        if( typ == TOOL_PROJECT_ACTION ) {
            doAction( mt->actionName() );
        } else if( _activeVComp ) {
            _activeVComp->setFocus();
            if( typ == TOOL_ITEM_ACTION ) {
                MItem* m = _activeVComp->selectedItem();
                if( m ) {
                    _activeVComp->doAction( m, mt->actionName() );
                }
            } else if( typ == TOOL_TARGET_ACTION ) {
                _activeVComp->doAction( _activeVComp->target(), mt->actionName() );
            }
        }
    }
}

bool VpeMain::toolChanged( WToolBar* /*tool*/, WToolBarState state )
{
    switch( state ) {
        case WToolBarClosed: {
            _toolBarActive = FALSE;
            break;
        }
    }
    return( TRUE );
}

void VpeMain::buildMenuBar()
{
    setUpdates( FALSE );
    delete clearToolBar();
    delete clearMenu();

    removeAccelKey( WKeyInsert );
    removeAccelKey( WKeyDelete );
    removeAccelKey( WKeyCtrlN );
    removeAccelKey( WKeyCtrlO );
    removeAccelKey( WKeyCtrlS );
    removeAccelKey( WKeyDelete );

    WMenu* menuBar = new WMenu();
    VToolBar* toolBar = new VToolBar();
    toolBar->onChanged( this, (cbtb)&VpeMain::toolChanged );

    WPopupMenu* pop0 = makeMenu( &popup0, toolBar );
    menuBar->insertPopup( pop0, 0 );

    WPopupMenu* pop1 = makeMenu( &popup1, toolBar );
    menuBar->insertPopup( pop1, 1 );

    WPopupMenu* pop2 = makeMenu( &popup2, toolBar );
    menuBar->insertPopup( pop2, 2 );
    _targetPopup = pop2;

    WPopupMenu* pop3 = makeMenu( &popup3, toolBar );
    menuBar->insertPopup( pop3, 3 );
    _itemsPopup = pop3;

    WPopupMenu* pop4 = makeMenu( &popup4, toolBar );
    menuBar->insertPopup( pop4, 4 );

    WPopupMenu* pop5 = makeMenu( &popup5, toolBar );
    menuBar->insertPopup( pop5, 5 );
    _logPopup = pop5;

    WPopupMenu* pop6 = getMdiPopup();
    pop6->onPopup( this, (cbp)&VpeMain::onPopup6 );
    menuBar->insertPopup( pop6, 6 );
    pop6->insertItem( new WMenuItem( "&Refresh", this, (cbm)&VpeMain::mRefresh, (cbh)&VpeMain::mHint, "Refresh target and file attributes from disk directories." ), 0 );

    unsigned    helpcnt;
    MAction     *action;
    WString     mname;
    WMenuItem   *mi;
    int         i;

    WPopupMenu* pop7 = makeMenu( &popup7, toolBar );
    menuBar->insertPopup( pop7, 7 );
    helpcnt = _config->helpactions().count();
    if( helpcnt > 0 ) pop7->insertSeparator();
    for( i = 0; i < helpcnt; i++ ) {
        action = (MAction *)_config->helpactions()[i];
        action->menuName( &mname );
        mi = new WMenuItem( mname, this, (cbm)&VpeMain::mHelpItem,
                                   (cbh)&VpeMain::mHint, action->hint() );
        mi->setTagPtr( action );
        pop7->insertItem( mi );
    }
    pop7->insertSeparator();
    mi = new WMenuItem( "&About...", this,
                        (cbm)&VpeMain::about, (cbh)&VpeMain::mHint,
                        "Display program information." );
    pop7->insertItem( mi );

    setMenu( menuBar );

    for( i=0; i<_compViews.count(); i++ ) {
        VComponent* vc = (VComponent*)_compViews[i];
        vc->resetPopups();
    }

    WVList& toolItems = _config->toolItems();
    for( i=0; i<toolItems.count(); i++ ) {
        MToolItem* mt = (MToolItem*)toolItems[i];
        VToolItem* t = new VToolItem( mt->toolType(), mt->toolId(), this,
                       (cbtbi)&VpeMain::toolPicked, mt->hint(), mt->tip() );
        t->setTagPtr( mt );
        toolBar->addTool( t );
    }

    setToolBar( toolBar );
    _toolBarActive = TRUE;

    addAccelKey( WKeyInsert, this, (bcbk)&VpeMain::kAddItem );
    addAccelKey( WKeyDelete, this, (bcbk)&VpeMain::kRemoveItem );
    addAccelKey( WKeyCtrlN, this, (bcbk)&VpeMain::kNewProject );
    addAccelKey( WKeyCtrlO, this, (bcbk)&VpeMain::kOpenProject );
    addAccelKey( WKeyCtrlS, this, (bcbk)&VpeMain::kSaveProject );
    setUpdates();
}

WPopupMenu* VpeMain::makeMenu( MenuPop* popup, VToolBar* tools )
{
    WPopupMenu* pop = new WPopupMenu( popup->name );
    pop->onPopup( this, popup->onpop );
    MenuData* menu = popup->menu;
    if( menu ) {
        for( int i=0; i<popup->count; i++ ) {
            if( menu[i].pop ) {
                WPopupMenu* p = makeMenu( menu[i].pop, tools );
                pop->insertPopup( p, i );
            } else if( !menu[i].name ) {
                pop->insertSeparator( i );
            } else {
                WMenuItem* m = new WMenuItem( menu[i].name, this, menu[i].callback, (cbh)&VpeMain::mHint, menu[i].hint );
                pop->insertItem( m, i );
                if( tools && menu[i].toolId ) {
                    VToolItem* t = new VToolItem( TOOL_MENU, menu[i].toolId, this, (cbtbi)&VpeMain::toolPicked, menu[i].hint, menu[i].tip );
                    t->setTagPtr( m );
                    tools->addTool( t );
                }
            }
        }
    }
    return( pop );
}

MenuPop VpeMain::popup0a = { "Set Sou&rce Control", (cbp)&VpeMain::onPopup0a, menu0a, 7 };
MenuData VpeMain::menu0a[] = {
    "MKS &SI", (cbm)&VpeMain::setMksSi, "Use MKS Source Integrity revision control.", 0, NULL, NULL,
    "MKS &RCS", (cbm)&VpeMain::setMksRcs, "Use MKS RCS revision control.", 0, NULL, NULL,
    "&PVCS", (cbm)&VpeMain::setPvcs, "Use Intersolv PVCS revision control.", 0, NULL, NULL,
    "&Other", (cbm)&VpeMain::setOtherRcs, "Use another revision control system.", 0, NULL, NULL,
    "&None", (cbm)&VpeMain::setNoRcs, "Disable revision control functions.", 0, NULL, NULL,
    "Object &Cycle", (cbm)&VpeMain::setObjectCycle, "Use Object Cycle.", 0, NULL, NULL,
    "P&erforce", (cbm)&VpeMain::setPerforce, "Use Perforce.", 0, NULL, NULL
};

MenuPop VpeMain::popup0 = { "&File", (cbp)&VpeMain::onPopup0, menu0, 16 };
MenuData VpeMain::menu0[] = {
    "&New Project...\tCTRL+N", (cbm)&VpeMain::newProject, "Create a new project.", B_newproj, NULL, "New Project",
    "&Open Project...\tCTRL+O", (cbm)&VpeMain::openProject, "Open existing project.", B_openproj, NULL, "Open Project",
    "&Save Project\tCTRL+S", (cbm)&VpeMain::saveProject, "Save current project.", B_saveproj, NULL, "Save Project",
    "Save Project &As...", (cbm)&VpeMain::saveProjectAs, "Save project with new name.", 0, NULL, NULL,
    "&Close Project", (cbm)&VpeMain::closeProject, "Close current project.", 0, NULL, NULL,
    NULL, NULL, NULL, 0, NULL, NULL,
    "Show &Tool Bar", (cbm)&VpeMain::toolBar, "Show/hide toolbar.", 0, NULL, NULL,
    "Show Stat&us Bar", (cbm)&VpeMain::statusBar, "Show/hide status bar.", 0, NULL, NULL,
    "Auto Refres&h", (cbm)&VpeMain::toggleAutoRefresh, "Turn frequent file list refreshes on/off.", 0, NULL, NULL,
    "Set Text E&ditor...", (cbm)&VpeMain::setEditor, "Set the text editor.", 0, NULL, NULL,
    "", NULL, NULL, 0, &popup0a, NULL,
    NULL, NULL, NULL, 0, NULL, NULL,
    "B&efore...", (cbm)&VpeMain::setBefore, "Setup commands to execute before any make.", 0, NULL, NULL,
    "A&fter...", (cbm)&VpeMain::setAfter, "Setup commands to execute after any successful make.", 0, NULL, NULL,
    NULL, NULL, NULL, 0, NULL, NULL,
    "E&xit", (cbm)&VpeMain::exit, "Exit the Open Watcom IDE.", 0, NULL, NULL,
};

void VpeMain::onPopup0a( WPopupMenu* pop )
{
    int         systype;
    unsigned    i;

    for( i=0; i < popup0a.count; i++ ) {
        pop->checkItem( FALSE, i );
    }
    systype = _rcsClient.QuerySystem();
    switch( systype ) {
    case GENERIC:
        pop->checkItem( TRUE, 3 );
        break;
    case NO_RCS:
        pop->checkItem( TRUE, 4 );
        break;
    case MKS_RCS:
        pop->checkItem( TRUE, 1 );
        break;
    case MKS_SI:
        pop->checkItem( TRUE, 0 );
        break;
    case O_CYCLE:
        pop->checkItem( TRUE, 5 );
        break;
    case PVCS:
        pop->checkItem( TRUE, 2 );
        break;
    case PERFORCE:
        pop->checkItem( TRUE, 6 );
        break;
    }
}

void VpeMain::onPopup0( WPopupMenu* pop )
{
    int base = popup0.count;
    int jcount = pop->childCount();
    for( int j=jcount; j>base; ) {
        j--;
        delete pop->removeItemAt( j );
    }
    if( _oldProjects.count() > 0 ) {
        pop->insertSeparator( base );
        for( int i=0; i<_oldProjects.count(); i++ ) {
            WFileName* fn = (WFileName*)_oldProjects[ i ];
            WString str; str.printf( "&%d. %s", i+1, (const char*)*fn );
            WMenuItem* mi = new WMenuItem( str, this, (cbm)&VpeMain::openOldProject );
            mi->setTagPtr( fn );
            pop->insertItem( mi, i+1+base );
        }
    }
    bool isp = (_project!=NULL);
    pop->enableItem( TRUE, 0 );        //new
    pop->enableItem( TRUE, 1 );        //open
    pop->enableItem( isp, 2 );         //save
    pop->enableItem( isp, 3 );         //save as
    pop->enableItem( isp, 4 );         //close
    pop->enableItem( TRUE, 6 );        //toolbar
    pop->checkItem( _toolBarActive, 6 );//...
    pop->enableItem( TRUE, 7 );        //statusbar
    pop->checkItem( (_statusBar!=NULL), 7 );//...
    pop->enableItem( TRUE, 8 );        //AutoRefresh
    pop->checkItem( _autoRefresh, 8 ); //...
    pop->enableItem( TRUE, 9 );        //set editor
    pop->enableItem( TRUE, 10 );        //set source control
    pop->enableItem( isp, 12 );         //set before
    pop->enableItem( isp, 13 );        //set after
    pop->enableItem( TRUE, 15 );       //always true   //exit
}

MenuPop VpeMain::popup1 = { "&Actions", (cbp)&VpeMain::onPopup1, menu1, 1 };
MenuData VpeMain::menu1[] = {
    "Dummy", (cbm)&VpeMain::mDummy, "Dummy", 0, NULL, NULL
};

void VpeMain::onPopup1( WPopupMenu* pop )
{
    int jcount = pop->childCount();
    for( int j=jcount; j>0; ) {
        j--;
        delete pop->removeItemAt( j );
    }

    int icount = _config->actions().count();
    int i;
    for( i=0; i<icount; i++ ) {
        MAction* action = (MAction*)_config->actions()[i];
        WString  mname;
        action->menuName( &mname );
        WMenuItem* mi = new WMenuItem( mname, this, (cbm)&VpeMain::mAction, (cbh)&VpeMain::mHint, action->hint() );
        mi->setTagPtr( action );
        pop->insertItem( mi, i);
    }
    WMenuItem* mi = new WMenuItem( "Source &Control Shell", this,
                                    (cbm)&VpeMain::mRcsShell,
                                    (cbh)&VpeMain::mHint,
                                    "Invoke Source Control Shell." );
    pop->insertItem( mi, i);
    if( _rcsClient.HasShell() ) {
        pop->enableItem( TRUE, i );
    } else {
        pop->enableItem( FALSE, i );
    }
}

MenuPop VpeMain::popup2 = { "&Targets", (cbp)&VpeMain::onPopup2, menu2, 5 };
MenuData VpeMain::menu2[] = {
    "&New Target...", (cbm)&VpeMain::vAddComponent,"Create a new target.", 0, NULL, NULL,
    "Remo&ve Target", (cbm)&VpeMain::removeComponent, "Remove/delete current target.", 0, NULL, NULL,
    "Ren&ame Target...", (cbm)&VpeMain::renameComponent, "Rename current target.", 0, NULL, NULL,
    "", NULL, NULL, 0, &popup2a, NULL,
    "Mar&k Target for Remake", (cbm)&VpeMain::touchComponent, "Mark current target and (optionally) all its components files for remake.", 0, NULL, NULL
};

void VpeMain::onPopup2( WPopupMenu* pop )
{
    int base = popup2.count;
    int jcount = pop->childCount();
    for( int j=jcount; j>base; ) {
        j--;
        delete pop->removeItemAt( j );
    }
    if( _activeVComp ) {
        MItem* m = _activeVComp->target();
        WVList actlist;
        if( m ) {
            m->addActions( actlist );
            int icount = actlist.count();
            int ii = 0;
            for( int i=0; i<icount; i++ ) {
                MAction* action = (MAction*)actlist[i];
                if( !action->button() ) {
                    if( ii == 0 ) {
                        pop->insertSeparator( base );
                        ii++;
                    }
                    WString  mname;
                    action->menuName( &mname );
                    WMenuItem* mi = new WMenuItem( mname, this, (cbm)&VpeMain::mActionComponent, (cbh)&VpeMain::mHint, action->hint() );
                    mi->setTagPtr( action );
                    pop->insertItem( mi, ii+base );
                    ii++;
                }
            }
        }
    }
    bool isp = (_project!=NULL);
    bool ist = (_activeVComp!=NULL);
    pop->enableItem( isp, 0 );         //new target
    pop->enableItem( ist, 1 );         //remove target
    pop->enableItem( ist, 2 );         //rename target
    pop->enableItem( ist, 3 );         //setup target
    pop->enableItem( ist, 4 );         //touch target
}

MenuPop VpeMain::popup2a = { "Target &Options", (cbp)&VpeMain::onPopup2a, menu2a, 10 };
MenuData VpeMain::menu2a[] = {
    "&Switches...", (cbm)&VpeMain::setupComponent, "Set switch values for making current target.", 0, NULL, NULL,
    "Show Make &Command...", (cbm)&VpeMain::showCompCommand, "Show command for making current target.", 0, NULL, NULL,
    NULL, NULL, NULL, 0, NULL, NULL,
    "Execute B&efore...", (cbm)&VpeMain::setCompBefore, "Setup commands to execute before making current target.", 0, NULL, NULL,
    "Execute A&fter...", (cbm)&VpeMain::setCompAfter, "Setup commands to do after successfully making current target.", 0, NULL, NULL,
    NULL, NULL, NULL, 0, NULL, NULL,
    "Use &Autodepend", (cbm)&VpeMain::mAutodepend, "Enable/disable autodepend feature of makefile.", 0, NULL, NULL,
    "Use &Autotrack", (cbm)&VpeMain::mAutotrack, "Enable/disable target autotrack feature.", 0, NULL, NULL,
    "Use De&velopment Switches", (cbm)&VpeMain::mDebugMode, "Use development switches during make.", 0, NULL, NULL,
    "Use Re&lease Switches", (cbm)&VpeMain::mDebugMode, "Use release switches during make.", 0, NULL, NULL
};

void VpeMain::onPopup2a( WPopupMenu* pop )
{
    int base = popup2a.count;
    int jcount = pop->childCount();
    for( int j=jcount; j>base; ) {
        j--;
        delete pop->removeItemAt( j );
    }

    addComponentActionSetups( pop, base );

    WString ss;
    if( _activeVComp ) {
        MTool* tool = _activeVComp->target()->rule()->tool();
        if( tool != _config->nilTool() ) {
            tool->name( ss );
            ss.concat( ' ' );
        }
    }
    ss.concat( menu2a[0].name );
    pop->setItemText( ss, 0 );                //set target switch text

    bool isv = (_activeVComp!= NULL);
    pop->enableItem( isv, 0 );                //setup target
    pop->enableItem( isv, 1 );                //show command
    pop->enableItem( isv, 3 );                //set before
    pop->enableItem( isv, 4 );                //set after
    bool isa = isv && _activeVComp->target()->rule()->autodepend();
    pop->enableItem( isa, 6 );                //autodepend
    pop->checkItem( isa && _activeVComp->component()->autodepend(), 6 );      //...
    bool ist = isv && _activeVComp->target()->rule()->autotrack();
    pop->enableItem( ist, 7 );                //autotrack
    pop->checkItem( ist && _activeVComp->component()->autotrack(), 7 );       //...
    bool isd = isv && _activeVComp->debugMode();
    pop->enableItem( isv && !isd, 8 );        //debug mode
    pop->checkItem( isv && isd, 8 );          //...
    pop->enableItem( isv && isd, 9 );         //release mode
    pop->checkItem( isv && !isd, 9 );         //...
}

MenuPop VpeMain::popup3 = { "&Sources", (cbp)&VpeMain::onPopup3, menu3, 9 };
MenuData VpeMain::menu3[] = {
    "Check &Out Source", (cbm)&VpeMain::mCheckout, "Check out the current source file.", 0, NULL, NULL,
    "&Check In Source", (cbm)&VpeMain::mCheckin, "Check in the current source file.", 0, NULL, NULL,
    NULL, NULL, NULL, 0, NULL, NULL,
    "&New Source...\tIns", (cbm)&VpeMain::mAddItem, "Create/add a new source file to the target.", 0, NULL, NULL,
    "Remo&ve Source\tDel", (cbm)&VpeMain::mRemoveItem, "Remove current source file from target.", 0, NULL, NULL,
    "Ren&ame Source...", (cbm)&VpeMain::mRenameItem, "Rename current source file in the target.", 0, NULL, NULL,
    "", NULL, NULL, 0, &popup3a, NULL,
    "Mar&k Source for Remake", (cbm)&VpeMain::mTouchItem, "Mark current source file for remake.", 0, NULL, NULL,
    "&Included Files...", (cbm)&VpeMain::mIncludedItems, "Edit included source files.", 0, NULL, NULL
};

void VpeMain::onPopup3( WPopupMenu* pop )
{
    int base = popup3.count;
    int jcount = pop->childCount();
    for( int j=jcount; j>base; ) {
        j--;
        delete pop->removeItemAt( j );
    }

    MItem* m = NULL;
    if( _activeVComp ) {
        m = _activeVComp->selectedItem();
    }
    if( m ) {
        WVList actlist;
        m->addActions( actlist );
        int icount = actlist.count();
        if( icount > 0 ) {
            pop->insertSeparator( base );
            int addcnt = 0;
            for( int i=0; i<icount; i++ ) {
                MAction* action = (MAction*)actlist[i];
                if( !m->isMask() || action->okForMask() ) {
                    WString      mname;
                    action->menuName( &mname );
                    WMenuItem* mi = new WMenuItem( mname, this,
                                                   (cbm)&VpeMain::mActionItem,
                                                   (cbh)&VpeMain::mHint,
                                                   action->hint() );
                    mi->setTagPtr( action );
                    pop->insertItem( mi, addcnt + base + 1 );
                    addcnt ++;
                }
            }
        }
    }
    bool isv = (_activeVComp!=NULL);
    bool isp = (m != NULL);
    bool ism = isp && m->isMask();
    bool rcs = isp && !ism && _rcsClient.QuerySystem() != NO_RCS;
    pop->enableItem( rcs, 0 ); //checkin item
    pop->enableItem( rcs, 1 ); //checkout item
    pop->enableItem( isv, 3 );         //add item
    pop->enableItem( isp, 4 );         //remove item
    pop->enableItem( isp && !ism, 5 ); //rename item
    pop->enableItem( isp, 6 );         //setup item
    pop->enableItem( isp , 7 );        //touch item
    pop->enableItem( isp && !ism && m->ismakeable(), 8 );//included items
}

MenuPop VpeMain::popup3a = { "Source &Options", (cbp)&VpeMain::onPopup3a, menu3a, 2 };
MenuData VpeMain::menu3a[] = {
    "&Switches...", (cbm)&VpeMain::mSetupItem, "Set switch values for making current source file.", 0, NULL, NULL,
    "Show Make &Command...", (cbm)&VpeMain::showItemCommand, "Show command for making current source file.", 0, NULL, NULL
};

void VpeMain::onPopup3a( WPopupMenu* pop )
{
    int base = popup3a.count;
    int jcount = pop->childCount();
    for( int j=jcount; j>base; ) {
        j--;
        delete pop->removeItemAt( j );
    }

    MItem* m = NULL;
    if( _activeVComp ) {
        m = _activeVComp->selectedItem();
    }
    if( m ) {
        WVList actlist;
        m->addActions( actlist );
        int icount = actlist.count();
        int ii = 0;
        for( int i=0; i<icount; i++ ) {
            MAction* action = (MAction*)actlist[i];
            if( action->hasSwitches( TRUE ) ) {
                if( ii == 0 ) {
                    pop->insertSeparator( base );
                    ii++;
                }
                WString n( action->name() ); n.concat( " Switches..." );
                WString h( "Setup switches for '" );
                action->text( h );
                h.concat( "' action on selected source file." );
                WMenuItem* mi = new WMenuItem( n, this, (cbm)&VpeMain::mActionItemSetup, (cbh)&VpeMain::mHint, h );
                mi->setTagPtr( action );
                pop->insertItem( mi, ii+base );
                ii++;
            }
        }
    }

    WString ss;
    if( m ) {
        MTool* tool = m->rule()->tool();
        if( tool != _config->nilTool() ) {
            tool->name( ss );
            ss.concat( ' ' );
        }
    }
    ss.concat( menu3a[0].name );
    pop->setItemText( ss, 0 );                //set item switch text

    bool isp = (m != NULL);
    pop->enableItem( isp && m->ismakeable(), 0 );     //setup item
    pop->enableItem( isp && m->ismakeable(), 1 );     //show command
}

MenuPop VpeMain::popup4 = { "&Options", (cbp)&VpeMain::onPopup4, menu4, 1 };
MenuData VpeMain::menu4[] = {
    "&Switches...", (cbm)&VpeMain::setupComponent, "Set switch values for making current target.", 0, NULL, NULL
};

void VpeMain::onPopup4( WPopupMenu* pop )
{
    int base = popup4.count;
    int jcount = pop->childCount();
    for( int j=jcount; j>base; ) {
        j--;
        delete pop->removeItemAt( j );
    }

    //add switch setting for current target
    WString ss;
    if( _activeVComp ) {
        MTool* tool = _activeVComp->target()->rule()->tool();
        if( tool != _config->nilTool() ) {
            tool->name( ss );
            ss.concat( ' ' );
        }
    }
    ss.concat( menu4[0].name );
    pop->setItemText( ss, 0 );                //set target switch text

    int ccount = base;
    //add switch setting for all make-actions of current item
    ccount += addComponentActionSetups( pop, ccount );

    //for each 'mask' in current target, add switch setting for that item
    ccount += addComponentMaskSetups( pop, ccount );

    bool isv = (_activeVComp != NULL);
    pop->enableItem( isv, 0 );                //setup target
}

MenuPop VpeMain::popup5 = { "&Log", (cbp)&VpeMain::onPopup5, menu5, 5 };
MenuData VpeMain::menu5[] = {
    "&Edit File", (cbm)&VpeMain::editLog, "Edit file whose name is in the message.", 0, NULL, NULL,
    "&Help on Message", (cbm)&VpeMain::helpLog, "Display help on the message.", 0, NULL, NULL,
    "&Stop", (cbm)&VpeMain::cancelLog, "Stop the executing operation.", 0, NULL, NULL,
    NULL, NULL, NULL, 0, NULL, NULL,
    "Save Log &As...", (cbm)&VpeMain::saveLogAs, "Save contents of the IDE log window to a disk file.", 0, NULL, NULL
};

void VpeMain::onPopup5( WPopupMenu* pop )
{
    bool editOk = FALSE;
    bool helpOk = FALSE;
    if( _msgLog ) {
        _msgLog->getState( editOk, helpOk );
    }
    pop->enableItem( editOk, 0 );      //edit
    pop->enableItem( helpOk, 1 );      //help
    pop->enableItem( running(), 2 );   //stop
    bool islog = _msgLog && _msgLog->logExists();
    pop->enableItem( islog && !running(), 4 );  //save
}

void VpeMain::onPopup6( WPopupMenu* pop )
{
    bool isp = (_project!=NULL);
    pop->enableItem( isp, 0 ); //refresh
}

MenuPop VpeMain::popup7 = { "&Help", (cbp)&VpeMain::onPopup7, menu7, 4 };
MenuData VpeMain::menu7[] = {
    "&Contents", (cbm)&VpeMain::helpContents, "List Open Watcom IDE help topics.", 0, NULL, NULL,
    "&Search for Help On...", (cbm)&VpeMain::helpSearch, "Search for help on a specific topic.", 0, NULL, NULL,
    NULL, NULL, NULL, 0, NULL, NULL,
    "&How to Use Help", (cbm)&VpeMain::helpUsage, "Display information on how to use help.", 0, NULL, NULL
};

void VpeMain::onPopup7( WPopupMenu* pop )
{
    bool hp = ( _config->helpFile().size() > 0 );
    pop->enableItem( hp, 0 );                  //contents
    pop->enableItem( hp, 1 );                  //search for help
    pop->enableItem( hp, 3 );                  //help on help
    pop->enableItem( TRUE, 4 );                //about
}

int VpeMain::addComponentActionSetups( WPopupMenu* pop, int base )
{
    int ii = 0;
    if( _activeVComp ) {
        MItem* m = _activeVComp->target();
        WVList actlist;
        if( m ) {
            m->addActions( actlist );
            int icount = actlist.count();
            for( int i=0; i<icount; i++ ) {
                MAction* action = (MAction*)actlist[i];
                if( action->hasSwitches( TRUE ) ) {
                    if( ii == 0 ) {
                        pop->insertSeparator( base );
                        ii++;
                    }
                    WString n( action->name() ); n.concat( " Switches..." );
                    WString h( "Setup switches for '" );
                    action->text( h );
                    h.concat( "' action on selected target." );
                    WMenuItem* mi = new WMenuItem( n, this, (cbm)&VpeMain::mActionComponentSetup, (cbh)&VpeMain::mHint, h );
                    mi->setTagPtr( action );
                    pop->insertItem( mi, ii+base );
                    ii++;
                }
            }
        }
    }
    return( ii );
}

int VpeMain::addComponentMaskSetups( WPopupMenu* pop, int base )
{
    int ii = 0;
    if( _activeVComp ) {
        MComponent* comp = _activeVComp->component();
        WVList itemlist;
        comp->getMaskItems( itemlist );
        int icount = itemlist.count();
        for( int i=0; i<icount; i++ ) {
            MItem* m = (MItem*)itemlist[i];
            MTool* tool = m->rule()->tool();
            if( tool != _config->nilTool() ) {
                if( ii == 0 ) {
                    pop->insertSeparator( base );
                    ii++;
                }
                WString n;
                tool->name( n );
                n.concat( " Switches..." );
                WString h( "Setup switches for '" );
                tool->name( h );
                h.concat( "' in selected target." );
                WMenuItem* mi = new WMenuItem( n, this, (cbm)&VpeMain::mSetupItem2, (cbh)&VpeMain::mHint, h );
                mi->setTagPtr( m );
                pop->insertItem( mi, ii+base );
                ii++;
            }
        }
    }
    return( ii );
}

bool VpeMain::keyDown( WORD key )
{
    if( key == GUI_KEY_F6 ) {
        if( _editorClient->connected() ) {
            _editorClient->sendMsg( "TakeFocus" );
        }
        return( TRUE );
    }
    return( FALSE );
}

bool VpeMain::gettingFocus( WWindow* )
{
    if( _activeVComp ) {
        _activeVComp->setFocus();
        return( TRUE );
    }
    return( FALSE );
}

bool VpeMain::appActivate( bool activated ) {
    if( activated && _project != NULL ) {
        if( _autoRefresh ) {
            _project->refresh( FALSE );
        } else {
            _project->refresh( TRUE );
        }
    }
    return( FALSE );
}

bool VpeMain::validateProjectName( WFileName& fn )
{
    if( fn.size() > 0 ) {
//        fn.toLower();
        if( fn.legal() ) {
            if( strlen( fn.ext() ) == 0 ) {
                fn.setExt( ".wpj" );
            }
            return( TRUE );
        }
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "Project name '%s' is invalid", (const char*)fn );
    }
    return( FALSE );
}

bool VpeMain::kNewProject( WKeyCode )
{
    newProject( NULL );
    return( FALSE );
}

void VpeMain::newProject( WMenuItem* )
{
    if( okToClear() ) {
        HelpStack.push( HLP_OPENING_A_PROJECT );
        WFileDialog dlg( this, pFilter );
        WFileName fn( dlg.getOpenFileName( "noname.wpj", "Enter project filename", WFOpenNewAll ) );
        if( validateProjectName( fn ) ) {
            if( !fn.dirExists() ) {
                if( !confirm( "Do you want to create directory for '%s'?", fn ) ) {
                    HelpStack.pop();
                    return;
                }
                if( !fn.makeDir() ) {
                    WMessageDialog::messagef( this, MsgError, MsgOk, _viperError,
                                "Unable to create directory for '%s'",
                                (const char *)fn );
                    HelpStack.pop();
                    return;
                }
            }
            clearProject();
            _project = new MProject( fn );
            attachModel( _project );
            if( addComponent( NULL ) ) {
                _neverSaved = TRUE;
            }
        }
    }
    HelpStack.pop();
}

bool VpeMain::loadProject( const WFileName& fn )
{
    WObjectFile of;
    if( of.open( fn, OStyleReadB ) ) {
        fn.setCWD();    //so the load will work for relative filenames
        if( of.version() < OLDEST_SUPPORTED_VERSION ) {
            WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "Project '%s' format is obsolete; you must create the project again or use an older version of the IDE.", (const char*)fn );
        } else if( of.version() > LATEST_SUPPORTED_VERSION ) {
            WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "Project '%s' format is too new; you must use a newer version of the IDE.", (const char*)fn );
        } else {
            if( of.version() >= 34 ) {
                char ident[ sizeof( _projectIdent ) + 1 ];
                of.readObject( ident, sizeof( ident ) - 1 );
                if( !streq( ident, _projectIdent ) ) {
                    WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "Project '%s' format is bad.", (const char*)fn );
                    of.close();
                    return( FALSE );
                }
            }
            setStatus( "Loading..." );
            startWait();
            of.readObject( this );
            of.close();
            stopWait();
            setStatus( NULL );
            return( TRUE );
        }
        of.close();
        return( FALSE );
    }
    WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "Unable to open project '%s'", (const char*)fn );
    return( FALSE );
}

bool VpeMain::unloadProject( const WFileName& fn, bool checkout )
{
    bool ok = TRUE;
    setStatus( "Saving..." );
    startWait();
    WObjectFile of( LATEST_SUPPORTED_VERSION );
    if( !of.open( fn, OStyleWriteB ) ) {
        WFileName proj = _project->filename();
        WFileName targ = ""; // no target
        if( checkout && _rcsClient.QuerySystem() != NO_RCS ) {
            if( WMessageDialog::messagef( this, MsgQuestion, MsgOkCancel,
                _viperRequest, "Project file '%s' is read-only. checkout? ",
                (const char*)fn ) == MsgRetOk ) {
                _rcsClient.Checkout( (WFileName*)&fn, proj, targ );
                unloadProject( fn, FALSE );
            } else {
                WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "Unable to write project file '%s'", (const char*)fn );
                ok = FALSE;
            }
        } else {
                WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "Unable to write project file '%s'", (const char*)fn );
                ok = FALSE;
        }
    } else {
        of.writeObject( _projectIdent );
        of.writeObject( this );
        of.close();
        ok = of.objOk();
        if( ok ) {
            _neverSaved = FALSE;
        }
    }
    stopWait();
    setStatus( NULL );
    return( ok );
}

bool VpeMain::kOpenProject( WKeyCode ) {
    openProject( NULL );
    return FALSE;
}

void VpeMain::openProject( WMenuItem* )
{
    if( okToClear() ) {
        HelpStack.push( HLP_OPENING_A_PROJECT );
        WFileDialog* fd = new WFileDialog( this, pFilter );
//      WFileName fn( fd->getOpenFileName( NULL, "Open", WFOpenExisting ) );
        WFileName fn( fd->getOpenFileName( NULL, "Open", WFOpenNew ) );
        if( validateProjectName( fn ) ) {
            clearProject();
            if( loadProject( fn ) ) {
                addOldProject( fn );
            }
        }
        delete fd;
        HelpStack.pop();
    }
}

void VpeMain::addOldProject( const WFileName& filename )
{
    int i;

    for( i=0; i<_oldProjects.count(); i++ ) {
        WFileName* fn = (WFileName*)_oldProjects[ i ];
        if( *fn == filename ) {
            delete _oldProjects.removeAt( i );
            break;
        }
    }
    _oldProjects.insertAt( 0, new WFileName( filename ) );
    for( i=_oldProjects.count(); i > MAXOLDPROJECTS; ) {
        i--;
        delete _oldProjects.removeAt( i );
    }
}

void VpeMain::openOldProject( WMenuItem* m )
{
    if( okToClear() ) {
        WFileName* fn = (WFileName*)m->tagPtr();
        if( fn->size() > 0 ) {
            clearProject();
            if( loadProject( *fn ) ) {
                _oldProjects.removeSame( fn );  //move to the front
                _oldProjects.insertAt( 0, fn );
            }
        }
    }
}

void VpeMain::closeProject( WMenuItem* )
{
    if( okToClear() ) {
        clearProject();
    }
}


bool VpeMain::okToReplace( WFileName& fn )
{
    if( fn.attribs() ) {
        if( !confirm( "File '%s' exists! Ok to replace?", fn ) ) {
            return( FALSE );
        }
    }
    return( TRUE );
}

bool VpeMain::kSaveProject( WKeyCode )
{
    saveProject();
    return( TRUE );
}

bool VpeMain::saveProject()
{
    if( checkProject() ) {
        WFileName& fn = _project->filename();
        if( !fn.legal() ) {
            WFileName def;
            _project->filename().noPath( def );
            return( saveProjectAs( def ) );
        } else if( !_neverSaved || okToReplace( fn ) ) {
            addOldProject( fn );
            return( unloadProject( fn ) );
        }
    }
    return( FALSE );
}

bool VpeMain::saveProjectAs( const WFileName& def )
{
    if( checkProject() ) {
        WFileDialog fd( this, pFilter );
        WFileName fn( fd.getSaveFileName( def, "Save Project as", WFSaveDefault ) );
        if( fn.legal() && okToReplace( fn ) ) {
//            fn.toLower();
            _project->setFilename( fn );
            addOldProject( fn );
            return( unloadProject( fn ) );
        }
    }
    return( FALSE );
}

void VpeMain::setBefore( WMenuItem* )
{
    HelpStack.push( HLP_EXECUTING_SPECIAL_COMMANDS );
    WEditDialog ed( this, ".Before text" );
    MCommand txt( _project->before() );
    if( ed.edit( txt ) ) {
        _project->setBefore( txt );
    }
    HelpStack.pop();
}

void VpeMain::setAfter( WMenuItem* )
{
    HelpStack.push( HLP_EXECUTING_SPECIAL_COMMANDS );
    WEditDialog ed( this, ".After text" );
    MCommand txt( _project->after() );
    if( ed.edit( txt ) ) {
        _project->setAfter( txt );
    }
    HelpStack.pop();
}

void VpeMain::setCompBefore( WMenuItem* )
{
    HelpStack.push( HLP_EXECUTING_SPECIAL_COMMANDS );
    _activeVComp->setFocus();
    _activeVComp->setCompBefore();
    HelpStack.pop();
}

void VpeMain::setCompAfter( WMenuItem* )
{
    HelpStack.push( HLP_EXECUTING_SPECIAL_COMMANDS );
    _activeVComp->setFocus();
    _activeVComp->setCompAfter();
    HelpStack.pop();
}

bool VpeMain::optionToSave()
{
    MsgRetType ret = WMessageDialog::messagef( this, MsgQuestion, MsgYesNoCancel, _viperRequest,
        "The project %s is being closed. Do you want to save the changes that were made in this session?", (const char*)_project->filename() );
    if( ret == MsgRetCancel ) {
        return( FALSE );
    }
    if( ret == MsgRetYes ) {
        return( saveProject() );
    }
    return( TRUE );
}

bool VpeMain::okToQuit()
{
    if( running() ) {
        MsgRetType ret = WMessageDialog::messagef( this, MsgInfo, MsgOkCancel, NULL,
            "Batch execution is busy. Quit anyways?" );
        if( ret == MsgRetOk ) {
            _quitAnyways = TRUE;
            _msgLog->killBatcher();
        }
        return( FALSE );
    } else if( _project && _project->isDirty() ) {
        return( optionToSave() );
    }
    return( TRUE );
}

bool VpeMain::okToClear()
{
    if( running() ) {
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "Can't close project while batch execution is active" );
        return( FALSE );
    } else if( _project && _project->isDirty() ) {
        return( optionToSave() );
    }
    return( TRUE );
}

void VpeMain::clearProject()
{
    setUpdates( FALSE );
    setStatus( "Clearing project..." );
    delete _project;
    _project = NULL;
    _neverSaved = FALSE;
    _activeVComp = NULL;
    _compViews.deleteContents();
    deleteMsglog();
    setUpdates();
    setStatus( NULL );
}

void VpeMain::deleteMsglog()
{
    delete _msgLog;
    _msgLog = NULL;
}

void VpeMain::toolBar( WMenuItem* )
{
    if( _toolBarActive ) {
        delete clearToolBar();
        _toolBarActive = FALSE;
    } else {
        buildMenuBar();
    }
}

void VpeMain::createStatusBar()
{
    if( !_statusBar ) {
        int h = getTextExtentY( "X" ) * 3/2;
        _statusBar = new WStatWindow( this, WRect( 0,-h,-1,h), "Ready" );
        _statusBar->show();
    }
}

void VpeMain::deleteStatusBar()
{
    delete _statusBar;
    _statusBar = NULL;
}


void VpeMain::setEditor( WMenuItem* ) {

    VEditDlg            dlg( this, _editor, _editorParms, _editorIsDll );
    WFileName           fn( _editor );
    WString             parms( _editorParms );
    bool                isdll;

    isdll = _editorIsDll;
    if( dlg.process( fn, parms, isdll ) ) {
        if( isdll != _editorIsDll ||
            !( fn == _editor ) || !( parms == _editorParms )  ) {
            _editorIsDll = isdll;
            _editor = fn;
            _editorParms = parms;
            _editorDll.UnloadDll();
            if( _editorIsDll ) {
                loadNewEditor( _editor );
            }
        }
    }
}

void VpeMain::statusBar( WMenuItem* )
{
    if( _statusBar ) {
        deleteStatusBar();
    } else {
        createStatusBar();
    }
}

bool VpeMain::createDirectory( const WFileName& f )
{
    WFileName d( f );
    d.absoluteTo( _project->filename() );
    if( !d.dirExists() ) {
        if( !confirm( "Do you want to create directory for '%s'?", f ) ) {
            return( FALSE );
        }
        if( !d.makeDir() ) {
            WMessageDialog::messagef( this, MsgError, MsgOk, _viperError,
                        "Unable to create directory for '%s'", (const char *)f );
            return( FALSE );
        }
    }
    return( TRUE );
}

#if 0
char* VpeMain::getFilters()
{
    WStringList filtList;
    int maxLen = 0;
    WPickList& rules = _config->rules();
    int icount = rules.count();
    for( int i=0; i<icount; i++ ) {
        MRule* r = (MRule*)rules[i];
        if( r->ismakeable() ) {
            WString* name = new WString();
            r->name( *name );
            name->concatf( "(%s)", (char*)r->mask() );
            filtList.add( name );
            maxLen += name->size() + 1;
            filtList.add( new WString( r->mask() ) );
            maxLen += r->mask().size() + 1;
        }
    }
    filtList.add( new WString( "All Files(*.*)" ) );
    maxLen += 14 + 1;
    filtList.add( new WString( "*.*" ) );
    maxLen += 3 + 1;
    char* filts = new char[ maxLen + 1 ];
    int off = 0;
    icount = filtList.count();
    for( i=0; i<icount; i++ ) {
        char* p = filtList.cStringAt( i );
        strcpy( &filts[off], p );
        off += strlen( p ) + 1;
    }
    filts[off] = '\0';
    return( filts );
}
#endif

WStyle VpeMain::vCompStyle()
{
    if( _activeVComp ) {
        if( !_activeVComp->isMaximized() ) {
            return( WStyleDefault );
        }
    }
    return( WStyleDefault | WStyleMaximize );
}

bool VpeMain::addComponent( WMenuItem* )
{
static char cFilter[] = { "Target Files (*.tgt)\0*.tgt\0Executables (*.exe)\0*.exe\0Static Libraries (*.lib)\0*.lib\0Dynamic Libraries (*.dll)\0*.dll\0All Files (*.*)\0*.*\0\0" };
    bool ok = FALSE;
    HelpStack.push( HLP_ADDING_A_TARGET );
    VCompDialog dlg( this, "New Target", _project, cFilter );
    WFileName fn( _project->filename().fName() );
    MRule* rule;
    WString mask;
    if( dlg.process( fn, &rule, mask, NULL ) ) {
        setStatus( "Adding.." );
        startWait();
        if( strieq( fn.ext(), ".tgt" ) ) {
            ok = attachTgtFile( fn );
        } else {
            if( createDirectory( fn ) ) {
                MComponent* mcomp = new MComponent( _project, rule, mask, fn );
                _project->addComponent( mcomp );
                VComponent* vcomp = new VComponent( this, mcomp, vCompStyle() );
                _compViews.add( vcomp );
                mcomp->updateItemList();
                ok = TRUE;
            }
        }
        stopWait();
        setStatus( NULL );
    }
    HelpStack.pop();
    return( ok );
}

void VpeMain::vAddComponent( WMenuItem* )
{
    addComponent();
}

bool VpeMain::attachTgtFile( WFileName& fn )
{
    bool ok = FALSE;
    MComponent* mcomp = _project->attachComponent( fn );
    if( !mcomp ) {
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "Unable to read file '%s'", (const char*)fn );
    } else {
        VComponent* vcomp = new VComponent( this, mcomp, vCompStyle() );
        _compViews.add( vcomp );
        ok = TRUE;
    }
    return( ok );
}

void VpeMain::removeComponent( WMenuItem* )
{
    _activeVComp->setFocus();
    MComponent* comp = (MComponent*)_activeVComp->model();
    WFileName target( *comp->target() );
    target.setExt( ".tgt" );
    if( confirm( "Are you sure you wish to remove '%s'?", target ) ) {
        delete _compViews.removeSame( _activeVComp );
        delete _project->removeComponent( comp );
    }
}

void VpeMain::renameComponent( WMenuItem* )
{
    HelpStack.push( HLP_RENAMING_A_TARGET );
    _activeVComp->setFocus();
    VComponent* vcomp = _activeVComp;
//static char cFilter[] = { "Executables (*.exe)\0*.exe\0Static Libraries (*.lib)\0*.lib\0Dynamic Libraries (*.dll)\0*.dll\0All Files (*.*)\0*.*\0\0" };
static char cFilter[] = { "Target Files (*.tgt)\0*.tgt\0Executables (*.exe)\0*.exe\0Static Libraries (*.lib)\0*.lib\0Dynamic Libraries (*.dll)\0*.dll\0All Files(*.*)\0*.*\0\0" };
    VCompDialog dlg( this, "Rename Target", _project, cFilter );
    MComponent* comp = vcomp->component();
    WFileName fn( comp->relFilename() );
    fn.setExt( vcomp->target()->ext() );
    MRule* rule;
    WString mask;
    if( dlg.process( fn, &rule, mask, comp ) ) {
        if( strieq( fn.ext(), ".tgt" ) ) {
            WRect rr; vcomp->getRectangle( rr );
            if( attachTgtFile( fn ) ) {
                VComponent* newvc = _activeVComp;
                delete _compViews.removeSame( vcomp );
                delete _project->removeComponent( comp );
                _activeVComp = newvc;
                _activeVComp->move( rr );
                _activeVComp->setFocus();
            }
        } else {
            vcomp->renameComponent( fn, rule, mask );
        }
    }
    HelpStack.pop();
}

void VpeMain::setupComponent( WMenuItem* )
{
    _activeVComp->setFocus();
    _activeVComp->setupComponent();
}

void VpeMain::showCompCommand( WMenuItem* )
{
    _activeVComp->setFocus();
    _activeVComp->showCompCommand();
}

void VpeMain::touchComponent( WMenuItem* )
{
    _activeVComp->setFocus();
    _activeVComp->touchComponent();
}

void VpeMain::mAutodepend( WMenuItem* )
{
    _activeVComp->setFocus();
    _activeVComp->mAutodepend();
}

void VpeMain::mAutotrack( WMenuItem* )
{
    _activeVComp->setFocus();
    _activeVComp->mAutotrack();
}

void VpeMain::mDebugMode( WMenuItem* )
{
    _activeVComp->setFocus();
    _activeVComp->mDebugMode();
}

void VpeMain::mHint( WMenuItem*, const char* hint )
{
    setStatus( hint );
}

void VpeMain::mAddItem( WMenuItem* )
{
    kAddItem( WKeyNone );
}

bool VpeMain::kAddItem( WKeyCode  )
{
    bool        ret;

    ret = FALSE;
    if( _activeVComp ) {
        HelpStack.push( HLP_ADDING_SOURCE_FILES );
        _activeVComp->setFocus();
        _activeVComp->mAddItem();
        ret = TRUE;
        HelpStack.pop();
    }
    return( ret );
}

bool VpeMain::kDynAccel( WKeyCode kc ) {

    bool        done;
    MAction     *action;
    int         i;

    done = FALSE;
    // search project level accelerators
    int icount = _config->actions().count();
    for( i=0; i<icount; i++ ) {
        action = (MAction*)_config->actions()[i];
        if( action->menuAccel() == kc ) {
            doAction( action );
            done = TRUE;
            break;
        }
    }
    if( _activeVComp ) {
        MItem   *m;
        WVList  actlist;
        // search target level accelerators
        if( !done ) {
            m = _activeVComp->target();
            if( m ) {
                m->addActions( actlist );
                icount = actlist.count();
                for( i=0; i<icount; i++ ) {
                    action = (MAction*)actlist[i];
                    if( action->menuAccel() == kc ) {
                        _activeVComp->doAction( _activeVComp->target(), action );
                        done = TRUE;
                        break;
                    }
                }
            }
        }
        // search source level accelerators
        if( !done ) {
            m = _activeVComp->selectedItem();
            actlist.reset();
            if( m ) {
                m->addActions( actlist );
                icount = actlist.count();
                for( i=0; i<icount; i++ ) {
                    action = (MAction*)actlist[i];
                    if( action->menuAccel() == kc ) {
                        _activeVComp->doAction( _activeVComp->selectedItem(), action );
                        done = TRUE;
                        break;
                    }
                }
            }
        }
    }
    return( TRUE );
}

void VpeMain::mRemoveItem( WMenuItem* )
{
    kRemoveItem( WKeyNone );
}

bool VpeMain::kRemoveItem( WKeyCode )
{
    bool        ret;

    ret = FALSE;
    _refuseFileLists = TRUE;
    HelpStack.push( HLP_REMOVING_A_SOURCE_FILE );
    if( _activeVComp ) {
        _activeVComp->setFocus();
        if( _activeVComp->selectedItem() ) {
            _activeVComp->mRemoveItem();
            ret = TRUE;
        }
    }
    _refuseFileLists = FALSE;
    HelpStack.pop();
    return( ret );
}

void VpeMain::mRenameItem( WMenuItem* )
{
    HelpStack.push( HLP_RENAMING_A_SOURCE_FILE );
    _refuseFileLists = TRUE;
    _activeVComp->setFocus();
    _activeVComp->mRenameItem();
    _refuseFileLists = FALSE;
    HelpStack.pop();
}

void VpeMain::mSetupItem( WMenuItem* )
{
    _refuseFileLists = TRUE;
    _activeVComp->setFocus();
    _activeVComp->setupItem( _activeVComp->selectedItem() );
    _refuseFileLists = FALSE;
}

void VpeMain::mSetupItem2( WMenuItem* item )
{
    _activeVComp->setFocus();
    _activeVComp->setupItem( (MItem*)item->tagPtr() );
}

void VpeMain::showItemCommand( WMenuItem* )
{
    _activeVComp->setFocus();
    _activeVComp->showItemCommand();
}

void VpeMain::mTouchItem( WMenuItem* )
{
    _activeVComp->setFocus();
    _activeVComp->touchItem( _activeVComp->selectedItem() );
}

void VpeMain::mIncludedItems( WMenuItem* )
{
    _refuseFileLists = TRUE;
    _activeVComp->setFocus();
    _activeVComp->mIncludedItems();
    _refuseFileLists = FALSE;
}

void VpeMain::mActionItem( WMenuItem* item )
{
    _activeVComp->setFocus();
    _activeVComp->doAction( _activeVComp->selectedItem(), (MAction*)item->tagPtr() );
}

void VpeMain::mActionItemSetup( WMenuItem* item )
{
    _activeVComp->setFocus();
    _activeVComp->actionSetup( _activeVComp->selectedItem(), (MAction*)item->tagPtr() );
}

void VpeMain::mActionComponent( WMenuItem* item )
{
    _activeVComp->setFocus();
    _activeVComp->doAction( _activeVComp->target(), (MAction*)item->tagPtr() );
}

void VpeMain::mActionComponentSetup( WMenuItem* item )
{
    _activeVComp->setFocus();
    _activeVComp->actionSetup( _activeVComp->target(), (MAction*)item->tagPtr() );
}

void VpeMain::mRefresh( WMenuItem* )
{
    startWait();
    _project->refresh();
    stopWait();
}

void VpeMain::mAction( WMenuItem* item )
{
    doAction( (MAction*)item->tagPtr() );
}

void VpeMain::doAction( const WString& actionName )
{
    int icount = _config->actions().count();
    for( int i=0; i<icount; i++ ) {
        MAction* action = (MAction*)_config->actions()[i];
        if( action->name().match( actionName ) ) {
            doAction( action );
            break;
        }
    }
}

void VpeMain::doAction( MAction* action )
{
    WString cmd;
    WFileName target( "dummy" );
    SwMode mode = SWMODE_RELEASE;
    if( _activeVComp ) {
        _activeVComp->target()->absName( target );
        mode = _activeVComp->component()->mode();
    }
    int location = action->expand( cmd, &target, "*", NULL, mode );
    WString t;
    action->text( t );
    executeCommand( cmd, location, t );
}

void VpeMain::mHelpItem( WMenuItem *mi ) {
    doAction( (MAction*) mi->tagPtr() );
}

void VpeMain::helpContents( WMenuItem* )
{
    _help->sysHelpContent();
}

void VpeMain::helpSearch( WMenuItem* )
{
    _help->sysHelpSearch( "" );
}

void VpeMain::helpUsage( WMenuItem* )
{
    _help->sysHelpOnHelp();
}

void VpeMain::about( WMenuItem* )
{
    WAbout about( this, &_hotSpotList, 4, _viperAboutTitle, _viperDesc );
    about.process();
}

void VpeMain::editLog( WMenuItem* m )
{
    _msgLog->editRequest( m );
}

void VpeMain::helpLog( WMenuItem* m )
{
    _msgLog->helpRequest( m );
}

void VpeMain::cancelLog( WMenuItem* m )
{
    _msgLog->stopRequest( m );
}

void VpeMain::saveLogAs( WMenuItem* )
{
    _msgLog->saveLogAs();
}

void WEXPORT VpeMain::updateView()
{
    WString title( _viperTitle );
    if( _project ) {
        title.concatf( " [%s]", (const char*)_pModel->filename() );
    }
    setText( title );
}

void WEXPORT VpeMain::modelGone()
{
    _project = NULL;
    updateView();
}

void VpeMain::quickRefresh()
{
    if( _activeVComp ) {
        _activeVComp->component()->refresh();
    }
}

//static char okAnswer[] = "ok";
//static char noAnswer[] = "no";

void VpeMain::editorNotify( const char* msg )
{
    if( strieq( msg, "disconnect" ) ) {
        quickRefresh();
    }
}

void VpeMain::browseNotify( const char* msg )
{
    if( strieq( msg, "disconnect" ) ) {
        quickRefresh();
    }
}

#if(0)
WString* VpeMain::serverNotify( const char* msg )
{
    bool ok = TRUE;
    WStringList toks( msg );
    if( toks.stringAt(0) == "Editor" ) {
        executeCommand( toks.cString( 1 ), EXECUTE_EDITOR, "AutoEdit" );
    } else if( toks.stringAt(0) == "Browse" ) {
        executeCommand( toks.cString( 1 ), EXECUTE_BROWSE, "AutoBrowse" );
    } else if( toks.stringAt(0) == "TakeFocus" ) {
        setFocus();
        this->setFocus();
    } else {
        return( NULL );
    }
    return( ok ? new WString( okAnswer ) : new WString( noAnswer ) );
}
#endif

const char* VpeMain::toolName( char tag )
{
    static WString tagstr;
    tagstr = "";
    tagstr.concat( tag );
    tagstr.concat( _myHandle );
    return( tagstr );
}

bool VpeMain::running()
{
    return( _msgLog && _msgLog->running() );
}

void VpeMain::runBatch( const WString& cmd )
{
    if( running() ) {
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "Batch execution is busy" );
    } else if( _batchOk ) {
        setStatus( "Executing Log..." );
        if( !_msgLog ) {
            _msgLog = new VMsgLog( this );
            _msgLog->startConnect();
            if( _activeVComp ) {
                _activeVComp->setFocus();
            }
        }
        if( !_msgLog ) return;
        WFileName dir; dir.getCWD( FALSE );
        _msgLog->setDirectory( dir );
        _msgLog->runCommand( cmd );
    }
}

void VpeMain::executeBrowse( const WString& cmd )
{
    if( _config->debug() && !confirm( "executeBrowse: '%s'", cmd ) ) {
        return;
    }
    setStatus( "Executing source browser..." );
    WString bat;
    WStringList x( cmd );
    bat.concat( _config->browse() );
    if( x.stringAt(0) == "Open" ) {
        bat.concat( " " );
        bat.concat( x.cString( 1 ) );
    }

    const char *fn = x.cString(1);

    // don't allow browsing if no browseable files
    bool browseable = FALSE;
    bool exists = FALSE;
    if( _activeVComp ) {
        MComponent* comp = _activeVComp->component();
        WVList itemlist;
        comp->getMaskItems( itemlist );
        int icount = itemlist.count();
        for( int i=0; i<icount; i++ ) {
            MItem* m = (MItem*)itemlist[i];
            if( m->rule()->browseSwitch().size() != 0 ) {
                browseable = TRUE;
            }
        }
        exists = comp->tryBrowse();
        if( exists ) comp->makeMakeFile();
    }

    if( !browseable ) {
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperInfo,
                "Browsing is not supported for any files in the current Target." );
        return;
    }
    if( !exists ) {
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperInfo,
            "No Browser files found.\n Please add the -db Compiler Option and Make again.", fn+1 );
    } else {
        execute( bat );
    }
}

bool VpeMain::confirm( const char* prompt, const char* msg )
{
    MsgRetType ret = WMessageDialog::messagef( this, MsgQuestion, MsgYesNo, _viperRequest, prompt, msg );
    return( ret == MsgRetYes );
}

WHotSpots& VpeMain::hotSpotList()
{
    return( _hotSpotList );
}

SayReturn VpeMain::say( SayStyle style, SayCode code, const char* text )
{
    static MsgLevel styles[] = { MsgInfo, MsgWarning, MsgError };
    static MsgButtons codes[] = { MsgOk, MsgOkCancel, MsgYesNoCancel };
    switch( WMessageDialog::message( this, styles[ style ], codes[ code ], text ) ) {
        case MsgRetNo:
            return( RetNo );
        case MsgRetOk:
            return( RetOk );
        case MsgRetYes:
            return( RetYes );
        default:
            return( RetCancel );
    }
}

bool VpeMain::checkProject()
{
    if( !_project ) {
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "No active project" );
        return( FALSE );
    }
    return( TRUE );
}

bool VpeMain::contextHelp( bool is_act_wnd ) {
    if( !is_act_wnd && !HelpStack.isempty() ) {
        _help->sysHelpId( (int)HelpStack.getTop() );
    }
    return( TRUE );
}

void VpeMain::mCheckin( WMenuItem *) {
    WFileName           fn;

    if( _activeVComp ) {
        if( _activeVComp->selectedItem() ) {
            _activeVComp->selectedItem()->absName( fn );
            WFileName proj = _project->filename();
            WFileName targ = _activeVComp->component()->filename();
            _editorDll.EDITSaveThis( &fn );
            if( !_rcsClient.Checkin( &fn, proj, targ ) ) {
                WMessageDialog::messagef( this, MsgError, MsgOk, _viperError,
                                "Checkin was not completed" );
            } else if( _autoRefresh && _project != NULL ) {
                _project->refresh( FALSE );
            }
        }
    }
}

void VpeMain::mCheckout( WMenuItem *) {

    WFileName           fn;

    if( _activeVComp ) {
        if( _activeVComp->selectedItem() ) {
            _activeVComp->selectedItem()->absName( fn );
            if( _editorDll.EDITIsFileInBuf( &fn ) ) {
                WMessageDialog::messagef( this, MsgError, MsgOk, _viperError,
                                "Checkout was not completed.  "
                                "'%s' is currently opened in the editor.  "
                                "Please close it and try again", fn.gets() );
            } else {
                WFileName proj = _project->filename();
                WFileName targ = _activeVComp->component()->filename();
                if( !_rcsClient.Checkout( &fn, proj, targ ) ) {
                    WMessageDialog::messagef( this, MsgError, MsgOk,
                                _viperError, "Checkout was not completed" );
                } else if( _autoRefresh && _project != NULL ) {
                    _project->refresh( FALSE );
                }
            }
        }
    }
}

void VpeMain::setRcsScheme( int sys ) {
    if( !_rcsClient.SetSystem( sys ) ) {
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperError,
                        "Unable to change the current Source Control system" );
    }
}

void VpeMain::setMksRcs( WMenuItem *) {
    setRcsScheme( MKS_RCS );
}

void VpeMain::setMksSi( WMenuItem *) {
    setRcsScheme( MKS_SI );
}

void VpeMain::setObjectCycle( WMenuItem *) {
    setRcsScheme( O_CYCLE );
}

void VpeMain::setPerforce( WMenuItem *) {
    setRcsScheme( PERFORCE );
}

void VpeMain::setPvcs( WMenuItem *) {
    setRcsScheme( PVCS );
}

void VpeMain::setOtherRcs( WMenuItem *) {
    setRcsScheme( GENERIC );
}

void VpeMain::setNoRcs( WMenuItem *) {
    setRcsScheme( NO_RCS );
}

void VpeMain::mRcsShell( WMenuItem *) {
    if( !_rcsClient.RunShell() ) {
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperError,
                                "Unable to run the Source Control Shell" );
    } else if( _autoRefresh && _project != NULL ) {
        _project->refresh( FALSE );
    }
}

void VpeMain::toggleAutoRefresh( WMenuItem * ) {
    _autoRefresh = !_autoRefresh;
}
