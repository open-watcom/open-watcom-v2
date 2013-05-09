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
* Description:  Named pipes link internals.
*
****************************************************************************/


#define PREFIX          "\\PIPE\\"
#define PREFIX_LEN      (sizeof( PREFIX ) - 1)

#define BINDERY         PREFIX "WDNMP.BND"
#define BINDERY_LEN     (sizeof( BINDERY ) - 1)

#define READ_SERV_SUFF  ".SRD"
#define READ_TRAP_SUFF  ".TRD"
#define WRITE_SERV_SUFF ".SWR"
#define WRITE_TRAP_SUFF ".TWR"
#define CONN_SERV_SUFF  ".SCN"
#define CONN_TRAP_SUFF  ".TCN"

#define MAX_NAME        8
#define MAX_TRANS       (2048+10)
#define DEFAULT_NAME    "NMPLink"

#define MACH_NAME       128

#define MAX_PIPE_NAME   (MACH_NAME+MAX_NAME+PREFIX_LEN+2)

#define BUFF_LEN        (2*MAX_TRANS)

enum {
    OPEN_SERV           = 'o',
    OPEN_TRAP           = 'O',
    CONNECT_SERV        = 'c',
    CONNECT_TRAP        = 'C',
    END_CONNECT_SERV    = 'e',
    END_CONNECT_TRAP    = 'E',
    DISCO_SERV          = 'd',
    DISCO_TRAP          = 'D',
    BIND_ACK            = 'a',
    BIND_NACK           = 'n',
    BIND_KILL           = 'k'
};

#ifdef SERVER
    #define READ_SUFF           READ_SERV_SUFF
    #define WRITE_SUFF          WRITE_SERV_SUFF
    #define CONN_SUFF           CONN_SERV_SUFF
    #define OPEN_REQUEST        OPEN_SERV
    #define CONNECT_REQUEST     CONNECT_SERV
    #define CONNECT_DONE        END_CONNECT_SERV
    #define DISCO_REQUEST       DISCO_SERV
#else
    #define READ_SUFF           READ_TRAP_SUFF
    #define WRITE_SUFF          WRITE_TRAP_SUFF
    #define CONN_SUFF           CONN_TRAP_SUFF
    #define OPEN_REQUEST        OPEN_TRAP
    #define CONNECT_REQUEST     CONNECT_TRAP
    #define CONNECT_DONE        END_CONNECT_TRAP
    #define DISCO_REQUEST       DISCO_TRAP
#endif

#define PIPE_ALREADY_OPEN 231
#define BROKEN_PIPE       109

#if defined( __NT__ )
#define bhandle         HANDLE
#define BHANDLE_INVALID INVALID_HANDLE_VALUE
#define BHANDLE_STDERR  GetStdHandle( STD_ERROR_HANDLE )
#elif defined( __OS2__ )
#define bhandle         HFILE
#define BHANDLE_INVALID ((HFILE)-1L)
#define BHANDLE_STDERR  2
#else
#define bhandle         int
#define BHANDLE_INVALID (-1)
#define BHANDLE_STDERR  2
#endif

extern bhandle myopen( char * );
extern void myclose( bhandle );
extern int myread( bhandle, char *, int );
extern int mywrite( bhandle, char *, int );
extern void mysnooze( void );
