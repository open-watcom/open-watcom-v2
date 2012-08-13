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


#error obsolete
#include "cpplib.h"
#include "rtexcept.h"
#include "rtinit.h"


static RO_STATE* arr_state_table( // POINT AT STATE TABLE
    RO_DTREG* ro )              // - R/O entry
{
    return ro->array.state_table;
}


static void* arr_point_object(  // POINT AT OBJECT FOR ENTRY
    RW_DTREG* rw,               // - R/W entry
    RT_STATE_VAR state_var )    // - state variable
{
    size_t size = CPPLIB( ts_size )( rw->base.ro->array.sig );
    return (void*)( (char*)rw->array.array + ( state_var - 1 ) * size );
}


static void arr_init_traverse(  // INITIALIZE TRAVERSAL DATA STRUCTURE
    STAB_TRAVERSE* traverse,    // - traversal information
    RW_DTREG*  )                // - read/write data
{
    traverse->obj_type = OBT_OBJECT;
}

static DTREG_VFTBL arr_vftable =// virtual functions for DTRG_ARRAY
{   &arr_state_table
,   &arr_point_object
,   &arr_init_traverse
};


static void init(               // PROGRAM INITIALIZATION
    void )
{
    CPPDATA( dtreg_vfptrs )[ DTRG_ARRAY ] = &arr_vftable;
}

XI( CPPDATA( stab_init_arr ), init, INIT_PRIORITY_RUNTIME )


extern "C"
THREAD_CTL* CPPLIB( arr_register )( // REGISTER ARRAY BLOCKS
    void* array,                // - array pointer
    INT_DTREG_ARRAY* icb,       // - R/O block
    RT_TYPE_SIG sig )           // - type signature of element
{
    icb->ro.base.reg_type = DTRG_ARRAY;
    icb->ro.sig = sig;
    icb->ro.state_table[0].dtor = sig->clss.dtor;
    icb->ro.state_table[0].u.data_offset = 0;
#ifndef NDEBUG
    icb->ro.state_table[1].dtor = 0;
    icb->ro.state_table[1].u.data_offset = 0;
#endif
    icb->rw.array = array;
    icb->rw.base.ro = (RO_DTREG*)&icb->ro;
    return CPPLIB( base_register )( (RW_DTREG*)&icb->rw
                                  , (RO_DTREG*)&icb->ro
                                  , 0 );
}
