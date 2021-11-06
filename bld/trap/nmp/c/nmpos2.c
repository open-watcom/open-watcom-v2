/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


bhandle myopen( char *name )
{
    HFILE hdl;
    USHORT action;

    if( DosOpen( name, &hdl, &action, 0,
                FILE_NORMAL,
                OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,
                0 ) == 0 ) {
        return( hdl );
    }
    return( BHANDLE_INVALID );
}

void myclose( bhandle handle )
{
    DosClose( handle );
}

int myread( bhandle handle, void FAR *buff, int len )
{
    USHORT      rc;
    USHORT      bytes_read;

    rc = DosRead( handle, buff, len, &bytes_read );
    if( rc != 0 ) return( 0 );
    return( bytes_read );
}


int mywrite( bhandle handle, void FAR *buff, int len )
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
