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


#include <wos2.h>
#include "nmp.h"
#include <dos.h>
#include <share.h>

enum {
        OPENFLAG_FAIL_IF_EXISTS         = 0x00,
        OPENFLAG_OPEN_IF_EXISTS         = 0x01,
        OPENFLAG_REPLACE_IF_EXISTS      = 0x02,
        OPENFLAG_FAIL_IF_NOT_EXISTS     = 0x00,
        OPENFLAG_CREATE_IF_NOT_EXISTS   = 0x10
};

enum {
        OPENMODE_DASD                   = 0x8000,
        OPENMODE_WRITE_THROUGH          = 0x4000,
        OPENMODE_FAIL_ERRORS            = 0x2000,
        OPENMODE_INHERITANCE            = 0x0080,
        OPENMODE_SHARE_MASK             = 0x0070,
        OPENMODE_DENY_ALL               = 0x0010,
        OPENMODE_DENY_WRITE             = 0x0020,
        OPENMODE_DENY_READ              = 0x0030,
        OPENMODE_DENY_NONE              = 0x0040,
        OPENMODE_ACCESS_MASK            = 0x0007,
        OPENMODE_ACCESS_RDONLY          = 0x0000,
        OPENMODE_ACCESS_WRONLY          = 0x0001,
        OPENMODE_ACCESS_RDWR            = 0x0002
};

int myopen( char *name )
{
    HFILE hdl;
    USHORT action;

    if( DosOpen( name, &hdl, &action, 0ul,
                _A_NORMAL, OPENFLAG_OPEN_IF_EXISTS,
                OPENMODE_ACCESS_RDWR+SH_DENYNO, 0ul ) == 0 ) {
        return( hdl );
    }
    return( -1 );
}

void myclose( int handle )
{
    DosClose( handle );
}

int myread( int handle, char *buff, int len )
{
    USHORT      rc;
    USHORT      bytes_read;

    rc = DosRead( handle, buff, len, &bytes_read );
    if( rc != 0 ) return( 0 );
    return( bytes_read );
}


int mywrite( int handle, char *buff, int len )
{
    USHORT      rc;
    USHORT      bytes_written;

    rc = DosWrite( handle, buff, len, &bytes_written );
    if( rc != 0 ) return( 0 );
    return( bytes_written );
}

void mysnooze( void )
{
    DosSleep( 100 );
}
