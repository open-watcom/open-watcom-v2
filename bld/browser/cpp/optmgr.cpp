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


#include <string.h>
#include <mbctype.h>

#include <wfiledlg.hpp>
#include <wfilenam.hpp>
#include <wmsgdlg.hpp>

#include "assure.h"
#include "browse.h"
#include "busyntc.h"
#include "chbffile.h"
#include "death.h"
#include "eddlldg.h"
#include "fileinfo.h"
#include "gtcllopt.h"
#include "gtinhopt.h"
#include "death.h"
#include "enumstl.h"
#include "memfilt.h"
#include "menuids.h"
#include "querycfg.h"
#include "rxcfg.h"
#include "viewmgr.h"
#include "wbrwin.h"

#define SIG     "WATCOM Source Browser Option File\032\015\023"
#define SIGLEN  (sizeof(SIG))
#define VER     "VER 1.1"
#define VERLEN  (sizeof(VER))

static char * OBRFilter = "Option File (*.obr)\0" "*.obr\0"
                          "All Files(*.*)\0" "*.*\0\0";
static char * DefaultOBRFile = "setup.obr";

const MemberFilter DefaultMemFilt = {
    MemberFilter::ILNoInherited,// don't show any inherited members
    MemberFilter::AccAll,       // show members with any access (public,etc)
    MemberFilter::MemAll        // show all members (static, virt, variables, functions)
};
const RXOptions      DefaultRXOpt = {
    TRUE,                       // RX anchored
    "",                         // all RX characters
};

static const char * DefaultEditorParms = "%f";
#if defined( __OS2__ )
    static bool DefaultEditorIsDLL = TRUE;
    static const char * DefaultEditor = "epmlink";
#else
#pragma initialize before program
    static bool DefaultEditorIsDLL = !__IsDBCS;
    static const char * DefaultEditor =
        ( __IsDBCS ? "notepad.exe" : "weditviw.dll" );
#endif

OptionManager::OptionSet::OptionSet()
        : _memberFilter( DefaultMemFilt )
        , _enumStyle( EV_HexLowerCase )     // enum-view style is 0xa5
        , _treeAutoArrange( FALSE )
        , _treeDirection( TreeVertical )
        , _edgeType( EdgesStraight )
        , _ignoreCase( TRUE )
        , _wholeWord( FALSE )
        , _useRX( FALSE )
        , _rxOptions( DefaultRXOpt )
        , _classOpts( GTClassOpts() )
        , _functionOpts( GTFunctionOpts() )
        , _autoSave( FALSE )
//----------------------------------------------------------------------
{
}

static OptionManager::OptionSet OptionManager::_default;

OptionManager::OptionManager()
        : _menuManager( NULL )
        , _modified( FALSE )
        , _fileName( NULL )
        , _queryConfig( NULL )
        , _editorName( DefaultEditor )
        , _editorIsDLL( DefaultEditorIsDLL )
        , _editorParms( DefaultEditorParms )
//------------------------------------------
{
}

OptionManager::~OptionManager()
//-----------------------------
{
    delete _fileName;
    delete _queryConfig;
    delete _inheritOption;
    delete _callOption;
}

static MIMenuID OptManagerMenus[] = {
    MIMenuID( MMFile,           FMLoad ),
    MIMenuID( MMFile,           FMSave ),
    MIMenuID( MMFile,           FMSaveAs ),
    MIMenuID( MMOptions,        OMRegExp ),
    MIMenuID( MMOptions,        OMMemberFilters ),
    MIMenuID( MMOptions,        OMQuery ),
    MIMenuID( MMOptions,        OMEnumStyles ),
    MIMenuID( MMOptions,        OMTreeAutoArrange ),
    MIMenuID( MMOptions,        OMTreeSquareLines ),
    MIMenuID( MMOptions,        OMTreeHorizontal ),
    MIMenuID( MMOptions,        OMInheritTreeLegend ),
    MIMenuID( MMOptions,        OMCallTreeLegend ),
    MIMenuID( MMOptions,        OMEditorDLL ),
    MIMenuID( MMOptions,        OMAutoSave ),
};

#define NumOptMgrMenus ( sizeof( OptManagerMenus ) / sizeof( MIMenuID ) )

void OptionManager::setMenus( MenuManager * mgr )
//-----------------------------------------------
{
    int i;

    _menuManager = mgr;
    for( i = 0; i < NumOptMgrMenus; i += 1 ) {
        _menuManager->registerForMenu( this, OptManagerMenus[ i ] );
    }
}

void OptionManager::unSetMenus( MenuManager * )
//---------------------------------------------
{
    int i;

    for( i = 0; i < NumOptMgrMenus; i += 1 ) {
        _menuManager->unRegister( OptManagerMenus[ i ] );
    }

    _menuManager = NULL;
}

void OptionManager::menuSelected( const MIMenuID & id )
//-----------------------------------------------------
{
    switch( id.mainID() ) {
    case MMFile:
        switch( id.subID() ) {
        case FMLoad:
            load();
            break;
        case FMSave:
            if( hasFile() ) {
                saveTo( _fileName->gets() );
            } else {
                saveTo( DefaultOBRFile );
            }
            break;
        case FMSaveAs:
            save();
            break;
        default:
            NODEFAULT;
        }
        break;
    case MMOptions:
        switch( id.subID() ) {
        case OMRegExp:
            editRegExp();
            break;
        case OMMemberFilters:
            editMemberFilters();
            break;
        case OMQuery:
            editQuery();
            break;
        case OMEnumStyles:
            editEnumStyle();
            break;
        case OMTreeAutoArrange:
            toggleTreeAutoArrange();
            break;
        case OMTreeSquareLines:
            toggleEdgeType();
            break;
        case OMTreeHorizontal:
            toggleTreeDirection();
            break;
        case OMInheritTreeLegend:
            editInheritLegend();
            break;
        case OMCallTreeLegend:
            editCallLegend();
            break;
        case OMEditorDLL:
            editEditorDLL();
            break;
        case OMAutoSave:
            toggleAutoSave();
            break;
        default:
            NODEFAULT;
        }
        break;
    default:
        NODEFAULT;
    }
}

bool OptionManager::exit()
//------------------------
{
    MsgRetType ret;

    if( _current._autoSave && hasFile() ) {
        saveTo( _fileName->gets() );
        return TRUE;    // ok to terminate
    }

    if( _modified || _current._autoSave ) {
        if( hasFile() ) {
            ret = WMessageDialog::messagef( topWindow, MsgQuestion,
                                        MsgYesNoCancel, "Source Browser",
                                        "Save Option changes to %s?",
                                        _fileName->gets() );
        } else {
            ret = WMessageDialog::message( topWindow, MsgQuestion,
                                        MsgYesNoCancel,
                                        "Save Option changes?",
                                        "Source Browser" );
        }
        switch( ret ) {
        case MsgRetYes:
            if( hasFile() ) {
                saveTo( _fileName->gets() );
                return TRUE;
            } else {
                return save();      // terminate only if save works
            }
        case MsgRetNo:
            return TRUE;            // terminate
        default:
            return FALSE;           // don't terminate
        }
    }
    return TRUE;
}

bool OptionManager::isModified() const
//------------------------------------
{
    return _modified;
}

void OptionManager::editMemberFilters()
//-------------------------------------
{
    MemberFilterDlg dlg( topWindow, _default._memberFilter,
                         _current._memberFilter );

    if( dlg.process() ) {
        modified();
    }
}

void OptionManager::editEnumStyle()
//---------------------------------
{
    EnumStyleEdit dlg( topWindow, _default._enumStyle,
                        _current._enumStyle );

    if( dlg.process() ) {
        modified();
    }
}

void OptionManager::toggleTreeAutoArrange()
//-----------------------------------------
{
    _current._treeAutoArrange = !_current._treeAutoArrange;
    modified();
}

void OptionManager::toggleEdgeType()
//----------------------------------
{
    _current._edgeType = (_current._edgeType == EdgesSquare) ?
                         EdgesStraight : EdgesSquare;
    modified();
}

void OptionManager::toggleTreeDirection()
//---------------------------------------
{
    _current._treeDirection = (_current._treeDirection == TreeVertical) ?
                         TreeHorizontal : TreeVertical;
    modified();
}

void OptionManager::editInheritLegend()
//-------------------------------------
{
    if( !_inheritOption ) {
        _inheritOption = new GTInheritOption( _current._classOpts );
    }
    _inheritOption->process( topWindow );
}

void OptionManager::editCallLegend()
//----------------------------------
{
    if( !_callOption ) {
        _callOption = new GTCallOption( _current._functionOpts );
    }
    _callOption->process( topWindow );
}

void OptionManager::editRegExp()
//------------------------------
{
    RegExpCfg rxcfg( topWindow, _default._rxOptions, _current._rxOptions );

    if( rxcfg.process() ) {
        modified();
    }
}

void OptionManager::editQuery()
//-----------------------------
{
    if( !_queryConfig ) {
        _queryConfig = new QueryConfig( &_queryKey, NULL, "Query" );
    }

    if( _queryConfig->process( topWindow ) ) {
        modified();
        WBRWinBase::viewManager()->eventOccured( VEQueryFiltChange, NULL );
    }
}

void OptionManager::editEditorDLL()
//---------------------------------
{
    NewEditDLL  edit( browseTop, _editorName, _editorIsDLL, _editorParms,
                      DefaultEditor, DefaultEditorIsDLL, DefaultEditorParms );

    if( edit.process() ) {
        setEditor( edit.getEditor(), edit.isEditorDLL(), edit.getEditorParms());
    }
}

void OptionManager::toggleAutoSave()
//----------------------------------
{
    _current._autoSave = !_current._autoSave;
    modified();
}

void OptionManager::checkMenus()
//------------------------------
{
    _menuManager->checkMenu( MIMenuID( MMOptions, OMAutoSave ),
                             _current._autoSave );
    _menuManager->checkMenu( MIMenuID( MMOptions, OMTreeAutoArrange ),
                             _current._treeAutoArrange );
    _menuManager->checkMenu( MIMenuID( MMOptions, OMTreeSquareLines ),
                             _current._edgeType == EdgesSquare );
    _menuManager->checkMenu( MIMenuID( MMOptions, OMTreeHorizontal ),
                             _current._treeDirection == TreeHorizontal );
}

const char * OptionManager::fileName() const
//------------------------------------------
// FIXME -- returns full pathname all the time
{
    const char * NoOptFileMsg = "no option file";

    if( hasFile() ) {
        return _fileName->gets();
    } else {
        return NoOptFileMsg;
    }
}

void OptionManager::modified()
//----------------------------
{
    checkMenus();

    if( !_modified ) {
        _modified = TRUE;
        browseTop->postTitle();
    }

    WBRWinBase::viewManager()->eventOccured( VEOptionValChange, NULL );
}

bool OptionManager::hasFile() const
//---------------------------------
{
    return( _fileName != NULL );
}

bool OptionManager::load()
//------------------------
{
    WFileDialog fileselect( topWindow, OBRFilter );

    if( _modified && _current._autoSave ) {
        MsgRetType ret;

        ret = WMessageDialog::message( topWindow, MsgQuestion, MsgYesNo,
                                        "Save Option Changes?" );
        if( ret == MsgRetYes ) {
            if( !save() ) {
                return FALSE;
            }
        }
        _modified = FALSE;
    }

    const char* result = fileselect.getOpenFileName( NULL,
                                        "Load Options File" );
    if( result && *result ) {
        loadFrom( result );
        return TRUE;
    } else {
        return FALSE;
    }
}

void OptionManager::loadDefault()
//-------------------------------
{
    FileInfo finf( DefaultOBRFile );

    if( finf.exists() ) {
        loadFrom( DefaultOBRFile );
    }
}

void OptionManager::loadFrom( const char * file )
//-----------------------------------------------
// FIXME -- this is too primitive -- need search path
{
    BusyNotice          busy( "Loading Options..." );
    int                 len;
    char                sigCheck[ SIGLEN + VERLEN ];

    if( _modified && _current._autoSave ) {
        MsgRetType ret;

        ret = WMessageDialog::message( topWindow, MsgQuestion, MsgYesNo,
                                        "Save Option Changes?" );
        if( ret == MsgRetYes ) {
            save();
        }
    }

    delete _fileName;
    _fileName = new WFileName( file );
    _fileName->toLower();

    CheckedBufferedFile loadFile( _fileName->gets() );

    try {
        loadFile.open( CheckedFile::ReadBinary, CheckedFile::UserReadWrite );

        len = loadFile.read( sigCheck, SIGLEN + VERLEN );
        if( len != SIGLEN + VERLEN ) {
            errMessage( "%s is not a valid option file", file );
            throw DEATH_BY_FILE_READING;    // fixme -- change throw
        }

        if( memcmp( sigCheck, SIG, SIGLEN ) ) {
            errMessage( "%s is not a valid option file", file );
            throw DEATH_BY_FILE_READING;    // fixme -- change throw
        }

        if( memcmp( sigCheck + SIGLEN, VER, VERLEN ) ) {
            errMessage( "%s is an out-of-date option file -- ignoring", file );
            throw DEATH_BY_FILE_READING;    // fixme -- change throw
        }

        len = loadFile.read( &_initial, sizeof( OptionSet ) );

        if( len != sizeof( OptionSet ) ) {
            errMessage( "%s is not a valid option file", file );
            throw DEATH_BY_FILE_READING;    // fixme -- change throw
        }

        // just in case they are not there
        _editorIsDLL = DefaultEditorIsDLL;
        _editorName = DefaultEditor;
        _editorParms = DefaultEditorParms;

        _queryKey.read( loadFile );
        loadFile.readNString( _editorName );
        loadFile.read( &_editorIsDLL, sizeof( _editorIsDLL ) );
        loadFile.readNString( _editorParms );

        memcpy( &_current, &_initial, sizeof( OptionSet ) );

        delete _queryConfig;        // make sure pick up new patterns.
        _queryConfig = NULL;

        WBRWinBase::viewManager()->eventOccured( VEOptionFileChange, NULL );
        WBRWinBase::viewManager()->eventOccured( VEQueryFiltChange, NULL );
        modified();
        _modified = FALSE;
        browseTop->postTitle();
    } catch( FileExcept file ) {
        errMessage( "%s: %s", file._fileName, file._message );
    } catch( CauseOfDeath cause ) {
        loadFile.close();
        IdentifyAssassin( cause );
    }
}

bool OptionManager::save()
//------------------------
{
    WFileDialog fileselect( topWindow, OBRFilter );

    const char * result;

    result = fileselect.getSaveFileName( NULL,
                                        "Save Options File" );
    if( result && *result ) {
        saveTo( result );
        return TRUE;
    } else {
        return FALSE;
    }
}

void OptionManager::saveTo( const char * file )
//---------------------------------------------
// NYI -- back up old copy if there is one, use signature
{
    WString             str;

    if( _fileName ) {
        *_fileName = file;
    } else {
        _fileName = new WFileName( file );
    }
    _fileName->toLower();

    CheckedBufferedFile saveFile( _fileName->gets() );

    try {
        saveFile.open( CheckedFile::WriteBinary, CheckedFile::UserReadWrite );

        saveFile.write( SIG, SIGLEN );
        saveFile.write( VER, VERLEN );
        saveFile.write( &_current, sizeof( OptionSet ) );

        _queryKey.write( saveFile );
        saveFile.writeNString( _editorName );
        saveFile.write( &_editorIsDLL, sizeof( _editorIsDLL ) );
        saveFile.writeNString( _editorParms );

        saveFile.close();
        _modified = FALSE;
        browseTop->postTitle();
    } catch( FileExcept file ) {
        errMessage( "%s: %s", file._fileName, file._message );
    } catch( CauseOfDeath cause ) {
        saveFile.close();
        IdentifyAssassin( cause );
    }
}

MemberFilter OptionManager::getMemberFilter() const
//-------------------------------------------------
{
    return _current._memberFilter;
}

EnumViewStyle OptionManager::getEnumStyle() const
//-----------------------------------------------
{
    return _current._enumStyle;
}

bool OptionManager::getTreeAutoArrange() const
//--------------------------------------------
{
    return _current._treeAutoArrange;
}

TreeDirection OptionManager::getTreeDirection() const
//---------------------------------------------------
{
    return _current._treeDirection;
}

EdgeType OptionManager::getEdgeType() const
//-----------------------------------------
{
    return _current._edgeType;
}

bool OptionManager::getIgnoreCase() const
//---------------------------------------
{
    return _current._ignoreCase;
}

bool OptionManager::getWholeWord() const
//--------------------------------------
{
    return _current._wholeWord;
}

bool OptionManager::getUseRX() const
//----------------------------------
{
    return _current._useRX;
}

bool OptionManager::getAnchored() const
//-------------------------------------
{
    return _current._rxOptions._anchored;
}

const char * OptionManager::getMagicString() const
//------------------------------------------------
{
    return _current._rxOptions._magicString;
}

bool OptionManager::getAutoSave() const
//-------------------------------------
{
    return _current._autoSave;
}

const GTClassOpts & OptionManager::getClassOpts() const
//-----------------------------------------------------
{
    return _current._classOpts;
}

const GTFunctionOpts & OptionManager::getFunctionOpts() const
//-----------------------------------------------------------
{
    return _current._functionOpts;
}

const KeySymbol & OptionManager::getQueryFilt() const
//---------------------------------------------------
{
    return _queryKey;
}

const char * OptionManager::getEditorName() const
//----------------------------------------------------
{
    return _editorName;
}

bool OptionManager::isEditorDLL() const
//-------------------------------------
{
    return _editorIsDLL;
}

const char * OptionManager::getEditorParms() const
//-----------------------------------------------------
{
    return _editorParms;
}

void OptionManager::setMemberFilter( MemberFilter filt )
//------------------------------------------------------
{
    _current._memberFilter = filt;
}

void OptionManager::setEnumStyle( EnumViewStyle vs )
//--------------------------------------------------
{
    _current._enumStyle = vs;
}

void OptionManager::setTreeAutoArrange( bool arrange )
//----------------------------------------------------
{
    _current._treeAutoArrange = arrange;
}

void OptionManager::setTreeDirection( TreeDirection dir )
//-------------------------------------------------------
{
    _current._treeDirection = dir;
}

void OptionManager::setEdgeType( EdgeType type )
//----------------------------------------------
{
    _current._edgeType = type;
}

void OptionManager::setIgnoreCase( bool ignore )
//----------------------------------------------
{
    _current._ignoreCase = ignore;
}

void OptionManager::setWholeWord( bool whole )
//--------------------------------------------
{
    _current._wholeWord = whole;
}

void OptionManager::setUseRX( bool use )
//--------------------------------------
{
    _current._useRX = use;
}

void OptionManager::setAnchored( bool anchor )
//--------------------------------------------
{
    _current._rxOptions._anchored = anchor;
}

void OptionManager::setMagicString( const char * str )
//----------------------------------------------------
// FIXME -- no checking
{
    strncpy( _current._rxOptions._magicString, str, NumMagicChars + 1 );
}

void OptionManager::setAutoSave( bool set )
//-----------------------------------------
{
    _current._autoSave = set;
}

void OptionManager::setClassOpts( const GTClassOpts * op )
//--------------------------------------------------------
{
    if( op ) {
        _current._classOpts = *op;
        modified();
    }
}

void OptionManager::setFunctionOpts( const GTFunctionOpts * op )
//--------------------------------------------------------------
{
    if( op ) {
        _current._functionOpts = *op;
        modified();
    }
}

void OptionManager::setEditor( const char *name, bool isDLL, const char *parms )
//------------------------------------------------------------------------------
{
    _editorName = name;
    _editorIsDLL = isDLL;
    _editorParms = parms;
    modified();
}
