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
* Description:  User program registers access and management.
*
****************************************************************************/


#include <string.h>
#include <stddef.h>
#include "dbgdefn.h"
#include "dbgreg.h"
#include "dbglit.h"
#include "dbgmem.h"
#include "dbgerr.h"
#include "dbgtoggl.h"
#include "dbginfo.h"
#include "dbgtback.h"
#include "dipwv.h"
#include "dbgrep.h"
#include "dbgitem.h"
#include "mad.h"
#include "dui.h"

extern int              AddrComp( address a, address b );
extern char             *Format( char * buff, char * fmt, ... );
extern void             RecordEvent( char *p );
extern char             *GetCmdName( int index );
extern bool             DlgUpTheStack( void );
extern bool             DlgBackInTime( bool lost_mem_state );
extern bool             DlgIncompleteUndo( void );
extern bool             TraceModifications( MAD_MEMREF_WALKER *wk, void *d );
extern void             ReportMADFailure( mad_status );

extern void             LocationCreate( location_list *ll, location_type lt, void *d );
extern dip_status       LocationAssign( location_list *dst, location_list *src, unsigned long len, bool sign_extend );
extern void             PushLocation( location_list *ll, dip_type_info *ti );
extern void             DoAssign( void );
extern char             *StrCopy( char *, char * );
extern void             MadTypeToDipTypeInfo( mad_type_handle mt, dip_type_info *ti );

extern machine_state    *DbgRegs;
extern machine_state    *PrevRegs;
extern unsigned         OvlSize;
extern address          NilAddr;
extern char             *TxtBuff;
extern unsigned char    CurrRadix;

static int              StackPos;
static bool             AlreadyWarnedUndo;
extern system_config    SysConfig;


#define MAX_DELTA_BITS  8
#define MAX_DELTA_BYTES ((1<<MAX_DELTA_BITS)-1)

typedef struct memory_delta {
    struct memory_delta *next;
    address             addr;
    unsigned            size            : MAX_DELTA_BITS;
    unsigned            after_set       : 1;
    unsigned            _volatile       : 1;
    unsigned_8          data[1]; /* variable sized */
} memory_delta;

typedef struct save_state {
    struct save_state   *prev;
    struct save_state   *next;
    unsigned            valid           : 1;
    unsigned            lost_mem_state  : 1;
    memory_delta        *mem;
    int                 action;
    machine_state       s;      /* variable sized */
} save_state;

static  save_state      *StateCurr;
static  save_state      *StateLast;
static  int             NumStateEntries = 0;

extern long             GetDataLong( void );
extern void             StartupErr( char * );
extern unsigned         ReqExpr( void );
extern void             ReqEOC( void );
extern void             DbgUpdate( update_list );
extern unsigned         SetCurrRadix( unsigned int );
extern void             Warn( char * );
extern void             LogLine( char * );
extern void             FindAddrSectId( address *, int );
extern void             SetStateOvlSect( machine_state *, unsigned int );
extern unsigned int     RemoteOvlSectSize( void );
extern void             ReleaseProgOvlay( bool );
extern void             SectTblRead( machine_state * );
extern char             *Rtrm( char * );
extern dtid_t           RemoteSetThread( dtid_t );
extern thread_state     *FindThread( dtid_t );
extern bool             InitOvlState( void );
extern void             FiniOvlState( void );
extern unsigned         ProgPoke( address addr, void *data, unsigned len );
extern unsigned         ProgPeek( address addr, void *data, unsigned len );
extern void             SetCodeLoc( address );
extern char             *NamePos( void );
extern unsigned int     NameLen( void );
extern void             Scan( void );
extern void             AddrFix( address * );
extern void             AddrFloat( address * );
extern void             AddrSection( address *, unsigned );
extern mad_reg_info     *LookupRegName( mad_reg_info *, lookup_item * );
extern void             LocationAppend( location_list *, location_list * );
extern dip_status       RegLocation( machine_state *regs, mad_reg_info const *ri, location_list *ll );
extern void             InitLC( location_context *new, bool use_real_regs );
extern address          DefAddrSpaceForAddr( address );


void DefAddr( memory_expr def_seg, address *addr )
{
    switch( def_seg ) {
    case EXPR_CODE:
        *addr = Context.execution;
        addr->mach.offset = 0;
        break;
    case EXPR_DATA:
        *addr = DefAddrSpaceForAddr( Context.execution );
        break;
    }
}

static address AddrRegIP( machine_state *regs )
{
    address    addr;

    MADRegSpecialGet( MSR_IP, &regs->mr, &addr.mach );
    AddrSection( &addr, OVL_MAP_CURR );
    return( addr );
}


address GetRegIP( void )
{
    return( AddrRegIP( DbgRegs ) );
}


void SetRegIP( address addr )
{
    AddrFix( &addr );
    MADRegSpecialSet( MSR_IP, &DbgRegs->mr, &addr.mach );
    SetStateOvlSect( DbgRegs, addr.sect_id );
}


void RecordSetRegIP( address addr )
{
    Format( TxtBuff, "%s %A", GetCmdName( CMD_SKIP ), addr );
    RecordEvent( TxtBuff );
    SetRegIP( addr );
}


void SetRegSP( address addr )
{
    AddrFix( &addr );
    MADRegSpecialSet( MSR_SP, &DbgRegs->mr, &addr.mach );
}


void SetRegBP( address addr )
{
    AddrFix( &addr );
    MADRegSpecialSet( MSR_FP, &DbgRegs->mr, &addr.mach );
}


address GetRegSP( void )
{
    address     addr;

    MADRegSpecialGet( MSR_SP, &DbgRegs->mr, &addr.mach );
    AddrSection( &addr, OVL_MAP_CURR );
    return( addr );
}


address GetRegBP( void )
{
    address     addr;

    MADRegSpecialGet( MSR_FP, &DbgRegs->mr, &addr.mach );
    AddrSection( &addr, OVL_MAP_CURR );
    return( addr );
}


static memory_delta *NewMemDelta( address addr, unsigned bytes )
{
    memory_delta *new;

    _Alloc( new, (sizeof( *new )-1) + 2*bytes );
    if( new == NULL ) return( NULL );
    memset( new, 0, sizeof( *new ) );
    new->addr = addr;
    new->next = StateCurr->mem;
    new->size = bytes;
    StateCurr->mem = new;
    return( new );
}

static walk_result FindMemRefs( address a, mad_type_handle th,
                        mad_memref_kind mk, void *d )
{
    mad_type_info       mti;
    memory_delta        *new;
    unsigned            bytes;


    d = d;
    if( (mk & (MMK_VOLATILE|MMK_WRITE)) == 0 ) return( WR_CONTINUE );
    MADTypeInfo( th, &mti );
    bytes = mti.b.bits / BITS_PER_BYTE;
    if( bytes > MAX_DELTA_BYTES ) return( WR_STOP ); /* don't fit */
    new = NewMemDelta( a, bytes );
    if( new == NULL ) return( WR_STOP );
    if( mk & MMK_VOLATILE ) new->_volatile = TRUE;
    /* can keep in target form */
    new->size = ProgPeek( a, &new->data[0], bytes );
    return( WR_CONTINUE );
}

void SetMemBefore( bool tracing )
{
    if( !tracing ) {
        StateCurr->lost_mem_state = TRUE;
        return;
    }
    if( !TraceModifications( FindMemRefs, NULL ) ) {
        StateCurr->lost_mem_state = TRUE;
    }
}


void SetMemAfter( bool tracing )
{
    memory_delta        **owner;
    memory_delta        *curr;

    if( !tracing ) return;
    if( StateCurr->lost_mem_state ) return;
    owner = &StateCurr->mem;
    for( ;; ) {
        curr = *owner;
        if( curr == NULL ) break;
        if( curr->after_set ) break;
        if( ProgPeek( curr->addr, &curr->data[curr->size], curr->size ) != curr->size ) {
            StateCurr->lost_mem_state = TRUE;
        }
        curr->after_set = TRUE;
        if( !curr->_volatile
          && memcmp( &curr->data[0], &curr->data[curr->size], curr->size ) == 0 ) {
            /* don't need the sucker */
            *owner = curr->next;
            _Free( curr );
        } else {
            owner = &curr->next;
        }
    }
}

static void FreeMemDelta( save_state *state )
{
    memory_delta        *mem;
    memory_delta        *junk;

    mem = state->mem;
    while( mem != NULL ) {
        junk = mem;
        mem = mem->next;
        _Free( junk );
    }
    state->mem = NULL;
}

static void FreeState( save_state *state )
{
    state->prev->next = state->next;
    state->next->prev = state->prev;
    _Free( state->s.ovl );
    FreeMemDelta( state );
    _Free( state );
    if( DbgRegs == &state->s ) DbgRegs = NULL;
    if( PrevRegs == &state->s ) PrevRegs = NULL;
    --NumStateEntries;
}

unsigned        CurrRegSize = 0;

void ResizeRegData( void )
{
    unsigned            new_size;
    bool                found_dbgregs;
    save_state          *state;
    save_state          *old;
    machine_state       *ms;

    new_size = MADRegistersSize();
    if( new_size > CurrRegSize ) {
        /* should I zero out new memory? */
        /* Yes, it is neccessary for debug version */
        found_dbgregs = FALSE;
        state = StateCurr;
        for( ;; ) {
            old = state;
            _Alloc( state, sizeof( save_state ) + new_size );
            if( state == NULL ) {
                ReportMADFailure( MS_NO_MEM );
                new_size = CurrRegSize;
            } else {
                memset( state, 0, sizeof( save_state ) + new_size );
                memcpy( state, old, sizeof( save_state ) );
            }
            if( old == StateCurr ) StateCurr = state;
            if( old == StateLast ) StateLast = state;
            state->prev->next = state;
            state->next->prev = state;
            if( &old->s == DbgRegs ) {
                found_dbgregs = TRUE;
                DbgRegs = &state->s;
            }
            if( &old->s == PrevRegs ) PrevRegs = &state->s;
            state = state->next;
            if( state == StateCurr ) break;
        }
        if( !found_dbgregs ) {
            /* just a machine state on it's own */
            ms = DbgRegs;
            _Alloc( ms, sizeof( machine_state ) + new_size );
            if( ms == NULL ) {
                ReportMADFailure( MS_NO_MEM );
                new_size = CurrRegSize;
            } else {
                memset( ms, 0, sizeof( machine_state ) + new_size );
                memcpy( ms, DbgRegs, sizeof( machine_state ) );
                if( DbgRegs == PrevRegs ) PrevRegs = ms;
                DbgRegs = ms;
            }
        }
        CurrRegSize = new_size;
    }
}

static save_state *AllocState( void )
{
    unsigned    size;
    save_state  *state;

    size = sizeof( *state ) + CurrRegSize;
    _Alloc( state, size );
    if( state == NULL ) return( NULL );
    memset( state, 0, size );
    if( OvlSize != 0 ) {
        _Alloc( state->s.ovl, OvlSize );
        if( state->s.ovl == NULL ) {
            _Free( state );
            return( NULL );
        }
    }
    ++NumStateEntries;
    return( state );
}


void ClearMachState( void )
{
    save_state  *state, *next;

    state = StateCurr->next->next;
    while( state != StateCurr ) {
        next = state->next;
        FreeState( state );
        state = next;
    }
    OvlSize = 0;
    state = StateCurr;
    do {
        _Free( state->s.ovl );
        state->s.ovl = NULL;
        state->s.tid = 1;
        state->s.mad = MAD_NIL;
        FreeMemDelta( state );
        state = state->next;
    } while( state != StateCurr );
    PrevRegs = DbgRegs = &StateCurr->s;
    StateLast = StateCurr;
    AlreadyWarnedUndo = FALSE;
}


void InitMachState( void )
{
    save_state  *other;

    StateCurr = AllocState();
    StateCurr->action = ACTION_NONE;
    other = AllocState();
    other->action = ACTION_NONE;
    if( StateCurr == NULL || other == NULL ) {
        StartupErr( LIT( ERR_NO_MEMORY ) );
    }
    other->next = StateCurr;
    other->prev = StateCurr;
    StateCurr->next = other;
    StateCurr->prev = other;

    ClearMachState();
}


void FiniMachState( void )
{
    FiniOvlState();
    ClearMachState();
    FreeState( StateCurr->next );
    FreeState( StateCurr );
    StateCurr = NULL;
    StateLast = NULL;
}


void SetupMachState( void )
{
    save_state      *state;

    OvlSize = RemoteOvlSectSize();
    if( OvlSize == 0 ) return;
    state = StateCurr;
    do {
        _Free( state->s.ovl );
        _Alloc( state->s.ovl, OvlSize );
        if( state->s.ovl == NULL ) {
            ReleaseProgOvlay( FALSE );
            Error( ERR_NONE, LIT( ERR_NO_OVL_STATE ) );
        }
        state = state->next;
    } while( state != StateCurr );
    if( !InitOvlState() ) {
        ReleaseProgOvlay( FALSE );
        Error( ERR_NONE, LIT( ERR_NO_OVL_STATE ) );
    }
    SectTblRead( DbgRegs );
}

void CopyMachState( machine_state *from, machine_state *to )
{
    to->tid = from->tid;
    to->mad = from->mad;
    memcpy( &to->mr, &from->mr, CurrRegSize );
    if( to->ovl != NULL ) memcpy( to->ovl, from->ovl, OvlSize );
}

machine_state *AllocMachState( void )
{
    machine_state   *state;
    unsigned        state_size;

    state_size = sizeof( machine_state ) + CurrRegSize;
    state = DbgMustAlloc( state_size );
    memset( state, 0, sizeof( *state ) );
    if( OvlSize != 0 ) {
        _Alloc( state->ovl, OvlSize );
        if( state->ovl == NULL ) {
            _Free( state );
            state = NULL;
            Error( ERR_NONE, LIT( ERR_NO_MEMORY ) );
        }
    }
    return( state );
}

void FreeMachState( machine_state *state )
{
    _Free( state->ovl );
    _Free( state );
}


void CollapseMachState( void )
{
    machine_state       *curr, *prev;

    if( StateCurr == NULL ) return;
    if( !StateCurr->valid ) return;
    if( StateCurr->mem != NULL ) return;
    if( StateCurr->prev == NULL ) return;
    if( StateCurr->prev->mem != NULL ) return;
    if( StateCurr->s.mad != StateCurr->prev->s.mad ) return;
    curr = &StateCurr->s;
    if( !StateCurr->prev->valid ) return;
    prev = &StateCurr->prev->s;
    if( prev->tid != curr->tid ) return;
    if( memcmp( &prev->mr, &curr->mr, CurrRegSize ) != 0 ) return;
    StateCurr->valid = FALSE;
    StateCurr = StateCurr->prev;
    StateLast = StateCurr;
    PrevRegs = DbgRegs = &StateCurr->s;
    if( StateCurr->prev->valid ) {
        PrevRegs = &StateCurr->prev->s;
    }
}

bool CheckStackPos( void )
{
    if( StackPos != 0 ) {
        if( _IsOff( SW_IN_REPLAY_MODE ) ) {
            if( !DlgUpTheStack() ) return( FALSE );
        }
        SetRegIP( Context.execution );
        SetRegSP( Context.stack );
        SetRegBP( Context.frame );
        StackPos = 0;
    }
    return( TRUE );
}


bool AdvMachState( int action )
{
    save_state          *new;
    bool                warn;

    warn = FALSE;
    new = StateCurr;
    while( new != StateLast ) {
        new = new->next;
        if( new->lost_mem_state ) warn = TRUE;
    }
    if( StateCurr != StateLast && _IsOff( SW_IN_REPLAY_MODE ) ) {
        if( !DlgBackInTime( warn ) ) return( FALSE );
    }
    new = StateCurr;
    while( new != StateLast ) {
        new = new->next;
        new->valid = FALSE;
    }
    PrevRegs = DbgRegs;
    StateCurr->valid = TRUE;
    if( SysConfig.mad != MAD_MSJ ) {
        if( !StateCurr->next->valid ) {
            StateCurr = StateCurr->next;
        } else {
            new = AllocState();
            new->action = action;
            if( new == NULL ) {
                StateCurr = StateCurr->next;
            } else {
                new->prev = StateCurr;
                new->next = StateCurr->next;
                new->next->prev = new;
                StateCurr->next = new;
                StateCurr = new;
            }
        }
    }
    StateCurr->valid = TRUE;
    DbgRegs = &StateCurr->s;
    StateLast = StateCurr;

    FreeMemDelta( StateCurr );
    CopyMachState( PrevRegs, DbgRegs );
    AlreadyWarnedUndo = FALSE;
    return( TRUE );
}


unsigned ChangeMem( address addr, void * to, unsigned size )
{
    memory_delta        *curr;
    unsigned_8          *p;
    unsigned            amount;
    unsigned            left;

    p = to;
    left = size;
    for( ;; ) {
        if( left == 0 ) break;
        amount = left;
        if( amount > MAX_DELTA_BYTES ) amount = MAX_DELTA_BYTES;
        curr = NewMemDelta( addr, amount );
        if( curr == NULL ) {
            StateCurr->lost_mem_state = TRUE;
        } else {
            if( ProgPeek( addr, &curr->data[0], amount ) != amount ) {
                StateCurr->lost_mem_state = TRUE;
            }
            curr->after_set = TRUE;
            memcpy( &curr->data[amount], p, amount );
        }
        addr.mach.offset += amount;
        p += amount;
        left -= amount;
    }
    addr.mach.offset -= size;
    return( ProgPoke( addr, to, size ) );
}

static void ReverseMemList( save_state * state )
{
    memory_delta        *curr;
    memory_delta        *next;
    memory_delta        *reverse;

    curr = state->mem;
    reverse = NULL;
    while( curr != NULL ) {
         next = curr->next;
         curr->next = reverse;
         reverse = curr;
         curr = next;
    }
    state->mem = reverse;
}


unsigned UndoLevel( void )
{
    int         count;
    save_state  *state;

    count = 0;
    for( state = StateCurr; state != StateLast; state = state->next ) {
        ++count;
    }
    return( count );
}

static unsigned RedoLevel( void )
{
    int         count;
    save_state  *state;

    count = 0;
    state = StateCurr;
    for( ;; ) {
        state = state->prev;
        if( state == StateLast ) break;
        if( !state->valid ) break;
        ++count;
    }
    return( count );
}


#ifdef DEADCODE
bool MachStateInfoRelease( void )
{
    save_state  *state, *next;
    bool        freed;

    state = StateCurr;
    freed = FALSE;
    do {
        next = state->next;
        if( !state->valid ) {
            FreeState( state );
            freed = TRUE;
        }
        state = next;
    } while( state != StateCurr );
    if( !freed ) {
        if( StateLast->next != StateCurr ) {;
            FreeState( StateLast->next );
            freed = TRUE;
        }
    }
    if( PrevRegs == NULL ) PrevRegs = &StateCurr->s;
    if(  DbgRegs == NULL )  DbgRegs = &StateCurr->s;
    return( freed );
}
#endif


typedef struct {
    location_context    lc;
    int                 targ;
    int                 curr;
    bool                success;
} move_info;

static CALL_CHAIN_RTN CheckOneLevel;

void SetStackPos( location_context *lc, int pos )
{
    StackPos = pos;
    Context.execution = lc->execution;
    Context.stack = lc->stack;
    Context.frame = lc->frame;
    Context.up_stack_level = lc->up_stack_level;
    Context.maybe_have_frame = lc->maybe_have_frame;
    Context.have_frame = lc->have_frame;
    SetCodeLoc( GetRegIP() );
    DbgUpdate( UP_STACKPOS_CHANGE );
}

void MoveStackPos( int by )
{
    move_info   info;

    if( StackPos + by > 0 ) {
        Warn( LIT( Bottom_Of_Stack ) );
        return;
    }
    info.targ = StackPos + by;
    info.curr = 0;
    info.success = FALSE;
    WalkCallChain( CheckOneLevel, &info );
    if( info.success ) {
        SetStackPos( &info.lc, info.targ );
    } else {
        Warn( LIT( Top_Of_Stack ) );
    }
}

int GetStackPos( void )
{
    return( StackPos );
}

void PosMachState( int rel_pos )
{
    save_state          *new;
    int                 adv,bkup;
    int                 i;
    save_state          *curr;
    memory_delta        *mem;
    int                 stack_pos;
    bool                lost_mem_state;

    new = StateCurr;
    if( rel_pos == 0 ) return;
    if( rel_pos > 0 ) {
        adv = UndoLevel();
        if( rel_pos > adv ) {
            Warn( LIT( No_More_Undos ) );
            rel_pos = adv;
        }
        for( i = rel_pos; i > 0; --i ) {
            new = new->next;
        }
    } else {
        bkup = RedoLevel();
        if( -rel_pos > bkup ) {
            Warn( LIT( No_More_Undos ) );
            rel_pos = -bkup;
        }
        for( i = rel_pos; i < 0; ++i ) {
            new = new->prev;
        }
    }
    if( new->s.tid != DbgRegs->tid ) {
        if( FindThread( new->s.tid ) == NULL ) {
            Warn( LIT( Thread_Not_Exist ) );
            rel_pos = 0;
        } else {
            RemoteSetThread( new->s.tid );
        }
    }
    if( rel_pos == 0 ) return;
    stack_pos = GetStackPos();
    MoveStackPos( -stack_pos );
    if( rel_pos > 0 ) {
        curr = StateCurr;
        do {
            curr = curr->next;
            ReverseMemList( curr );
            for( mem = curr->mem; mem != NULL; mem = mem->next ) {
                ProgPoke( mem->addr, &mem->data[mem->size], mem->size );
            }
            ReverseMemList( curr );
        } while( curr != new );
    } else {
        lost_mem_state = FALSE;
        for( curr = StateCurr; curr != new; curr = curr->prev ) {
            if( curr->lost_mem_state ) {
                lost_mem_state = TRUE;
            }
        }
        curr = StateCurr;
        if( lost_mem_state ) {
            if( AlreadyWarnedUndo ) {
                lost_mem_state = FALSE;
            } else if( DlgIncompleteUndo() ) {
                lost_mem_state = FALSE;
                AlreadyWarnedUndo = TRUE;
            }
        }
        if( !lost_mem_state ) {
            do {
                for( mem = curr->mem; mem != NULL; mem = mem->next ) {
                    ProgPoke( mem->addr, &mem->data[0], mem->size );
                }
                curr = curr->prev;
            } while( curr != new );
        }
    }
    StateCurr = curr;
    PrevRegs = DbgRegs;
    DbgRegs = &StateCurr->s;
    InitLC( &Context, TRUE );
    SetCodeLoc( GetRegIP() );
    DbgUpdate(UP_MEM_CHANGE | UP_CSIP_JUMPED | UP_CSIP_CHANGE | UP_REG_CHANGE | UP_THREAD_STATE);
    MoveStackPos( stack_pos );
    if( StateCurr == StateLast ) {
        AlreadyWarnedUndo = FALSE;
    }
}

void LastMachState( void )
{
    PosMachState( UndoLevel() );
}

static bool CheckOneLevel( call_chain_entry *entry, void *_info )
{
    move_info  *info = _info;

    if( info->curr == info->targ ) {
        info->success = TRUE;
        info->lc = entry->lc;
        return( FALSE );
    } else {
        info->curr--;
        return( TRUE );
    }
}


void LastStackPos( void )
{
    if( StackPos != 0 ) {
        MoveStackPos( -StackPos );
    }
}


/************************ command language stuff ***********************/

void ProcRegister( void )
{
    int         val;
    unsigned    old;

    old = SetCurrRadix( 10 );
    val = (int) ReqExpr();
    ReqEOC();
    if( val != 0 ) {
        PosMachState( val );
    }
    SetCurrRadix( old );
}


void ProcUndo( void )
{
    int         val;
    unsigned    old;

    old = SetCurrRadix( 10 );
    val = (int) ReqExpr();
    SetCurrRadix( old );
    ReqEOC();
    if( val != 0 ) {
        PosMachState( -val );
    }
}


char *GetActionString( int action )
/*********************************/
{
    switch( action ) {
    case ACTION_EXECUTE:
        return( LIT( Str_ACTION_EXECUTE ) );
    case ACTION_ASSIGNMENT:
        return( LIT( Str_ACTION_ASSIGNMENT ) );
    case ACTION_THREAD_CHANGE:
        return( LIT( Str_ACTION_THREAD_CHANGE ) );
    case ACTION_MODIFY_IP:
        return( LIT( Str_ACTION_MODIFY_IP ) );
    case ACTION_MODIFY_MEMORY:
        return( LIT( Str_ACTION_MODIFY_MEMORY ) );
    case ACTION_MODIFY_REGISTER:
        return( LIT( Str_ACTION_MODIFY_REGISTER ) );
    case ACTION_MODIFY_VARIABLE:
        return( LIT( Str_ACTION_MODIFY_VARIABLE ) );
    case ACTION_NONE:
        return( NULL );
    default:
        return( "" );
    }
}

char *GetUndoString( void )
/*************************/
{
    if( StateCurr == NULL ) return( NULL );
    return( GetActionString( StateCurr->action ) );
}

char *GetRedoString( void )
/*************************/
{
    if( StateCurr == NULL ) return( NULL );
    if( UndoLevel() == 0 ) return( GetActionString( ACTION_NONE ) );
    return( GetActionString( StateCurr->next->action ) );
}

void ProcStackPos( void )
{
    int         val;
    unsigned    old;

    old = SetCurrRadix( 10 );
    val = (int) ReqExpr();
    SetCurrRadix( old );
    ReqEOC();
    MoveStackPos( val - StackPos );
}


void GoHome( void )
{
    LastStackPos();
    LastMachState();
    DbgUpdate( UP_CSIP_CHANGE );
}

/*
 * ParseRegSet - create a register set location list
 */

struct parsed_regs {
    struct parsed_regs  *prev;
    mad_reg_info        *ri;
};

void ParseRegSet( bool multiple, location_list *ll, dip_type_info *ti )
{
    lookup_item         li;
    struct parsed_regs  *list, *new;
    mad_reg_info        *ri;
    location_list       reg_loc;

    li.scope.start = NULL;
    li.type = ST_NONE;
    li.case_sensitive = FALSE;
    list = NULL;
    for( ;; ) {
        li.name.start = NamePos();
        li.name.len = NameLen();
        ri = LookupRegName( NULL, &li );
        if( ri == NULL ) break;
        Scan();
        _AllocA( new, sizeof( *new ) );
        /* build the list backwards because the location list wants
           to be little endian */
        new->ri = ri;
        new->prev = list;
        list = new;
        if( !multiple ) break;
    }
    ti->size = 0;
    ll->num = 0;
    ll->flags = 0;
    while( list != NULL ) {
        ti->size += list->ri->bit_size / BITS_PER_BYTE;
        RegLocation( DbgRegs, list->ri, &reg_loc );
        LocationAppend( ll, &reg_loc );
        list = list->prev;
    }
    //MAD: hmmm.... get typing info out of list->ri->type?
    switch( ti->size ) {
    case 1:
    case 2:
    case 4:
        ti->kind = TK_INTEGER;
        ti->modifier = TM_UNSIGNED;
        break;
    case 6:
        ti->kind = TK_ADDRESS;
        ti->modifier = TM_FAR;
        break;
    case 8:
        ti->kind = TK_REAL;
        ti->modifier = TM_NONE;
        break;
    default:
        ti->kind = TK_NONE;
        ti->modifier = TM_NONE;
        break;
    }
}

void RegValue( item_mach *value, const mad_reg_info *reginfo, machine_state *mach )
{
    location_list               src_ll;
    location_list               dst_ll;
    unsigned                    size;

    size = ( reginfo->bit_size + (BITS_PER_BYTE-1) ) / BITS_PER_BYTE;
    RegLocation( mach, reginfo, &src_ll );
    LocationCreate( &dst_ll, LT_INTERNAL, value );
    LocationAssign( &dst_ll, &src_ll, size, FALSE );
}

void RegNewValue( const mad_reg_info *reginfo,
                      item_mach *new_val,
                      mad_type_handle type )
{
    char                        *p;
    location_list               dst_ll,src_ll;
    dip_type_info               dst_ti,src_ti;
    unsigned                    max;

    if( !AdvMachState( ACTION_MODIFY_REGISTER ) ) return;
    RegLocation( DbgRegs, reginfo, &dst_ll );
    MadTypeToDipTypeInfo( reginfo->type, &dst_ti );
    PushLocation( &dst_ll, &dst_ti );
    LocationCreate( &src_ll, LT_INTERNAL, new_val );
    MadTypeToDipTypeInfo( type, &src_ti );
    PushLocation( &src_ll, &src_ti );
    DoAssign();
    p = StrCopy( GetCmdName( CMD_ASSIGN ), TxtBuff );
    p = StrCopy( " ", p );
    p += MADRegFullName( reginfo, ".", TXT_LEN, p );
    p = StrCopy( "=", p );
    max = TXT_LEN - ( p - TxtBuff );
    MADTypeHandleToString( CurrRadix, type, new_val, p, &max );
    p += max;
    RecordEvent( TxtBuff );
    CollapseMachState();
    DbgUpdate( UP_REG_CHANGE );
}
