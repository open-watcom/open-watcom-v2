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
* Description:  Examine program data.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgerr.h"
#include "dbggptr.h"
#include "dbgitem.h"
#include "dbglit.h"
#include "strutil.h"
#include "dbgmad.h"
#include "dbgscan.h"
#include "madinter.h"


extern void             AsmExam( void );
extern void             OptMemAddr( memory_expr, address * );
extern unsigned int     OptExpr( unsigned int );
extern void             ChkExpr( void );
extern void             SrcExam( void );
extern void             WndMemInspect( address, char *, unsigned, mad_type_handle );
extern void             WndIOInspect( address *, mad_type_handle );
extern void             WndAddrInspect( address );
extern void             SetDataDot( address );
extern address          GetDataDot( void );
extern char             *GetCmdName( int );


/*
 * IOExam -- perform the examine port command
 */

static void IOExam( mad_type_handle type )
{
    address     addr;

    OptMemAddr( EXPR_GIVEN, &addr );
    ReqEOC();
    WndIOInspect( &addr, type );
}

/*
 * MemExam -- perform the examine command
 */

static void MemExam( mad_type_handle type )
{
    address     addr;
    const char  *start;
    char        *expr;
    unsigned    len;

    addr = GetDataDot();
    OptMemAddr( EXPR_DATA, &addr );
    expr = NULL;
    if( CurrToken == T_COMMA ) {
        Scan();
        if( CurrToken != T_COMMA ) {
            start = ScanPos();
            ChkExpr();
            len = ScanPos() - start;
            expr = DupStrLen( start, len );
        }
    }
    len = 0;
    if( CurrToken == T_COMMA ) {
        Scan();
        len = OptExpr( 0 );
    }
    ReqEOC();
    SetDataDot( addr );
    WndMemInspect( addr, expr, len, type );
}

OVL_EXTERN void TypeExam( void )
{
    mad_type_handle     th;
    mad_type_kind       tk;

    th = ScanType( MAS_ALL | MTK_ALL, &tk );
    if( th == MAD_NIL_TYPE_HANDLE ) {
        Error( ERR_LOC, LIT( ERR_BAD_OPTION ), GetCmdName( CMD_EXAMINE ) );
    }
    if( tk & MAS_IO ) {
        IOExam( th );
    } else {
        MemExam( th );
    }
}

static const char FmtNameTab[] = {
    "Assembly\0"
    "Source\0"
};

static void (* const ExamJmpTab[])( void ) = {
    &TypeExam,
    &AsmExam,
    &SrcExam,
};


/*
 * ProcExamine -- process examine command
 */

void ProcExamine( void )
{
    if( CurrToken != T_DIV ) {
        MemExam( GetMADTypeHandleDefaultAt( NilAddr, MTK_BASIC ) );
    } else {
        Scan();
        (*ExamJmpTab[ ScanCmd( FmtNameTab ) ])();
    }
}
