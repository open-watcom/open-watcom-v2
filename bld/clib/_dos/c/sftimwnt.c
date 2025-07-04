/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of _dos_setftime() for Win32.
*
****************************************************************************/


#include "variety.h"
#include <time.h>
#include <dos.h>
#include <windows.h>
#include "iomode.h"
#include "seterrno.h"
#include "ntext.h"


_WCRTLINK unsigned _dos_setftime( int handle, unsigned date, unsigned time )
{
    HANDLE      osfh;
    FILETIME    ctime, atime, wtime;

    osfh = __getOSHandle( handle );
    if( GetFileTime( osfh, &ctime, &atime, &wtime ) ) {
        __FromDOSDT( date, time, &wtime );
        if( SetFileTime( osfh, &ctime, &wtime, &wtime ) ) {
            return( 0 );
        }
    }
    return( __set_errno_nt_reterr() );
}
