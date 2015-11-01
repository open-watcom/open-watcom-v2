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


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgerr.h"
#include "dbgitem.h"
#include "dbgstk.h"
#include "mad.h"
#include "dbgscan.h"
#include "dbgutil.h"
#include "dbgexpr3.h"
#include "dbgexpr.h"
#include "dbgparse.h"
#include "dlgscan.h"
#include "dlgexpr.h"
#include "dlgnewws.h"


typedef bool rtn_func(const char *,void *);
typedef bool dlg_func(const char *,char *,unsigned);

/* to be moved to header files ! */
extern char             *StrDouble(xreal*,char*);

extern stack_entry      *ExprSP;

#define EXPR_LEN        128


static bool DoDlgGet( gui_window *gui, gui_ctl_id id, void *value, rtn_func *rtn )
{
    char        *str;
    bool        ok;

    str = GUIGetText( gui, id );
    if( str == NULL ) {
        str = LIT_ENG( Empty );
        ok = rtn( LIT_ENG( Empty ), value );
    } else {
        ok = rtn( str, value );
        GUIMemFree( str );
    }
    return( ok );
}


bool DlgGetLong( gui_window *gui, gui_ctl_id id, long *value )
{
    return( DoDlgGet( gui, id, value, (rtn_func *)DlgScanLong ) );
}

bool DlgGetCodeAddr( gui_window *gui, gui_ctl_id id, address *value )
{
    return( DoDlgGet( gui, id, value, (rtn_func *)DlgScanCodeAddr ) );
}

bool DlgGetDataAddr( gui_window *gui, gui_ctl_id id, address *value )
{
    return( DoDlgGet( gui, id, value, (rtn_func *)DlgScanDataAddr ) );
}

void DlgSetLong( gui_window *gui, gui_ctl_id id, long value )
{
    CnvLong( value, TxtBuff, TXT_LEN );
    GUISetText( gui, id, TxtBuff );
}

static bool     DlgGetItemWithRtn( char *buff, unsigned buff_len, const char *title,
                                   void *value, rtn_func *rtn, dlg_func *dlg )
{
    bool        rc;

    for( ;; ) {
        dlg( title, buff, buff_len );
        if( buff[0] == '\0' )
            return( FALSE );
        rc = rtn( buff, value );
        if( rc ) return( TRUE );
        PrevError( TxtBuff );
    }
}

static bool     DlgGetItem( char *buff, unsigned buff_len, const char *title, void *value, rtn_func *rtn )
{
    return( DlgGetItemWithRtn( buff, buff_len, title, value, rtn, DlgNewWithSym ) );
}

bool    DlgLongExpr( const char *title, long *value )
{
    char        new[EXPR_LEN];

    CnvLong( *value, new, sizeof( new ) );
    return( DlgGetItem( new, EXPR_LEN, title, value, (rtn_func *)DlgScanLong ) );
}

bool    DlgAnyExpr( const char *title, char *buff, unsigned buff_len )
{
    return( DlgGetItem( buff, buff_len, title, NULL, (rtn_func *)DlgScanAny ) );
}


static void InitAddr( char *new, address *value, unsigned max )
{
    if( IS_NIL_ADDR( (*value) ) ) {
        new[0] = '\0';
    } else {
        UniqStrAddr( value, new, max );
    }
}

bool    DlgCodeAddr( const char *title, address *value )
{
    char        new[EXPR_LEN];

    InitAddr( new, value, sizeof( new ) );
    return( DlgGetItem( new, EXPR_LEN, title, value, (rtn_func *)DlgScanCodeAddr ) );
}

bool    DlgDataAddr( const char *title, address *value )
{
    char        new[EXPR_LEN];

    InitAddr( new, value, sizeof( new ) );
    return( DlgGetItem( new, EXPR_LEN, title, value, (rtn_func *)DlgScanDataAddr ) );
}

bool    DlgGivenAddr( const char *title, address *value )
{
    char        new[EXPR_LEN];

    new[0] = '\0';
    return( DlgGetItem( new, EXPR_LEN, title, value, (rtn_func *)DlgScanGivenAddr ) );
}

bool    DlgModName( const char *title, mod_handle *mod )
{
    char        new[EXPR_LEN];

    if( *mod == NO_MOD ) {
        new[0] = '\0';
    } else {
        ModName( *mod, new, EXPR_LEN );
    }
    return( DlgGetItemWithRtn( new, EXPR_LEN, title, mod, DlgScanModName, DlgNewWithMod ) );
}

bool DlgString( const char *title, char *buff )
{
    char        new[EXPR_LEN];

    new[0] = '\0';
    return( DlgGetItemWithRtn( new, EXPR_LEN, title, buff, DlgScanString, DlgNew ) );
}

bool DlgMadTypeExpr( const char *title, item_mach *value, mad_type_handle th )
{
    bool                ok;
    mad_type_info       mti;
    char                buff[EXPR_LEN];
    unsigned            buff_len = sizeof( buff );

    MADTypeInfo( th, &mti );
    MADTypeToString( CurrRadix, &mti, value, buff, &buff_len );
    ok = DlgAnyExpr( title, buff, sizeof( buff ) );
    if( !ok ) return( FALSE );
    ToItemMAD( ExprSP, value, &mti );
    PopEntry();
    return( TRUE );
}
