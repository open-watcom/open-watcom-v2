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


#include <wclistit.h>

#include <wfilenam.hpp>
#include <wfiledlg.hpp>
#include <wmsgdlg.hpp>

#include "assure.h"
#include "browse.h"
#include "chfile.h"
#include "dbmgr.h"
#include "death.h"
#include "edmodule.h"
#include "fileinfo.h"
#include "newdbrdg.h"
#include "menuids.h"
#include "module.h"
#include "util.h"
#include "viewmgr.h"
#include "wbrwin.h"

static char * DBRFilter = "Database File (*.dbr)\0" "*.dbr\0"
                          "All Files(*.*)\0" "*.*\0";

DatabaseManager::DatabaseManager()
            : _menuManager( NULL )
            , _module( NULL )
//--------------------------------
{
}

DatabaseManager::~DatabaseManager()
//---------------------------------
{
    delete _module;
}

static MIMenuID DBManagerMenus[] = {
    MIMenuID( MMFile,           FMNew ),
    MIMenuID( MMFile,           FMOpen ),
    MIMenuID( MMFile,           FMModules ),
};

#define NumDBMgrMenus ( sizeof( DBManagerMenus ) / sizeof( MIMenuID ) )

void DatabaseManager::setMenus( MenuManager * mgr )
//-------------------------------------------------
{
    int i;

    _menuManager = mgr;
    for( i = 0; i < NumDBMgrMenus; i += 1 ) {
        _menuManager->registerForMenu( this, DBManagerMenus[ i ] );
    }

    _menuManager->enableMenu( MIMenuID( MMFile, FMModules ), FALSE );
}

void DatabaseManager::unSetMenus( MenuManager * )
//-----------------------------------------------
{
    int i;

    for( i = 0; i < NumDBMgrMenus; i += 1 ) {
        _menuManager->unRegister( DBManagerMenus[ i ] );
    }

    _menuManager = NULL;
}

void DatabaseManager::menuSelected( const MIMenuID & id )
//-------------------------------------------------------
{
    switch( id.mainID() ) {
    case MMFile:
        switch( id.subID() ) {
        case FMNew:
            newDatabase();
            break;
        case FMModules:
            ASSERTION( _module );
            editModule( _module->fileName() );
            break;
        case FMOpen:
            openDatabase();
            break;
        default:
            NODEFAULT;
        }
        break;
    default:
        NODEFAULT;
    }
}

void DatabaseManager::newDatabase()
//---------------------------------
{
    NewDBRFile getDBR( topWindow, DBRFilter );

    WFileName fname;

    if( getDBR.process() ) {
        fname = getDBR.getFileName();

        if( *(fname.ext()) == '\0' ) {
            fname.setExt( "dbr" );
        }

        FileInfo finf( fname.gets() );

        if( finf.exists() ) {
            if( !fname.removeFile() ) { // user agreed in GetDbr
                errMessage( "Can't overwrite %s", fname.gets() );
                return;
            }
        }

        fname.absoluteTo();
        fname.toLower();
    } else {            /* user cancelled */
        return;
    }


    editModule( fname, TRUE );
}

void DatabaseManager::openDatabase()
//----------------------------------
{
    WFileDialog fileSelect( topWindow, DBRFilter );
    const char * file;

    file = fileSelect.getOpenFileName( NULL,
                        "Open Browser Database File",
                        WFOpenExisting );

    if( file && *file ) {
        WFileName fname( file );

        if( *(fname.ext()) == '\0' ) {
            fname.setExt( "dbr" );
        }

        fname.absoluteTo();
        fname.toLower();

        FileInfo finf( fname );

        if( finf.exists() ) {
            setModule( fname.gets(), NULL );
        } else {
            errMessage( "%s: file not found", fname.gets() );
        }
    }
}

void DatabaseManager::editModule( const char * fName, bool create )
//-----------------------------------------------------------------
// create is true if the module is not to use the components of the
// existing module
{
    WCValSList<String>  newEnb;
    WCValSList<String>  newDis;
    int                 i;
    WVList              enabled;
    WVList              disabled;
    bool                showGbl = (_module == NULL);
    WFileName           file( fName );

    if( _module && !create ) {
        WVList & en( _module->enabledFiles() );
        WVList & ds( _module->disabledFiles() );

        for( i = 0; i < en.count(); i += 1 ) {
            enabled.add( new WFileName( *((WFileName *)en[i]) ) );
        }

        for( i = 0; i < ds.count(); i += 1 ) {
            disabled.add( new WFileName( *((WFileName *)ds[i]) ) );
        }
    }

    EdModule edit( topWindow, file, enabled, disabled );

    if( edit.process() ) {
        for( i = 0; i < enabled.count(); i += 1 ) {
            newEnb.append( String( ((WFileName *) enabled[ i ])->gets() ) );
        }
        for( i = 0; i < disabled.count(); i += 1 ) {
            newDis.append( String( ((WFileName *) disabled[ i ])->gets() ) );
        }

        delete _module;

        file.removeFile();
        _module = new Module( file, newEnb, newDis );
        databaseChanged();
        if( showGbl ) {
            WBRWinBase::viewManager()->showGlobalView(
                                        GlobalView::GlobalViewInheritTree );
        }
    }
}

const char * DatabaseManager::fileName()
//--------------------------------------
{
    const char * NoDatabaseMessage = "no database";

    if( _module && _module->fileName() ) {
        return _module->fileName();
    } else {
        return NoDatabaseMessage;
    }
}

void DatabaseManager::setModule( const char * name,
                                 WCValSList<String> * enabled,
                                 WCValSList<String> * disabled )
//--------------------------------------------------------------
// open or create a database file
{
    Module *            mod;
    bool                showGbl = (_module == NULL);
    WCValSList<String>  enb;
    WCValSList<String>  dis;

    if( enabled ) {
        enb = *enabled;
    }

    if( !enabled || enabled->entries() == 0 ) {
        FileInfo finf( name );
        if( !finf.exists() ) {
            editModule( name );
            return;
        }
    }

    if( disabled ) {
        dis = *disabled;
    }

    try {
        mod = new Module( name, enb, dis );
        delete _module;
        _module = mod;
        databaseChanged();
        if( showGbl ) {
            WBRWinBase::viewManager()->showGlobalView(
                                        GlobalView::GlobalViewInheritTree );
        }
   } catch( FileExcept err ) {
        errMessage( "%s: %s", name, err._message );
    } catch( CauseOfDeath cause ) {
        if( cause == DEATH_BY_BAD_SIGNATURE ) {
            errMessage( "%s: invalid Browser database", name );
        } else {
            IdentifyAssassin( cause );
        }
    } catch ( ... ) {
        errMessage( "Unknown error in %s", name ); // NYI -- fix this up!
    }
}

void DatabaseManager::databaseChanged()
//-------------------------------------
{
    WBRWinBase::viewManager()->eventOccured( VEBrowseFileChange, NULL );

    _menuManager->enableMenu( MIMenuID( MMFile, FMModules ),
                              module() != NULL );
    WBRWinBase::viewManager()->enableMenus( TRUE );
    browseTop->postTitle();
}
