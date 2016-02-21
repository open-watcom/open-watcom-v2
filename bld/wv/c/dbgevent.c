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
#include "dbgmain.h"
#include "dbginvk.h"
#include "dbgbrk.h"
#include "dbgpend.h"
#include "dbgparse.h"
#include "dbgprog.h"
#include "dbgreg.h"
#include "dbgupdt.h"


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
        Format( TxtBuff, "%s %d {%s}", GetCmdName( CMD_RECORD ), ev->radix, ev->cmd->buff );
        DUIDlgTxt( TxtBuff );
    }
}

bool OkToSaveReplay( void )
{
    if( _IsOff( SW_HAD_ASYNCH_EVENT ) ) return( true );
    if( DUIAskIfAsynchOk() ) return( true );
    return( false );
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
    new->after_asynch = false;
    new->radix = CurrRadix;
    if( _IsOn( SW_HAD_ASYNCH_EVENT ) ) {
        if( _IsOn( SW_EVENT_RECORDED_SINCE_ASYNCH ) ) {
            new->after_asynch = true;
        }
    }
    *owner = new;
}

void ProcRecord( void )
{
    const char  *start;
    size_t      len;
    mad_radix   new_radix;
    mad_radix   old_radix;

    old_radix = NewCurrRadix( 10 );
    new_radix = (mad_radix)ReqExpr();
    NewCurrRadix( old_radix );
    if( !ScanEOC() ) {
        if( ScanItem( false, &start, &len ) ) {
            ReqEOC();
            old_radix = NewCurrRadix( new_radix );
            AddEvent( start, len, NilAddr );
            NewCurrRadix( old_radix );
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
        return( true );
    case INP_RTN_EOL:
        NewCurrRadix( (mad_radix)(pointer_int)parm );
        return( false );
    default:
        return( false );
    }
}


static void PushRadixChange( mad_radix radix )
{
    PushInpStack( (inp_data_handle)(pointer_int)radix, DoneRadix, false );
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
            PushRadixChange( ev->radix );
            TypeInpStack( INP_REPLAYED );
            if( ev == EventList )
                break;
            for( prev = EventList; prev->next != ev; prev = prev->next )
                ;
            ev = prev;
        }
    }
    PushRadixChange( CurrRadix );
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

void RecordCommand( const char *startpos, wd_cmd cmd )
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
