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


static uint_8 sizeofReg( processor_reg reg )
{
    if( reg < FIRST_WORD_REG ) {
        return( 1 );
    } else if( reg < FIRST_DWORD_REG ) {
        return( 2 );
    } else if( reg < FIRST_SEG_REG ) {
        return( 4 );
    } else {
        return( 2 );
    }
}

static char sizeToBWL( uint_8 size )
{
    switch( size ) {
    case 1:
        return( 'b' );
    case 2:
        return( 'w' );
    case 4:
        return( 'l' );
    default:
        return( '\0' );
    }
}

static char pickLST( mem_modifier modifier )
{
    switch( modifier ) {
    case MOD_LREAL:
        return( 'l' );
    case MOD_SREAL:
        return( 's' );
    case MOD_TREAL:
        return( 't' );
    default:
        return( '\0' );
    }
}


static void appendLorLL( char * buff, mem_modifier modifier )
{
    char * append;

    switch( modifier ) {
    case MOD_LINT:
        append = "l";
        break;
    case MOD_LREAL:
        append = "ll";
    }
    strcat( buff, append );
}


/* Note: The following func's should be called before the operands are swapped */

static void appendSuffix( char * buff, instruction * instr )
{
    uint_8      size = 0;
    char        append;
    uint_16     opcode = instr->opcode;
    int         len;

    if( !(instr->pref & FP_INS) ) {     // 386 instruction
        switch( opcode ) {
        case I_JA:
        case I_JAE:
        case I_JB:
        case I_JBE:
        case I_JCXZ:
        case I_JE:
        case I_JG:
        case I_JGE:
        case I_JL:
        case I_JLE:
        case I_JMP:
        case I_JMP_FAR:
        case I_JNE:
        case I_JNO:
        case I_JNS:
        case I_JO:
        case I_JP:
        case I_JPO:
        case I_JS:
        case I_SETO:
        case I_SETNO:
        case I_SETB:
        case I_SETAE:
        case I_SETE:
        case I_SETNE:
        case I_SETBE:
        case I_SETA:
        case I_SETS:
        case I_SETNS:
        case I_SETP:
        case I_SETPO:
        case I_SETL:
        case I_SETGE:
        case I_SETLE:
        case I_SETG:
        case I_LAHF:
        case I_SAHF:
        case I_CMC:
        case I_CLC:
        case I_STC:
        case I_CLI:
        case I_STI:
        case I_CLD:
        case I_STD:
        case I_DAA:
        case I_DAS:
        case I_AAA:
        case I_AAS:
        case I_AAM:
        case I_AAD:
        case I_WAIT:
        case I_ESC:
        case I_MOVSB:
        case I_MOVSW:
        case I_MOVSD:
        case I_CMPSB:
        case I_CMPSW:
        case I_CMPSD:
        case I_STOSB:
        case I_STOSW:
        case I_STOSD:
        case I_LODSB:
        case I_LODSW:
        case I_LODSD:
        case I_SCASB:
        case I_SCASW:
        case I_SCASD:
        case I_XLAT:
        case I_REPE:
        case I_REPNE:
        case I_RET:
        case I_CALL:
        case I_RET_FAR:
        case I_RET_FAR_D:
        case I_CALL_FAR:
        case I_ENTER:
        case I_LEAVE:
        case I_LOOP:
        case I_LOOPNE:
        case I_LOOPE:
        case I_INT:
        case I_INTO:
        case I_IRET:
        case I_SLDT:
        case I_STR:
        case I_LLDT:
        case I_LTR:
        case I_VERR:
        case I_VERW:
        case I_SGDT:
        case I_SIDT:
        case I_LGDT:
        case I_LIDT:
        case I_SMSW:
        case I_LMSW:
        case I_LAR:
        case I_LSL:
        case I_CLTS:
        case I_LOCK:
        case I_NOP:
        case I_HALT:
            return;
        case I_MOVZX:
        case I_MOVSX:
            size = sizeofReg( instr->op[ OP_1 ].base );
            break;
        default:
            if( instr->mem_ref_op != NULL_OP ) {
                size = instr->mem_ref_size;
                switch( opcode ) {
                case I_LDS:
                case I_LES:
                case I_LFS:
                case I_LGS:
                case I_LSS:
                    size -= 2;
                }
            } else {
                int i;
                for( i = 0; i < instr->num_oper; i++ ) {
                    if( ( size = instr->op[ i ].size ) ) {  // not ==
                        break;
                    } else if( instr->op[ i ].mode == ADDR_REG ) {
                        size = sizeofReg( instr->op[ i ].base );
                        break;
                    }
                }
            }
        }
        append = sizeToBWL( size );
    } else if( opcode < FIRST_WTK_INS ) {       // 387 instruction
        if( instr->mem_ref_op != NULL_OP ) {
            switch( opcode ) {
            case I_FLD:
            case I_FST:
            case I_FSTP:
            case I_FADD:
            case I_FSUB:
            case I_FSUBR:
            case I_FMUL:
            case I_FDIV:
            case I_FDIVR:
            case I_FCOM:
            case I_FCOMP:
                append = pickLST( instr->modifier );
                break;
            case I_FILD:
            case I_FIST:
            case I_FISTP:
            case I_FIADD:
            case I_FIMUL:
            case I_FIDIV:
            case I_FIDIVR:
            case I_FICOM:
            case I_FICOMP:
                appendLorLL( buff, instr->modifier );  // fall through
            default:
                return;
            }
        } else {
            return;
        }
    } else {
//      remove the return and add this line for debugging
//      append = "";
        return;
    }
    len = strlen( buff );
    buff[ len ] = append;
    buff[ len + 1 ] = '\0';
}

/*
 * "fix" the instruction name in buff to Unix386's
 */
void ToUnixInsName( char * buff, instruction * instr )
{
    appendSuffix( buff, instr );
    switch( instr->opcode ) {
    case I_MOVZX:
    case I_MOVSX:
        buff[ 4 ] = sizeToBWL( instr->mem_ref_op != NULL_OP ?
                                instr->mem_ref_size :
                                sizeofReg( instr->op[ OP_2 ].base ) );
        break;
    case I_CBW:         // to cbtw
    case I_CWD:         // to cwtd
    case I_CDQ:         // to cltd
        strcpy( &buff[ 1 ], "ltd" );
        break;
    case I_CWDE:        // to cwtl
        buff[ 3 ] = 'l';
        buff[ 2 ] = 't';
        buff[ 4 ] = '\0';
        break;
    case I_MOVSD:       // to movsl
    case I_CMPSD:       // to cmpsl
    case I_STOSD:       // to stosl
    case I_LODSD:       // to lodsl
    case I_SCASD:       // to scasl
        buff[ 4 ] = 'l';
        break;
    case I_RET_FAR:
    case I_CALL_FAR:
    case I_JMP_FAR:
        memmove( buff + 1, buff, strlen( buff ) + 1 );
        buff[ 0 ] = 'l';
        if( instr->opcode == I_RET_FAR ) {      // lretf --> lret
            buff[ 4 ] = '\0';
        }
    }
}
