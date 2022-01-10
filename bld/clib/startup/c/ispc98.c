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
#ifdef __WINDOWS__
    #include <windows.h>
#endif
#include "tinyio.h"
#include "ispc98.h"
#include "rtinit.h"
#include "realmod.h"


#define BIOS_CHK_OFFS    0xfff3

static int __is_PC98( void )
{
#if defined( __DOS__ ) || defined( __WINDOWS__ )
    unsigned short _WCFAR   *p;

  #if defined(__WINDOWS_386__)
    p = MK_FP( __F000, BIOS_CHK_OFFS );
  #elif defined(__WINDOWS__)
    extern char _WCFAR  _F000h[];

    p = MK_FP( _F000h, BIOS_CHK_OFFS );
  #elif defined(__DOS__) && defined(__386__)
    if( _ExtenderRealModeSelector == 0 )
        return( 0 );
    p = MK_FP( _ExtenderRealModeSelector, ( 0xf000 << 4 ) | BIOS_CHK_OFFS );
  #elif defined(__DOS__)
    p = MK_FP( 0xf000, BIOS_CHK_OFFS );
  #endif
    if( *p == 0xFD80 )
        return( 1 );
#endif
    return( 0 );
}

/****
***** If this module is linked in, the startup code will call this function,
***** which will initialize the __isPC98 global variable.
****/

int     _WCNEAR __isPC98 = 0;   // 0  IBM PC, 1  NEC PC-98

static void init_on_startup( void )
{
    __isPC98 = __is_PC98();
}

AXI( init_on_startup, INIT_PRIORITY_LIBRARY )
