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
* Description:  Statement number list
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "fmemmgr.h"
#include "rststmt.h"
#include "cgmagic.h"



unsigned_32     GetStmtNum( sym_id sn ) {
//=======================================

// Get statement number from statement number field.

    unsigned_32 st_number;

    st_number = sn->u.st.number;
    if( sn->u.st.flags & SN_ADD_65535 ) {
        st_number += 65535;
    }
    return( st_number );
}


sym_id  STStmtNo( unsigned_32 stmnt_no ) {
//========================================

// Search the symbol table for a statement number. If it is not
// already in the symbol table, add it to the symbol table.

    sym_id      ste_ptr;
    unsigned_32 st_number;

    ste_ptr = SList;
    while( ste_ptr != NULL ) {
        st_number = GetStmtNum( ste_ptr );
        if( st_number == stmnt_no ) {
            return( ste_ptr );
        }
        ste_ptr = ste_ptr->u.st.link;
    }
    ste_ptr = FMemAlloc( sizeof( stmtno ) );
    ste_ptr->u.st.block = CSHead->block;
    ste_ptr->u.st.flags = SN_INIT;
    if( stmnt_no > 65535 ) {
        stmnt_no -= 65535;
        ste_ptr->u.st.flags |= SN_ADD_65535;
    }
    ste_ptr->u.st.number = stmnt_no;
    ste_ptr->u.st.address = NextLabel();
    ste_ptr->u.st.ref_count = 0;
    ste_ptr->u.st.link = SList;
    SList = ste_ptr;
    return( ste_ptr );
}
