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
#include "rtdata.h"
#include "rtinit.h"


/*
 * This function is actually a _WCRTLINK function.  It's prototyped
 * incorrectly so that this module will work correctly with both dynamic
 * and static versions of the C library.
 *
 * The pragma is there so that this module will work with both stack and
 * register calling conventions, eliminating the need to have a separate
 * object file for each.
 *
 * Similarly, it's declared _WCI86FAR so that the same object file will work
 * in any 16-bit memory model.
 */
#pragma aux __set_commode "*_";
extern void _WCI86FAR   __set_commode( void );


static void do_it( void )
{
    __set_commode();
}


AXI( do_it, INIT_PRIORITY_LIBRARY )
