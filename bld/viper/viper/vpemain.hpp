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


#ifndef vpemain_class
#define vpemain_class
#ifndef NULL
#define NULL 0
#endif

#include "wserver.hpp"
#include "wmdiwndw.hpp" // after wserver.hpp - we need windows.h before gui.h
#include "wview.hpp"
#include "wpopmenu.hpp"
#include "wmenuitm.hpp"
#include "mproject.hpp"

#include "vtoolitm.hpp"
#include "whotspot.hpp"
#include "veditdll.hpp"
#include "vwmclien.hpp"
#include "vrcscli.hpp"

WCLASS WServer;
WCLASS WClient;
WCLASS MRule;
WCLASS MAction;
WCLASS VComponent;
WCLASS VMsgLog;
WCLASS WStatWindow;
WCLASS WSystemHelp;

typedef struct mPop MenuPop;
typedef struct mData MenuData;

WCLASS VpeMain : public WMdiWindow, public WView
{
    Declare( VpeMain )
    public:
        VpeMain();
        ~VpeMain();
        void WEXPORT setActive( VComponent* comp ) { _activeVComp = comp; }
        void WEXPORT updateView();
        void WEXPORT modelGone();
        bool gettingFocus( WWindow* );
        bool keyDown( WORD key );
        virtual bool WEXPORT keyDown( WKeyCode code, WKeyState state ) {
            return( WWindow::keyDown( code, state ) );
        };
        bool reallyClose();
        bool queryEndSession( void ) { return reallyClose(); }

        bool executeCommand( const char* cmd, int location, const char* title=NULL );
        WPopupMenu* targetPopup() { return _targetPopup; }
        WPopupMenu* itemsPopup() { return _itemsPopup; }
        WPopupMenu* logPopup() { return _logPopup; }
        MProject* project() { return _project; }
        SayReturn say( SayStyle style, SayCode code, const char* text );
        void            quickRefresh();
        void setStatus( const char* msg );
        bool confirm( const char* prompt, const char* msg );
        WHotSpots& hotSpotList();
        bool createDirectory( const WFileName& f );
        void deleteMsglog();
        bool attachTgtFile( WFileName& fn );
        WString* DdeCallback( const char* c );
        bool appActivate( bool activated );
        virtual bool contextHelp( bool );
    private:
        void cForPBProject( WFileName &pj, bool nt );
        void remoteFileOp( const char *cmd );
        void readIdeInit();
        void buildMenuBar();
        bool addComponent( WMenuItem* =NULL );
        void removeComponent( WMenuItem* =NULL );
        void renameComponent( WMenuItem* =NULL );
        void setupComponent( WMenuItem* =NULL );
        void showCompCommand( WMenuItem* );
        void touchComponent( WMenuItem* =NULL );
        void toolBar( WMenuItem* =NULL );
        void createStatusBar();
        void deleteStatusBar();
        void statusBar( WMenuItem* =NULL );
        void setEditor( WMenuItem* =NULL );
        void    mCheckin( WMenuItem *);
        void    mCheckout( WMenuItem *);
        void    mRcsShell( WMenuItem *);
        void    setRcsScheme( int );
        void    setMksRcs( WMenuItem *);
        void    setMksSi( WMenuItem *);
        void    setObjectCycle( WMenuItem *);
        void    setPvcs( WMenuItem *);
        void    setOtherRcs( WMenuItem *);
        void    setNoRcs( WMenuItem *);
        void    mDummy( WMenuItem* ) {}
        void    mHint( WMenuItem * item, const char* hint );
        void    mAddItem( WMenuItem * item );
        bool    kAddItem( WKeyCode );
        void    mRemoveItem( WMenuItem* item );
        bool    kRemoveItem( WKeyCode );
        void    mRenameItem( WMenuItem* item );
        void    mSetupItem( WMenuItem* item );
        void    mSetupItem2( WMenuItem* item );
        void showItemCommand( WMenuItem* );
        void    mTouchItem( WMenuItem* item );
        void    mIncludedItems( WMenuItem* item );
        void    mActionItem( WMenuItem* item );
        void    mActionItemSetup( WMenuItem* item );
        void    mActionComponent( WMenuItem* item );
        void    mActionComponentSetup( WMenuItem* item );
        void    mAutodepend( WMenuItem* );
        void    mAutotrack( WMenuItem* );
        void    mDebugMode( WMenuItem* );
        void    mRefresh( WMenuItem* item );
        void kNewProject( WKeyCode );
        void newProject( WMenuItem* );
        bool loadProject( const WFileName& f );
        WStyle vCompStyle();
        bool unloadProject( const WFileName& f, bool try_checkout =1 );
        void kOpenProject( WKeyCode );
        void openProject( WMenuItem* );
        void openOldProject( WMenuItem* );
        void closeProject( WMenuItem* );
        bool kSaveProject( WKeyCode );
        bool saveProject();
        void saveProject( WMenuItem* ) { saveProject(); }
        bool saveProjectAs( const WFileName& def );
        void saveProjectAs( WMenuItem* ) { saveProjectAs( "" ); }
        void setBefore( WMenuItem* );
        void setAfter( WMenuItem* );
        void setCompBefore( WMenuItem* );
        void setCompAfter( WMenuItem* );
        void exit( WMenuItem* ) { close(); }
        void toolPicked( WToolBarItem* );
        bool toolChanged( WToolBar*, WToolBarState );
        bool _toolBarActive;

        bool okToClear();
        bool okToQuit();
        bool optionToSave();
        void clearProject();
        void mAction( WMenuItem* );
        void doAction( const WString& );
        void doAction( MAction* );
        WSystemHelp*    _help;          // help and otherhelp should be one item
        WSystemHelp*    _otherhelp;
        void helpContents( WMenuItem* );
        void helpSearch( WMenuItem* );
        void helpUsage( WMenuItem* );
        void about( WMenuItem* );

        void editLog( WMenuItem* );
        void helpLog( WMenuItem* );
        void cancelLog( WMenuItem* );
        void saveLogAs( WMenuItem* );

        void onPopup0( WPopupMenu* );
        void onPopup0a( WPopupMenu* );
        void onPopup1( WPopupMenu* );
        void onPopup2( WPopupMenu* );
        void onPopup2a( WPopupMenu* );
        void onPopup3( WPopupMenu* );
        void onPopup3a( WPopupMenu* );
        void onPopup4( WPopupMenu* );
        void onPopup5( WPopupMenu* );
        void onPopup6( WPopupMenu* );
        void onPopup7( WPopupMenu* );

        int addComponentActionSetups( WPopupMenu*, int );
        int addComponentMaskSetups( WPopupMenu*, int );

        WPopupMenu*     _targetPopup;
        WPopupMenu*     _itemsPopup;
        WPopupMenu*     _logPopup;

        WStatWindow*    _statusBar;
        WVList          _compViews;
        VMsgLog*        _msgLog;
        VComponent*     _activeVComp;

        MProject*       _project;
        bool            _neverSaved;
        WString         _myHandle;

        // editor stuff
        VEditDLL        _editorDll;
        WFileName       _editor;                // editor for the
        bool            _editorIsDll;           // current plateform
        WString         _editorParms;           // parms to non-dll editor
        #if 0
        WFileName       _winEditor;
        bool            _winEditorIsDll;
        WFileName       _ntEditor;
        bool            _ntEditorIsDll;
        WFileName       _os2Editor;
        bool            _os2EditorIsDll;
        #endif

        const char* toolName( char tag );
//      WServer*        _server;
        WString*        serverNotify( const char* msg );
        VWinMakerClient _winMakerClient;
        bool            _refuseFileLists;
        VRcsClient      _rcsClient;
        WClient*        _editorClient;
        void            editorNotify( const char* msg );
        WClient*        _browseClient;
        bool    running();
        void            browseNotify( const char* msg );
        bool executeOne( const WString& cmd );
        bool execute( const WString& cmd );
        void executeEditor( const WString& cmd );
        void executeBrowse( const WString& cmd );
        void runBatch( const WString& cmd );
        bool            _batchOk;
        bool makeMake();
        bool            _quitAnyways;

        static MenuPop popup7;
        static MenuData menu7[];
        static MenuPop popup5;
        static MenuData menu5[];
        static MenuPop popup4;
        static MenuData menu4[];
        static MenuPop popup3a;
        static MenuData menu3a[];
        static MenuPop popup3;
        static MenuData menu3[];
        static MenuPop popup2a;
        static MenuData menu2a[];
        static MenuPop popup2;
        static MenuData menu2[];
        static MenuPop popup1;
        static MenuData menu1[];
        static MenuPop popup0a;
        static MenuData menu0a[];
        static MenuPop popup0;
        static MenuData menu0[];
        WPopupMenu* makeMenu( MenuPop* popup, VToolBar* tools );
        bool checkProject();

        WFileName       _initFile;
        WVList          _oldProjects;
        WServer         _ddeServer;
        bool            _autoRefresh;
        void addOldProject( const WFileName& );
        bool okToReplace( WFileName& fn );
        bool registerAccel( WKeyCode kc );
        bool kDynAccel( WKeyCode kc );
        bool validateProjectName( WFileName& fn );
        void mHelpItem( WMenuItem *mi );
        void loadNewEditor( WFileName &fn );
        void toggleAutoRefresh( WMenuItem * );
};
#endif

