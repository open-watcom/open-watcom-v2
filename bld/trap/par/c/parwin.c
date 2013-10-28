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
#include <dos.h>

static  unsigned long __far *BiosTime;

char *InitSys()
{
    BiosTime = MK_FP( 0x40, 0x6c );
    return( 0 );
}

void FiniSys()
{
}

unsigned long Ticks()
{
    return( *BiosTime >> 1 );
}


int NumPrinters()
{
    unsigned short __far *pp;

    pp = MK_FP(0x40,8);
    if( pp[0] == 0 ) return( 0 );
    if( pp[1] == 0 ) return( 1 );
    if( pp[2] == 0 ) return( 2 );
    return( 3 );
}


#pragma off(unreferenced);
unsigned PrnAddress( int printer )
#pragma on(unreferenced);
{
    unsigned short __far *pp;
    #ifdef SERVER
    {
        extern int CurrentPort;
        CurrentPort = printer;
    }
    #endif

    pp = MK_FP(0x40,8);
    return( pp[printer] );

}

void AccessPorts( unsigned first, unsigned last )
{
  first = first; last = last;
}

unsigned FreePorts( unsigned first, unsigned last )
{
  first = first; last = last;
  return( 1 );
}
