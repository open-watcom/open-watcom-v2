/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  OS/2 version of the spawn() worker routine.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <string.h>
#include <stddef.h>
#include <process.h>
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#define INCL_DOSSESMGR
#define INCL_DOSQUEUES
#define INCL_DOSMEMMGR
#if defined( _M_I86 )
 #define INCL_DOSINFOSEG
 #include "liballoc.h"
#endif
#include <wos2.h>
#include "rtdata.h"
#include "seterrno.h"
#include "_process.h"


#define TERM_QUEUE      "\\queues\\session"

#pragma on(stack_check);

#define MakeHexDigit( c )  ((c > 9) ? (c - 0x0A + 'A') : (c + '0'))
#define GetNthDigit( u, n ) ((u >> (n << 2)) & 0x0f)
#define QUEUENAME_SIZE  sizeof( TERM_QUEUE ) + 16

static void makeqname( char *qname, ULONG pid, ULONG tid )
{
    int         x;
    char        *px;
    char        res;

    strcpy( qname, TERM_QUEUE );
    px = qname + sizeof( TERM_QUEUE ) - 1;
    for( x = 7; x >= 0; x-- ) {
        res = GetNthDigit( pid, x );
        *px = MakeHexDigit( res );
        px++;
    }
    *px = '.';
    px++;
    for( x = 2; x >= 0; x-- ) {
        res = GetNthDigit( tid, x );
        *px = MakeHexDigit( res );
        px++;
    }
    *px = 0;
}

int _dospawn( int mode, char *pgm, char *cmdline, char *envp, const char * const argv[] )
{
    APIRET      rc;
    RESULTCODES returncodes;
    unsigned    exec_flag;
    STARTDATA   sd;
    HQUEUE      termq;
    SHORT       *data_address;
    BYTE        element_priority;
    USHORT      related;
    char        use_exec_pgm;
    char        queuename[QUEUENAME_SIZE];
    OS_UINT     app_type;
    OS_UINT     session;
    OS_UINT     data_len;

    __ccmdline( pgm, argv, cmdline, 0 );
    if( mode == P_NOWAIT ) {
        exec_flag = EXEC_ASYNCRESULT;
    } else if( mode == P_NOWAITO ) {
        exec_flag = EXEC_ASYNC;
    } else {
        exec_flag = EXEC_SYNC;
    }
#if defined( _M_I86 )
    {
        SEL             sglobal;
        SEL             slocal;
        LINFOSEG        _WCFAR *local;
        USHORT          pid;
        ULONG           request_data;
        USHORT          proc_type;

        use_exec_pgm = 0;

        rc = DosQAppType( pgm, &app_type );
        if( rc != 0 ) {
            app_type = FAPPTYP_NOTSPEC; // Works around a prob in the NT OS/2 subsystem
        }
        if( (app_type & FAPPTYP_EXETYPE) == FAPPTYP_NOTSPEC || (app_type & FAPPTYP_DOS) ) {
            /* type of program not specified, or is a DOS app */
            use_exec_pgm = 1;
        } else {
            rc = DosGetInfoSeg( &sglobal, &slocal );
            if( rc != 0 ) {
                return( __set_errno_dos( rc ) );
            }
            local = _MK_FP( slocal, 0 );
            proc_type = local->typeProcess;
            if( (app_type & FAPPTYP_DOS) == 0 ) {
                switch( app_type & FAPPTYP_EXETYPE ) {
                case FAPPTYP_WINDOWCOMPAT:
                    if( proc_type == PT_FULLSCREEN
                      || proc_type == PT_WINDOWABLEVIO ) {
                        use_exec_pgm = 1;
                    }
                    break;
                case FAPPTYP_NOTWINDOWCOMPAT:
                    if( proc_type == PT_REALMODE ) {
                        use_exec_pgm = 1;
                    }
                    break;
                case FAPPTYP_NOTSPEC:
                    if( proc_type == PT_FULLSCREEN ) {
                        use_exec_pgm = 1;
                    }
                    break;
                case FAPPTYP_WINDOWAPI:
                    if( proc_type == PT_PM ) {
                        use_exec_pgm = 1;
                    }
                    break;
                }
            }
        }
        if( use_exec_pgm ) {
            char    *np;
            int     len;

            if( app_type & FAPPTYP_DOS ) { // DOS app
                // merge argv[0] & argv[1]
                cmdline[ strlen( cmdline ) ] = ' ';
                len = strlen( cmdline ) + 8;
    #if defined( __BIG_DATA__ )
                np = lib_fmalloc( len );
    #else
                np = lib_nmalloc( len );
    #endif
                if( np == NULL ) {
                    np = (char *)alloca( len );
                    if( np == NULL ) {
                        return( __set_errno_dos( ERROR_NOT_ENOUGH_MEMORY ) );
                    }
                }
                strcpy( np, "cmd /c " );
                strcat( np, cmdline );
                cmdline = np;
                cmdline[3] = '\0';
                pgm = getenv( "COMSPEC" );
            }
            rc = DosExecPgm( NULL, 0, exec_flag, cmdline, envp, &returncodes, pgm );
            if( app_type & FAPPTYP_DOS ) { // cleanup: DOS app only
    #if defined( __BIG_DATA__ )
                lib_ffree( cmdline );
    #else
                lib_nfree( cmdline );
    #endif
            }
        } else {
            termq = 0;
            related = 0;        //SSF_RELATED_INDEPENDENT;
            makeqname( queuename, local->pidCurrent, local->tidCurrent );
            if( mode == P_WAIT ) {
                rc = DosCreateQueue( &termq, 0, queuename );
                if( rc != 0 ) {
                    return( __set_errno_dos( rc ) );
                }
                related = 1;    //SSF_RELATED_CHILD;
            }
            sd.Length = 30;
            sd.Related = related;
            sd.FgBg = 0;
            sd.TraceOpt = 0;
            sd.PgmTitle = NULL;
            while( *cmdline != '\0' )
                ++cmdline;      // don't need argv[0]
            ++cmdline;
            sd.PgmName = pgm;
            sd.PgmInputs = (PBYTE)cmdline;
            sd.SessionType = SSF_TYPE_DEFAULT;
            sd.Environment = (PBYTE)envp;
            sd.TermQ = (PBYTE)queuename;
            sd.InheritOpt = 1;
            sd.IconFile = NULL;
            sd.PgmHandle = 0;
            rc = DosStartSession( &sd, &session, &pid );
            if( ( rc == 0 ) || ( rc == ERROR_SMG_START_IN_BACKGROUND ) ) {
                rc = 0;
                if( mode == P_WAIT ) {
                    DosReadQueue( termq, &request_data, &data_len,
                                  (PVOID)&data_address, 0, 0,
                                  &element_priority, 0 );
                    returncodes.codeResult = data_address[1];
                    DosCloseQueue( termq );
                } else {
                    returncodes.codeTerminate = pid;
                }
            }
        }
    }
#else
    {
        TIB             *ptib;
        PIB             *ppib;
        PID             pid;
        REQUESTDATA     request_data;
        ULONG           proc_type;

        use_exec_pgm = 0;
        rc = DosQueryAppType( pgm, &app_type );
        if( rc != 0 ) {
            return( __set_errno_dos( rc ) );
        }
        if( (app_type & FAPPTYP_EXETYPE) == FAPPTYP_NOTSPEC && (app_type & FAPPTYP_DOS) == 0 ) {
            /* type of program not specified in executable file */
            use_exec_pgm = 1;
        } else {
            rc = DosGetInfoBlocks( &ptib, &ppib );
            if( rc != 0 ) {
                return( __set_errno_dos( rc ) );
            }
            proc_type = ppib->pib_ultype;
            if( (app_type & FAPPTYP_DOS) == 0 ) {
                switch( app_type & FAPPTYP_EXETYPE ) {
                case FAPPTYP_WINDOWCOMPAT:
                    if( proc_type == PT_FULLSCREEN
                      || proc_type == PT_WINDOWABLEVIO
                      || proc_type == PT_DETACHED ) {
                        use_exec_pgm = 1;
                    }
                    break;
                case FAPPTYP_NOTWINDOWCOMPAT:
                    if( proc_type == PT_REALMODE ) {
                        use_exec_pgm = 1;
                    }
                    break;
                case FAPPTYP_NOTSPEC:
                    if( proc_type == PT_FULLSCREEN
                      || proc_type == PT_DETACHED ) {
                        use_exec_pgm = 1;
                    }
                    break;
                case FAPPTYP_WINDOWAPI:
                    if( proc_type == PT_PM ) {
                        use_exec_pgm = 1;
                    }
                    break;
                }
            }
        }
        if( use_exec_pgm ) {
            rc = DosExecPgm( NULL, 0, exec_flag, cmdline, envp, &returncodes, pgm );
        } else {
            termq = NULLHANDLE;
            related = SSF_RELATED_INDEPENDENT;
            makeqname( queuename, ppib->pib_ulpid, ptib->tib_ordinal );
            if( mode == P_WAIT ) {
                rc = DosCreateQueue( &termq, QUE_FIFO, queuename );
                if( rc != 0 ) {
                    return( __set_errno_dos( rc ) );
                }
                related = SSF_RELATED_CHILD;
            }
            sd.Length = 32;
            sd.Related = related;
            sd.FgBg = SSF_FGBG_FORE;
            sd.TraceOpt = SSF_TRACEOPT_NONE;
            sd.PgmTitle = NULL;
            while( *cmdline != '\0' )
                ++cmdline;    // don't need argv[0]
            ++cmdline;
            sd.PgmName = pgm;
            sd.PgmInputs = (PBYTE)cmdline;
            if( app_type & FAPPTYP_DOS ) {  // A DOS program
                if( proc_type == PT_FULLSCREEN ) {
                    sd.SessionType = SSF_TYPE_VDM;
                } else {
                    sd.SessionType = SSF_TYPE_WINDOWEDVDM;
                }
                sd.Environment = NULL;
            } else {
                sd.SessionType = SSF_TYPE_DEFAULT;
                sd.Environment = (PBYTE)envp;
            }
            sd.TermQ = (PBYTE)queuename;
            sd.InheritOpt = SSF_INHERTOPT_PARENT;
            sd.IconFile = NULL;
            sd.PgmHandle = 0;
            rc = DosStartSession( &sd, &session, &pid );
            if( ( rc == 0 ) || ( rc == ERROR_SMG_START_IN_BACKGROUND ) ) {
                rc = 0;
                if( mode == P_WAIT ) {
                    DosReadQueue( termq, &request_data, &data_len,
                                  (PPVOID)&data_address, 0, DCWW_WAIT,
                                  &element_priority, 0 );
                    returncodes.codeResult = data_address[1];
                    DosFreeMem( data_address );
                    DosCloseQueue( termq );
                } else {
                    returncodes.codeTerminate = pid;
                }
            }
        }
    }
#endif
    if( rc != 0 ) {
        return( __set_errno_dos( rc ) );
    }
    if( mode == P_WAIT ) {
        return( returncodes.codeResult );
    }
    return( returncodes.codeTerminate );        /* process id of child */
}
