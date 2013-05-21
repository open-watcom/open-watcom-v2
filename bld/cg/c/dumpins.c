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
* Description:  Dump instruction.
*
****************************************************************************/


#include "cgstd.h"
#include "cgdefs.h"
#include "coderep.h"
#include "pattern.h"
#include "opcodes.h"
#include "cfloat.h"
#include "dump.h"
#include "feprotos.h"

extern  void            DumpRegName(hw_reg_set);
extern  bool            DumpFPUIns(instruction*);
extern  void            DumpNL();
extern  void            DumpPtr(pointer);
extern  void            DumpId(unsigned);
extern  void            DumpOpcodeName(int);
extern  void            DumpBlkId(block*);
extern  void            DumpInt(int);
extern  void            DumpLong(signed_32);
extern  void            DumpPossible(byte);
extern  void            DumpTab(opcode_entry*);
extern  name            *DeAlias(name*);
extern  char            *AskName(pointer,cg_class);
extern  void            Dump8h(unsigned_32);
extern  void            DumpChar(char);
extern  hw_reg_set      HighReg(hw_reg_set);
extern  hw_reg_set      LowReg(hw_reg_set);
extern  void            DumpGBit(global_bit_set*);
extern  void            DumpLBit(local_bit_set*);
extern  void            DumpByte( byte n );
extern  type_length     FlagsToAlignment( i_flags );


extern  name            *Names[];

static  void    DoOffset( char *str, unsigned_32 o ) {
/****************************************************/

    DumpString( str );
    DumpChar( '\t' );
    Dump8h( o );
    DumpNL();
}


extern  void    DumpInsOffsets() {
/********************************/

    DoOffset( "head.prev", offsetof( instruction, head.prev ) );
    DoOffset( "head.next", offsetof( instruction, head.next ) );
    DoOffset( "head.live", offsetof( instruction, head.live ) );
    DoOffset( "head.line_num", offsetof( instruction, head.line_num ) );
    DoOffset( "head.opcode", offsetof( instruction, head.opcode ) );
    DoOffset( "head.state", offsetof( instruction, head.state ) );
    DoOffset( "table\t", offsetof( instruction, table ) );
    DoOffset( "u.gen_table", offsetof( instruction, u.gen_table ) );
    DoOffset( "u2.cse_link", offsetof( instruction, u2.cse_link ) );
    DoOffset( "u2.parm_list", offsetof( instruction, u2.parm_list ) );
    DoOffset( "zap\t", offsetof( instruction, zap ) );
    DoOffset( "result\t", offsetof( instruction, result ) );
    DoOffset( "id\t", offsetof( instruction, id ) );
    DoOffset( "type_class", offsetof( instruction, type_class ) );
    DoOffset( "base_type_class", offsetof( instruction, base_type_class ) );
    DoOffset( "sequence", offsetof( instruction, sequence ) );
    DoOffset( "flags.byte", offsetof( instruction, flags.byte ) );
    DoOffset( "flags.bool", offsetof( instruction, flags.bool ) );
    DoOffset( "flags.call_flag", offsetof( instruction, flags.call_flags ) );
    DoOffset( "flags.nop_flags", offsetof( instruction, flags.nop_flags ) );
    DoOffset( "t.index_needs", offsetof( instruction, t.index_needs ) );
    DoOffset( "t.stk_max", offsetof( instruction, t.stk_max ) );
    DoOffset( "stk_entry", offsetof( instruction, stk_entry ) );
    DoOffset( "num_operands", offsetof( instruction, num_operands ) );
    DoOffset( "ins_flags", offsetof( instruction, ins_flags ) );
    DoOffset( "stk_exit", offsetof( instruction, stk_exit ) );
    DoOffset( "s.stk_extra", offsetof( instruction, s.stk_extra ) );
    DoOffset( "operands[ 0 ]", offsetof( instruction, operands[ 0 ] ) );
}


extern  void    DumpInOut( instruction *ins ) {
/*********************************************/

    DumpLiteral( "     " );
    DumpGBit( &ins->head.live.out_of_block );
    DumpLiteral( " " );
    DumpLBit( &ins->head.live.within_block );
    if( !HW_CEqual( ins->head.live.regs, HW_EMPTY ) &&
        !HW_COvlap( ins->head.live.regs, HW_UNUSED ) ) {
        DumpLiteral( "  " );
        DumpRegName( ins->head.live.regs );
    }
    DumpNL();
}


extern  void    DumpITab( instruction *ins ) {
/********************************************/

    if( ins->u.gen_table != NULL ) DumpTab( ins->u.gen_table );
}


static char * Types[] = {
/***********************/

    " U1 ",
    " I1 ",
    " U2 ",
    " I2 ",
    " U4 ",
    " I4 ",
    " U8 ",
    " I8 ",
    " CP ",
    " PT ",
    " FS ",
    " FD ",
    " FL ",
    " XX ",
    ""
};


extern  void    DumpClass( type_class_def tipe ) {
/***********************************************/

    DumpString( Types[ tipe ] );
}


extern  void    DumpOperand( name *operand ) {
/********************************************/

    char        buffer[20];
    hw_reg_set  reg;
    name        *base;

    if( operand->n.class == N_INDEXED ) {
        if( operand->i.base != NULL ) {
            if( !( operand->i.index_flags & X_FAKE_BASE ) ) {
                if( operand->i.index_flags & X_LOW_ADDR_BASE ) {
                    DumpLiteral( "l^" );
                }
                DumpOperand( operand->i.base );
                if( operand->i.constant > 0 ) {
                    DumpLiteral( "+" );
                }
            }
        }
        if( operand->i.constant != 0 ) {
            DumpLong( operand->i.constant );
        }
        DumpLiteral( "[" );
        if( operand->i.index_flags & X_BASE ) {
            reg = operand->i.index->r.reg;
#if _TARGET & ( _TARG_IAPX86 | _TARG_80386 )
            if( HW_COvlap( reg, HW_SEGS ) ) {

                hw_reg_set tmp;

                tmp = reg;
                HW_COnlyOn( tmp, HW_SEGS );
                DumpRegName( tmp );
                DumpLiteral( ":" );
                HW_CTurnOff( reg, HW_SEGS );
            }
#endif
            if( operand->i.index_flags & X_HIGH_BASE ) {
                DumpRegName( HighReg( reg ) );
                DumpLiteral( "+" );
                DumpRegName( LowReg( reg ) );
            } else {
                DumpRegName( LowReg( reg ) );
                DumpLiteral( "+" );
                DumpRegName( HighReg( reg ) );
            }
        } else {
            DumpOperand( operand->i.index );
        }
        if( operand->i.scale != 0 ) {
            DumpLiteral( "*" );
            DumpInt( 1 << operand->i.scale );
        }
        if( operand->i.index_flags & ( X_ALIGNED_1 | X_ALIGNED_2 | X_ALIGNED_4 | X_ALIGNED_8 ) ) {
            DumpLiteral( "$" );
            DumpInt( FlagsToAlignment( operand->i.index_flags ) );
        }
        DumpLiteral( "]" );
        base = operand->i.base;
        if( base != NULL ) {
            if( operand->i.index_flags & X_FAKE_BASE ) {
                DumpLiteral( "{" );
                if( base->n.class == N_MEMORY ) {
                    DumpXString( AskName(base->v.symbol, base->m.memory_type) );
                } else if( base->n.class == N_TEMP ) {
                    if( _FrontEndTmp( base ) ) {
                        DumpXString( FEName( base->v.symbol ) );
                    } else {
                        DumpOperand( base );
                    }
                }
                DumpLiteral( "}" );
            }
        }
    } else if( operand->n.class == N_CONSTANT ) {
        if( operand->c.const_type == CONS_ABSOLUTE ) {
            if( operand->c.int_value != 0 ) {
                if( operand->c.int_value_2 != 0
                && operand->c.int_value_2 != -1 )
                    Dump8h( operand->c.int_value_2 );
                Dump8h( operand->c.int_value );
            } else {
                CFCnvFS( operand->c.value, buffer, 20 );
                DumpXString( buffer );
            }
        } else {
            if( operand->c.const_type == CONS_SEGMENT ) {
                DumpLiteral( "SEG(" );
                if( operand->c.value == NULL ) {
                    DumpInt( operand->c.int_value );
                } else {
                    DumpOperand( operand->c.value );
                }
            } else if( operand->c.const_type == CONS_OFFSET ) {
                DumpLiteral( "OFFSET(" );
#if _TARGET == _TARG_370
                DumpInt( operand->c.int_value );
#else
                DumpOperand( operand->c.value );
#endif
            } else if( operand->c.const_type == CONS_ADDRESS ) {
                DumpLiteral( "ADDRESS(" );
                DumpOperand( operand->c.value );
            } else if( operand->c.const_type == CONS_TEMP_ADDR ) {
                DumpLiteral( "TMPADDR(" );
                DumpOperand( operand->c.value );
            } else if( operand->c.const_type == CONS_HIGH_ADDR ) {
                DumpLiteral( "HIGH_ADDR(h^" );
                if( operand->c.value != NULL ) {
                    DumpOperand( operand->c.value );
                } else {
                    if( operand->c.int_value != 0 ) {
                        DumpLong( operand->c.int_value );
                    } else {
                        DumpLiteral( "NULL" );
                    }
                }
            }
            DumpLiteral( ")" );
        }
    } else if( operand->n.class == N_MEMORY ) {
        DumpXString( AskName( operand->v.symbol, operand->m.memory_type ) );
        if( operand->m.memory_type != CG_FE ) {
            DumpPtr( operand->v.symbol );
        }
        if( operand->v.offset > 0 ) {
            DumpLiteral( "+" );
            DumpLong( operand->v.offset );
        } else if( operand->v.offset < 0 ) {
            DumpLong( operand->v.offset );
        }
    } else if( operand->n.class == N_TEMP ) {
        DumpLiteral( "t" );
        DumpInt( operand->t.v.id );
        if( operand->v.offset > 0 ) {
            DumpLiteral( "+" );
            DumpLong( operand->v.offset );
        } else if( operand->v.offset < 0 ) {
            DumpLong( operand->v.offset );
        }
        if( _FrontEndTmp( operand ) ) {
            DumpLiteral( "(" );
            DumpXString( FEName( operand->v.symbol ) );
            DumpLiteral( ")" );
        } else if( operand->v.symbol != NULL ) {
            DumpLiteral( "(" );
            DumpOperand( (name *)operand->v.symbol );
            DumpLiteral( ")" );
        }
    } else if( operand->n.class == N_REGISTER ) {
        DumpRegName( operand->r.reg );
    } else {
        DumpLiteral( "Unknown class " );
        DumpInt( operand->n.class );
    }
}


extern  void DoDumpIInfo( instruction *ins, bool fp ) {
/*****************************************************/

    if( ins->ins_flags & INS_DEMOTED ) {
        DumpLiteral( "d" );
    } else {
        DumpLiteral( " " );
    }
    if( ins->ins_flags & INS_PROMOTED ) {
        DumpLiteral( "p" );
    } else {
        DumpLiteral( " " );
    }
    if( ins->ins_flags & INS_RISCIFIED ) {
        DumpLiteral( "r" );
    } else {
        DumpLiteral( " " );
    }
    if( _OpIsIFunc( ins->head.opcode ) || _OpIsCall( ins->head.opcode ) || fp ) {
        DumpByte( ins->sequence );
        DumpChar( ' ' );
        DumpChar( ins->stk_entry + '0' );
        DumpChar( ins->stk_exit + '0' );
        DumpChar( ins->s.stk_depth + '0' );
    } else {
        DumpLiteral( "     " );
    }
    if( ins->ins_flags & INS_CC_USED ) {
        DumpLiteral( "c" );
    } else {
        DumpLiteral( " " );
    }
    if( ins->head.opcode == OP_CALL ) {
        DumpLiteral( " " );
        DumpOperand( ins->operands[ CALL_OP_ADDR ] );
    }
    DumpClass( ins->type_class );
    if( ins->head.opcode == OP_CONVERT ) {
        DumpClass( ins->base_type_class );
    }
    if( _OpIsCondition( ins->head.opcode ) ) {
        if( _TrueIndex( ins ) == 0 ) {
            DumpLiteral( "T=0 " );
        } else {
            DumpLiteral( "T=1 " );
        }
    }
}


extern  void DumpFPInfo( instruction *ins ) {
/*******************************************/

    DoDumpIInfo( ins, TRUE );
}


extern  void DumpIInfo( instruction *ins ) {
/*******************************************/

    DoDumpIInfo( ins, FALSE );
}


extern void DumpInsOnly( instruction *ins ) {
/*******************************************/

    int i;

    DumpOpcodeName( ins->head.opcode );
    DumpIInfo( ins );
    if( ins->num_operands != 0 ) {
        DumpOperand( ins->operands[ 0 ] );
        i = 0;
        while( ++i < ins->num_operands ) {
            DumpLiteral( ", " );
            DumpOperand( ins->operands[ i ] );
        }
    }
    if( ins->result != NULL ) {
        DumpLiteral( " ==> " );
        DumpOperand( ins->result );
    }
}


extern  void    DumpLineNum( instruction *ins ) {
/***********************************************/

    if( ins->head.line_num != 0 ) {
        DumpLiteral( "    Line number=" );
        DumpInt( ins->head.line_num );
        DumpNL();
    }
}


extern  void    DumpInsNoNL( instruction *ins ) {
/*******************************************/

    DumpLineNum( ins );
    DumpPtr( ins );
    DumpLiteral( " " );
    if( ins->head.opcode == OP_BLOCK ) {
        DumpId( 9999 );
        DumpLiteral( ":  " );
        DumpLiteral( "BLOCK" );
    } else {
        DumpId( ins->id );
        DumpLiteral( ":  " );
        #if _TARGET & (_TARG_80386|_TARG_IAPX86)
            if( DumpFPUIns( ins ) ) return;
        #endif
        DumpInsOnly( ins );
        if( !HW_CEqual( ins->zap->reg, HW_EMPTY ) ) {
            DumpLiteral( "           zaps " );
            DumpRegName( ins->zap->reg );
        }
    }
}


extern  void    DumpIns( instruction *ins ) {
/*******************************************/

   DumpInsNoNL( ins );
   DumpNL();
}


extern void DumpInstrsOnly( block *blk ) {
/****************************************/

    instruction *ins;

    ins = blk->ins.hd.next;
    while( ins->head.opcode != OP_BLOCK ) {
        DumpIns( ins );
        ins = ins->head.next;
    }
}


extern  void    DumpCond( instruction *ins, block *blk ) {
/********************************************************/

    int         i;

    if( !_OpIsCondition( ins->head.opcode ) ) return;
    if( ins->result == NULL ) {
        i = _TrueIndex( ins );
        if( i != NO_JUMP ) {
            DumpLiteral( " then " );
            DumpBlkId( blk->edge[  i  ].destination );
        }
        i = _FalseIndex( ins );
        if( i != NO_JUMP ) {
            DumpLiteral( " else " );
            DumpBlkId( blk->edge[  i  ].destination );
        }
    }
}


static char * Usage[] = {
/***********************/

    "USE_IN_BLOCK, ",
    "USE_IN_OTHER, ",
    "DEF_IN_BLOCK, ",
    "USE_ADDRESS , ",
    "USE_MEMORY  , ",
    "VAR_VOLATILE, ",
    "HAS_MEMORY  , ",
    "NEEDS_MEMORY, ",
    ""
};


extern  void    DumpVUsage( name *v ) {
/*************************************/

    var_usage   u;
    int         i;
    int         j;

    u = v->v.usage;
    DumpLiteral( "  Usage " );
    i = 0;
    j = 0;
    for(;;) {
        if( u & 1 ) {
            j += 14;
            if( j > 71 ) {
                DumpNL();
                j = 0;
            }
            DumpString( Usage[ i ] );
        }
        u >>= 1;
        ++ i;
        if( u == 0 ) break;
    }
    DumpNL();
}


extern  void    DumpSym( name *sym ) {
/************************************/

    DumpPtr( sym );
    DumpLiteral( " " );
    DumpOperand( sym );
    DumpClass( sym->n.name_class );
    if( sym->n.name_class == XX ) {
        DumpLong( sym->n.size );
    }
    if( sym->n.class == N_MEMORY || sym->n.class == N_TEMP ) {
        DumpNL();
        DumpVUsage( sym );
        DumpLiteral( "  offset " );
        DumpLong( sym->v.offset );
        if( sym->n.class == N_TEMP ) {
            DumpLiteral( "  location " );
            DumpLong( sym->t.location );
            DumpLiteral( "  block id " );
            DumpInt( sym->t.u.block_id );
            // DumpPossible( sym->t.possible );
            if( sym->t.temp_flags & ALIAS ) {
                DumpLiteral( " ALIAS " );
                sym = DeAlias( sym );
                DumpPtr( sym );
            } else if( !( sym->t.temp_flags & ALIAS ) ) {
                DumpLiteral( " MASTER" );
            }
            if( sym->t.temp_flags & VISITED ) {
                DumpLiteral( " VISITED" );
            }
            if( sym->t.temp_flags & INDEXED ) {
                DumpLiteral( " INDEXED" );
            }
            if( sym->t.temp_flags & CAN_STACK ) {
                DumpLiteral( " CAN_STACK" );
            }
            if( sym->t.temp_flags & PUSH_LOCAL ) {
                DumpLiteral( " PUSH_LOCAL" );
            }
            if( sym->t.temp_flags & HAD_CONFLICT ) {
                DumpLiteral( " HAD_CONFLICT" );
            }
            if( sym->t.temp_flags & STACK_PARM ) {
                DumpLiteral( " STACK_PARM" );
            }
            if( sym->t.temp_flags & CROSSES_BLOCKS ) {
                DumpLiteral( " CROSSES_BLOCKS" );
            }
        } else {
            DumpLiteral( "  alignment " );
            DumpLong( sym->m.alignment );
        }
    } else if( sym->n.class == N_INDEXED ) {
        if( sym->i.index_flags & X_SEGMENTED ) {
            DumpLiteral( " X_SEGMENTED" );
        }
        if( sym->i.index_flags & X_VOLATILE ) {
            DumpLiteral( " X_VOLATILE" );
        }
        if( sym->i.index_flags & X_ALIGNED_1 ) {
            DumpLiteral( " X_UNALIGNED" );
        }
        if( sym->i.index_flags & X_ALIGNED_2 ) {
            DumpLiteral( " X_ALIGNED_2" );
        }
        if( sym->i.index_flags & X_ALIGNED_4 ) {
            DumpLiteral( " X_ALIGNED_4" );
        }
        if( sym->i.index_flags & X_ALIGNED_8 ) {
            DumpLiteral( " X_ALIGNED_8" );
        }
    }
    DumpNL();
}

extern  void    DumpTempWId( int id ) {
/************************************/

    name        *sym;

    DumpNL();
    sym = Names[ N_TEMP ];
    while( sym != NULL ) {
        if( sym->t.v.id == id ) DumpSym( sym );
        sym = sym->n.next_name;
    }
}


extern  void    DumpSymList( name *sym ) {
/****************************************/

    DumpNL();
    while( sym != NULL ) {
        DumpSym( sym );
        sym = sym->n.next_name;
    }
}


extern  void    DumpNTemp() {
/***************************/

    DumpSymList( Names[ N_TEMP ] );
}


extern  void    DumpNMemory() {
/*****************************/

    DumpSymList( Names[ N_MEMORY ] );
}


extern  void    DumpNIndexed() {
/******************************/

    DumpSymList( Names[ N_INDEXED ] );
}


extern  void    DumpNConst() {
/****************************/

    DumpSymList( Names[ N_CONSTANT ] );
}


extern  void    DumpNRegister() {
/*******************************/

    DumpSymList( Names[ N_REGISTER ] );
}


extern  void    DumpInsList( block *blk ) {
/*****************************************/

    instruction *ins;

    ins = blk->ins.hd.next;
    for(;;) {
        DumpInOut( ins );
        if( ins->head.opcode == OP_BLOCK ) break;
        DumpInsNoNL( ins );
        DumpLiteral( " " );
        DumpCond( ins, blk );
        DumpNL();
        ins = ins->head.next;
    }
}
