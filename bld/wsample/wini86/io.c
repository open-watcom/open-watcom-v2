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
* Description:  File I/O functions for 16-bit Windows.
*
****************************************************************************/


#include <stdio.h>
#include <io.h>
#include <malloc.h>
#include <fcntl.h>
#include <string.h>
#include "commonui.h"
#include "sample.h"
#include "smpstuff.h"
#include "sampwin.h"
#include "sysio.h"

extern int      SysOpen( char *name );

int SampWrite( void FAR_PTR *buff, unsigned len )
{
    unsigned    written;
    int         handle;
    int         rc;

    _fstrcpy( SampName, SharedMemory->SampName );
    if( SharedMemory->SampleHandle == 0 ) {
        handle = SysCreate( SampName );
        SharedMemory->SampleHandle = 1;
    } else {
        handle = SysOpen( SampName );
        if( handle >= 0 ) {
            SysSeek( handle, SharedMemory->SampOffset );
        }
    }
    if( handle == -1 ) {
        FarWriteProblem = 1;
        return( -1 );
    }

    written = SysWrite( handle, buff, len );
    if( written == len ) {
        SharedMemory->SampOffset += len;
        rc = 0;
    } else {
        SysSeek( handle, SharedMemory->SampOffset );
        SysWrite( handle, "", 0 );
        FarWriteProblem = 1;
        rc = -1;
    }
    SysClose( handle );
    return( rc );

} /* SampWrite */

int SampSeek( unsigned long loc )
{
    SharedMemory->SampOffset = loc;
    return( 0 );

} /* SampSeek */

int SampClose( void )
{
    SharedMemory->SampOffset = 0;
    SharedMemory->SampleHandle = 0;
    return( 0 );
}
