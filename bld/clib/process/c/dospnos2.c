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


#include "variety.h"
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <string.h>
#include <stddef.h>
#include <malloc.h>
#include <process.h>
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#define INCL_DOSSESMGR
#define INCL_DOSQUEUES
#define INCL_DOSMEMMGR
#if defined( M_I86 )
 #define INCL_DOSINFOSEG
 #include "liballoc.h"
#endif
#include <wos2.h>
#include "rtdata.h"
#include "seterrno.h"

#define FS_SESSION      0
#define PMC_SESSION     2
#define PM_SESSION      3
#define DETACH_SESSION  4
#define TERM_QUEUE      "\\queues\\session"

extern  void    __ccmdline( char *, char **, char *, int );

#pragma aux     _dospawn "_*" parm caller [];
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

int _dospawn( int mode, char *pgm, char *cmdline, char *envp, char *argv[] )
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

    __ccmdline( pgm, argv, cmdline, 0 );
    if( mode == P_NOWAIT ) {
        exec_flag = EXEC_ASYNCRESULT;
    } else if( mode == P_NOWAITO ) {
        exec_flag = EXEC_ASYNC;
    } else {
        exec_flag = EXEC_SYNC;
    }
#if defined( __WARP__ )
    {
        ULONG           app_type;
        TIB             *ptib;
        PIB             *ppib;
        ULONG           session;
        PID             pid;
        REQUESTDATA     request_data;
        ULONG           data_len;

        use_exec_pgm = 0;
        rc = DosQueryAppType( pgm, &app_type );
        if( rc != 0 ) {
            __set_errno_dos( rc );
            return( -1 );
        }
        if( (app_type & FAPPTYP_EXETYPE) == FAPPTYP_NOTSPEC && !( app_type & FAPPTYP_DOS ) ) {
            /* type of program not specified in executable file */
            use_exec_pgm = 1;
        } else {
            rc = DosGetInfoBlocks( &ptib, &ppib );
            if( rc != 0 ) {
                __set_errno_dos( rc );
                return( -1 );
            }
            if( !( app_type & FAPPTYP_DOS ) ) {
                app_type &= FAPPTYP_EXETYPE;
                if( (ppib->pib_ultype == FS_SESSION)
                 || (ppib->pib_ultype == DETACH_SESSION) ) {
                    if( (app_type == FS_SESSION) || (app_type == PMC_SESSION) ){
                        use_exec_pgm = 1;
                    }
                } else if( ppib->pib_ultype == app_type ) {
                    use_exec_pgm = 1;
                }
            }
        }
        if( use_exec_pgm ) {
            rc = DosExecPgm( NULL, 0, exec_flag,
                             cmdline, envp, &returncodes, pgm );
        } else {
            termq = NULL;
            related = SSF_RELATED_INDEPENDENT;
            makeqname( queuename, ppib->pib_ulpid, ptib->tib_ordinal );
            if( mode == P_WAIT ) {
                rc = DosCreateQueue( &termq, QUE_FIFO, queuename );
                if( rc != 0 ) {
                    __set_errno_dos( rc );
                    return( -1 );
                }
                related = SSF_RELATED_CHILD;
            }
            sd.Length = 32;
            sd.Related = related;
            sd.FgBg = SSF_FGBG_FORE;
            sd.TraceOpt = SSF_TRACEOPT_NONE;
            sd.PgmTitle = NULL;
            while( *cmdline != '\0' ) ++cmdline;    // don't need argv[0]
            ++cmdline;
            sd.PgmName = pgm;
            sd.PgmInputs = cmdline;
            if( app_type & FAPPTYP_DOS ) {  // A DOS program
                sd.SessionType = ( ppib->pib_ultype == FS_SESSION )
                                 ? SSF_TYPE_VDM: SSF_TYPE_WINDOWEDVDM;
                sd.Environment = NULL;
            } else {
                sd.SessionType = SSF_TYPE_DEFAULT;
                sd.Environment = envp;
            }
            sd.TermQ = queuename;
            sd.InheritOpt = SSF_INHERTOPT_PARENT;
            sd.IconFile = NULL;
            sd.PgmHandle = 0;
            rc = DosStartSession( &sd, &session, &pid );
            if( ( rc == 0 ) || ( rc == ERROR_SMG_START_IN_BACKGROUND ) ) {
                rc = 0;
                if( mode == P_WAIT ) {
                    DosReadQueue( termq, &request_data, &data_len,
                                  &data_address, 0, DCWW_WAIT,
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
#elif defined( M_I86 )
    {
        USHORT          app_type;
        SEL             sglobal;
        SEL             slocal;
        LINFOSEG        _WCFAR *local;
        USHORT          session;
        USHORT          pid;
        ULONG           request_data;
        USHORT          data_len;

        use_exec_pgm = 0;

        rc = DosQAppType( pgm, &app_type );
        if( rc != 0 ) {
            app_type = 0x00;  // Works around a prob in the NT OS/2 subsystem
        }
        if( ( app_type & 0x03 ) == 0 || ( app_type & 0x20 ) ) {
            /* type of program not specified, or is a DOS app */
            use_exec_pgm = 1;
        } else {
            rc = DosGetInfoSeg( &sglobal, &slocal );
            if( rc != 0 ) {
                __set_errno_dos( rc );
                return( -1 );
            }
            local = (LINFOSEG _WCFAR *) (slocal:>0);
            if( !( app_type & 0x20 ) ) {
                app_type &= 0x03;
                if( local->typeProcess == FS_SESSION ) {
                    if( (app_type == FS_SESSION) || (app_type == PMC_SESSION) ){
                        use_exec_pgm = 1;
                    }
                } else if( local->typeProcess == app_type & 0x03 ) {
                    use_exec_pgm = 1;
                }
            }
        }
        if( use_exec_pgm ) {
            char    *np;
            int     len;

            if( app_type & 0x20 ) { // DOS app
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
                        __set_errno_dos( ERROR_NOT_ENOUGH_MEMORY );
                        return( -1 );
                    }
                }
                strcpy( np, "cmd /c " );
                strcat( np, cmdline );
                cmdline = np;
                cmdline[3] = '\0';
                pgm = getenv( "COMSPEC" );
            }
            rc = DosExecPgm( NULL, 0, exec_flag,
                             cmdline, envp, &returncodes, pgm );
            if( app_type & 0x20 ) { // cleanup: DOS app only
                #if defined( __BIG_DATA__ )
                    lib_ffree( cmdline );
                #else
                    lib_nfree( cmdline );
                #endif
            }
        } else {
            termq = NULL;
            related = 0; //SSF_RELATED_INDEPENDENT;
            makeqname( queuename, local->pidCurrent, local->tidCurrent );
            if( mode == P_WAIT ) {
                rc = DosCreateQueue( &termq, 0, queuename );
                if( rc != 0 ) {
                    __set_errno_dos( rc );
                    return( -1 );
                }
                related = 1; //SSF_RELATED_CHILD;
            }
            sd.Length = 30;
            sd.Related = related;
            sd.FgBg = 0;
            sd.TraceOpt = 0;
            sd.PgmTitle = NULL;
            while( *cmdline != '\0' ) ++cmdline;    // don't need argv[0]
            ++cmdline;
            sd.PgmName = pgm;
            sd.PgmInputs = cmdline;
            sd.SessionType = 0;
            sd.Environment = envp;
            sd.TermQ = queuename;
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
    #error platform not supported
#endif
    if( rc != 0 ) {
        __set_errno_dos( rc );
        return( -1 );
    }
    if( mode == P_WAIT ) {
        return( returncodes.codeResult );
    }
    return( returncodes.codeTerminate );        /* process id of child */
}
