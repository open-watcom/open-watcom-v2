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


#include "asmglob.h"
#include "asmalloc.h"
#include "asmfixup.h"
#include "fppatch.h"
#if defined( _STANDALONE_ )
  #include "directiv.h"
#endif


#define USE_FPU_EMUL    ((floating_point==DO_FP_EMULATION)&&(!rCode->use32)&&(ins->cpu & P_FPU_MASK)!=P_NO87)

#if defined( _STANDALONE_ )
extern void     AddLinnumDataRef( void );
#endif
extern bool     AddFPPatchAndFixups( fp_patches patch );

enum fpe        floating_point = DO_FP_EMULATION;

static bool match_phase_3( bool *found, asmins_idx *i, OPNDTYPE determinant );

static bool output_3DNow( asmins_idx ins_pos )
/********************************************/
{
    const struct asm_ins ASMFAR *ins = &AsmOpTable[ins_pos];

    if( ins->byte1_info == F_0F0F ) {
        AsmCodeByte( ins->opcode | Code->info.opcode );
    }
    return( RC_OK );
}

static bool output( asmins_idx ins_pos )
/**************************************/
/*
- determine what code should be output and their order;
- output prefix bytes ( ADRSIZ, OPSIZ, LOCK, REPxx, segment override prefix, etc )
  , opcode, "mod r/m" byte and "s-i-b" byte;
*/
{
    const struct asm_ins ASMFAR *ins = &AsmOpTable[ins_pos];
    struct asm_code             *rCode = Code;
    unsigned_8                  tmp;
    fp_patches                  patch = FPP_NONE;

#if defined( _STANDALONE_ )
    /*
     * Output debug info - line numbers
     */
    if( Options.debug_flag && !PhaseError && ( Parse_Pass != PASS_1 ) ) {
        AddLinnumDataRef();
    }

    /*
     * Check if FP is valid
     */
    if(( (ins->cpu & P_FPU_MASK) != P_NO87 ) && ( floating_point == NO_FP_ALLOWED )) {
        AsmError( NO_FP_WITH_FPC_SET );
        return( RC_ERROR );
    }
#endif

    /*
     * Check if CPU and FPU is valid for output code
     */
    if( (ins->cpu & P_CPU_MASK) > (rCode->info.cpu & P_CPU_MASK)
        || (ins->cpu & P_FPU_MASK) > (rCode->info.cpu & P_FPU_MASK)
        || (ins->cpu & P_EXT_MASK) > (ins->cpu & rCode->info.cpu & P_EXT_MASK) ) {
        AsmError( INVALID_INSTRUCTION_WITH_CURRENT_CPU_SETTING );
        return( RC_ERROR );
    }

    /*
     * Check if FPU instruction requires emulator patch and fixup
     */
    if( USE_FPU_EMUL ) {
        if( ins->token == T_FWAIT ) {
            if( (rCode->info.cpu & P_FPU_MASK) < P_387 ) {
                patch = FPP_WAIT;
            }
        } else if( ins->allowed_prefix != NO_FWAIT ) {
            switch( Code->prefix.seg ) {
            case PREFIX_EMPTY: patch = FPP_NORMAL; break;
            case PREFIX_ES: patch = FPP_ES;     break;
            case PREFIX_CS: patch = FPP_CS;     break;
            case PREFIX_SS: patch = FPP_SS;     break;
            case PREFIX_DS: patch = FPP_DS;     break;
            case PREFIX_FS: patch = FPP_FS;     break;
            case PREFIX_GS: patch = FPP_GS;     break;
            }
        }
    }

    /*
     * Add extra FWAIT instruction if necessary
     */
    if( patch == FPP_NONE && ins->token != T_FWAIT && ins->allowed_prefix != NO_FWAIT ) {
        if( ins->allowed_prefix == FWAIT ) {
            AsmCodeByte( OP_WAIT );
        } else if( (rCode->info.cpu & P_CPU_MASK) < P_286 && (ins->cpu & P_FPU_MASK) == P_87 ) {
            // implicit FWAIT synchronization for 8087 (CPU 8086/80186)
            AsmCodeByte( OP_WAIT );
        }
    }

    /*
     * Output instruction prefix LOCK, REP or REPNE
     */
    if( rCode->prefix.ins != T_NULL ) {
        switch( rCode->prefix.ins ) {
        case T_LOCK:
            if( ins->allowed_prefix != LOCK ) {
                AsmError( LOCK_PREFIX_IS_NOT_ALLOWED_ON_THIS_INSTRUCTION );
                return( RC_ERROR );
            }
            break;
        case T_REP:
            if( ins->allowed_prefix != REP ) {
                AsmError( REP_PREFIX_IS_NOT_ALLOWED_ON_THIS_INSTRUCTION );
                return( RC_ERROR );
            }
            break;
        case T_REPZ:
        case T_REPE:
        case T_REPNZ:
        case T_REPNE:
            if( ins->allowed_prefix != REPxx ) {
                AsmError( REPX_PREFIX_IS_NOT_ALLOWED_ON_THIS_INSTRUCTION );
                return( RC_ERROR );
            }
            break;
        default:
            break;
        }
        AsmCodeByte( AsmOpTable[AsmOpcode[rCode->prefix.ins].position].opcode );
    }
    /*
     * Output instruction prefix REP or REPNE for SSEx instructions
     */
    switch( ins->byte1_info ) {
    case F_F20F:
        AsmCodeByte( 0xF2 );
        break;
    case F_F3:
    case F_F30F:
        AsmCodeByte( 0xF3 );
        break;
    case F_16:
    case F_32:
    case F_660F:
    case F_0F:
    case F_0F0F:
    default:
        break;
    }

    /*
     * Output address size prefix
     */
    if( rCode->prefix.adrsiz == TRUE ) {
        AsmCodeByte( ADRSIZ );
    }
    /*
     * Output operand size prefix
     */
    if( ins->cpu & NO_OPPRFX ) {
        SET_OPSIZ_OFF( rCode );
    }
    switch( ins->byte1_info ) {
    case F_16:
        if( rCode->use32 ) {
            SET_OPSIZ_ON( rCode );
        }
        break;
    case F_32:
        if( !rCode->use32 ) {
            SET_OPSIZ_ON( rCode );
        }
        break;
    case F_660F:
        SET_OPSIZ_ON( rCode );
        break;
    case F_0F:
    case F_0F0F:
    case F_F20F:
    case F_F30F:
    default:
        break;
    }
    if( rCode->prefix.opsiz == TRUE ) {
        if( (rCode->info.cpu & P_CPU_MASK) < P_386 ) {
            AsmError( CANNOT_USE_386_OPSIZE_MODE_WITH_CURRENT_CPU_SETTING );
            return( RC_ERROR );
        }
        /*
            Certain instructions use the ADDRSIZE prefix when they really
            should use OPERSIZE prefix (well, I think so!). Stupid Intel.
        */
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

    /*
     * Output FPU instruction emulation patch code and fixups
     */
    if( AddFPPatchAndFixups( patch ) ) {
        return( RC_ERROR );
    }

    /*
     * Output segment prefix
     */
    if( rCode->prefix.seg != PREFIX_EMPTY ) {
        AsmCodeByte( rCode->prefix.seg );
    }
    /*
     * Output extended opcode
     * special case for some 286 and 386 instructions
     * or 3DNow!, MMX and SSEx instructions
     */
    switch( ins->byte1_info ) {
    case F_0F0F:
        AsmCodeByte( EXTENDED_OPCODE );
        // no break
    case F_0F:
    case F_660F:
    case F_F20F:
    case F_F30F:
        AsmCodeByte( EXTENDED_OPCODE );
        break;
    case F_F3:
    default:
        break;
    }
    if( ins->opnd_dir ) {
        /* The reg and r/m fields are backwards */
        tmp = rCode->info.rm_byte;
        rCode->info.rm_byte = (tmp & 0xc0) | ((tmp >> 3) & 0x7) | ((tmp << 3) & 0x38);
    }
    switch( ins->rm_info ) {
    case R_in_OP:
        AsmCodeByte( ins->opcode | (rCode->info.rm_byte & NOT_BIT_67) );
        break;
    case no_RM:
        AsmCodeByte( ins->opcode | rCode->info.opcode );
        break;
    case no_WDS:
        rCode->info.opcode = 0;
        // no break
    default:
        // don't output opcode for 3DNow! instructions
        if( ins->byte1_info != F_0F0F ) {
            AsmCodeByte( ins->opcode | rCode->info.opcode );
        }
        tmp = ins->rm_byte | rCode->info.rm_byte;
        AsmCodeByte( tmp );
        if( IS_ADRSIZ_32( rCode ) ) {
            switch( tmp & NOT_BIT_345 ) {
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
    return( RC_OK );
}

static bool output_data( OPNDTYPE determinant, operand_idx index )
/***************************************************************/
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
    case T_INS:
    case T_SCAS:
    case T_STOS:
    case T_XLAT:
        /* these instructions don't really want the memory operand */
        return( RC_OK );
    }

#if defined( _STANDALONE_ )
    store_fixup( index );
#endif

    if( determinant & OP_I8 ) {   // 8 bit
        out = 1;
    } else if( determinant & OP_I16 ) { // 16 bit
        out = 2;
    } else if( determinant & (OP_I32 | OP_J32) ) { // 32 bit
        out = 4;
    } else if( determinant & OP_J48 ) {
        out = 6;
    } else if( determinant & OP_M_ANY ) {
        // switch on the mode ( the leftmost 2 bits )
        switch( Code->info.rm_byte & BIT_67 ) {
        case MOD_01:  // mode = 01
            out = 1;
            break;
        case MOD_00:
            if( IS_ADRSIZ_32( Code ) ) {
                switch( Code->info.rm_byte & BIT_012 ) {
                case S_I_B:
                    if( (Code->sib & BIT_012) != D32 ) {
                        break;  // out = 0
                    }
                    // no break
                case D32:
                    out = 4;
                }
            } else {
                if( (Code->info.rm_byte & BIT_012) == D16 ) {
                     out = 2;
                }
            }
            break;
        case MOD_10:  // mode = 10
            if( IS_ADRSIZ_32( Code ) ) {
                out = 4;
            } else {
                out = 2;
            }
        }
    }
    while( out > 0 ) {
        AsmCodeByte( (uint_8)Code->data[index] );
        Code->data[index] >>= 8;
        out--;
    }
    return( RC_OK );
}

static bool match_phase_2( bool *found, asmins_idx *ins_pos )
/***************************************************************
- a routine used by match_phase_1() to determine whether both operands match
  with that in the assembly instructions table;
- call by match_phase_1() only;
*/
{
    if( Code->info.opnd_type[OPND2] != OP_NONE ) {
        // 2 opnds instruction
        return( match_phase_3( found, ins_pos, AsmOpTable[*ins_pos].opnd_type[OPND1] ) );
    } else {
        // 1 opnd instruction
        // make sure the second opnd also match, i.e. has to be OP_NONE
        if( AsmOpTable[*ins_pos].opnd_type[OPND2] == OP_NONE ) {
            if( output( *ins_pos ) ) {
                return( RC_ERROR );
            }
            // output idata or disp ( if first opnd is OP_M / OP_I )
            return( output_data( Code->info.opnd_type[OPND1], OPND1 ) );
        } else {
            // still cannot find match
            *found = FALSE;
            return( RC_ERROR );
        }
    }
}

bool match_phase_1( void )
/*************************
- this routine will look up the assembler opcode table and try to match
  the first operand in table with what we get;
- if first operand match then it will call match_phase_2() to determine if the
  second operand also match; if not, it must be error;
*/
{
    asmins_idx      ins_pos;
    bool            rc;
    bool            temp_opsiz = FALSE;
    OPNDTYPE        cur_opnd;
    OPNDTYPE        asm_op1;
    bool            found = TRUE;

    // if nothing inside, no need to output anything
    if( Code->info.token == T_NULL ) {
        if( Code->prefix.seg != PREFIX_EMPTY ) {
            /* we have:     REG: on line */
            AsmError( SYNTAX_ERROR );
            return( RC_ERROR );
        } else if( Code->info.opnd_type[OPND1] == OP_NONE ) {
            return( RC_OK );
        } else {
            AsmError( SYNTAX_ERROR );
            return( RC_ERROR );
        }
    }
    // look up the hash table to get the real position of instruction
    ins_pos = AsmOpcode[Code->info.token].position;

    // make sure the user want 80x87 ins
    if( (AsmOpTable[ins_pos].cpu & P_FPU_MASK) != P_NO87 ) {
        if( (Code->info.cpu & P_FPU_MASK) == P_NO87 ) {
            AsmError( INVALID_INSTRUCTION_WITH_CURRENT_CPU_SETTING );
            return( RC_ERROR );
        } else {
            SET_OPSIZ_OFF( Code );
        }
    }

    // make sure the user want 80x86 protected mode ins
    if( AsmOpTable[ins_pos].cpu & P_PM ) {
        if( (Code->info.cpu & P_PM) == 0 ) {
            AsmError( INVALID_INSTRUCTION_WITH_CURRENT_CPU_SETTING );
            return( RC_ERROR );
        }
    }

    // fixme
    // moved down 4 lines cur_opnd = Code->info.opnd_type[OPND1];
    while( AsmOpTable[ins_pos].token == Code->info.token ) {
        // get the real opnd
        asm_op1 = AsmOpTable[ins_pos].opnd_type[OPND1];
        cur_opnd = Code->info.opnd_type[OPND1];

        /* fixme -- temporary fix for all OP_M types */
        // if( cur_opnd & OP_M ) { I think there is a problem here
        if( cur_opnd & OP_M_ANY ) {
            if( !( (asm_op1 & cur_opnd) && ( (asm_op1 & OP_M_DFT) == (cur_opnd & OP_M_DFT) ) ) ) {
                if( MEM_TYPE( Code->mem_type, BYTE ) ) {
                    cur_opnd = OP_M_B;
                } else if( MEM_TYPE( Code->mem_type, WORD ) ) {
                    cur_opnd = OP_M_W;
                } else if( MEM_TYPE( Code->mem_type, DWORD ) ) {
                    cur_opnd = OP_M_DW;
                } else if( Code->mem_type == MT_FWORD ) {
                    cur_opnd = OP_M_FW;
                } else if( Code->mem_type == MT_QWORD ) {
                    cur_opnd = OP_M_QW;
                } else if( Code->mem_type == MT_TBYTE ) {
                    cur_opnd = OP_M_TB;
                } else if( Code->mem_type == MT_OWORD ) {
                    cur_opnd = OP_M_OW;
                } else if( Code->mem_type == MT_FAR ) {
                    cur_opnd = ( Code->use32 ) ? OP_M_FW : OP_M_DW ;
                } else if( Code->mem_type == MT_NEAR ) {
                    cur_opnd = ( Code->use32 ) ? OP_M_DW : OP_M_W ;
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
            if( cur_opnd & OP_MMX ) {
                return( match_phase_2( &found, &ins_pos ) );
            }
            break;
        case OP_XMM:
            if( cur_opnd & OP_XMM ) {
                return( match_phase_2( &found, &ins_pos ) );
            }
            break;
        case OP_R16:
            if( cur_opnd & asm_op1 ) {
                temp_opsiz = Code->prefix.opsiz;
                SET_OPSIZ_OFF( Code );
                rc = match_phase_2( &found, &ins_pos );
                if( found )
                    return( rc );
                found = TRUE;
                Code->prefix.opsiz = temp_opsiz;
            }
            break;
        case OP_M16:
            if( cur_opnd & asm_op1 ) {
                temp_opsiz = Code->prefix.opsiz;
                rc = match_phase_2( &found, &ins_pos );
                if( found )
                    return( rc );
                found = TRUE;
                Code->prefix.opsiz = temp_opsiz;
            }
            break;
        case OP_I32:
        case OP_I16:
            if( cur_opnd & asm_op1 ) {
                Code->info.opnd_type[OPND1] = asm_op1;
                rc = match_phase_2( &found, &ins_pos );
                if( found )
                    return( rc );
                found = TRUE;
            }
            break;
        case OP_I8_U:
            if( cur_opnd & OP_I ) {
                if( Code->data[OPND1] <= UCHAR_MAX ) {
                    temp_opsiz = Code->prefix.opsiz;
                    Code->info.opnd_type[OPND1] = OP_I8;
                    rc = match_phase_2( &found, &ins_pos );
                    if( found )
                        return( rc );
                    found = TRUE;
                    Code->prefix.opsiz = temp_opsiz;
                }
            }
            break;
        case OP_I_3:                    // for INT only
            if( ( (cur_opnd & OP_I8) && Code->data[OPND1] == 3 ) &&
                                Code->info.opnd_type[OPND2] == OP_NONE ) {
                return( output( ins_pos ) );
            }
            break;
        case OP_NONE:
            if( cur_opnd == OP_NONE &&
                    Code->info.opnd_type[OPND2] == OP_NONE ) {
                return( output( ins_pos ) );
            }
            break;
        default:
            /* this is the check to see if the operand we have matches
             * the type of the one we need
             */
            if( (asm_op1 & cur_opnd)
                && ( ( (cur_opnd & OP_M_ANY) == 0 )
                    || ( (asm_op1 & OP_M_DFT) == (cur_opnd & OP_M_DFT) ) ) ) {
                rc = match_phase_2( &found, &ins_pos );
                if( found )
                    return( rc );
                found = TRUE;
            }
            break;
        }
        ins_pos++;
    }
    AsmError( INVALID_INSTRUCTION_OPERANDS );
    return( RC_ERROR );
}

static bool check_3rd_operand( asmins_idx ins_pos )
/*************************************************/
{
    OPNDTYPE    cur_opnd;

    cur_opnd = Code->info.opnd_type[OPND3];
    if( ( AsmOpTable[ins_pos].opnd_type_3rd == OP3_NONE )
        || (AsmOpTable[ins_pos].opnd_type_3rd & OP3_HID) ) {
        if( cur_opnd == OP_NONE ) {
            return( RC_OK );
        } else {
            return( RC_ERROR );
        }
    } else if( cur_opnd == OP_NONE ) {
        return( RC_ERROR );
    } else {
        return( RC_OK );
    }
}

static bool output_3rd_operand( asmins_idx ins_pos )
/**************************************************/
{
    if( AsmOpTable[ins_pos].opnd_type_3rd == OP3_NONE ) {
        return( RC_OK );
    } else if( AsmOpTable[ins_pos].opnd_type_3rd == OP3_I8_U ) {
        if( Code->info.opnd_type[OPND3] & OP_I ) {
            return( output_data( OP_I8, OPND3 ) );
        } else {
            return( RC_ERROR );
        }
    } else if( AsmOpTable[ins_pos].opnd_type_3rd & OP3_HID ) {
        Code->data[OPND3] = AsmOpTable[ins_pos].opnd_type_3rd & ~OP3_HID;
        return( output_data( OP_I8, OPND3 ) );
    } else {
        return( RC_OK );
    }
}

static bool match_phase_3( bool *found, asmins_idx *ins_pos, OPNDTYPE determinant )
/************************************************************************
- this routine will look up the assembler opcode table and try to match
  the second operand with what we get;
- if second operand match then it will output code; if not, pass back to
  match_phase_1() and try again;
- call by match_phase_2() only;
*/
{
    OPNDTYPE    cur_opnd;
    OPNDTYPE    last_opnd;
    OPNDTYPE    asm_op2;
    asm_token   instruction;

    instruction = AsmOpTable[*ins_pos].token;

    last_opnd = Code->info.opnd_type[OPND1];
    cur_opnd  = Code->info.opnd_type[OPND2];

    while( AsmOpTable[*ins_pos].opnd_type[OPND1] == determinant &&
           AsmOpTable[*ins_pos].token == instruction ) {
        asm_op2 = AsmOpTable[*ins_pos].opnd_type[OPND2];
        switch( asm_op2 ) {
        case OP_CR:
        case OP_DR:
        case OP_TR:
        case OP_ST:
        case OP_STI:
            if( cur_opnd & asm_op2 ) {
                return( output( *ins_pos ) );
            }
            break;
        case OP_SR:
        case OP_DX:
        case OP_A:
        case OP_R:
        case OP_R1632:
        case OP_R32:
            if( cur_opnd & asm_op2 ) {
                if( check_3rd_operand( *ins_pos ) )
                    break;
                if( output( *ins_pos ) )
                    return( RC_ERROR );
                if( output_data( last_opnd, OPND1 ) )
                    return( RC_ERROR );
                return( output_3rd_operand( *ins_pos ) );
            }
            break;
        case OP_CL:
            if( cur_opnd & asm_op2 ) {
                // CL is encoded in bit 345 of rm_byte, but we don't need it
                // so clear it here
                Code->info.rm_byte &= NOT_BIT_345;
                if( output( *ins_pos ) )
                    return( RC_ERROR );
                return( output_data( last_opnd, OPND1 ) );
            }
            break;
        case OP_R16:
            if( cur_opnd & asm_op2 ) {
                SET_OPSIZ_OFF( Code );
                if( output( *ins_pos ) )
                    return( RC_ERROR );
                return( output_data( last_opnd, OPND1 ) );
            }
            break;
        case OP_I:
            if( cur_opnd & asm_op2 ) {
#if defined(_STANDALONE_)
                long operand = Code->data[OPND2];
#endif
                if( last_opnd & OP_R8 ) {
                    // 8-bit register, so output 8-bit data
#if defined(_STANDALONE_)
                    if( Parse_Pass == PASS_1 && !InRange( operand, 1 ) ) {
                        AsmWarn( 1, IMMEDIATE_CONSTANT_TOO_LARGE );
                    }
#endif
                    SET_OPSIZ_OFF( Code );
                    cur_opnd = OP_I8;
                    if( InsFixups[OPND2] != NULL ) {
                        InsFixups[OPND2]->fixup_type = FIX_LOBYTE;
                    }
                } else if( last_opnd & OP_R16 ) {
                    // 16-bit register, so output 16-bit data
#if defined(_STANDALONE_)
                    if( Parse_Pass == PASS_1 && !InRange( operand, 2 ) ) {
                        AsmWarn( 1, IMMEDIATE_CONSTANT_TOO_LARGE );
                    }
#endif
                    cur_opnd = OP_I16;
                } else if( last_opnd & OP_R32 ) {
                    // 32-bit register, so output 32-bit data
                    SET_OPSIZ_32( Code );
                    cur_opnd = OP_I32;
                } else if( last_opnd & OP_M_ANY ) {
                    /* there is no reason this should be only for T_MOV */
                    switch( OperandSize( last_opnd ) ) {
                    case 1:
                        cur_opnd = OP_I8;
                        SET_OPSIZ_OFF( Code );
                        break;
                    case 2:
                        cur_opnd = OP_I16;
                        SET_OPSIZ_16( Code );
                        break;
                    case 4:
                        cur_opnd = OP_I32;
                        SET_OPSIZ_32( Code );
                        break;
                    default:
                        AsmError( INVALID_INSTRUCTION_OPERANDS );
                        break;
                    }
                }
                if( output( *ins_pos ) )
                    return( RC_ERROR );
                if( output_data( last_opnd, OPND1 ) )
                    return( RC_ERROR );
                return( output_data( cur_opnd, OPND2 ) );
            }
            break;
        case OP_I8_U:
            if( ( cur_opnd != OP_I8 )
                && ( cur_opnd != OP_I8_U )
                && ( cur_opnd != OP_I16 ) ) {
                break;
            }
            // range of unsigned 8-bit is 0 - 255
            if( Code->data[OPND2] > UCHAR_MAX ) {
                break;
            }
            if( output( *ins_pos ) )
                return( RC_ERROR );
            if( output_data( last_opnd, OPND1 ) )
                return( RC_ERROR );
            return( output_data( OP_I8, OPND2 ) );
        case OP_I8:
            if( cur_opnd == OP_I8 ) {
                /* do nothing yet */
                if( InsFixups[OPND2] != NULL ) {
                    break;
                }
            } else if( (cur_opnd & OP_I)
                && ( InsFixups[OPND2] == NULL )
                && ( (last_opnd & OP_R16)
                || (last_opnd & OP_M16) && ( MEM_TYPE( Code->mem_type, WORD ) ) ) ) {
                if( (int_8)Code->data[OPND2] == (int_16)Code->data[OPND2] ) {
                    Code->info.opnd_type[OPND2] = OP_I8;
                    Code->data[OPND2] = (int_8)Code->data[OPND2];
                } else {
                    break;
                }
            } else if( (cur_opnd & OP_I)
                && ( InsFixups[OPND2] == NULL )
                && ( (last_opnd & OP_R32)
                || (last_opnd & OP_M32) && ( MEM_TYPE( Code->mem_type, DWORD ) ) ) ) {
                if( (int_8)Code->data[OPND2] == (int_32)Code->data[OPND2] ) {
                    Code->info.opnd_type[OPND2] = OP_I8;
                    Code->data[OPND2] = (int_8)Code->data[OPND2];
                } else {
                    break;
                }
            } else {
                break;
            }
            if( output( *ins_pos ) )
                return( RC_ERROR );
            if( output_data( last_opnd, OPND1 ) )
                return( RC_ERROR );
            return( output_data( OP_I8, OPND2 ) );
        case OP_I_1:
            if( cur_opnd == OP_I8 && Code->data[OPND2] == 1 ) {
                if( output( *ins_pos ) )
                    return( RC_ERROR );
                return( output_data( last_opnd, OPND1 ) );
            }
            break;
        case OP_M16:
            if( (cur_opnd & OP_M)
              && ( MEM_TYPE( Code->mem_type, WORD )
                || Code->mem_type == MT_EMPTY ) ) {
                if( output( *ins_pos ) )
                    return( RC_ERROR );
                if( output_data( last_opnd, OPND1 ) )
                    return( RC_ERROR );
                return( output_data( cur_opnd, OPND2 ) );
            }
            break;
        case OP_M:
            if( cur_opnd & asm_op2 ) {
                if( check_3rd_operand( *ins_pos ) )
                    break;
                if( output( *ins_pos ) )
                    return( RC_ERROR );
                if( output_data( last_opnd, OPND1 ) )
                    return( RC_ERROR );
                if( output_data( cur_opnd, OPND2 ) )
                    return( RC_ERROR );
                if( output_3rd_operand( *ins_pos ) )
                    return( RC_ERROR );
                return( output_3DNow( *ins_pos ) );
            }
            break;
        default:
            if( (asm_op2 & (OP_MMX | OP_XMM))
              && (cur_opnd & asm_op2) ) {
                if( check_3rd_operand( *ins_pos ) )
                    break;
                if( output( *ins_pos ) )
                    return( RC_ERROR );
                if( output_data( last_opnd, OPND1 ) )
                    return( RC_ERROR );
                if( (cur_opnd & OP_M_ANY) && (asm_op2 & OP_M_ANY) ) {
                    if( output_data( cur_opnd, OPND2 ) ) {
                        return( RC_ERROR );
                    }
                }
                if( output_3rd_operand( *ins_pos ) )
                    return( RC_ERROR );
                return( output_3DNow( *ins_pos ) );
            }
            break;
        }
        (*ins_pos)++;
    }
    (*ins_pos)--;
    *found = FALSE;
    return( RC_ERROR );
}
