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
* Description:  Address/name/const/bool/instruction dumping.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "dump.h"
#include "data.h"

extern  void            DumpNL();
extern  void            DumpOperand(name*);
extern  void            DumpInt(int);
extern  void            DumpLong(signed_32);
extern  void            DumpPtr(pointer);
extern  void            DumpIns(instruction*);

static char *Classes[] = {
    "CL_ADDR_GLOBAL",
    "CL_ADDR_TEMP",
    "CL_POINTER",
    "CL_VALUE2",
    "CL_VALUE4",
    "CL_CONS2",
    "CL_CONS4",
    "CL_GLOBAL_INDEX",
    "CL_TEMP_INDEX",
    "CL_TEMP_OFFSET",
    ""
};


static char * Formats[] = {
    "NF_ADDR",
    "NF_NAME",
    "NF_CONS",
    "NF_BOOL",
    "NF_INS",
    ""
};


static  void    DumpAnAddr( an node ) {
/*************************************/

    DumpString( Classes[ node->class ] );
    DumpLiteral( " " );
    switch( node->class ) {
    case CL_VALUE2:
    case CL_VALUE4:
        DumpOperand( node->u.name );
        break;
    case CL_TEMP_OFFSET:
        DumpOperand( node->u.name );
        DumpLiteral( "+" );
        DumpLong( node->offset );
        break;
    case CL_ADDR_GLOBAL:
    case CL_ADDR_TEMP:
        DumpLiteral( "&" );
        DumpOperand( node->u.name );
        break;
    case CL_POINTER:
    case CL_GLOBAL_INDEX:
    case CL_TEMP_INDEX:
        if( node->class == CL_POINTER ) {
            DumpLiteral( "&" );
        }
        if( node->u.name != NULL ) {
            DumpOperand( node->u.name );
        }
        DumpLiteral( "[" );
        if( node->index != NULL ) {
            DumpOperand( node->index );
        }
        DumpLiteral( "+" );
        DumpLong( node->offset );
        DumpLiteral( "]" );
        break;
    }
}

static  void    DumpLbl( code_lbl **what ) {
/*****************************************************/

    block       *blk;
    int         i;

    blk = HeadBlock;
    for(;;) {
        if( blk == NULL ) {
            DumpLiteral( "Target ????" );
            DumpPtr( what );
            return;
        }
        i = blk->targets;
        while( --i >= 0 ) {
            if( what == &blk->edge[ i ].destination.u.lbl ) {
                DumpLiteral( "Target " );
                DumpPtr( blk );
                DumpLiteral( "(" );
                DumpInt( i + 1 );
                DumpLiteral( ") " );
                return;
            }
        }
        blk = blk->next_block;
    }
}

static  void    DumpAnBool( bn node ) {
/*************************************/

    DumpLiteral( "TRUE " );
    DumpLbl( node->t );
    DumpLiteral( " FALSE " );
    DumpLbl( node->f );
    DumpLiteral( " ENTRY L" );
    DumpPtr( node->e );
}

static  void    DumpAnIns( an node ) {
/************************************/

    DumpIns( node->u.ins );
}

static  void    DumpAnCons( an node ) {
/*************************************/

    DumpOperand( node->u.name );
}

static  void    DumpAnName( an node ) {
/*************************************/

    DumpOperand( node->u.name );
}

extern  void    Dumpan( an node ) {
/*********************************/

    DumpString( Formats[ node->format ] );
    DumpLiteral( " " );
    switch( node->format ) {
    case NF_ADDR:
        DumpAnAddr( node );
        break;
    case NF_NAME:
        DumpAnName( node );
        break;
    case NF_CONS:
        DumpAnCons( node );
        break;
    case NF_BOOL:
        DumpAnBool( (bn)node );
        break;
    case NF_INS:
        DumpAnIns( node );
        break;
    }
    DumpNL();
}
