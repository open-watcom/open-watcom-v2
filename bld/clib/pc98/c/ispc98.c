/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
#include <i86.h>
#include <stdlib.h>
#include "tinyio.h"
#include "rtdata.h"
#include "rtinit.h"
#include "realmod.h"


/*
 * This code identify NEC PC-98 hardware for C run-time library.
 *
 * The best way to distinguish between AT compatibles and PC-9800 is to check
 * the data following the ROM BIOS entry point (FFFF:0000).
 * The AT BIOS date start at FFFF:0005 after far jump (5 bytes).
 * The BIOS date is written in ASCII characters, so when the date
 * separators '/' (2Fh in hexadecimal) are not present, it is PC-9800.
 * One should check FFFF:0007 and FFFF:000A. This detection method assumes
 * that machines other than AT compatibles are PC-9800.
 */

/****
***** If this module is linked in, the startup code will call this function,
***** which will initialize the __isPC98 global variable.
****/

#define BIOS_CHK_OFFS    0xfff7

unsigned char   __isPC98 = 0;   // 0 - IBM PC, 1 - NEC PC-98

#if defined( __DOS__ ) || defined( __WINDOWS__ ) && defined( _M_I86 )

static void init_on_startup( void )
{
    char _WCFAR     *p;

  #if defined(__WINDOWS__)
    extern char _WCFAR  _F000H[];

    p = MK_FP( _F000H, BIOS_CHK_OFFS );
  #elif defined(__DOS__) && defined(__386__)
    if( _ExtenderRealModeSelector == 0 )
        return;
    p = MK_FP( _ExtenderRealModeSelector, ( 0xf000 << 4 ) | BIOS_CHK_OFFS );
  #elif defined(__DOS__)
    p = MK_FP( 0xf000, BIOS_CHK_OFFS );
  #endif
    if( p[0] != '/' && p[3] != '/' ) {
        __isPC98 = 1;
    }
}

AXI( init_on_startup, INIT_PRIORITY_LIBRARY )

#endif
