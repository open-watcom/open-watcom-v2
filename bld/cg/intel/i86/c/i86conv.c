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
#include "pattern.h"
#include "coderep.h"
#include "opcodes.h"
#include "regset.h"
#include "rtclass.h"
#include "vergen.h"
#include "model.h"
#include "funits.h"

extern  name    *       AllocTemp(type_class_def);
extern  instruction*    MakeUnary(opcode_defs,name*,name*,type_class_def);
extern  void            MoveSegOp(instruction*,instruction*,int);
extern  void            PrefixIns(instruction*,instruction*);
extern  instruction*    MakeMove(name*,name*,type_class_def);
extern  void            DupSeg(instruction*,instruction*);
extern  void            ReplIns(instruction*,instruction*);

extern    int   RoutineNum;


static  opcode_entry    C2to1[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg fu*/
_Un( R,    ANY,  NONE ),  V_NO,       R_MOVELOW,        RG_TWOBYTE,FU_NO,
_Un( R,    ANY,  NONE ),  V_NO,       R_MOVOP1TEMP,     RG_,    FU_NO,
_Un( U,    ANY,  NONE ),  V_CONSTTEMP, G_UNKNOWN,       RG_,    FU_NO,
_Un( C,    ANY,  NONE ),  V_OP1RELOC, R_MOVOP1TEMP,     RG_,    FU_NO,
_Un( ANY,  ANY,  NONE ),  V_NO,       R_MOVELOW,        RG_,    FU_NO,
};


static  opcode_entry    C4to1[] = {
/*********************************/
/*    from  to    eq          verify          gen             reg fu*/
_Un( ANY,  ANY,  NONE ),     V_NO,           R_CONVERT_LOW,  RG_DBL_BYTE,FU_NO,
};


static  opcode_entry    C4to2[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg fu*/
_Un( ANY,  ANY,  NONE ),  V_NO,         R_MOVELOW,      RG_,FU_NO,
};


static  opcode_entry    C8to4[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg fu*/
_Un( ANY,  ANY,  NONE ),  V_NO,         R_MOVE8LOW,      RG_,FU_NO,
};


static  opcode_entry    Ext1[] = {
/*********************************/
/*    from  to    eq          verify   gen             reg fu*/
_Un( ANY,  ANY,  NONE ),     V_NO,    R_CLRHIGH_B,    RG_,FU_NO,
};


static  opcode_entry    Ext2[] = {
/*********************************/
/*    from  to    eq          verify          gen             reg fu*/
_Un( ANY,  ANY,  NONE ),     V_NO,           R_CLRHIGH_W,    RG_WORD_DBL,FU_NO,
};


static  opcode_entry    Ext4[] = {
/*********************************/
/*    from  to    eq          verify          gen             reg fu*/
_Un( ANY,  ANY,  NONE ),     V_NO,           R_CLRHIGH_D,   RG_WORD_DBL,FU_NO,
};


static  opcode_entry    SExt1[] = {
/*********************************/
/*    from  to    eq          verify          gen             reg fu*/
_Un( R,    R,    NONE ),     V_NO,           G_SIGNEX,       RG_BYTE_EXT,FU_ALU1,
_Un( ANY,  ANY,  NONE ),     V_NO,           R_OP1RESREG,    RG_BYTE_EXT,FU_NO,
};


static  opcode_entry    SExt2[] = {
/*********************************/
/*    from  to    eq          verify          gen             reg fu*/
_Un( R,    R,    NONE ),     V_NO,           G_SIGNEX,       RG_WORD_EXT,FU_ALU1,
_Un( ANY,  ANY,  NONE ),     V_NO,           R_OP1RESREG,    RG_WORD_EXT,FU_NO,
};

static  opcode_entry    SExt4[] = {
/*********************************/
/*    from  to    eq          verify          gen             reg fu*/
_Un( ANY,  ANY,  NONE ),     V_NO,           R_CDQ,   RG_WORD_DBL,FU_NO,
};


static  opcode_entry    ExtPT[] = {
/*********************************/
/*    from  to    eq          verify          gen             reg fu*/
_Un( ANY,  ANY,  NONE ),     V_NO,           R_EXTPT,        RG_,FU_NO,
};


static  opcode_entry    CRtn[] = {
/*********************************/
/*    from  to    eq          verify          gen             reg fu*/
_Un( ANY,  ANY,  NONE ),     V_NO,           R_MAKECALL,     RG_,FU_NO,
};

static opcode_entry     *CvtAddr[] = {
        &C2to1,
        &C4to1,
        &C4to2,
        &C8to4,
        &Ext1,
        &Ext2,
        &Ext4,
        &SExt1,
        &SExt2,
        &SExt4,
        &ExtPT
        };

static  rt_class         CvtTable[] = {
/*                               from*/
/*U1    I1     U2     I2     U4     I4     U8     I8     CP     PT     FS     FD    FL       to*/
OK,    OK,    C2TO1, C2TO1, C4TO1, C4TO1, C4TO1, C4TO1, CU4,   CU4,   CU4,   CU4,  CU4,     /* U1*/
OK,    OK,    C2TO1, C2TO1, C4TO1, C4TO1, C4TO1, C4TO1, CU4,   CU4,   CI4,   CI4,  CI4,     /* I1*/
EXT1,  S_EXT1,OK,    OK,    C4TO2, C4TO2, C4TO2, C4TO2, CU4,   CU4,   CU4,   CU4,  CU4,     /* U2*/
EXT1,  S_EXT1,OK,    OK,    C4TO2, C4TO2, C4TO2, C4TO2, CU4,   CU4,   CI4,   CI4,  CI4,     /* I2*/
CU2,   CI2,   EXT2,  S_EXT2,OK,    OK,    C8TO4, C8TO4, OK,    OK,    C_S_U, C_D_U,C_D_U,   /* U4*/
CU2,   CI2,   EXT2,  S_EXT2,OK,    OK,    C8TO4, C8TO4, OK,    OK,    C_S_4, C_D_4,C_D_4,   /* I4*/
CU4,   CI4,   CU4,   CI4,   EXT4,  S_EXT4,OK,    OK,    OK,    OK,    C_S_U8,C_D_U8,C_D_U8, /* U8*/
CU4,   CI4,   CU4,   CI4,   EXT4,  S_EXT4,OK,    OK,    OK,    OK,    C_S_I8,C_D_I8,C_D_I8, /* I8*/
CU4,   CI4,   EXT_PT,CI4,   OK,    OK,    OK,    OK,    OK,    OK,    BAD,   BAD,  BAD,     /* CP*/
CU4,   CI4,   EXT_PT,CI4,   OK,    OK,    OK,    OK,    OK,    OK,    BAD,   BAD,  BAD,     /* PT*/
CU4,   CI4,   CU4,   CI4,   C_U4_S,C_I4_S,C_U8_S,C_I8_S,BAD,   BAD,   OK,    C_D_S,C_D_S,   /* FS*/
CU4,   CI4,   CU4,   CI4,   C_U4_D,C_I4_D,C_U8_D,C_I8_D,BAD,   BAD,   C_S_D, OK,   OK,      /* FD*/
CU4,   CI4,   CU4,   CI4,   C_U4_D,C_I4_D,C_U8_D,C_I8_D,BAD,   BAD,   C_S_D, OK,   OK,      /* FL*/
};

static  rt_class FPCvtTable[] = {
/*                               from*/
/*U1    I1     U2     I2     U4     I4     U8     I8     CP     PT     FS     FD    FL       to*/
OK,    OK,    C2TO1, C2TO1, C4TO1, C4TO1, C4TO1, C4TO1, CU4,   CU4,   CI2,   CI2,   CI2,     /* U1*/
OK,    OK,    C2TO1, C2TO1, C4TO1, C4TO1, C4TO1, C4TO1, CU4,   CU4,   CI2,   CI2,   CI2,     /* I1*/
EXT1,  S_EXT1,OK,    OK,    C4TO2, C4TO2, C4TO2, C4TO2, CU4,   CU4,   CI4,   CI4,   CI4,     /* U2*/
EXT1,  S_EXT1,OK,    OK,    C4TO2, C4TO2, C4TO2, C4TO2, CU4,   CU4,   FPOK,  FPOK,  FPOK,    /* I2*/
CU2,   CI2,   EXT2,  S_EXT2,OK,    OK,    C8TO4, C8TO4, OK,    OK,    FPOK,  FPOK,  FPOK,    /* U4*/
CU2,   CI2,   EXT2,  S_EXT2,OK,    OK,    C8TO4, C8TO4, OK,    OK,    FPOK,  FPOK,  FPOK,    /* I4*/
CU4,   CI4,   CU4,   CI4,   EXT4,  S_EXT4,OK,    OK,    OK,    OK,    C7S_U8,C7D_U8,C7D_U8,  /* U8*/
CU4,   CI4,   CU4,   CI4,   EXT4,  S_EXT4,OK,    OK,    OK,    OK,    FPOK,  FPOK,  FPOK,    /* I8*/
CU4,   CI4,   EXT_PT,CI4,   OK,    OK,    OK,    OK,    OK,    OK,    BAD,   BAD,   BAD,     /* CP*/
CU4,   CI4,   EXT_PT,CI4,   OK,    OK,    OK,    OK,    OK,    OK,    BAD,   BAD,   PT,      /* PT*/
CI2,   CI2,   CI4,   FPOK,  FPOK,  FPOK,  C7U8_S,FPOK,  BAD,   BAD,   FPOK,  FPOK,  FPOK,    /* FS*/
CI2,   CI2,   CI4,   FPOK,  FPOK,  FPOK,  C7U8_D,FPOK,  BAD,   BAD,   FPOK,  FPOK,  FPOK,    /* FD*/
CI2,   CI2,   CI4,   FPOK,  FPOK,  FPOK,  C7U8_D,FPOK,  BAD,   BAD,   FPOK,  FPOK,  FPOK,    /* FL*/
};

extern  rt_class        AskHow( type_class_def fr, type_class_def to ) {
/**********************************************************************/

    if( _FPULevel( FPU_87 ) ) {
        return( FPCvtTable[  fr + to * XX  ] );
    } else {
        return( CvtTable[  fr + to * XX  ] );
    }
}


extern  bool    CvtOk( type_class_def fr, type_class_def to ) {
/*************************************************************/

    if( fr == XX ) return( FALSE );
    if( to == XX ) return( FALSE );
    if( AskHow( fr, to ) != BAD ) return( TRUE );
    return( FALSE );
}


extern  instruction     *rDOCVT( instruction *ins ) {
/**************************************************/

    name        *src;
    name        *dst;
    name        *name;
    instruction *new_ins;
    rt_class    how;

    src = ins->operands[ 0 ];
    dst = ins->result;
    if( src->n.name_class != XX && ins->base_type_class == XX ) {
        ins->base_type_class = src->n.name_class;
    }
    ins->head.state = INS_NEEDS_WORK;
    if( src->n.class == N_CONSTANT && src->c.const_type == CONS_ABSOLUTE ) {
        how = OK;
    } else {
        how = AskHow( ins->base_type_class, ins->type_class );
    }
    if( how < OK ) {
        name = AllocTemp( how );
        new_ins = MakeUnary( ins->head.opcode, src, name, how );
        new_ins->base_type_class = ins->base_type_class;
        new_ins->type_class = how;
        ins->base_type_class = how;
        ins->table = NULL;
        ins->operands[ 0 ] = name;
        MoveSegOp(ins,new_ins,0);
        PrefixIns( ins, new_ins );
    } else if( how == OK ) {
        new_ins = MakeMove( src, dst, ins->type_class );
        DupSeg(ins,new_ins);
        ReplIns( ins, new_ins );
    } else if( how == FPOK ) {
        ins->head.opcode = OP_MOV;
        ins->table = NULL;
        ins->u.gen_table = NULL;
        new_ins = ins;
        if( !_IsFloating( ins->type_class ) ) {
            ins->type_class = ins->base_type_class;
        }
    } else if( how <= EXT_PT ) {
        ins->table = CvtAddr[  how - ( OK + 1 )  ];
        new_ins = ins;
    } else {
        ins->table = &CRtn;
        RoutineNum = how - BEG_RTNS;
        new_ins = ins;
    }
    return( new_ins );
}
