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


#include "standard.h"
#include "cgdefs.h"
#include "coderep.h"
#include "opcodes.h"
#include "sysmacro.h"
#include "addrname.h"
#include "tree.h"
#include "seldef.h"

extern  instruction     *NewIns( int );
extern  void            AddIns( instruction * );
extern  type_def        *TypeAddress( cg_type );

extern  type_length     TypeClassLength[];

#define MAX_COST        0x7FFF

extern  int     ScanCost( select_node *s_node ) {
/***********************************************/

    s_node = s_node;
    return( MAX_COST );
}


extern  int     JumpCost( select_node *s_node ) {
/***********************************************/

    s_node = s_node;
    return( MAX_COST );
}


extern  int     IfCost( select_node *s_node ) {
/*********************************************/

    s_node = s_node;
    return( 0 );
}


extern  tbl_control     *MakeScanTab( select_list *list, signed_32 hi,
                                      label_handle other, cg_type tipe,
                                      cg_type real_tipe ) {
/*****************************************************************/

    list = list;
    hi = hi;
    tipe = tipe;
    other = other;
    real_tipe = real_tipe;
    return( NULL );
}


extern  tbl_control     *MakeJmpTab( select_list *list, signed_32 lo,
                                     signed_32 hi, label_handle other ) {
/*****************************************************************/

    list = list;
    lo = lo;
    hi = hi;
    other = other;
    return( NULL );
}


extern  name_def        *SelIdx( tbl_control *table, an node ) {
/**************************************************************/

    node = node;
    table = table;
    return( NULL );
}

extern name *ScanCall( tbl_control* tbl, name* tmp, type_class_def class) {
/**********************************************/

    tbl = tbl;
    tmp = tmp;
    class = class;
    return( NULL );
}

extern  type_def        *SelNodeType( an node, bool is_signed ) {
/************************************************************/

    cg_type     unsigned_t;
    cg_type     signed_t;

    switch( node->tipe->length ) {
    case 1:
        unsigned_t = T_UINT_1;
        signed_t = T_INT_1;
        break;
    case 2: /* no support in switch */
        unsigned_t = T_UINT_2;
        signed_t = T_INT_2;
        break;
    case 4:
        unsigned_t = T_UINT_4;
        signed_t = T_INT_4;
        break;
    default: /* an error */
        unsigned_t = NULL;
        signed_t =  NULL;
        break;
    }
    return( TypeAddress( is_signed ? signed_t : unsigned_t ) );
}

extern  void    MkSelOp( name *idx, cg_type tipe ) {
/**************************************************/

    instruction         *ins;

    ins = NewIns( 2 );
    ins->head.opcode = OP_ADD;
    ins->type_class = WD;
    ins->operands[ 0 ] = idx->i.index;
    ins->operands[ 1 ] = idx->i.index;
    ins->result = idx->i.index;
    AddIns( ins );
    ins = NewIns( 1 );
    ins->operands[ 0 ] = idx;
    ins->result = idx->i.index;
    ins->head.opcode = OP_SELECT;
    ins->type_class = tipe;
    ins->ins_flags |= INS_CC_USED;
    AddIns( ins );
}
