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


#include <drsrchdf.h>
#include <drgetref.h>
#include <regexp.h>
#include <wcvector.h>
#include <wclist.h>
#include <wclistit.h>

#include <wmsgdlg.hpp>
#include <wfilenam.hpp>

#include "assure.h"
#include "browse.h"
#include "dwmerger.h"
#include "keysym.h"
#include "module.h"
#include "util.h"
#include "death.h"
#include "mem.h"
#include "symbol.h"
#include "referenc.h"
#include "fileinfo.h"

struct FindSymData {
    KeySymbol *         keysym;
    WVList *            data;
    Module *            call_module;
    SearchContext *     context;
    int                 numItems;
};

Module::Module( const char *name, WCValSList<String> & enabled,
                WCValSList<String> & disabled )
//------------------------------------------------------------
{
    WCPtrOrderedVector<ComponentFile> components;
    FileInfo                          finf( name );
    int                               i;
    MsgRetType                        ret;

    _dataFile = new ElfFile( name, FALSE );

    DwarfFileMerger merger( name, enabled, disabled );

    if( !merger.upToDate() ) {
        if( !finf.exists() ) {
            merger.doMerge();
        } else {
            if( enabled.entries() != 0 ) {
                ret = WMessageDialog::messagef( topWindow,
                        MsgQuestion, MsgYesNo, "Source Browser",
                        "Database %s is not consistent with module files.\n"
                        "Merge the database now?", name );

                if( ret == MsgRetYes ) {
                    merger.doMerge();
                }
            }
        }
    }

    _dataFile->initSections();

    _dataFile->getEnabledComponents( &components );
    for( i = 0; i < components.entries(); i += 1 ) {
        _enabledFiles.add( new WFileName( components[i]->name ) );
    }

    components.clear();
    _dataFile->getDisabledComponents( &components );
    for( i = 0; i < components.entries(); i += 1 ) {
        _disabledFiles.add( new WFileName( components[i]->name ) );
    }

    checkSourceTime();

    _dbgInfo = DRDbgInit( this, _dataFile->getDRSizes(), false );
    DRSetDebug( _dbgInfo );
}

Module::~Module()
//---------------
{
    if( _dataFile->isInitialized() ) {
        DRDbgFini( _dbgInfo );
    }
    delete _dataFile;
    _enabledFiles.deleteContents();
    _disabledFiles.deleteContents();
}

void Module::setModule()
//----------------------
// tell the dwarf library that we are reading out of this module
{
    REQUIRE( _dataFile->isInitialized(), "Module::setModule - not initialized" );
    DRSetDebug( _dbgInfo );
}

static bool searchHook( dr_sym_context * symctxt, void *data )
//------------------------------------------------------------
{
    FindSymData * info;
    bool          accept;
    Symbol *      sym;

    info = (FindSymData *) data;
    if( symctxt->name == NULL ) {
        symctxt->name = DRGetName( symctxt->handle );
    }
    accept = info->keysym->matches( symctxt );

    if( accept ) {
        if( info->numItems == 0 ) {
            WBRFree( symctxt->name );
            symctxt->name = NULL;
            info->context = new SearchContext( *symctxt->context );
            return FALSE;   // <----- early return -- stop search
        }

        if( info->numItems > 0 ) {
            info->numItems -= 1;
        }

        sym = Symbol::defineSymbol( symctxt->type, symctxt->handle,
                                    symctxt->context->classhdl,
                                    info->call_module,
                                    symctxt->name );
        info->data->add( sym );

#if DEBUG
        if( !(info->data->count() % 10) ) {
            WString stat;
            stat.printf( "Loading - %d found", info->data->count() );
            browseTop->statusText( stat.gets() );
        }
#endif
    } else {
        WBRFree( symctxt->name );
    }

    return TRUE;    // continue searching
}

SearchContext * Module::findSymbols( WVList *symbols, KeySymbol *filter,
                                      SearchContext * ctxt, int numItems )
//-------------------------------------------------------------------------
{
    bool        done;
    FindSymData info;
    unsigned    depth;

    ASSERTION( _dataFile->isInitialized() );

    depth = DR_DEPTH_FUNCTIONS;
    if( DRGetLanguage() == DR_LANG_CPLUSPLUS ) {
        depth |= DR_DEPTH_CLASSES;
    }

    info.keysym = filter;
    info.data = symbols;
    info.call_module = this;
    info.numItems = numItems;
    info.context = NULL;

    if( ctxt ) {
        done = DRResumeSymSearch( ctxt, DR_SEARCH_ALL, (dr_depth)depth,
                                  NULL, &info, searchHook);
    } else {
        done = DRSymSearch( DR_SEARCH_ALL, (dr_depth) depth,
                            NULL, &info, searchHook);
    }

    if( done ) {
        delete info.context;
        return NULL;
    } else {
        return info.context;
    }
}

static bool referenceHook( dr_handle, dr_ref_info * refinfo, char * name,
                           void * info )
//-----------------------------------------------------------------------
{
    Reference * ref;
    WVList *    list = (WVList *) info;

    for( int i = 0; i < list->count(); i++ ) {
        ref = (Reference *) (*list)[i];
        if( ref->line() == refinfo->line && ref->column() == refinfo->column
                && strcmp( ref->sourceFile(), refinfo->file ) == 0 ) {
            WBRFree( name );
            return TRUE;
        }
    }
    list->add( new Reference( refinfo, name ) );
    return TRUE;    // keep going
}

bool Module::findReferences( WVList *list, Symbol *data )
//-------------------------------------------------------
{
    REQUIRE( _dataFile->isInitialized(), "Module::findReferences -- not init" );

    DRReferredToBy( data->getHandle(), list, &referenceHook );
    return list->count() != 0;
}

struct RefSymData {
    Module *    me;
    WVList *    list;
};

static bool refSymHook( dr_handle handle, dr_ref_info *, char * name,
                        void * info )
//-------------------------------------------------------------------
{
    RefSymData *data = (RefSymData *) info;
    dr_sym_type stype;
    Symbol *    sym;
    int         i;

    for( i = 0; i < data->list->count(); i += 1 ) {
        if( ((Symbol *)(*data->list)[ i ])->getHandle() == handle ) {
            WBRFree( name );
            return TRUE;        // <---- early return
        }
    }

    stype = DRGetSymType( handle );
    sym = Symbol::defineSymbol( stype, handle, 0, data->me, name );
    data->list->add( sym );

    return TRUE;    // continue
}

bool Module::findRefSyms( WVList *list, Symbol * sym )
//----------------------------------------------------
{
    RefSymData  data;

    REQUIRE( _dataFile->isInitialized(), "Module::findRefSymbols -- not init" );

    data.me = this;
    data.list = list;

    DRReferredToBy( sym->getHandle(), &data, &refSymHook );
    return list->count() != 0;
}

/*
 * these are the routines which are called by the debug reading library
*/

static void dbgReadHook( void * obj, dr_section sect, void * buf, size_t len )
//-------------------------------------------------------------------------
{
    ((Module *)obj)->readSect( sect, buf, len );
}

static void dbgSeekHook( void * obj, dr_section sect, long pos )
//--------------------------------------------------------------
{
    ((Module *)obj)->seekSect( sect, pos );
}

static void dbgExceptHook( dr_except type )
//-----------------------------------------
{
    throw (CauseOfDeath) (DEATH_BY_KILLER_DWARFS + type);
}

extern "C" {

DWRSetRtns(dbgReadHook,dbgSeekHook,WBRAlloc,WBRRealloc,WBRFree,dbgExceptHook)

};

void Module::readSect( dr_section sect, void * buf, int len )
//-----------------------------------------------------------
{
    REQUIRE( _dataFile->isInitialized(), "Module::findSymbols -- not init" );

    _dataFile->readSect( sect, buf, len );
}

void Module::seekSect( dr_section sect, long pos )
//------------------------------------------------
{
    REQUIRE( _dataFile->isInitialized(), "Module::findSymbols -- not init" );

    _dataFile->seekSect( sect, pos );
}

void Module::checkSourceTime()
//----------------------------
{
}
