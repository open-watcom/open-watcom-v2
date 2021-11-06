/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include "dbgmain.h"
#include "dbgparse.h"
#include "dbgdot.h"
#include "dbgexins.h"
#include "dbgexdat.h"


extern void     WndMemInspect( address, char *, unsigned, mad_type_handle );
extern void     WndIOInspect( address *, mad_type_handle );
extern void     WndAddrInspect( address );


/*
 * IOExam -- perform the examine port command
 */

static void IOExam( mad_type_handle mth )
{
    address     addr;

    OptMemAddr( EXPR_GIVEN, &addr );
    ReqEOC();
    WndIOInspect( &addr, mth );
}

/*
 * MemExam -- perform the examine command
 */

static void MemExam( mad_type_handle mth )
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
    WndMemInspect( addr, expr, len, mth );
}

static void TypeExam( void )
{
    mad_type_handle     mth;
    mad_type_kind       tk;

    mth = ScanType( MAS_ALL | MTK_ALL, &tk );
    if( mth == MAD_NIL_TYPE_HANDLE ) {
        Error( ERR_LOC, LIT_ENG( ERR_BAD_OPTION ), GetCmdName( CMD_EXAMINE ) );
    }
    if( tk & MAS_IO ) {
        IOExam( mth );
    } else {
        MemExam( mth );
    }
}

#define FMT_DEFS \
    pick( "Assembly", AsmExam ) \
    pick( "Source",   SrcExam )

static const char FmtNameTab[] = {
    #define pick(t,p)   t "\0"
    FMT_DEFS
    #undef pick
};

static void (* const ExamJmpTab[])( void ) = {
    #define pick(t,p)   p,
    FMT_DEFS
    #undef pick
};


/*
 * ProcExamine -- process examine command
 */

void ProcExamine( void )
{
    int     cmd;

    if( CurrToken != T_DIV ) {
        MemExam( GetMADTypeHandleDefaultAt( NilAddr, MTK_BASIC ) );
    } else {
        Scan();
        cmd = ScanCmd( FmtNameTab );
        if( cmd <  0 ) {
            TypeExam();
        } else {
            (*ExamJmpTab[cmd])();
        }
    }
}
