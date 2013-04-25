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
#include "rtinit.h"


extern "C"
void CPPLIB( module_dtor )      // DTOR STATICS FOR PROGRAM
    ( void )
{
    THREAD_CTL *thr;            // - thread control ptr.
    RW_DTREG* rw;               // - read/write entry
    union {
        RW_DTREG inlined;       // - inlined copy for destruction
        RW_DTREG_INITLS ls;     // - inlined copy for destruction
    };


#ifdef RW_REGISTRATION
    inlined.base.handler = & CPPLIB( fs_handler_rtn );
    thr = 0;
#else
    thr = &_RWD_ThreadData;
#endif
    for( ; ; ) {
#ifdef __MT__
        _RWD_StaticInitSema.p();
#endif
        rw = _RWD_ModuleInit;
        if( rw != 0 ) {
            _RWD_ModuleInit = rw->base_st.prev;
        }
#ifdef __MT__
        _RWD_StaticInitSema.v();
#endif
        if( rw == 0) break;
        RO_STATE* state = &rw->base_st.ro->init_ls.state_table[0];
        (*state->dtor)( state->u.data_addr, DTOR_NULL );
    }
}


#if 0


// *************************************************************************
// Support for state table and destruction of file-scope objects
// *************************************************************************


static RO_STATE* fs_state_table( // POINT AT STATE TABLE
    RO_DTREG* ro )              // - R/O entry
{
    return ro->init_fs.state_table;
}


static void* fs_point_object(   // POINT AT OBJECT FOR ENTRY
    RW_DTREG* rw,               // - R/W entry
    RT_STATE_VAR state_var )    // - state variable
{
    return (void*)( rw->base.ro
                      ->init_fs.state_table[ state_var - 1 ].u.data_addr );
}


static void fs_init_traverse(   // INITIALIZE TRAVERSAL DATA STRUCTURE
    STAB_TRAVERSE* traverse,    // - traversal information
    RW_DTREG* )                 // - read/write data
{
    traverse->obj_type = OBT_OBJECT;
}


static DTREG_VFTBL fs_vftable =// virtual functions for DTRG_FUN
{   &fs_state_table
,   &fs_point_object
,   &fs_init_traverse
};



// *************************************************************************
// Registration and Initialization
// *************************************************************************

static void init(               // PROGRAM INITIALIZATION
    void )
{
    CPPLIBDATA( dtreg_vfptrs )[ DTRG_STATIC_INITFS ] = &fs_vftable;
}

XI( CPPLIBDATA( stab_init_mod ), init, INIT_PRIORITY_RUNTIME )

#endif

extern "C"
void CPPLIB( mod_register )(    // REGISTRATION FOR MODULE INITIALIZED OBJECTS
    RW_DTREG* rw )              // - R/W block
{
#ifdef __MT__
    _RWD_StaticInitSema.p();
#endif
    rw->base_st.prev = _RWD_ModuleInit;
    _RWD_ModuleInit = rw;
#ifdef __MT__
    _RWD_StaticInitSema.v();
#endif
}
