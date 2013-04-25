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


static RO_STATE* ls_state_table( // POINT AT STATE TABLE
    RO_DTREG* ro )              // - R/O entry
{
    return ro->init_ls.state_table;
}


static void* ls_point_object(   // POINT AT OBJECT FOR ENTRY
    RW_DTREG* rw,               // - R/W entry
    RT_STATE_VAR )              // - state variable
{
    return rw->init_ls.object;
}


static void ls_init_traverse(   // INITIALIZE TRAVERSAL DATA STRUCTURE
    STAB_TRAVERSE* traverse,    // - traversal information
    RW_DTREG* )                 // - read/write data
{
    traverse->obj_type = OBT_OBJECT;
}


static DTREG_VFTBL ls_vftable =// virtual functions for DTRG_STATIC_INITLS
{   &ls_state_table
,   &ls_point_object
,   &ls_init_traverse
};


static void init(               // PROGRAM INITIALIZATION
    void )
{
    CPPLIBDATA( dtreg_vfptrs )[ DTRG_STATIC_INITLS ] = &ls_vftable;
}

XI( CPPLIBDATA( stab_init_lcl ), init, INIT_PRIORITY_RUNTIME )

#endif

extern "C"
void CPPLIB( lcl_register )(    // REGISTRATION OF LOCAL INITIALIZATION
    RW_DTREG RT_FAR *rw )       // - read/write block
{
    CPPLIB( mod_register )( rw );
}
