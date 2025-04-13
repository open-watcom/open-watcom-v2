/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  OS/2 Spawn server for IDE.
*
****************************************************************************/


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


static HPIPE           LnkHdl;
static HPIPE           RedirHdl;

static char            RedirName[PREFIX_LEN + NAME_MAXLEN + 1];
static char            CmdProc[COMSPEC_MAXLEN + 1];
static unsigned        ProcId;

static batch_data      pdata;

static void exit_link( int rc )
{
    if( LnkHdl != NULLHANDLE ) {
        DosClose( LnkHdl );
    }
    if( RedirHdl != NULLHANDLE ) {
        DosClose( RedirHdl );
    }
    exit( rc );
}

static void RunCmd( const char *cmd_name )
{
    char        cmd[COMSPEC_MAXLEN + 4 + TRANS_DATA_MAXLEN + 9 + PREFIX_LEN + NAME_MAXLEN + 6 + 1];
    RESULTCODES res;
    UINT        rc;

    /*
     * NYI: spawning of DOS and PM programs
     * DOS programs can be handled by quoting the redirection
     * operators:
     *          prog 1^>out 2^>&1
     */
    snprintf( cmd, sizeof( cmd ), "%s%c/C %s 0<NUL 1>%s 2>&1%c", CmdProc, '\0', cmd_name, RedirName, '\0' );
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

static void ProcessConnection( void )
{
    ULONG               bytes_read;
    int                 len;
    struct _AVAILDATA   BytesAvail;
    ULONG               PipeState;
    APIRET              rc;
    RESULTCODES         res;
    PID                 dummy;
    const char          *dir;

    for( ;; ) {
        if( DosRead( LnkHdl, pdata.u.buffer, TRANS_BDATA_MAXLEN, &bytes_read ) != 0 )
            break;
        len = bytes_read - 1;
        if( len < 0 )
            break;
        /*
         * add additional null terminate character
         */
        pdata.u.s.u.data[len] = '\0';
        switch( pdata.u.s.cmd ) {
        case LNK_CWD:
            rc = 0;
            dir = pdata.u.s.u.data;
            if( isalpha( dir[0] ) && dir[1] == ':' ) {
                rc = DosSetDefaultDisk( toupper( dir[0] ) - ( 'A' - 1 ) );
                dir += 2;
            }
            if( rc == 0 && dir[0] != '\0' ) {
                rc = DosSetCurrentDir( dir );
            }
            pdata.u.s.cmd = LNK_STATUS;
            pdata.u.s.u.status = rc;
            DosWrite( LnkHdl, pdata.u.buffer, 1 + sizeof( pdata.u.s.u.status ), &bytes_read );
            break;
        case LNK_RUN:
            DosSetNPHState( RedirHdl, NP_NOWAIT | NP_READMODE_BYTE );
            DosConnectNPipe( RedirHdl );
            DosSetNPHState( RedirHdl, NP_WAIT | NP_READMODE_BYTE );
            RunCmd( pdata.u.s.u.data );
            break;
        case LNK_QUERY:
            len = pdata.u.s.u.len;
            rc = DosPeekNPipe( RedirHdl, pdata.u.buffer, 0, &bytes_read, &BytesAvail, &PipeState );
            if( rc == 0 && BytesAvail.cbpipe != 0 ) {
                /*
                 * limit read length to maximum output length
                 */
                if( len > TRANS_DATA_MAXLEN )
                    len = TRANS_DATA_MAXLEN;
                DosRead( RedirHdl, pdata.u.s.u.data, len, &bytes_read );
                pdata.u.s.cmd = LNK_OUTPUT;
            } else if( DosWaitChild( DCWA_PROCESS, DCWW_NOWAIT, &res, &dummy, ProcId ) == ERROR_CHILD_NOT_COMPLETE ) {
                /* let someone else run */
                DosSleep( 1 );
                pdata.u.s.cmd = LNK_NOP;
                bytes_read = 0;
            } else {
                DosDisConnectNPipe( RedirHdl );
                ProcId = 0;
                pdata.u.s.cmd = LNK_STATUS;
                pdata.u.s.u.status = res.codeResult;
                bytes_read = sizeof( pdata.u.s.u.status );
            }
            DosWrite( LnkHdl, pdata.u.buffer, bytes_read + 1, &bytes_read );
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
            exit_link( 0 );
            break;
        }
    }
}


void main( int argc, char *argv[] )
{
    APIRET          rc;
    unsigned long   actiontaken;
    unsigned long   sent;
    char            *p;

    strcpy( RedirName, PREFIX DEFAULT_LINK_NAME );
    if( argc > 1 && (argv[1][0] == 'q' || argv[1][0] == 'Q') ) {
        rc = DosOpen( RedirName, &LnkHdl, &actiontaken, 0,
                        FILE_NORMAL,
                        OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                        OPEN_SHARE_DENYNONE | OPEN_ACCESS_WRITEONLY,
                        0 );
        if( rc == 0 ) {
            pdata.u.s.cmd = LNK_SHUTDOWN;
            DosWrite( LnkHdl, pdata.u.buffer, 1, &sent );
        }
        exit_link( 0 );
    }
    p = getenv( "COMSPEC" );
    if( p == NULL ) {
        fprintf( stderr, "Unable to find command processor\n" );
        exit_link( 1 );
    }
    strncpy( CmdProc, p, COMSPEC_MAXLEN );
    CmdProc[COMSPEC_MAXLEN] = '\0';
    //NYI: need to accept name for link pipe
    rc = DosCreateNPipe( RedirName, &LnkHdl,
        NP_NOINHERIT | NP_NOWRITEBEHIND | NP_ACCESS_DUPLEX,
        NP_WAIT | NP_READMODE_MESSAGE | NP_TYPE_MESSAGE | 1,
        TRANS_MAXLEN, TRANS_MAXLEN, 0 );
    if( rc != 0 ) {
        fprintf( stderr, "Unable to create link pipe\n" );
        exit_link( 1 );
    }
    sprintf( RedirName, PREFIX "%d", getpid() );
    rc = DosCreateNPipe( RedirName, &RedirHdl,
        NP_NOINHERIT | NP_WRITEBEHIND | NP_ACCESS_INBOUND,
        NP_WAIT | NP_READMODE_BYTE | NP_TYPE_BYTE | 1,
        TRANS_MAXLEN, TRANS_MAXLEN, 0 );
    if( rc != 0 ) {
        fprintf( stderr, "Unable to create redirection pipe\n" );
        exit_link( 1 );
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
