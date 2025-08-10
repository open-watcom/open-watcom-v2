/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
#include <stddef.h>
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


_WCRTLINK unsigned _dos_creat( const char *name, unsigned attr, int *phandle )
{
    HANDLE      osfh;
    DWORD       desired_access;
    DWORD       os_attr;
    int         handle;
    unsigned    iomode_flags;

    /*
     * First try to get the required slot.
     * No point in creating a file only to not use it.
     */
    handle = __allocPOSIXHandleDummy();
    if( handle == -1 ) {
        return( __set_errno_dos_reterr( ERROR_NOT_ENOUGH_MEMORY ) );
    }

    __GetNTCreateAttr( attr, &desired_access, &os_attr );
    osfh = CreateFile( (LPTSTR)name, desired_access, 0, 0, CREATE_ALWAYS, os_attr, NULL );
    if( osfh == INVALID_HANDLE_VALUE ) {
        /*
         * Give back the slot we got
         */
        __freePOSIXHandle( handle );
        return( __set_errno_nt_reterr() );
    }
    /*
     * Now use the slot we got.
     */
    __setOSHandle( handle, osfh );

    *phandle = handle;

    iomode_flags = _READ;
    if( (attr & _A_RDONLY) == 0 )
        iomode_flags |= _WRITE;
    __SetIOMode( handle, iomode_flags );
    return( 0 );
}
