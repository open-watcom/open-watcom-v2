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
#include <stddef.h>
#include <conio.h>
#include <malloc.h>
#include <dos.h>

#define INCL_DOSNMPIPES
#define INCL_DOSPROCESS
#include <wos2.h>
#define INCL_NOXLATE_DOS16
#include "namepipe.h"
#include "nmp.h"
#include "trperr.h"
#include <process.h>

HPIPE   BindHdl;

typedef struct a_pipe {
    HPIPE       read_hdl;
    HPIPE       write_hdl;
    HPIPE       conn_hdl;
    unsigned    present : 1;
    unsigned    connect_started : 1;
    unsigned    connected : 1;
} a_pipe;

typedef struct a_link {
    struct a_link       *next;
    char                *name;
    a_pipe              serv;
    a_pipe              trap;
} a_link;

a_link  *Links = NULL;

typedef struct threadinfo {
    a_link      *link;
    a_pipe      *me;
    a_pipe      *him;
} threadinfo;

APIRET PipeOpen( char *name, HPIPE *hdl )
{
    return( DosMakeNmPipe( name, hdl,
            NP_NOINHERIT | NP_NOWRITEBEHIND | NP_ACCESS_DUPLEX,
            NP_NOWAIT | NP_READMODE_MESSAGE | NP_TYPE_MESSAGE | 1,
            MAX_TRANS, MAX_TRANS, 0 ) );
}


static void DoClose( HPIPE hdl )
{
    DosDisConnectNmPipe( hdl );
    DosClose( hdl );
}


static int DoOpen( char *buff, char *end, char *suff, HPIPE *phdl )
{
    APIRET      rc;

    strcpy( end, suff );
    rc = PipeOpen( buff+1, phdl );
    if( rc != 0 ) return( FALSE );
    DosConnectNmPipe( *phdl );
    DosSetNmPHandState( *phdl, NP_WAIT | NP_READMODE_MESSAGE );
    return( TRUE );
}

int WrBuff( HPIPE hdl, char *data, int length )
{
    USHORT      bytes_written;

    if( DosWrite( hdl, data, length, &bytes_written ) != 0 ) {
        return( 0 );
    }
    return( bytes_written );
}


void Nack( HPIPE hdl )
{
    char        ch;

    ch = BIND_NACK;
    WrBuff( hdl, &ch, 1 );
}

void Ack( HPIPE hdl )
{
    char        ch;

    ch = BIND_ACK;
    WrBuff( hdl, &ch, 1 );
}

#define STACK_SIZE      8*1024

static void StartThread( void (*rtn)(void FAR *), a_link *link, a_pipe *me, a_pipe *him )
{
    void        *stack;
    threadinfo  *thread;

    stack = malloc( STACK_SIZE + sizeof( threadinfo ) );
    thread = (threadinfo*)((char*)stack + STACK_SIZE);
    thread->link = link;
    thread->me = me;
    thread->him = him;
    _beginthread( rtn, stack, STACK_SIZE, thread );
}

void JoinPipeThread( void FAR * _thread )
{
    threadinfo  *thread = (threadinfo *)_thread;
    char        buff[BUFF_LEN];
    APIRET      rc;
    USHORT      bytes_read;
    USHORT      bytes_written;
    SEL         sel_global;
    SEL         sel_local;
    a_pipe      *me;
    a_pipe      *him;
    a_link      *link;

    me = thread->me;
    him = thread->him;
    link = thread->link;
    rc = DosGetInfoSeg( &sel_global, &sel_local );
    rc = DosSetPrty( PRTYS_THREAD, PRTYC_TIMECRITICAL,
                     0, ((LINFOSEG FAR *)MK_FP( sel_local, 0 ))->tidCurrent );
    while( me->connected && him->connected ) {
        rc = DosRead( me->write_hdl, buff, BUFF_LEN, &bytes_read );
        if( rc != 0 || bytes_read == 0 ) break;
        rc = DosWrite( him->read_hdl, buff, bytes_read, &bytes_written );
        if( rc != 0 || bytes_written == 0 ) break;
    }
    DoClose( me->write_hdl );
    DoClose( him->read_hdl );
    me->connected = FALSE;
    me->connect_started = FALSE;
    _endthread();
}


static void FreeLink( a_link *junk )
{
    a_link      **owner;

    for( owner = &Links; *owner != junk; owner = &(*owner)->next ) ;
    *owner = junk->next;
    free( junk->name );
    free( junk );
}


void ConnectThread( void FAR * _thread )
{
    threadinfo  *thread = (threadinfo *)_thread;
    char        buff[BUFF_LEN];
    APIRET      rc;
    USHORT      bytes_read;
    HPIPE       hdl;
    a_pipe      *me;
    a_pipe      *him;
    a_link      *link;
    char        *end;

    me = thread->me;
    him = thread->him;
    link = thread->link;
    hdl = me->conn_hdl;
    DosConnectNmPipe( hdl ); // wait for other side to do its open
//    cprintf( "Connect thread going for (%d)\r\n", hdl );
    while( me->present ) {
        DosSleep( 100 );
        rc = DosRead( hdl, buff, BUFF_LEN, &bytes_read );
        if( rc != 0 || bytes_read == 0 ) break;
        switch( buff[0] ) {
        case END_CONNECT_SERV:
        case END_CONNECT_TRAP:
            me->connected = TRUE;
            if( him->connected ) {
                StartThread( JoinPipeThread, link, &link->serv, &link->trap );
                StartThread( JoinPipeThread, link, &link->trap, &link->serv );
            }
            if( buff[0] == END_CONNECT_SERV ) {
//              cprintf( "Ack to end connect Serv\r\n" );
            } else {
//              cprintf( "Ack to end connect Trap\r\n" );
            }
            Ack( hdl );
            break;
        case CONNECT_SERV:
            if( him->connect_started ) {
                me->connect_started = TRUE;
//              cprintf( "Ack to connect serv\r\n" );
                Ack( hdl );
            } else {
//              cprintf( "NAck to connect serv\r\n" );
                Nack( hdl );
            }
            break;
        case CONNECT_TRAP:
            if( him->present ) {
                me->connect_started = TRUE;
                end = buff + strlen( buff );
                DoOpen( buff, end, READ_TRAP_SUFF, &link->trap.read_hdl );
                DoOpen( buff, end, READ_SERV_SUFF, &link->serv.read_hdl );
                DoOpen( buff, end, WRITE_TRAP_SUFF, &link->trap.write_hdl );
                DoOpen( buff, end, WRITE_SERV_SUFF, &link->serv.write_hdl );
//              cprintf( "Ack to connect trap\r\n" );
                Ack( hdl );
            } else {
//              cprintf( "NAck to connect trap\r\n" );
                Nack( hdl );
            }
            break;
        case DISCO_SERV:
        case DISCO_TRAP:
            me->connect_started = FALSE;
//          cprintf( "Disco\r\n" );
            DosSleep( 100 );
            Ack( hdl );
            break;
        }
    }
//    cprintf( "Connect thread ending for (%d)\r\n", hdl );
    me->present = FALSE;
    DoClose( hdl );
    if( !him->present ) {
        FreeLink( link );
    }
    _endthread();
}


static a_link *FindLink( char *buff )
{
    a_link      *link;

    for( link = Links; link != NULL; link = link->next ) {
        if( stricmp( link->name, buff+1 ) == 0 ) {
            break;
        }
    }
    return( link );
}


void ProcessRequest( HPIPE hdl, char *buff )
{
    a_link      *link;
    char        *end;

    end = buff + strlen( buff );
    link = FindLink( buff );
    switch( buff[0] ) {
    case OPEN_SERV:
        if( link == NULL ) {
            link = malloc( sizeof( *link ) );
            link->name = strdup( buff+1 );
            link->next = Links;
            Links = link;
            link->trap.present = FALSE;
            link->serv.present = FALSE;
            link->trap.connected = FALSE;
            link->serv.connected = FALSE;
            link->trap.connect_started = FALSE;
            link->serv.connect_started = FALSE;
            if( DoOpen( buff, end, CONN_SERV_SUFF, &link->serv.conn_hdl ) ) {
                link->serv.present = TRUE;
                StartThread( ConnectThread, link, &link->serv, &link->trap );
//              cprintf( "Ack to Open Serv (%d)\r\n", link->serv.conn_hdl );
                Ack( hdl );
                break;
            }
        }
        Nack( hdl );
        break;
    case OPEN_TRAP:
        if( link && DoOpen( buff, end, CONN_TRAP_SUFF, &link->trap.conn_hdl ) ) {
            link->trap.present = TRUE;
            StartThread( ConnectThread, link, &link->trap, &link->serv );
//          cprintf( "Ack to Open Trap (%d)\r\n", link->trap.conn_hdl );
            Ack( hdl );
        } else {
            Nack( hdl );
        }
        break;
    case BIND_KILL:
        Ack( hdl );
        exit( 0 );
        break;
    }
}


static void CheckForTraffic( HPIPE hdl )
{
    char        buff[BUFF_LEN];
    USHORT      bytes_read;
    APIRET      rc;

    rc = DosConnectNmPipe( hdl );
    DosSleep( 500 );
    if( rc != 0 ) return;
    rc = DosRead( hdl, buff, BUFF_LEN-1, &bytes_read );
    if( rc == 0 && bytes_read != 0 ) {
        buff[ bytes_read ] = '\0';
    }
    if( bytes_read >= 1 ) {
        ProcessRequest( hdl, buff );
    }
    DosDisConnectNmPipe( hdl );
}

void Error( char *msg )
{
    mywrite( BHANDLE_STDERR, msg, strlen( msg ) );
    mywrite( BHANDLE_STDERR, "\r\n", 2 );
    exit( 1 );
}

int main( int argc, char *argv[] )
{
    APIRET      rc;
    char        req;
    bhandle     bind;

    if( argc > 1 && argv[1][0] == 'q' ) {
        bind = myopen( BINDERY );
        if( bind != BHANDLE_INVALID ) {
            req = BIND_KILL;
            mywrite( bind, &req, sizeof( req ) );
            myread( bind, &req, sizeof( req ) );
            myclose( bind );
        }
        exit( 0 );
    }
    rc = PipeOpen( BINDERY, &BindHdl );
    if( rc != 0 ) {
        if( rc == PIPE_ALREADY_OPEN ) Error( TRP_NMPBIND_running );
        Error( TRP_OS2_no_pipe );
    }
    for( ;; ) {
        CheckForTraffic( BindHdl );
    }
    return( 0 );
}

