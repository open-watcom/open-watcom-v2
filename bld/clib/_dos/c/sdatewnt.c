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
* Description:  Set current date (Win32 version).
*
****************************************************************************/


#include "variety.h"
#include <dos.h>
#include <windows.h>
#include "osver.h"
#include "seterrno.h"


_WCRTLINK unsigned _dos_setdate( struct dosdate_t *date )
{
    int                 error;
    SYSTEMTIME          st;
    HANDLE              htoken;
    TOKEN_PRIVILEGES    tp;

    error = 0;
    if( WIN32_IS_NT ) {
        if( !OpenProcessToken( GetCurrentProcess(),
                               TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                               &htoken ) ) {
            return( __set_errno_nt_reterr() );
        }
        LookupPrivilegeValue( NULL, SE_SYSTEMTIME_NAME,
                              &tp.Privileges[0].Luid );
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges( htoken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)NULL,
                               NULL );
    }

    GetLocalTime( &st );
    st.wDay = date->day;
    st.wMonth = date->month;
    st.wYear = date->year;
    st.wDayOfWeek = date->dayofweek;
    if( !SetLocalTime( &st ) ) {
        error = __set_errno_nt_reterr();
    }

    if( WIN32_IS_NT ) {
        tp.Privileges[0].Attributes = 0;
        AdjustTokenPrivileges( htoken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)NULL,
                               NULL );
    }
    return( error );
}
