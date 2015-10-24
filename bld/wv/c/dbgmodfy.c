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
* Description:  Modify program memory or I/O port.
*
****************************************************************************/


#include <stdlib.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgstk.h"
#include "dbglit.h"
#include "dbgerr.h"
#include "dbgitem.h"
#include "ldsupp.h"
#include "dui.h"
#include "madinter.h"
#include "dbgmad.h"
#include "dbgscan.h"
#include "dbgmemor.h"
#include "dbgexpr4.h"
#include "dbgexpr3.h"
#include "dbgexpr.h"
#include "dbgmain.h"
#include "dbgparse.h"
#include "dbgdot.h"
#include "dbgmodfy.h"


extern void             DbgUpdate( update_list );
extern void             RecordCommand( const char *startpos, wd_cmd cmd );
extern bool             AdvMachState( int );

extern stack_entry      *ExprSP;

/*
 *  MemMod -- modify memory or io port
 */

static void MemMod( mad_type_handle th, mad_type_kind mas )
{
    item_mach           item;
    item_type           ops;
    address             addr;
    mad_type_info       mti;

    ops = IT_ERR | IT_INC;
    if( mas & MAS_IO ) {
        ops |= IT_IO;
        addr.mach.offset = ReqExpr();
    } else {
        addr = GetDataDot();
        OptMemAddr( EXPR_DATA, &addr );
    }
    if( CurrToken == T_COMMA ) Scan();
    MADTypeInfo( th, &mti );
    while( !ScanEOC() ) {
        if( !( ops & IT_IO ) ) {
            SetDataDot( addr );
        }
        if( CurrToken != T_COMMA ) {
            NormalExpr();
            ToItemMAD( ExprSP, &item, &mti );
            PopEntry();
            if( CurrToken != T_COMMA && !ScanEOC() ) {
                Error( ERR_LOC, LIT_ENG( ERR_WANT_EOC ) );
            }
            ItemPutMAD( &addr, &item, ops, th );
        }
        if( CurrToken == T_COMMA ) Scan();
    }
    if( !(ops & IT_IO) ) {
        DbgUpdate( UP_MEM_CHANGE | UP_CODE_ADDR_CHANGE | UP_REG_CHANGE );
    }
}

/*
 * ProcModify -- process modify command
 */

void ProcModify( void )
{
    const char          *startpos;
    mad_type_handle     th;
    mad_type_kind       tk;

    if( !AdvMachState( ACTION_MODIFY_MEMORY ) ) {
        FlushEOC();
        return;
    }
    startpos = ScanPos();
    if( CurrToken != T_DIV ) {
        MemMod( GetMADTypeHandleDefaultAt( NilAddr, MTK_BASIC ), MAS_MEMORY );
    } else {
        Scan();
        th = ScanType( MAS_ALL | MTK_ALL, &tk );
        if( th == MAD_NIL_TYPE_HANDLE ) {
            Error( ERR_LOC, LIT_ENG( ERR_BAD_OPTION ), GetCmdName( CMD_MODIFY ) );
        }
        MemMod( th, tk );
    }
    RecordCommand( startpos, CMD_MODIFY );
}
