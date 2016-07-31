/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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


#include "cgstd.h"
#include "coderep.h"
#include "cgdefs.h"
#include "symdbg.h"
#include "ppcenc.h"
#include "reloc.h"
#include "rscenc2.h"
#include "rscobj.h"


void EncodeRet( oc_ret *oc ) {

    ppc_ins             encoding;

    oc = oc;
    encoding = 0x6bfa8001;      // this is a little cheesy - ret ra(r26)
    ObjBytes( &encoding, sizeof( encoding ) );
}

static void doBranch( ppc_ins opcode, pointer lbl, uint reg )
{
    reg = reg;
    opcode = 0; // _Opcode( opcode ) | _A( reg );
    OutReloc( lbl, OWL_RELOC_BRANCH_REL, 0 );
    ObjBytes( &opcode, sizeof( opcode ) );
}

void EncodeJump( oc_handle *oc ) {

    doBranch( 0x30, oc->handle, 0 );
}

void EncodeCall( oc_handle *oc ) {

    doBranch( 0x34, oc->handle, 0 );
}

static  uint_8  BranchOpcodes[][2] = {
    { 0x39, 0x31 },                     /* OP_CMP_EQUAL */
    { 0x3d, 0x35 },                     /* OP_CMP_NOT_EQUAL */
    { 0x3f, 0x37 },                     /* OP_CMP_GREATER */
    { 0x3b, 0x33 },                     /* OP_CMP_LESS_EQUAL */
    { 0x3a, 0x32 },                     /* OP_CMP_LESS */
    { 0x3e, 0x36 },                     /* OP_CMP_GREATER_EQUAL */
};

void EncodeCond( oc_jcond *oc ) {

    ppc_ins     opcode;
    int         floating;

    floating = 0;
    if( oc->hdr.class & ATTR_FLOAT ) {
        floating = 1;
    }
    opcode = BranchOpcodes[ oc->cond - FIRST_COMPARISON ][ floating  ];
    doBranch( opcode, oc->handle, oc->index );
}
