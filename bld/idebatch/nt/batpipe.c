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


#include <windows.h>
#include "batpipe.h"

char                    *SharedMem;
HANDLE                  SemReadUp;
HANDLE                  SemWritten;
HANDLE                  SemReadDone;

unsigned BatservRead( void *buff, unsigned len )
{
    unsigned    bytes_read;

    ReleaseSemaphore( SemReadUp, 1, NULL );
    WaitForSingleObject( SemWritten, INFINITE );
    bytes_read = *(unsigned*)SharedMem;
    if( bytes_read > len ) bytes_read = len;
    memcpy( buff, SharedMem + sizeof( unsigned ), bytes_read );
    ReleaseSemaphore( SemReadDone, 1, NULL );
    return( bytes_read );
}

unsigned BatservWrite( void *buff, unsigned len )
{
    WaitForSingleObject( SemReadUp, INFINITE );
    *(unsigned*)SharedMem = len;
    memcpy( SharedMem + sizeof( unsigned ), buff, len );
    ReleaseSemaphore( SemWritten, 1, NULL );
    WaitForSingleObject( SemReadDone, INFINITE );
    return( len );
}

