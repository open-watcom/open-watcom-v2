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
#include "dbgtoken.h"
#include "dbginfo.h"
#include "dbgstk.h"
#include "dbglit.h"
#include "dbgerr.h"
#include "dbgtoggl.h"
#include "dbgitem.h"
#include "ldsupp.h"
#include "dui.h"
#include "mad.h"


extern void             OptMemAddr( memory_expr, address *);
extern bool             ScanEOC( void );
extern char             *ReScan( char * );
extern void             ReqEOC( void );
extern void             Scan( void );
extern unsigned int     ScanCmd( char * );
extern mad_type_handle  ScanType( mad_type_kind, mad_type_kind * );
extern void             PopEntry( void );
extern void             DbgUpdate( update_list );
extern void             NormalExpr( void );
extern void             ConvertTo( stack_entry *, type_kind, type_modifier, unsigned );
extern unsigned         ReqExpr( void );
extern void             SetDataDot( address addr );
extern address          GetDataDot( void );
extern char             *GetCmdName( int );
extern void             FlushEOC( void );
extern void             RecordCommand( char *startpos, int cmd );
extern bool             AdvMachState( int );
extern char             *ScanPos( void );
extern mad_type_handle  GetMADTypeHandleDefaultAt( address a, mad_type_kind mtk );
extern void             ToItemMAD( stack_entry *entry, item_mach *tmp, mad_type_info * );

extern address          NilAddr;
extern tokens           CurrToken;
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
                Error( ERR_LOC, LIT( ERR_WANT_EOC ) );
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
    char                *startpos;
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
            Error( ERR_LOC, LIT( ERR_BAD_OPTION ), GetCmdName( CMD_MODIFY ) );
        }
        MemMod( th, tk );
    }
    RecordCommand( startpos, CMD_MODIFY );
}
