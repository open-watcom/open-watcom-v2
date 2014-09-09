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
* Description:  CALL/JMP processing
*
****************************************************************************/


#include "asmglob.h"
#include "asmfixup.h"
#include "asmeval.h"
#if defined( _STANDALONE_ )
  #include "directiv.h"
  #include "asminput.h"
#endif

/* prototypes */
bool ptr_operator( memtype mem_type, bool fix_mem_type );

#if defined( _STANDALONE_ )

extern bool             SymIs32( struct asm_sym *sym );
extern void             check_assume( struct asm_sym *sym, prefix_reg default_reg );
extern void             find_frame( struct asm_sym *sym );

static int getJumpNegation( asm_token tok, char *buffer )
/*******************************************************/
{
    switch( tok ) {
    case T_JA:      tok = T_JNA;    break;
    case T_JAE:     tok = T_JNAE;   break;
    case T_JB:      tok = T_JNB;    break;
    case T_JBE:     tok = T_JNBE;   break;
    case T_JC:      tok = T_JNC;    break;
    case T_JE:      tok = T_JNE;    break;
    case T_JG:      tok = T_JNG;    break;
    case T_JGE:     tok = T_JNGE;   break;
    case T_JL:      tok = T_JNL;    break;
    case T_JLE:     tok = T_JNLE;   break;
    case T_JNA:     tok = T_JA;     break;
    case T_JNAE:    tok = T_JAE;    break;
    case T_JNB:     tok = T_JB;     break;
    case T_JNBE:    tok = T_JBE;    break;
    case T_JNC:     tok = T_JC;     break;
    case T_JNE:     tok = T_JE;     break;
    case T_JNG:     tok = T_JG;     break;
    case T_JNGE:    tok = T_JGE;    break;
    case T_JNL:     tok = T_JL;     break;
    case T_JNLE:    tok = T_JLE;    break;
    case T_JNO:     tok = T_JO;     break;
    case T_JNP:     tok = T_JP;     break;
    case T_JNS:     tok = T_JS;     break;
    case T_JNZ:     tok = T_JZ;     break;
    case T_JO:      tok = T_JNO;    break;
    case T_JP:      tok = T_JNP;    break;
    case T_JPE:     tok = T_JPO;    break;
    case T_JPO:     tok = T_JPE;    break;
    case T_JS:      tok = T_JNS;    break;
    case T_JZ:      tok = T_JNZ;    break;
    default:
        *buffer = '\0';
        return( 0 );
    }
    return( GetInsString( tok, buffer ) );
}

static void jumpExtend( int far_flag )
/*************************************/
{
    token_idx   i;
    unsigned    next_ins_size;
    char        buffer[MAX_LINE_LEN];
    char        *p;
    size_t      len;

    /* there MUST be a conditional jump instruction in asmbuffer */
    for( i = 0; ; i++ ) {
        if( ( AsmBuffer[i].class == TC_INSTR )
            && IS_JMP( AsmBuffer[i].u.token ) ) {
            break;
        }
    }

    AsmNote( 4, EXTENDING_JUMP );

    p = buffer + getJumpNegation( AsmBuffer[i].u.token, buffer );
    if( far_flag ) {
        next_ins_size = Code->use32 ? 7 : 5;
    } else {
        next_ins_size = Code->use32 ? 5 : 3;
    }
    sprintf( buffer + strlen( buffer ), " SHORT $+%d ", next_ins_size + 2 );
    InputQueueLine( buffer );
    p = buffer;
    if( far_flag ) {
        p = CATLIT( p, "jmpf " );
    } else {
        p = CATLIT( p, "jmp " );
    }
    for( i++; AsmBuffer[i].class != TC_FINAL; i++ ) {
        switch( AsmBuffer[i].class ) {
        case TC_NUM:
            p += sprintf( p, "%lu", AsmBuffer[i].u.value );
            break;
        case TC_OP_SQ_BRACKET:
            *p++ = '[';
            break;
        case TC_CL_SQ_BRACKET:
            *p++ = ']';
            break;
        default:
            len = strlen( AsmBuffer[i].string_ptr );
            p = CATSTR( p, AsmBuffer[i].string_ptr, len );
            break;
        }
    }
    *p = '\0';
    InputQueueLine( buffer );
    return;
}

static void FarCallToNear( void )
/*******************************/
{
    token_idx   i;
    char        buffer[MAX_LINE_LEN];
    char        *p;
    size_t      len;

    /* there MUST be a call instruction in asmbuffer */
    for( i = 0; ; i++ ) {
        if( ( AsmBuffer[i].class == TC_INSTR )
            && ( AsmBuffer[i].u.token == T_CALL ) ) {
            break;
        }
    }
    if( Parse_Pass == PASS_2 )
        AsmNote( 4, CALL_FAR_TO_NEAR );
    InputQueueLine( "PUSH CS" );
    p = buffer;
#if defined( _STANDALONE_ )
    if( Options.mode & MODE_IDEAL ) {
        p = CATLIT( p, "CALL NEAR " );
    } else {
        p = CATLIT( p, "CALL NEAR PTR " );
    }
#else
    p = CATLIT( p, "CALL NEAR PTR " );
#endif
    for( i++; AsmBuffer[i].class != TC_FINAL; i++ ) {
        switch( AsmBuffer[i].class ) {
        case TC_NUM:
            p += sprintf( p, "%lu", AsmBuffer[i].u.value );
            break;
        case TC_OP_SQ_BRACKET:
            *p++ = '[';
            break;
        case TC_CL_SQ_BRACKET:
            *p++ = ']';
            break;
        default:
            len = strlen( AsmBuffer[i].string_ptr );
            p = CATSTR( p, AsmBuffer[i].string_ptr, len );
            break;
        }
    }
    *p = '\0';
    InputQueueLine( buffer );
    return;
}
#endif

bool jmp( expr_list *opndx, int *flags )
/*
  determine the displacement of jmp;
*/
{
    int_32              addr;
    enum fixup_types    fixup_type;
    enum fixup_options  fixup_option;
    enum sym_state      state;
    struct asm_sym      *sym;
#if defined( _STANDALONE_ )
    dir_node            *seg;
#endif

    *flags = 0;
    Code->data[Opnd_Count] = opndx->value;
    sym = opndx->sym;
    if( sym == NULL ) {
        if( Code->info.token == T_CALL ) {
            Code->info.token = T_CALLF;
        } else if( Code->info.token == T_JMP ) {
            Code->info.token = T_JMPF;
        }
        if( Code->data[Opnd_Count] > USHRT_MAX )
            Code->info.opnd_type[Opnd_Count] = OP_I32;
        else
            Code->info.opnd_type[Opnd_Count] = OP_I16;
        return( RC_OK );
    }

#if defined( _STANDALONE_ )
    if( sym->mem_type == MT_ERROR ) {
        AsmError( LABEL_NOT_DEFINED );
        return( RC_ERROR );
    }
#endif
    state = sym->state;
#if defined( _STANDALONE_ )
    seg = GetSeg( sym );
    if( seg == NULL || CurrSeg == NULL || CurrSeg->seg != seg ) {
        /* jumps to another segment are just like to another file */
        state = SYM_EXTERNAL;
    }
#endif

    if( !Code->mem_type_fixed ) {
        Code->mem_type = MT_EMPTY;
    }
    fixup_option = OPTJ_NONE;
    fixup_type = FIX_RELOFF8;
    switch( state ) {
    case SYM_INTERNAL:
#if defined( _STANDALONE_ )
    case SYM_PROC:
#endif
        if(  ( Code->mem_type == MT_EMPTY || Code->mem_type == MT_SHORT
                || Code->mem_type == MT_NEAR )
            && sym->mem_type != MT_WORD
            && sym->mem_type != MT_DWORD
            && sym->mem_type != MT_FWORD
            && !IS_JMPCALLF( Code->info.token ) ) {
#if defined( _STANDALONE_ )
            if( ( Code->info.token == T_CALL )
                && ( Code->mem_type == MT_EMPTY )
                && ( sym->mem_type == MT_FAR ) ) {
                FarCallToNear();
                *flags = SCRAP_INSTRUCTION;
                return( RC_OK );
            }
            addr = sym->offset;
#else
            addr = sym->addr;
#endif
            addr -= ( AsmCodeAddress + 2 );  // calculate the displacement
            addr += Code->data[Opnd_Count];
            switch( Code->info.token ) {
            case T_JCXZ:
            case T_LOOPW:
            case T_LOOPEW:
            case T_LOOPZW:
            case T_LOOPNEW:
            case T_LOOPNZW:
                if( Code->use32 ) {
                    // 1 extra byte for OPNSIZ
                    addr--;
                }
                break;
            case T_JECXZ:
            case T_LOOPD:
            case T_LOOPED:
            case T_LOOPZD:
            case T_LOOPNED:
            case T_LOOPNZD:
                if( !Code->use32 ) {
                    // 1 extra byte for OPNSIZ
                    addr--;
                }
                break;
            }
            if( Code->info.token == T_CALL && Code->mem_type == MT_EMPTY ) {
                Code->mem_type = MT_NEAR;
            }
            if( Code->mem_type != MT_NEAR && Code->info.token != T_CALL
                && ( addr >= SCHAR_MIN && addr <= SCHAR_MAX ) ) {
                Code->info.opnd_type[Opnd_Count] = OP_I8;
            } else {
                /* near jmp */
                if( Code->use32 ) {
                    Code->info.opnd_type[Opnd_Count] = OP_I32;
                    addr -= 3; // 32 bit displacement
                } else {
                    Code->info.opnd_type[Opnd_Count] = OP_I16;
                    addr -= 1; // 16 bit displacement
                }
                if( IS_JMP( Code->info.token ) ) {
                    switch( Code->info.token ) {
                    case T_JMP:
                    case T_JMPF:
                    case T_JCXZ:
                    case T_JECXZ:
                        break;
                    default:
                        // 1 extra byte for opcode ( 0F )
                        addr--;
                        break;
                    }
                }
            }

            /* store the displacement */
            Code->data[Opnd_Count] = addr;

            switch( Code->info.token ) {
            case T_JCXZ:
            case T_JECXZ:
            case T_LOOP:
            case T_LOOPE:
            case T_LOOPNE:
            case T_LOOPNZ:
            case T_LOOPZ:
            case T_LOOPD:
            case T_LOOPED:
            case T_LOOPNED:
            case T_LOOPNZD:
            case T_LOOPZD:
            case T_LOOPW:
            case T_LOOPEW:
            case T_LOOPNEW:
            case T_LOOPNZW:
            case T_LOOPZW:
#if defined( _STANDALONE_ )
                if( !PhaseError && (Code->info.opnd_type[Opnd_Count] != OP_I8) ) {
#else
                if( (Code->info.opnd_type[Opnd_Count] != OP_I8) ) {
#endif
                    AsmError( JUMP_OUT_OF_RANGE );
                    return( RC_ERROR );
                }
                Code->info.opnd_type[Opnd_Count] = OP_I8;
                break;
            }

            if( (Code->info.cpu&P_CPU_MASK) < P_386 && IS_JMP( Code->info.token ) ) {
                /* look into jump extension */
                switch( Code->info.token ) {
                case T_JMP:
                case T_JMPF:
                    break;
                default:
                    if( Code->info.opnd_type[Opnd_Count] != OP_I8 ) {
#if defined( _STANDALONE_ )
                        if( Code->mem_type == MT_EMPTY ) {
                            jumpExtend( 0 );
                            *flags = SCRAP_INSTRUCTION;
                            return( RC_OK );
                        } else if( !PhaseError ) {
                            AsmError( JUMP_OUT_OF_RANGE );
                            return( RC_ERROR );
                        }
#else
                        AsmError( JUMP_OUT_OF_RANGE );
                        return( RC_ERROR );
#endif
                    }
                }
            }
            break;
        }
        /* otherwise fall through & get handled like external symbols */
    case SYM_UNDEFINED:
    case SYM_EXTERNAL:

        /* forward ref, or external symbol */
        if( Code->mem_type == MT_EMPTY && sym->mem_type != MT_EMPTY ) {
            switch( sym->mem_type ) {
            case MT_FAR:
                if( Code->info.token == T_CALL ) {
                    Code->info.token = T_CALLF;
                } else if( Code->info.token == T_JMP ) {
                    Code->info.token = T_JMPF;
                }
                // fall through
            case MT_SHORT:
            case MT_NEAR:
                Code->mem_type = sym->mem_type;
                break;
#if defined( _STANDALONE_ )
            case MT_PROC:
                if( IS_PROC_FAR() ) {
                    Code->mem_type = MT_FAR;
                    if( Code->info.token == T_CALL ) {
                        Code->info.token = T_CALLF;
                    } else if( Code->info.token == T_JMP ) {
                        Code->info.token = T_JMPF;
                    }
                } else {
                    Code->mem_type = MT_NEAR;
                }
                break;
#endif
            case MT_FWORD:
                if( ptr_operator( MT_FWORD, true ) )
                    return( RC_ERROR );
                break;
            default:
                Code->mem_type = sym->mem_type;
            }
        }
        if( Code->mem_type == MT_FAR ) {
            if( Code->info.token == T_CALL ) {
                Code->info.token = T_CALLF;
            } else if( Code->info.token == T_JMP ) {
                Code->info.token = T_JMPF;
            }
        }
        switch( Code->info.token ) {
        case T_CALLF:
        case T_JMPF:
            switch( Code->mem_type ) {
            case MT_SHORT:
            case MT_NEAR:
                if( Opnd_Count == OPND1 && Code->mem_type_fixed ) {
                    AsmError( CANNOT_USE_SHORT_OR_NEAR );
                    return( RC_ERROR );
                }
                /* fall through */
            case MT_FAR:
            case MT_EMPTY:
#if defined( _STANDALONE_ )
                SET_OPSIZ( Code, SymIs32( sym ));
                find_frame( sym );
#endif
                if( Opnd_Count == OPND2 ) {
                    if( IS_OPSIZ_32( Code ) ) {
                        fixup_type = FIX_OFF32;
                        Code->info.opnd_type[Opnd_Count] = OP_I32;
                    } else {
                        fixup_type = FIX_OFF16;
                        Code->info.opnd_type[Opnd_Count] = OP_I16;
                    }
                } else {
                    if( IS_OPSIZ_32( Code ) ) {
                        fixup_type = FIX_PTR32;
                        Code->info.opnd_type[Opnd_Count] = OP_J48;
                    } else {
                        fixup_type = FIX_PTR16;
                        Code->info.opnd_type[Opnd_Count] = OP_J32;
                    }
                }
                break;
            case MT_BYTE:
            case MT_WORD:
#if defined( _STANDALONE_ )
            case MT_SBYTE:
            case MT_SWORD:
#endif
                AsmError( INVALID_SIZE );
                return( RC_ERROR );
            case MT_DWORD:
            case MT_FWORD:
#if defined( _STANDALONE_ )
            case MT_SDWORD:
#endif
                *flags = INDIRECT_JUMP;
                return( RC_OK );
            case MT_QWORD:
            case MT_TBYTE:
            case MT_OWORD:
                AsmError( INVALID_SIZE );
                return( RC_ERROR );
            }
            break;
        case T_CALL:
            if( Code->mem_type == MT_SHORT ) {
                AsmError( CANNOT_USE_SHORT_WITH_CALL );
                return( RC_ERROR );
            } else if( Code->mem_type == MT_EMPTY ) {
#if defined( _STANDALONE_ )
                fixup_option = OPTJ_CALL;
#else
                fixup_option = OPTJ_NONE;
#endif
                if( Code->use32 ) {
                    fixup_type = FIX_RELOFF32;
                    Code->info.opnd_type[Opnd_Count] = OP_I32;
                } else {
                    fixup_type = FIX_RELOFF16;
                    Code->info.opnd_type[Opnd_Count] = OP_I16;
                }
                break;
            }
            /* fall through */
        case T_JMP:
            switch( Code->mem_type ) {
            case MT_SHORT:
                fixup_option = OPTJ_EXPLICIT;
                fixup_type = FIX_RELOFF8;
                Code->info.opnd_type[Opnd_Count] = OP_I8;
                break;
            case MT_FAR:
                AsmError( SYNTAX_ERROR );
                break;
            case MT_EMPTY:
                // forward reference
                // inline assembler jmp default distance is near
                // stand-alone assembler jmp default distance is short
                fixup_option = OPTJ_NONE;
#if defined( _STANDALONE_ )
                /* guess short if JMP, we will expand later if needed */
                fixup_type = FIX_RELOFF8;
                Code->info.opnd_type[Opnd_Count] = OP_I8;
#else
                if( Code->use32 ) {
                    fixup_type = FIX_RELOFF32;
                    Code->info.opnd_type[Opnd_Count] = OP_I32;
                } else {
                    fixup_type = FIX_RELOFF16;
                    Code->info.opnd_type[Opnd_Count] = OP_I16;
                }
#endif
                break;
            case MT_NEAR:
                fixup_option = OPTJ_EXPLICIT;
                if( Code->use32 ) {
                    fixup_type = FIX_RELOFF32;
                    Code->info.opnd_type[Opnd_Count] = OP_I32;
                } else {
                    fixup_type = FIX_RELOFF16;
                    Code->info.opnd_type[Opnd_Count] = OP_I16;
                }
                break;
            case MT_DWORD:
            case MT_WORD:
#if defined( _STANDALONE_ )
            case MT_SDWORD:
            case MT_SWORD:
#endif
                *flags = INDIRECT_JUMP;
                return( RC_OK );
#if defined( _STANDALONE_ )
            case MT_SBYTE:
#endif
            case MT_BYTE:
            case MT_FWORD:
            case MT_QWORD:
            case MT_TBYTE:
            case MT_OWORD:
                AsmError( INVALID_SIZE );
                return( RC_ERROR );
            }
//            check_assume( sym, PREFIX_EMPTY );
            break;
        case T_JCXZ:
        case T_JECXZ:
            // JCXZ and JECXZ always require SHORT label
        case T_LOOP:
        case T_LOOPE:
        case T_LOOPNE:
        case T_LOOPNZ:
        case T_LOOPZ:
        case T_LOOPD:
        case T_LOOPED:
        case T_LOOPNED:
        case T_LOOPNZD:
        case T_LOOPZD:
        case T_LOOPW:
        case T_LOOPEW:
        case T_LOOPNEW:
        case T_LOOPNZW:
        case T_LOOPZW:
#if defined( _STANDALONE_ )
            if( ( Code->mem_type != MT_EMPTY ) &&
                ( Code->mem_type != MT_SHORT ) &&
                ( (Options.mode & MODE_IDEAL) == 0 ) ) {
#else
            if( ( Code->mem_type != MT_EMPTY ) &&
                ( Code->mem_type != MT_SHORT ) ) {
#endif
                AsmError( ONLY_SHORT_DISPLACEMENT_IS_ALLOWED );
                return( RC_ERROR );
            }
            Code->info.opnd_type[Opnd_Count] = OP_I8;
            fixup_option = OPTJ_EXPLICIT;
            fixup_type = FIX_RELOFF8;
            break;
        default:
            if( (Code->info.cpu&P_CPU_MASK) >= P_386 ) {
                switch( Code->mem_type ) {
                case MT_SHORT:
                    fixup_option = OPTJ_EXPLICIT;
                    fixup_type = FIX_RELOFF8;
                    Code->info.opnd_type[Opnd_Count] = OP_I8;
                    break;
                case MT_EMPTY:
                    // forward reference
                    // inline assembler default distance is near
                    // stand-alone assembler default distance is short
#if defined( _STANDALONE_ )
                    fixup_option = OPTJ_JXX;
                    fixup_type = FIX_RELOFF8;
                    Code->info.opnd_type[Opnd_Count] = OP_I8;
                    break;
#endif
                case MT_NEAR:
                    fixup_option = OPTJ_EXPLICIT;
                    if( Code->use32 ) {
                        fixup_type = FIX_RELOFF32;
                        Code->info.opnd_type[Opnd_Count] = OP_I32;
                    } else {
                        fixup_type = FIX_RELOFF16;
                        Code->info.opnd_type[Opnd_Count] = OP_I16;
                    }
                    break;
                case MT_FAR:
#if defined( _STANDALONE_ )
                    jumpExtend( 1 );
                    *flags = SCRAP_INSTRUCTION;
                    return( RC_OK );
#endif
                default:
                    AsmError( ONLY_SHORT_AND_NEAR_DISPLACEMENT_IS_ALLOWED );
                    return( RC_ERROR );
                }
            } else {
                // the only mode in 8086, 80186, 80286 is
                // Jxx SHORT
                switch( Code->mem_type ) {
                case MT_EMPTY:
#if defined( _STANDALONE_ )
                    fixup_option = OPTJ_EXTEND;
                    fixup_type = FIX_RELOFF8;
                    Code->info.opnd_type[Opnd_Count] = OP_I8;
                    break;
#endif
                case MT_SHORT:
                    fixup_option = OPTJ_EXPLICIT;
                    fixup_type = FIX_RELOFF8;
                    Code->info.opnd_type[Opnd_Count] = OP_I8;
                    break;
                default:
                    AsmError( ONLY_SHORT_DISPLACEMENT_IS_ALLOWED );
                    return( RC_ERROR );
                }
            }
        }
        AddFixup( sym, fixup_type, fixup_option );
        break;
    default: /* SYM_STACK */
        AsmError( NO_JUMP_TO_AUTO );
        return( RC_ERROR );
    }
    return( RC_OK );
}

bool ptr_operator( memtype mem_type, bool fix_mem_type )
/******************************************************/
/*
  determine what should be done with SHORT, NEAR, FAR, BYTE, WORD, DWORD, PTR
  operator;
*/
{
    /* new idea:
     * when we get a near/far/dword/etc, just set distance / mem_type
     * operator will be called again with PTR, then we set the opsiz, etc.
     */

    if( Code->info.token == T_LEA )
        return( RC_OK );
    if( Code->info.token == T_SMSW )
        return( RC_OK );
    if( mem_type == MT_PTR ) {
        /* finish deciding what type to make the inst NOW
         * ie: decide size overrides etc.
         */
        if( Code->use32 && MEM_TYPE( Code->mem_type, WORD ) ) {
            // if we are in use32 mode, we have to add OPSIZ prefix for
            // most of the 386 instructions ( except MOVSX and MOVZX )
            // when we find WORD PTR

            if( !IS_BRANCH( Code->info.token ) ) {
                if( Code->info.opnd_type[OPND1] == OP_MMX ) {
                /* JBS 2001/02/19
                no WORD operands for MMX instructions, only 64-bit or 128-bit
                so no WORD override needed
                    */
                } else {
                    switch( Code->info.token ) {
                    case T_MOVSX:
                    case T_MOVZX:
                        break;
                    default:
                        SET_OPSIZ_ON( Code );
                        break;
                    }
                }
            }

        } else if( !Code->use32 && MEM_TYPE( Code->mem_type, DWORD ) ) {

            /* if we are not in use32 mode, we have to add OPSIZ
             * when we find DWORD PTR
             * unless we have a LXS ins.
             * which moves a DWORD ptr into SR:word reg
             * fixme  - can this be done by breaking up the LXS instructions in
             *          asmins.h, and then putting F_32 or F_16 to append
             *      opsize bytes when necessary ?
             */
            if( !IS_BRANCH( Code->info.token ) ) {

                if( Code->info.opnd_type[OPND1] == OP_MMX ) {
                    /* JBS 2001/02/19
                       no WORD operands for MMX instructions, only 64-bit or 128-bit
                       so no DWORD override needed
                     */
                } else {
                    switch( Code->info.token ) {
                    case T_LDS:
                    case T_LES:
                    case T_LFS:
                    case T_LGS:
                    case T_LSS:
                        /* in these cases, opsize does NOT need to be changed  */
                        break;
                    default:
                        // OPSIZ prefix
                        SET_OPSIZ_ON( Code );
                    }
                }
            }
        }
    } else {
        if( ( mem_type != MT_EMPTY ) && !Code->mem_type_fixed ) {
#if defined( _STANDALONE_ )
            if( mem_type != MT_STRUCT ) {
#endif
                Code->mem_type = mem_type;
                if( fix_mem_type ) {
                    Code->mem_type_fixed = true;
                    if( mem_type == MT_FAR ) {
                        if( Code->info.token == T_CALL ) {
                            Code->info.token = T_CALLF;
                        } else if( Code->info.token == T_JMP ) {
                            Code->info.token = T_JMPF;
                        }
                    }
                }

#if defined( _STANDALONE_ )
            }
#endif
        }
    }
    return( RC_OK );
}
