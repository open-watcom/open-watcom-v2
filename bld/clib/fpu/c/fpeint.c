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
* Description:  FPU interrupt handling support.
*
****************************************************************************/


#include "variety.h"
#include "rtinit.h"

/*
 * This file can be used to customize numeric coprocessor interrupt
 * handling in the math libraries (math387r.lib, math387s.lib) for
 * NEC PC9800 series, Fujitsu, or IBM AT compatible PCs.
 *
 * Compile modified fpeint.c as follows:
 *      To replace fpeint in math387r.lib
 *           wcc386 -r -ox -w3 -zq -zc -zl -ms -3r -7 fpeint.c
 *           wlib math387r -+fpeint
 *      To replace fpeint in math387s.lib
 *           wcc386 -r -ox -w3 -zq -zc -zl -ms -3s -7 fpeint.c
 *           wlib math387s -+fpeint
 */

/*
 * -------------------------------------------------------------------
 *
 * For the IBM PC, we use IRQ13 (math coprocessor exception)
 * Level     Function
 * -----     --------
 * NMI       Parity, Watchdog Timer,
 *           Arbitration time-out,
 *           Channel Check
 * IRQ 0     Timer
 * IRQ 1     Keyboard
 * IRQ 2     Cascaded Interrupt Control -- IRQ  8 Real Time Clock
 *                                         IRQ  9 Redirect Cascade
 *                                         IRQ 10 Reserved
 *                                         IRQ 11 Reserved
 *                                         IRQ 12 Mouse
 *                                         IRQ 13 Math Coprocessor Exception
 *                                         IRQ 14 Fixed Disk
 *                                         IRQ 15 Reserved
 * IRQ 3     Serial Alternate
 * IRQ 4     Serial Primary
 * IRQ 5     Reserved
 * IRQ 6     Diskette
 * IRQ 7     Parallel Port
 * IRQ 8 through 15 are cascaded through IRQ 2
 *
 * IRQ 0 through  7 map to INT 0x08 through INT 0x0F
 * IRQ 8 through 15 map to INT 0x70 through INT 0x77
 *
 * -------------------------------------------------------------------
 *
 * For the NEC PC98, we would use IRQ 8 (math coprocessor exception)
 * Level     Function                                Vector Number
 * -----     --------
 * IRQ  0     Timer                                  08
 * IRQ  1     Keyboard                               09
 * IRQ  2     CTRV                                   0A
 * IRQ  3     Expansion bus INT0                     0B
 * IRQ  4     Serial Primary (RS-232C ch0)           0C
 * IRQ  5     Expansion bus INT1                     0D
 * IRQ  6     Expansion bus INT2                     0E
 * IRQ  7     Slave                                  0F
 * IRQ  8     Math coprocessor (286, 386, ...)       10
 * IRQ  9     Expansion bus INT3 (HD, HDLC)          11
 * IRQ 10     Expansion bus INT41 (640KB FD)         12
 * IRQ 11     Expansion bus INT42 (1MB FD)           13
 * IRQ 12     Expansion bus INT5 (RS-232C ch1, ch2)  14
 * IRQ 13     Expansion bus INT6 (mouse)             15
 * IRQ 14     Reserved (math coprocessor in 70116)   16
 * IRQ 15     Reserved                               17
 *
 * IRQ 0 through  7 map to INT 0x08 through INT 0x0F
 * IRQ 8 through 15 map to INT 0x10 through INT 0x17
 */

/*
 * If IRQ_NUM is less than 8, we have a problem in the code.
 */


_WCRTDATA extern unsigned char __FPE_int = 0;    /* Used for Rational Systems and Intel */
#if defined(__386__)
_WCRTDATA extern unsigned char __IRQ_num = 0;    /* Used for PharLap DOS Extender */
_WCRTDATA extern unsigned char __MST_pic = 0;    /* Master PIC port number */
_WCRTDATA extern unsigned char __SLV_pic = 0;    /* Slave PIC port number */
#endif



/****
***** If this module is linked in, the startup code will call this function,
***** which will initialize some global variables.
****/

static void init_on_startup( void )
{
    __FPE_int = 0x02;           /* INT 0x02 */
#ifdef __386__
    __IRQ_num = 0x0D;           /* IRQ 13 */
    __MST_pic = 0x20;           /* Master PIC port number */
    __SLV_pic = 0xA0;           /* Slave PIC port number */
#endif
}


AXI( init_on_startup, INIT_PRIORITY_FPU + 2 )
