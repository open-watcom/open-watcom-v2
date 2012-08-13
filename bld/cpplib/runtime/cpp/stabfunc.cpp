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


#include "cpplib.h"
#include "rtexcept.h"

#if 0
#include "rtinit.h"


static RO_STATE* fun_state_table( // POINT AT STATE TABLE
    RO_DTREG* ro )              // - R/O entry
{
    return ro->fun.state_table;
}


static void* fun_point_object(  // POINT AT OBJECT FOR ENTRY
    RW_DTREG* rw,               // - R/W entry
    RT_STATE_VAR state_var )    // - state variable
{
    return (void*)( rw->base.ro
                      ->fun.state_table[ state_var - 1 ].u.data_offset
                  + (char*)rw );
}


static void fun_init_traverse(  // INITIALIZE TRAVERSAL DATA STRUCTURE
    STAB_TRAVERSE* traverse,    // - traversal information
    RW_DTREG* )                 // - read/write data
{
    traverse->obj_type = OBT_OBJECT;
}

static DTREG_VFTBL fun_vftable =// virtual functions for DTRG_FUN
{   &fun_state_table
,   &fun_point_object
,   &fun_init_traverse
};


static void init(               // PROGRAM INITIALIZATION
    void )
{
    CPPDATA( dtreg_vfptrs )[ DTRG_FUN ] = &fun_vftable;
}

extern "C" XI( CPPDATA( stab_init_fun ), init, INIT_PRIORITY_RUNTIME )

#ifdef _M_IX86
    #pragma aux CPPDATA(stab_init_fun) "_*";
#endif

#endif

extern "C"
_WPRTLINK
void CPPLIB( fun_register )(    // REGISTRATION FOR FUNCTION
    RW_DTREG* rw,               // - R/W block
    RO_DTREG* ro )              // - R/O block
{
    CPPLIB( base_register )( rw, ro, 0 );
}
