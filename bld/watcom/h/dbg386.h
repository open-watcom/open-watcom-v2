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


/*

Debug register 6 definitions

*/

#define DR6_B0 0x00000001L              /* Breakpoint 0 */
#define DR6_B1 0x00000002L              /* Breakpoint 1 */
#define DR6_B2 0x00000004L              /* Breakpoint 2 */
#define DR6_B3 0x00000008L              /* Breakpoint 3 */
#define DR6_BD 0x00002000L              /* Debug register accessed */
#define DR6_BS 0x00004000L              /* Single step */
#define DR6_BT 0x00008000L              /* Task switch */

#define DR6_BSHIFT(n)   (n)             /* Number of bits to shift to get Bn */


/*

Debug register 7 definitions

*/

#define DR7_LE 0x00000100L              /* Enable all local breakpoints */
#define DR7_GE 0x00000200L              /* Enable all global breakpoints */

#define DR7_GLMASK 0x00000003L          /* Mask for G and L bits for a bkpt */
#define DR7_GLSHIFT(n) ((n) * 2)        /* Number of bits to shift to get */
                                        /* G and L bits for bkpt Bn */
#define DR7_GEMASK 0x00000002L          /* global enable (G bit) mask */
#define DR7_LEMASK 0x00000001L          /* local enable (L bit) mask */

#define DR7_RWLMASK 0x0000000FL         /*Mask for R/W and LEN bits for a bkpt*/
#define DR7_RWLSHIFT(n) (16 + (n) * 4)  /* Number of bits to shift to get */
                                        /* R/W/LEN bits for bkpt Bn */
#define DR7_RWMASK 0x00000003L          /* R/W bits mask */
#define DR7_BINST 0x00000000L           /* R/W specifies break on instr exec */
#define DR7_BWR 0x00000001L             /* R/W specifies break on data write */
#define DR7_BRW 0x00000003L             /* R/W specifies break on data read */
                                        /* or write */
#define DR7_LMASK 0x0000000CL           /* LEN bits mask */
#define DR7_L1  0x00000000L             /* LEN specifies 1-byte length */
#define DR7_L2  0x00000004L             /* LEN specifies 2-byte length */
#define DR7_L4  0x0000000CL             /* LEN specifies 4-byte length */

#define DRLen( l ) ( ( (l)==1 ) ? DR7_L1 : ( ( (l)==2 ) ? DR7_L2 : DR7_L4 ) )
