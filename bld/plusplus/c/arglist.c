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

#include "plusplus.h"
#include "cgfront.h"
#include "memmgr.h"
#include "ring.h"
#include "initdefs.h"

typedef struct {                // temp_arg_list : arg_list + link
    void* next;                 // - next in ring
    arg_list real;              // - visible arg_list (returned)
} temp_arg_list;

static temp_arg_list* tempList; // list of allocated, but unfreed temps


arg_list *InitArgList( arg_list *args )
/*************************************/
{
    args->num_args = 0;
    args->except_spec = NULL;
    args->qualifier = TF1_NULL;
    args->type_list[0] = NULL;
    return( args );
}


arg_list *AllocArgListPerm( int num_args )
/****************************************/
{
    size_t      amt;
    arg_list    *retn;

    amt = sizeof( arg_list );
    if( num_args > 0 ) {
        amt += ( num_args - 1 ) * sizeof( TYPE );
    }
    retn = CPermAlloc( amt );
    retn = InitArgList( retn );
    retn->num_args = num_args;
    return( retn );
}


arg_list *AllocArgListTemp( int num_args )
/****************************************/
{
    size_t      amt;
    arg_list    *retn;

    amt = sizeof( arg_list );
    if( num_args > 0 ) {
        amt += ( num_args - 1 ) * sizeof( TYPE );
    }
    retn = CMemAlloc( amt );
    retn = InitArgList( retn );
    retn->num_args = num_args;
    return( retn );
}


arg_list* ArgListTempAlloc(     // ALLOCATE TEMPORARY ARG LIST
    TEMP_ARG_LIST* default_args,// - default args
    unsigned count )            // - # arguments
{
    arg_list* retn;             // - arg_list to be used

    if( count <= AUTO_ARG_MAX ) {
        retn = (arg_list*)default_args;
    } else {
        temp_arg_list* temp;
        temp = (temp_arg_list*)CMemAlloc( count * sizeof( TYPE )
                                        - sizeof( TYPE )
                                        + sizeof( temp_arg_list ) );
        retn = &temp->real;
        RingAppend( &tempList, temp );
    }
    retn = InitArgList( retn );
    retn->num_args = count;
    return retn;
}


// Invoked by ArgListTempFree Macro
//
void ArgListTempFreeMem(        // FREE TEMPORARY ARG LIST
    arg_list* argl )            // - to be freed
{
    temp_arg_list* to_free = (temp_arg_list*)
        ( (char*)argl - offsetof( temp_arg_list, real ) );
    RingPrune( &tempList, to_free );
    CMemFree( to_free );
}


#if 0
void ArgListTempFree(           // FREE TEMPORARY ARG LIST
    arg_list* used,             // - used list
    unsigned count )            // - # arguments
{
    if( count > AUTO_ARG_MAX ) {
        CMemFree( used );
    }
}
#endif


#if 0
PTREE* PtListAlloc(             // ALLOCATE PTREE LIST (TEMPORARILY)
    PTREE* default_list,        // - default list
    unsigned count )            // - # arguments
{
    PTREE* retn;                // - PTREE list to be used

    if( count <= AUTO_ARG_MAX ) {
        retn = default_list;
    } else {
        retn = (PTREE*)CMemAlloc( count * sizeof( PTREE ) );
    }
    return retn;
}
#endif


#if 0
void PtListFree(                // FREE PTREE LIST
    PTREE* actual_list,         // - list used
    unsigned count )            // - # arguments
{
    if( count > AUTO_ARG_MAX ) {
        CMemFree( actual_list );
    }
}
#endif


arg_list *MakeMemberArgList( SYMBOL sym, int num_args )
/*****************************************************/
// allocate and initialize an argument list with num_args entries.
// designed for member functions that need a this_ptr inserted
// at the start of the arg list
// this is a temporary structure used by fnovload.c
{
    arg_list *mocklist;
    arg_list *func_args;

    mocklist = AllocArgListTemp( num_args );
    func_args = SymFuncArgList( sym );
    mocklist->except_spec = func_args->except_spec;
    mocklist->type_list[0] = TypeThisSymbol( sym, TRUE );
    if( num_args > 1 ) {
        memcpy( &mocklist->type_list[1]
              , func_args->type_list
              , sizeof( func_args->type_list[0] ) * ( num_args - 1 ) );
    }
    return( mocklist );
}


arg_list *MakeMockArgList( TYPE type, int num_args )
/**************************************************/
// allocate and initialize an argument list with num_args entries.
// handle cases where list is shorter because it has an ellipsis
// or where list is longer by one because it has an ellipsis
// this is a temporary structure used by fnovload.c
{
    int       i;
    arg_list *mocklist;
    TYPE     *src, *dst;        // for copying

    type = FunctionDeclarationType( type );
    mocklist = AllocArgListTemp( num_args );
    mocklist->except_spec = type->u.f.args->except_spec;
    if( num_args > 0 ) {
        dst = mocklist->type_list;
        src = type->u.f.args->type_list;
        if( num_args < type->u.f.args->num_args ) {
            // copy ones we want from real list into mock list
            for( i = 0 ; i < num_args ; i++, dst++, src++ ) {
                *dst = *src;
            }
        } else {
            // copy all available from real list into mock list
            for( i = 0 ; i < type->u.f.args->num_args ; i++, dst++, src++ ) {
                *dst = *src;
            }
            // duplicate last entry from real list into mock list
            src--;
            for( ; i < num_args ; i++, dst++ ) {
                *dst = *src;
            }
        }
    }
    return( mocklist );
}


static void init(               // MODULE INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    tempList = NULL;
}


static void fini(               // MODULE COMPLETION
    INITFINI* defn )            // - definition
{
    defn = defn;
    tempList = NULL;
}


INITDEFN( argument_lists, init, fini );
