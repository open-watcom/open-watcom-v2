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


#include <wtptlist.hpp>
#include "assure.h"
#include "classtyp.h"
#include "mem.h"
#include "optmgr.h"
#include "util.h"

const int POOLSIZE = 32;

#pragma warning 549 5           // sizeof contains compiler genned info.
MemoryPool ClassType::_pool( sizeof( ClassType ), "ClassType", POOLSIZE );
#pragma warning 549 3

struct MemberSearchData {
    ClassType *     me;
    WVList *        list;
    dr_access       access;
};

void * ClassType::operator new( size_t )
//--------------------------------------
{
    return( _pool.alloc() );
}

void ClassType::operator delete( void * mem )
//-------------------------------------------
{
    _pool.free( mem );
}

static bool CheckAccess( dr_handle handle, dr_access inherit )
//------------------------------------------------------------
// returns TRUE if the given handle is acceptable given the filters
{
    int          access;
    MemberFilter filt = WBRWinBase::optManager()->getMemberFilter();
    int          ret;

    if( filt._accessLevel == MemberFilter::AccAll ) {
        ret = TRUE;     // acess is ok
    } else {
        access = DRGetAccess( handle ) + inherit;

        switch( access ) {
        case DR_ACCESS_PRIVATE:
            ret = ( filt._accessLevel & MemberFilter::AccPrivate );
            break;
        case DR_ACCESS_PROTECTED:
            ret = ( filt._accessLevel & MemberFilter::AccProtected );
            break;
        case DR_ACCESS_PUBLIC:
            ret = ( filt._accessLevel & MemberFilter::AccPublic );
            break;
        default:
            ret = ( filt._inheritLevel & MemberFilter::ILAllInherited );
        }
    }

    return( ret != 0 );
}

static int ClassType::memberHook( dr_sym_type symtype, dr_handle handle,
                                  dr_handle prt, char * name, void * info )
//-------------------------------------------------------------------------
{
    MemberSearchData *  data = (MemberSearchData *) info;
    Symbol *            sym;
    bool                quit;
    MemberFilter        filt = WBRWinBase::optManager()->getMemberFilter();

    quit = FALSE;

    quit = !CheckAccess( handle, data->access );

    if( symtype == DR_SYM_FUNCTION ) {
        if( !quit && !(filt._members & MemberFilter::MemFuncStatic) ) {
            quit = ( DRIsFunctionStatic( handle ) != 0 );
        }
    } else {
        if( !quit && !(filt._members & MemberFilter::MemVarStatic) ) {
            quit = ( DRIsMemberStatic( handle ) != 0 );
        }
    }

    if( !quit && !(filt._members & MemberFilter::MemVirtual) ) {
        quit = ( DRGetVirtuality( handle ) == DR_VIRTUALITY_VIRTUAL );
    }

    if( !quit && !(filt._members & MemberFilter::MemVariables ) ) {
        quit = ( symtype != DR_SYM_FUNCTION );
    }

    if( !quit && !(filt._members & MemberFilter::MemFunctions ) ) {
        quit = ( symtype == DR_SYM_FUNCTION );
    }

    if( quit ) {
        WBRFree( name );
    } else {
        sym = defineSymbol( symtype, handle, prt,
                            data->me->getModule(), name );
        data->list->add( sym );
    }
    return TRUE;
}

void ClassType::dataMembers( WVList & list )
//------------------------------------------
{
    getMembers( list, DR_SEARCH_VARIABLES );
}

void ClassType::memberFunctions( WVList & list )
//----------------------------------------------
{
    getMembers( list, DR_SEARCH_FUNCTIONS );
}

void ClassType::friendFunctions( WVList & list )
//----------------------------------------------
{
    getMembers( list, DR_SEARCH_FRIENDS );
}

void ClassType::getMembers( WVList & list, dr_search srch )
//---------------------------------------------------------
{
    MemberSearchData data;
    ClassLattice *   latt = new ClassLattice( this );
    MemberFilter     filt = WBRWinBase::optManager()->getMemberFilter();

    data.me = this;
    data.list = &list;
    data.access = (dr_access) 0;

    if( srch == DR_SEARCH_FRIENDS ) {
        DRFriendsSearch( latt->getHandle(), &data, memberHook );
    } else {
        DRKidsSearch( latt->getHandle(), srch, &data,
                      memberHook );
    }

    if( filt._inheritLevel > MemberFilter::ILNoInherited ) {
        doBases( latt, srch, &data );
    }

    latt->deleteLattice();
}

void ClassType::doBases( ClassLattice * latt, dr_search srch,
                         MemberSearchData * data )
//-----------------------------------------------------------
{
    WVList bases;

    latt->baseClasses( bases );
    for( int i = bases.count(); i > 0; i -= 1 ) {
        ClassLattice * cls = (ClassLattice *) bases[ i - 1 ];
        data->access = cls->getAccess();

        if( srch == DR_SEARCH_FRIENDS ) {
            DRFriendsSearch( cls->getHandle(), data, memberHook );
        } else {
            DRKidsSearch( cls->getHandle(), srch, data, memberHook );
        }

        doBases( cls, srch, data );
    }
}
