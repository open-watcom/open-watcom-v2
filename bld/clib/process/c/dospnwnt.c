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
#include "widechar.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <string.h>
#include <stddef.h>
#include <malloc.h>
#include <process.h>
#include <windows.h>
#include "libwin32.h"
#include "osver.h"
#include "rtdata.h"
#include "seterrno.h"

extern  void    __F_NAME(__ccmdline,__wccmdline)( CHAR_TYPE *, CHAR_TYPE **, CHAR_TYPE *, int );

#ifdef _M_IX86
 #pragma aux    _dospawn "_*" parm caller [];
#endif
int __F_NAME(_dospawn,_wdospawn)( int mode, CHAR_TYPE *pgmname, CHAR_TYPE *cmdline,
                                  CHAR_TYPE *envp, CHAR_TYPE *argv[] )
{
    STARTUPINFO         sinfo;
    PROCESS_INFORMATION pinfo;
    DWORD               rc;
    BOOL                osrc;

    __F_NAME(__ccmdline,__wccmdline)( pgmname, argv, cmdline, 0 );

    memset( &sinfo, 0, sizeof( sinfo ) );
    sinfo.wShowWindow = SW_NORMAL;

    // When passing in Unicode environments, the OS may not know which code
    // page to use when translating to MBCS in spawned program's startup
    // code.  Result: Possible corruption of Unicode environment variables.
    #ifdef __WIDECHAR__
        #if defined(__AXP__) || defined(__PPC__)
            osrc = CreateProcessW( NULL, cmdline, NULL, NULL, TRUE,
                                   CREATE_UNICODE_ENVIRONMENT, envp,
                                   NULL, &sinfo, &pinfo );
        #else
            osrc = __lib_CreateProcessW( cmdline, TRUE, envp, &pinfo );
        #endif
    #else
        osrc = CreateProcessA( NULL, cmdline, NULL, NULL, TRUE, 0,
                               envp, NULL, &sinfo, &pinfo );
    #endif

    if( osrc == FALSE ) {
        DWORD err;
        err = GetLastError();
        if( (err == ERROR_ACCESS_DENIED)
         || (err == ERROR_BAD_EXE_FORMAT)
         || (err == ERROR_BAD_PATHNAME) ) {
            err = ERROR_FILE_NOT_FOUND;
        }
        return( __set_errno_dos( err ) );
    }
    if( mode == P_WAIT ) {
        if( WIN32_IS_WIN32S ) {
            // this is WIN32s
            Sleep( 1000 );
            rc = STILL_ACTIVE;
            while( rc == STILL_ACTIVE ) {
                Sleep( 100 );
                if( !GetExitCodeProcess( pinfo.hProcess, &rc ) ) {
                    return( __set_errno_nt() );
                }
            }
        } else {
            // this is WIN32 or Windows95
            if( WaitForSingleObject( pinfo.hProcess, -1 ) == 0 ) {
                GetExitCodeProcess( pinfo.hProcess, &rc );
            } else {
                rc = __set_errno_nt();
            }
        }
        CloseHandle( pinfo.hProcess );
    } else {
        /* no difference between P_NOWAIT and P_NOWAITO */
        rc = (int)pinfo.hProcess;
        /* cwait will close (free) the handle */
    }
    CloseHandle( pinfo.hThread );
    return( rc );
}
