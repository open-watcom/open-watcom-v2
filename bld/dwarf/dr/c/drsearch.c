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
* Description:  Search DWARF debug information (useful for source browser).
*
****************************************************************************/


#include "drpriv.h"
#include "drutils.h"
#include <regexp.h>
#include <string.h>

typedef struct {
    DRSYMSRCH   callback;
    regexp      *name;
    void        *data;
    dr_search   searchtype;
} sym_search_data;

static bool DRSearchMacro( regexp *, void *, DRSYMSRCH cb );

static bool CheckEntry( dr_handle abbrev, dr_handle mod, mod_scan_info *minfo, void *data )
/*****************************************************************************************/
{
    int                 index;
    sym_search_data     *sinfo;
    dr_sym_context      symctxt;

    sinfo = (sym_search_data *)data;

    symctxt.handle = minfo->handle;
    symctxt.context = minfo->context;

    symctxt.name = NULL;
    if( sinfo->name != NULL ) {
        symctxt.name = DWRGetName( abbrev, mod );
        if( symctxt.name == NULL ) return( TRUE );
        if( !RegExec( sinfo->name, symctxt.name, TRUE ) ) {
            DWRFREE( symctxt.name );
            return( TRUE );
        }
    }

    symctxt.type = DR_SYM_MACRO;
    for( index = 0; index < DR_SYM_NOT_SYM; index++ ) {
        if( DWRSearchArray( SearchTags[index], minfo->tag ) ) {
            symctxt.type = index;
            break;
        }
    }

    return( sinfo->callback( &symctxt, sinfo->data ) );
}

extern bool DRSymSearch( dr_search search, dr_depth depth, void *_name,
                                        void *data, DRSYMSRCH callback )
/**********************************************************************/
// search the debugging information for interesting symbols (of type dr_search)
// optionally search inside lexical blocks or classes (dr_depth)
// optionally look for a particular name.
{
    sym_search_data info;
    bool            done = FALSE;
    regexp          *name = _name;

    if( search == DR_SEARCH_MACROS || search == DR_SEARCH_ALL ) {
        done |= DRSearchMacro( name, data, callback );
    }

    if( search != DR_SEARCH_MACROS ) {
        info.callback = callback;
        info.name = name;
        info.data = data;
        info.searchtype = search;
        done |= DWRScanAllCompileUnits( NULL, CheckEntry, SearchTypes[search], depth, &info );
    }

    return( done );
}

extern bool DRResumeSymSearch( dr_search_context *ctxt, dr_search search,
                               dr_depth depth, void *_name,
                               void *data,
                               DRSYMSRCH callback )
/************************************************************************/
// resume a search from context information in ctxt
{
    sym_search_data info;
    bool            done = FALSE;
    regexp          *name = _name;

    if( search == DR_SEARCH_MACROS || search == DR_SEARCH_ALL ) {
        done |= DRSearchMacro( name, data, callback );
    }

    if( search != DR_SEARCH_MACROS ) {
        info.callback = callback;
        info.name = name;
        info.data = data;
        info.searchtype = search;
        done |= DWRScanAllCompileUnits( ctxt, CheckEntry, SearchTypes[search], depth, &info );
    }

    return( done );
}

static bool DRSearchMacro( regexp *name, void * data, DRSYMSRCH callback )
/************************************************************************/
{
    name = name;            // just to avoid warnings.
    data = data;
    callback = callback;
    // NYI

    return( FALSE );        // more info, in case anyone checks
}

extern dr_search_context * DRDuplicateSearchContext( dr_search_context *cxt )
/***************************************************************************/
{
    int                 i;
    dr_search_context   *newCtxt;

    newCtxt = DWRALLOC( sizeof( dr_search_context ) );
    *newCtxt = *cxt; /* structure copy */


    /* but allocate and copy own stack */
    newCtxt->stack.stack = DWRALLOC( newCtxt->stack.size * sizeof( uint_32 ) );

    for( i = 0; i < cxt->stack.free; i += 1 ) {
        newCtxt->stack.stack[i] = cxt->stack.stack[i];
    }

    return( newCtxt );
}

extern void DRFreeSearchContext( dr_search_context *ctxt )
/********************************************************/
{
    DWRFREE( ctxt->stack.stack );
    DWRFREE( ctxt );
}
