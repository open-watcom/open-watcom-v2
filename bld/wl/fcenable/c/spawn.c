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


#include <errno.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <string.h>
#include <stddef.h>
#include <malloc.h>
#include <process.h>
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#include <wos2.h>

extern  void    __ccmdline( char *, char **, char *, int );

#pragma aux     _dospawn "_*" parm caller [];
#pragma on(stack_check);
int _dospawn( int mode, char *pgm, char *cmdline, char *envp, char *argv[] )
{
    APIRET      rc;
    RESULTCODES returncodes;
    unsigned    exec_flag;

    __ccmdline( pgm, argv, cmdline, 0 );
    if( mode == P_NOWAIT ) {
        exec_flag = EXEC_ASYNCRESULT;
    } else if( mode == P_NOWAITO ) {
        exec_flag = EXEC_ASYNC;
    } else {
        exec_flag = EXEC_SYNC;
    }
    rc = DosExecPgm( NULL, 0, exec_flag, cmdline, envp, &returncodes, pgm );
    if( rc != 0 ) {
        return( -1 );
    }
    if( mode == P_WAIT ) {
        return( returncodes.codeResult );
    }
    return( returncodes.codeTerminate );        /* process id of child */
}
