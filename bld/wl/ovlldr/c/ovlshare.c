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
* Description:  set the sharing flags used by the overlay manager
*
****************************************************************************/


#include "ovlstd.h"

/* this can be called by the app. */

#define SHARE_SHIFT     4

extern unsigned far _ovl_openflags( unsigned share )
/**************************************************/
{
    unsigned    old;

    old = __OVLSHARE__ >> SHARE_SHIFT;
    if( !(__OVLFLAGS__ & OVL_DOS3) ) {
        share = 0;
    } else if( share > 4 ) {
        share = 4;
    }
    __OVLSHARE__ = share << SHARE_SHIFT;
    __CloseOvl__();
    return( old );
}
