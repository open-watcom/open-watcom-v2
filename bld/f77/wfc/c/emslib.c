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


//
// EMSLIB       : support for EMS
//

#include "ftnstd.h"
#include "posio.h"
#include "ems.h"

#include <fcntl.h>
#include <dos.h>
#include <dosfunc.h>

unsigned int    EMSsegment = { 0 };
int             EMShandle;


int     _EMS_available() {
//========================

// Determine if EMS memory available.

    int         handle;
    union REGS  r;
    char        buffer;

    handle = open( "EMMXXXX0", O_RDONLY | O_BINARY, 0 );
    if( handle >= 0 ) {
        r.h.ah = DOS_IOCTL;
        r.h.al = GET_DEV_INFO;
        r.x.bx = handle;
        r.x.cx = 0;
        r.x.dx = (unsigned)&buffer;
        intdos( &r, &r );
        if( ( r.x.cflag == 0 ) && ( ( r.x.dx & DEV_FLAG ) != 0 ) ) {
            r.h.ah = DOS_IOCTL;
            r.h.al = GET_OUT_STAT;
            r.x.bx = handle;
            r.x.cx = 0;
            r.x.dx = (unsigned) &buffer;
            intdos( &r, &r );
            close( handle );
            if( r.x.cflag == 0 ) {      // if no error
                if( r.h.al == 0xff ) {  // if device driver is present
                    if( _EMS_get_emm_status() == 0 ) {  // if status is OK
                        return( 1 );    // indicate present
                    }
                }
            }
        }
    }
    return( 0 );            // indicate not present
}


int     _EMS_get_emm_status() {
//=============================

    auto union REGS     r;

    r.h.ah = 0x40;
    int86( EMM, &r, &r );
    return( r.h.ah );
}


int     _EMS_get_page_frame_segment( unsigned *segment ) {
//========================================================

    auto union REGS     r;

    r.h.ah = 0x41;
    int86( EMM, &r, &r );
    *segment = r.x.bx;
    return( r.h.ah );
}


int     _EMS_get_number_of_pages( unsigned *unallocated, unsigned *total ) {
//==========================================================================

    auto union REGS     r;

    r.h.ah = 0x42;
    int86( EMM, &r, &r );
    *unallocated = r.x.bx;
    *total = r.x.dx;
    return( r.h.ah );
}


int     _EMS_allocate_memory( unsigned num_of_pages, int *handle ) {
//==================================================================

    auto union REGS     r;

    r.h.ah = 0x43;
    r.x.bx = num_of_pages;
    int86( EMM, &r, &r );
    *handle = r.x.dx;
    if( r.h.ah != 0 ) *handle = 0;          // 07-may-89
    return( r.h.ah );
}


int     _EMS_map_memory( int handle, int logical_page, int physical_page ) {
//==========================================================================

    auto union REGS     r;

    r.h.ah = 0x44;
    r.h.al = physical_page;         // 0,1,2 or 3
    r.x.bx = logical_page;          // 0 to n-1
    r.x.dx = handle;
    int86( EMM, &r, &r );
    return( r.h.ah );
}


int     _EMS_version( int *version ) {
//====================================

    auto union REGS     r;

    r.h.ah = 0x46;
    int86( EMM, &r, &r );
    *version = r.h.al;
    return( r.h.ah );
}
