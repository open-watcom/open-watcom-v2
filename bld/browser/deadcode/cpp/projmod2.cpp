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


#include <stdlib.h>
#include "wbrdefs.h"
#include "assure.h"
#include "keysym.h"
#include "module.h"
#include "util.h"
#include "projmodl.h"
#include "opstream.h"
#include "opscan.h"
#include "opterms.h"
#include "mem.h"
#include "edmodule.h"

const FilterFlags DEF_FilterFlags[ Filt_Both ] =
                    {
                        FILT_ALL_DEFAULT,
                        FILT_ALL_DEFAULT,
                        FILT_ALL_DEFAULT
                    };

#if 0
TreeOptionCollection *  ProjectModel::getOptionCluster( TreeOptionCluster idx )
//-----------------------------------------------------------------------------
{
    REQUIRE( idx >= 0 && idx < TreeLastCluster, "ProjectModel::getopclster, bad idx" );

    return _treeOpCluster[ idx ];
}
#endif

Module * ProjectModel::findModule( const char * fileName )
//--------------------------------------------------------
{
    Module * result;

    result = NULL;
    for( int i = 0; i < _modules.count(); i++ ) {
        // have to make sure that PathString does an adequate comparison
        if( stricmp(((Module *)_modules[i])->fileName(), fileName) == 0 ) {
            result = (Module *)_modules[i];
            break;
        }
    }
    return result;
}

bool ProjectModel::addModule( const char *fileName )
//--------------------------------------------------
{
    #if 0 // NYI - come in here while busy!
    if( busy() ) {
        errMessage( "Browse database is still being changed; try again in a moment" );
        return FALSE;
    }
    #endif

    bool status = FALSE;
    bool found = FALSE;
    try {
        startWork();
        // check to see if module is not already in set:
        if( findModule( fileName ) == NULL && Module::create( this, fileName )) {
            _needsSave=TRUE;
            status = TRUE;
        }
        endWork();
    }
    catch(...) {                /* make sure endwork called! */
        endWork();
        throw;
    }
    return status;
}

void ProjectModel::addFile( const char * module, const char *fileName,
                            bool enable )
//--------------------------------------------------------------------
{
    Module * mod = findModule( module );

    if( !mod ) {
        mod = Module::create( this, module );
    }

    mod->addFile( fileName, enable );
}

bool ProjectModel::removeModule( const char *name )
//-------------------------------------------------
{
    if( busy() ) {
        errMessage( "Browse database is still being changed; try again in a moment" );
        return FALSE;
    }
    bool status = FALSE;
    try {
        startWork();
        Module *cooked = findModule( name );
        if( cooked != NULL ) {
            _modules.removeSame( cooked );
            delete cooked;
            _needsSave=TRUE;
            status = TRUE;
        }
        endWork();
    }
    catch(...) {                /* make sure endwork called! */
        endWork();
        throw;
    }
    return status;
}

bool ProjectModel::editModule( WWindow * prt, const char * fileName )
//-------------------------------------------------------------------
{
    int ret;

    startWork();
    Module *mod = findModule( fileName );

    if( !mod ) {
        mod = Module::create( this, fileName );
    }

    WRect rect;
    prt->getRectangle( rect );
    EdModule edmod = EdModule( prt, mod );

    ret = edmod.process();

    endWork();

    return (bool) ret;
}

void ProjectModel::checkModuleTimeStamp()
//---------------------------------------
// check to see if any modules need to be re-merged
{
    for( int i = 0; i < _modules.count(); i += 1 ) {
        ((Module *)_modules[i])->checkTimeDate();
    }
}

bool ProjectModel::enableModule( const char *fileName, bool state )
//-----------------------------------------------------------------
{
#if 0
    if( busy() ) {
        errMessage( "Browse database is still being changed; try again in a moment" );
        return FALSE;
    }
#endif

    bool status = FALSE;
    try {
        startWork();
        Module *mod = findModule( fileName );
        if( mod != NULL ) {
            mod->enableModule( state );

            _needsSave=TRUE;
            status = TRUE;
        }
        endWork();
    }
    catch(...) {                /* make sure endwork called! */
        endWork();
        throw;
    }
    return status;
}

bool ProjectModel::iterateModules( WVList *syms, void *data, modfn fn )
//---------------------------------------------------------------------
{
    if( busy() ) {
        errMessage( "Browse database is still being changed; try again in a moment" );
        return FALSE;
    }
    bool found = FALSE;
    Module * mod;
    for( int i = 0; i < _modules.count(); i++ ) {
        mod = (Module *) _modules[i];
        if( mod->enabled() ) {
            found |= (mod->*fn)( syms, data );
        }
    }
    return found;
}

ProjSearchCtxt * ProjectModel::findSymbols( WVList *symbols,
                                            KeySymbol *filter,
                                            ProjSearchCtxt * ctxt,
                                            int numItems )
//-------------------------------------------------------------------
{
    Module *            mod;
    ProjSearchCtxt *    ret;

    if( busy() ) {
        errMessage( "Browse database is still being changed; try again in a moment" );
        return FALSE;
    }

    for( int i = (ctxt) ? ctxt->_modIdx : 0; i < _modules.count(); i++ ) {
        mod = (Module *) _modules[i];
        if( mod->enabled() ) {
            ret = mod->findSymbols( symbols, filter, ctxt, numItems );
            if( ret ) {
                ret->_modIdx = i;
                break;
            }
        }
    }

    return ret;
}

bool ProjectModel::findReferences( WVList *dest, Symbol *sym )
//------------------------------------------------------------
{
    return iterateModules( dest, sym, Module::findReferences );
}

bool ProjectModel::findRefSyms( WVList *dest, Symbol *sym )
//---------------------------------------------------------
{
    return iterateModules( dest, sym, Module::findRefSyms );
}

bool ProjectModel::findScopedSymbols( WVList &symInfos, const char *name,
                                      WFileName &file, ulong line, uint column )
//------------------------------------------------------------------------------
// finds same-name symbols in scope at point line,column,file
{
    if( busy() ) {
        errMessage( "Browse database is still being changed; try again in a moment" );
        return FALSE;
    }
    bool filePosFound = FALSE;
    Module * mod;
    for( int i = 0; i < _modules.count(); i++ ) {
        mod = (Module *) _modules[i];
        if( mod->enabled() ) {
            filePosFound = mod->findScopedSymbols( symInfos, name ,file, line,
                                                   column );
            if( filePosFound ) break;
        }
    }
    return filePosFound;
}

void ProjectModel::setAllDefault()
//--------------------------------
{
    for( int i = 0; i < Filt_Both; i += 1 ) {
        _flags[ i ] = DEF_FilterFlags[ i ];
        _flagsSet[ i ] = FALSE;
    }

    _treeDirect = TreeHorizontal;
    _smartEdges = TRUE;
}

FilterFlags ProjectModel::setFiltFlags( FilterSets set, FilterFlags flags )
//-------------------------------------------------------------------------
{
    REQUIRE( set <= Filt_Both && set >= 0, "ProjectModel::setfiltflags ack" );
    FilterFlags ret = _flags[ set - 1 ];

    _flags[ set - 1 ] = flags;
    _flagsSet[ set - 1 ] = TRUE;

    return ret;
}

FilterFlags ProjectModel::getFiltFlags( FilterSets set ) const
//------------------------------------------------------------
{
    REQUIRE( set < Filt_Both && set >= 0, "ProjectModel::getfiltflags ack" );

    // if these flags weren't set, check to see if both were set. if not,
    // reply with the default.

    if( !_flagsSet[ set - 1 ] && _flagsSet[ Filt_Both - 1 ] ) {
        return _flags[ Filt_Both - 1 ];
    } else {
        return _flags[ set - 1 ];
    }
}

void ProjectModel::setMagic( MagicOptions magic )
//-----------------------------------------------
{
    magic = magic;
    #if 0
    switch( magic ) {
        case Magic_All :
            MagicFlag = TRUE;
            break;
        case Magic_None :
            MagicFlag = FALSE;
            SearchOptions::noMagic = TRUE;
            break;
        case Magic_Some :
            MagicFlag = FALSE;
            SearchOptions::noMagic = FALSE;
            break;
    }
    #endif
}


void ProjectModel::setMagicString( char * magicString )
//-----------------------------------------------------
{
    magicString = magicString;
    #if 0
    if( magicString != NULL ) {
        if( SearchOptions::userMagicString != NULL ) {
            WBRFree( SearchOptions::userMagicString );
        }
        SearchOptions::userMagicString = WBRStrDup( magicString );
    }
    #endif
}

void ProjectModel::getFile( WFileName & file )
//--------------------------------------------
{
    if( _appFile ) {
        file = *_appFile;
    } else {
        file.puts( "" );
    }
}

const char * ProjectModel::file()
//-------------------------------
{
    if( _appFile ) {
        return( _appFile->fName() );
    } else {
        return "";
    }
}

void ProjectModel::tellMeIncluded( char * fileName )
//--------------------------------------------------
{
    if( !_hasFile ) {
        _hasFile = TRUE;
        _appFile = new WFileName( fileName );
    }
}

ProjSearchCtxt::ProjSearchCtxt( int modIdx )
                    : _modIdx( modIdx )
                    , _context( NULL )
//------------------------------------------
{
}

ProjSearchCtxt::~ProjSearchCtxt()
//-------------------------------
{
    if( _context ) {
        DRFreeSearchContext( _context );
    }
}

void ProjSearchCtxt::setContext( dr_search_context * ctxt )
//---------------------------------------------------------
{
    if( _context ) {
        DRFreeSearchContext( _context );
    }

    _context = DRDuplicateSearchContext( ctxt );
}
