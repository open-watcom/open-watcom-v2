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
* Description:  Ctrl-Break interrupt handling support.
*                   (DOS and Windows 3.x code)
*
****************************************************************************/


#include "variety.h"
#include "rtinit.h"
#include "_ctrlc.h"
#include "rtdata.h"


unsigned char    _WCNEAR __ctrl_break_int = 0;

/****
***** If this module is linked in, the startup code will call this function,
***** which will initialize some global variables.
****/

static void init_on_startup( void )
{
    if( _RWD_isPC98 ) { /* NEC PC-98 */
        __ctrl_break_int = 0x06;    /* INT 0x06 */
    } else {            /* IBM PC */
        __ctrl_break_int = 0x1B;    /* INT 0x1B */
    }
}

AXI( init_on_startup, INIT_PRIORITY_LIBRARY + 1 )
