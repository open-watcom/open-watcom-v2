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


#include <drgetref.h>
#include "module.h"
#include "mem.h"
#include "funcsym.h"

const int POOLSIZE = 32;

#pragma warning 549 9           // sizeof contains compiler genned info.
MemoryPool FunctionSym::_pool( sizeof( FunctionSym ), "FunctionSym", POOLSIZE );
#pragma warning 549 3

struct FuncSearchData {
    FunctionSym *   me;
    WVList *        list;
};

void * FunctionSym::operator new( size_t )
//----------------------------------------
{
    return( _pool.alloc() );
}

void FunctionSym::operator delete( void * mem )
//---------------------------------------------
{
    _pool.free( mem );
}

static int FunctionSym::memberHook( dr_sym_type symtype, dr_handle handle,
                                    dr_handle prt, char * name, void * info )
//---------------------------------------------------------------------------
{
    FuncSearchData * data = ( FuncSearchData * ) info;
    Symbol *         sym;

    sym = defineSymbol( symtype, handle, prt, data->me->getModule(), name );
    data->list->add( sym );

    return TRUE;
}


void FunctionSym::localVars( WVList & list )
//------------------------------------------
{
    FuncSearchData data;
    data.me = this;
    data.list = &list;
    DRKidsSearch( getHandle(), DR_SEARCH_VARIABLES, &data, memberHook );
}

struct callSearchData {
    FunctionSym * me;
    WVList *      list;
};

void FunctionSym::callees( WVList & list )
//----------------------------------------
{
    callSearchData data;

    data.me = this;
    data.list = &list;

    DRRefersTo( getHandle(), &data, &callHook );
}

void FunctionSym::callers( WVList & list )
//----------------------------------------
{
    int i;

    getModule()->findRefSyms( &list, this );

    for( i = 0; i < list.count(); i += 1 ) {
        Symbol * sym = (Symbol *) list[ i ];
        if( sym->symtype() != DR_SYM_FUNCTION ) {
            list.removeAt( i );
        }
    }
}

static bool FunctionSym::callHook( dr_handle, dr_ref_info * ref, char * name,
                                   void * info )
//---------------------------------------------------------------------------
{
    Symbol *         sym;
    callSearchData * data = (callSearchData *) info;
    dr_handle        other;
    dr_sym_type      stype;

    other = ref->dependent;
    stype = DRGetSymType( other );
    if( stype == DR_SYM_FUNCTION ) {
        sym = Symbol::defineSymbol(stype, other, 0, data->me->getModule(),name);
        data->list->add( sym );
    } else {
        WBRFree( name );
    }
    return TRUE;    // continue
}
