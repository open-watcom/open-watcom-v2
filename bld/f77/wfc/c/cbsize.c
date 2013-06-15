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
// CBSIZE       : common block size computations
//

#include "ftnstd.h"
#include "symbol.h"


void    SetComBlkSize( sym_id com, intstar4 com_size ) {
//======================================================

// Set size of common block.

#if defined( __386__ )
    com->ns.xt.size = com_size;
#else
    com->ns.xt.size = com_size / 0x10000L;
    com->ns.u1.xsize = com_size % 0x10000L;
#endif
}


intstar4    GetComBlkSize( sym_id com ) {
//=======================================

// Get size of common block.

#if defined( __386__ )
    return( com->ns.xt.size );
#else
    return( (intstar4)com->ns.xt.size * 0x10000L + com->ns.u1.xsize );
#endif
}
