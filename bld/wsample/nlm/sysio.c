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


#include <stdio.h>
#include <io.h>
#include <malloc.h>
#include <fcntl.h>
#include "sample.h"
#include "smpstuff.h"
#include "sysio.h"

extern void *AllocTag;
extern void *Alloc( int, void* );
extern void Free( void* );

int SysCreate( char *name )
{
    return( open( name, O_BINARY | O_WRONLY | O_TRUNC | O_CREAT, 0 ) );
}

unsigned SysWrite( int handle, void FAR_PTR *buff, unsigned len )
{
    return( write( handle, buff, len ) );
}

unsigned long SysSeek( int handle, unsigned long loc )
{
    return( lseek( handle, loc, SEEK_SET ) );
}

int SysClose( int handle )
{
    return( close( handle ) );
}

void FAR_PTR *my_alloc( int size )
{
    return( Alloc( size, AllocTag ) );
}

void my_free( void FAR_PTR *chunk )
{
    Free( chunk );
}
