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
* Description:  Source Browser mainline.
*
****************************************************************************/


#include <wwinmain.hpp>
#include <wstatwin.hpp>
#include <wmsgdlg.hpp>
#include <wflashp.hpp>
#include <wabout.hpp>
#include <wsyshelp.hpp>
#include <wfilenam.hpp>


#include "assure.h"
#include "busyntc.h"
#include "banner.h"
#include "cmdparse.h"
#include "dbmgr.h"
#include "fileinfo.h"
#include "hotspots.h"
#include "icons.h"
#include "menuids.h"
#include "menumgr.h"
#include "optmgr.h"
#include "util.h"
#include "viewmgr.h"
#include "wbrdefs.h"
#include "weditdll.h"

#ifdef REPORT_IMPLEMENTED
#  include "reportdg.h"
#endif

#include "browse.h"

#include <stdlib.h>
#include <string.h>

Browse  * browseTop = NULL;
WWindow * topWindow = browseTop;

static MIMenuID BrowseMenus[] = {
#if REPORT_IMPLEMENTED
    MIMenuID( MMFile, FMReport ),
#endif
    MIMenuID( MMFile, FMExit ),
    MIMenuID( MMHelp, HMContents ),
    MIMenuID( MMHelp, HMSearch ),
    MIMenuID( MMHelp, HMUsingHelp ),
    MIMenuID( MMHelp, HMAbout ),
};

#define _BrowseHelpFile     "wbrw.hlp"
#define _BrowseHtmlHelpFile "wbrw.chm"

#define NumBrowseMenus ( sizeof( BrowseMenus ) / sizeof( MIMenuID ) )

#pragma warning 438 9
static const char *BrowseTitle = { "About Open Watcom Source Browser" };
static const char *SplashInfo[] = {
    "",
    banner1w( "Source Browser", _WBRW_VERSION_ ),
    banner2( "1994" ),
    banner3,
    banner3a,
    NULL };
#pragma warning 438 1


static WFlashPage *showFlashPage( int interval ) {
//------------------------------------------------

    return( new WFlashPage( NULL, GlobalHotSpots, SplashPage, BrowseTitle,
                            SplashInfo, interval ) );
}

Browse::Browse()
    : WMdiWindow( "" )
//--------------------
{
    browseTop = this;
    topWindow = browseTop;
    hookF1Key( TRUE );
}

Browse::Browse( char * cmdLine )
    : WMdiWindow( "" )
    , _helpInfo( NULL )
    , _status( new WStatWindow( this, WRect(0,0,0,0), "Ready" ) )
    , _editorDLL( NULL )
//---------------------------------------------------------------
{
    WString         errMessage;

    CommandParser   prs( cmdLine, TRUE );
    _searchPath = prs.searchPath();

    WFlashPage *    flash_page = showFlashPage( 0 );

    browseTop = this;
    topWindow = browseTop;
    _helpInfo = new WSystemHelp( this, BrowseTitle, _BrowseHelpFile,
                                 _BrowseHtmlHelpFile );

    setIcon( WBRWIcon );
    setupMenus();
    viewManager()->registerForEvents( this );

    // important that options comes before the database
    // since the option file can set the query and cause a re-load
    // of the inheritance graph

    if( prs.options() ) {
        optManager()->loadFrom( prs.options() );
    } else {
        optManager()->loadDefault();
    }

    // this must come after the option loads
    if( optManager()->isEditorDLL() ) {
        setEditorDLL( optManager()->getEditorName() );
    }

    if( prs.database() ) {
        dbManager()->setModule( prs.database(), prs.files() );
    } else {
        /* if there is no database, disable interesting menus */
        viewManager()->enableMenus( FALSE );
    }

    postTitle();

    delete flash_page;
    show();
    hookF1Key( TRUE );
}

Browse::~Browse()
//---------------
{
    wbrCleanup();
    hookF1Key( FALSE );
}

bool Browse::makeFileName( char *buff )
//-------------------------------------
{
    WFileName   file;
    WFileName   nfile;
    char        *p;
    char        *q;

    file = buff;
    if( file.attribs( NULL ) ) {
        // file exists
        return( TRUE );
    }

    if( _searchPath.length() == 0 ) {
        return( FALSE );
    }

    p = new char[_searchPath.length()+1];
    if( p == NULL ) return( FALSE );
    strcpy( p, _searchPath );

    q = strtok( p, ";" );
    while( q != NULL ) {
        nfile = "";
        nfile.setDir( q );
        nfile.setFName( file.fName() );
        nfile.setExt( file.ext() );
        if( nfile.attribs( NULL ) ) {
            strcpy( buff, nfile.gets() );
            delete []p;
            return( TRUE );
        }
        q = strtok( NULL, ";" );
    }
    delete []p;
    return FALSE;
}

void Browse::event( ViewEvent ve, View * )
//----------------------------------------
{
    switch( ve ) {
    case VEOptionValChange:
        if( _editorDLLName != optManager()->getEditorName() ) {
            if( optManager()->isEditorDLL() ) {
                setEditorDLL( optManager()->getEditorName() );
            } else {
            }
        }
        break;
    }
}

void Browse::setEditorDLL( const char * dll )
//-------------------------------------------
{
    WString errMsg;
    bool    enable;

    // the Goto-Definition item is enabled iff the Show-Detail is

    enable = menuManager()->menuEnabled( MIMenuID( MMDetail, DMDetail ) );

    _editorDLLName = dll;
    delete _editorDLL;

    _editorDLL = new WEditDLL;
    _editorDLL->LoadDll( _editorDLLName, &errMsg );
    if( !_editorDLL->isInitialized() ) {
        WMessageDialog::messagef( this, MsgError, MsgOk,
                                  BrowseTitle, errMsg.gets() );
        delete _editorDLL;
        _editorDLL = NULL;
        enable = FALSE;
    }

    menuManager()->enableMenu( MIMenuID( MMDetail, DMDefinition ), enable );
}

void Browse::setupMenus()
//-----------------------
{
    int i;

    menuManager()->setupMenus( this );

    for( i = 0; i < NumBrowseMenus; i += 1 ) {
        menuManager()->registerForMenu( this, BrowseMenus[ i ] );
    }

    viewManager()->setMenus( menuManager() );
    optManager()->setMenus( menuManager() );
    dbManager()->setMenus( menuManager() );

    menuManager()->registerForViewEvents( viewManager() );

    viewManager()->enableMenus( FALSE );
}

void Browse::postTitle()
//----------------------
{
    const char * titleText = "Open Watcom Source Browser";
    WString      title;

    title.printf( "%s -- (%s%s), (%s)", titleText,
                  optManager()->fileName(),
                  (optManager()->isModified()) ? "*" : "",
                  dbManager()->fileName() );

    setText( title );
}

bool Browse::reallyClose()
//------------------------
{
    if( optManager()->exit() ) {
        viewManager()->eventOccured( VEBrowseTopDying, NULL );

        delete _status;
        _status = NULL;

        delete _helpInfo;
        _helpInfo = NULL;

        if( _editorDLL != NULL) {
            _editorDLL->EDITDisconnect();
        }
        delete _editorDLL;
        _editorDLL = NULL;

        return TRUE;
    } else {
        return FALSE;
    }
}

void Browse::menuSelected( const MIMenuID & id )
//----------------------------------------------
{
    switch( id.mainID() ) {
    case MMFile:
        fileMenu( id.subID() );
        break;
    case MMHelp:
        helpMenu( id.subID() );
        break;
    default:
        NODEFAULT;
    }
}

void Browse::fileMenu( MISubMenu sub )
//------------------------------------
{
    switch( sub ) {
#ifdef REPORT_IMPLEMENTED
    case FMReport: {
        ReportDialog rep( this );
        rep.process();
    } break;
#endif

    case FMExit: {
        close();    // will call reallyClose();
    } break;

    default:
        NODEFAULT;
    }
}

void Browse::helpMenu( MISubMenu sub )
//------------------------------------
{
    switch( sub ) {
    case HMContents:
        _helpInfo->sysHelpContent();
        break;
    case HMSearch:
        _helpInfo->sysHelpSearch( NULL );
        break;
    case HMUsingHelp:
        _helpInfo->sysHelpOnHelp();
        break;
    case HMAbout:
        WAbout about( topWindow, GlobalHotSpots, SplashPage, BrowseTitle,
                      SplashInfo );
        about.process();
        break;
    }
}

bool Browse::contextHelp( bool is_active_win )
//--------------------------------------------
{
    if( is_active_win ) {
        _helpInfo->sysHelpContent();
    }
    return( TRUE );
}

bool Browse::canEdit()
//--------------------
{
    if( optManager()->isEditorDLL() ) {
        if( _editorDLL != NULL && _editorDLL->isInitialized() ) {
            return TRUE;
        } else {
            return FALSE;
        }
    } else {
        return( optManager()->getEditorName() != NULL );
    }
}
void Browse::positionEditor( char * fileName, ulong line, uint col,
                             int hiliteLen )
//-----------------------------------------------------------------
{
    FileInfo finf( fileName );

    if( finf.exists() ) {
        if( optManager()->isEditorDLL() ) {
            if( _editorDLL->isInitialized() ) {
                if( _editorDLL->EDITConnect() ) {
                    _editorDLL->EDITFile( fileName, "" );
                    _editorDLL->EDITLocate( line, col, hiliteLen );
                    _editorDLL->EDITShowWindow( EDIT_SHOWNORMAL );
                } else {
                    errMessage( "Unable to connect to editor" );
                }
            } else {
                ASSERTION( 0 /* uh-oh -- the menu item should be grayed */ );
            }
        } else {
            WString cmd( optManager()->getEditorName() );
            WString editorParms( optManager()->getEditorParms() );

            cmd.concat( ' ' ); // space after editor name before parms
            int parmsize = editorParms.size();

            for( int i=0; i < parmsize; i++ ) {
                switch( editorParms[i] ) {
                case '%':
                    switch( editorParms[i+1] ) {
                    case 'f': // file name
                        cmd.concat( fileName );
                        break;
                    case 'r': // row to go to
                        cmd.concatf( "%ld", line );
                        break;
                    case 'c': // column to go to
                        cmd.concatf( "%d", col );
                        break;
                    case 'l': // length of hilight
                        cmd.concatf( "%d", hiliteLen );
                        break;
                    #if 0
                    case 'h': // helpid
                        cmd.concat( x.stringAt( 5 ) );
                        break;
                    case 'e': // error message
                        cmd.concat( x.stringAt( 6 ) );
                        break;
                    #endif
                    case '%': // a real '%'
                        cmd.concat( '%' );
                        break;
                    default:
                        // ignore the '%' and the character
                        break;
                    }
                    i+=2; // skip % and following char
                default:
                    cmd.concat( editorParms[i] );
                    break;
                }
            }
            WSystemService::sysExec( cmd, WWinStateShowNormal, 0 );
        }
    } else {
        errMessage( "File \"%s\" not found", fileName );
    }
}
void Browse::showSource( Symbol * sym )
//-------------------------------------
{
    char        file[_MAX_PATH];
    ulong       line;
    uint        col;

    sym->filePosition( line, col );
    if( sym->defSourceFile( file ) ) {
        int size = strlen( sym->name() );
        browseTop->positionEditor( file, line, col, size );
    } else {
        errMessage( "Unable to edit %s:  no source information.",
                    sym->name() );
    }
}

void Browse::statusText( const char * text )
//------------------------------------------
{
    _status->setStatus( text );
}
