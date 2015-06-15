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
#include <dos.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include "iomode.h"
#include "fileacc.h"
#include "ntext.h"
#include "openmode.h"
#include "seterrno.h"

_WCRTLINK unsigned _dos_creat( const char *name, unsigned mode, int *posix_handle )
{
    HANDLE      handle;
    DWORD       desired_access;
    DWORD       attr;
    int         hid;
    unsigned    iomode_flags;

    // First try to get the required slot.
    // No point in creating a file only to not use it.  JBS 99/11/01
    hid = __allocPOSIXHandle( DUMMY_HANDLE );
    if( hid == -1 ) {
        return( __set_errno_dos_reterr( ERROR_NOT_ENOUGH_MEMORY ) );
    }

    __GetNTCreateAttr( mode, &desired_access, &attr );
    handle = CreateFile( (LPTSTR) name, desired_access, 0, 0, CREATE_ALWAYS,
                    attr, NULL );
    if( handle == (HANDLE)-1 ) {
        __freePOSIXHandle( hid );
        return( __set_errno_nt_reterr() );
    }
    // Now use the slot we got.
    __setOSHandle( hid, handle );   // JBS 99/11/01

    *posix_handle = hid;

    iomode_flags = _READ;
    if( !(mode & _A_RDONLY) )
        iomode_flags |= _WRITE;
    __SetIOMode( hid, iomode_flags );
    return( 0 );
}
