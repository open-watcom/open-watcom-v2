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

/*
        BATSERV : OS/2 Spawn server for Viper
*/

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <process.h>
#include <ctype.h>
#include <sys/types.h>
#include <direct.h>

#define INCL_DOSNMPIPES
#define INCL_DOSPROCESS
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#define INCL_DOSEXCEPTIONS
#include <os2.h>

#include "batpipe.h"

HPIPE           LnkHdl;
HPIPE           RedirHdl;

char            RedirName[ PREFIX_LEN + MAX_NAME + 1];
unsigned        CmdStatus;
char            *CmdProc;
unsigned        ProcId;

static void RunCmd( char *cmd_name )
{
    char        cmd[MAX_TRANS+80];
    RESULTCODES res;
    UINT        rc;

    //NYI: spawning of DOS and PM programs
    //DOS programs can be handled by quoting the redirection
    //operators:
    //          prog 1^>out 2^>&1
    sprintf( cmd, "%s%c/C %s 0<NUL 1>%s 2>&1%c", CmdProc, 0, cmd_name, RedirName, 0 );
    rc = DosExecPgm( NULL, 0,           /* don't care about fail name */
                EXEC_ASYNCRESULT,       /* execflags */
                cmd,                    /* args */
                NULL,                   /* inherit environment */
                &res,                   /* result codes */
                CmdProc );              /* pgmname */
    if( rc == 0 ) {
        ProcId = res.codeTerminate;
    } else {
        ProcId = 0;
    }
}

static void SendStatus( unsigned long status )
{
    struct {
        unsigned char   cmd;
        unsigned long   stat;
    } buff;
    ULONG       dummy;

    buff.cmd = LNK_STATUS;
    buff.stat = status;
    DosWrite( LnkHdl, &buff, sizeof( buff ), &dummy );
}

static void ProcessConnection()
{
    char                buff[MAX_TRANS];
    ULONG               bytes_read;
    unsigned long       max;
    struct _AVAILDATA   BytesAvail;
    ULONG               PipeState;
    APIRET              rc;
    RESULTCODES         res;
    PID                 dummy;
    char                *dir;

    for( ;; ) {
        DosRead( LnkHdl, buff, sizeof( buff ), &bytes_read );
        if( bytes_read == 0 ) break;
        buff[bytes_read] = '\0';
        switch( buff[0] ) {
        case LNK_CWD:
            rc = 0;
            dir = &buff[1];
            if( isalpha( dir[0] ) && dir[1] == ':' ) {
                rc = DosSetDefaultDisk( toupper( dir[0] ) - ('A' - 1) );
                dir += 2;
            }
            if( rc == 0 && dir[0] != '\0' ) {
                rc = DosSetCurrentDir( dir );
            }
            SendStatus( rc );
            break;
        case LNK_RUN:
            DosSetNPHState( RedirHdl, NP_NOWAIT | NP_READMODE_BYTE );
            DosConnectNPipe( RedirHdl );
            DosSetNPHState( RedirHdl, NP_WAIT | NP_READMODE_BYTE );
            RunCmd( &buff[1] );
            break;
        case LNK_QUERY:
            max = *(unsigned long *)&buff[1];
            if( max > sizeof( buff ) ) max = sizeof( buff );
            --max;
            rc = DosPeekNPipe(RedirHdl, buff, 0, &bytes_read,
                        &BytesAvail, &PipeState );
            if( rc == 0 && BytesAvail.cbpipe != 0 ) {
                DosRead( RedirHdl, &buff[1], max, &bytes_read );
                buff[0] = LNK_OUTPUT;
                DosWrite( LnkHdl, buff, bytes_read + 1, &bytes_read );
            } else {
                rc = DosWaitChild( DCWA_PROCESS, DCWW_NOWAIT, &res,
                                        &dummy, ProcId );
                if( rc != ERROR_CHILD_NOT_COMPLETE ) {
                    DosDisConnectNPipe( RedirHdl );
                    SendStatus( res.codeResult );
                    ProcId = 0;
                } else {
                    /* let someone else run */
                    DosSleep( 1 );
                    buff[0] = LNK_NOP;
                    DosWrite( LnkHdl, buff, 1, &bytes_read );
                }
            }
            break;
        case LNK_CANCEL:
            DosSendSignalException( ProcId, XCPT_SIGNAL_INTR );
            break;
        case LNK_ABORT:
            DosKillProcess( DKP_PROCESSTREE, ProcId );
            break;
        case LNK_DONE:
            return;
        case LNK_SHUTDOWN:
            exit( 0 );
            break;
        }
    }
}


main( int argc, char *argv[] )
{
    APIRET      rc;
    HFILE       h;
    unsigned long       actiontaken;
    unsigned long       sent;
    char        done;

    if( argc > 1 && (argv[1][0] == 'q' || argv[1][0] == 'Q') ) {
        rc = DosOpen( PREFIX DEFAULT_NAME, &h, &actiontaken, 0ul,
                        0, 0x01, 0x41, 0ul );
        if( rc == 0 ) {
            done = LNK_SHUTDOWN;
            DosWrite( h, &done, sizeof( done ), &sent );
            DosClose( h );
        }
        exit( 0 );
    }
    CmdProc = getenv( "COMSPEC" );
    if( CmdProc == NULL ) {
        fprintf( stderr, "Unable to find command processor\n" );
        exit( 1 );
    }
    //NYI: need to accept name for link pipe
    rc = DosCreateNPipe( PREFIX DEFAULT_NAME, &LnkHdl,
        NP_NOINHERIT | NP_NOWRITEBEHIND | NP_ACCESS_DUPLEX,
        NP_WAIT | NP_READMODE_MESSAGE | NP_TYPE_MESSAGE | 1,
        MAX_TRANS, MAX_TRANS, 0 );
    if( rc != 0 ) {
        fprintf( stderr, "Unable to create link pipe\n" );
        exit( 1 );
    }
    sprintf( RedirName, PREFIX "%d", getpid() );
    rc = DosCreateNPipe( RedirName, &RedirHdl,
        NP_NOINHERIT | NP_WRITEBEHIND | NP_ACCESS_INBOUND,
        NP_WAIT | NP_READMODE_BYTE | NP_TYPE_BYTE | 1,
        MAX_TRANS, MAX_TRANS, 0 );
    if( rc != 0 ) {
        fprintf( stderr, "Unable to create redirection pipe\n" );
        exit( 1 );
    }
    DosSetFHState( LnkHdl, OPEN_FLAGS_NOINHERIT );
    DosSetFHState( RedirHdl, OPEN_FLAGS_NOINHERIT );
    for( ;; ) {
        rc = DosConnectNPipe( LnkHdl );
        if( rc == 0 ) {
            ProcessConnection();
            DosDisConnectNPipe( LnkHdl );
        }
    }
}
