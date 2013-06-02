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
* Description:  OS/2 2.x specific parallel port access routines.
*
****************************************************************************/


#include <stddef.h>
#define INCL_DOSDEVICES
#define INCL_DOSPROCESS
#define INCL_DOSMISC
#include <os2.h>
#include "i86.h"


APIRET16 APIENTRY16 DosPortAccess(USHORT,USHORT,USHORT,USHORT);

extern USHORT APIENTRY16 output_port(USHORT, USHORT);
extern USHORT APIENTRY16 input_port(USHORT);

#define NUM_ELTS(a)   (sizeof(a) / sizeof(a[0]))

USHORT PortTest[] = {
        0x378, 0x3bc, 0x278
};

USHORT PortAddress[NUM_ELTS(PortTest)] = {
        0,0,0
};

USHORT PortsFound;

int NumPrinters( void )
{
    char                num_printers;
    APIRET              rc;

    rc = DosDevConfig(&num_printers, DEVINFO_PRINTER);
    if (rc != 0)
        return 0;
    if (num_printers > PortsFound)
        num_printers = PortsFound;
    return num_printers;
}

USHORT AccessPorts(USHORT first, USHORT last)
{
    DosPortAccess(0, 0, first, last);
    return 1;
}

void FreePorts(USHORT first, USHORT last)
{
    DosPortAccess(0, 1, first, last);
}

static int CheckForPort(int i, unsigned char value)
{
    output_port(PortTest[i], value);
    DosSleep(1);
    return (input_port(PortTest[i]) == value);
}


char *InitSys( void )
{
    int         i;

    PortsFound = 0;
    for (i = 0; i < NUM_ELTS(PortTest); ++i) {
        AccessPorts(PortTest[i], PortTest[i]);
        if (CheckForPort( i, 0x55 ) && CheckForPort(i, 0xaa)) {
            PortAddress[PortsFound++] = PortTest[i];
        }
        FreePorts(PortTest[i], PortTest[i]);
    }
    return 0;
}


void FiniSys( void )
{
}


ULONG Ticks( void )
{
    ULONG  ulMsecs;

    DosQuerySysInfo(QSV_MS_COUNT, QSV_MS_COUNT, &ulMsecs, sizeof(ulMsecs));
    return  ulMsecs / 100;
}


USHORT PrnAddress(int printer)
{
    return PortAddress[printer];
}
