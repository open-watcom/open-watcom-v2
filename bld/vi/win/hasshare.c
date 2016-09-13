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


#include "vi.h"
#include <dos.h>
#include <fcntl.h>

#ifndef __NT__
static bool lockFileRegion( int handle )
{
    union REGS  r;

    r.w.ax = 0x5c00;
    r.w.bx = handle;
    r.w.cx = 0;
    r.w.dx = 0;
    r.w.si = 0;
    r.w.di = 1;
    intdos( &r, &r );
    if( r.w.cflag ) {
        return( false );
    }
    return( true );

} /* lockFileRegion */

static void unlockFileRegion( int handle )
{
    union REGS  r;

    r.w.ax = 0x5c01;
    r.w.bx = handle;
    r.w.cx = 0;
    r.w.dx = 0;
    r.w.si = 0;
    r.w.di = 1;
    intdos( &r, &r );

} /* unlockFileRegion */

/*
 * HasShare - check if share is running
 */
bool HasShare( void )
{
    char        buff[FILENAME_MAX];
    int         handle;
    bool        has_share;

    GetModuleFileName( InstanceHandle, buff, sizeof( buff ) );
    if( _dos_open( buff, O_RDONLY, &handle ) ) {
        return( false );
    }
    has_share = false;
    if( lockFileRegion( handle ) ) {
        unlockFileRegion( handle );
        has_share = true;
    }
    _dos_close( handle );
    return( has_share );

} /* HasShare */
#endif
