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


#include <dos.h>

#if defined(_FMR)
void fmr_read_clk(unsigned __off, unsigned __seg);
#pragma aux fmr_read_clk = \
    "push DS"           \
    "push DI"           \
    "mov DI,AX"         \
    "mov DS,DX"         \
    "mov AH,1"          \
    "int 96h"           \
    "pop DI"            \
    "pop DS"            \
    parm [ax][dx]       \
    modify [ax];

int fmr_chk_prt(char prt_num);
#pragma aux fmr_chk_prt = \
    "mov AH,0"          \
    "int 94h"           \
    parm caller [al]    \
    value [ax]          \
    modify [ax dx];
#else
#include "dosequip.h"

static  unsigned long far *BiosTime;
#endif

char *InitSys()
{
#if !defined(_FMR)
    BiosTime = MK_FP( 0x40, 0x6c );
#endif
    return( 0 );
}

void FiniSys()
{
}

/* value of Ticks is incremented approx every 1/10 th of a second */

unsigned long Ticks()
{
#if defined(_FMR)
    auto struct date_time_block {
            unsigned short year;            /* 1980 - 2079 */
            unsigned char  month;           /* 1 - 12 */
            unsigned char  day;             /* 1 - 31 */
            unsigned char  day_of_week;     /* 0 - 6 */
            unsigned char  hours;           /* 0 - 23 */
            unsigned char  minutes;         /* 0 - 59 */
            unsigned char  seconds;         /* 0 - 59 */
            unsigned char  hundreds;        /* 1/100 second 0 - 99 */
            unsigned char  fill;            /* always 0 */
    } dt;
    fmr_read_clk( FP_OFF(&dt), FP_SEG(&dt) );
    return( dt.hundreds / 10 +
            (dt.seconds + ((dt.minutes + (dt.hours * 60)) * 60L)) * 10L );
#else
    return( *BiosTime >> 1 );
#endif
}


int NumPrinters()
{
#if defined(_FMR)
    int num_printers;
    char status;

    for( num_printers = 0; ; num_printers++ ) {
        status = fmr_chk_prt( num_printers ) >> 8;
        if( status != 0 ) break;
    }
    return( num_printers );
#else
    return( Equipment().num_printers );
#endif
}


unsigned PrnAddress( int printer )
{
#if defined(_FMR)
    if( printer == 0 ) return( 0x800 );
    return( 0 );    /* we don't know how to get port address */
#else
    return( *(unsigned far *) MK_FP( BIOS_SEG, PRINTER_BASE + printer*2 ) );
#endif
}

#pragma off(unreferenced);
void FreePorts( unsigned first, unsigned last )
#pragma on(unreferenced);
{
}

#pragma off(unreferenced);
unsigned AccessPorts( unsigned first, unsigned last )
#pragma on(unreferenced);
{
    return( 1 );
}
