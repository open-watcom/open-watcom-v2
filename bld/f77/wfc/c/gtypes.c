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
// GTYPES :     Generate typing information
//

#include "ftnstd.h"
#include "symflgs.h"
#include "itdefn.h"
#include "symtypes.h"
#include "types.h"
#include "emitobj.h"


void            DumpType( TYPE typ, uint size ) {
//===============================================

// Emit a single type.

    OutU16( ParmType( typ, size ) );
}


void            DumpTypes( TYPE typ1, uint size1, TYPE typ2, uint size2 ) {
//=========================================================================

// Emit 2 types.

    OutU16( ( ParmType( typ1, size1 ) << 8 ) + ParmType( typ2, size2 ) );
}


void            GenType( itnode *op ) {
//=====================================

// Emit a single type.

    OutU16( ParmType( op->typ, op->size ) );
}


void            GenTypes( itnode *op1, itnode *op2 ) {
//====================================================

// Emit 2 types.

    OutU16( ( ParmType( op1->typ, op1->size ) << 8 ) +
              ParmType( op2->typ, op2->size ) );
}
