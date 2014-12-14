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
* Description:  Breakpoint and watchpoint management.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgerr.h"
#include "dbgitem.h"
#include "dbglit.h"
#include "dbgmem.h"
#include "namelist.h"
#include "dbgstk.h"
#include "dbgrep.h"
#include "dbgio.h"
#include "mad.h"
#include "dui.h"
#include "spawn.h"
#include "enterdb.h"


extern cue_fileid       CueFileId( cue_handle * );
extern unsigned         CueFile( cue_handle *ch, char *file, unsigned max );
extern unsigned long    CueLine( cue_handle *ch );
extern unsigned long    ReqLongExpr( void );
extern unsigned int     ReqExpr( void );
extern char             *ReScan( char * );
extern void             ReqMemAddr( memory_expr, address * );
extern bool             ScanQuote( char **, size_t * );
extern bool             ScanEOC( void );
extern int              AddrComp( address, address );
extern unsigned int     ScanCmd( char * );
extern void             ReqEOC( void );
extern void             Scan( void );
extern cmd_list         *AllocCmdList( char *, size_t );
extern void             FreeCmdList( cmd_list * );
extern void             PushCmdList( cmd_list * );
extern address          GetRegIP( void );
extern int              SectIsLoaded( unsigned int, int  );
extern dword            RemoteSetBreak( address );
extern void             RemoteRestoreBreak( address, dword );
OVL_EXTERN brkp         *TypePoint( memory_expr );
OVL_EXTERN brkp         *BadPoint( memory_expr );
OVL_EXTERN brkp         *ImageBreak( memory_expr );
OVL_EXTERN brkp         *ActivatePoint( memory_expr );
OVL_EXTERN brkp         *ClearPoint( memory_expr );
OVL_EXTERN brkp         *TogglePoint( memory_expr );
OVL_EXTERN brkp         *ResumePoint( memory_expr );
OVL_EXTERN brkp         *UnResumePoint( memory_expr );
OVL_EXTERN brkp         *DeactivatePoint( memory_expr );
OVL_EXTERN brkp         *SetBreak( memory_expr );
OVL_EXTERN brkp         *SetWatch( memory_expr );
OVL_EXTERN brkp         *SetPoint( memory_expr def_seg, mad_type_handle );
extern bool             RemoteSetWatch( address, unsigned, unsigned long * );
extern void             RemoteRestoreWatch( address, unsigned );
extern void             ErrorBox( char * );
extern bool             DlgBreak( address );
extern void             SetProgState( unsigned );
extern void             TypeInpStack( input_type );
extern char             *DupStr( char * );
extern char             *DupStrLen( char const *str, size_t len );
extern bool             CheckBPIns( void );
extern char             *GetCmdEntry( char *, int, char * );
extern char             *Format( char *buff, char *fmt, ... );
extern void             InvokeAFile( char * );
extern void             CreateInvokeFile( char *name, void ( *rtn ) ( void ) );
extern void             UnAsm( address addr, char *buff, unsigned buff_len );
extern char             *AddHexSpec( char * );
extern char             *GetCmdName( int );
extern bool             DlgAreYouNuts( unsigned long );
extern inspect_type     WndGetExprSPInspectType( address *paddr );
extern char             *StrCopy( char *, char * );
extern void             RecordEvent( char *p );
extern void             SetRecord( bool on );
extern unsigned         DefaultSize( default_kind );
extern void             ReadDbgRegs( void );
extern void             WriteDbgRegs( void );
extern bool             SetMsgText( char *, unsigned * );
extern unsigned         GetMADTypeNameForCmd( mad_type_handle th, char *buff, unsigned buff_len );
extern mad_type_handle  ScanType( mad_type_kind, mad_type_kind * );
extern mad_type_handle  FindMADTypeHandle( mad_type_kind tk, unsigned size );
extern char             *CnvULongHex( unsigned long value, char *buff, unsigned buff_len );
extern unsigned         NewCurrRadix( unsigned );
extern void             WriteDbgRegs( void );
extern void             BreakOnImageLoad( char *name, unsigned len, bool clear );
extern bool             ScanItem( bool blank_delim, char **start, size_t *len );
extern void             InitMappableAddr( mappable_addr *loc );
extern void             FiniMappableAddr( mappable_addr *loc );
extern void             UnMapPoints( image_entry * );
extern void             ReMapPoints( image_entry * );
extern char             *StrAddr( address *addr, char *buff, unsigned max );
extern image_entry      *ImageEntry( mod_handle mh );
extern char             *AddrToString( address *a, mad_address_format af, char *buff, unsigned buff_len );
extern bool             DlgScanCodeAddr( char *str, address *value );
extern void             DoInput( void );
extern char             *CnvNearestAddr( address, char *, unsigned );
extern void             DbgUpdate( update_list );
extern bool             DUIGetSourceLine( cue_handle *ch, char *buff, unsigned len );
extern void             DUIRemoveBreak( brkp *bp );
extern void             CheckForNewThreads( bool );
extern void             LValue( stack_entry * );


extern char_ring        *DLLList;
extern stack_entry      *ExprSP;

static char PointNameTab[] = {
    "Activate\0"
    "Clear\0"
    "Deactivate\0"
    "Set\0"
    "Modify\0"
    "Toggle\0"
    "Resume\0"
    "UNResume\0"
    "INdex\0"
    "IMage\0"
    "Unmapped\0"
    "MAPaddress\0"
    "SYMaddress\0"
};

typedef enum {
    B_ACTIVATE = 1,
    B_CLEAR,
    B_DEACTIVATE,
    B_SET,
    B_MODIFY,
    B_TOGGLE,
    B_RESUME,
    B_UNRESUME,
    B_INDEX,
    B_IMAGE,
    B_UNMAPPED,
    B_MAPADDRESS,
    B_SYMADDRESS,
} brk_event;

typedef struct {
    brkp *(* rtn)( memory_expr );
    memory_expr type;
} bpjmptab_type;

/*
 *  Queried from trap file supplemental services
 */
bool    Supports8ByteBreakpoints = FALSE;
bool    SupportsExactBreakpoints = FALSE;

static bpjmptab_type BPJmpTab[] = {
    { &TypePoint,       EXPR_DATA },
    { &ActivatePoint,   EXPR_CODE },
    { &ClearPoint,      EXPR_CODE },
    { &DeactivatePoint, EXPR_CODE },
    { &SetBreak,        EXPR_CODE },
    { &SetWatch,        EXPR_DATA },
    { &TogglePoint,     EXPR_CODE },
    { &ResumePoint,     EXPR_CODE },
    { &UnResumePoint,   EXPR_CODE },
    { &BadPoint,        EXPR_DATA },
    { &ImageBreak,      EXPR_DATA },
};


static char *BrkFmt( void )
{
    return( ( DbgLevel != ASM ) ? "%l" : "%a" );
}


/*
 * InitBPs -- initialize breakpoints
 */

void InitBPs( void )
{
    NullStatus( &UserTmpBrk );
    NullStatus( &DbgTmpBrk );
    BrkList = NULL;
}



/*
 * InsertOneBP -- insert one break point into memory
 */

static bool InsertOneBP( brkp *bp, bool force )
{
    bool    at_ip;

    at_ip = FALSE;
    if( bp->status.b.active ) {
        if( !force && AddrComp( bp->loc.addr, GetRegIP() ) == 0 ) {
            at_ip = TRUE;
        } else if( SectIsLoaded( bp->loc.addr.sect_id, OVL_MAP_EXE ) ) {
            bp->status.b.in_place = TRUE;
            bp->item.ud = RemoteSetBreak( bp->loc.addr );
        }
    }
    return( at_ip );
}



/*
 * InsertBPs -- put break points into memory
 */

bool InsertBPs( bool force )
{
    brkp    *bp;
    bool    at_ip;

    at_ip = FALSE;
    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        bp->status.b.cmds_pushed = FALSE;
        if( bp->th != MAD_NIL_TYPE_HANDLE ) continue;
        bp->status.b.in_place = FALSE;
        bp->status.b.hit = FALSE;
        if( ( UserTmpBrk.status.b.active )
         && ( AddrComp( UserTmpBrk.loc.addr, bp->loc.addr ) == 0 ) ) continue;
        if( ( DbgTmpBrk.status.b.active )
         && ( AddrComp( DbgTmpBrk.loc.addr, bp->loc.addr ) == 0 ) ) continue;
        at_ip |= InsertOneBP( bp, force );
    }
    UserTmpBrk.status.b.hit = FALSE;
    UserTmpBrk.status.b.in_place = FALSE;
    DbgTmpBrk.status.b.hit = FALSE;
    DbgTmpBrk.status.b.in_place = FALSE;
    if( UserTmpBrk.status.b.active ) {
        at_ip |= InsertOneBP( &UserTmpBrk, force );
    }
    if( DbgTmpBrk.status.b.active
     && ( !UserTmpBrk.status.b.active
       || ( AddrComp( UserTmpBrk.loc.addr, DbgTmpBrk.loc.addr ) != 0 ) ) ) {
        at_ip |= InsertOneBP( &DbgTmpBrk, force );
    }
    return( at_ip );
}



/*
 * RemoveOneBP -- remove one breakpoint from memory
 */

static void RemoveOneBP( brkp *bp )
{
    if( ( bp->status.b.in_place ) && SectIsLoaded( bp->loc.addr.sect_id, OVL_MAP_EXE ) ) {
        RemoteRestoreBreak( bp->loc.addr, bp->item.ud );
    }
}

static void RemoveOneWP( brkp *bp )
{
    mad_type_info       mti;

    if( ( bp->status.b.in_place ) && SectIsLoaded( bp->loc.addr.sect_id,OVL_MAP_EXE ) ) {
        bp->status.b.in_place = FALSE;
        MADTypeInfo( bp->th, &mti );
        RemoteRestoreWatch( bp->loc.addr, mti.b.bits / BITS_PER_BYTE );
    }
}


/*
 * RemoveBPs -- remove breakpoints from memory
 */

void RemoveBPs( void )
{
    brkp    *bp;

    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        if( bp->th != MAD_NIL_TYPE_HANDLE ) {
            RemoveOneWP( bp );
        } else {
            RemoveOneBP( bp );
        }
    }
    if( UserTmpBrk.status.b.active ) {
        RemoveOneBP( &UserTmpBrk );
    }
    if( DbgTmpBrk.status.b.active ) {
        RemoveOneBP( &DbgTmpBrk );
    }
}


void GetBPAddr( brkp *bp, char *buff )
{
    char                *p;

    if( bp->status.b.unmapped ) {
        p = StrCopy( LIT( Image_Not_Loaded ), buff );
        if( bp->image_name != NULL ) {
            *p++ = '(';
            p = StrCopy( bp->image_name, p );
            *p++ = ')';
        }
    } else if( bp->th != MAD_NIL_TYPE_HANDLE && bp->source_line != NULL ) {
        p = StrCopy( bp->source_line, buff );
    } else {
        p = CnvNearestAddr( bp->loc.addr, buff, TXT_LEN );
    }
    StrCopy( ":", p );
}


void GetBPText( brkp *bp, char *buff )
{
    unsigned    max;

    //MAD: might be a different mad then when break set
    max = ~0;
    if( bp->th == MAD_NIL_TYPE_HANDLE ) {
        if( bp->source_line != NULL ) {
            strcpy( buff, bp->source_line );
        } else {
            if( IS_NIL_ADDR( bp->loc.addr ) ) {
                strcpy( buff, LIT( Quest_Marks ) );
            } else {
                UnAsm( bp->loc.addr, buff, max );
            }
        }
    } else {
        MADTypeHandleToString( CurrRadix, bp->th, &bp->item, buff, &max );
    }
}


extern brkp *FindBreakByLine( mod_handle mod, cue_fileid id, unsigned line )
{
    brkp        *bp;
    mod_handle  brk_mod;
    DIPHDL( cue, ch );


    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        if( DeAliasAddrMod( bp->loc.addr, &brk_mod ) == SR_NONE )continue;
        if( brk_mod != mod )continue;
        if( DeAliasAddrCue( brk_mod, bp->loc.addr, ch ) == SR_NONE ) continue;
        if( CueFileId( ch ) != id ) continue;
        if( CueLine( ch ) != line ) continue;
        return( bp );
    }
    return( NULL );
}


extern brkp *FindBreak( address addr )
{
    brkp    *bp;

    if( IS_NIL_ADDR( addr ) ) return( NULL );
    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        if( ( AddrComp( bp->loc.addr, addr ) == 0 ) ) {
            return( bp );
        }
    }
    return( NULL );
}


/*
 * DispBPMsg -- display breakpoints that were hit
 */

static char *StrVal( char *which, brkp *wp, char *p )
{
    unsigned    max;

    for( *p++ = ' '; *which != NULLCHAR; *p++ = *which++ ) {}
    *p++ = '=';
    max = ~0;
    MADTypeHandleToString( CurrRadix, wp->th, &wp->item, p, &max );
    p += max;
    return( p );
}


static char     *GetBPAddrText( brkp *bp, char *p )
{
    if( bp->th != MAD_NIL_TYPE_HANDLE ) {
        p = StrCopy( LIT( Break_on_write ), p );
        if( bp->source_line != NULL ) {
            p = StrCopy( bp->source_line, p );
        } else {
            p = Format( p, "%a", bp->loc.addr );
        }
    } else {
        p = Format( p, LIT( Break_on_execute ) );
        p = Format( p, BrkFmt(), bp->loc.addr );
    }
    return( p );
}


static void GetWPVal( brkp *wp )
{
    wp->status.b.has_value = FALSE;
    if( ItemGetMAD( &wp->loc.addr, &wp->item, IT_NIL, wp->th ) ) {
        wp->status.b.has_value = TRUE;
    }
}


bool DispBPMsg( bool stack_cmds )
{
    brkp    *bp, *next;
    char    *p;
    bool    ret;
    cmd_list    *cmds;

    ret = FALSE;
    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        if( !bp->status.b.hit ) continue;
        if( !bp->status.b.silent ) {
            if( bp->th != MAD_NIL_TYPE_HANDLE ) {
                p = GetBPAddrText( bp, TxtBuff );
                p = StrCopy( " - ", p );
                p = StrVal( LIT( OldVal ), bp, p );
                GetWPVal( bp );
                p = StrVal( LIT( NewVal ), bp, p );
                *p = NULLCHAR;
            } else {
                p = GetBPAddrText( bp, TxtBuff );
            }
            DUIInfoBox( TxtBuff );
        }
        if( stack_cmds && bp->status.b.resume ) {
            cmds = AllocCmdList( "go/keep", sizeof( "go/keep" ) );
            PushCmdList( cmds );
            TypeInpStack( INP_HOOK );
            FreeCmdList( cmds );
            ret = TRUE;
        }
        if( stack_cmds && ( bp->cmds != NULL ) && bp->status.b.use_cmds ) {
            bp->status.b.cmds_pushed = TRUE;
            PushCmdList( bp->cmds );
            TypeInpStack( INP_BREAK_POINT );
            ret = TRUE;
        }
    }
    for( bp = BrkList; bp != NULL; bp = next ) {
        next = bp->next;
        if( bp->status.b.autodestruct ) {
            DUIRemoveBreak( bp );
        }
    }
    if( UserTmpBrk.status.b.hit ) {
        p = Format( TxtBuff, LIT( Break_on_execute ) );
        Format( p, BrkFmt(), UserTmpBrk.loc.addr );
        DUIDlgTxt( TxtBuff );
    }
    return( ret );
}


static char *GetBPCmd( brkp *bp, brk_event event, char *buff, unsigned buff_len )
{
    char        *cmds;
    char        *cond;
    char        *p;
    char        *end = buff + buff_len ;

    cmds = cond = LIT( Empty );
    if( bp != NULL ) {
        if( bp->cmds != NULL ) cmds = bp->cmds->buff;
        if( bp->condition != NULL ) cond = bp->condition;
    }
    p = Format( buff, "%s", GetCmdName( CMD_BREAK ) );
    switch( event ) {
    case B_SET:
        *p++ = '/';
        if( bp->th == MAD_NIL_TYPE_HANDLE ) {
            p = GetCmdEntry( PointNameTab, B_SET, p );
        } else {
            p += GetMADTypeNameForCmd( bp->th, p, end - p );
            *p++ = ' ';
        }
        if( bp->status.b.resume ) {
            *p++ = '/';
            p = GetCmdEntry( PointNameTab, B_RESUME, p );
        }
        if( !bp->status.b.active ) {
            *p++ = '/';
            p = GetCmdEntry( PointNameTab, B_DEACTIVATE, p );
        }
        *p++ = ' ';
        *p++ = '/';
        p = GetCmdEntry( PointNameTab, B_INDEX, p );
        p = Format( p, " %d, ", bp->index );
        if( bp->loc.image_name != NULL ) {
            *p++ = '/';
            p = GetCmdEntry( PointNameTab, B_UNMAPPED, p );
            *p++ = ' ';
            *p++ = '/';
            p = GetCmdEntry( PointNameTab, B_MAPADDRESS, p );
            *p++ = ' ';
            p = StrCopy( bp->loc.image_name, p );
            *p++ = ' ';
            p = AddHexSpec( p );
            p = CnvULongHex( bp->loc.addr.mach.segment, p, end - p );
            *p++ = ' ';
            p = AddHexSpec( p );
            p = CnvULongHex( bp->loc.addr.mach.offset, p, end - p );
            *p++ = ',';
        } else if( bp->image_name != NULL && bp->mod_name != NULL ) {
            *p++ = '/';
            p = GetCmdEntry( PointNameTab, B_UNMAPPED, p );
            *p++ = ' ';
            *p++ = '/';
            p = GetCmdEntry( PointNameTab, B_SYMADDRESS, p );
            *p++ = ' ';
            p = StrCopy( bp->image_name, p );
            *p++ = ' ';
            p = StrCopy( bp->mod_name, p );
            *p++ = ' ';
            p = StrCopy( bp->sym_name, p );
            *p++ = ' ';
            p = AddHexSpec( p );
            p = CnvULongHex( bp->cue_diff, p, end - p );
            *p++ = ' ';
            p = AddHexSpec( p );
            p = CnvULongHex( bp->addr_diff, p, end - p );
            *p++ = ',';
        } else {
            p = AddrToString( &bp->loc.addr, MAF_FULL, p, end - p );
//          p = Format( p, " %A", bp->loc.addr );
        }
        p = Format( p, " {%s} {%s}", cmds, cond );
        if( bp->initial_countdown != 0 ) {
            p = StrCopy( " ", p );
            p = AddHexSpec( p );
            p = CnvULongHex( bp->initial_countdown, p, end - p );
        }
        return( p );
    case B_CLEAR:
    case B_ACTIVATE:
    case B_DEACTIVATE:
    case B_RESUME:
    case B_UNRESUME:
        *p++ = '/';
        p = GetCmdEntry( PointNameTab, event, p );
        if( bp == NULL ) {
            p = StrCopy( "*", p );
        } else {
            p = AddrToString( &bp->loc.addr, MAF_FULL, p, end - p );
//          p = Format( p, " %A", bp->loc.addr );
        }
        return( p );
    }
    return( NULL );
}


static void RecordBreakEvent( brkp *bp, brk_event event )
{
    GetBPCmd( bp, event, TxtBuff, TXT_LEN );
    RecordEvent( TxtBuff );
    if( event == B_SET && !IS_NIL_ADDR( bp->loc.addr ) ) {
        GetBPAddrText( bp, TxtBuff );
        DUIStatusText( TxtBuff );
    }
}


static void DoActPoint( brkp *bp, bool act )
{
    bp->status.b.active = act;
    RecordBreakEvent( bp, act ? B_ACTIVATE : B_DEACTIVATE );
    if( act && bp->th != MAD_NIL_TYPE_HANDLE ) {
        GetWPVal( bp );
    }
}

void ActPoint( brkp *bp, bool act )
{
    if( act && bp->status.b.unmapped ) return;
    DoActPoint( bp, act );
    DbgUpdate( UP_BREAK_CHANGE );
}

void BrkEnableAll( void )
{
    brkp        *bp;

    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        DoActPoint( bp, TRUE );
    }
    DbgUpdate( UP_BREAK_CHANGE );
}


void BrkDisableAll( void )
{
    brkp    *bp;

    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        DoActPoint( bp, FALSE );
    }
    DbgUpdate( UP_BREAK_CHANGE );
}


void RemovePoint( brkp *bp )
{
    brkp        **owner;

    for( owner = &BrkList; ; owner = &(*owner)->next ) {
        if( ( *owner ) == bp ) {
            RecordBreakEvent( bp, B_CLEAR );
            FreeCmdList( bp->cmds );
            _Free( bp->condition );
            _Free( bp->source_line );
            _Free( bp->image_name );
            _Free( bp->mod_name );
            _Free( bp->sym_name );
            *owner = bp->next;
            DbgUpdate( UP_BREAK_CHANGE );
            FiniMappableAddr( &bp->loc );
            _Free( bp );
            break;
        }
    }
}


bool RemoveBreak( address addr )
{
    brkp        *bp;

    bp = FindBreak( addr );
    if( bp == NULL ) return( FALSE );
    RemovePoint( bp );
    return( TRUE );
}


void BrkClearAll( void )
{
    while( BrkList != NULL ) {
        RemoveBreak( BrkList->loc.addr );
    }
}


/*
 * BPsDeac -- deactivate all breakpoints
 */

void BPsDeac( void )
{
    brkp    *bp;

    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        DoActPoint( bp, FALSE );
    }
    DbgUpdate( UP_BREAK_CHANGE );
    NullStatus( &UserTmpBrk );
    NullStatus( &DbgTmpBrk );
}


/*
 * BPsUnHit -- turn off all BP_HIT indicators
 */

void BPsUnHit( void )
{
    brkp    *bp;

    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        bp->status.b.hit = FALSE;
    }
    NullStatus( &UserTmpBrk );
    NullStatus( &DbgTmpBrk );
}


void RecordNewPoint( brkp *bp )
{
    SetRecord( TRUE );
    RecordBreakEvent( bp, B_SET );
}


void RecordPointStart( void )
{
    brkp    *bp;

    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        RecordNewPoint( bp );
    }
}


void RecordClearPoint( brkp *bp )
{
    SetRecord( TRUE );
    RecordBreakEvent( bp, B_CLEAR );
}


void GetBreakOnImageCmd( char *name, char *buff, bool clear )
{
    char        *p;

    p = Format( buff, "%s", GetCmdName( CMD_BREAK ) );
    *p++ = '/';
    p = GetCmdEntry( PointNameTab, B_IMAGE, p );
    if( clear ) {
        *p++ = '/';
        p = GetCmdEntry( PointNameTab, B_CLEAR, p );
    }
    *p++ = ' ';
    p = StrCopy( name, p );
}


void ShowBPs( void )
{
    brkp        *bp;
    char_ring   *dll;

    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        GetBPCmd( bp, B_SET, TxtBuff, TXT_LEN );
        DUIDlgTxt( TxtBuff );
    }
    for( dll = DLLList; dll != NULL; dll = dll->next ) {
        GetBreakOnImageCmd( dll->name, TxtBuff, FALSE );
        DUIDlgTxt( TxtBuff );
    }
}


/*
 * ProcBreak -- process break command
 */

static void DoProcBreak( void )
{
    int         cmd;

    if( CurrToken == T_DIV ) {
        Scan();
        cmd = ScanCmd( PointNameTab );
        BPJmpTab[ cmd ].rtn( BPJmpTab[ cmd ].type );
    } else if( ScanEOC() ) {
        ShowBPs();
    } else {
        SetBreak( EXPR_CODE );
    }
}

void ProcBreak( void )
{
    DoProcBreak();
}

/*
 * SetBreak -- set a break point
 */

OVL_EXTERN brkp *SetBreak( memory_expr def_seg )
{
    return( SetPoint( def_seg, MAD_NIL_TYPE_HANDLE ) );
}

OVL_EXTERN brkp *TypePoint( memory_expr def_seg )
{
    mad_type_handle     th;

    th = ScanType( MAS_MEMORY | MTK_ALL, NULL );
    if( th == MAD_NIL_TYPE_HANDLE ) {
        BadPoint( def_seg );
    }
    return( SetPoint( def_seg, th ) );
}


OVL_EXTERN brkp *SetWatch( memory_expr def_seg )
{
    return( SetPoint( def_seg, MADTypeDefault( MTK_INTEGER, 0, &DbgRegs->mr, NULL ) ) );
}


/*
 * BadPoint -- handle bad break point command
 */

OVL_EXTERN brkp *BadPoint( memory_expr def_seg )
{
    def_seg=def_seg;
    Error( ERR_LOC, LIT( ERR_BAD_OPTION ), GetCmdName( CMD_BREAK ) );
    return( NULL );
}


/*
 * ImageBreak -- break on image loaded
 */

OVL_EXTERN brkp *ImageBreak( memory_expr def_seg )
{
    char        *start;
    size_t      len;
    bool        clear = FALSE;

    def_seg=def_seg;
    while( CurrToken == T_DIV ) {
        Scan();
        switch( ScanCmd( PointNameTab ) ) {
        case 0:
            goto done;
        case B_CLEAR:
            clear = TRUE;
            break;
        default:
            Error( ERR_LOC, LIT( ERR_BAD_OPTION ), GetCmdName( CMD_BREAK ) );
            break;
        }
    }
done:;
    if( !ScanItem( TRUE, &start, &len ) ) {
        BadPoint( def_seg );
    }
    BreakOnImageLoad( start, len, clear );
    Scan();
    ReqEOC();
    return( NULL );
}


/*
 * PointBreak -- get 'brkp *' from command line
 */

static brkp *PointBreak( memory_expr def_seg, address *addr )
{
    unsigned    index;
    brkp        *bp;
    unsigned    old;

    if( addr != NULL ) {
        *addr = NilAddr;
    }
    if( CurrToken == T_SHARP ) {
        Scan();
        old = NewCurrRadix( 10 );
        index = ReqExpr(); // always decimal
        NewCurrRadix( old );
        for( bp = BrkList; bp != NULL; bp = bp->next ) {
            if( bp->index == index ) break;
        }
    } else {
        ReqMemAddr( def_seg, addr );
        for( bp = BrkList; bp != NULL; bp = bp->next ) {
            if( AddrComp( bp->loc.addr, *addr ) == 0 ) break;
        }
    }
    ReqEOC();
    if( bp != NULL && addr != NULL ) {
        *addr = bp->loc.addr;
    }
    return( bp );
}


static brkp *BPNotNull( brkp *bp )
{
    if( bp == NULL ) {
        Error( ERR_NONE, LIT( ERR_NO_SUCH_POINT ) );
    }
    return( bp );
}

static void ResPoint( brkp *bp, bool res )
{
    bp->status.b.resume = res;
    RecordBreakEvent( bp, res ? B_RESUME : B_UNRESUME );
}

/*
 * Ac_DeacPoint -- (un)resume (de)activate specified break point
 */

static brkp *Ac_DeacPoint( memory_expr def_seg,
                          bool act, void ( *rtn ) ( brkp *, bool ) )
{
    brkp        *bp;
    address     addr;

    if( CurrToken == T_MUL ) {
        Scan();
        ReqEOC();
        for( bp = BrkList; bp != NULL; bp = bp->next ) {
            rtn( bp, act );
        }
        return( NULL );
    } else {
        bp = BPNotNull( PointBreak( def_seg, &addr ) );
        rtn( bp, act );
        return( bp );
    }
}


/*
 * ActivatePoint -- activate specified break point
 */

OVL_EXTERN brkp *ActivatePoint( memory_expr def_seg )
{
    return( Ac_DeacPoint( def_seg, TRUE, ActPoint ) );
}


/*
 * DeactivatePoint -- deactivate specified break point
 */

OVL_EXTERN brkp *DeactivatePoint( memory_expr def_seg )
{
    return( Ac_DeacPoint( def_seg, FALSE, ActPoint ) );
}

OVL_EXTERN brkp *ResumePoint( memory_expr def_seg )
{
    return( Ac_DeacPoint( def_seg, TRUE, ResPoint ) );
}

OVL_EXTERN brkp *UnResumePoint( memory_expr def_seg )
{
    return( Ac_DeacPoint( def_seg, FALSE, ResPoint ) );
}


/*
 * ClearPoint -- clear specified break point
 */

static brkp *ClearPoint( memory_expr def_seg )
{
    address addr;
    if( CurrToken == T_MUL ) {
        Scan();
        ReqEOC();
        while( BrkList != NULL ) {
            RemovePoint( BrkList );
        }
    } else {
        RemovePoint( BPNotNull( PointBreak( def_seg, &addr ) ) );
    }
    return( NULL );
}


char *CopySourceLine( cue_handle *ch )
{
    char        *p;
    char        *source_line;

    if( !DUIGetSourceLine( ch, TxtBuff, TXT_LEN ) ) return( NULL );
    for( p = TxtBuff; *p == ' '; ++p ) ;
    source_line = DupStr( p );
    return( source_line );
}


bool GetBPSymAddr( brkp *bp, address *addr )
{
    char        buff[TXT_LEN],*p;
    bool        rc;

    p = buff;
    if( bp->image_name != NULL ) {
        p = StrCopy( bp->image_name, p );
        p = StrCopy( "@", p );
    }
    p = StrCopy( bp->mod_name, p );
    p = StrCopy( "@", p );
    p = StrCopy( bp->sym_name, p );
    _SwitchOn( SW_AMBIGUITY_FATAL );
    rc = DlgScanCodeAddr( buff, addr );
    _SwitchOff( SW_AMBIGUITY_FATAL );
    return( rc );
}


void SetPointAddr( brkp *bp, address addr )
{
    DIPHDL( cue, ch );
    image_entry *image;
    mod_handle  mod;
    char  const *start;
    size_t      len;
    bool        ok;

    if( bp->status.b.unmapped ) return;
    _Free( bp->source_line );
    bp->source_line = NULL;
    bp->loc.addr = addr;
    _Free( bp->mod_name );
    bp->mod_name = NULL;
    _Free( bp->image_name );
    bp->image_name = NULL;
    _Free( bp->sym_name );
    bp->sym_name = NULL;
    bp->cue_diff = 0;
    bp->addr_diff = 0;
    if( bp->th != MAD_NIL_TYPE_HANDLE ) {
        GetWPVal( bp );
    } else if( DeAliasAddrMod( addr, &mod ) != SR_NONE ) {
        image = ImageEntry( mod );
        if( image == NULL ) return;
        ModName( mod, TxtBuff, TXT_LEN );
        bp->mod_name = DupStr( TxtBuff );
        if( image->image_name != NULL ) {
            start = SkipPathInfo( image->image_name, OP_REMOTE );
            len = ExtPointer( start, OP_REMOTE ) - start;
            bp->image_name = DupStrLen( start, len );
        } else {
            bp->image_name = NULL;
        }
        switch( DeAliasAddrCue( NO_MOD, addr, ch ) ) {
        case SR_EXACT:
            bp->source_line = CopySourceLine( ch );
            Format( TxtBuff, "%d", CueLine( ch ) );
            bp->sym_name = DupStr( TxtBuff );
            ok = GetBPSymAddr( bp, &addr );
            break;
        case SR_CLOSEST:
            Format( TxtBuff, "%d", CueLine( ch ) );
            bp->sym_name = DupStr( TxtBuff );
            bp->addr_diff = addr.mach.offset - CueAddr( ch ).mach.offset;
            ok = GetBPSymAddr( bp, &addr );
            break;
        default:
            ok = FALSE;
        }
        if( !ok ) {
            _Free( bp->image_name );
            _Free( bp->mod_name );
            _Free( bp->sym_name );
            bp->image_name = NULL;
            bp->mod_name = NULL;
            bp->sym_name = NULL;
        }
    }
}


bool BrkCheckWatchLimit( address loc, mad_type_handle th )
{
    bool                enough_iron;
    unsigned long       mult;
    brkp                *wp;
    mad_type_info       mti;
    unsigned            size;

    if( th != MAD_NIL_TYPE_HANDLE ) {
        MADTypeInfo( th, &mti );
        size = mti.b.bits / BITS_PER_BYTE;
        enough_iron = RemoteSetWatch( loc, size, &mult );
    } else {
        enough_iron = TRUE;
    }
    for( wp = BrkList; wp != NULL; wp = wp->next ) {
        if( wp->th == MAD_NIL_TYPE_HANDLE ) continue;
        MADTypeInfo( wp->th, &mti );
        if( !RemoteSetWatch( wp->loc.addr, mti.b.bits / BITS_PER_BYTE, &mult ) ) {
            enough_iron = FALSE;
        }
    }
    if( th != MAD_NIL_TYPE_HANDLE ) {
        RemoteRestoreWatch( loc, size );
    }
    for( wp = BrkList; wp != NULL; wp = wp->next ) {
        if( wp->th == MAD_NIL_TYPE_HANDLE ) continue;
        MADTypeInfo( wp->th, &mti );
        RemoteRestoreWatch( wp->loc.addr, mti.b.bits / BITS_PER_BYTE );
    }
    if( !enough_iron ) {
        return( DlgAreYouNuts( mult ) );
    }
    return( TRUE );
}


static int FindNextBPIndex( void )
{
    brkp        *bp;
    int         max;

    max = 0;
    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        if( bp->index > max ) max = bp->index;
    }
    return( max+1 );
}


static brkp *AddPoint( address loc, mad_type_handle th, bool unmapped )
{
    brkp                *bp;
    brkp                **owner;

    if( th != MAD_NIL_TYPE_HANDLE && !BrkCheckWatchLimit( loc, th ) ) return( NULL );
    _Alloc( bp, sizeof( brkp ) );
    InitMappableAddr( &bp->loc );
    bp->th = th;
    bp->mad = SysConfig.mad;
    NullStatus( bp );
    bp->status.b.active = TRUE;
    bp->source_line = NULL;
    bp->image_name = NULL;
    bp->mod_name = NULL;
    bp->sym_name = NULL;
    bp->cue_diff = 0;
    bp->status.b.unmapped = unmapped;
    SetPointAddr( bp, loc );
    bp->cmds = NULL;
    bp->status.b.use_cmds = FALSE;
    bp->index = FindNextBPIndex();

    bp->total_hits = 0;
    bp->countdown = 0;
    bp->initial_countdown = 0;
    bp->status.b.use_countdown = FALSE;
    bp->condition = NULL;
    bp->status.b.use_condition = FALSE;
    bp->error = NULL;
    owner = &BrkList;
    while( *owner != NULL ) {
        owner = &((*owner)->next);
    }
    bp->next = NULL;
    *owner = bp;
    DbgUpdate( UP_BREAK_CHANGE );
    return( bp );
}


extern brkp *AddBreak( address addr )
{
    brkp        *bp;

    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        if( AddrComp( bp->loc.addr, addr ) == 0 ) {
            DoActPoint( bp, TRUE );
            DbgUpdate( UP_BREAK_CHANGE );
            return( bp );
        }
    }
    bp = AddPoint( addr, MAD_NIL_TYPE_HANDLE, FALSE );
    if( bp == NULL ) return( NULL );
    RecordBreakEvent( bp, B_SET );
    return( bp );
}


extern void ToggleBreak( address addr )
{
    brkp        *bp;

    if( IS_NIL_ADDR( addr ) ) return;
    bp = FindBreak( addr );
    if( bp == NULL ) {
        AddBreak( addr );
    } else if( bp->status.b.active ) {
        ActPoint( bp, FALSE );
    } else {
        RemovePoint( bp );
    }
}


/*
 * TogglePoint -- toggle specified break point
 */

OVL_EXTERN brkp *TogglePoint( memory_expr def_seg )
{
    brkp        *bp;
    address     addr;

    bp = PointBreak( def_seg, &addr );
    if( IS_NIL_ADDR( addr ) ) {
        Error( ERR_NONE, LIT( ERR_NO_SUCH_POINT ) );
        return( NULL );
    } else {
        ToggleBreak( addr );
        return( bp );
    }
}


void BrkAddrRefresh( void )
{
    brkp        *bp;

    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        SetPointAddr( bp, bp->loc.addr );
    }
}


/*
 * SetPoint -- set specified break point
 */

static void ReqComma( void )
{
    if( CurrToken != T_COMMA ) {
        Error( ERR_LOC, LIT( ERR_BAD_OPTION ), GetCmdName( CMD_BREAK ) );
    }
    Scan();
}

void SetBPCountDown( brkp *bp, long countdown )
{
    bp->countdown = countdown;
    bp->initial_countdown = countdown;
    bp->status.b.use_countdown = ( countdown != 0 );
}

bool GetBPAutoDestruct( brkp *bp )
{
    return( bp->status.b.autodestruct );
}

void SetBPAutoDestruct( brkp *bp, int autodes )
{
    bp->status.b.autodestruct = autodes;
}

void SetBPCondition( brkp *bp, char *condition )
{
    if( bp->condition != NULL ) {
        _Free( bp->condition );
    }
    if( condition == NULL || condition[0] == '\0' ) {
        bp->condition = NULL;
    } else {
        bp->condition = DupStr( condition );
    }
    bp->status.b.use_condition = ( bp->condition != NULL );
}

void SetBPPatch( brkp *bp, char *patch )
{
    char        *end;

    end = StrCopy( patch, StrCopy( " ", StrCopy( GetCmdName( CMD_DO ), TxtBuff ) ) );
    if( bp->cmds != NULL ) {
        FreeCmdList( bp->cmds );
    }
    if( patch == NULL || patch[0] == '\0' ) {
        bp->cmds = NULL;
    } else {
        bp->cmds = AllocCmdList( TxtBuff, end - TxtBuff );
    }
    bp->status.b.use_cmds = ( bp->cmds != NULL );
}

void SetBPResume( brkp *bp, int resume )
{
    bp->status.b.resume = resume;
}

long GetBPCountDown( brkp *bp )
{
    return( bp->initial_countdown );
}

char *GetBPCondition( brkp *bp )
{
    if( bp->condition == NULL ) return( "" );
    return( bp->condition );
}

char *GetBPPatch( brkp *bp )
{
    if( bp->cmds == NULL ) return( "" );
    return( bp->cmds->buff + strlen( GetCmdName( CMD_DO ) ) + 1 );
}

int GetBPResume( brkp *bp )
{
    return( bp->status.b.resume );
}

static brkp *SetPoint( memory_expr def_seg, mad_type_handle th )
{
    brkp            *bp;
    char            *start;
    size_t          len;
    address         loc;
    cmd_list        *cmds;
    char            *condition;
    long            countdown;
    bool            resume;
    bool            active;
    int             index;
    mad_type_info   mti;
    unsigned        old;
    bool            unmapped;
    bool            mapaddress;
    bool            symaddress;
    char            *image_name;
    char            *mod_name;
    char            *sym_name;
    long            cue_diff;
    long            addr_diff;

    resume = FALSE;
    index = 0;
    active = TRUE;
    unmapped = FALSE;
    mapaddress = FALSE;
    symaddress = FALSE;
    while( CurrToken == T_DIV ) {
        Scan();
        switch( ScanCmd( PointNameTab ) ) {
        case 0:
            goto done;
        case B_RESUME:
            resume = TRUE;
            break;
        case B_UNRESUME:
            resume = FALSE;
            break;
        case B_ACTIVATE:
            active = TRUE;
            break;
        case B_DEACTIVATE:
            active = FALSE;
            break;
        case B_UNMAPPED:
            unmapped = TRUE;
            break;
        case B_MAPADDRESS:
            mapaddress = TRUE;
            ScanItem( TRUE, &start, &len );
            image_name = DupStrLen( start, len );
            loc.mach.segment = ReqLongExpr();
            loc.mach.offset = ReqLongExpr();
            ReqComma();
            break;
        case B_SYMADDRESS:
            symaddress = TRUE;
            ScanItem( TRUE, &start, &len );
            image_name = DupStrLen( start, len );
            ScanItem( TRUE, &start, &len );
            mod_name = DupStrLen( start, len );
            ScanItem( TRUE, &start, &len );
            sym_name = DupStrLen( start, len );
            cue_diff = ReqLongExpr();
            addr_diff = ReqLongExpr();
            loc = NilAddr;
            ReqComma();
            break;
        case B_INDEX:
            old = NewCurrRadix( 10 );
            index = ReqExpr();
            NewCurrRadix( old );
            ReqComma();
            break;
            /* fall thru */
        default:
            Error( ERR_LOC, LIT( ERR_BAD_OPTION ), GetCmdName( CMD_BREAK ) );
            break;
        }
    }
done:;
    if( !unmapped ) {
        ReqMemAddr( def_seg, &loc );
    }
    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        if( AddrComp( bp->loc.addr, loc ) == 0 ) {
            Error( ERR_NONE, LIT( ERR_POINT_EXISTS ) );
        }
    }
    cmds = NULL;
    condition = NULL;
    countdown = 0;
    if( ScanQuote( &start, &len ) ) {
        if( len != 0 ) cmds = AllocCmdList( start, len );
        if( ScanQuote( &start, &len ) ) {
            if( len != 0 ) condition = DupStrLen( start, len );
            if( !ScanEOC() ) {
                countdown = ReqExpr();
            }
        }
    }
    ReqEOC();
    if( th != MAD_NIL_TYPE_HANDLE ) {
        MADTypeInfo( th, &mti );
        switch( mti.b.bits / BITS_PER_BYTE ) {
        case 1:
        case 2:
        case 4:
            break;
        case 8:
            if( Supports8ByteBreakpoints )
                break;
        default:
            Error( ERR_NONE, LIT( ERR_NOT_WATCH_SIZE ) );
            break;
        }
    }
    bp = AddPoint( loc, th, unmapped );
    if( bp == NULL ) return( NULL );
    bp->status.b.unmapped = unmapped;
    if( mapaddress ) {
        bp->loc.image_name = image_name;
    }
    if( symaddress ) {
        bp->image_name = image_name;
        bp->mod_name = mod_name;
        bp->sym_name = sym_name;
        bp->cue_diff = cue_diff;
        bp->addr_diff = addr_diff;
    }
    bp->cmds = cmds;
    if( cmds != NULL ) bp->status.b.use_cmds = TRUE;
    bp->condition = condition;
    if( condition != NULL ) bp->status.b.use_condition = TRUE;
    SetBPCountDown( bp, countdown );
    bp->status.b.resume = resume;
    bp->status.b.active = active;
    if( index != 0 ) bp->index = index;
    RecordBreakEvent( bp, B_SET );
    return( bp );
}


bool BreakWrite( address addr, mad_type_handle th, char *comment )
{
    brkp                *bp;
    mad_type_info       mti;
    bool                ok_to_try = TRUE;
    if( th == MAD_NIL_TYPE_HANDLE ) return( FALSE );
    MADTypeInfo( th, &mti );
    switch( mti.b.bits / BITS_PER_BYTE ) {
    case 8:
        if( !Supports8ByteBreakpoints )
            ok_to_try = FALSE;
    case 1:
    case 2:
    case 4:
        if( ok_to_try ) {
            if(  FindBreak( addr ) != NULL ) {
                Error( ERR_NONE, LIT( ERR_POINT_EXISTS ) );
            }
            bp = AddPoint( addr, th, FALSE );
            if( bp == NULL ) return( TRUE );
            bp->source_line = DupStr( comment );
            RecordBreakEvent( bp, B_SET );
            return( TRUE );
        }
    default:
        return( FALSE );
    }
}

typedef struct tmp_break_struct
{
    address     addr;
    int         size;
    char        *comment;
} tmp_break_struct;

static void BreakOnAddress( void *_s )
/***********************************************/
{
    tmp_break_struct *s = _s;

    if( IS_NIL_ADDR( s->addr )
     || !BreakWrite( s->addr,
                    FindMADTypeHandle( MAS_MEMORY|MTK_INTEGER, s->size ),
                     s->comment ) ) {
        Error( ERR_NONE, LIT( ERR_NOT_WATCH_SIZE ) );
    }
}


bool BreakOnRawMemory( address addr, char *comment, int size )
/************************************************************/
{
    tmp_break_struct s;
    s.addr = addr;
    s.comment = comment;
    s.size = size;
    return( SpawnP( BreakOnAddress, &s ) == 0 );
}

extern void BreakOnExprSP( char *comment )
{
    address             addr;
    dip_type_info       tinfo;
    tmp_break_struct    s;

    LValue( ExprSP );
    tinfo.size = ExprSP->info.size;
    if( !( ExprSP->flags & SF_LOCATION ) ) {
        tinfo.size = DefaultSize( DK_INT );
    }
    switch( WndGetExprSPInspectType( &addr ) ) {
    case INSP_CODE:
        AddBreak( addr );
        break;
    case INSP_DATA:
    case INSP_RAW_DATA:
        s.addr = addr;
        s.size = tinfo.size;
        s.comment = comment;
        BreakOnAddress( &s );
        break;
    }
}


void PointFini( void )
{
    SetRecord( FALSE );
    while( BrkList != NULL ) {
        RemovePoint( BrkList );
    }
}


static  bool    HaveHitBP( brkp *bp )
{
    if( !bp->status.b.active ) return( FALSE );
    if( !bp->status.b.in_place ) return( FALSE );
    if( !SectIsLoaded( bp->loc.addr.sect_id, OVL_MAP_EXE ) ) return( FALSE );
    if( AddrComp( bp->loc.addr, GetRegIP() ) != 0 ) return( FALSE );
    return( TRUE );
}


OVL_EXTERN      void    TestExpression( void *_bp )
{
    brkp        *bp = _bp;
    char        *old;
    int         val;

    old = ReScan( bp->condition );
    val = ReqExpr();
    ReqEOC();
    ReScan( old );
    if( val ) {
        bp->status.b.expr_true = TRUE;
    }
}


void CheckBPErrors( void )
{
    brkp    *bp;
    brkp    *next;

    for( bp = BrkList; bp != NULL; bp = next ) {
        next = bp->next;
        if( bp->status.b.expr_error || bp->status.b.cmd_error ) {
            DlgBreak( bp->loc.addr );
        }
    }
}


void BrkCmdError( void )
{
    brkp    *bp;
    brkp    *next;

    for( bp = BrkList; bp != NULL; bp = next ) {
        next = bp->next;
        if( bp->status.b.cmds_pushed ) {
            bp->status.b.cmd_error = TRUE;
        }
    }
}


unsigned CheckBPs( unsigned conditions, unsigned run_conditions )
{
    brkp                *bp;
    item_mach           item;
    bool                hit, bphit, wphit;
    bool                state_set;
    mad_type_info       mti;

    wphit = FALSE;
    state_set = FALSE;
    bphit = FALSE;
    for( bp = BrkList; bp != NULL; bp = bp->next ) {
        if( !bp->status.b.active ) continue;
        hit = FALSE;
        if( bp->th != MAD_NIL_TYPE_HANDLE ) {
            if( SectIsLoaded( bp->loc.addr.sect_id, OVL_MAP_EXE ) ) {
                MADTypeInfo( bp->th, &mti );
                if( ItemGetMAD( &bp->loc.addr, &item, IT_NIL, bp->th ) ) {
                    
                    /*
                     * If the breakpoint fires here because of a write, but the value hasn't changed then
                     * the breakpoint does not fire off!!!! The SupportsExactBreakpoints actually enables
                     * break on write, not break on change and allocates the exact data space - not plus
                     * or minus a few bytes...
                     *
                     */
                    
                    if( _IsOn( SW_BREAK_ON_WRITE ) && SupportsExactBreakpoints ) {

                        bool    drop_hit = FALSE;
                        
                        if( ( UserTmpBrk.status.b.active ) || ( DbgTmpBrk.status.b.active ) ) {
                        
                            if( HaveHitBP( &UserTmpBrk ) ) {
                                drop_hit = TRUE;
                            }
                            if( HaveHitBP( &DbgTmpBrk ) ) {
                                drop_hit = TRUE;
                            }
                            if( ! ( conditions & ( COND_BREAK | COND_WATCH | COND_TRACE | COND_USER | COND_EXCEPTION | COND_STOP ) ) )
                                drop_hit = TRUE;
                        }
                        
                        if( !drop_hit ) 
                            hit = TRUE;
                    } else {
                        if( ( memcmp( &bp->item, &item, mti.b.bits / BITS_PER_BYTE ) != 0 ) || !bp->status.b.has_value ) {
                            hit = TRUE;
                        }
                    }
                } else if( bp->status.b.has_value ) {
                    if( conditions & ( COND_BREAK | COND_WATCH | COND_TRACE | COND_USER | COND_EXCEPTION | COND_STOP ) )
                        hit = TRUE;
                }
            }
        } else {
            if( HaveHitBP( bp ) ) {
                hit = TRUE;
            }
        }
        if( hit ) {
            bp->total_hits++;
        }
        if( hit && bp->condition != NULL && bp->status.b.use_condition ) {
            _SwitchOn( SW_ERR_IN_TXTBUFF );
            if( !state_set ) {
                /* gets all the registers updated */
                SetProgState( run_conditions );
                state_set = TRUE;
            }
            if( SpawnP( TestExpression, bp ) == 0 ) {
                if( !bp->status.b.expr_true ) {
                    hit = FALSE;
                } else {
                    bp->status.b.expr_true = FALSE;
                }
            } else {
                bp->status.b.expr_error = TRUE;
                bp->error = DupStr( TxtBuff );
                hit = TRUE;
            }
            _SwitchOff( SW_ERR_IN_TXTBUFF );
        }

        if( hit && bp->status.b.use_countdown && bp->countdown != 0 && !bp->status.b.expr_error ) {
            if( --bp->countdown != 0 ) {
                hit = FALSE;
            }
        }
        if( hit ) {
            if( bp->th != MAD_NIL_TYPE_HANDLE ) {
                wphit = TRUE;
            } else {
                bphit = TRUE;
            }
            bp->status.b.hit = TRUE;
        }
    }
    if( state_set ) {
        /* we might have changed a register in eval'ing conditions */
        WriteDbgRegs();
    }
    if( HaveHitBP( &UserTmpBrk ) ) {
        bphit = TRUE;
        UserTmpBrk.status.b.hit = TRUE;
    }
    if( HaveHitBP( &DbgTmpBrk ) ) {
        bphit = TRUE;
        DbgTmpBrk.status.b.hit = TRUE;
    }
    if( bphit ) return( COND_BREAK | ( conditions & ~COND_STOPPERS ) );
    if( wphit ) return( COND_WATCH | ( conditions & ~COND_STOPPERS ) );
    if( conditions & COND_BREAK ) {
        unsigned        max = TXT_LEN;

        ReadDbgRegs();
        if( MADUnexpectedBreak( &DbgRegs->mr, TxtBuff, &max ) == MS_OK ) {
            WriteDbgRegs();
            if( max == 0 ) {
                conditions &= ~COND_STOPPERS;
                if( _IsOff( SW_CONTINUE_UNEXPECTED_BREAK ) ) {
                    conditions |= COND_TRACE;
                }
                return( conditions );
            } else if( SetMsgText( TxtBuff, &conditions ) ) {
                DUIMsgBox( TxtBuff );
                return( COND_TRACE | COND_MESSAGE | ( conditions & ~COND_STOPPERS ) );
            } else {
                return( conditions & ~COND_STOPPERS );
            }
        } else {
            return( conditions & ~COND_STOPPERS );
        }
    } else if( conditions & COND_WATCH ) {
        return( conditions & ~COND_STOPPERS );
    }
    return( conditions );
}


bool UpdateWPs( void )
{
    brkp                *wp;
    bool                have_active;

    have_active = FALSE;
    for( wp = BrkList; wp != NULL; wp = wp->next ) {
        if( wp->th == MAD_NIL_TYPE_HANDLE ) continue;
        wp->status.b.hit = FALSE;
        wp->status.b.has_value = FALSE;
        if( wp->status.b.active && SectIsLoaded( wp->loc.addr.sect_id, OVL_MAP_EXE ) ) {
            have_active = TRUE;
            GetWPVal( wp );
        }
    }
    return( have_active );
}


void InsertWPs( void )
{
    brkp                *wp;
    unsigned long       mult;
    mad_type_info       mti;

    for( wp = BrkList; wp != NULL; wp = wp->next ) {
        if( wp->th == MAD_NIL_TYPE_HANDLE ) continue;
        if( wp->status.b.active && SectIsLoaded( wp->loc.addr.sect_id, OVL_MAP_EXE ) ) {
            wp->status.b.in_place = TRUE;
            MADTypeInfo( wp->th, &mti );
            RemoteSetWatch( wp->loc.addr, mti.b.bits / BITS_PER_BYTE, &mult );
        }
    }
}


void SaveBreaksToFile( char *name )
{
    UnMapPoints( NULL );
    CreateInvokeFile( name, ShowBPs );
    ReMapPoints( NULL );
}


void RestoreBreaksFromFile( char *name )
{
    BrkClearAll();
    InvokeAFile( name );
    DoInput();
    ReMapPoints( NULL );
}


typedef struct cue_first {
    cue_handle  *dest;
    bool        found;
} cue_first;

OVL_EXTERN walk_result FindCue( cue_handle *ch, void *_d )
{
    cue_first          *d = _d;
    HDLAssign( cue, d->dest, ch );
    d->found = TRUE;
    return( WR_STOP );
}

bool    FindFirstCue( mod_handle mod, cue_handle *ch )
{
    cue_first           d;

    d.dest = ch;
    d.found = FALSE;
    if( mod != NO_MOD ) {
        d.dest = ch;
        d.found = FALSE;
        WalkFileList( mod, FindCue, &d );
    }
    return( d.found );
}

void BreakAllModEntries( mod_handle handle )
{
    name_list   list;
    address     addr;
    int         i;
    bool        have_mod_cue;
    sym_info    sinfo;
    DIPHDL( cue, ch );
    DIPHDL( cue, ch_mod );

    NameListInit( &list, WF_CODE );
    NameListAddModules( &list, handle, FALSE, TRUE );
    have_mod_cue = FindFirstCue( handle, ch_mod );
    for( i = 0; i < NameListNumRows( &list ); ++i ) {
        addr = NameListAddr( &list, i );
        SymInfo( NameListHandle( &list, i ), NULL, &sinfo );
        if( !sinfo.is_global && !sinfo.is_public ) continue;
        if( have_mod_cue && DeAliasAddrCue( handle, addr, ch ) != SR_NONE ) {
            if( CueFileId( ch ) != CueFileId( ch_mod ) ) continue;
        }
        if( FindBreak( addr ) != NULL ) continue;
        AddBreak( addr );
    }
    NameListFree( &list );
}

void ClearAllModBreaks( mod_handle handle )
{
    brkp        *bp, *next;
    mod_handle  mh;

    for( bp = BrkList; bp != NULL; bp = next ) {
        next = bp->next;
        DeAliasAddrMod( bp->loc.addr, &mh );
        if( mh == handle ) {
            RemovePoint( bp );
        }
    }
}

address GetRowAddrDirectly( mod_handle mod, cue_fileid file_id, int row, bool exact )
{
    DIPHDL( cue, ch );

    if( mod == NO_MOD || row < 0 ) return( NilAddr );
    switch( LineCue( mod, file_id, row+1, 0, ch ) ) {
    case SR_NONE:
        return( NilAddr );
    case SR_CLOSEST:
        if( exact ) return( NilAddr );
        break;
    }
    return( CueAddr( ch ) );
}

brkp *GetBPAt( int row )
{
    int     i = 0;
    brkp    *bp = NULL;

    for( bp = BrkList , i = 0 ; bp != NULL; bp = bp->next , ++i ) {
        if ( i == row ) {
            return( bp );
        }
    }
    return( bp );
}

int GetBPsCount( void )
{
    return( FindNextBPIndex() - 1 );
}
