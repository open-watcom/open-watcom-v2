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


#include <string.h>
#include <ctype.h>
#include "dbgdefn.h"
#include "dbgwind.h"
#include "dbgtoggl.h"
#include "dbgreg.h"
#include "dbginfo.h"
#include "dbgerr.h"
#include "dbgitem.h"
#include "dbgstk.h"
#include "mad.h"

/* to be moved to header files ! */
extern bool             DlgNewWithMod(char*,char*,int);
extern bool             DlgNewWithSym(char*,char*,int);
extern char             *ReScan(char*);
extern void             ChkExpr(void);
extern void             ReqMemAddr(memory_expr , address *);
extern void             ReqEOC();
extern char             *CnvLong(long,char*);
extern void             NormalExpr(void);
extern void             FreezeStack();
extern void             UnFreezeStack( bool );
extern char             *ScanPos(void);
extern void             PrevError(char*);
extern char             *UniqStrAddr(address *,char * ,unsigned);
extern char             *StrDouble(xreal*,char*);
extern char             *DupStrLen(char*,unsigned);
extern char             *ScanPos(void);
extern unsigned int     ScanLen(void);
extern mod_handle       LookupModName( mod_handle, char *, unsigned );
extern void             Scan(void);
extern void             ToItemMAD( stack_entry *entry, item_mach *tmp, mad_type_info *mti );
extern void             PopEntry(void);

extern char             *TxtBuff;
extern unsigned char    CurrRadix;
extern stack_entry      *ExprSP;

#define EXPR_LEN        128

extern bool DlgScanModName( char *str, void *value );
extern bool DlgScanLong( char *str, void *value );
extern bool DlgScanCodeAddr( char *str, void *value );
extern bool DlgScanDataAddr( char *str, void *value );
extern bool DlgScanGivenAddr( char *str, void *value );
extern bool DlgScanAny( char *str, void *value );
extern bool DlgScanString( char *str, void *value );

static bool DoDlgGet( gui_window *gui, unsigned id, void *value, bool (*rtn)(char*,void*) )
{
    char        *str;
    bool        ok;

    str = GUIGetText( gui, id );
    if( str == NULL ) {
        str = LIT( Empty );
        ok = rtn( LIT( Empty ), value );
    } else {
        ok = rtn( str, value );
        GUIMemFree( str );
    }
    return( ok );
}


extern bool DlgGetLong( gui_window *gui, unsigned id, long *value )
{
    return( DoDlgGet( gui, id, value, DlgScanLong ) );
}

extern bool DlgGetCodeAddr( gui_window *gui, unsigned id, address *value )
{
    return( DoDlgGet( gui, id, value, DlgScanCodeAddr ) );
}

extern bool DlgGetDataAddr( gui_window *gui, unsigned id, address *value )
{
    return( DoDlgGet( gui, id, value, DlgScanDataAddr ) );
}

extern void DlgSetLong( gui_window *gui, unsigned id, long value )
{
    CnvLong( value, TxtBuff );
    GUISetText( gui, id, TxtBuff );
}

static bool     DlgGetItemWithRtn( char *new, unsigned max_len, char *title,
                                   void *value, bool (*rtn)(char*,void*),
                                   bool (*dlg)(char*,char*,int) )
{
    bool        rc;

    for( ;; ) {
        dlg( title, new, max_len );
        if( new[0] == '\0' ) return( FALSE );
        rc = rtn( new, value );
        if( rc ) return( TRUE );
        PrevError( TxtBuff );
    }
}

static bool     DlgGetItem( char *new, unsigned max_len, char *title, void *value, bool (*rtn)(char*,void*) )
{
    return( DlgGetItemWithRtn( new, max_len, title, value, rtn, DlgNewWithSym ) );
}

bool    DlgLongExpr( char *title, long *value )
{
    char        new[EXPR_LEN];

    CnvLong( *value, new );
    return( DlgGetItem( new, EXPR_LEN, title, value, DlgScanLong ) );
}

bool    DlgAnyExpr( char *title, char *value, unsigned max_len )
{
    return( DlgGetItem( value, max_len, title, NULL, DlgScanAny ) );
}


static void InitAddr( char *new, address *value, unsigned max )
{
    if( IS_NIL_ADDR( (*value) ) ) {
        new[0] = '\0';
    } else {
        UniqStrAddr( value, new, max );
    }
}

bool    DlgCodeAddr( char *title, address *value )
{
    char        new[EXPR_LEN];

    InitAddr( new, value, sizeof( new ) );
    return( DlgGetItem( new, EXPR_LEN, title, value, DlgScanCodeAddr ) );
}

bool    DlgDataAddr( char *title, address *value )
{
    char        new[EXPR_LEN];

    InitAddr( new, value, sizeof( new ) );
    return( DlgGetItem( new, EXPR_LEN, title, value, DlgScanDataAddr ) );
}

bool    DlgGivenAddr( char *title, address *value )
{
    char        new[EXPR_LEN];

    new[0] = '\0';
    return( DlgGetItem( new, EXPR_LEN, title, value, DlgScanGivenAddr ) );
}

bool    DlgModName( char *title, mod_handle *mod )
{
    char        new[EXPR_LEN];

    if( *mod == NO_MOD ) {
        new[0] = '\0';
    } else {
        ModName( *mod, new, EXPR_LEN );
    }
    return( DlgGetItemWithRtn( new, EXPR_LEN, title, mod, DlgScanModName, DlgNewWithMod));
}

bool DlgString( char *title, char *buff )
{
    buff[isprint(buff[0]) ? 1 : 0] = '\0';
    return( DlgGetItemWithRtn( buff, EXPR_LEN, title, buff, DlgScanString, DlgNew));
}

bool DlgMadTypeExpr( char *title, item_mach *value, mad_type_handle th )
{
    bool                ok;
    mad_type_info       mti;
    char                new[EXPR_LEN];
    unsigned            max = EXPR_LEN;

    MADTypeInfo( th, &mti );
    MADTypeToString( CurrRadix, &mti, value, &max, new );
    ok = DlgAnyExpr( title, new, EXPR_LEN );
    if( !ok ) return( FALSE );
    ToItemMAD( ExprSP, value, &mti );
    PopEntry();
    return( TRUE );
}
