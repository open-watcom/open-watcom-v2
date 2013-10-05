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
* Description:  MIPS specific branch/jump instruction encoding.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "pattern.h"
#include "symdbg.h"
#include "ocentry.h"
#include "mipsenc.h"
#include "reloc.h"

extern void ObjBytes( const void *, unsigned );
extern void OutReloc( pointer, owl_reloc_type, unsigned );


void EncodeRet( oc_ret *oc )
/**************************/
{
    mips_ins            encoding;

    oc = oc;
    // 'jr ra'
    encoding = _Opcode( 0 ) | _Rs( MIPS_RETURN_ADDR ) | _Function( 0x08 );
    ObjBytes( &encoding, sizeof( encoding ) );
    // TODO: Handle delay slot better
    encoding = MIPS_NOP;
    ObjBytes( &encoding, sizeof( encoding ) );
}


static void doBranch( mips_ins opcode, uint_8 cc, pointer lbl, uint reg1, uint reg2 )
/***********************************************************************************/
{
    opcode = _Opcode( opcode ) | _Rs( reg1 ) | _Rt( reg2 ) | _Rt( cc );
    OutReloc( lbl, OWL_RELOC_BRANCH_REL, 0 );
    ObjBytes( &opcode, sizeof( opcode ) );
    // TODO: Handle delay slot better
    opcode = MIPS_NOP;
    ObjBytes( &opcode, sizeof( opcode ) );
}


static void doCopBranch( mips_ins opcode, uint_8 cc, pointer lbl )
/****************************************************************/
{
    mips_ins    nop_code;

    // TODO: This is lame - there must be at least one instruction
    // between a FP comparison instruction and a branch testing the result
    nop_code = MIPS_NOP;
    ObjBytes( &nop_code, sizeof( nop_code ) );

    opcode = _Opcode( 0x11 ) | _Rs( opcode ) | _Rt( cc );
    OutReloc( lbl, OWL_RELOC_BRANCH_REL, 0 );
    ObjBytes( &opcode, sizeof( opcode ) );
    // TODO: Handle delay slot better
    opcode = MIPS_NOP;
    ObjBytes( &opcode, sizeof( opcode ) );
}


void EncodeJump( oc_handle *oc )
/******************************/
{
    // 'beq $zero,$zero,displacement'
    doBranch( 0x04, 0x00, oc->handle, MIPS_ZERO_SINK, MIPS_ZERO_SINK );
}


void EncodeCall( oc_handle *oc )
/******************************/
{
    mips_ins            encoding;

    // TODO: PIC call
    // Call to absolute address
    // 'jal target'
    encoding = _Opcode( 0x03 );
    OutReloc( oc->handle, OWL_RELOC_JUMP_ABS, 0 );
    ObjBytes( &encoding, sizeof( encoding ) );
    // TODO: Handle delay slot better
    encoding = MIPS_NOP;
    ObjBytes( &encoding, sizeof( encoding ) );
}


static  uint_8  BranchOpcodes[][2][2] = {
    { { 0x04, 0x00 }, { 0x08, 0x01 } },   /* OP_CMP_EQUAL */
    { { 0x05, 0x00 }, { 0x08, 0x00 } },   /* OP_CMP_NOT_EQUAL */
    { { 0x07, 0x00 }, { 0x08, 0x00 } },   /* OP_CMP_GREATER */
    { { 0x06, 0x00 }, { 0x08, 0x01 } },   /* OP_CMP_LESS_EQUAL */
    { { 0x01, 0x00 }, { 0x08, 0x01 } },   /* OP_CMP_LESS */
    { { 0x01, 0x01 }, { 0x08, 0x00 } },   /* OP_CMP_GREATER_EQUAL */
};

void EncodeCond( oc_jcond *oc )
/*****************************/
{
    mips_ins    opcode;
    uint_8      cncode;
    int         floating;
    uint        reg2;

    floating = 0;
    if( oc->hdr.class & ATTR_FLOAT ) {
        floating = 1;
    }
    reg2 = oc->index2 == -1 ? 0 : oc->index2;
    opcode = BranchOpcodes[oc->cond - FIRST_COMPARISON][floating][0];
    cncode = BranchOpcodes[oc->cond - FIRST_COMPARISON][floating][1];
    // Floating conditionals are quite different - we only have bc1f/bc1t
    // but have a full set of comparison instructions
    if( floating ) {
        doCopBranch( opcode, cncode, oc->handle );
    } else {
        if( (oc->cond != OP_CMP_EQUAL) && (oc->cond != OP_CMP_NOT_EQUAL) ) {
            // Only beq/bne can do reg/reg comparisons
            assert( reg2 == MIPS_ZERO_SINK );
        }
        assert( opcode );
        doBranch( opcode, cncode, oc->handle, oc->index, reg2 );
    }
}
