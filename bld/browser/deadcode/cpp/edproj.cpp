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


// definition of class EdProject
#include <wname.hpp>
#include <wpshbttn.hpp>
#include <wlistbox.hpp>

#include <wmsgdlg.hpp>
#include <wfiledlg.hpp>
#include <wfilenam.hpp>
#include <wfilelst.hpp>

#include "assure.h"
#include "module.h"
#include "projmodl.h"
#include "safefile.h"
#include "util.h"
#include "edproj.h"

// this string is used for file selection
static char* EdProjFilter = "Database (*.cbr)\0*.cbr\0"
                            "Component (*.mbr)\0*.mbr\0";
static char * HelpText =
    "Use this screen to enable, disable, add and remove databases";


EdProject::EdProject( Browse* prnt )
    : EditModuleList( prnt, "Project Modules" )
    , _browse( prnt )
    , _model( prnt->project() )
//---------------------------------------------
{
}

EdProject::~EdProject()
//---------------------
{
}

void EdProject::loadBox()
//-----------------------
{
    int             i;
    WFileName *     strModuleName;
    ModuleItem *    modItem;
    WVList &        modlist( _model->modules() );

    for( i = 0; i < modlist.count(); i += 1 ) {
        strModuleName = new WFileName(( (Module *) modlist[ i ])->fileName());

        modItem = new ModuleItem( strModuleName,
                        ((Module *) modlist[ i ])->enabled() );

        _moduleBox->insert( modItem );
    }
}

void EdProject::componentsButton( WWindow * )
//-------------------------------------------
{
    if( _moduleBox->selected() >= 0 ) {
        _model->editModule( this,
            (*_moduleBox)[ _moduleBox->selected() ]->_str->gets() );
    }
}

void EdProject::helpButton( WWindow * )
//-------------------------------------
{
    WMessageDialog::message( this, MsgInfo, MsgOk, HelpText, "Browser Modules Help" );
}


void EdProject::addButton( WWindow * )
//------------------------------------
{
    int             i;
    int             fileIdx;
    WFileNameList   files;
    WFileDialog     fileSelect( this, EdProjFilter );
    bool            result;
    WFileName *     file;
    bool            found;

    result = fileSelect.getOpenFileName( files, NULL,
                                        "Select Browser Database File",
                                        WFOpenNew );
    if( result ) {
        for( fileIdx = 0; fileIdx < files.count(); fileIdx += 1 ) {
            file = new WFileName( files.stringAt( fileIdx ) );

            if( *( file->ext() ) == '\0' ) {
                file->setExt("cbr");
            }

            file->absoluteTo();
            file->toLower();

            found = FALSE;
            for( i = 0; i < _moduleBox->count() && !found; i += 1 ) {
                WString * str = (*_moduleBox)[ i ]->_str;

                if( *str == *file ) {
                    errMessage( "Module %s already in project", file->fName() );
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

void EdProject::okButton( WWindow * )
//-----------------------------------
{
    int         i;
    WString *   pStr;
    bool        enable;

    for( i = 0; i < _moduleBox->count(); i++ ) {
        pStr = (*_moduleBox)[ i ]->_str;
        enable = (*_moduleBox)[ i ]->_enabled;

        _model->addModule( pStr->gets() );                  // projmodl prevents duplicates
        _model->enableModule( pStr->gets(), enable );
    }

    for( i = 0; i < _removedModuleItems->entries(); i++ ) {
        pStr = (*_removedModuleItems)[i]->_str;
        _model->removeModule( pStr->gets() );
    }

    _model->checkModuleTimeStamp();

    #if 0 // need some dirty-checking of some kind.
    if( _droppedModules.count() != 0 || _disabledModules.count() != 0 ) {
        /* some popups may be viewing removed / disabled modules
         * so they are all killed.  It would be better if a more selective
         * message were sent to the windows.
         */

        ((Browse *)parent())->cleanPopups();
    }
    #endif

    quit( TRUE );
}

void EdProject::cancelButton( WWindow * )
//-------------------------------------------
{
    quit( FALSE );
}

