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
#include <bios.h>
#include "necibm.h"
#include "nonibm.h"

struct EquipBits {
    unsigned bootsFromDisk      : 1;
    unsigned hasCoPro           : 1;
    unsigned ramSize            : 2;
    unsigned initialVideoMode   : 2;
    unsigned numDisketteDrives  : 2;
    unsigned noDma              : 1;
    unsigned numSerialPorts     : 3;
    unsigned hasGamePort        : 1;
    unsigned hasSerialPrinter   : 1;
    unsigned numParPrinters     : 2;
};


_WCRTLINK unsigned short _bios_equiplist( void )
{
    unsigned short          necRc;
    union {
        struct EquipBits    bits;
        unsigned short      val;
    } equip;

    if( !__NonIBM ) {
        return( __ibm_bios_equiplist() );
    } else {
        /*** Obtain as much information as we can from NEC98 info ***/
        necRc = __nec98_bios_equiplist();
        equip.val = 0;
        equip.bits.hasCoPro = necRc&0x0002 ? 1 : 0;
        equip.bits.numDisketteDrives = (necRc>>3) & 0x0003;
        equip.bits.numParPrinters = necRc&0x4000 ? 1 : 0;
        equip.bits.numSerialPorts = (necRc>>9);
        return( equip.val );
    }
}
