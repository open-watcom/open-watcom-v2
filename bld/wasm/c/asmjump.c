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


#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "asmglob.h"//
#include "asmops1.h"
#include "asmops2.h"
#include "asmopnds.h"
#include "asmins1.h"
#include "asmerr.h"
#include "asmsym.h"
#include "asmdefs.h"
#ifdef _WASM_
    #include "directiv.h"
    #include "myassert.h"
#endif

extern int_8                    Label_Idx;      // used in check_assume()
extern int_8                    PhaseError;
/* prototypes */
int operator( int mem_type, uint_8 fix_mem_type );
int jmp( int i );

extern int              mem2code( char, int, int );

#ifdef _WASM_

extern seg_list         *CurrSeg;       // points to stack of opened segments

#define Address         ( GetCurrAddr() )

extern void             InputQueueLine( char * );
extern void             GetInsString( enum asm_token, char *, int );
extern uint_32          GetCurrAddr( void );

static enum asm_token getJumpNegation( enum asm_token instruction )
/*****************************************************************/
{
    switch( instruction ) {
    case T_JA:          return( T_JNA );
    case T_JAE:         return( T_JNAE );
    case T_JB:          return( T_JNB );
    case T_JBE:         return( T_JNBE );
    case T_JC:          return( T_JNC );
    case T_JE:          return( T_JNE );
    case T_JG:          return( T_JNG );
    case T_JGE:         return( T_JNGE );
    case T_JL:          return( T_JNL );
    case T_JLE:         return( T_JNLE );
    case T_JNA:         return( T_JA );
    case T_JNAE:        return( T_JAE );
    case T_JNB:         return( T_JB );
    case T_JNBE:        return( T_JBE );
    case T_JNC:         return( T_JC );
    case T_JNE:         return( T_JE );
    case T_JNG:         return( T_JG );
    case T_JNGE:        return( T_JGE );
    case T_JNL:         return( T_JL );
    case T_JNLE:        return( T_JLE );
    case T_JNO:         return( T_JO );
    case T_JNP:         return( T_JP );
    case T_JNS:         return( T_JS );
    case T_JNZ:         return( T_JZ );
    case T_JO:          return( T_JNO );
    case T_JP:          return( T_JNP );
    case T_JPE:         return( T_JPO );
    case T_JPO:         return( T_JPE );
    case T_JS:          return( T_JNS );
    case T_JZ:          return( T_JNZ );
    default:
        return( (enum asm_token)ERROR );
    }
}

static void jumpExtend( void )
/****************************/
{
    unsigned i;
    unsigned next_ins_size;
    enum asm_token negation;
    char buffer[MAX_LINE_LEN];

    /* there MUST be a conditional jump instruction in asmbuffer */
    for( i=0; ; i++ ) {
        if( AsmBuffer[i]->token==T_INS && IS_JMP( AsmBuffer[i]->value ) ) break;
    }

    AsmWarn( 4, EXTENDING_JUMP );

    negation = getJumpNegation( AsmBuffer[i]->value );
    GetInsString( negation, buffer, MAX_LINE_LEN );
    next_ins_size = Code->use32 ? 5 : 3;
    sprintf( buffer + strlen( buffer ), " $+%d ", next_ins_size+2 );
    InputQueueLine( buffer );
    strcpy( buffer, "jmp NEAR PTR " );
    for( i++; AsmBuffer[i]->token != T_FINAL; i++ ) {
        switch( AsmBuffer[i]->token ) {
        case T_NUM:
        case T_DEC_NUM:
        case T_OCT_NUM:
        case T_HEX_NUM_0:
        case T_HEX_NUM:
        case T_BIN_NUM:
            itoa( AsmBuffer[i]->value, buffer+strlen( buffer ), 10 );
            break;
        case T_OP_SQ_BRACKET:
            strcat( buffer, "[" );
            break;
        case T_CL_SQ_BRACKET:
            strcat( buffer, "]" );
            break;
        default:
            strcat( buffer, AsmBuffer[i]->string_ptr );
            break;
        }
    }
    InputQueueLine( buffer );
    return;
}
#endif

int jmp( int i )                // Bug: can't handle indirect jump
/*
  determine the displacement of jmp;
*/
{
    struct asm_sym      *sym;
    int_32              addr;
    int_32              temp;
    enum sym_state      state;
    #ifdef _WASM_
        dir_node                *seg;
        bool                    old_use32;
    #endif

    sym = AsmLookup( AsmBuffer[i]->string_ptr );
    if( sym == NULL ) return( ERROR );

#ifdef _WASM_
    if( sym->mem_type == ERROR ) {
        AsmError( LABEL_NOT_DEFINED );
        return ERROR;
    }
#endif
    if( Code->info.token == T_CALLF || Code->info.token == T_JMPF ) {
        if( Code->distance == EMPTY ) {
            Code->distance = T_FAR;
        }
    }
    state = sym->state;
    #ifdef _WASM_
        seg = GetSeg( sym );
        if( seg == NULL || CurrSeg == NULL || CurrSeg->seg != seg ) {
            /* jumps to another segment are just like to another file */
            state = SYM_EXTERNAL;
        }
    #endif

    switch( state ) {
    case SYM_INTERNAL:
    #ifdef _WASM_
        case SYM_PROC:
    #endif
        if(  ( Code->distance == EMPTY || Code->distance == T_SHORT
                || Code->distance == T_NEAR ) &&
            Code->mem_type == EMPTY &&
            sym->mem_type != T_WORD &&
            sym->mem_type != T_DWORD &&
            sym->mem_type != T_FWORD &&
            sym->mem_type != T_PWORD &&
            sym->mem_type != T_FAR ) {
            temp = 0;

            #ifdef _WASM_
                addr = sym->offset;
            #else
                addr = sym->addr;
            #endif
            addr -= ( Address + 2 );  // calculate the displacement
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
            if( Code->info.token == T_CALL && Code->distance == EMPTY ) {
                Code->distance = T_NEAR;
            }
            if( Code->distance != T_NEAR && Code->info.token != T_CALL
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
                #ifdef _WASM_
                    #define GOOD_PHASE  !PhaseError &&
                #else
                    #define GOOD_PHASE
                #endif
                if( GOOD_PHASE (Code->info.opnd_type[Opnd_Count] != OP_I8) ) {
                    AsmError( JUMP_OUT_OF_RANGE );
                    return( ERROR );
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
                        #ifdef _WASM_
                            if( Code->distance == EMPTY ) {
                                jumpExtend();
                                return( SCRAP_INSTRUCTION );
                            } else if( !PhaseError ) {
                                AsmError( JUMP_OUT_OF_RANGE );
                                return( ERROR );
                            }
                        #else
                            AsmError( JUMP_OUT_OF_RANGE );
                            return( ERROR );
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
        if( Code->mem_type == EMPTY && Code->distance == EMPTY &&
            sym->mem_type != 0 ) {
            switch( sym->mem_type ) {
            case T_FAR:
                if( Code->info.token == T_CALL || Code->info.token == T_JMP ) {
                    Code->info.token++;
                }
                // fall through
            case T_SHORT:
            case T_NEAR:
                Code->distance = sym->mem_type;
                break;
            case T_FWORD:
            case T_PWORD:
                if( operator( T_FWORD, TRUE ) == ERROR ) return( ERROR );
                break;
            default:
                Code->mem_type = sym->mem_type;
            }
        }
        if( operator( T_PTR, FALSE ) == ERROR ) return( ERROR ); // fixme

        if( !Code->use32 && MEM_TYPE( Code->mem_type, DWORD ) ) {
            Code->distance = T_FAR;
            if( Code->info.token == T_CALL  ||  Code->info.token == T_JMP ) {
                Code->info.token++;
            }
        }

        if( Code->info.token == T_CALL && Code->distance == EMPTY ) {
            Code->distance = T_NEAR;
        }

        switch( Code->info.token ) {
        case T_CALLF:
        case T_JMPF:

#ifdef _WASM_

            old_use32 = Code->use32;
            seg = GetSeg( sym );
            if( seg != NULL ) {
                Code->use32 = seg->e.seginfo->segrec->d.segdef.use_32;
            }
#endif

            switch( Code->distance ) {
            case T_SHORT:
            case T_NEAR:
                AsmError( CANNOT_USE_SHORT_OR_NEAR );
                return( ERROR );
            case EMPTY:
            case T_FAR:
                switch( Code->mem_type ) {
                case T_BYTE:
                case T_WORD:
#ifdef _WASM_
                case T_SBYTE:
                case T_SWORD:
#endif
                    AsmError( INVALID_SIZE );
                    return( ERROR );
                case T_DWORD:
                case T_FWORD:
                case T_PWORD:
#ifdef _WASM_
                case T_SDWORD:
#endif
                    return( INDIRECT_JUMP );
                case T_QWORD:
                case T_TBYTE:
                    AsmError( INVALID_SIZE );
                    return( ERROR );
                case EMPTY:
                    if( Code->use32 ) {
                        temp = FIX_PTR32;
                        Code->info.opnd_type[Opnd_Count] = OP_J48;
                        #ifdef _WASM_
                        if( !old_use32 ) {
                            Code->opsiz = NOT_EMPTY;
                        }
                        #endif
                    } else {
                        temp = FIX_PTR16;
                        Code->info.opnd_type[Opnd_Count] = OP_J32;
                        #ifdef _WASM_
                        if( old_use32 ) {
                            Code->opsiz = NOT_EMPTY;
                        }
                        #endif
                    }
                }
                break;
            }
            AddFixup( sym, temp );
            break;
        case T_CALL:
            if( Code->mem_type == T_SHORT ) {
                AsmError( CANNOT_USE_SHORT_WITH_CALL );
                return( ERROR );
            }
            /* fall through */
        case T_JMP:
            switch( Code->distance ) {
            case T_SHORT:
                temp = FIX_RELOFF8;
                Code->info.opnd_type[Opnd_Count] = OP_I8;
                break;
            case EMPTY:
                /* guess short, we will expand later if needed */
                if( Code->mem_type == EMPTY && Code->info.token == T_JMP && state != SYM_EXTERNAL ) {
                    temp = FIX_RELOFF8;
                    Code->info.opnd_type[Opnd_Count] = OP_I8;
                    break;
                }
            case T_NEAR:
                switch( Code->mem_type ) {
                case EMPTY:
                    /* near */
                    if( Code->use32 ) {
                        temp = FIX_RELOFF32;
                        Code->info.opnd_type[Opnd_Count] = OP_I32;
                    } else {
                        temp = FIX_RELOFF16;
                        Code->info.opnd_type[Opnd_Count] = OP_I16;
                    }
                    break;
                case T_DWORD:
                case T_WORD:
#ifdef _WASM_
                case T_SDWORD:
                case T_SWORD:
#endif
                    return( INDIRECT_JUMP );
#ifdef _WASM_
                case T_SBYTE:
#endif
                case T_BYTE:
                case T_FWORD:
                case T_PWORD:
                case T_QWORD:
                case T_TBYTE:
                    AsmError( INVALID_SIZE );
                    return( ERROR );
                default:
                    // something wierd?
                    if( Code->use32 ) {
                        temp = FIX_RELOFF32;
                        Code->info.opnd_type[Opnd_Count] = OP_I32;
                    } else {
                        temp = FIX_RELOFF16;
                        Code->info.opnd_type[Opnd_Count] = OP_I16;
                    }
                    break;
                }
                break;
            case T_FAR:
                AsmError( SYNTAX_ERROR );
#ifdef _WASM_
                myassert( 0 );
#endif
            }
            AddFixup( sym, temp );
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
            if( Code->mem_type != EMPTY && Code->mem_type != T_SHORT ) {
                AsmError( ONLY_SHORT_DISPLACEMENT_IS_ALLOWED );
                return( ERROR );
            }
            Code->info.opnd_type[Opnd_Count] = OP_I8;
            AddFixup( sym, FIX_RELOFF8 );
            break;
        default:
            if( (Code->info.cpu&P_CPU_MASK) >= P_386 ) {
                switch( Code->distance ) {
                case EMPTY:
                case T_SHORT:
                    temp = FIX_RELOFF8;
                    Code->info.opnd_type[Opnd_Count] = OP_I8;
                    break;
                case T_NEAR:
                    temp = FIX_RELOFF32;
                    Code->info.opnd_type[Opnd_Count] = OP_I32;
                    break;
                default:
                    if( Code->use32 ) {
                        temp = FIX_RELOFF32;
                        Code->info.opnd_type[Opnd_Count] = OP_I32;
                    } else {
                        temp = FIX_RELOFF16;
                        Code->info.opnd_type[Opnd_Count] = OP_I16;
                    }
                }
            } else {
                // the only mode in 8086, 80186, 80286 is
                // Jxx SHORT
                temp = FIX_RELOFF8;
                Code->info.opnd_type[Opnd_Count] = OP_I8;
            }
            AddFixup( sym, temp );
        }
        break;
    default: /* SYM_STACK */
        AsmError( NO_JUMP_TO_AUTO );
        return( ERROR );
    }
    if( Code->mem_type != EMPTY ) {
        if( mem2code( SCALE_FACTOR_1, EMPTY, EMPTY ) == ERROR ) {
            return( ERROR );
        }
    }
    return( NOT_ERROR );
}

int operator( int mem_type, uint_8 fix_mem_type )
/***********************************************/
/*
  determine what should be done with SHORT, NEAR, FAR, BYTE, WORD, DWORD, PTR
  operator;
*/
{
    /* new idea:
     * when we get a near/far/dword/etc, just set distance / mem_type
     * operator will be called again with PTR, then we set the opsiz, etc.
     */

    if( mem_type == T_PTR ) {
        /* finish deciding what type to make the inst NOW
         * ie: decide size overrides etc.
         */

        if( Code->use32 && MEM_TYPE( Code->mem_type, WORD ) ) {
            // if we are in use32 mode, we have to add OPSIZ prefix for
            // most of the 386 instructions ( except MOVSX and MOVZX )
            // when we find WORD PTR

            if( Code->info.opnd_type[OPND1] == OP_MMX ) {
                /* JBS 2001/02/19
                   no WORD operands for MMX instructions, only 64-bit or 128-bit
                        so no WORD override needed
                */
            } else {
                switch( Code->info.token ) {
                case T_MOVSX:
                case T_MOVZX:
                case T_LEA:
                    break;
                default:
                    Code->opsiz = NOT_EMPTY;
                    break;
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
                        Code->opsiz = NOT_EMPTY;
                    }
                }
            } else {
                /* jmp dword ptr XX --> we want a FAR jump */
                if( Code->distance == EMPTY ) {
                    mem_type = T_FAR; /* caught below */
                }
            }
        } else if( !Code->use32 &&
                (Code->mem_type == T_FWORD || Code->mem_type == T_PWORD) ) {
            Code->opsiz = NOT_EMPTY;
        }
    } else if( mem_type == T_FAR || mem_type == T_NEAR || mem_type == T_SHORT ){
        Code->distance = mem_type;
    } else {
        if( mem_type != 0 && Code->mem_type_fixed == FALSE ) {
            #ifdef _WASM_
            if( mem_type != T_STRUCT ) {
            #endif
                Code->mem_type = mem_type;
                if( fix_mem_type ) {
                    Code->mem_type_fixed = TRUE;
                }

                #if 0
                /* this was screwing up instructions such as FCOM,
                 * and appears not to be necessary
                 */
                if( MEM_TYPE( mem_type, DWORD ) || MEM_TYPE( mem_type, WORD ) ) {
                    Code->info.opcode |= W_BIT;
                }
                #endif
            #ifdef _WASM_
            }
            #endif
        }
    }

    if( mem_type == T_FAR || mem_type == T_FWORD || mem_type == T_PWORD ) {
        Code->distance = T_FAR;
        if( Code->info.token == T_CALL  ||  Code->info.token == T_JMP ) {
            Code->info.token++;
        }
    }
    return( NOT_ERROR );
}
