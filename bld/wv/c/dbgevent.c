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
* Description:  Record or replay debugger events.
*
****************************************************************************/


#include <stdlib.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbglit.h"
#include "dbgmem.h"
#include "dbgrep.h"
#include "dbgevent.h"
#include "dui.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgutil.h"


extern void             RecordPointStart( void );
extern void             RecordNewProg( void );
extern void             PointFini( void );
extern int              GetStackPos( void );
extern char             *GetCmdName( int index );
extern unsigned         UndoLevel( void );
extern address          GetRegIP( void );
extern void             CreateInvokeFile( const char *name, void(*rtn)(void) );
extern void             InvokeAFile( const char * );
extern unsigned         ReqExpr( void );
extern char             *CopySourceLine( cue_handle *ch );
extern char             *GetEventAddress( event_record *ev );
extern void             ReplayTo( event_record *ev );
extern void             ProcInput( void );
extern void             DbgUpdate( update_list );


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

void RecordInit( void )
{
    EventList = NULL;
}

void RecordFini( void )
{
    _SwitchOff( SW_HAD_ASYNCH_EVENT );
    while( EventList != NULL ) {
        FreeOneEvent( EventList );
    }
}

void ShowReplay( void )
{
    event_record        *ev;
    for( ev = EventList; ev != NULL; ev = ev->next ) {
        Format( TxtBuff, "%s %d {%s}", GetCmdName( CMD_RECORD ), ev->rad, ev->cmd->buff );
        DUIDlgTxt( TxtBuff );
    }
}

bool OkToSaveReplay( void )
{
    if( _IsOff( SW_HAD_ASYNCH_EVENT ) ) return( TRUE );
    if( DUIAskIfAsynchOk() ) return( TRUE );
    return( FALSE );
}

void SaveReplayToFile( const char *name )
{
    CreateInvokeFile( name, ShowReplay );
}

void RestoreReplayFromFile( const char *name )
{
    RecordFini();
    InvokeAFile( name );
    ProcInput();
    ReplayTo( NULL );
}

static void AddEvent( const char *start, size_t len, address ip )
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

void ProcRecord( void )
{
    const char  *start;
    size_t      len;
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

void RecordStart( void )
{
    RecordFini();
    RecordNewProg();
    RecordPointStart();
}

OVL_EXTERN bool DoneRadix( inp_data_handle parm, inp_rtn_action action )
{
    switch( action ) {
    case INP_RTN_INIT:
    case INP_RTN_FINI:
        return( TRUE );
    case INP_RTN_EOL:
        NewCurrRadix( (unsigned)(pointer_int)parm );
        return( FALSE );
    default:
        return( FALSE );
    }
}


static void PushRadChange( unsigned rad )
{
    PushInpStack( (inp_data_handle)(pointer_int)rad, DoneRadix, FALSE );
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


void CheckEventRecorded( void )
{
    if( _IsOn( SW_EXECUTE_ABORTED ) ) {
        while( *LastOwner != NULL ) {
            FreeOneEvent( *LastOwner );
        }
    }
}

void RecordEvent( const char *p )
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


void RecordAsynchEvent( void )
{
    _SwitchOn( SW_HAD_ASYNCH_EVENT );
    _SwitchOff( SW_EVENT_RECORDED_SINCE_ASYNCH );
}

void RecordGo( const char *p )
{
    if( _IsOn( SW_HAD_ASYNCH_EVENT ) && _IsOff( SW_EVENT_RECORDED_SINCE_ASYNCH ) ) {
        _SwitchOff( SW_HAD_ASYNCH_EVENT );
    }
    RecordEvent( p );
}

void RecordCommand( const char *startpos, int cmd )
{
    const char  *endpos;
    char        *p;

    endpos = ScanPos();
    p = StrCopy( GetCmdName( cmd ), TxtBuff );
    *p++ = ' ';
    memcpy( p, startpos, endpos - startpos );
    p += endpos - startpos;
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
        p = StrCopy( LIT_ENG( Quest_Marks ), TxtBuff );
    } else {
        p = StrAddr( &ev->ip, TxtBuff, TXT_LEN );
    }
    StrCopy( ":", p );
    return( TxtBuff );
}
