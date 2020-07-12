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
* Description:  Miscellaneous odds and ends that don't have a home.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgerr.h"
#include "dbgmem.h"
#include "dui.h"
#include "dbglit.h"
#include "madinter.h"
#include "i64.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgutil.h"
#include "dbgexec.h"
#include "dbgstk.h"
#include "dbgexpr.h"
#include "dbgmain.h"
#include "dbgshow.h"
#include "dbgparse.h"
#include "dbgdot.h"
#include "dbgtrace.h"
#include "dbgtback.h"
#include "remcore.h"
#include "dbgmisc.h"
#include "remthrd.h"
#include "remrtrd.h"
#include "dbgreg.h"
#include "addarith.h"
#include "dbgevent.h"
#include "dbgupdt.h"


#define SWITCH_DEFS \
    pick( "ON",     SWITCH_ON     ) \
    pick( "OFf",    SWITCH_OFF    )

#define GO_DEFS \
    pick( "Keep",   GO_KEEP       ) \
    pick( "Noflip", GO_NOFLIP     ) \
    pick( "Until",  GO_UNTIL      )

#define THREAD_DEFS \
    pick( "Show",   THREAD_SHOW   ) \
    pick( "Freeze", THREAD_FREEZE ) \
    pick( "Thaw",   THREAD_THAW   ) \
    pick( "Change", THREAD_CHANGE )


enum {
    #define pick(t,e)   e,
    SWITCH_DEFS
    #undef pick
};

enum {
    #define pick(t,e)   e,
    GO_DEFS
    #undef pick
};

typedef enum thread_cmd {
    THREAD_BAD = -1,
    #define pick(t,e)   e,
    THREAD_DEFS
    #undef pick
} thread_cmd;

extern void             FlipScreen( void );

static const char SwitchNameTab[] = {
    #define pick(t,e)   t "\0"
    SWITCH_DEFS
    #undef pick
};

static const char GoOptionTab[] = {
    #define pick(t,e)   t "\0"
    GO_DEFS
    #undef pick
};

static const char ThreadOps[] = {
    #define pick(t,e)   t "\0"
    THREAD_DEFS
    #undef pick
};

static const char AddTab[] = "Add\0";

static const char MainTab[] = "MAin\0";

static const char NogoTab[] = "NOgo\0";

bool ScanCmdAdd( void )
{
    return( ScanCmd( AddTab ) == 0 );
}

char *GetCmdAdd( char *buffer )
{
    return( GetCmdEntry( AddTab, 0, buffer ) );
}

bool ScanCmdMain( void )
{
    return( ScanCmd( MainTab ) == 0 );
}

char *GetCmdMain( char *buffer )
{
    return( GetCmdEntry( MainTab, 0, buffer ) );
}

bool ScanCmdNogo( void )
{
    return( ScanCmd( NogoTab ) == 0 );
}

char *GetCmdNogo( char *buffer )
{
    return( GetCmdEntry( NogoTab, 0, buffer ) );
}

int SwitchOnOffOnly( void )
{
    switch( ScanCmd( SwitchNameTab ) ) {
    case SWITCH_OFF:
        return( false );
    case SWITCH_ON:
        return( true );
    default:
        return( -1 );
    }
}

void GetSwitchOnly( bool on, char *buffer )
{
    GetCmdEntry( SwitchNameTab, on ? SWITCH_ON : SWITCH_OFF, buffer );
}

void Flip( uint_16 wait )
{
//    char        ch;

    if( !(ScrnState & USR_SCRN_VISIBLE) ) {
        DUIWndUser();
    }
    if( !(ScrnState & DBG_SCRN_VISIBLE) ) {
//        ch = RemoteReadUserKey( wait );
        RemoteReadUserKey( wait );
    }
}

void ProcFlip( void )
{
    const char  *old;
    uint_16     wait;

    wait = 0;
    if( !ScanEOC() ) {
        old = ScanPos();
        switch( SwitchOnOffOnly() ) {
        case false:
            ReqEOC();
            _SwitchOn( SW_FLIP );
            return;
        case true:
            ReqEOC();
            _SwitchOff( SW_FLIP );
            return;
        default:
            ReScan( old );
            wait = (uint_16)ReqExpr();
            ReqEOC();
            break;
        }
    }
    Flip( wait );
}


void ConfigFlip( void )
{
    char *p;

    ReqEOC();
    p = StrCopy( GetCmdName( CMD_FLIP ), TxtBuff );
    *p++ = ' ';
    GetSwitchOnly( _IsOff( SW_FLIP ), p );
    DUIDlgTxt( TxtBuff );
}

/*
 * ProcRemark - process a remark command
 */

void ProcRemark( void )
{
    char        c;

    RawScanInit();
    while( (c = RawScanChar()) != NULLCHAR ) {
        RawScanAdvance();
        /* The CR character indicates end of line in a multi-line command
           list. */
        if( c == '\r' ) {
            break;
        }
    }
    RawScanFini();
}



/*
 * ProcDo - process do command
 */

void ProcDo( void )
{
    NormalExpr();
    PopEntry();
    ReqEOC();
}


void ProcAssign( void )
{
    const char      *startpos;

    if( !AdvMachState( ACTION_ASSIGNMENT ) ) {
        FlushEOC();
        return;
    }
    startpos = ScanPos();
    NormalExpr();
    PopEntry();
    ReqEOC();
    RecordCommand( startpos, CMD_ASSIGN );
}


static void SetTempBreak( address addr )
{
    NullStatus( &UserTmpBrk );
    UserTmpBrk.status.b.has_address = true;
    UserTmpBrk.status.b.active = true;
    UserTmpBrk.loc.addr = addr;
}


void GoToAddr( address addr )
{
    if( IS_NIL_ADDR( addr ) )
        return;
    SetTempBreak( addr );
    Go( true );
    NullStatus( &UserTmpBrk );
}


void StepIntoFunction( const char *func )
{
    const char  *old;
    address     stop;

    old = ReScan( func );
    ReqMemAddr( EXPR_CODE, &stop );
    ReScan( old );
    GoToAddr( stop );
}


void SkipToAddr( address addr )
{
    if( IS_NIL_ADDR( addr ) )
        return;
    if( !AdvMachState( ACTION_MODIFY_IP ) )
        return;
    RecordSetRegIP( addr );
    DbgUpdate( UP_CSIP_CHANGE | UP_REG_CHANGE );
}


void GoToReturn( void )
{
    address     ra;

    ra = ReturnAddress();
    if( !IS_NIL_ADDR( ra ) ) {
        GoToAddr( ra );
    } else {
        Error( ERR_NONE, LIT_ENG( ERR_NO_RETURN_ADDRESS ) );
    }
}


/*
 * ProcGo -- process go command
 */

void ProcGo( void )
{
    address     start;
    address     stop;
    bool        have_start;
    bool        have_stop;
    bool        have_keep;
    bool        doflip;
    bool        flip_on;
    bool        until;
    unsigned    conditions;
    int         count;

    have_keep = false;
    doflip = true;
    until = false;
    while( CurrToken == T_DIV ) {
        Scan();
        switch( ScanCmd( GoOptionTab ) ) {
        case GO_KEEP:
            have_keep = true;
            break;
        case GO_NOFLIP:
            doflip = false;
            break;
        case GO_UNTIL:
            until = true;
            break;
        default:
            Error( ERR_LOC, LIT_ENG( ERR_BAD_OPTION ), GetCmdName( CMD_GO ) );
        }
    }
    if( !have_keep )
        TraceKill();
    NullStatus( &DbgTmpBrk );
    start = GetRegIP();
    have_stop = false;
    have_start = false;
    if( !ScanEOC() ) {
        stop = start;
        ReqMemAddr( EXPR_GIVEN, &stop );
        if( CurrToken == T_COMMA ) {
            start = stop;
            have_start = true;
            Scan();
            if( !ScanEOC() ) {
                ReqMemAddr( EXPR_GIVEN, &stop );
                have_stop = true;
            }
        } else {
            have_stop = true;
        }
    }
    ReqEOC();
    count = 0;
    do {
        if( have_stop ) {
            if( have_keep ) {
                if( UserTmpBrk.status.b.has_address ) {
                    NullStatus( &UserTmpBrk );
                    UserTmpBrk.status.b.has_address = true;
                } else {
                    NullStatus( &UserTmpBrk );
                }
                Error( ERR_NONE, LIT_ENG( ERR_INCONSISTENT_GO ) );
            }
            SetTempBreak( stop );
        } else if( have_keep ) {
            if( UserTmpBrk.status.b.has_address ) {
                UserTmpBrk.status.b.active = true;
            }
        } else {
            NullStatus( &UserTmpBrk );
        }
        if( have_start ) {
            RecordSetRegIP( start );
        }
        flip_on = _IsOn( SW_FLIP );
        if( !doflip ) {
            _SwitchOff( SW_FLIP );
        }
        conditions = Go( doflip );
        if( flip_on )
            _SwitchOn( SW_FLIP );
        if( UserTmpBrk.status.b.has_address ) {
            NullStatus( &UserTmpBrk );
            UserTmpBrk.status.b.has_address = true;
        } else {
            NullStatus( &UserTmpBrk );
        }
    } while( until && (conditions & COND_BREAK) && AddrComp( stop, GetRegIP() ) != 0 && ++count < 100 );
}

/*
 * ProcSkip -- process skip command
 */

void ProcSkip( void )
{
    address    start;

    ReqMemAddr( EXPR_GIVEN, &start );
    ReqEOC();
    SkipToAddr( start );
}

/*
 * ProcIf -- process if command
 */

void ProcIf( void )
{
    const char          *start;
    size_t              len;
    const char          *true_start;
    size_t              true_len;
    unsigned_64         res;
    cmd_list            *cmd;
    bool                have_true;

    true_len = 0;
    have_true = false;
    for( ;; ) {
        if( have_true ) {
            ChkExpr();
            U64Clear( res );
        } else {
            res = ReqU64Expr();
        }
        if( !ScanQuote( &start, &len ) ) {
            Error( ERR_LOC, LIT_ENG( ERR_WANT_COMMAND_LIST ) );
        }
        if( U64Test( &res ) != 0 ) {
            true_start = start;
            true_len   = len;
            have_true  = true;
        }
        if( ScanCmd( "ELSEIF\0" ) != 0 ) {
            break;
        }
    }
    ScanCmd( "ELSE\0" ); /* optional else */
    if( ScanQuote( &start, &len ) && !have_true ) {
        true_start = start;
        true_len   = len;
    }
    ReqEOC();
    if( true_len != 0 ) {
        cmd = AllocCmdList( true_start, true_len );
        PushCmdList( cmd );
        CopyInpFlags();
        FreeCmdList( cmd );
    }
}



/*
 * ProcWhile -- process While command
 */

void ProcWhile( void )
{
    const char          *start;
    size_t              len;
    unsigned_64         res;
    cmd_list            *cmd;

    res = ReqU64Expr();
    if( !ScanQuote( &start, &len ) || len == 0 ) {
        Error( ERR_LOC, LIT_ENG( ERR_WANT_COMMAND_LIST ) );
    }
    ReqEOC();
    if( U64Test( &res ) != 0 ) {
        cmd = AllocCmdList( start, len );
        ReScan( CmdStart );
        PushCmdList( cmd );
        CopyInpFlags();
        FreeCmdList( cmd );
    }
}


/*
 * ProcError -- proccess error command
 */

void ProcError( void )
{
    const char  *start;
    size_t      len;

    ScanItem( false, &start, &len );
    ReqEOC();
    memcpy( TxtBuff, start, len );
    TxtBuff[len] = NULLCHAR;
    Error( ERR_NONE, LIT_ENG( ERR_GENERIC ), TxtBuff );
}


/*
 * ProcQuit -- process quit command
 */

void ProcQuit( void )
{
    ReqEOC();
    DebugExit();
}

/*
 * ProcThread -- process thread command
 */

static void FormThdState( thread_state *thd, char *buff, unsigned buff_len )
{
    char *p;
    char *p1;

    p = CnvULongHex( thd->tid, buff, buff_len );
    *p++ = ' ';
    *p++ = ' ';
    switch( thd->state ) {
    case THD_THAW:  p1 = LIT_ENG( Runnable ); break;
    case THD_FREEZE:p1 = LIT_ENG( Frozen );   break;
    case THD_DEAD:  p1 = LIT_ENG( Dead );     break;
    default:        p1 = "";              break;
    }
    p1 = StrCopy( p1, p );
    while( (p1 - p) < 10 )
        *p1++ = ' ';
    StrCopy( thd->name, p1 );
}


void MakeThdCurr( thread_state *thd )
{
    error_handle    errh;

    if( !AdvMachState( ACTION_THREAD_CHANGE ) )
        return;
    // NYI - PUI - record the thread change?
    WriteDbgRegs();
    if( RemoteSetThreadWithErr( thd->tid, &errh ) == 0 ) {
        Error( ERR_NONE, LIT_ENG( ERR_NO_MAKE_CURR_THREAD ), thd->tid, errh );
    }
    DbgRegs->tid = thd->tid;
    ReadDbgRegs();
    SetCodeDot( GetRegIP() );
    DbgUpdate( UP_REG_CHANGE | UP_CSIP_CHANGE | UP_THREAD_STATE );
}


void MakeRunThdCurr( thread_state *thd )
{
    error_handle    errh;

    if( !AdvMachState( ACTION_THREAD_CHANGE ) )
        return;
    if( RemoteSetRunThreadWithErr( thd->tid, &errh ) == 0 ) {
        Error( ERR_NONE, LIT_ENG( ERR_NO_MAKE_CURR_THREAD ), thd->tid, errh );
    }
    DbgRegs->tid = thd->tid;
    ReadDbgRegs();
    SetCodeDot( GetRegIP() );
    DbgUpdate( UP_REG_CHANGE | UP_CSIP_CHANGE | UP_THREAD_STATE );
}


dtid_t RemoteSetThread( dtid_t tid )
{
    error_handle    errh;

    if( HaveRemoteRunThread() ) {
        return( RemoteSetRunThreadWithErr( tid, &errh ) );
    } else {
        return( RemoteSetThreadWithErr( tid, &errh ) );
    }
}


static void ThdCmd( thread_state *thd, thread_cmd tcmd )
{
    unsigned    up;

    up = UP_THREAD_STATE;
    switch( tcmd ) {
    case THREAD_SHOW:
        FormThdState( thd, TxtBuff, TXT_LEN );
        DUIDlgTxt( TxtBuff );
        up = UP_NO_CHANGE;
        break;
    case THREAD_FREEZE:
        if( thd->state == THD_THAW )
            thd->state = THD_FREEZE;
        break;
    case THREAD_THAW:
        if( thd->state == THD_FREEZE )
            thd->state = THD_THAW;
        break;
    case THREAD_CHANGE:
        MakeThdCurr( thd );
        break;
    }
    DbgUpdate( up );
}


bool IsThdCurr( thread_state *thd )
{
    return( DbgRegs->tid == thd->tid );
}


static thread_state     *AddThread( dtid_t tid, unsigned state )
{
    thread_state    **owner;
    thread_state    *thd;
    char            name[UTIL_LEN];

    for( owner = &HeadThd; (thd = *owner) != NULL; owner = &thd->link ) {
        if( thd->tid > tid )
            break;
        if( thd->tid == tid ) {
            if( _IsOn( SW_THREAD_EXTRA_CHANGED ) ) {
                *owner = thd->link;
                state = thd->state & ~THD_DEAD;
                _Free( thd );
                break;
            }
            thd->state &= ~THD_DEAD;
            return( thd );
        }
    }
    if( HaveRemoteRunThread() ) {
        RemoteRunThdName( tid, name );
    } else {
        RemoteThdName( tid, name );
    }
    _Alloc( thd, sizeof( thread_state ) + strlen( name ) );
    if( thd == NULL )
        return( NULL );
    thd->link = *owner;
    *owner = thd;
    strcpy( thd->name, name );
    thd->tid = tid;
    thd->state = state;
    thd->cs = 0;
    thd->eip = 0;
    thd->extra[0] = 0;
    return( thd );
}


dtid_t GetNextTID( void )
/***********************/
{
    thread_state    *thd;

    for( thd = HeadThd; thd != NULL; thd = thd->link ) {
        if( IsThdCurr( thd ) ) {
            thd = thd -> link;
            break;
        }
    }
    if( thd == NULL )
        thd = HeadThd;
    if( thd == NULL )
        return( 0 );
    return( thd->tid );
}


void NameThread( dtid_t tid, const char *name )
/*********************************************/
{
    thread_state        **owner, *curr, *new;

    for( owner = &HeadThd; (curr = *owner) != NULL; owner = &curr->link ) {
        if( curr->tid == tid ) {
            _Alloc( new, sizeof( thread_state ) + strlen( name ) );
            *new = *curr;
            *owner = new;
            strcpy( new->name, name );
            DbgUpdate( UP_THREAD_STATE );
            _Free( curr );
            break;
        }
    }
}

static void MarkThreadsDead( void )
{
    thread_state    *thd;

    for( thd = HeadThd; thd != NULL; thd = thd->link ) {
        thd->state |= THD_DEAD;
    }
}

static void KillDeadThreads( void )
{
    thread_state    **owner;
    thread_state    *thd;

    for( owner = &HeadThd; (thd = *owner) != NULL; ) {
        if( thd->state & THD_DEAD ) {
            *owner = thd->link;
            _Free( thd );
        } else {
            owner = &thd->link;
        }
    }
}

static void RefreshThreads( bool set_exec )
{
    dtid_t              tid;
    thread_state        *thd;
    unsigned            state;

    for( tid = 0; (tid = RemoteGetNextThread( tid, &state )) != 0; ) {
        thd = AddThread( tid, state );
        if( set_exec && thd != NULL && thd->tid == DbgRegs->tid ) {
            ExecThd = thd;
        }
    }
}

static void RefreshRunThreads( bool set_exec )
{
    dtid_t              tid;
    thread_state        *thd;

    for( tid = 0; (tid = RemoteGetNextRunThread( tid )) != 0; ) {
        thd = AddThread( tid, 0 );
        RemoteUpdateRunThread( thd );
        if( set_exec && thd != NULL && thd->tid == DbgRegs->tid ) {
            ExecThd = thd;
        }
    }
}

void CheckForNewThreads( bool set_exec )
{
    if( set_exec )
        ExecThd = NULL;
    MarkThreadsDead();
    if( HaveRemoteRunThread() ) {
        RefreshRunThreads( set_exec );
    } else {
        RefreshThreads( set_exec );
    }
    KillDeadThreads();
    _SwitchOff( SW_THREAD_EXTRA_CHANGED );
}


thread_state   *FindThread( dtid_t tid )
{
    thread_state    *thd;

    for( thd = HeadThd; thd != NULL; thd = thd->link ) {
        if( thd->tid == tid ) {
            break;
        }
    }
    return( thd );
}


void ProcThread( void )
{
    thread_cmd          tcmd;
    int                 cmd;
    dtid_t              tid;
    bool                all;
    thread_state        *thd;
    mad_radix           old_radix;

    tcmd = THREAD_BAD;
    if( CurrToken == T_DIV ) {
        Scan();
        cmd = ScanCmd( ThreadOps );
        if( cmd < 0 ) {
            Error( ERR_NONE, LIT_ENG( ERR_BAD_OPTION ), GetCmdName( CMD_THREAD ) );
        } else {
            tcmd = (thread_cmd)cmd;
        }
    }
    CheckForNewThreads( false );
    if( CurrToken == T_MUL ) {
        if( tcmd == THREAD_BAD )
            tcmd = THREAD_SHOW;
        Scan();
        all = true;
    } else if( ScanEOC() ) {
        if( tcmd == THREAD_BAD )
            tcmd = THREAD_SHOW;
        if( tcmd == THREAD_SHOW ) {
            all = true;
        } else {
            tid = DbgRegs->tid;
            all = false;
        }
    } else {
        if( tcmd == THREAD_BAD )
            tcmd = THREAD_CHANGE;
        old_radix = SetCurrRadix( 16 );
        tid = ReqExpr();
        all = false;
        SetCurrRadix( old_radix );
    }
    ReqEOC();
    if( all ) {
        if( tcmd == THREAD_CHANGE )
            Error( ERR_NONE, LIT_ENG( ERR_CANT_SET_ALL_THREADS ) );
        for( thd = HeadThd; thd != NULL; thd = thd->link ) {
            ThdCmd( thd, tcmd );
        }
    } else {
        thd = FindThread( tid );
        if( thd == NULL ) {
            Error( ERR_NONE, LIT_ENG( ERR_NO_SUCH_THREAD ), tid );
        } else {
            ThdCmd( thd, tcmd );
        }
    }
}

void FreeThreads( void )
{
    thread_state    *thd;

    while( (thd = HeadThd) != NULL ) {
        HeadThd = thd->link;
        _Free( thd );
    }
}
