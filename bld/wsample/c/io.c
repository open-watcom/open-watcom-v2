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
* Description:  Execution sampler I/O routines.
*
****************************************************************************/


#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "sample.h"
#include "wmsg.h"
#include "smpstuff.h"

static  long    SampOffset;
static  int     SampleHandle;

extern int      SysCreate( char *name );
extern unsigned SysWrite( int handle, void FAR_PTR *buff, unsigned len );
extern int      SysSeek( int handle, unsigned long loc );
extern int      SysClose( int handle );

int  SampCreate( char *name )
{
    SampOffset = 0;
    SampleHandle = SysCreate( name );
    return( SampleHandle == -1 );
}

int SampWrite( void FAR_PTR *buff, unsigned len )
{
    unsigned written;

    written = SysWrite( SampleHandle, buff, len );
    if( written == len ) {
        SampOffset += len;
        return( 0 );
    } else {
        SysSeek( SampleHandle, SampOffset );
        SysWrite( SampleHandle, "", 0 );                /* truncate the file */
        FarWriteProblem = 1;
        return( -1 );
    }
}

int SampSeek( unsigned long loc )
{
    if( SysSeek( SampleHandle, loc ) != loc ) return( -1 );
    SampOffset = loc;
    return( 0 );
}

int SampClose( void )
{
    SampOffset = 0;
    return( SysClose( SampleHandle ) );
}

void Output( char FAR_PTR *str )
{
    char FAR_PTR *p = str;
    int len = 0;
    while( *p++ ) ++len;
    SysWrite( 2, str, len );
}
