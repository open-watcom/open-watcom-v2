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
* Description:  temporary, label manager
*
****************************************************************************/


#include "ftnstd.h"
#include "fcgbls.h"
#include "global.h"
#include "fmemmgr.h"


label_id        NextLabel( void ) {
//===========================

// Return next label.

    label_id    label;

    label = LabelIdx;
    LabelIdx++;
    return( label );
}


sym_id  TmpVar( TYPE typ, uint size ) {
//====================================

// Allocate a temporary symbol table entry that can be freed at the end
// of the statement.

    sym_id     temp;

    temp = FMemAlloc( sizeof( symbol ) - STD_SYMLEN );
    temp->u.ns.link = MList;
    MList = temp;
    temp->u.ns.flags = SY_USAGE | SY_TYPE | SY_VARIABLE;
    temp->u.ns.u1.s.xflags = 0;
    temp->u.ns.u3.address = NULL;
    temp->u.ns.xt.size = size;
    temp->u.ns.u1.s.typ = typ;
    _MgcSetClass( temp, MAGIC_TEMP );
    temp->u.ns.si.ms.sym = NULL;
    return( temp );
}


sym_id  StaticAlloc( uint size, TYPE typ ) {
//=========================================

// Allocate a static temporary symbol table entry.

    return( TmpVar( typ, size ) );
}
