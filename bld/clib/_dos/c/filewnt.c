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
* Description:  NT specific implementation of _dos file functions.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <share.h>
#include <direct.h>
#include <windows.h>
#include <dos.h>
#include "iomode.h"
#include "fileacc.h"
#include "ntex.h"
#include "openmode.h"
#include "rtdata.h"
#include "rtcheck.h"
#include "seterrno.h"

_WCRTLINK unsigned _dos_open( const char *name, unsigned mode, int *posix_handle )
{
    HANDLE      handle;
    DWORD       rwmode,share_mode;
    DWORD       desired_access,attr;
    unsigned    iomode_flags;
    int         hid;

    // First try to get the required slot.
    // No point in creating a file only to not use it.  JBS 99/11/01
    hid = __allocPOSIXHandle( DUMMY_HANDLE );
    if( hid == -1 ) {
        return( __set_errno_dos_reterr( ERROR_NOT_ENOUGH_MEMORY ) );
    }

    rwmode = mode & OPENMODE_ACCESS_MASK;

    __GetNTAccessAttr( rwmode, &desired_access, &attr );
    __GetNTShareAttr( mode & (OPENMODE_SHARE_MASK|OPENMODE_ACCESS_MASK),
                      &share_mode );
    handle = CreateFile( (LPTSTR) name, desired_access, share_mode, 0,
                        OPEN_EXISTING, attr, NULL );
    if( handle == (HANDLE)-1 ) {
        __freePOSIXHandle( hid );
        return( __set_errno_nt_reterr() );
    }
    // Now use the slot we got.
    __setOSHandle( hid, handle );   // JBS 99/11/01

    *posix_handle = hid;

    if( rwmode == O_RDWR )  iomode_flags = _READ | _WRITE;
    if( rwmode == O_RDONLY) iomode_flags = _READ;
    if( rwmode == O_WRONLY) iomode_flags = _WRITE;
    __SetIOMode( hid, iomode_flags );
    return( 0 );
}

_WCRTLINK unsigned _dos_close( int hid )
{
    HANDLE  h;

    h = __getOSHandle( hid );
    __SetIOMode_nogrow( hid, 0 );
    __freePOSIXHandle( hid );
    if( !CloseHandle( h ) ) {
        return( __set_errno_nt_reterr() );
    }
    return( 0 );
}

_WCRTLINK unsigned _dos_commit( int hid )
{
    __handle_check( hid, ERROR_INVALID_HANDLE );

    if( !FlushFileBuffers( __getOSHandle( hid ) ) ) {
        return( __set_errno_nt_reterr() );
    }
    return( 0 );
}
