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


#include "plusplus.h"

#include <float.h>

#include "cgfront.h"
#include "cgback.h"
#include "memmgr.h"
#include "codegen.h"
#include "cgbackut.h"
#include "ring.h"
#include "initdefs.h"

#ifndef NDEBUG
    #include <stdio.h>
    #include "dbg.h"
    #include "toggle.h"
#endif


static void padOffsetToPtrSize( // ADD PADDING TO EXTEND OFFSET TO PTR SIZE
    void )
{
    target_size_t padding;      // - # bytes to pad

    DbgAssert( CgbkInfo.size_data_ptr >= CgbkInfo.size_offset );
    padding =  CgbkInfo.size_data_ptr - CgbkInfo.size_offset;
    if( padding > 0 ) {
        DGIBytes( padding, 0 );
    }
}


static SYMBOL cgStateTableCmd(  // GENERATE A STATE-TABLE ENTRY FOR A CMD
    SYMBOL sym,                 // - symbol referenced or NULL
    unsigned offset )           // - offset from symbol
{
    DgPtrSymCode( NULL );
    if( sym == NULL ) {
        DgPtrSymData( sym );
    } else {
        DgPtrSymOff( sym, offset );
    }
    return sym;
}


bool StabGenerate(              // GENERATE A STATE TABLE
    STAB_CTL* sctl )            // - state-table information
{
    STAB_DEFN* defn;            // - state-table definition
    SE* se;                     // - current state entry
    SE* state_table;            // - the state table

    if( sctl->rw == NULL ) return FALSE;
    defn = sctl->defn;
    CgBackGenLabelInternal( defn->ro );
    DgOffset( defn->kind );
#if _CPU == _AXP
    if( defn->kind == DTRG_FUN ) {
        DgOffset( - CgOffsetRw( 0 ) );
    }
#endif
    state_table = defn->state_table;
    RingIterBeg( state_table, se ) {
        if( se->base.gen ) {
            switch( se->base.se_type ) {
              case DTC_SYM_AUTO :
                DbgVerify( UNDEF_AREL != se->sym_auto.offset, "cgStateTable -- no offset for SYM_AUTO" );
                DgPtrSymCode( se->sym_auto.dtor );
                DgOffset( CgOffsetRw( se->sym_auto.offset ) );
                padOffsetToPtrSize();
                break;
              case DTC_SYM_STATIC :
                DgPtrSymCode( se->sym_static.dtor );
                DgPtrSymData( se->sym_static.sym );
                break;
              case DTC_TEST_FLAG :
                cgStateTableCmd( CgCmdTestFlag( se ), 0 );
                break;
              case DTC_TRY :
                se->try_blk.sym = cgStateTableCmd( CgCmdTry( se ), 0 );
                break;
              case DTC_CATCH :
                cgStateTableCmd( se->catch_blk.try_blk->try_blk.sym, sizeof( DTOR_CMD_CODE ) );
                break;
              case DTC_FN_EXC :
                cgStateTableCmd( CgCmdFnExc( se ), 0 );
                break;
              case DTC_SET_SV :
                if( se != state_table ) {
                    cgStateTableCmd( CgCmdSetSv( se ), 0 );
                }
                break;
              case DTC_ACTUAL_DBASE :
              case DTC_ACTUAL_VBASE :
              case DTC_COMP_VBASE :
              case DTC_COMP_DBASE :
              case DTC_COMP_MEMB :
                cgStateTableCmd( CgCmdComponent( se ), 0 );
                break;
              case DTC_ARRAY_INIT :
                cgStateTableCmd( CgCmdArrayInit( se ), 0 );
                break;
              case DTC_DLT_1 :
                cgStateTableCmd( CgCmdDlt1( se ), 0 );
                break;
              case DTC_DLT_2 :
                cgStateTableCmd( CgCmdDlt2( se ), 0 );
                break;
              case DTC_DLT_1_ARRAY :
                cgStateTableCmd( CgCmdDlt1Array( se ), 0 );
                break;
              case DTC_DLT_2_ARRAY :
                cgStateTableCmd( CgCmdDlt2Array( se ), 0 );
                break;
              case DTC_CTOR_TEST :
                cgStateTableCmd( CgCmdCtorTest( se ), 0 );
                break;
              DbgDefault( "cgStateTable -- impossible" );
            }
        }
    } RingIterEnd( se )
#if !defined( NDEBUG ) || !defined( _INTEL_CPU )
    DgPtrSymCode( NULL );
    DgPtrSymData( NULL );
#endif
    StabDefnFreeStateTable( defn );
    return TRUE;
}
