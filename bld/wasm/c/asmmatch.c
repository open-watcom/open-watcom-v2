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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <malloc.h>

#include "asmglob.h"
#include "asmops1.h"//
#include "asmops2.h"
#include "asmins1.h"
#include "asmopnds.h"
#include "asmerr.h"
#include "asmsym.h"
#include "asmdefs.h"
#include "asmalloc.h"

#ifdef _WASM_
    #include "directiv.h"
    #include "womp.h"
    #include "queue.h"

extern int_8                    PhaseError;

#endif

extern const struct asm_ins ASMFAR AsmOpTable[];

extern int OperandSize( unsigned long opnd );
extern int InRange( unsigned long, unsigned );

#ifdef _WASM_
extern int AddFloatingPointEmulationFixup( const struct asm_ins ASMFAR *, bool );
    qdesc       *LinnumQueue = NULL;    // queue of linnum_data structs
#endif

static int output( int i )
/************************/
/*
- determine what code should be output and their order;
- output prefix bytes ( ADRSIZ, OPSIZ, LOCK, REPxx, segment override prefix, etc )
  , opcode, "mod r/m" byte and "s-i-b" byte;
*/
{
    const struct asm_ins ASMFAR *ins = &AsmOpTable[i];
    struct asm_code             *rCode = Code;
    unsigned_8                  tmp;

#ifdef _WASM_
    if( Options.debug_flag && !PhaseError && (Parse_Pass != PASS_1) ) {
        AddLinnumData();
    }

    if( Options.floating_point == DO_FP_EMULATION &&
        ( !rCode->use32 ) &&
        ( ins->allowed_prefix == FWAIT  || (ins->cpu&P_FPU_MASK) != P_NO87) ) {
            if( AddFloatingPointEmulationFixup( ins, FALSE ) == ERROR ) {
                return( ERROR );
            }
    }
#endif

    if( (ins->cpu&P_CPU_MASK) > (rCode->info.cpu&P_CPU_MASK)
      ||(ins->cpu&P_FPU_MASK) > (rCode->info.cpu&P_FPU_MASK) ) {
        AsmError( INVALID_INSTRUCTION_WITH_CURRENT_CPU_SETTING );
        return( ERROR );
    }

    // some instructions have to add prefix when in 386/486 mode
    switch( ins->byte1_info ) {
    case F_16:
        if( rCode->use32 ) rCode->opsiz = NOT_EMPTY;
        break;
    case F_32:
        if( !rCode->use32 ) rCode->opsiz = NOT_EMPTY;
        break;
    case F_0F:
        rCode->extended_ins = NOT_EMPTY;
        break;
    }
    if( rCode->prefix != EMPTY ) {
        if( rCode->prefix == T_LOCK ) {
            if( ins->allowed_prefix != LOCK ) {
                AsmError( LOCK_PREFIX_IS_NOT_ALLOWED_ON_THIS_INSTRUCTION );
                return( ERROR );
            }
        } else {
            if( ins->allowed_prefix != REPxx ) {
                AsmError( REP_PREFIX_IS_NOT_ALLOWED_ON_THIS_INSTRUCTION );
                return( ERROR );
            }
        }
        AsmCodeByte( AsmOpTable[AsmOpcode[rCode->prefix].position].opcode );
    }

    if( ins->token == T_FWAIT &&
        ((rCode->info.cpu&P_CPU_MASK) < P_386  && (ins->cpu&P_FPU_MASK) == P_87) ) {
        #ifdef _WASM_
        if( Options.floating_point == DO_FP_EMULATION ) {
        #endif
            AsmCodeByte( OP_NOP );
        #ifdef _WASM_
        }
        #endif
    } else if( ins->allowed_prefix == FWAIT  ||
        ((rCode->info.cpu&P_CPU_MASK) < P_386  && (ins->cpu&P_FPU_MASK) != P_NO87) ) {
        AsmCodeByte( OP_WAIT );
    }

    #ifdef _WASM_
    if( Options.floating_point == DO_FP_EMULATION &&
        ( ins->allowed_prefix == FWAIT  ||
          ((rCode->info.cpu&P_CPU_MASK) < P_386  && (ins->cpu&P_FPU_MASK) != P_NO87) ) ) {
            if( AddFloatingPointEmulationFixup( ins, TRUE ) == ERROR ) {
                return( ERROR );
            }
    }
    #endif
    if( rCode->adrsiz != EMPTY ) {
        AsmCodeByte( ADRSIZ );
    }
    if( rCode->opsiz != EMPTY ) {
        /*
            Certain instructions use the ADDRSIZE prefix when they really
            should use OPERSIZE prefix (well, I think so!). Stupid Intel.
        */
        if( ins->opnd_type[OPND2] == (OP_MMX|OP_M) ) {
            /*
                An operand size prefix for MMX instructions indicates a
                128-bit operand.  We don't support this properly. JBS 2001/02/19
            */
        } else {
            switch( ins->token ) {
            case T_JCXZ:
            case T_JECXZ:
            case T_LOOPD:
            case T_LOOPW:
            case T_LOOPED:
            case T_LOOPEW:
            case T_LOOPNED:
            case T_LOOPNEW:
            case T_LOOPZD:
            case T_LOOPZW:
            case T_LOOPNZD:
            case T_LOOPNZW:
                AsmCodeByte( ADRSIZ );
                break;
            default:
                AsmCodeByte( OPSIZ );
                break;
            }
        }
    }
    if( rCode->seg_prefix != EMPTY ) {
        AsmCodeByte( rCode->seg_prefix );
    }
    if( rCode->extended_ins != EMPTY ) {
        // special case for some 286 and 386 instructions
        AsmCodeByte( EXTENDED_OPCODE );
    }
    if( rCode->info.opnd_type[0] == OP_MMX && rCode->info.opnd_type[1] == OP_MMX ) {
        /* The reg and r/m fields are backwards */
        tmp = rCode->info.rm_byte;
        rCode->info.rm_byte = 0xc0 | ((tmp >> 3) & 0x7) | ((tmp << 3) & 0x38);
    }

    switch( ins->rm_info ) {
    case R_in_OP:
        AsmCodeByte( ins->opcode | ( rCode->info.rm_byte & NOT_BIT_67 ) );
        break;
    case no_RM:
        AsmCodeByte( ins->opcode | rCode->info.opcode );
        break;
    case no_WDS:
        rCode->info.opcode = 0;
        // no break
    default:
        AsmCodeByte( ins->opcode  | rCode->info.opcode );
        tmp = ins->rm_byte | rCode->info.rm_byte;
        AsmCodeByte( tmp );
        if( addr_32( rCode ) ) {
            switch ( tmp & NOT_BIT_345 ) {
            case 0x04:
                 // mod = 00, r/m = 100
                 // s-i-b is present
            case 0x44:
                 // mod = 01, r/m = 100
                 // s-i-b is present
            case 0x84:
                 // mod = 10, r/m = 100
                 // s-i-b is present
                 AsmCodeByte( rCode->sib );
            }
        }
        break;
    }
    return( NOT_ERROR );
}

static int output_data( unsigned long determinant, int index )
/************************************************************/
/*
  output address displacement and immediate data;
*/
{
    int                 out = 0;

    mark_fixupp( determinant, index );
    switch( Code->info.token ) {
    case T_CMPS:
    case T_LODS:
    case T_MOVS:
    case T_OUTS:
    case T_INS2:
    case T_SCAS:
    case T_STOS:
    case T_XLAT:
        /* these instructions don't really want the memory operand */
        return( NOT_ERROR );
    }

#ifdef _WASM_
//    if( store_fixup( index ) == ERROR ) return( ERROR );
    store_fixup( index );
#endif

    if( determinant & OP_I8 ) {   // 8 bit
        out = 1;
    } else if( determinant & OP_I16 ) { // 16 bit
        out = 2;
    } else if( determinant & ( OP_I32 | OP_J32 ) ) { // 32 bit
        out = 4;
    } else if( determinant & OP_J48 ) {
        out = 6;
    } else if( determinant == OP_M16_IND ||
               determinant == OP_M32_IND ||
               determinant == OP_M48_IND ) {
        if( Code->use32 ) {
            out = 4;
        } else {
            out = 2;
        }
    } else if( determinant & OP_M_ANY ) {
        // switch on the mode ( the leftmost 2 bits )
        switch( Code->info.rm_byte & BIT_67 ) {
        case MOD_01:  // mode = 01
            out = 1;
            break;
        case MOD_00:
            if( !addr_32( Code ) ) {
                if( ( Code->info.rm_byte & BIT_012 ) == D16 ) {
                     out = 2;
                }
            } else {
                switch( Code->info.rm_byte & BIT_012 ) {
                case S_I_B:
                    if( ( Code->sib & BIT_012 ) != D32 ) {
                        break;  // out = 0
                    }
                    // no break
                case D32:
                    out = 4;
                }
            }
            break;
        case MOD_10:  // mode = 10
            if( !addr_32( Code ) ) {
                out = 2;
            } else {
                out = 4;
            }
        }
    }
    while( out > 0 ) {
        AsmCodeByte( Code->data[index] );
        Code->data[index] >>= 8;
        out--;
    }
    return( NOT_ERROR );
}

int match_phase_1( void )
/*
- this routine will look up the assembler opcode table and try to match
  the first operand in table with what we get;
- if first operand match then it will call match_phase_2() to determine if the
  second operand also match; if not, it must be error;
*/
{
    int                 i;
    int                 retcode;
    signed char         temp_opsiz = 0;
    unsigned long       cur_opnd;
    unsigned long       asm_op1;

    // if nothing inside, no need to output anything
    if( Code->info.token == T_NULL ) {
        if( Code->seg_prefix != EMPTY ) {
            /* we have:     REG: on line */
            AsmError( SYNTAX_ERROR );
            return( ERROR );
        } else if( Code->info.opnd_type[OPND1] == OP_NONE ) {
            return( NOT_ERROR );
        } else {
            AsmError( SYNTAX_ERROR );
            return( ERROR );
        }
    }
    switch( Code->info.token ) {
    case T_LIDT:
    case T_LGDT:
        /* kludge fix to allow "LIDT fword ptr foo" */
        switch( Code->mem_type ) {
        case T_PWORD:
        case T_FWORD:
            Code->info.opnd_type[OPND1] = OP_M_DW;
            break;
        }
        break;
    }

    // look up the hash table to get the real position of instruction
    i = AsmOpcode[Code->info.token].position;

    // make sure the user want 80x87 ins
    if( ( AsmOpTable[i].cpu & P_FPU_MASK ) != P_NO87 ) {
        if( ( Code->info.cpu & P_FPU_MASK ) == P_NO87 ) {
            AsmError( INVALID_INSTRUCTION_WITH_CURRENT_CPU_SETTING );
            return( ERROR );
        } else {
            Code->opsiz = EMPTY;
        }
    }

    // make sure the user want 80x86 protected mode ins
    if( ( AsmOpTable[i].cpu & P_PM ) != 0 ) {
        if( ( Code->info.cpu & P_PM ) == 0 ) {
            AsmError( INVALID_INSTRUCTION_WITH_CURRENT_CPU_SETTING );
            return( ERROR );
        }
    }

    // fixme
    // moved down 4 lines cur_opnd = Code->info.opnd_type[OPND1];
    while( AsmOpTable[i].token == Code->info.token ) {
        // get the real opnd
        asm_op1 = AsmOpTable[i].opnd_type[OPND1];
        cur_opnd = Code->info.opnd_type[OPND1];

        /* fixme -- temporary fix for all OP_M types */
        // if( cur_opnd & OP_M ) { I think there is a problem here
        if( cur_opnd & OP_M_ANY ) {
            if( ! ( cur_opnd & asm_op1 ) ) {
                if( MEM_TYPE( Code->mem_type, BYTE ) ) {
                    cur_opnd = OP_M_B;
                } else if( MEM_TYPE( Code->mem_type, WORD ) ) {
                    cur_opnd = OP_M_W;
                } else if( MEM_TYPE( Code->mem_type, DWORD ) ) {
                    cur_opnd = OP_M_DW;
                } else if( Code->mem_type == T_QWORD ) {
                    cur_opnd = OP_M_QW;
                } else if( Code->mem_type == T_TBYTE ) {
                    cur_opnd = OP_M_TB;
                }
            }
        }

        /* fixme -- temporary fix for all OP_I types */
        if( cur_opnd & OP_I ) {
            if( cur_opnd == OP_I8 ) {
                cur_opnd = OP_GE_8;
            } else if( cur_opnd == OP_I16 ) {
                cur_opnd = OP_GE_16;
            }
        }

        switch( asm_op1 ) {
        case OP_MMX:
             if( cur_opnd == OP_MMX ) {
                 return( match_phase_2( &i ) );
             }
             break;
        case OP_R16:
            if( cur_opnd & asm_op1 ) {
                temp_opsiz = Code->opsiz;
                Code->opsiz = EMPTY;
                switch( match_phase_2( &i ) ) {
                case EMPTY:
                    Code->opsiz = temp_opsiz;
                    break;
                case ERROR:
                    return( ERROR );
                case NOT_ERROR:
                    return( NOT_ERROR );
                }
            }
            break;
        case OP_M16:
            if( cur_opnd & asm_op1 ) {
                temp_opsiz = Code->opsiz;
                switch( match_phase_2( &i ) ) {
                case EMPTY:
                    Code->opsiz = temp_opsiz;
                    break;
                case ERROR:
                    return( ERROR );
                case NOT_ERROR:
                    return( NOT_ERROR );
                }
            }
            break;
        case OP_I32:
        case OP_I16:
            if( cur_opnd & asm_op1 ) {
                Code->info.opnd_type[OPND1] = asm_op1;
                switch( match_phase_2( &i ) ) {
                case EMPTY:
                    break;
                case ERROR:
                    return( ERROR );
                case NOT_ERROR:
                    return( NOT_ERROR );
                }
            }
            break;
        case OP_I8_U:
            if( cur_opnd & OP_I ) {
                if( Code->data[OPND1] <= UCHAR_MAX ) {
                    temp_opsiz = Code->opsiz;
                    Code->info.opnd_type[OPND1] = OP_I8;
                    switch( match_phase_2( &i ) ) {
                    case EMPTY:
                        Code->opsiz = temp_opsiz;
                        break;
                    case ERROR:
                        return( ERROR );
                    case NOT_ERROR:
                        return( NOT_ERROR );
                    }
                }
            }
            break;
        case OP_I_3:                    // for INT only
            if( ( ( cur_opnd & OP_I8 ) && Code->data[0] == 3 ) &&
                                Code->info.opnd_type[OPND2] == OP_NONE ) {
                return( output( i ) );
            }
            break;
        case OP_NONE:
            if( cur_opnd == OP_NONE &&
                    Code->info.opnd_type[OPND2] == OP_NONE ) {
                return( output( i ) );
            }
            break;
        default:
            /* this is the check to see if the operand we have matches
             * the type of the one we need
             */
            if( asm_op1 & cur_opnd ) {
                retcode = match_phase_2( &i );
                if( retcode != EMPTY ) {
                    return( retcode );
                }
            }
            break;
        }
        i++;
    }
    AsmError( INVALID_INSTRUCTION_OPERANDS );
    return( ERROR );
}

int match_phase_2( int *i )
/*
- a routine used by match_phase_1() to determine whether both operands match
  with that in the assembly instructions table;
- call by match_phase_1() only;
*/
{
    if( Code->info.opnd_type[OPND2] != OP_NONE ) {
        // 2 opnds instruction
        return( match_phase_3( i, AsmOpTable[*i].opnd_type[OPND1] ) );
    } else {
        // 1 opnd instruction
        // make sure the second opnd also match, i.e. has to be OP_NONE
        if( AsmOpTable[*i].opnd_type[OPND2] == OP_NONE ) {
            if( output( *i ) == ERROR ) {
                return( ERROR );
            }
            // output idata or disp ( if first opnd is OP_M / OP_I )
            return( output_data( Code->info.opnd_type[OPND1], OPND1 ) );
        } else {
            // still cannot find match
            return( EMPTY );
        }
    }
}

int match_phase_3( int *i, unsigned long determinant )
/*
- this routine will look up the assembler opcode table and try to match
  the second operand with what we get;
- if second operand match then it will output code; if not, pass back to
  match_phase_1() and try again;
- call by match_phase_2() only;
*/
{
    unsigned long       cur_opnd;
    unsigned long       last_opnd;
    unsigned long       asm_op2;
    unsigned            instruction;

    instruction = AsmOpTable[*i].token;

    last_opnd = Code->info.opnd_type[OPND1];
    cur_opnd  = Code->info.opnd_type[OPND2];

    while( AsmOpTable[*i].opnd_type[OPND1] == determinant &&
           AsmOpTable[*i].token == instruction ) {
        asm_op2 = AsmOpTable[*i].opnd_type[OPND2];
        switch( asm_op2 ) {
        case OP_CR:
        case OP_DR:
        case OP_TR:
        case OP_ST:
        case OP_STI:
            if( cur_opnd & asm_op2 ) {
                if( output( *i ) == ERROR ) return( ERROR );
                return( NOT_ERROR );
            }
            break;
        case OP_SR:
        case OP_DX:
        case OP_A:
        case OP_R:
        case OP_R1632:
        case OP_R32:
            if( cur_opnd & asm_op2 ) {
                if( output( *i ) == ERROR ) return( ERROR );
                return( output_data( last_opnd, OPND1 ) );
            }
            break;
        case OP_CL:
            if( cur_opnd & asm_op2 ) {
                // CL is encoded in bit 345 of rm_byte, but we don't need it
                // so clear it here
                Code->info.rm_byte &= NOT_BIT_345;
                if( output( *i ) == ERROR ) return( ERROR );
                return( output_data( last_opnd, OPND1 ) );
            }
            break;
        case OP_R16:
            if( cur_opnd & asm_op2 ) {
                Code->opsiz = EMPTY;
                if( output( *i ) == ERROR ) return( ERROR );
                return( output_data( last_opnd, OPND1 ) );
            }
            break;
        case OP_I:
            if( cur_opnd & asm_op2 ) {
                #if defined(_WASM_)
                    long operand = Code->data[OPND2];
                #endif
                if( last_opnd & OP_R8 ) {
                    // 8-bit register, so output 8-bit data
                    #if defined(_WASM_)
                        if( Parse_Pass == PASS_1 && !InRange( operand, 1 ) ) {
                            AsmWarn( 1, IMMEDIATE_CONSTANT_TOO_LARGE );
                        }
                    #endif
                    Code->opsiz = EMPTY;
                    cur_opnd = OP_I8;
                } else if( last_opnd & OP_R16 ) {
                    // 16-bit register, so output 16-bit data
                    #if defined(_WASM_)
                        if( Parse_Pass == PASS_1 && !InRange( operand, 2 ) ) {
                            AsmWarn( 1, IMMEDIATE_CONSTANT_TOO_LARGE );
                        }
                    #endif
                    cur_opnd = OP_I16;
                } else if( last_opnd & OP_R32 ) {
                    // 32-bit register, so output 32-bit data
                    Code->opsiz = Code->use32 ? EMPTY : NOT_EMPTY;/* 12-feb-92 */
                    cur_opnd = OP_I32;
                } else if( last_opnd & OP_M_ANY ) {
                    /* there is no reason this should be only for T_MOV */
                    switch( OperandSize( last_opnd ) ) {
                    case 1:
                        cur_opnd = OP_I8;
                        Code->opsiz = EMPTY;
                        break;
                    case 2:
                        cur_opnd = OP_I16;
                        Code->opsiz = Code->use32 ? NOT_EMPTY : EMPTY;
                        break;
                    case 4:
                        cur_opnd = OP_I32;
                        Code->opsiz = Code->use32 ? EMPTY : NOT_EMPTY;
                        break;
                    }
                }
                if( output( *i ) == ERROR ) {
                    return( ERROR );
                }
                if( output_data( last_opnd, OPND1 ) == ERROR ) return( ERROR );
                return( output_data( cur_opnd, OPND2 ) );
            }
            break;
        case OP_I8_U:
            if( cur_opnd != OP_I8 && cur_opnd != OP_I16 ) {
                break;
            }
            // range of unsigned 8-bit is 0 - 255
            if( Code->data[OPND2] > UCHAR_MAX ) {
                break;
            }
            if( output( *i ) == ERROR ) {
                return( ERROR );
            }
            if( output_data( last_opnd, OPND1 ) == ERROR ) return( ERROR );
            return( output_data( OP_I8, OPND2 ) );
            break;
        case OP_I8:
            if( cur_opnd == asm_op2 ) {
                /* do nothing yet */
            } else if( ( last_opnd & OP_M16_R16 ) && ( cur_opnd & OP_I ) &&
                ( MEM_TYPE( Code->mem_type, WORD ) ) &&
                InsFixups[OPND2] == NULL ) {
                if( (int_8)Code->data[OPND2] ==
                    (int_16)Code->data[OPND2] ) {
                    Code->info.opnd_type[OPND2] = OP_I8;
                    Code->data[OPND2] = (int_8)Code->data[OPND2];
                } else {
                    break;
                }
            } else if( ( last_opnd & OP_M32_R32 ) && ( cur_opnd & OP_I ) &&
                ( MEM_TYPE( Code->mem_type, DWORD ) ) &&
                InsFixups[OPND2] == NULL ) {
                if( (int_8)Code->data[OPND2] ==
                    (int_32)Code->data[OPND2] ) {
                    Code->info.opnd_type[OPND2] = OP_I8;
                    Code->data[OPND2] = (int_8)Code->data[OPND2];
                } else {
                    break;
                }
            } else {
                break;
            }
            if( output( *i ) == ERROR ) return( ERROR );
            if( output_data( last_opnd, OPND1 ) == ERROR ) return( ERROR );
            return( output_data( OP_I8, OPND2 ) );
        case OP_I_1:
            if( cur_opnd == OP_I8  &&  Code->data[OPND2] == 1 ) {
                if( output( *i ) == ERROR ) return( ERROR );
                return( output_data( last_opnd, OPND1 ) );
            }
            break;
        case OP_M16:
            if( cur_opnd & OP_M &&
                   ( MEM_TYPE( Code->mem_type, WORD ) ||
                     Code->mem_type == EMPTY ) ) {
                if( output( *i ) == ERROR ) return( ERROR );
                if( output_data( last_opnd, OPND1 ) == ERROR ) return( ERROR );
                return( output_data( cur_opnd, OPND2 ) );
            }
            break;
        case OP_MMX|OP_M:
        case OP_MMX:
            if( cur_opnd == OP_MMX ) {
                if( output( *i ) == ERROR ) return( ERROR );
                return( output_data( last_opnd, OPND1 ) );
//              return( NOT_ERROR );
            }
            if( !(asm_op2 & OP_M) ) break;
            /* fall through */
        case OP_M:
            if( cur_opnd & asm_op2 ) {
                if( output( *i ) == ERROR ) return( ERROR );
                if( output_data( last_opnd, OPND1 ) == ERROR ) return( ERROR );
                return( output_data( cur_opnd, OPND2 ) );
            }
            break;
        }
        (*i)++;
    }
    (*i)--;
    return( EMPTY );
}

#ifdef _WASM_

static void AddLinnumData( void )
/*******************************/
/* store a reference for the current line at the current address */
{
    struct linnum_data  *curr;
    struct queuenode    *node;

    if( LineNumber < 0x8000 )  {
        curr = AsmAlloc( sizeof( struct linnum_data ) );
        curr->number = LineNumber;
        curr->offset = Address;
        node = AsmAlloc( sizeof( queuenode ) );
        node->data = curr;

        if( LinnumQueue == NULL ) {
            LinnumQueue = AsmAlloc( sizeof( qdesc ) );
            QInit( LinnumQueue );
        }
        QEnqueue( LinnumQueue, node );
    }
}
#endif
