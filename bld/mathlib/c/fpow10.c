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


#include "variety.h"

/* floating point powers of 10 table for large powers of 10 */

const short int _BigPow10Table[] = {
        216,    0xF9C6,0x40C6,0x34E9,0x6CC7,    /* 10**216 */
        108,    0x4E12,0xCC83,0x403D,0x565B,    /* 10**108 */
        54,     0xC9CE,0x8814,0xE187,0x4B24,    /* 10**54  */
        27,     0x8402,0xE4FE,0xD971,0x4589,    /* 10**27  */
        14,     0x0000,0x1E90,0xBCC4,0x42D6,    /* 10**14  */
        8,      0x0000,0x0000,0xD784,0x4197,    /* 10**8   */
        4,      0x0000,0x0000,0x8800,0x40C3,    /* 10**4   */
        1,      0x0000,0x0000,0x0000,0x4024     /* 10**1   */
};

