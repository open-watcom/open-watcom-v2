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
* Description:  Worker routines for various input dialogs.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgerr.h"
#include "dbgitem.h"
#include "dbgstk.h"
#include "dbglit.h"
#include "wspawn.h"
#include "mad.h"
#include "dbgscan.h"
#include "dbgexpr.h"
#include "dbgparse.h"
#include "dlgscan.h"
#include "dbglkup.h"


extern void             RawScanInit( void );


static void GetExprAny( void *value )
{
    /* unused parameters */ (void)value;

    NormalExpr();
    ReqEOC();
}

static void GetExprLong( void *value )
{
    *(long *)value = ReqLongExpr();
    ReqEOC();
}

static void GetExprSyntax( void *value )
{
    /* unused parameters */ (void)value;

    ChkExpr();
    ReqEOC();
}

static void GetExprCodeAddr( void *value )
{
    ReqMemAddr( EXPR_CODE, value );
    ReqEOC();
}

static void GetExprDataAddr( void *value )
{
    ReqMemAddr( EXPR_DATA, value );
    ReqEOC();
}

static void GetExprGivenAddr( void *value )
{
    ReqMemAddr( EXPR_GIVEN, value );
    ReqEOC();
}

static void GetModName( void *value )
{
    mod_handle  mod;
    const char  *start;
    size_t      len;

    RawScanInit();
    ScanItem( false, &start, &len );
    mod = LookupModName( NO_MOD, start, len );
    if( mod == NO_MOD ) {
        Error( ERR_NONE, LIT_ENG( ERR_NO_MODULE ), start, len );
    }
    Scan();
    ReqEOC();
    *(mod_handle *)value = mod;
}

static bool DlgDoScan( const char *str, void *value, wspawn_funcP *rtn )
{
    const char  *old;
    bool        ok;

    old = ReScan( str );
    _SwitchOn( SW_ERR_IN_TXTBUFF );
    FreezeStack();
    ok = ( SpawnP( rtn, value ) == 0 );
    UnFreezeStack( false );
    _SwitchOff( SW_ERR_IN_TXTBUFF );
    ReScan( old );
    return( ok );
}

bool DlgScanLong( const char *str, long *value )
{
    bool        ok;

    ok = DlgDoScan( str, value, GetExprLong );
    return( ok );
}

bool DlgScanSyntax( const char *str )
{
    bool        ok;
    char        dummy;

    ok = DlgDoScan( str, &dummy, GetExprSyntax );
    return( ok );
}

bool DlgScanCodeAddr( const char *str, address *value )
{
    return( DlgDoScan( str, value, GetExprCodeAddr ) );
}

bool DlgScanDataAddr( const char *str, address *value )
{
    return( DlgDoScan( str, value, GetExprDataAddr ) );
}

bool DlgScanGivenAddr( const char *str, address *value )
{
    return( DlgDoScan( str, value, GetExprGivenAddr ) );
}

bool DlgScanAny( const char *str, void *value )
{
    return( DlgDoScan( str, value, GetExprAny ) );
}

bool DlgScanString( const char *str, void *value )
{
    strcpy( value, str );
    return( true );
}

bool DlgScanModName( const char *str, void *value )
{
    return( DlgDoScan( str, value, GetModName ) );
}
