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


#include "cgstd.h"
#include "coderep.h"

static  opcode_defs     FlipBranch[] = {
/**************************************/

        OP_BIT_TEST_FALSE,      /* OP_BIT_TEST_TRUE*/
        OP_BIT_TEST_TRUE,       /* OP_BIT_TEST_FALSE*/
        OP_CMP_NOT_EQUAL,       /* OP_CMP_EQUAL*/
        OP_CMP_EQUAL,           /* OP_CMP_NOT_EQUAL*/
        OP_CMP_LESS_EQUAL,      /* OP_CMP_GREATER*/
        OP_CMP_GREATER,         /* OP_CMP_LESS_EQUAL*/
        OP_CMP_GREATER_EQUAL,   /* OP_CMP_LESS*/
        OP_CMP_LESS             /* OP_CMP_GREATER_EQUAL*/
        };

static  opcode_defs     RevBranch[] = {
/*************************************/

        OP_BIT_TEST_TRUE,       /* OP_BIT_TEST_TRUE */
        OP_BIT_TEST_FALSE,      /* OP_BIT_TEST_FALSE*/
        OP_CMP_EQUAL,           /* OP_CMP_EQUAL*/
        OP_CMP_NOT_EQUAL,       /* OP_CMP_NOT_EQUAL*/
        OP_CMP_LESS,            /* OP_CMP_GREATER*/
        OP_CMP_GREATER_EQUAL,   /* OP_CMP_LESS_EQUAL*/
        OP_CMP_GREATER,         /* OP_CMP_LESS*/
        OP_CMP_LESS_EQUAL };    /* OP_CMP_GREATER_EQUAL*/


extern  void    RevCond( instruction *ins ) {
/*******************************************/
/* what to do to a conditional if we reverse its operands */

    ins->head.opcode = RevBranch[  ins->head.opcode - FIRST_CONDITION ];
}

extern  opcode_defs     FlipOpcode( opcode_defs opcode ) {
/********************************************************/

    return( FlipBranch[ opcode - FIRST_CONDITION ] );
}

extern  void    FlipCond( instruction *ins ) {
/*******************************************/
/* what to do to a conditional if we complement (!) it */

    ins->head.opcode = FlipOpcode( ins->head.opcode );
}
