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


#include <string.h>
#include "disasm.h"


instruction             CurrIns;

static  char            W_Bit;
static  char            D_Bit;

static const address_mode RMMode[] = {
    ADDR_BASE_INDEX, /* [BX+SI] */
    ADDR_BASE_INDEX, /* [BX+DI] */
    ADDR_BASE_INDEX, /* [BP+SI] */
    ADDR_BASE_INDEX, /* [BP+DI] */
    ADDR_INDEX,      /* [SI] */
    ADDR_INDEX,      /* [DI] */
    ADDR_BASE,       /* [BP] */
    ADDR_BASE        /* [BX] */
};

static const processor_reg RMBase[] = {
    BX_REG, /* [BX+SI] */
    BX_REG, /* [BX+DI] */
    BP_REG, /* [BP+SI] */
    BP_REG, /* [BP+DI] */
    0,      /* [SI] */
    0,      /* [DI] */
    BP_REG, /* [BP] */
    BX_REG  /* [BX] */
};

static const processor_reg RMIndex[] = {
    SI_REG, /* [BX+SI] */
    DI_REG, /* [BX+DI] */
    SI_REG, /* [BP+SI] */
    DI_REG, /* [BP+DI] */
    SI_REG, /* [SI] */
    DI_REG, /* [DI] */
    0,      /* [BP] */
    0,      /* [BX] */
};

static const processor_reg RMSeg[] = {
    DS_REG,         /* [BX+SI] */
    DS_REG,         /* [BX+DI] */
    SS_REG,         /* [BP+SI] */
    SS_REG,         /* [BP+DI] */
    DS_REG,         /* [SI] */
    DS_REG,         /* [DI] */
    SS_REG,         /* [BP] */
    DS_REG,         /* [BX] */
};

static const  processor_reg   RM32Reg[] = {
    EAX_REG,
    ECX_REG,
    EDX_REG,
    EBX_REG,
    ESP_REG,
    EBP_REG,
    ESI_REG,
    EDI_REG
};

static const processor_reg RM32Seg[] = {
    DS_REG,
    DS_REG,
    DS_REG,
    DS_REG,
    SS_REG,
    SS_REG,
    DS_REG,
    DS_REG
};


/*
 * Static function prototypes
 */
static  void            GetOperands( operand_type, bool );
static  ins_name        GetInsName( ins_name );


static processor_reg VarReg( processor_reg reg )
/**********************************************/

{
    if( CurrIns.pref & OPND_LONG ) {
        reg +=  FIRST_DWORD_REG - FIRST_WORD_REG;
    }
    return( reg );
}

static uint_8 InsOffset( void )
/*****************************/

{
    return( GetOffset() - InsAddr );
}


#if !defined( O2A )
static int  LookFor87Emulation( void )
/************************************/

{
    char        second_byte;
    int         inssize;
    int         seg_reg;

    inssize = 0;
    if( Opcode == 0xCD ) {
        second_byte = GetNextByte();
        if( second_byte >= 0x34 && second_byte <= 0x3d ) {
            CurrIns.pref |= EMU_INTERRUPT;
            CurrIns.op[ OP_3 ].disp = second_byte;
            if( second_byte <= 0x3b ) {
                Opcode = GetDataByte() + 0xA4;
                ++inssize;
            } else if ( second_byte == 0x3D ) {
                GetDataByte();
                Opcode = 0x90;
                inssize++;
            } else {                            /* 0x3C */
                GetDataByte();
                Opcode = GetDataByte();
                seg_reg = ( ~Opcode >> 6 ) & 3;
                CurrIns.pref |= SegPrefixTab[ seg_reg ];
                CurrIns.seg_used = FIRST_SEG_REG + seg_reg;
                inssize += 2;
                Opcode |= 0xC0;
            }
        }
    }
    return( inssize );
}
#endif


static  void    SetOperands( char opcode )
/****************************************/
{
    W_Bit = opcode & 1;
    if( CurrIns.opcode == I_BOUND ) {
        W_Bit = 1;
    }
    if( W_Bit ) {
        if( CurrIns.pref & OPND_LONG ) {
            CurrIns.mem_ref_size = 4;
        } else {
            CurrIns.mem_ref_size = 2;
        }
    } else {
        CurrIns.mem_ref_size = 1;
    }
    if( CurrIns.opcode == I_ARPL ) {
        D_Bit = 0;
    } else {
        D_Bit = opcode & 2;
    }
}


void  DoCode( instruction *curr, bool use_32 )
/*******************************************/

{
    operand_type        op_type;
    ins_name            instr_name;
    int                 seg_reg;
#if defined( O2A )
    int                 i;
    operand             *op;
#endif

    memset( &CurrIns, 0, sizeof( instruction ) );
    CurrIns.modifier = MOD_NONE;
    CurrIns.mem_ref_op = NULL_OP;
    for( ;; ) {                 /* collect prefix */
        if( EndOfSegment() ) {
            if( instr_name == I_WAIT ) {
                instr_name = I_NULL;
            } else {
                instr_name = I_INVALID;
            }
            break;
        }
        Opcode = GetDataByte();
#if !defined( O2A )
        LookFor87Emulation();
#endif
        instr_name = InsTab[ Opcode ].name;
        op_type = InsTab[ Opcode ].op_type;
        if( op_type == OT_PREFIX ) {
            if( CurrIns.pref & PrefixTab[ Opcode - 0xf0 ] ) {
                break;
            }
            CurrIns.pref |= PrefixTab[ Opcode - 0xf0 ]; /* 0xf0 == LOCK */
        } else if( op_type == OT_SEG_REG ) {
            seg_reg = _SegReg2Field( Opcode );
            if( CurrIns.pref & SegPrefixTab[ seg_reg ] ) {
                break;
            }
            CurrIns.pref |= SegPrefixTab[ seg_reg ];
            CurrIns.seg_used = FIRST_SEG_REG + seg_reg;
        } else if( op_type == OT_FS ) {
            CurrIns.pref |= PREF_FS;
            CurrIns.seg_used = FS_REG;
        } else if( op_type == OT_GS ) {
            CurrIns.pref |= PREF_GS;
            CurrIns.seg_used = GS_REG;
        } else if( op_type == OT_OPND_SIZE ) {
            CurrIns.pref |= PREF_OPND_SIZE;
        } else if( op_type == OT_ADDR_SIZE ) {
            CurrIns.pref |= PREF_ADDR_SIZE;
        } else if( instr_name == I_WAIT ) {
            /* any prefixes collected so far will apply to the WAIT instruction
               so decode it separately */
            if( CurrIns.pref != 0 ) {
                break;
            }
            CurrIns.pref |= PREF_FWAIT;
        } else {
            break;
        }
    }
    if( CurrIns.pref & PREF_OPND_SIZE ) {
        if( !use_32 ) CurrIns.pref |= OPND_LONG;
    } else {
        if( use_32 ) CurrIns.pref |= OPND_LONG;
    }
    if( CurrIns.pref & PREF_ADDR_SIZE ) {
        if( !use_32 ) CurrIns.pref |= ADDR_LONG;
    } else {
        if( use_32 ) CurrIns.pref |= ADDR_LONG;
    }
    SecondByte = GetNextByte();
    CurrIns.opcode = GetInsName( instr_name );
    if( CurrIns.opcode != I_INVALID ) {
        SetOperands( Opcode );
        GetOperands( op_type, use_32 );
        if( CurrIns.opcode != I_INVALID ) {
            if( CurrIns.seg_used == NULL_REG ) {
                CurrIns.seg_used = DS_REG;
            }
#if defined( O2A )
            if( CurrIns.opcode == I_RET_FAR ) {
                RetFarUsed = true;
            }
            for( i = 0; i < CurrIns.num_oper; ++i ) {     /* locate labels */
                op = &CurrIns.op[ i ];
                if( op->mode == ADDR_LABEL ) {
                    FindLabel( InsAddr + op->offset, op->disp, Segment );
                } else if( op->mode == ADDR_SEG_OFFSET ) {
                    FindSymbol( InsAddr + op->offset );
                } else if( op->offset != 0 ) {
                    FindSymbol( InsAddr + op->offset );
                }
            }
#endif
        }
    }
    if( IsWtk() ) {
        DoWtk( /*&CurrIns*/ );
    }
    CurrIns.ins_size = InsOffset();
    *curr = CurrIns;
}


static ins_name GetInsName( ins_name instr_name )
/***********************************************/

{
    int                 index;

    index = _RegField( SecondByte );
    switch( instr_name ) {
    case I_IMMED:
        instr_name = ImmedTable[ index ];
        break;
    case I_SHIFT:
        instr_name = ShiftTable[ index ];
        break;
    case I_GROUP1:
        instr_name = Group1Table[ index ];
        break;
    case I_GROUP2:
        instr_name = Group2Table[ index ];
        break;
    case I_GROUP3:
        instr_name = Group3Table[ index ];
        break;
    case I_ESC:
        instr_name = GetFppCode();
        if( instr_name != I_INVALID ) CurrIns.pref |= FP_INS;
        break;
    default:
        if( CurrIns.pref & OPND_LONG ) {
            switch( instr_name ) {
            case I_RET_FAR:
                /* change retf to retd only in use16 segment */
                if( !(CurrIns.pref & PREF_OPND_SIZE) ) break;
                /* fall through */
            case I_PUSHF:
            case I_POPF:
            case I_PUSHA:
            case I_POPA:
            case I_INSW:
            case I_OUTSW:
            case I_MOVSW:
            case I_CMPSW:
            case I_STOSW:
            case I_LODSW:
            case I_SCASW:
            case I_IRET:
            case I_CBW:
            case I_CWD:
                ++instr_name; /* ie: INSW -> INSD */
            }
        }
    }
    return( instr_name );
}


void DoControl( int base )
/************************/
{
    int op1,op2;

    if( D_Bit ) {
        op1 = OP_2;
        op2 = OP_1;
    } else {
        op1 = OP_1;
        op2 = OP_2;
    }
    CurrIns.op[ op1 ].mode = ADDR_REG;
    CurrIns.op[ op1 ].base = FIRST_DWORD_REG + ( SecondByte & 7 );
    CurrIns.op[ op2 ].mode = ADDR_REG;
    CurrIns.op[ op2 ].base = base + ( ( SecondByte >> 3 ) & 7 );
    CurrIns.num_oper = 2;
    GetDataByte();
}


void DoOTModRM( void )
/********************/
{
    int         reg;
    int         op1;
    int         op2;

    if( D_Bit ) {
        op1 = OP_2;
        op2 = OP_1;
    } else {
        op1 = OP_1;
        op2 = OP_2;
    }
    CurrIns.num_oper = 2;
    ModRMOp( op1, false );
    CurrIns.op[ op2 ].mode = ADDR_REG;
    reg = _RegField( SecondByte );
    if( W_Bit ) {
        CurrIns.op[ op2 ].base = VarReg( FIRST_WORD_REG + reg );
    } else {
        CurrIns.op[ op2 ].base = FIRST_BYTE_REG + reg;
    }
}


static void SetPWord( void )
/**************************/
{
    if( CurrIns.pref & OPND_LONG ) {
        CurrIns.modifier = MOD_PWORD;
        CurrIns.mem_ref_size = 6;
    } else {
        CurrIns.modifier = MOD_LINT;
        CurrIns.mem_ref_size = 4;
    }
}


static void MMModRM( int direction )
{
    D_Bit = direction;
    W_Bit = 0;
    DoOTModRM();
    CurrIns.modifier = MOD_NONE;

    if( CurrIns.op[ OP_1 ].mode == ADDR_REG ) {
        if( !(CurrIns.opcode == I_MOVD && direction == 0) ) {
            CurrIns.op[ OP_1 ].base += FIRST_MM_REG - FIRST_BYTE_REG;
        } else {
            CurrIns.op[ OP_1 ].base += FIRST_DWORD_REG - FIRST_BYTE_REG;
        }
    }
    if( CurrIns.op[ OP_2 ].mode == ADDR_REG ) {
        if( !(CurrIns.opcode == I_MOVD && direction == 1) ) {
            CurrIns.op[ OP_2 ].base += FIRST_MM_REG - FIRST_BYTE_REG;
        } else {
            CurrIns.op[ OP_2 ].base += FIRST_DWORD_REG - FIRST_BYTE_REG;
        }
    }
}


static  void    Opcode0F( bool use_32 ) {
/***************************************/

    int                 index;
    unsigned char       second;

    GetDataByte();
    second = SecondByte;
    SecondByte = GetNextByte();
    W_Bit = second & 1;
    D_Bit = 0;
    switch( ( second >> 4 ) & 0x0F ) {
    case 0x00:
        index = _RegField( SecondByte );
        switch( second ) {
        case 0:
            CurrIns.opcode = S00Ops[ index ];
            GetOperands( OT_RMW, use_32 );
            CurrIns.mem_ref_size = 2;
            break;
        case 1:
            CurrIns.opcode = S01Ops[ index ];
            if( index == 7 ) {  // INVLPG
                GetOperands( OT_M, use_32 );
                CurrIns.modifier = MOD_NONE;
                CurrIns.mem_ref_size = 1;
            } else {
                GetOperands( OT_RMW, use_32 );
                if( index <= 3 ) {      // SIDT SGDT LGDT LIDT
                    CurrIns.mem_ref_size = 6;
                } else {
                    CurrIns.mem_ref_size = 2;
                }
            }
            break;
        case 2:
            CurrIns.opcode = I_LAR;
            GetOperands( OT_RV_RMW, use_32 );
            break;
        case 3:
            CurrIns.opcode = I_LSL;
            GetOperands( OT_RV_RMW, use_32 );
            break;
        case 6:
            CurrIns.opcode = I_CLTS;
            break;
        case 8:
            CurrIns.opcode = I_INVD;
            break;
        case 9:
            CurrIns.opcode = I_WBINVD;
            break;
        default:
            CurrIns.opcode = I_INVALID;
        }
        break;
    case 0x02:
        CurrIns.opcode = I_MOV;
        D_Bit = second & 2;
        GetOperands( S2xOpType[ second & 0x0f ], use_32 );
        break;
    case 0x03:
        switch( second & 0x0f ) {
        case 0x00:
            CurrIns.opcode = I_WRMSR;
            break;
        case 0x01:
            CurrIns.opcode = I_RDTSC;
            break;
        case 0x02:
            CurrIns.opcode = I_RDMSR;
            break;
        case 0x03:
            CurrIns.opcode = I_RDMPC;
            break;
        default:
            CurrIns.opcode = I_INVALID;
            break;
        }
        break;
    case 0x04:
        CurrIns.opcode = S4xInsTab[ second & 0x0f ].name;
        GetOperands( S4xInsTab[ second & 0x0f ].op_type, use_32 );
        break;
    case 0x08:
        CurrIns.opcode = S8xOps[ second & 0x0f ];
        GetOperands( OT_IP_INC16, use_32 );
        break;
    case 0x09:
        CurrIns.opcode = S9xOps[ second & 0x0f ];
        GetOperands( OT_RMB, use_32 );
        break;
    case 0x0A:
        CurrIns.opcode = SAxOps[ second & 0x0f ];
        if( CurrIns.opcode == I_IMUL ) {
            D_Bit = 1;
        }
        GetOperands( SAxOpType[ second & 0x0f ], use_32 );
        break;
    case 0x0B:
        CurrIns.opcode = SBxOps[ second & 0x0f ];
        if( CurrIns.opcode == I_BSF || CurrIns.opcode == I_BSR ) D_Bit = 1;
        if( CurrIns.opcode == I_BTx ) {
            W_Bit = 1;
            CurrIns.opcode = SBAOps[ _RegField( SecondByte ) ];
        }
        GetOperands( SBxOpType[ second & 0x0f ], use_32 );
        break;
    case 0x0C:
        if( second & 0x08 ) {
            CurrIns.opcode = I_BSWAP;
            CurrIns.num_oper = 1;
            CurrIns.op[ OP_1 ].mode = ADDR_REG;
            CurrIns.op[ OP_1 ].base = VarReg( FIRST_WORD_REG + ( second & 7 ) );
        } else if( (second & 0x0f) == 0x07 ) {
            /* group 9 */
            if( (SecondByte & 0x38) == 0x08 ) {
                CurrIns.opcode = I_CMPXCHG8B;
                CurrIns.num_oper = 1;
                ModRMOp( OP_1, false );
            } else {
                CurrIns.opcode = I_INVALID;
            }
        } else {
            CurrIns.opcode = SCxOps[ second & 0x07 ];
            GetOperands( SCxOpType[ second & 0x07 ], use_32 );
        }
        break;
    case 0x06:
        CurrIns.opcode = S6xInsTab[ second & 0x0f ].name;
        GetOperands( S6xInsTab[ second & 0x0f ].op_type, use_32 );
        break;
    case 0x07:
        CurrIns.opcode = S7xInsTab[ second & 0x0f ].name;
        switch( second & 0x0f ) {
        case 0x1:
        case 0x2:
        case 0x3:
            /* a shift immediate opcode */
            switch( SecondByte & 0xf8 ) {
            case 0xF0: /* PSLL */
                break;
            case 0xD0: /* PSRL */
                CurrIns.opcode += 1;
                break;
            case 0xE0: /* PSRA */
                CurrIns.opcode += 2;
                if( CurrIns.opcode == I_PSRAQ ) {
                    /* doesn't exist */
                    CurrIns.opcode = I_INVALID;
                    return;
                }
                break;
            default:
                CurrIns.opcode = I_INVALID;
                return;
            }
            GetDataByte(); /* get set up for grabbing the immediate */
        }
        GetOperands( S7xInsTab[ second & 0x0f ].op_type, use_32 );
        break;
    case 0x0d:
        CurrIns.opcode = SDxInsTab[ second & 0x0f ].name;
        GetOperands( SDxInsTab[ second & 0x0f ].op_type, use_32 );
        break;
    case 0x0e:
        CurrIns.opcode = SExInsTab[ second & 0x0f ].name;
        GetOperands( SExInsTab[ second & 0x0f ].op_type, use_32 );
        break;
    case 0x0f:
        CurrIns.opcode = SFxInsTab[ second & 0x0f ].name;
        GetOperands( SFxInsTab[ second & 0x0f ].op_type, use_32 );
        break;
    default:
        CurrIns.opcode = I_INVALID;
        break;
    }
}


static  void  ConstByteOp( int op )
/*********************************/

{
    operand     *opnd;

    opnd = &CurrIns.op[ op ];
    opnd->mode = ADDR_CONST;
    opnd->size = 1;
    opnd->offset = InsOffset();
    opnd->disp = GetDataByte();
}


static void ConstSexOp( int op )
/******************************/

{
    ConstByteOp( op );
    CurrIns.op[ op ].size = ( CurrIns.pref & OPND_LONG ) ? 4 : 2;
}


static  void  ConstWordOp( int op )
/*********************************/

{
    operand     *opnd;

    opnd = &CurrIns.op[ op ];
    opnd->mode = ADDR_CONST;
    opnd->size = 2;
    opnd->offset = InsOffset();
    opnd->disp = GetDataWord();
}


static  void  ConstLongOp( int op )
/*********************************/

{
    operand     *opnd;

    opnd = &CurrIns.op[ op ];
    opnd->mode = ADDR_CONST;
    opnd->size = 4;
    opnd->offset = InsOffset();
    opnd->disp = GetDataLong();
}


static  void  ConstVarOp( int op )
/*********************************/

{
    if( CurrIns.pref & OPND_LONG ) {
        ConstLongOp( op );
    } else {
        ConstWordOp( op );
    }
}


static  void  AccumOp( int op )
/*****************************/

{
    CurrIns.op[ op ].mode = ADDR_REG;
    if( W_Bit ) {
        CurrIns.op[ op ].base = VarReg( AX_REG );
    } else {
        CurrIns.op[ op ].base = AL_REG;
    }
}

#define _FixDisp( x ) ( use_32 ? (x) : (x) & 0xFFFF )

static  void  GetOperands( operand_type op_type, bool use_32 )
/************************************************************/
{
    int                 op1;
    int                 op2;
    int                 reg;
    uint_32             disp;
    address_mode        mode;
    prefix              save_pref;

    op1 = OP_1;
    op2 = OP_2;
    switch( op_type ) {
    case OT_NULL:
        break;
    case OT_ES_DI:              /* stos, scas */
        if( CurrIns.seg_used == NULL_REG ) {
            CurrIns.seg_used = ES_REG;
        }
        /* fall through */
    case OT_DS_SI:              /* lods */
    case OT_DS_SI_ES_DI:        /* movs */
        CurrIns.mem_ref_op = OP_1;
        mode = ADDR_DS_SI + ( op_type - OT_DS_SI );
        if( CurrIns.pref & ADDR_LONG ) ++mode;  /* DS_SI -> DS_ESI */
        CurrIns.op[ OP_1 ].mode = mode;
        break;
    case OT_THREE:              /* int 3 */
        CurrIns.num_oper = 1;
        CurrIns.op[ OP_1 ].mode = ADDR_CONST;
        CurrIns.op[ OP_1 ].disp = 3;
        break;
    case OT_ENTER:              /* enter locals,level */
        CurrIns.num_oper = 2;
        ConstWordOp( OP_1 );
        ConstByteOp( OP_2 );
        break;
    case OT_DX_ACCUM:           /* out DX,AL */
        op1 = OP_2;
        op2 = OP_1;
        /* fall through */
    case OT_ACCUM_DX:           /* in AL,DX */
        CurrIns.num_oper = 2;
        AccumOp( op1 );
        CurrIns.op[ op2 ].mode = ADDR_REG;
        CurrIns.op[ op2 ].base = DX_REG;
        break;
    case OT_IMMED8_ACCUM:       /* out 10,AX */
        op1 = OP_2;
        op2 = OP_1;
        /* fall through */
    case OT_ACCUM_IMMED8:       /* in AL,10 */
        CurrIns.num_oper = 2;
        AccumOp( op1 );
        ConstByteOp( op2 );
        break;
    case OT_ACCUM_IMMED:        /* mov AL,7 */
        CurrIns.num_oper = 2;
        AccumOp( OP_1 );
        if( W_Bit ) {
            ConstVarOp( OP_2 );
        } else {
            ConstByteOp( OP_2 );
        }
        break;
    case OT_ACCUM_MEM:          /* mov AX,[a000] */
        if( D_Bit ) {
            op1 = OP_2;
            op2 = OP_1;
        }
        CurrIns.num_oper = 2;
        AccumOp( op1 );
        CurrIns.mem_ref_op = op2;
        if( CurrIns.pref & ADDR_LONG ) {
            ConstLongOp( op2 );
        } else {
            ConstWordOp( op2 );
        }
        CurrIns.op[ op2 ].mode = ADDR_ABS;
#if defined( O2A )
        if( Options & FORM_ASSEMBLER ) {
            if( W_Bit == 0 ) {
                CurrIns.modifier = MOD_BYTE;
            } else if( CurrIns.pref & OPND_LONG ) {
                CurrIns.modifier = MOD_LINT;
            } else {
                CurrIns.modifier = MOD_WORD;
            }
        }
#endif
        break;
    case OT_ACCUM_REG16:        /* xchg AX,CX */
        CurrIns.num_oper = 2;
        CurrIns.op[ OP_1 ].mode = ADDR_REG;
        CurrIns.op[ OP_1 ].base = VarReg( AX_REG );
        CurrIns.op[ OP_2 ].mode = ADDR_REG;
        CurrIns.op[ OP_2 ].base = VarReg( FIRST_WORD_REG + ( Opcode & 7 ) );
        break;
    case OT_ESC:                /* float instructions */
        GetFppOp();
        break;
    case OT_IMMED:              /* push $12 */
        CurrIns.num_oper = 1;
        ConstSexOp( OP_1 );
        break;
    case OT_IMMED8:             /* int $21 */
        CurrIns.num_oper = 1;
        ConstByteOp( OP_1 );
        break;
    case OT_IMMED_WORD:         /* ret 2 */
        CurrIns.num_oper = 1;
        ConstWordOp( OP_1 );
        break;
    case OT_IMMED16:            /* ret 2 */
        CurrIns.num_oper = 1;
        ConstVarOp( OP_1 );
        break;
    case OT_IP_INC8:            /* je L1 */
    case OT_IP_INC16:           /* call/jmp L2 */
        CurrIns.num_oper = 1;
        CurrIns.op[ OP_1 ].mode = ADDR_LABEL;
        CurrIns.op[ OP_1 ].offset = InsOffset();
        if( op_type == OT_IP_INC8 ) {
            CurrIns.op[ OP_1 ].size = 1;
            disp = GetDataByte();
        } else if ( CurrIns.pref & OPND_LONG ) {
            CurrIns.op[ OP_1 ].size = 4;
            disp = GetDataLong();
        } else {
            CurrIns.op[ OP_1 ].size = 2;
            disp = GetDataWord();
        }
        disp += GetOffset();
        CurrIns.op[ OP_1 ].disp = _FixDisp( disp );
        break;
    case OT_MEM_IMMED8:
    case OT_MEM_IMMED16:        /* mov 2[BX],4 */
        if( op_type == OT_MEM_IMMED8 ) {
            CurrIns.mem_ref_size = 1;
        } else if( CurrIns.pref & OPND_LONG ) {
            CurrIns.mem_ref_size = 4;
        } else {
            CurrIns.mem_ref_size = 2;
        }
        CurrIns.num_oper = 2;
        ModRMOp( OP_1, true );
        if( CurrIns.mem_ref_size == 1 ) {
            ConstByteOp( OP_2 );
        } else {
            ConstVarOp( OP_2 );
        }
        break;
    case OT_MOD_RM_IMMED8:      /* shld eax,ebx,5 */
        W_Bit = 1;
        DoOTModRM();
        ConstByteOp( OP_3 );
        CurrIns.num_oper = 3;
        break;
    case OT_MOD_RM_CL:          /* shrd ecx,ebx,cl */
        W_Bit = 1;
        DoOTModRM();
        CurrIns.op[ OP_3 ].mode = ADDR_REG;
        CurrIns.op[ OP_3 ].base = CL_REG;
        CurrIns.num_oper = 3;
        break;
    case OT_CC_MODRM:
        W_Bit = 1;
        D_Bit = 1;
        DoOTModRM();
        break;
    case OT_MOD_RM_NOSIZE:      /* bt edx,eax */
        W_Bit = 1;
        CurrIns.mem_ref_size = 1;
        /* fall through */
    case OT_MOD_RM:             /* add al,3[di] */
        DoOTModRM();
        break;
    case OT_MOD_REG:            /* lea AX,3[SI] */
        CurrIns.num_oper = 2;
        ModRMRegOp( OP_1 );
        ModRMOp( OP_2, false );
        if( Opcode == 0x8d ) {      /* lea */
            CurrIns.mem_ref_op = NULL_OP;
        } else {                    /* lds or les */
            SetPWord();
        }
        break;
    case OT_REG8_IMMED:         /* mov AL,32 */
        CurrIns.num_oper = 2;
        CurrIns.op[ OP_1 ].mode = ADDR_REG;
        CurrIns.op[ OP_1 ].base = FIRST_BYTE_REG + ( Opcode & 7 );
        ConstByteOp( OP_2 );
        break;
    case OT_REG16_IMMED:        /* mov CX,1 */
        CurrIns.num_oper = 2;
        CurrIns.op[ OP_1 ].mode = ADDR_REG;
        CurrIns.op[ OP_1 ].base = VarReg( FIRST_WORD_REG + ( Opcode & 7 ) );
        ConstVarOp( OP_2 );
        break;
    case OT_REG16:              /* inc AX */
        CurrIns.num_oper = 1;
        CurrIns.op[ OP_1 ].mode = ADDR_REG;
        CurrIns.op[ OP_1 ].base = VarReg( FIRST_WORD_REG + ( Opcode & 7 ) );
        break;
    case OT_RM:                 /* pop 2[BX] */
        CurrIns.num_oper = 1;
        reg = _RegField( SecondByte );
        if( reg <= 1 && Opcode != 0x8f ) {      /* INC or DEC (not POP) */
            ModRMOp( OP_1, true );
        } else {
            ModRMOp( OP_1, false );
            if( reg == 3 || reg == 5 ) {        /* call/jmp dword ptr x */
                SetPWord();
            } else if( reg == 2 || reg == 4 ) { /* call/jmp word ptr x */
                if( CurrIns.pref & OPND_LONG ) {
                    CurrIns.modifier = MOD_LINT;
                } else {
                    CurrIns.modifier = MOD_WORD;
                }
            }
        }
        break;
    case OT_RM_1:               /* shl AX,1 */
        CurrIns.num_oper = 2;
        ModRMOp( OP_1, true );
        CurrIns.op[ OP_2 ].mode = ADDR_CONST;
        CurrIns.op[ OP_2 ].disp = 1;
        break;
    case OT_RM_CL:              /* shl AX,CL */
        CurrIns.num_oper = 2;
        ModRMOp( OP_1, true );
        CurrIns.op[ OP_2 ].mode = ADDR_REG;
        CurrIns.op[ OP_2 ].base = CL_REG;
        break;
    case OT_RM_TEST:            /* test -9[BP],10 */
        ModRMOp( OP_1, true );
        reg = _RegField( SecondByte );
        if( reg != 0 ) {        /* TEST has 2 opnds, else only 1 opnd */
            CurrIns.num_oper = 1;
        } else {
            CurrIns.num_oper = 2;
            if( Opcode != 0xf6 ) {
                ConstVarOp( OP_2 );
            } else {
                ConstByteOp( OP_2 );
            }
        }
        break;
    case OT_RM_IMMED:           /* add 4[DI],8 */
        CurrIns.num_oper = 2;
        ModRMOp( OP_1, true );
        if( W_Bit == 0 ) {          /* byte operation */
            ConstByteOp( OP_2 );
        } else if( D_Bit == 0 ) {   /* word operation, with full constant */
            ConstVarOp( OP_2 );
        } else {           /* D_Bit == 1: word op'n, with sign-ext'd constant */
            ConstSexOp( OP_2 );
        }
        break;
    case OT_SEG_ADDR:           /* call seg:addr */
        CurrIns.num_oper = 1;
        CurrIns.op[ OP_1 ].mode = ADDR_SEG_OFFSET;
        CurrIns.op[ OP_1 ].size = 4;
        CurrIns.op[ OP_1 ].offset = InsOffset();
        if( CurrIns.pref & OPND_LONG ) {
            CurrIns.op[ OP_1 ].disp = _FixDisp( GetDataLong() );
        } else {
            CurrIns.op[ OP_1 ].disp = _FixDisp( GetDataWord() );
        }
        CurrIns.op[ OP_2 ].disp = _FixDisp( GetDataWord() );
        break;
    case OT_SEG_OP:             /* push ES */
        CurrIns.num_oper = 1;
        CurrIns.op[ OP_1 ].mode = ADDR_REG;
        CurrIns.op[ OP_1 ].base = FIRST_SEG_REG + _SegReg2Field( Opcode );
        break;
    case OT_SR_RM:              /* mov DS,AX */
        if( D_Bit ) {
            op1 = OP_2;
            op2 = OP_1;
        }
        CurrIns.num_oper = 2;
        W_Bit = 1;
        CurrIns.mem_ref_size = 2;
        save_pref = CurrIns.pref;
        CurrIns.pref &= ~OPND_LONG;
        ModRMOp( op1, false );
        CurrIns.pref = save_pref;
        CurrIns.op[ op2 ].mode = ADDR_REG;
        CurrIns.op[ op2 ].base = FIRST_SEG_REG + _SegReg3Field( SecondByte );
        break;
    case OT_REG_MEM_IMMED8:
    case OT_REG_MEM_IMMED16: /* imul eAX,eBX,3 */
        if( CurrIns.pref & OPND_LONG ) {
            CurrIns.mem_ref_size = 4;
        } else {
            CurrIns.mem_ref_size = 2;
        }
        reg = _RegField( SecondByte );
        if( _ModField( SecondByte ) != 3 || _RMField( SecondByte ) != reg ) {
            CurrIns.num_oper = 3;
            CurrIns.op[ OP_1 ].mode = ADDR_REG;
            CurrIns.op[ OP_1 ].base = VarReg( FIRST_WORD_REG + reg );
            op1 = OP_2;
            op2 = OP_3;
        } else {
            CurrIns.num_oper = 2;
        }
        ModRMOp( op1, false );
        if( op_type == OT_REG_MEM_IMMED8 ) {
            ConstByteOp( op2 );
        } else {
            ConstVarOp( op2 );
        }
        break;
    case OT_286:    /* secondary opcode space */
        Opcode0F( use_32 );
        break;
    case OT_RMW: /* SLDT etc */
        /* mem_ref_size ????????????? */
        CurrIns.num_oper = 1;
        W_Bit = 1;
        save_pref = CurrIns.pref;
        CurrIns.pref &= ~OPND_LONG;
        ModRMOp( OP_1, true );
        CurrIns.pref = save_pref;
        break;
    case OT_M: /* SGDT etc */
        CurrIns.num_oper = 1;
        ModRMOp( OP_1, false );
        CurrIns.modifier = MOD_PWORD;
        CurrIns.mem_ref_size = 6;
        break;
    case OT_RV_RMW: /* LAR, LSL */
        CurrIns.num_oper = 2;
        ModRMRegOp( OP_1 );
        W_Bit = 1;
        save_pref = CurrIns.pref;
        CurrIns.pref &= ~OPND_LONG;
        ModRMOp( OP_2, true );
        CurrIns.pref = save_pref;
        break;
    case OT_RV_RMB: /* MOVSX  eAX,byte ptr foo*/
        CurrIns.mem_ref_size = 1;
        CurrIns.num_oper = 2;
        ModRMRegOp( OP_1 );
        ModRMOp( OP_2, true );
        break;
    case OT_RD_RMW: /* MOVZX  EAX,word ptr foo*/
        CurrIns.mem_ref_size = 2;
        CurrIns.num_oper = 2;
        CurrIns.op[ OP_1 ].mode = ADDR_REG;
        CurrIns.op[ OP_1 ].base = FIRST_DWORD_REG + _RegField( SecondByte );
        save_pref = CurrIns.pref;
        CurrIns.pref &= ~OPND_LONG;
        ModRMOp( OP_2, true );
        CurrIns.pref = save_pref;
        break;
    case OT_RMB: /* SETBE etc */
        CurrIns.num_oper = 1;
        CurrIns.mem_ref_size = 1;
        W_Bit = 0;
        ModRMOp( OP_1, true );
        break;
    case OT_FS:
        CurrIns.op[ OP_1 ].base = FS_REG;
        CurrIns.op[ OP_1 ].mode = ADDR_REG;
        CurrIns.num_oper = 1;
        break;
    case OT_GS:
        CurrIns.op[ OP_1 ].base = GS_REG;
        CurrIns.op[ OP_1 ].mode = ADDR_REG;
        CurrIns.num_oper = 1;
        break;
    case OT_DEBUG:
        DoControl( FIRST_DEBUG_REG );
        break;
    case OT_CONTROL:
        DoControl( FIRST_CONTROL_REG );
        break;
    case OT_TEST:
        DoControl( FIRST_TEST_REG );
        break;
    case OT_NYI:
        break;
    case OT_EXTRA: /* AAD, AAM */
        GetDataByte();
        break;
    case OT_MM_MODRM:
        MMModRM( 1 );
        break;
    case OT_MODRM_MM:
        MMModRM( 0 );
        break;
    case OT_MM_IMMED8:
        CurrIns.num_oper = 2;
        CurrIns.op[ OP_1 ].mode = ADDR_REG;
        CurrIns.op[ OP_1 ].base = FIRST_MM_REG + _RMField( SecondByte );
        ConstByteOp( OP_2 );
        break;
    }
}


void  ModRMRegOp( int op )
/************************/
{
    int         reg;

    reg = _RegField( SecondByte );
    CurrIns.op[ op ].mode = ADDR_REG;
    CurrIns.op[ op ].base = VarReg( FIRST_WORD_REG + reg );
}


void  ModRMOp( int op, bool byte_ptr )
/************************************/
{
    uint_8              mod;
    uint_8              rm;
    uint_8              base;
    operand             *opnd;

#if defined( O2A )
    if( Options & FORM_ASSEMBLER ) {        /* fully qualify everything */
        byte_ptr = true;
    }
#endif
    CurrIns.mem_ref_op = op;
    GetDataByte();
    mod = _ModField( SecondByte );
    rm = _RMField( SecondByte );
    opnd = &CurrIns.op[ op ];
    if( mod == 3 ) {        /* register */
        CurrIns.mem_ref_op = NULL_OP;
        opnd->mode = ADDR_REG;
        if( W_Bit ) {
            opnd->base = VarReg( FIRST_WORD_REG + rm );
        } else {
            opnd->base = FIRST_BYTE_REG + rm;
        }
        byte_ptr = false;
    } else if( CurrIns.pref & ADDR_LONG ) {
        if( rm == 4 ) { /* s-i-b byte follows */
            rm = GetDataByte();
            opnd->scale = 1 << _ScaleField( rm );
            base = _BaseField( rm );
            opnd->index = RM32Reg[ _IndexField( rm ) ];
            if( mod == 0 && base == 5 ) {
                if( opnd->index == ESP_REG ) {
                    opnd->index = 0;
                    opnd->scale = 0;
                    opnd->mode = ADDR_ABS;
                    if( CurrIns.seg_used == NULL_REG ) {
                        CurrIns.seg_used = DS_REG;
                    }
                } else {
                    opnd->mode = ADDR_INDEX;
                }
                opnd->offset = InsOffset();
                opnd->disp = GetDataLong();
                opnd->size = 4;
            } else {
                opnd->base = RM32Reg[ base ];
                if( opnd->index == ESP_REG ) {
                    opnd->index = 0;
                    opnd->scale = 0;
                    opnd->mode = ADDR_BASE;
                } else {
                    opnd->mode = ADDR_BASE_INDEX;
                }
                if( mod == 1 ) {
                    opnd->offset = InsOffset();
                    opnd->disp = GetDataByte();
                    opnd->size = 1;
                } else if( mod == 2 ) {
                    opnd->offset = InsOffset();
                    opnd->disp = GetDataLong();
                    opnd->size = 4;
                }
            }
            if( CurrIns.seg_used == NULL_REG ) {
                CurrIns.seg_used = RM32Seg[ base ];
            }
        } else if ( mod == 0 && rm == 5 ) { /* direct address */
            opnd->offset = InsOffset();
            opnd->disp = GetDataLong();
            opnd->size = 4;
            opnd->mode = ADDR_ABS;
            if( CurrIns.seg_used == NULL_REG ) {
                CurrIns.seg_used = DS_REG;
            }
        } else {
            opnd->mode = ADDR_BASE;
            opnd->index = 0;
            opnd->scale = 1;
            opnd->base = RM32Reg[ rm ];
            if( mod == 1 ) {
                opnd->offset = InsOffset();
                opnd->disp = GetDataByte();
                opnd->size = 1;
            } else if ( mod ==  2 ) {
                opnd->offset = InsOffset();
                opnd->disp = GetDataLong();
                opnd->size = 4;
            }
            if( CurrIns.seg_used == NULL_REG ) {
                CurrIns.seg_used = RM32Seg[ rm ];
            }
        }
    } else {
        if( mod == 0 && rm == 6 ) {    /* direct address */
            ConstWordOp( op );
            opnd->mode = ADDR_ABS;
            if( CurrIns.seg_used == NULL_REG ) {
                CurrIns.seg_used = DS_REG;
            }
        } else {                        /* disp[ base+index ] */
            opnd->mode = RMMode[ rm ];
            opnd->base = RMBase[ rm ];
            opnd->index = RMIndex[ rm ];
            opnd->scale = 1;
            if( mod == 0 ) {
                opnd->disp = 0;
            } else if( mod == 1 ) {
                opnd->offset = InsOffset();
                opnd->disp = GetDataByte();
                opnd->size = 1;
            } else if( mod == 2 ) {
                opnd->offset = InsOffset();
                opnd->disp = GetDataWord();
                opnd->size = 2;
            }
            if(CurrIns.mem_ref_op != NULL_OP && CurrIns.seg_used == NULL_REG) {
                CurrIns.seg_used = RMSeg[ rm ];
            }
        }
    }
    if( byte_ptr ) {
        if( W_Bit == 0 ) {
            CurrIns.modifier = MOD_BYTE;
        } else if( CurrIns.pref & OPND_LONG ) {
            CurrIns.modifier = MOD_LINT;
        } else {
            CurrIns.modifier = MOD_WORD;
        }
    }
}
