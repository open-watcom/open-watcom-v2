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


#include <stddef.h>
#include <i86.h>
#include <sys/osinfo.h>
#include "trperr.h"

#define INFO_SEG    0x40
#define PAR_BASE    0x08

static struct _timesel  __far *SysTime;

int NumPrinters()
{
    unsigned short  __far *par;
    int i;

    par = MK_FP( INFO_SEG, PAR_BASE );
    for( i = 3; i > 0; --i ) {
        if( par[i-1] != 0 ) return( i );
    }
    return( 0 );
}


#pragma off(unreferenced);
unsigned PrnAddress( int printer )
#pragma on(unreferenced);
{
    unsigned short  __far *par;

    par = MK_FP( INFO_SEG, PAR_BASE );
    return( par[printer] );
}

#pragma aux get_cs = "mov ax,cs" value [ax];
#pragma aux get_flags = "pushfd" "pop eax" value [eax];

extern unsigned short get_cs(void);
extern unsigned get_flags(void);

#define PRIV_MASK       3
#define IOPL_SHIFT      12

#define CPL()   (get_cs() & PRIV_MASK)
#define IOPL()  ((get_flags() >> IOPL_SHIFT) & PRIV_MASK)

unsigned AccessPorts( unsigned first, unsigned last )
{
    first = first;
    last = last;
    return( CPL() <= IOPL() );
}

void FreePorts( unsigned first, unsigned last )
{
    first = first;
    last = last;
}

char *InitSys()
{
    struct _osinfo  osinfo;

    if( CPL() > IOPL() ) {
        return( TRP_ERR_cannot_access_parallel_ports );
    }
    qnx_osinfo( 0, &osinfo );
    SysTime = MK_FP( osinfo.timesel, 0 );
    return( NULL );
}

void FiniSys()
{
}

unsigned long Ticks() {

    return( SysTime->nsec / 100000000 + SysTime->seconds * 10 );
}
