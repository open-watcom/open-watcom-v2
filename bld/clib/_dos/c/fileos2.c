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
* Description:  OS/2 specific implementation of _dos file functions.
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
#include <dos.h>
#include <wos2.h>
#include "rtdata.h"
#include "fileacc.h"
#include "openmode.h"
#include "iomode.h"
#include "seterrno.h"


_WCRTLINK unsigned _dos_open( const char *name, unsigned mode, int *handle )
{
    APIRET      rc;
    OS_UINT     actiontaken;
    HFILE       fhandle;
    OS_UINT     rwmode, share, openmode;
    unsigned    iomode_flags;

    while( *name == ' ' ) ++name;
    rwmode = mode & OPENMODE_ACCESS_MASK;
    /* Can't open WRONLY file in bound application under DOS */
#if defined(__OS2_286__)
    if( rwmode == OPENMODE_ACCESS_WRONLY && _RWD_osmode == DOS_MODE ) {
#else
    if( rwmode == OPENMODE_ACCESS_WRONLY ) {
#endif
        rwmode = OPENMODE_ACCESS_RDWR;
    }
    share = mode & OPENMODE_SHARE_MASK;
    if( share == OPENMODE_DENY_COMPAT ) {
        share = OPENMODE_DENY_NONE;
    }
    openmode = share + rwmode;
    rc = DosOpen( (PSZ)name, &fhandle, &actiontaken, 0ul,
        _A_NORMAL, OPENFLAG_OPEN_IF_EXISTS, openmode, 0ul );
    if( rc ) {
        return( __set_errno_dos_reterr( rc ) );
    }
    *handle = fhandle;
    if( rwmode == O_RDWR ) iomode_flags = _READ | _WRITE;
    if( rwmode == O_RDONLY) iomode_flags = _READ;
    if( rwmode == O_WRONLY) iomode_flags = _WRITE;
    __SetIOMode( fhandle, iomode_flags );
    return( 0 );
}

unsigned _dos_close( int handle )
{
    APIRET  rc;

    __SetIOMode_nogrow( handle, 0 );
    rc = DosClose( handle );
    if( rc ) {
        return( __set_errno_dos_reterr( rc ) );
    }
    return( 0 );
}

unsigned _dos_commit( int handle )
{
    APIRET  rc;

    rc = DosBufReset( handle );
    if( rc ) {
        return( __set_errno_dos_reterr( rc ) );
    }
    return( 0 );
}
