/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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


#include <windows.h>
#include "batpipe.h"


batch_data              bdata;
batch_shmem             *SharedMemPtr;
HANDLE                  SemReadUp;
HANDLE                  SemWritten;
HANDLE                  SemReadDone;

int BatservReadData( void )
{
    int     bytes_read;

    ReleaseSemaphore( SemReadUp, 1, NULL );
    WaitForSingleObject( SemWritten, INFINITE );
    bytes_read = SharedMemPtr->len;
    if( bytes_read > 0 ) {
        memcpy( bdata.u.buffer, SharedMemPtr->data.u.buffer, bytes_read );
    } else {
        bdata.u.s.cmd = LNK_NOP;
        bytes_read = 0;
    }
    ReleaseSemaphore( SemReadDone, 1, NULL );
    return( bytes_read - 1 );
}

int BatservWriteCmd( char link_cmd )
{
    WaitForSingleObject( SemReadUp, INFINITE );
    SharedMemPtr->len = 1;
    SharedMemPtr->data.u.s.cmd = link_cmd;
    ReleaseSemaphore( SemWritten, 1, NULL );
    WaitForSingleObject( SemReadDone, INFINITE );
    return( 0 );
}

int BatservWriteData( char link_cmd, const void *buff, int len )
{
    WaitForSingleObject( SemReadUp, INFINITE );
    SharedMemPtr->len = len + 1;
    SharedMemPtr->data.u.s.cmd = link_cmd;
    memcpy( SharedMemPtr->data.u.s.u.data, buff, len );
    ReleaseSemaphore( SemWritten, 1, NULL );
    WaitForSingleObject( SemReadDone, INFINITE );
    return( len );
}
