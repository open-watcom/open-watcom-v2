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
#include "dbglit.h"
#include "dbginfo.h"
#include "dbgtoggl.h"
#include "dbgmem.h"
#include "dbginp.h"
#include "dbgrep.h"
#include "dbgevent.h"
#include "dui.h"
#include <string.h>

extern void             RecordPointStart(void);
extern void             RecordNewProg(void);
extern cmd_list         *AllocCmdList( char *start, unsigned len );
extern void             FreeCmdList( cmd_list *cmds );
extern void             PointFini();
extern void             PushCmdList( cmd_list *cmds );
extern void             TypeInpStack( input_type set );
extern int              GetStackPos();
extern char             *Format( char *buff, char *fmt, ... );
extern char             *GetCmdName( int index );
extern unsigned         UndoLevel();
extern address          GetRegIP();
extern void             CreateInvokeFile( char *name, void (*rtn)(void) );
extern void             InvokeAFile(char*);
extern bool             ScanEOC();
extern bool             ScanItem( bool blank_delim, char **start, unsigned *len );
extern void             ReqEOC();
extern char             *StrCopy( char *src, char *dest );
extern char             *StrAddr( address *addr, char *p ,unsigned);
extern unsigned         NewCurrRadix( unsigned rad );
extern unsigned         ReqExpr();
extern void             PushInpStack( void *handle, bool (*rtn)(), bool save_lang );
extern char             *ScanPos();
extern char             *DupStr( char *str );
extern void             UnAsm( address addr, unsigned, char *buff );
extern char             *CopySourceLine( cue_handle *ch );
extern char             *GetEventAddress( event_record *ev );
extern void             ReplayTo( event_record *ev );
extern void             ProcInput(void);
extern void             DbgUpdate( update_list );

extern char             *TxtBuff;
extern address          NilAddr;
extern unsigned char  CurrRadix;

event_record *EventList;
static event_record **LastOwner;

static event_record **FindOwner( event_record *of )
{
    event_record **owner;
    owner = &EventList;
    while( *owner != of ) {
        owner = &((*owner)->next);
    }
    return( owner );
}

static void FreeOneEvent( event_record *junk )
{
    event_record **owner;

    owner = FindOwner( junk );
    *owner = junk->next;
    FreeCmdList( junk->cmd );
    _Free( junk->addr_string );
    _Free( junk->cue );
    _Free( junk );
}

void RecordInit()
{
    EventList = NULL;
}

void RecordFini()
{
    _SwitchOff( SW_HAD_ASYNCH_EVENT );
    while( EventList != NULL ) {
        FreeOneEvent( EventList );
    }
}

void ShowReplay()
{
    event_record        *ev;
    for( ev = EventList; ev != NULL; ev = ev->next ) {
        Format( TxtBuff, "%s %d {%s}", GetCmdName( CMD_RECORD ), ev->rad, ev->cmd->buff );
        DUIDlgTxt( TxtBuff );
    }
}

bool OkToSaveReplay()
{
    if( _IsOff( SW_HAD_ASYNCH_EVENT ) ) return( TRUE );
    if( DUIAskIfAsynchOk() ) return( TRUE );
    return( FALSE );
}

void SaveReplayToFile( char *name )
{
    CreateInvokeFile( name, ShowReplay );
}

void RestoreReplayFromFile( char *name )
{
    RecordFini();
    InvokeAFile( name );
    ProcInput();
    ReplayTo( NULL );
}

void ProcRecord()
{
    char        *start;
    unsigned    len;
    unsigned    rad;
    unsigned    old;

    old = NewCurrRadix( 10 );
    rad = ReqExpr();
    NewCurrRadix( old );
    if( !ScanEOC() ) {
        if( ScanItem( FALSE, &start, &len ) ) {
            ReqEOC();
            old = NewCurrRadix( rad );
            AddEvent( start, len, NilAddr );
            NewCurrRadix( old );
        }
    }
}

void RecordStart()
{
    RecordFini();
    RecordNewProg();
    RecordPointStart();
}

OVL_EXTERN bool DoneRadix( void *parm, inp_rtn_action action )
{
    switch( action ) {
    case INP_RTN_INIT:
    case INP_RTN_FINI:
        return( TRUE );
    case INP_RTN_EOL:
        NewCurrRadix( (unsigned)parm );
        return( FALSE );
    default:
        return( FALSE );
    }
}


static void PushRadChange( unsigned rad )
{
    PushInpStack( (void *)rad, DoneRadix, FALSE );
    TypeInpStack( INP_NO_CMD );
}

void ReplayTo( event_record *ev )
{
    event_record *prev;

    if( ev != NULL && ev->after_asynch && !DUIAskIfAsynchOk() ) return;
    PointFini();
    if( EventList != NULL ) {
        if( ev == NULL ) {
            for( ev = EventList; ev->next != NULL; ev = ev->next ) ;
        }
        for( ;; ) {
            PushCmdList( ev->cmd );
            TypeInpStack( INP_REPLAYED );
            PushRadChange( ev->rad );
            TypeInpStack( INP_REPLAYED );
            if( ev == EventList ) break;
            for( prev = EventList; prev->next != ev; prev = prev->next ) ;
            ev = prev;
        }
    }
    PushRadChange( CurrRadix );
    TypeInpStack( INP_REPLAYED );
    RecordFini();
}


void CheckEventRecorded()
{
    if( _IsOn( SW_EXECUTE_ABORTED ) ) {
        while( *LastOwner != NULL ) {
            FreeOneEvent( *LastOwner );
        }
    }
}

static void AddEvent( char *start, unsigned len, address ip )
{
    event_record        **owner;
    event_record        *new;

    owner = FindOwner( NULL );
    _Alloc( new, sizeof( *new ) );
    new->addr_string = NULL;
    new->cue = NULL;
    new->next = NULL;
    new->cmd = AllocCmdList( start, len );
    new->ip = ip;
    new->after_asynch = FALSE;
    new->rad = CurrRadix;
    if( _IsOn( SW_HAD_ASYNCH_EVENT ) ) {
        if( _IsOn( SW_EVENT_RECORDED_SINCE_ASYNCH ) ) {
            new->after_asynch = TRUE;
        }
    }
    *owner = new;
}

void RecordEvent( char *p )
{
    int                 stackpos;
    unsigned            undo;
    char                buff[40];

    if( _IsOn( SW_HAD_ASYNCH_EVENT ) ) {
        _SwitchOn( SW_EVENT_RECORDED_SINCE_ASYNCH );
    }
    _SwitchOff( SW_EXECUTE_ABORTED );
    if( _IsOn( SW_CMD_INTERACTIVE ) ) {
        LastOwner = FindOwner( NULL );
        undo = UndoLevel();
        if( undo != 0 ) {
            Format( buff, "%s %d", GetCmdName( CMD_UNDO ), undo );
            AddEvent( buff, strlen( buff ), GetRegIP() );
        }
        stackpos = GetStackPos();
        if( stackpos != 0 ) {
            Format( buff, "%s %d", GetCmdName( CMD_STACKPOS ), stackpos );
            AddEvent( buff, strlen( buff ), GetRegIP() );
        }
        AddEvent( p, strlen( p ), GetRegIP() );
        DbgUpdate( UP_EVENT_CHANGE );
    }
}


void RecordAsynchEvent()
{
    _SwitchOn( SW_HAD_ASYNCH_EVENT );
    _SwitchOff( SW_EVENT_RECORDED_SINCE_ASYNCH );
}

void RecordGo( char *p )
{
    if( _IsOn( SW_HAD_ASYNCH_EVENT ) && _IsOff( SW_EVENT_RECORDED_SINCE_ASYNCH ) ) {
        _SwitchOff( SW_HAD_ASYNCH_EVENT );
    }
    RecordEvent( p );
}

void RecordCommand( char *startpos, int cmd )
{
    char        *endpos;
    char        *p;

    endpos = ScanPos();
    p = StrCopy( GetCmdName( cmd ), TxtBuff );
    *p++ = ' ';
    memcpy( p, startpos, endpos-startpos );
    p += endpos-startpos;
    *p = '\0';
    RecordEvent( TxtBuff );
}

void SetRecord( bool on )
{
    _SwitchSet( SW_CMD_INTERACTIVE, on );
}

char *GetEventAddress( event_record *ev )
{
    char        *p;

    if( IS_NIL_ADDR( ev->ip ) ) {
        p = StrCopy( LIT( Quest_Marks ), TxtBuff );
    } else {
        p = StrAddr( &ev->ip, TxtBuff, TXT_LEN );
    }
    StrCopy( ":", p );
    return( TxtBuff );
}
