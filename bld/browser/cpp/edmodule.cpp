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


#include <wcvector.h>
#include <wmsgdlg.hpp>
#include <wfiledlg.hpp>
#include <wfilenam.hpp>
#include <wfilelst.hpp>
#include <wstrlist.hpp>
#include <wpshbttn.hpp>

#include "assure.h"
#include "module.h"
#include "util.h"
#include "wbrwin.h"
#include "brwhelp.h"
#include "edmodule.h"

static char* EdModuleFilter = "Browser Module (*.mbr)\0*.mbr\0";


EdModule::EdModule( WWindow * prnt, const char * name,
                    WVList & en, WVList & dis )
    : EditModuleList( prnt, "Browser File Modules" )
    , _name( name )
    , _enabled( en )
    , _disabled( dis )
//--------------------------------------------------------------
{
}

void EdModule::initialize()
//-------------------------
{
    WString s;
    s.printf( "Modules for %s", _name );
    setText( s.gets() );

    EditModuleList::initialize();
}

void EdModule::loadBox()
//----------------------
{
    int             i;
    WFileName *     strModuleName;
    ModuleItem *    modItem;

    for( i = 0; i < _enabled.count(); i += 1 ) {
        strModuleName = new WFileName( *(WFileName *) _enabled[ i ] );
        modItem = new ModuleItem( strModuleName, TRUE );
        _moduleBox->insert( modItem );
    }

    for( i = 0; i < _disabled.count(); i += 1 ) {
        strModuleName = new WFileName( *(WFileName *) _disabled[ i ] );
        modItem = new ModuleItem( strModuleName, FALSE );
        _moduleBox->insert( modItem );
    }
}


void EdModule::addButton( WWindow * )
//-----------------------------------
{
    int             i;
    int             fileIdx;
    WFileNameList   files;
    WFileDialog     fileSelect( this, EdModuleFilter );
    bool            result;
    WFileName *     file;
    bool            found;

    result = fileSelect.getOpenFileName( files, NULL,
                                        "Select Module File(s)",
                                        WFOpenNew );

    if( result ) {
        for( fileIdx = 0; fileIdx < files.count(); fileIdx += 1 ) {
            file = new WFileName( files.cStringAt( fileIdx ) );

            if( *( file->ext() ) == '\0' ) {
                file->setExt("mbr");
            }
            file->absoluteTo();
            file->toLower();

            found = FALSE;

            if( !file->attribs() ) {
                errMessage( "Module %s%s does not exist", file->fName(), file->ext() );
                found = TRUE;
            }

            for( i = 0; i < _moduleBox->count() && !found; i += 1 ) {
                WString * str = (*_moduleBox)[ i ]->_str;

                if( *str == *file ) {
                    errMessage( "Module %s%s already in project", file->fName(), file->ext() );
                    found = TRUE;
                }
            }

            for( int i = 0; i < _removedModuleItems->entries() && !found; i++ ) {
                WString * str = (*_removedModuleItems)[ i ]->_str;

                if( *str == *file ) {
                    ModuleItem * item = _removedModuleItems->removeAt( i ) ;
                    item->_enabled = TRUE;
                    _moduleBox->insert( item );

                    found = TRUE;
                }
            }

            if( !found ) {
                ModuleItem * item = new ModuleItem( file, TRUE );
                _moduleBox->insert( item );
            }

        } /* for( i < files.count() ) */

    } /* if( result ) */
    _moduleBox->reset();

}

void EdModule::helpButton( WWindow * )
//------------------------------------
{
    WBRWinBase::helpInfo()->sysHelpId( BRH_EDIT_MODULE_LIST );
}

bool EdModule::contextHelp( bool is_active_win )
//----------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_EDIT_MODULE_LIST );
    }
    return( TRUE );
}

void EdModule::okButton( WWindow * )
//----------------------------------
{
    int          i;
    ModuleItem * item;

    for( i = 0; i < _moduleBox->count(); i += 1 ) {
        if( (*_moduleBox)[ i ]->_enabled ) {
            break;
        }
    }
    if( i >= _moduleBox->count() ) {
        WMessageDialog::messagef( NULL, MsgError, MsgOk, "Invalid Browser File",
                            "%s has no enabled modules.", _name );
        return;
    }

    _disabled.deleteContents();
    _enabled.deleteContents();

    for( i = 0; i < _moduleBox->count(); i++ ) {
        item = (*_moduleBox)[ i ];
        if( item->_enabled ) {
            _enabled.add( new WFileName( item->_str->gets() ) );
        } else {
            _disabled.add( new WFileName( item->_str->gets() ) );
        }
    }

    quit( TRUE );
}

void EdModule::cancelButton( WWindow * )
//-------------------------------------
{
    quit( FALSE );
}

// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9

EdModule::~EdModule()
//-------------------
{
}
