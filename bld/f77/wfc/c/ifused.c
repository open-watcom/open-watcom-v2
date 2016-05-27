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


//
// IFUSED    : Mark an intrinsic function as used.
//             Each bit in the array IFUsed corresponds to an intrinsic
//             function. ie : The bits of the arrary are numbered
//             0 to 112 from the low order bit of IFUsed[ 0 ] to the high
//             order bit of IFUsed[ 13 ]
//

#include "ftnstd.h"
#include "global.h"
#include "ifused.h"

#include <string.h>


void    IFInit(void) {
//================

// Initialize IFUsed flags.

    memset( IFUsed, 0, 24 );
}


void    MarkIFUsed( IFF function ) {
//==================================

// Turn on bit ( function mod 8 ) of IFUsed[ function/8 ].

    IFUsed[ function >> 3 ] |= ( 1 << ( function & 7 ) );
}


bool    IsIFUsed( IFF function ) {
//================================

// Return true if bit ( function mod 8 ) of IFUsed[ function/8 ] is on.

    return( ( IFUsed[ function >> 3 ] & ( 1 << ( function & 7 ) ) ) != 0 );
}
