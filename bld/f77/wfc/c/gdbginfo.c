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
// GDBGINFO  : generate debugging information
//

#include "ftnstd.h"
#include "global.h"
#include "fcodes.h"
#include "emitobj.h"


void    GSetDbugLine( void ) {
//======================

// Generate line # information (for all statements).

    EmitOp( FC_DBG_LINE );
    OutU16( SrcRecNum );
}


void    GSetSrcLine( void ) {
//=====================

// Generate line # information (for executable statements).

    source_t    *src;

    EmitOp( FC_SET_LINE );
    if( CurrFile->link != NULL ) {
        // for executable statments in an include file, the line number will
        // be the line number of the INCLUDE statement in the original file
        for( src = CurrFile; src->link != NULL; src = src->link ) {}
        OutU16( src->rec );
    } else {
        OutU16( SrcRecNum );
    }
}
