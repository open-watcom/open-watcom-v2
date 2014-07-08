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


#include <string.h>
#include "dbgdefn.h"
#include "dbgtoggl.h"
#include "dbgreg.h"
#include "dbginfo.h"
#include "dbgerr.h"
#include "dbgitem.h"
#include "dbgstk.h"
#include "dbglit.h"
#include "spawn.h"
#include "mad.h"


extern char             *ReScan( char * );
extern unsigned long    ReqLongExpr( void );
extern void             ReqMemAddr( memory_expr, address * );
extern void             ReqEOC( void );
extern void             NormalExpr( void );
extern void             ChkExpr( void );
extern void             FreezeStack( void );
extern void             UnFreezeStack( bool );
extern char             *ScanPos( void );
extern unsigned int     ScanLen( void );
extern mod_handle       LookupModName( mod_handle, char *, unsigned );
extern void             Scan( void );
extern void             RawScanInit( void );
extern bool             ScanItem( bool, char **, size_t * );


OVL_EXTERN void GetExprAny( void *value )
{
    value=value;
    NormalExpr();
    ReqEOC();
}

OVL_EXTERN void GetExprLong( void *value )
{
    *(long *)value = ReqLongExpr();
    ReqEOC();
}

OVL_EXTERN void GetExprSyntax( void *value )
{
    value = value;
    ChkExpr();
    ReqEOC();
}

OVL_EXTERN void GetExprCodeAddr( void *value )
{
    ReqMemAddr( EXPR_CODE, value );
    ReqEOC();
}

OVL_EXTERN void GetExprDataAddr( void *value )
{
    ReqMemAddr( EXPR_DATA, value );
    ReqEOC();
}

OVL_EXTERN void GetExprGivenAddr( void *value )
{
    ReqMemAddr( EXPR_GIVEN, value );
    ReqEOC();
}

OVL_EXTERN void GetModName( void *value )
{
    mod_handle  mod;
    char        *start;
    size_t      len;

    RawScanInit();
    ScanItem( FALSE, &start, &len );
    mod = LookupModName( NO_MOD, start, len );
    if( mod == NO_MOD ) {
        Error( ERR_NONE, LIT( ERR_NO_MODULE ), start, len );
    }
    Scan();
    ReqEOC();
    *(mod_handle *)value = mod;
}

static bool DlgDoScan( char *str, void *value, void (*rtn)(void*) )
{
    char        *old;
    bool        ok;

    old = ReScan( str );
    _SwitchOn( SW_ERR_IN_TXTBUFF );
    FreezeStack();
    ok = ( SpawnP( rtn, value ) == 0 );
    UnFreezeStack( FALSE );
    _SwitchOff( SW_ERR_IN_TXTBUFF );
    ReScan( old );
    return( ok );
}

bool DlgScanLong( char *str, long *value )
{
    bool        ok;

    ok = DlgDoScan( str, value, GetExprLong );
    return( ok );
}

bool DlgScanSyntax( char *str )
{
    bool        ok;
    char        dummy;

    ok = DlgDoScan( str, &dummy, GetExprSyntax );
    return( ok );
}

bool DlgScanCodeAddr( char *str, address *value )
{
    return( DlgDoScan( str, value, GetExprCodeAddr ) );
}

bool DlgScanDataAddr( char *str, address *value )
{
    return( DlgDoScan( str, value, GetExprDataAddr ) );
}

bool DlgScanGivenAddr( char *str, address *value )
{
    return( DlgDoScan( str, value, GetExprGivenAddr ) );
}

bool DlgScanAny( char *str, void *value )
{
    return( DlgDoScan( str, value, GetExprAny ) );
}

bool DlgScanString( char *str, void *value )
{
    strcpy( value, str );
    return( TRUE );
}

bool DlgScanModName( char *str, void *value )
{
    return( DlgDoScan( str, value, GetModName ) );
}
