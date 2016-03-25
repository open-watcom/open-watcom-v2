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


/* set this macro to 0 if you do NOT want to allow structures to be defined
 * ie:
 *    struct foo
 *      ...
 *    foo ends
 *    ...
 *    bar foo < 4, 2, 3 >
 */
#define ALLOW_STRUCT_INIT 1

#include "asmglob.h"
#include "hash.h"
#include "asmexpnd.h"
#include "asmfixup.h"
#include "asmeval.h"
#include "asmlabel.h"
#if defined( _STANDALONE_ )
  #include "directiv.h"
  #include "myassert.h"
  #include "asminput.h"
#endif
#include "asmjump.h"
#include "asminsd.h"
#include "asmopsd.gh"

#include "clibext.h"


extern bool             ptr_operator( memtype, bool );

static struct asm_code  Code_Info;
struct asm_code         *Code = &Code_Info;

operand_idx             Opnd_Count;

static void             SizeString( unsigned op_size );
static bool             check_size( void );
static bool             segm_override_jumps( expr_list *opndx );


#if defined( _STANDALONE_ )

extern bool             DefineProc;     // true if the definition of procedure
                                        // has not ended

static void             check_assume( struct asm_sym *, prefix_reg );

bool                    CheckSeg;       // if checking of opened segment is needed
struct asm_sym          *Frame;         // Frame of current fixup
struct asm_sym          *SegOverride;

#else

#define     directive( i, value )   cpu_directive( value )

#endif

extern void             make_inst_hash_table( void );

static bool             ConstantOnly;

static bool             mem2code( unsigned char, asm_token, asm_token, asm_sym * );

asm_tok                 AsmBuffer[MAX_TOKEN];  // buffer to store token

#if defined( _STANDALONE_ )

void find_frame( struct asm_sym *sym )
/*******************************************/
{
    if( SegOverride != NULL ) {
        sym = SegOverride;
        if( sym->state == SYM_GRP ) {
            Frame = sym;
        } else if( sym->segment != NULL ) {
            Frame = sym->segment;
        }
    } else {
        switch( sym->state ) {
        case SYM_INTERNAL:
        case SYM_PROC:
            if( sym->segment != NULL ) {
                if( GetGrp( sym ) != NULL ) {
                    Frame = GetGrp( sym );
                } else {
                    Frame = sym->segment;
                }
            }
            break;
        case SYM_GRP:
            Frame = sym;
            break;
        case SYM_SEG:
            Frame = sym->segment;
            break;
        default:
            break;
        }
    }
}

int GetInsString( asm_token token, char *string )
/***********************************************/
{
    int index;
    int len;

    len = AsmOpcode[token].len;
    index = AsmOpcode[token].index;
    if( AsmChars[index] == '.' ) {
        index++;
        len--;
    }
    memcpy( string, AsmChars + index, len );
    string[len] = '\0';
    return( len );
}
#endif

const asm_ins ASMFAR *get_instruction( char *string )
/***************************************************/
{
    asm_token   token;

    for( token = inst_table[hashpjw( string )]; token--; token = AsmOpcode[token].next ) {
        if( strnicmp( string, AsmChars + AsmOpcode[token].index, AsmOpcode[token].len ) == 0
            && string[AsmOpcode[token].len] == '\0' ) {
            return( AsmOpTable + AsmOpcode[token].position );
        }
    }
    return( NULL );
}

static bool comp_mem( asm_token reg1, asm_token reg2, unsigned char *rm_field )
/*****************************************************************************/
/*
- compare and return the r/m field encoding of 16-bit address mode;
- call by mem2code() only;
*/
{
    switch( reg1 ) {
    case T_BX:
        switch( reg2 ) {
        case T_SI:
            *rm_field = MEM_BX_SI;
            return( RC_OK );
        case T_DI:
            *rm_field = MEM_BX_DI;
            return( RC_OK );
        }
        break;
    case T_BP:
        switch( reg2 ) {
        case T_SI:
            *rm_field = MEM_BP_SI;
            return( RC_OK );
        case T_DI:
            *rm_field = MEM_BP_DI;
            return( RC_OK );
        }
        break;
    }
    AsmError( INVALID_MEMORY_POINTER );
    return( RC_ERROR );
}

static void seg_override( asm_token seg_reg, asm_sym *sym )
/*********************************************************/
/*
- determine if segment override is necessary with the current address mode;
*/
{
    prefix_reg          default_seg;
#if defined( _STANDALONE_ )
    enum assume_reg     assume_seg;

    switch( seg_reg ) {
    case T_SS:
    case T_BP:
    case T_EBP:
    case T_ESP:
        default_seg = PREFIX_SS;
        break;
    default:
        default_seg = PREFIX_DS;
        break;
    }
    if( sym != NULL ) {
        if( Code->prefix.seg == PREFIX_EMPTY ) {
            if( Code->info.token == T_LEA ) {
                check_assume( sym, PREFIX_EMPTY );
            } else {
                check_assume( sym, default_seg );
            }
        } else {
            switch( Code->prefix.seg ) {
            case PREFIX_ES:
                assume_seg = ASSUME_ES;
                break;
            case PREFIX_CS:
                assume_seg = ASSUME_CS;
                break;
            case PREFIX_SS:
                assume_seg = ASSUME_SS;
                break;
            case PREFIX_DS:
                assume_seg = ASSUME_DS;
                break;
            case PREFIX_FS:
                assume_seg = ASSUME_FS;
                break;
            case PREFIX_GS:
                assume_seg = ASSUME_GS;
                break;
            default:
                assume_seg = 0;
                break;
            }
            if( GetPrefixAssume( sym, assume_seg ) == ASSUME_NOTHING ) {
//                AsmWarn( 3, CANNOT_ADDRESS_WITH_ASSUMED_REGISTER );
            }
        }
    }

    if( Code->prefix.seg == default_seg ) {
        Code->prefix.seg = PREFIX_EMPTY;
    }
#else
    sym = sym;
    if( Code->prefix.seg != PREFIX_EMPTY ) {
        switch( seg_reg ) {
        case T_SS:
        case T_BP:
        case T_EBP:
        case T_ESP:
            default_seg = PREFIX_SS;
            break;
        default:
            default_seg = PREFIX_DS;
        }
        if( Code->prefix.seg == default_seg ) {
            Code->prefix.seg = PREFIX_EMPTY;
        }
    }
#endif
}

#if defined( _STANDALONE_ )

static void check_assume( struct asm_sym *sym, prefix_reg default_reg )
/**************************************************************************/
/* Check if an assumed register is found, and prefix a register if necessary */
{
    enum assume_reg     reg;
    enum assume_reg     def_reg;

    /**/myassert( sym != NULL );
    if( sym->state == SYM_UNDEFINED )
        return;

    switch( default_reg ) {
    case PREFIX_SS:
        def_reg = ASSUME_SS;
        break;
    case PREFIX_DS:
        def_reg = ASSUME_DS;
        break;
    default:
        def_reg = ASSUME_NOTHING;
        break;
    }

    reg = GetAssume( sym, def_reg );

    if( reg == ASSUME_NOTHING ) {
        if( ( sym->state != SYM_EXTERNAL ) && ( sym->state != SYM_PROC ) ) {
            if( Parse_Pass == PASS_2 ) {
                AsmWarn( 3, CANNOT_ADDRESS_WITH_ASSUMED_REGISTER );
            }
        } else {
            Code->prefix.seg = default_reg;
        }
    } else if( default_reg != PREFIX_EMPTY ) {
        switch( reg ) {
        case ASSUME_ES:
            Code->prefix.seg = PREFIX_ES;
            break;
        case ASSUME_CS:
            Code->prefix.seg = PREFIX_CS;
            break;
        case ASSUME_DS:
            Code->prefix.seg = PREFIX_DS;
            break;
        case ASSUME_GS:
            Code->prefix.seg = PREFIX_GS;
            break;
        case ASSUME_FS:
            Code->prefix.seg = PREFIX_FS;
            break;
        case ASSUME_SS:
            Code->prefix.seg = PREFIX_SS;
            break;
        default:
            break;
        }
    }
}

bool check_override( token_idx *i )
/*********************************/
/* Check if there is a register, segment or group override */
{
    token_idx   index;

    index = *i;

    if( ( index + 2 ) < Token_Count ) {
        if( AsmBuffer[index+1].class == TC_COLON ) {
            switch( AsmBuffer[index].class ) {
            case TC_REG:
                Code->prefix.seg =
                    AsmOpTable[AsmOpcode[AsmBuffer[index].u.token].position].opcode;
                (*i) += 2;
                if( *i >= Token_Count ) {
                    AsmError( LABEL_EXPECTED_AFTER_COLON );
                    return( RC_ERROR );
                }
                break;
            case TC_ID:      // Segment or Group override
                if( FixOverride(*i) ) {
                    return( RC_ERROR );
                }
                (*i) += 2;
                if( *i >= Token_Count ) {
                    AsmError( LABEL_EXPECTED_AFTER_COLON );
                    return( RC_ERROR );
                }
                break;
            default:
                break;
            }
        }
    }
    return( RC_OK );
}
#endif

static unsigned char Reg386( asm_token reg_token )
/************************************************/
{
    switch( reg_token ) {
    case T_EAX:         return( 0 );
    case T_ECX:         return( 1 );
    case T_EDX:         return( 2 );
    case T_EBX:         return( 3 );
    case T_ESP:         return( 4 );
    case T_EBP:         return( 5 );
    case T_ESI:         return( 6 );
    case T_EDI:         return( 7 );
    }
    /* I don't think this should happen */
    return( 0 );
}

int OperandSize( OPNDTYPE opnd )
/******************************/
{
    if( ( opnd == OP_NONE ) || ( opnd & OP_SPECIAL ) ) {
        return( 0 );
    } else if( opnd == OP_M ) {
        /* fixme */
        switch( Code->mem_type ) {
        case MT_EMPTY:  return( 0 );
#if defined( _STANDALONE_ )
        case MT_SBYTE:
#endif
        case MT_BYTE:    return( 1 );
#if defined( _STANDALONE_ )
        case MT_SWORD:
#endif
        case MT_WORD:    return( 2 );
#if defined( _STANDALONE_ )
        case MT_SDWORD:
#endif
        case MT_DWORD:   return( 4 );
        case MT_FWORD:   return( 6 );
        case MT_QWORD:   return( 8 );
        case MT_TBYTE:   return( 10 );
        case MT_OWORD:   return( 16 );
        default:         break;
        }
    } else if( opnd & ( OP_M8_R8 | OP_M_B | OP_I8 | OP_I_1 | OP_I_3 | OP_U8 ) ) {
        return( 1 );
    } else if( opnd & ( OP_M16_R16 | OP_M_W | OP_I16 | OP_SR ) ) {
        return( 2 );
    } else if( opnd & ( OP_R32 | OP_M_DW | OP_I32 | OP_J32 | OP_SPEC_REG ) ) {
        return( 4 );
    } else if( opnd & ( OP_I | OP_J48 ) ) {
        return( 6 );
    } else if( opnd & ( OP_M_QW | OP_MMX ) ) {
        return( 8 );
    } else if( opnd & ( OP_STI | OP_M_TB ) ) {
        return( 10 );
    } else if( opnd & ( OP_M_OW | OP_XMM ) ) {
        return( 16 );
    }
    return( 0 );
}

bool InRange( unsigned long val, unsigned bytes )
/**********************************************/
/*
     Can 'val' be represented in 'bytes' bytes?
*/
{
    unsigned long max;
    unsigned long mask;

    max = ( 1UL << ( bytes * 8 ) ) - 1;
    if( val <= max ) /* absolute value fits */
        return( true );
    mask = ~(max >> 1);
    if( ( val & mask ) == mask ) /* just a sign extension */
        return( true );
    return( false );

}

static bool mem2code( unsigned char ss, asm_token index, asm_token base, asm_sym *sym )
/*************************************************************************************/
/*
  encode the memory operand to machine code
*/
{
    struct asm_code     *rCode = Code;
    unsigned char       mod_field;
    unsigned char       rm_field;

    // clear mod
    rm_field = 0;
    if( InsFixups[Opnd_Count] != NULL ) {
        mod_field = MOD_10;
    } else if( rCode->data[Opnd_Count] == 0 ) {
        mod_field = MOD_00;
    } else if( ( rCode->data[Opnd_Count] > SCHAR_MAX )
       || ( rCode->data[Opnd_Count] < SCHAR_MIN ) ) {
        mod_field = MOD_10;
    } else {
        mod_field = MOD_01;
    }
    if( ( index == T_NULL ) && ( base == T_NULL ) ) {
        // direct memory
        // clear the rightmost 3 bits
        mod_field = MOD_00;
        if( IS_ADRSIZ_32( rCode ) ) {
            rm_field = D32;
        } else {
            if( !InRange( rCode->data[Opnd_Count], 2 ) ) {
                // expect 16-bit but got 32-bit address
                AsmError( DISPLACEMENT_OUT_OF_RANGE );
                return( RC_ERROR );
            }
            rm_field = D16;
        }
        // default is DS:[], DS: segment override is not needed
        seg_override( T_DS, sym );
    } else if( ( index == T_NULL ) && ( base != T_NULL ) ) {
        switch( base ) {
        case T_SI:
            rm_field = 0x04; // SI
            // default is DS:[], DS: segment override is not needed
            break;
        case T_DI:
            rm_field = 0x05; // DI
            // default is DS:[], DS: segment override is not needed
            break;
        case T_BP:
            rm_field = BP;
            if( mod_field == MOD_00 ) {
                mod_field = MOD_01;
            }
            // default is SS:[], SS: segment override is not needed
            break;
        case T_BX:
            rm_field = 0x07; // BX
            // default is DS:[], DS: segment override is not needed
            break;
        case T_EBP:
            rm_field = EBP;
            if( mod_field == MOD_00 ) {
                mod_field = MOD_01;
            }
            // default is SS:[], SS: segment override is not needed
            break;
        case T_ESP:
            rm_field = ESP;
            // ss = 00, index = 100 ( no index ), base = 100 ( ESP )
            rCode->sib = 0x24;
            // default is SS:[], SS: segment override is not needed
            break;
        default: // for 386 and up
            rm_field = Reg386( base );
            // default is DS:[], DS: segment override is not needed
        }
        seg_override( base, sym );
    } else if( ( index != T_NULL ) && ( base == T_NULL ) ) {
        // mod field is 00
        mod_field = MOD_00;
        // s-i-b is present ( r/m = 100 )
        rm_field = S_I_B;
        // scale factor, index, base ( 0x05 => no base reg )
        rCode->sib = ( ss | ( Reg386( index ) << 3 ) | 0x05 );
        // default is DS:[], DS: segment override is not needed
        seg_override( T_DS, sym );
    } else {
        // base != T_NULL && index != T_NULL
        switch( index ) {
        case T_BX:
        case T_BP:
            if( comp_mem( index, base, &rm_field ) )
                return( RC_ERROR );
            seg_override( index, sym );
            break;
        case T_SI:
        case T_DI:
            if( comp_mem( base, index, &rm_field ) )
                return( RC_ERROR );
            seg_override( base, sym );
            break;
        case T_ESP:
            AsmError( ESP_CANNOT_BE_USED_AS_INDEX );
            return( RC_ERROR );
        default:
            if( base < T_EAX ) {
                AsmError( CANNOT_MIX_16_AND_32_BIT_REGISTERS );
                return( RC_ERROR );
            } else if( base == T_EBP ) {
                if( mod_field == MOD_00 ) {
                    mod_field = MOD_01;
                }
            }
            // s-i-b is present ( r/m = 100 )
            rm_field |= S_I_B;
            rCode->sib = ( ss | (Reg386( index ) << 3) | Reg386( base ) );
            seg_override( base, sym );
        }
    }
    if( Opnd_Count == OPND2 ) {
        // shift the register field to left by 3 bit
        rCode->info.rm_byte = mod_field | ( rm_field << 3 ) | ( rCode->info.rm_byte & BIT_012 );
    } else if( Opnd_Count == OPND1 ) {
        rCode->info.rm_byte = mod_field | rm_field;
    }
    return( RC_OK );
}

static asm_cpu comp_opt( asm_token direct )
/*****************************************/
/*
  Compare function for CPU directive
*/
{
    switch( direct ) {
    case T_DOT_NO87:
        return( P_NO87 );
    case T_DOT_8086:
#if defined( _STANDALONE_ )
    case T_P8086:
#endif
        return( P_86 );
    case T_DOT_8087:
#if defined( _STANDALONE_ )
    case T_P8087:
#endif
        return( P_87 );
    case T_DOT_186:
#if defined( _STANDALONE_ )
    case T_P186:
#endif
        return( P_186 );
    case T_DOT_286:
#if defined( _STANDALONE_ )
    case T_P286:
#endif
        return( P_286 );
    case T_DOT_287:
#if defined( _STANDALONE_ )
    case T_P287:
#endif
        return( P_287 );
    case T_DOT_286P:
#if defined( _STANDALONE_ )
    case T_P286P:
#endif
        return( P_286p );
    case T_DOT_386:
#if defined( _STANDALONE_ )
    case T_P386:
#endif
        return( P_386 );
    case T_DOT_387:
#if defined( _STANDALONE_ )
    case T_P387:
#endif
        return( P_387 );
    case T_DOT_386P:
#if defined( _STANDALONE_ )
    case T_P386P:
#endif
        return( P_386p );
    case T_DOT_486:
#if defined( _STANDALONE_ )
    case T_P486:
#endif
        return( P_486 );
    case T_DOT_486P:
#if defined( _STANDALONE_ )
    case T_P486P:
#endif
        return( P_486p );
    case T_DOT_586:
#if defined( _STANDALONE_ )
    case T_P586:
#endif
        return( P_586 );
    case T_DOT_586P:
#if defined( _STANDALONE_ )
    case T_P586P:
#endif
        return( P_586p );
    case T_DOT_686:
#if defined( _STANDALONE_ )
    case T_P686:
#endif
        return( P_686 );
    case T_DOT_686P:
#if defined( _STANDALONE_ )
    case T_P686P:
#endif
        return( P_686p );
    case T_DOT_MMX:
#if defined( _STANDALONE_ )
    case T_PMMX:
#endif
        return( P_MMX );
    case T_DOT_K3D:
#if defined( _STANDALONE_ )
    case T_PK3D:
#endif
        return( P_K3D | P_MMX );
    case T_DOT_XMM:
#if defined( _STANDALONE_ )
    case T_PXMM:
#endif
        return( P_SSE | P_MMX );
    case T_DOT_XMM2:
#if defined( _STANDALONE_ )
    case T_PXMM2:
#endif
        return( P_SSE2 | P_SSE | P_MMX );
    case T_DOT_XMM3:
#if defined( _STANDALONE_ )
    case T_PXMM3:
#endif
        return( P_SSE3 | P_SSE2 | P_SSE | P_MMX );
    default:
        // not found
        return( P_EMPTY );
    }
}

static asm_cpu def_fpu( asm_token direct )
/****************************************/
/*
  get FPU from CPU directive
*/
{
    switch( direct ) {
    case T_DOT_8086:
    case T_DOT_186:
#if defined( _STANDALONE_ )
    case T_P8086:
    case T_P186:
#endif
        return( P_87 );
    case T_DOT_286:
    case T_DOT_286P:
#if defined( _STANDALONE_ )
    case T_P286:
    case T_P286P:
#endif
        return( P_287 );
    case T_DOT_386:
    case T_DOT_386P:
    case T_DOT_486:
    case T_DOT_486P:
    case T_DOT_586:
    case T_DOT_586P:
    case T_DOT_686:
    case T_DOT_686P:
#if defined( _STANDALONE_ )
    case T_P386:
    case T_P386P:
    case T_P486:
    case T_P486P:
    case T_P586:
    case T_P586P:
    case T_P686:
    case T_P686P:
#endif
        return( P_387 );
    default:
        return( 0 );
    }
}

#if defined( _STANDALONE_ )
static void MakeCPUConstant( asm_token tok )
/******************************************/
{
    char    buffer[MAX_KEYWORD_LEN + 1];

    GetInsString( tok, buffer );
    MakeConstantUnderscored( buffer );

    switch( tok ) {
    // fall right through
    case T_DOT_686P:
    case T_P686P:
    case T_DOT_686:
    case T_P686:
        MakeConstantUnderscored( "686" );
    case T_DOT_586P:
    case T_P586P:
    case T_DOT_586:
    case T_P586:
        MakeConstantUnderscored( "586" );
    case T_DOT_486P:
    case T_P486P:
    case T_DOT_486:
    case T_P486:
        MakeConstantUnderscored( "486" );
    case T_DOT_386P:
    case T_P386P:
    case T_DOT_386:
    case T_P386:
        MakeConstantUnderscored( "386" );
        break;
    case T_DOT_286P:
    case T_P286P:
    case T_DOT_286:
    case T_P286:
        MakeConstantUnderscored( "286" );
        break;
    }
    return;
}
#endif

bool cpu_directive( asm_token token )
/***********************************/
{
    asm_cpu     temp;

    if( (temp = comp_opt( token )) != P_EMPTY ) {
        if( token == T_DOT_NO87 ) {
            Code->info.cpu &= ~P_FPU_MASK;                 // turn off FPU bits
        } else if( temp & P_EXT_MASK ) {
            Code->info.cpu |= temp & P_EXT_MASK;           // turn on desired bit(s)
        } else if( temp & P_FPU_MASK ) {
            Code->info.cpu &= ~P_FPU_MASK;
            Code->info.cpu |= temp & P_FPU_MASK;           // setup FPU bits
        } else {
            Code->info.cpu &= ~( P_CPU_MASK | P_PM );
            Code->info.cpu |= temp & ( P_CPU_MASK | P_PM );// setup CPU bits
            Code->info.cpu &= ~P_FPU_MASK;
            Code->info.cpu |= def_fpu( token ) & P_FPU_MASK;   // setup FPU bits
        }
    } else {
        AsmError( UNKNOWN_DIRECTIVE );
        return( RC_ERROR );
    }

#if defined( _STANDALONE_ )
    MakeCPUConstant( token );
    switch( token ) {
    case T_DOT_686P:
    case T_P686P:
    case T_DOT_686:
    case T_P686:
    case T_DOT_586P:
    case T_P586P:
    case T_DOT_586:
    case T_P586:
    case T_DOT_486P:
    case T_P486P:
    case T_DOT_486:
    case T_P486:
    case T_DOT_386P:
    case T_P386P:
    case T_DOT_386:
    case T_P386:
        SetUse32Def( true );
        break;
    case T_DOT_286P:
    case T_P286P:
    case T_DOT_286C:
    case T_P286N:
    case T_DOT_286:
    case T_P286:
    case T_DOT_186:
    case T_P186:
    case T_DOT_8086:
    case T_P8086:
        SetUse32Def( false );
        break;
    default:
        // set FPU
        break;
    }
#endif
    return( RC_OK );
}

static bool idata_float( long value )
/***********************************/
/*
  check the correct operand/data size for float immediate operand;
*/
{
    switch( Code->mem_type ) {
    case MT_EMPTY:
        if( Code->info.token == T_PUSHW ) { // sigh. another special case
            // expect 32-bit code but get 16-bit
            AsmError( IMMEDIATE_DATA_OUT_OF_RANGE );
            return( RC_ERROR );
        }
        break;
    case MT_FAR:
    case MT_NEAR:
    case MT_SHORT:
#if defined( _STANDALONE_ )
    case MT_PROC:
#endif
        AsmError( SYNTAX_ERROR );
        return( RC_ERROR );
#if defined( _STANDALONE_ )
    case MT_SBYTE:
    case MT_SWORD:
#endif
    case MT_BYTE:
    case MT_WORD:
        AsmError( OPERANDS_MUST_BE_THE_SAME_SIZE );
        return( RC_ERROR );
#if defined( _STANDALONE_ )
    case MT_SDWORD:
#endif
    case MT_DWORD:
        // set w-bit
        Code->info.opcode |= W_BIT;
        break;
    default:
        break;
    }
    SET_OPSIZ_32( Code );
    Code->info.opnd_type[Opnd_Count] = OP_I32;
    Code->data[Opnd_Count] = value;
    return( RC_OK );
}

static unsigned char get_sr_rm_byte( prefix_reg seg_prefix )
/***************************************************************/
{
    switch( seg_prefix ) {
    case PREFIX_ES:
        return( 0 );
    case PREFIX_CS:
        return( 1 );
    case PREFIX_SS:
        return( 2 );
    case PREFIX_DS:
        return( 3 );
    case PREFIX_FS:
        return( 4 );
    case PREFIX_GS:
        return( 5 );
    default:
        break;
    }
#if defined( _STANDALONE_ )
    /**/myassert( 0 );
#endif
    return( 0 );
}

#if defined( _STANDALONE_ )

static bool proc_check( const char *curline, bool *prolog )
/*********************************************************/
/* Check if we are inside a procedure and write prologue statements if the
   current line is the first instruction line following the procedure
   declaration */
{
    *prolog = false;
    if( ( CurrProc == NULL ) || ( Token_Count == 0 ) || !DefineProc )
        return( RC_OK );

    if( AsmBuffer[0].class == TC_DIRECTIVE )
        return( RC_OK );

    if( Token_Count > 1 ) {
        if( ( AsmBuffer[1].class == TC_DIRECTIVE )
            || ( AsmBuffer[1].class == TC_DIRECT_EXPR ) ) {
            return( RC_OK );
        }
    }

    if( WritePrologue( curline ) )
        return( RC_ERROR );
    DefineProc = false;
    *prolog = true;
    return( RC_OK );
}

char *regs[4] = { "ax",  "dx",  "bx",  "cx" };

static bool get_register_argument( token_idx index, char *buffer, int *register_count, bool *on_stack )
{
    token_idx   i;
    int         size, j;
    char        ch;

    i = index;
    j = *register_count;
    if( j > 3 ) {
        *on_stack = true;
    } else {
        if( Use32 ) {
            size = 4;
        } else {
            size = 2;
        }
        if( AsmBuffer[i].class == TC_OP_SQ_BRACKET ) {
            i++;
            if( AsmBuffer[i].class == TC_RES_ID ) {
                switch( AsmBuffer[i].u.token ) {
                case T_BYTE:
                    size = 1;
                    break;
                case T_WORD:
                    size = 2;
                    break;
                case T_DWORD:
                    size = 4;
                    break;
                case T_FWORD:
                    if( Use32 ) {
                        size = 6;
                        break;
                    }
                case T_QWORD:
                    if( Use32 ) {
                        size = 8;
                        break;
                    }
                default:
                    AsmError( STRANGE_PARM_TYPE );
                    return( RC_ERROR );
                }
                i++;
            }
            if( ( AsmBuffer[i].class != TC_ID ) && ( AsmBuffer[i+1].class != TC_CL_SQ_BRACKET ) ) {
                AsmError( SYNTAX_ERROR );
                return( RC_ERROR );
            }
            if( Use32 ) {
                switch( size ) {
                case 1:
                    sprintf( buffer, "movzx e%s,[byte %s]", regs[j], AsmBuffer[i].string_ptr );
                    break;
                case 2:
                    sprintf( buffer, "movzx e%s,[word %s]", regs[j], AsmBuffer[i].string_ptr );
                    break;
                case 4:
                    sprintf( buffer, "mov e%s,[dword %s]", regs[j], AsmBuffer[i].string_ptr );
                    break;
                case 6:
                    if( j > 2 ) {
                        *on_stack = true;
                        return( RC_OK );
                    }
                    sprintf( buffer, "mov e%s,[dword %s]", regs[j], AsmBuffer[i].string_ptr );
                    InputQueueLine( buffer );
                    j++;
                    sprintf( buffer, "movzx e%s,[word %s+4]", regs[j], AsmBuffer[i].string_ptr );
                    *register_count = j;
                    break;
                case 8:
                    if( j > 2 ) {
                        *on_stack = true;
                        return( RC_OK );
                    }
                    sprintf( buffer, "mov e%s,[dword %s]", regs[j], AsmBuffer[i].string_ptr );
                    InputQueueLine( buffer );
                    j++;
                    sprintf( buffer, "movzx e%s,[dword %s+4]", regs[j], AsmBuffer[i].string_ptr );
                    *register_count = j;
                    break;
                default:
                    AsmError( SYNTAX_ERROR );
                    return( RC_ERROR );
                }
            } else {
                switch( size ) {
                case 1:
                    ch = regs[j][0];
                    sprintf( buffer, "xor %ch,%ch", ch, ch );
                    InputQueueLine( buffer );
                    sprintf( buffer, "mov %cl,[byte %s]", ch, AsmBuffer[i].string_ptr );
                    break;
                case 2:
                    sprintf( buffer, "mov %s,[word %s]", regs[j], AsmBuffer[i].string_ptr );
                    break;
                case 4:
                    if( j > 2 ) {
                        *on_stack = true;
                        return( RC_OK );
                    }
                    sprintf( buffer, "mov %s,[word %s]", regs[j], AsmBuffer[i].string_ptr );
                    InputQueueLine( buffer );
                    j++;
                    sprintf( buffer, "movzx %s,[word %s+2]", regs[j], AsmBuffer[i].string_ptr );
                    *register_count = j;
                    break;
                default:
                    AsmError( SYNTAX_ERROR );
                    return( RC_ERROR );
                }
            }
        } else if( ( AsmBuffer[i].class == TC_REG ) &&
                   ( ( AsmBuffer[i+1].class == TC_COMMA ) ||
                     ( AsmBuffer[i+1].class == TC_FINAL ) ) ) {
            switch( AsmBuffer[i].u.token ) {
            case T_EAX:
            case T_EBX:
            case T_ECX:
            case T_EDX:
            case T_EDI:
            case T_ESI:
            case T_EBP:
            case T_ESP:
                if( Use32 )
                    size = 4;
                break;
            case T_AX:
            case T_BX:
            case T_CX:
            case T_DX:
            case T_DI:
            case T_SI:
            case T_BP:
            case T_SP:
                size = 2;
                break;
            case T_DS:
            case T_CS:
            case T_ES:
            case T_SS:
            case T_FS:
            case T_GS:
                if( Use32 )
                    size = 4;
                else
                    size = 2;
                break;
            case T_AL:
            case T_AH:
            case T_BL:
            case T_BH:
            case T_CL:
            case T_CH:
            case T_DL:
            case T_DH:
                size = 1;
                break;
            }
            switch( size ) {
            case 1:
                if( Use32 ) {
                    sprintf( buffer, "movzx e%s,%s", regs[j], AsmBuffer[i].string_ptr );
                } else {
                    ch = regs[j][0];
                    sprintf( buffer, "mov %cl,%s", ch, AsmBuffer[i].string_ptr );
                }
                break;
            case 2:
                if( Use32 ) {
                    sprintf( buffer, "movzx e%s,%s", regs[j], AsmBuffer[i].string_ptr );
                } else {
                    sprintf( buffer, "mov %s,%s", regs[j], AsmBuffer[i].string_ptr );
                }
                break;
            case 4:
                if( Use32 ) {
                    sprintf( buffer, "mov e%s,%s", regs[j], AsmBuffer[i].string_ptr );
                    break;
                }
            default:
                AsmError( SYNTAX_ERROR );
                return( RC_ERROR );
            }
        } else if( AsmBuffer[i].class == TC_QUESTION_MARK ) {
            return( RC_OK );
        } else {
            if( Use32 )
                sprintf( buffer, "mov e%s,", regs[j] );
            else
                sprintf( buffer, "mov %s,", regs[j] );
            while( ( AsmBuffer[i].class != TC_FINAL ) &&
                   ( AsmBuffer[i].class != TC_COMMA ) ) {
                strcat( buffer, AsmBuffer[i++].string_ptr );
            }
        }
        InputQueueLine( buffer );
    }
    return( RC_OK );
}

static bool get_stack_argument( token_idx idx, char *buffer )
{
    int         size;

    if( Use32 ) {
        size = 4;
    } else {
        size = 2;
    }
    if( AsmBuffer[idx].class == TC_OP_SQ_BRACKET ) {
        idx++;
        if( AsmBuffer[idx].class == TC_RES_ID ) {
            switch( AsmBuffer[idx].u.token ) {
            case T_BYTE:
                size = 1;
                break;
            case T_WORD:
                size = 2;
                break;
            case T_DWORD:
                size = 4;
                break;
            case T_FWORD:
            case T_QWORD:
                if( Use32 ) {
                    if( AsmBuffer[idx].u.token == T_FWORD ) {
                        size = 6;
                    } else {
                        size = 8;
                    }
                    break;
                }
                /* fall through */
            default:
                AsmError( STRANGE_PARM_TYPE );
                return( RC_ERROR );
            }
            idx++;
        }
        if( ( AsmBuffer[idx].class != TC_ID ) || ( AsmBuffer[idx + 1].class != TC_CL_SQ_BRACKET ) ) {
            AsmError( SYNTAX_ERROR );
            return( RC_ERROR );
        }
        if( Use32 ) {
            switch( size ) {
            case 1:
                sprintf( buffer, "movzx eax,[byte %s]", AsmBuffer[idx].string_ptr );
                InputQueueLine( buffer );
                sprintf( buffer, "push eax" );
                break;
            case 2:
                sprintf( buffer, "movzx eax,[word %s]", AsmBuffer[idx].string_ptr );
                InputQueueLine( buffer );
                sprintf( buffer, "push eax" );
                break;
            case 4:
                sprintf( buffer, "push [dword %s]", AsmBuffer[idx].string_ptr );
                break;
            case 6:
                sprintf( buffer, "movzx eax,[word %s+4]", AsmBuffer[idx].string_ptr );
                InputQueueLine( buffer );
                sprintf( buffer, "push eax" );
                InputQueueLine( buffer );
                sprintf( buffer, "push [dword %s]", AsmBuffer[idx].string_ptr );
                break;
            case 8:
                sprintf( buffer, "push [dword %s+4]", AsmBuffer[idx].string_ptr );
                InputQueueLine( buffer );
                sprintf( buffer, "push [dword %s]", AsmBuffer[idx].string_ptr );
                break;
            }
        } else {
            switch( size ) {
            case 1:
                sprintf( buffer, "xor ah,ah" );
                InputQueueLine( buffer );
                sprintf( buffer, "mov al,[%s]", AsmBuffer[idx].string_ptr );
                InputQueueLine( buffer );
                sprintf( buffer, "push ax" );
                break;
            case 2:
                sprintf( buffer, "push [%s]", AsmBuffer[idx].string_ptr );
                break;
            case 4:
                sprintf( buffer, "push [word %s+2]", AsmBuffer[idx].string_ptr );
                InputQueueLine( buffer );
                sprintf( buffer, "push [word %s]", AsmBuffer[idx].string_ptr );
                break;
            }
        }
    } else {
        sprintf( buffer, "push " );
        while( ( AsmBuffer[idx].class != TC_FINAL ) && ( AsmBuffer[idx].class != TC_COMMA ) ) {
            strcat( buffer, AsmBuffer[idx++].string_ptr );
        }
    }
    InputQueueLine( buffer );
    return( RC_OK );
}

static bool expand_call( token_idx index, int lang_type )
{
    token_idx   i, j;
    token_idx   arglist[16];
    int         k;
    int         argcount, cleanup, reversed, register_count, register_arguments;
    bool        parameter_on_stack;
    char        buffer[MAX_LINE_LEN];

    argcount = cleanup = reversed = register_count = register_arguments = 0;
    parameter_on_stack = true;
    switch( lang_type ) {
    case LANG_C:
    case LANG_SYSCALL:
        cleanup++;
        reversed++;
        break;
    case LANG_WATCOM_C:
        if( Options.watcom_parms_passed_by_regs || !Use32 ) {
            parameter_on_stack = false;
        } else {
            cleanup++;
        }
        /* fall into T_STDCALL */
    case LANG_STDCALL:
        reversed++;
        break;
    case LANG_PASCAL:
    case LANG_FORTRAN:
    case LANG_BASIC:
        break;
    case LANG_NONE:
        if( AsmBuffer[index].class == TC_FINAL )
            break;
        AsmError( SYNTAX_ERROR );
        return( RC_ERROR );
    }
    for( i = index; AsmBuffer[i].class != TC_FINAL; ) {
        if( ( AsmBuffer[i].class != TC_COMMA ) ||
            ( AsmBuffer[i + 1].class == TC_FINAL ) ) {
            AsmError( SYNTAX_ERROR );
            return( RC_ERROR );
        }
        if( argcount == 16 ) {
            AsmError( TOO_MANY_ARGS );
            return( RC_ERROR );
        }
        for( j = ++i; ; j++ ) {
            if( ( AsmBuffer[j].class == TC_FINAL ) ||
                ( AsmBuffer[j].class == TC_COMMA ) ) {
                break;
            }
        }
        arglist[argcount++] = i;
        i = j;
    }
    if( !parameter_on_stack ) {
        for( k = 0; k < argcount; k++ ) {
            if( get_register_argument( arglist[k], buffer, &register_count, &parameter_on_stack ) )
                return( RC_ERROR );
            if( parameter_on_stack )
                break;
            register_count++;
            register_arguments++;
        }
    }
    /* put parameters on top of stack */
    if( reversed ) {    /* Reversed order (right to left)*/
        k = argcount;
        while( k > register_arguments ) {
            j = arglist[--k];
            if( get_stack_argument( j, buffer ) ) {
                return( RC_ERROR );
            }
        }
    } else {
        for( k = 0; k < argcount; k++ ) {
            j = arglist[k];
            if( get_stack_argument( j, buffer ) ) {
                return( RC_ERROR );
            }
        }
    }
    *buffer = 0;
    /* add original line up to before language */
    for( i = 0; i < index - 1; i++ ) {
        sprintf( buffer + strlen( buffer ), "%s ", AsmBuffer[i].string_ptr );
    }
    InputQueueLine( buffer );
    /* add cleanup after call */
    if( cleanup && argcount ) {
        if( Code->use32 )
            sprintf( buffer, "add esp,%d", argcount << 2 );
        else
            sprintf( buffer, "add sp,%d", argcount << 1 );
        InputQueueLine( buffer );
    }
    return( RC_OK );
}

#endif

static bool process_jumps( expr_list *opndx, int *jmp_flags )
/**********************************************************/
/*
  parse the jumps instructions operands
*/
{
    segm_override_jumps( opndx );

    if( ptr_operator( opndx->mem_type, opndx->explicit ) )
        return( RC_ERROR );
    if( ptr_operator( MT_PTR, opndx->explicit ) ) {
        return( RC_ERROR );
    }
    if( opndx->mbr != NULL ) {
        if( ptr_operator( opndx->mbr->mem_type, false ) )
            return( RC_ERROR );
        if( ptr_operator( MT_PTR, false ) ) {
            return( RC_ERROR );
        }
    }
    return( jmp( opndx, jmp_flags ) );
}

static bool segm_override_jumps( expr_list *opndx )
/*************************************************/
{
    if( opndx->override != INVALID_IDX ) {
        if( AsmBuffer[opndx->override].class == TC_REG ) {
            Code->prefix.seg = AsmOpTable[AsmOpcode[AsmBuffer[opndx->override].u.token].position].opcode;
        } else {
#if defined( _STANDALONE_ )
            if( FixOverride( opndx->override ) ) {
                return( RC_ERROR );
            }
#endif
        }
    }
    return( RC_OK );
}

static bool segm_override_idata( expr_list *opndx )
/*************************************************/
{
    if( opndx->override != INVALID_IDX ) {
        if( AsmBuffer[opndx->override].class == TC_REG ) {
            Code->prefix.seg = AsmOpTable[AsmOpcode[AsmBuffer[opndx->override].u.token].position].opcode;
        } else {
#if defined( _STANDALONE_ )
            if( FixOverride( opndx->override ) ) {
                return( RC_ERROR );
            }
#endif
        }
    }
    return( RC_OK );
}

static bool segm_override_memory( expr_list *opndx )
/**************************************************/
{
    if( opndx->override != INVALID_IDX ) {
        if( AsmBuffer[opndx->override].class == TC_REG ) {
            Code->prefix.seg = AsmOpTable[AsmOpcode[AsmBuffer[opndx->override].u.token].position].opcode;
        } else {
#if defined( _STANDALONE_ )
            if( FixOverride( opndx->override ) ) {
                return( RC_ERROR );
            }
#endif
        }
    }
    return( RC_OK );
}

static bool idata_nofixup( expr_list *opndx )
/*******************************************/
{
    OPNDTYPE    op_type = OP_NONE;
    long        value;

    value = opndx->value;
    Code->data[Opnd_Count] = value;

    switch( Code->mem_type ) {
    case MT_EMPTY:
        if( Code->info.token == T_PUSH ) {
            if( opndx->explicit ) {
                if( opndx->mem_type == MT_BYTE ) {
                    op_type = OP_I8;
                } else if( opndx->mem_type == MT_WORD ) {
                    op_type = OP_I16;
                    SET_OPSIZ_16( Code );
                } else if( opndx->mem_type == MT_DWORD ) {
                    op_type = OP_I32;
                    SET_OPSIZ_32( Code );
                } else {
                    // FIXME !!
                    op_type = OP_I32;
                    SET_OPSIZ_32( Code );
                }
                break;
            }
            if( Code->use32 ) {
                if( (int_8)value == (int_32)value ) {
                    op_type = OP_I8;
                } else {
                    op_type = OP_I32;
                }
            } else if( (unsigned long)value > USHRT_MAX ) {
                SET_OPSIZ_32( Code );
                if( (int_8)value == (int_32)value ) {
                    op_type = OP_I8;
                } else {
                    op_type = OP_I32;
                }
            } else {
                if( (int_8)value == (int_16)value ) {
                    op_type = OP_I8;
                } else {
                    op_type = OP_I16;
                }
            }
            break;
        } else if( Code->info.token == T_PUSHW ) {
            op_type = OP_I16;
            if( (int_8)value == (int_16)value ) {
                op_type = OP_I8;
            }
            break;
        } else if( Code->info.token == T_PUSHD ) {
            op_type = OP_I32;
            if( (int_8)value == (int_32)value ) {
                op_type = OP_I8;
            }
            break;
        }
        /* fall through */
        if( ( value > SHRT_MAX ) || ( value < SHRT_MIN ) ) {
            op_type = OP_I32;
        } else if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
            op_type = OP_I16;
        } else {
            op_type = OP_I8;
        }
        break;
    case MT_FAR:
        if( ( value > SHRT_MAX ) || ( value < SHRT_MIN ) ) {
            op_type = OP_I32;
        } else if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
            op_type = OP_I16;
        } else {
            op_type = OP_I8;
        }
        break;
    case MT_NEAR:
        if( !Code->use32 ) {
            op_type = OP_I16;
        } else {
            op_type = OP_I32;
        }
        break;
    case MT_SHORT:
        if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
            // expect 8-bit but got 16 bit
            AsmError( JUMP_OUT_OF_RANGE );
            return( RC_ERROR );
        }
        op_type = OP_I8;
        break;
    case MT_BYTE:
        if( !InRange( value, 1 ) ) {
            // expect 8-bit but got 16 bit
            AsmError( IMMEDIATE_DATA_OUT_OF_RANGE );
            return( RC_ERROR );
        }
        op_type = OP_I8;
        break;
#if defined( _STANDALONE_ )
    case MT_SBYTE:
        if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
            AsmError( IMMEDIATE_DATA_OUT_OF_RANGE );
            return( RC_ERROR );
        }
        op_type = OP_I8;
        break;
    case MT_SWORD:
        if( ( value > SHRT_MAX ) || ( value < SHRT_MIN ) ) {
            AsmError( IMMEDIATE_DATA_OUT_OF_RANGE );
            return( RC_ERROR );
        }
        if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
            op_type = OP_I16;
        } else {
            op_type = OP_I8;
        }
        // set w-bit
        Code->info.opcode |= W_BIT;
        break;
#endif
    case MT_WORD:
#if defined( _STANDALONE_ )
        if( Options.sign_value ) {
            if( !InRange( value, 2 ) ) {
                AsmError( IMMEDIATE_DATA_OUT_OF_RANGE );
                return( RC_ERROR );
            }
            if( value > UCHAR_MAX ) {
                op_type = OP_I16;
            } else {
                op_type = OP_I8;
            }
            // set w-bit
            Code->info.opcode |= W_BIT;
        } else {
#endif
            if( !InRange( value, 2 ) ) {
                AsmError( IMMEDIATE_DATA_OUT_OF_RANGE );
                return( RC_ERROR );
            }
            if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
                op_type = OP_I16;
            } else {
                op_type = OP_I8;
            }
            // set w-bit
            Code->info.opcode |= W_BIT;
#if defined( _STANDALONE_ )
        }
#endif
        break;
#if defined( _STANDALONE_ )
   case MT_SDWORD:
        if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
            op_type = OP_I32;
        } else {
            op_type = OP_I8;
        }
        // set w-bit
        Code->info.opcode |= W_BIT;
        break;
#endif
    case MT_DWORD:
#if defined( _STANDALONE_ )
        if( Options.sign_value ) {
            if( value > UCHAR_MAX ) {
                op_type = OP_I32;
            } else {
                op_type = OP_I8;
            }
            // set w-bit
            Code->info.opcode |= W_BIT;
        } else {
#endif
            if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
                op_type = OP_I32;
            } else {
                op_type = OP_I8;
            }
            // set w-bit
            Code->info.opcode |= W_BIT;
#if defined( _STANDALONE_ )
        }
#endif
        break;
    case MT_QWORD:
#if defined( _STANDALONE_ )
        if( Options.sign_value ) {
            if( value > UCHAR_MAX ) {
                op_type = OP_I32;
            } else {
                op_type = OP_I8;
            }
            // set w-bit
            Code->info.opcode |= W_BIT;
        } else {
#endif
            if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
                op_type = OP_I32;
            } else {
                op_type = OP_I8;
            }
            // set w-bit
            Code->info.opcode |= W_BIT;
#if defined( _STANDALONE_ )
        }
#endif
        break;
    default:
        break;
    }
    Code->info.opnd_type[Opnd_Count] = op_type;
    return( RC_OK );
}

static bool idata_fixup( expr_list *opndx )
/*****************************************/
{
    struct asmfixup     *fixup;
    enum fixup_types    fixup_type;
    int                 type;
    bool                sym32;

    Code->data[Opnd_Count] = opndx->value;
    segm_override_idata( opndx );

#if defined( _STANDALONE_ )
    if( ( opndx->sym->state == SYM_SEG )
        || ( opndx->sym->state == SYM_GRP )
        || ( opndx->instr == T_SEG ) ) {
        sym32 = false;
    } else if( opndx->abs ) {
        sym32 = false;
    } else {
        sym32 = SymIs32( opndx->sym );
    }
#else
    sym32 = Code->use32;
#endif
    if( opndx->instr != T_NULL ) {
        if( ( opndx->base_reg != INVALID_IDX )
            || ( opndx->idx_reg != INVALID_IDX ) ) {
            AsmError( INVALID_MEMORY_POINTER );
            return( RC_ERROR );
        }
    }
    if( opndx->instr == T_SEG ) {
        if( opndx->sym->state == SYM_STACK ) {
            AsmError( CANNOT_SEG_AUTO );
            return( RC_ERROR );
        }
    } else {
        if( MEM_TYPE( Code->mem_type, BYTE ) ) {
            AsmError( OFFSET_TOO_SMALL );
            return( RC_ERROR );
        }
        if( opndx->sym->state == SYM_STACK ) {
            AsmError( CANNOT_OFFSET_AUTO );
            return( RC_ERROR );
#if defined( _STANDALONE_ )
        } else if( opndx->sym->state == SYM_GRP ) {
            AsmError( CANNOT_OFFSET_GRP );
            return( RC_ERROR );
#endif
        }
    }
    switch( Code->mem_type ) {
    case MT_EMPTY:
        if( Opnd_Count > OPND1 ) {
            type = OperandSize( Code->info.opnd_type[OPND1] );
            if( type == 4 ) {
                Code->mem_type = MT_DWORD;
                Code->info.opnd_type[Opnd_Count] = OP_I32;
                SET_OPSIZ_32( Code );
                break;
            } else if( type == 2 ) {
                Code->mem_type = MT_WORD;
                Code->info.opnd_type[Opnd_Count] = OP_I16;
                SET_OPSIZ_16( Code );
                break;
            }
        }
        if( opndx->abs ) {
            if( opndx->mem_type == MT_BYTE ) {
                Code->mem_type = MT_BYTE;
                Code->info.opnd_type[Opnd_Count] = OP_I8;
                break;
            } else if( opndx->mem_type == MT_EMPTY ) {
                SET_OPSIZ_OFF( Code );
                if( IS_OPSIZ_32( Code ) ) {
                    Code->mem_type = MT_DWORD;
                    Code->info.opnd_type[Opnd_Count] = OP_I32;
                    sym32 = true;
                    break;
                }
            } else if( opndx->mem_type == MT_DWORD ) {
                sym32 = true;
            }
        }
        if( ( Code->info.token == T_PUSHD ) || sym32 ) {
            Code->mem_type = MT_DWORD;
            Code->info.opnd_type[Opnd_Count] = OP_I32;
            SET_OPSIZ_32( Code );
            break;
        }
        Code->mem_type = MT_WORD;
        // no break
#if defined( _STANDALONE_ )
    case MT_SWORD:
#endif
    case MT_WORD:
        Code->info.opnd_type[Opnd_Count] = OP_I16;
        SET_OPSIZ_16( Code );
        break;
#if defined( _STANDALONE_ )
    case MT_SDWORD:
#endif
    case MT_DWORD:
        Code->info.opnd_type[Opnd_Count] = OP_I32;
        SET_OPSIZ_32( Code );
        break;
    default:
        break;
    }
    if( opndx->instr == T_SEG ) {
        fixup_type = FIX_SEG;
    } else {
        if( Code->mem_type == MT_BYTE ) {
            fixup_type = FIX_LOBYTE;
        } else if( IS_OPSIZ_32( Code ) ) {
            fixup_type = ( sym32 ) ? FIX_OFF32 : FIX_OFF16;
        } else {
            if( sym32 ) {
                // fixme !!!! warning
                // operand size is 16bit
                // but fixup is 32-bit
            }
            fixup_type = FIX_OFF16;
        }
    }
    ConstantOnly = true;
    Code->info.opcode |= W_BIT;

#if defined( _STANDALONE_ )
    find_frame( opndx->sym );
#endif

    fixup = AddFixup( opndx->sym, fixup_type, OPTJ_NONE );
//    add_frame();   // ???
    if( fixup == NULL ) {
        return( RC_ERROR );
    }
    return( RC_OK );
}

static bool idata_operand( expr_list *opndx )
/*******************************************/
{
    if( opndx->sym == NULL ) {
        return( idata_nofixup( opndx ) );
    } else {
        return( idata_fixup( opndx ) );
    }
}

static bool memory_operand( expr_list *opndx )
/********************************************/
{
    unsigned char       ss = SCALE_FACTOR_1;
    asm_token           index = T_NULL;
    asm_token           base = T_NULL;
    struct asm_sym      *sym;
    bool                base_lock = false;
    enum fixup_types    fixup_type;
#if defined( _STANDALONE_ )
    bool                sym32;
#endif

    Code->data[Opnd_Count] = opndx->value;
    Code->info.opnd_type[Opnd_Count] = OP_M;

    segm_override_memory( opndx );

    if( ptr_operator( opndx->mem_type, opndx->explicit ) )
        return( RC_ERROR );
    if( ptr_operator( MT_PTR, opndx->explicit ) ) {
        return( RC_ERROR );
    }
    if( opndx->mbr != NULL ) {
        if( ptr_operator( opndx->mbr->mem_type, false ) )
            return( RC_ERROR );
        if( ptr_operator( MT_PTR, false ) ) {
            return( RC_ERROR );
        }
    }
    if( opndx->base_reg != INVALID_IDX ) {
        base = AsmBuffer[opndx->base_reg].u.token;
        switch( base ) {     // check for base registers
        case T_EAX:
        case T_EBX:
        case T_ECX:
        case T_EDX:
        case T_ESP:
        case T_EBP:
        case T_ESI:
        case T_EDI:
            if( ( Code->info.cpu & ( P_CPU_MASK | P_PM ) ) <= P_286p ) {
                // 286 and down cannot use 386 registers
                AsmError( CANNOT_USE_386_ADDRESSING_MODE_WITH_CURRENT_CPU_SETTING );
                return( RC_ERROR );
            }
            SET_ADRSIZ_32( Code );
            break;
        case T_BX:
        case T_BP:
        case T_SI:
        case T_DI:
            SET_ADRSIZ_16( Code );
            break;
        default:
            AsmError( INVALID_MEMORY_POINTER );
            return( RC_ERROR );
        }
    }
    if( opndx->idx_reg != INVALID_IDX ) {
        index = AsmBuffer[opndx->idx_reg].u.token;
        switch( index ) {     // check for index registers
        case T_EAX:
        case T_EBX:
        case T_ECX:
        case T_EDX:
        case T_ESP:
        case T_EBP:
        case T_ESI:
        case T_EDI:
            if( ( Code->info.cpu & ( P_CPU_MASK | P_PM ) ) <= P_286p ) {
                // 286 and down cannot use 386 registers
                AsmError( CANNOT_USE_386_ADDRESSING_MODE_WITH_CURRENT_CPU_SETTING );
                return( RC_ERROR );
            }
            SET_ADRSIZ_32( Code );
            break;
        case T_BX:
        case T_BP:
        case T_SI:
        case T_DI:
            SET_ADRSIZ_16( Code );
            break;
        default:
            AsmError( INVALID_MEMORY_POINTER );
            return( RC_ERROR );
        }
        if( AsmBuffer[opndx->idx_reg].u.token == T_ESP ) {
            if( opndx->scale == 1 ) {
                index = base;
                base = AsmBuffer[opndx->idx_reg].u.token;
            } else {
                AsmError( ESP_CANNOT_BE_USED_AS_INDEX );
                return( RC_ERROR );
            }
        }
        if( IS_ADRSIZ_32( Code ) ) {
            if( ( Code->info.cpu & P_CPU_MASK ) >= P_386 ) {
                if( !Code->use32 )
                    SET_ADRSIZ_ON( Code );
                switch( index ) {
                case T_ESP:
                case T_BX:
                case T_BP:
                case T_SI:
                case T_DI:
                    // cannot use ESP or 16-bit reg as index
                    AsmError( INVALID_INDEX_REGISTER );
                    return( RC_ERROR );
                default:
                    if( !Code->use32 )
                        SET_ADRSIZ_ON( Code );
                    switch( opndx->scale ) {
                    case 1:
                        // ss = 00
                        break;
                    case 2:
                        // ss = 01
                        ss = SCALE_FACTOR_2;
                        break;
                    case 4:
                        // ss = 10
                        ss = SCALE_FACTOR_4;
                        break;
                    case 8:
                        // ss = 11
                        ss = SCALE_FACTOR_8;
                        break;
                    default: // must be * 1, 2, 4 or 8
                        AsmError( SCALE_FACTOR_MUST_BE_1_2_4_OR_8 );
                        return( RC_ERROR );
                    }
                }
            } else {
                // 286 and down cannot use this memory mode
                AsmError( INVALID_ADDRESSING_MODE_WITH_CURRENT_CPU_SETTING );
                return( RC_ERROR );
            }
        }
    }
    sym = opndx->sym;
    if( sym != NULL ) {
        switch( sym->state ) {
        case SYM_UNDEFINED:
            // forward reference
            break;
#if defined( _STANDALONE_ )
        case SYM_SEG:
        case SYM_GRP:
            // error !!!!!
            break;
#endif
        case SYM_STACK:
            if( base != T_NULL ) {
                if( base_lock ) {
                    // [reg + data][reg + data] is not allowed
                    AsmError( TOO_MANY_BASE_REGISTERS );
                    return( RC_ERROR );
                }
                index = base;
            }
            if( Code->use32 ) {
                base = T_EBP;
            } else {
                base = T_BP;
            }
            base_lock = true;   // add lock
            /* fall through */
        default:
            if( Code->mem_type == MT_EMPTY ) {
                if( ptr_operator( sym->mem_type, false ) )
                    return( RC_ERROR );
                if( ptr_operator( MT_PTR, false ) ) {
                    return( RC_ERROR );
                }
            }
            break;
        }

#if defined( _STANDALONE_ )
        if( opndx->abs ) {
            sym32 = IS_ADRSIZ_32( Code );
        } else {
            sym32 = SymIs32( sym );
        }
        if( ( opndx->base_reg == INVALID_IDX ) && ( opndx->idx_reg == INVALID_IDX ) ) {
            SET_ADRSIZ( Code, sym32 );
            fixup_type = ( sym32 ) ? FIX_OFF32 : FIX_OFF16;
        } else {
            if( IS_ADRSIZ_32( Code ) ) {
                fixup_type = ( sym32 ) ? FIX_OFF32 : FIX_OFF16;
            } else {
                if( sym32 ) {
                    // fixme !!!! warning
                    // address size is 16bit
                    // but fixup is 32-bit
                }
                fixup_type = FIX_OFF16;
            }
        }

        AddFixup( sym, fixup_type, OPTJ_NONE );

        if( Modend ) {
            GetAssume( sym, ASSUME_NOTHING );
        } else {
            if( mem2code( ss, index, base, sym ) ) {
                return( RC_ERROR );
            }
        }
        add_frame();
#else
        fixup_type = ( Code->use32 ) ? FIX_OFF32 : FIX_OFF16;

        AddFixup( sym, fixup_type, OPTJ_NONE );

        if( mem2code( ss, index, base, sym ) ) {
            return( RC_ERROR );
        }
#endif
    } else {
        if( mem2code( ss, index, base, sym ) ) {
            return( RC_ERROR );
        }
    }
    if( Code->mem_type == MT_DWORD && !Code->use32 ) {
        if( Code->info.token == T_CALL ) {
            Code->info.token = T_CALLF;
        } else if( Code->info.token == T_JMP ) {
            Code->info.token = T_JMPF;
        }
    }
    return( RC_OK );
}

static bool process_address( expr_list *opndx, int *jmp_flags )
/*************************************************************/
/*
  parse the memory reference operand
*/
{
    memtype     mem_type;

    *jmp_flags = 0;
    if( opndx->indirect ) {           // indirect operand
        return( memory_operand( opndx ) );
    } else {                          // direct operand
        if( opndx->instr != T_NULL ) { // OFFSET ..., SEG ...
            if( IS_ANY_BRANCH( Code->info.token ) ) {  // jumps/call processing
                return( process_jumps( opndx, jmp_flags ) );
            } else {
                return( idata_operand( opndx ) );
            }
        } else {                      // direct operand only
            if( opndx->sym == NULL ) {       // without symbol
                if( opndx->override != INVALID_IDX ) {
                    // direct absolute memory without fixup ... DS:[0]
                    return( memory_operand( opndx ) );
                } else if( IS_ANY_BRANCH( Code->info.token ) ) {  // jumps/call processing
                    return( process_jumps( opndx, jmp_flags ) );
                } else {
                    return( idata_operand( opndx ) );  // error ????
                }
            } else {                  // with symbol
                if( ( opndx->sym->state == SYM_UNDEFINED ) && !opndx->explicit ) {
#if defined( _STANDALONE_ )
                    if( Parse_Pass != PASS_1 ) {
                        AsmErr( SYMBOL_NOT_DEFINED, opndx->sym->name );
                        return( RC_ERROR );
                    }
#endif
                    // undefined symbol, it is not possible to determine
                    // operand type and size
                    if( IS_ANY_BRANCH( Code->info.token ) ) {  // jumps/call processing
                        return( process_jumps( opndx, jmp_flags ) );
                    } else {
                        switch( Code->info.token ) {
                        case T_PUSH:
                        case T_PUSHW:
                        case T_PUSHD:
                            return( idata_nofixup( opndx ) );
                            break;
                        default:
                            return( memory_operand( opndx ) );
                            break;
                        }
                    }
#if defined( _STANDALONE_ )
                } else if( ( opndx->sym->state == SYM_SEG )
                    || ( opndx->sym->state == SYM_GRP ) ) {
                    // SEGMENT and GROUP symbol is converted to SEG symbol
                    // for next prrocessing
                    opndx->instr = T_SEG;
                    if( IS_ANY_BRANCH( Code->info.token ) ) {  // jumps/call processing
                        return( process_jumps( opndx, jmp_flags ) );
                    }
                    return( idata_operand( opndx ) );
#endif
                } else {
                    // CODE location is converted to OFFSET symbol
                    mem_type = ( opndx->explicit ) ? opndx->mem_type : opndx->sym->mem_type;
#if defined( _STANDALONE_ )
                    if( opndx->abs ) {
                        if( IS_ANY_BRANCH( Code->info.token ) ) {  // jumps/call processing
                            return( process_jumps( opndx, jmp_flags ) );
                        }
                        return( idata_operand( opndx ) );
                    }
#endif
                    switch( mem_type ) {
                    case MT_FAR:
                    case MT_NEAR:
                    case MT_SHORT:
#if defined( _STANDALONE_ )
                    case MT_PROC:
#endif
                        if( Code->info.token == T_LEA ) {
                            return( memory_operand( opndx ) );
#if defined( _STANDALONE_ )
                        } else if( IS_SYM_COUNTER( opndx->sym->name ) ) {
                            if( IS_ANY_BRANCH( Code->info.token ) ) {  // jumps/call processing
                                return( process_jumps( opndx, jmp_flags ) );
                            }
                            return( idata_operand( opndx ) );
#endif
                        } else if( opndx->mbr != NULL ) { // structure or structure member
                            return( memory_operand( opndx ) );
                        } else {
                            if( IS_ANY_BRANCH( Code->info.token ) ) {  // jumps/call processing
                                return( process_jumps( opndx, jmp_flags ) );
                            }
                            return( idata_operand( opndx ) );
                        }
                        break;
                    default:
                        // direct memory with fixup
                        return( memory_operand( opndx ) );
                        break;
                    }
                }
            }
        }
    }
//    return( RC_OK );
}

static bool process_const( expr_list *opndx, int *jmp_flags )
/***********************************************************/
{
    if( IS_ANY_BRANCH( Code->info.token ) )    // jumps/call processing
        return( process_jumps( opndx, jmp_flags ) );
    if( ( Code->info.token == T_IMUL )
        && ( Code->info.opnd_type[OPND1] & OP_R ) ) {
        if( Opnd_Count == OPND2 ) {
            Code->info.rm_byte = ( Code->info.rm_byte & ~BIT_345 )
                          | ( ( Code->info.rm_byte & BIT_012 ) << 3 );
        } else if( Opnd_Count == OPND3 ) {
            Code->info.opnd_type[OPND1] = Code->info.opnd_type[OPND2];
            Code->info.opnd_type[OPND2] = OP_NONE;
            Code->data[OPND1] = Code->data[OPND2];
            Code->data[OPND2] = 0;
            InsFixups[OPND1] = InsFixups[OPND2];
            InsFixups[OPND2] = NULL;
            Opnd_Count = OPND2;
        }
    }
    return( idata_operand( opndx ) );
}

static bool process_reg( expr_list *opndx, int *jmp_flags )
/*********************************************************/
/*
- parse and encode the register operand;
*/
{
    unsigned char       st_reg;
    unsigned char       reg;
    const asm_ins       ASMFAR *ins;

    if( opndx->indirect )  // simple register indirect operand ... [EBX]
        return( process_address( opndx, jmp_flags ) );
    ins = AsmOpTable + AsmOpcode[AsmBuffer[opndx->base_reg].u.token].position;
    reg = ins->opcode;
    Code->info.opnd_type[Opnd_Count] = ins->opnd_type2;
    switch( ins->opnd_type2 ) {
    case OP_AL:
    case OP_R8:
        Code->info.opcode &= NOT_W_BIT;         // clear w-bit
        break;
    case OP_CL: /* only appears in "shift opnd,CL" instructions */
        break;
    case OP_AX:
    case OP_DX: /* only appears in "in" and "out" instructions  */
    case OP_R16:
        Code->info.opcode |= W_BIT;             // set w-bit
        if( Code->use32 )
            SET_OPSIZ_ON( Code );
        break;
    case OP_MMX:
        break;
    case OP_XMM:
        break;
    case OP_ST:
        st_reg = (unsigned char)opndx->idx_reg & BIT_012;
        Code->info.rm_byte |= st_reg;
        if( st_reg != 0 )
            Code->info.opnd_type[Opnd_Count] = OP_ST_REG;
        break;
    case OP_SR3:                        // 386 segment register
        if( ( Code->info.cpu & ( P_CPU_MASK | P_PM ) ) <= P_286p ) {
            // 8086 ins cannot use 80386 segment register
            AsmError( CANNOT_USE_386_SEGMENT_REGISTER_WITH_CURRENT_CPU_SETTING );
            return( RC_ERROR );
        }
        /* fall through */
    case OP_SR:                                 // any seg reg
    case OP_SR2:                                // 8086 segment register
        if( AsmBuffer[opndx->base_reg].u.token == T_CS ) {
            // POP CS is not allowed
            if( Code->info.token == T_POP ) {
                AsmError( POP_CS_IS_NOT_ALLOWED );
                return( RC_ERROR );
            }
        }
        reg = get_sr_rm_byte( ins->opcode );
        break;
    case OP_EAX:
    case OP_R32:
        if( ( Code->info.cpu & ( P_CPU_MASK | P_PM ) ) <= P_286p ) {
            // 8086 ins cannot use 386 register
            AsmError( CANNOT_USE_386_REGISTER_WITH_CURRENT_CPU_SETTING );
            return( RC_ERROR );
        }
        Code->info.opcode |= W_BIT;             // set w-bit
        if( !Code->use32 )
            SET_OPSIZ_ON( Code );
        break;
    case OP_TR:                 // Test registers
        switch( AsmBuffer[opndx->base_reg].u.token ) {
        case T_TR3:
        case T_TR4:
        case T_TR5:
            if( ( ( ( Code->info.cpu & P_CPU_MASK ) < P_486 )
               || ( ( Code->info.cpu & P_CPU_MASK ) >= P_686 ) )
                && ( ( ins->cpu & P_CPU_MASK ) >= P_486 ) ) {
                // TR3, TR4, TR5 are available on 486 only
                AsmError( CANNOT_USE_TR3_TR4_TR5_IN_CURRENT_CPU_SETTING );
                return( RC_ERROR );
            }
            break;
        case T_TR6:
        case T_TR7:
            if( ( ( ( Code->info.cpu & P_CPU_MASK ) < P_386 )
               || ( ( Code->info.cpu & P_CPU_MASK ) >= P_686 ) )
                && ( ( ins->cpu & P_CPU_MASK ) >= P_386 ) ) {
                // TR6, TR7 are available on 386...586 only
                AsmError( CANNOT_USE_TR3_TR4_TR5_IN_CURRENT_CPU_SETTING );
                return( RC_ERROR );
            }
            break;
        }
    case OP_CR:                 // Control registers
    case OP_DR:                 // Debug registers
        if( Code->info.token != T_MOV ) {
            AsmError( ONLY_MOV_CAN_USE_SPECIAL_REGISTER );
            return( RC_ERROR );
        }
        break;
    }
    if( Opnd_Count == OPND1 ) {
        // the first operand
        // r/m is treated as a 'reg' field
        Code->info.rm_byte |= MOD_11;
        // fill the r/m field
        Code->info.rm_byte |= reg;
    } else {
        // the second operand
        if( ( Code->info.token == T_XCHG )
            && ( ( Code->info.opnd_type[OPND1] == OP_AX )
            || ( Code->info.opnd_type[OPND1] == OP_EAX ) ) ) {
            // XCHG can use short form if op1 is AX or EAX
            Code->info.rm_byte = ( Code->info.rm_byte & BIT_67 ) | reg;
        } else {
            // fill reg field with reg
            Code->info.rm_byte = ( Code->info.rm_byte & ~BIT_345 ) | ( reg << 3 );
        }
    }
    return( RC_OK );
}

bool AsmParse( const char *curline )
/**********************************/
/*
- co-ordinate the parsing process;
- it is a basically a big loop to loop through all the tokens and identify them
  with the switch statement;
*/
{
    token_idx           i;
    bool                cur_opnd_label = false;
    bool                last_opnd_label = false;
    struct asm_code     *rCode = Code;
    expr_list           opndx;
    token_idx           n;
    operand_idx         j;
    int                 jmp_flags;
    bool                flag;
#if defined( _STANDALONE_ )
    int                 temp;
    static bool         in_epilogue = false;
#endif

#if defined( _STANDALONE_ )
    Code->use32 = Use32;
    if( proc_check( curline, &flag ) )
        return( RC_ERROR );
    if( flag )
        return( RC_OK );
#else
    curline = curline;
#endif

    //init
    rCode->info.token     = T_NULL;
    rCode->info.opcode    = 0;
    rCode->info.rm_byte   = 0;
    rCode->prefix.ins     = T_NULL;
    rCode->prefix.seg     = PREFIX_EMPTY;
    SET_ADRSIZ_OFF( rCode );
    SET_OPSIZ_OFF( rCode );
    rCode->mem_type       = MT_EMPTY;
    rCode->mem_type_fixed = false;
    rCode->extended_ins   = EMPTY;
    rCode->sib            = 0;            // assume ss is *1
    rCode->indirect       = false;
    for( j = 0; j < OPND_MAX; j++ ) {
        rCode->info.opnd_type[j] = OP_NONE;
        rCode->data[j] = 0;
        InsFixups[j] = NULL;
    }
    Opnd_Count = 0;

    // check if continue initializing array
    if( NextArrayElement( &flag ) )
        return( RC_ERROR );
    if( flag )
        return( RC_OK );

#if defined( _STANDALONE_ )
    CheckSeg = true;
    Frame = NULL;
    SegOverride = NULL;
#endif

    for( i = 0; i < Token_Count; i++ ) {
        switch( AsmBuffer[i].class ) {
        case TC_INSTR:
//            ExpandTheWorld( i, false, true );
#if defined( _STANDALONE_ )
            if( ExpandAllConsts( i, false ) )
                return( RC_ERROR );
#endif
            if( last_opnd_label ) {
                // illegal operand is put before instruction
                AsmError( SYNTAX_ERROR );
                return( RC_ERROR );
            }
            cur_opnd_label = false;
#if defined( _STANDALONE_ )
            if( ( AsmBuffer[i+1].class == TC_DIRECTIVE )
                || ( AsmBuffer[i+1].class == TC_COLON ) ) {
                // instruction name is label
                AsmBuffer[i].class = TC_ID;
                i--;
                continue;
            }
#endif
            switch( AsmBuffer[i].u.token ) {
            // prefix
            case T_LOCK:
            case T_REP:
            case T_REPE:
            case T_REPNE:
            case T_REPNZ:
            case T_REPZ:
                rCode->prefix.ins = AsmBuffer[i].u.token;
                // prefix has to be followed by an instruction
                if( AsmBuffer[i+1].class != TC_INSTR ) {
                    AsmError( PREFIX_MUST_BE_FOLLOWED_BY_AN_INSTRUCTION );
                    return( RC_ERROR );
                }
                continue;
#if defined( _STANDALONE_ )
            case T_RET:
                if( ( CurrProc != NULL ) && !in_epilogue ) {
                    in_epilogue = true;
                    return( Ret( i, Token_Count, false ) );
                }
                /* fall through */
            case T_RETN:
            case T_RETF:
                in_epilogue = false;
                rCode->info.token = AsmBuffer[i].u.token;
                break;
            case T_IRET:
            case T_IRETD:
                if( ( CurrProc != NULL ) && !in_epilogue ) {
                    in_epilogue = true;
                    return( Ret( i, Token_Count, true ) );
                }
                /* fall through */
            case T_IRETF:
            case T_IRETDF:
                in_epilogue = false;
                rCode->info.token = AsmBuffer[i].u.token;
                break;
            case T_CALL:
                if( Options.mode & MODE_IDEAL ) {
                    for( n = i + 2; n < Token_Count; n++ ) {
                        if( !CheckForLang( n, &temp ) ) {
                            return( expand_call( n + 1, temp ) );
                        }
                    }
                }
                /* fall into default */
#endif
            default:
                rCode->info.token = AsmBuffer[i].u.token;
                break;
            }
            i++;
            if( EvalOperand( &i, Token_Count, &opndx, true ) ) {
                return( RC_ERROR );
            }
            if( opndx.empty )
                break;
            switch( opndx.type ) {
            case EXPR_ADDR:
                if( process_address( &opndx, &jmp_flags ) ) 
                    return( RC_ERROR );
//                if( jmp_flags == INDIRECT_JUMP )
//                    return( RC_ERROR );
                if( jmp_flags == SCRAP_INSTRUCTION )
                    return( RC_OK );
                break;
            case EXPR_CONST:
                process_const( &opndx, &jmp_flags );
                break;
            case EXPR_REG:
                process_reg( &opndx, &jmp_flags );
                break;
            case EXPR_UNDEF:
                return( RC_ERROR );
            default:
                break;
            }
            i--;
            break;
        case TC_RES_ID:
            if( rCode->info.token == T_NULL ) {
                n = ( i == 0 ) ? INVALID_IDX : 0;
                return( data_init( n, i ) );
            }
            AsmError( SYNTAX_ERROR );
            return( RC_ERROR );
        case TC_DIRECTIVE:
            return( directive( i, AsmBuffer[i].u.token ) );
#if defined( _STANDALONE_ )
        case TC_DIRECT_EXPR:
            if( Parse_Pass != PASS_1 ) {
                Modend = true;
                n = i + 1;
                if( EvalOperand( &n, Token_Count, &opndx, true ) ) {
                    return( RC_ERROR );
                }
                if( !opndx.empty && ( opndx.type == EXPR_ADDR ) ) {
                    process_address( &opndx, &jmp_flags );
                }
            }
            return( directive( i, AsmBuffer[i].u.token ) );
            break;
#endif
        case TC_ID:
#if defined( _STANDALONE_ )
            if( !( ( AsmBuffer[i+1].class == TC_DIRECTIVE )
                && ( ( AsmBuffer[i+1].u.token == T_EQU )
                || ( AsmBuffer[i+1].u.token == T_EQU2 )
                || ( AsmBuffer[i+1].u.token == T_TEXTEQU ) ) ) ) {
                bool expanded;
                if( ExpandSymbol( i, false, &expanded ) )
                    return( RC_ERROR );
                if( expanded ) {
                    // restart token processing
                    i--;
                    continue;
                }
            }
#endif
#if 0
            if( last_opnd_label ) {
                AsmError( SYNTAX_ERROR );
                return( RC_ERROR );
            }
#endif
            if( i == 0 ) {   // a new label
#if ALLOW_STRUCT_INIT
#if defined( _STANDALONE_ )
                if( IsLabelStruct( AsmBuffer[i].string_ptr )
                    && ( AsmBuffer[i+1].class != TC_DIRECTIVE ) ) {
                    AsmBuffer[i].class = TC_DIRECTIVE;
                    AsmBuffer[i].u.token = T_STRUCT;
                    return( data_init( INVALID_IDX, 0 ) );
                }
#endif
#endif

                switch( AsmBuffer[i+1].class ) {
                case TC_COLON:
                    cur_opnd_label = true;
                    break;
#if ALLOW_STRUCT_INIT
#if defined( _STANDALONE_ )
                case TC_ID:
                    /* structure declaration */
                    if( IsLabelStruct( AsmBuffer[i+1].string_ptr ) ) {
                        AsmBuffer[i+1].class = TC_DIRECTIVE;
                        AsmBuffer[i+1].u.token = T_STRUCT;
                    } else {
                        AsmError( SYNTAX_ERROR );
                        return( RC_ERROR );
                    }
                    /* fall through */
#endif
#endif
                case TC_RES_ID:
                    return( data_init( i, i+1 ) );
                    break;
#if defined( _STANDALONE_ )
                case TC_DIRECTIVE:
                    return( directive( i+1, AsmBuffer[i+1].u.token ) );
                    break;
#endif
                default:
                    AsmError( SYNTAX_ERROR );
                    return( RC_ERROR );
                }
            }
            break;
        case TC_COMMA:
            if( Opnd_Count > OPND2 ) {
                AsmError( TOO_MANY_COMMAS );
                return( RC_ERROR );
            }
            i++;
            cur_opnd_label = false;
#if defined( _STANDALONE_ )
            Frame = NULL;
            SegOverride = NULL;
#endif
            if( EvalOperand( &i, Token_Count, &opndx, true ) ) {
                return( RC_ERROR );
            }
            Opnd_Count++;
            if( opndx.empty ) {
                if( AsmBuffer[i].class == TC_FLOAT
                    || AsmBuffer[i].class == TC_MINUS
                    || AsmBuffer[i].class == TC_PLUS ) {
                    i--;
                    continue;
                }
                AsmError( SYNTAX_ERROR );
                return( RC_ERROR );
            }
            if( Opnd_Count == OPND3 ) {
                if( rCode->info.token == T_SHLD || rCode->info.token == T_SHRD ) {
                    if( opndx.type == EXPR_CONST ) {
                        Opnd_Count--;
                    } else if( opndx.type == EXPR_REG ) {
                        if( AsmBuffer[opndx.base_reg].u.token == T_CL ) {
                            Opnd_Count--;
                            i--;
                            break;
                        }
                    }
                }
            }
            switch( opndx.type ) {
            case EXPR_ADDR:
                if( process_address( &opndx, &jmp_flags ) )
                    return( RC_ERROR );
//                if( jmp_flags == INDIRECT_JUMP )
//                    return( RC_ERROR );
                if( jmp_flags == SCRAP_INSTRUCTION )
                    return( RC_OK );
                break;
            case EXPR_CONST:
                process_const( &opndx, &jmp_flags );
                break;
            case EXPR_REG:
                process_reg( &opndx, &jmp_flags );
                break;
            case EXPR_UNDEF:
                return( RC_ERROR );
            default:
                break;
            }
            i--;
            break;
        case TC_COLON:
            if( last_opnd_label ) {
                if( AsmBuffer[i+1].class != TC_RES_ID ) {
                    if( MakeLabel( AsmBuffer[i-1].string_ptr, MT_NEAR ) ) {
                         return( RC_ERROR );
                    }
                }
                cur_opnd_label = false;
            } else {
                AsmError( SYNTAX_ERROR_UNEXPECTED_COLON );
                return( RC_ERROR );
            }
            break;
        case TC_PLUS:
        case TC_MINUS:
            break;
        case TC_FLOAT:
            if( idata_float( AsmBuffer[i].u.value ) ) {
                return( RC_ERROR );
            }
            if( AsmBuffer[i-1].class == TC_MINUS ) {
                rCode->data[Opnd_Count] ^= 0x80000000;
            }
#if defined( _STANDALONE_ )
            AsmWarn( 4, FLOAT_OPERAND );
#endif
            break;
        default:
#if defined( _STANDALONE_ )
/* */myassert( 0 );
#endif
            break;
        }
        last_opnd_label = cur_opnd_label;
    }
    switch( rCode->info.token ) {
    case T_LODS:
    case T_SCAS:
    case T_STOS:
        SizeString( OperandSize( Code->info.opnd_type[OPND1] ) );
        break;
    }
    if( Opnd_Count > OPND1 ) {
        if( check_size() ) {
            return( RC_ERROR );
        }
    }
    return( match_phase_1() );
}

static void SizeString( unsigned op_size )
/****************************************/
{
    /* size an string instruction based on it's operand's size */
    switch( op_size ) {
    case 1:
        Code->mem_type = MT_BYTE;
        Code->info.opcode &= NOT_W_BIT;
        if( Code->use32 )
            SET_OPSIZ_OFF( Code );
        break;
    case 2:
        Code->mem_type = MT_WORD;
        Code->info.opcode |= W_BIT;
        SET_OPSIZ_16( Code );
        break;
    case 4:
        Code->mem_type = MT_DWORD;
        Code->info.opcode |= W_BIT;
        SET_OPSIZ_32( Code );
        break;
    }
}

static bool check_size( void )
/****************************/
/*
- use to make sure the size of first operand match the size of second operand;
- optimize MOV instruction;
*/
{
    OPNDTYPE        op1 = Code->info.opnd_type[OPND1];
    OPNDTYPE        op2 = Code->info.opnd_type[OPND2];
    bool            state;
    long            temp;
    int             op1_size;
    int             op2_size;
    int             op_size = 0;
    unsigned char   rm_byte;

    state = RC_OK;
    switch( Code->info.token ) {
#if 0
    case T_PSLLW:
    case T_PSLLD:
    case T_PSLLQ:
    case T_PSRLW:
    case T_PSRLD:
    case T_PSRLQ:
    case T_PSRAW:
    case T_PSRAD:
        // check was wrong - instructions take a m64 OR an 8 bit immediate
        if( op2 & OP_I ) {
            op_size = OperandSize( op2 );
            if( op_size >= 2 ) {
                AsmError( OP_2_TOO_BIG );
                state = RC_ERROR;
            }
        }
        break;
#endif
    case T_IN:
        if( op2 == OP_DX ) {
            switch( op1 ) {
            case OP_AX:
                break;
            case OP_AL:
                Code->info.opcode &= NOT_W_BIT;         // clear w-bit
            case OP_EAX:
                if( Code->use32 ) {
                    SET_OPSIZ_OFF( Code );
                }
                break;
            }
        }
        break;
    case T_OUT:
        if( op1 == OP_DX ) {
            switch( op2 ) {
            case OP_AX:
                break;
            case OP_AL:
                Code->info.opcode &= NOT_W_BIT;         // clear w-bit
            case OP_EAX:
                if( Code->use32 ) {
                    SET_OPSIZ_OFF( Code );
                }
            }
        }
        break;
    case T_INS:
    case T_CMPS:
        op_size = OperandSize( op1 );
        /* fall through */
    case T_MOVS:
    case T_OUTS:
        if( op_size == 0 )
            op_size = OperandSize( op2 );

        SizeString( op_size );
        break;
    case T_LEA:
        switch( OperandSize( op1 ) ) {
        case 2:
        case 4:
            break;
        default:
            AsmError( OPERANDS_MUST_BE_THE_SAME_SIZE );
            state = RC_ERROR;
        }
        break;
    case T_RCL:
    case T_RCR:
    case T_ROL:
    case T_ROR:
    case T_SAL:
    case T_SAR:
    case T_SHL:
    case T_SHR:
    case T_LDS:
    case T_LES:
    case T_LFS:
    case T_LGS:
    case T_LSS:
        // checking will be done later
        break;
    case T_ENTER:
        // ENTER has to be OP_I16, OP_I8
        if( op1 == OP_I32 ) {
            //parse_phase_1 will treat 16-bit data as OP_I32 if CPU is 386
            if( Code->data[OPND1] > (signed long)USHRT_MAX ) {
                // if op1 is really 32-bit data, then error
                AsmError( IMMEDIATE_DATA_TOO_BIG );
                state = RC_ERROR;
            }
        }
        // type cast op1 to OP_I16
        Code->info.opnd_type[OPND1] = OP_I16;
        // op2 have to be 8-bit data
        if( op2 >= OP_I16 ) {
            if( Code->data[OPND2] > UCHAR_MAX ) {
                AsmError( IMMEDIATE_DATA_TOO_BIG );
                state = RC_ERROR;
            }
            Code->info.opnd_type[OPND2] = OP_I8;
        }
        break;
    case T_CALLF:
    case T_JMPF:
        // segment can only be 16-bit
        if( op1 > OP_I16 ) {
            AsmError( SEGMENT_TOO_BIG );
            state = RC_ERROR;
        }
        if( ( Code->info.cpu & ( P_CPU_MASK | P_PM ) ) <= P_286p ) {
            // offset can only be 16-bit if CPU is 286 and down
            if( op2 > OP_I16 ) {
                AsmError( OFFSET_TOO_BIG );
                state = RC_ERROR;
            }
        }
        // swap the 2 opnds to make output easier
        if( InsFixups[OPND2] != NULL ) {
            // absolute segment + offset nnnn,offset
            Code->info.opnd_type[OPND1] = Code->info.opnd_type[OPND2];
            InsFixups[OPND1] = InsFixups[OPND2];
            InsFixups[OPND2] = NULL;
        } else {
            // absolute ptr nnnn,nnnn
            Code->info.opnd_type[OPND1] = Code->use32 ? OP_I32 : OP_I16;
        }
        temp = Code->data[OPND1];
        Code->data[OPND1] = Code->data[OPND2];
        Code->data[OPND2] = temp;
        Code->info.opnd_type[OPND2] = OP_I16;
        Code->info.opcode = 0;
        break;
    case T_MOVSX:
    case T_MOVZX:
        Code->info.opcode &= NOT_W_BIT;
        op2_size = OperandSize( op2 );
        switch( OperandSize( op1 ) ) {
        case 4:
            switch( op2_size ) {
            case 2:
                Code->info.opcode |= W_BIT;
                break;
            case 4:
                AsmError( OP_2_TOO_BIG );
                state = RC_ERROR;
            }
            if( Code->use32 ) {
                SET_OPSIZ_OFF( Code );     // - don't need opnd size prefix
            }
            break;
        case 2:
            if( op2_size >= 2 ) {
                AsmError( OP_2_TOO_BIG );
                state = RC_ERROR;
            }
            break;
        default:
            // op1 have to be r16/r32
            AsmError( OP_1_TOO_SMALL );
            state = RC_ERROR;
        }
        break;
    case T_LSL:                                 /* 19-sep-93 */
        op1_size = OperandSize( op1 );
        switch( op1_size ) {
        case 2:
            if( Code->use32 )
                SET_OPSIZ_ON( Code );
            break;
        case 4:
            if( Code->use32 )
                SET_OPSIZ_OFF( Code );
            break;
        default:
            AsmError( INVALID_SIZE );
            return( RC_ERROR );
        }
        op2_size = OperandSize( op2 );
        switch( op2_size ) {
        case 2:
        case 4:
            break;
        default:
            AsmError( INVALID_SIZE );
            state = RC_ERROR;
            break;
        }
        break;
    case T_CVTSD2SI:
    case T_CVTTSD2SI:
    case T_CVTSS2SI:
    case T_CVTTSS2SI:
    case T_MOVNTI:
        break;
    case T_MOVD:
#if 0
        op1_size = OperandSize( op1 );
        op2_size = OperandSize( op2 );
        if( ( op1_size != 0 ) && ( op1_size != 4 )
            || ( op2_size != 0 ) && ( op2_size != 4 ) ) {
            AsmError( OPERANDS_MUST_BE_THE_SAME_SIZE );
            state = RC_ERROR;
        }
#endif
        break;
    case T_MOV:
        if( op1 & OP_SR ) {
            op2_size = OperandSize( op2 );
            if( ( op2_size == 2 ) || ( op2_size == 4 ) ) {
//                SET_OPSIZ_OFF( Code );
                return( state );
            }
        } else if( op2 & OP_SR ) {
            op1_size = OperandSize( op1 );
            if( ( op1_size == 2 ) || ( op1_size == 4 ) ) {
//                if( op1 == OP_M )
//                    SET_OPSIZ_OFF( Code );
                return( state );
            }
        } else if( ( op1 == OP_M ) || ( op2 == OP_M ) ) {
            // to optimize MOV
            rm_byte = Code->info.rm_byte;
            if( Code->info.opnd_type[OPND1] & OP_A ) {
                rm_byte = ( rm_byte & BIT_67 ) | ( ( rm_byte & BIT_012 ) << 3 ) | ( ( rm_byte & BIT_345 ) >> 3 );
                if( IS_ADRSIZ_32( Code ) && ( rm_byte == D32 )
                    || IS_ADRSIZ_16( Code ) && ( rm_byte == D16 ) ) {
                    // DS:[d32] or DS:[d16] can use MOV Mem with Acc (short form)
                } else {
                    // we have to change OP_A to OP_R
                    Code->info.opnd_type[OPND1] &= ~OP_A;
                }
            } else if( Code->info.opnd_type[OPND2] & OP_A ) {
                if( IS_ADRSIZ_32( Code ) && ( rm_byte == D32 )
                    || IS_ADRSIZ_16( Code ) && ( rm_byte == D16 ) ) {
                    // DS:[d32] or DS:[d16] can use MOV Mem with Acc (short form)
                } else {
                    // we have to change OP_A to OP_R
                    Code->info.opnd_type[OPND2] &= ~OP_A;
                }
            }
        } else if( ( op1 & OP_SPEC_REG ) || ( op2 & OP_SPEC_REG ) ) {
            SET_OPSIZ_OFF( Code );
            return( state );
        }
        // no break;
    default:
        // make sure the 2 opnds are of the same type
        op1_size = OperandSize( op1 );
        op2_size = OperandSize( op2 );
        if( op1_size > op2_size ) {
            if( ( op2 >= OP_I8 ) && ( op2 <= OP_I32 ) ) {     /* immediate */
                op2_size = op1_size;    /* promote to larger size */
            }
        }
        if( ( op1_size == 1 ) && ( op2 == OP_I16 )
            && ( Code->data[OPND2] <= UCHAR_MAX ) ) {
            return( state ); // OK cause no sign extension
        }
        if( ( op1_size == 2 ) && ( op2 == OP_I32 )
            && ( Code->data[OPND2] <= USHRT_MAX ) ) {
            return( state ); // OK cause no sign extension
        }
        if( op1_size != op2_size ) {
            /* if one or more are !defined, set them appropriately */
            if( ( op1 | op2 ) & ( OP_MMX | OP_XMM ) ) {
            } else if( ( op1_size != 0 ) && ( op2_size != 0 ) ) {
                AsmError( OPERANDS_MUST_BE_THE_SAME_SIZE );
//                state = RC_ERROR;
            }
            if( op1_size == 0 ) {
                if( ( op1 & OP_M_ANY ) && ( op2 & OP_I ) ) {
                    if( (unsigned long)Code->data[OPND2] > USHRT_MAX ) {
                         Code->mem_type = MT_DWORD;
                         Code->info.opcode |= W_BIT;
                         Code->info.opnd_type[OPND2] = OP_I32;
#if defined( _STANDALONE_ )
                         if( Parse_Pass == PASS_1 ) {
                             if( (Options.mode & MODE_IDEAL) == 0 ) {
                                 AsmWarn( 1, ASSUMING_DWORD );
                             }
                         }
#endif
                    } else if( (unsigned long)Code->data[OPND2] > UCHAR_MAX ) {
                         Code->mem_type = MT_WORD;
                         Code->info.opcode |= W_BIT;
                         Code->info.opnd_type[OPND2] = OP_I16;
#if defined( _STANDALONE_ )
                         if( Parse_Pass == PASS_1 ) {
                             if( (Options.mode & MODE_IDEAL) == 0 ) {
                                 AsmWarn( 1, ASSUMING_WORD );
                             }
                         }
#endif
                    } else {
                         Code->mem_type = MT_BYTE;
                         Code->info.opnd_type[OPND2] = OP_I8;
#if defined( _STANDALONE_ )
                         if( Parse_Pass == PASS_1 ) {
                             if( (Options.mode & MODE_IDEAL) == 0 ) {
                                 AsmWarn( 1, ASSUMING_BYTE );
                             }
                         }
#endif
                    }
                } else if( ( op1 & OP_M_ANY ) && ( op2 & ( OP_R | OP_SR ) ) ) {
                } else if( ( op1 & ( OP_MMX | OP_XMM ) ) && ( op2 & OP_I ) ) {
                    if( (unsigned long)Code->data[OPND2] > USHRT_MAX ) {
                         Code->info.opnd_type[OPND2] = OP_I32;
                    } else if( (unsigned long)Code->data[OPND2] > UCHAR_MAX ) {
                         Code->info.opnd_type[OPND2] = OP_I16;
                    } else {
                         Code->info.opnd_type[OPND2] = OP_I8;
                    }
                } else if( ( op1 | op2 ) & ( OP_MMX | OP_XMM ) ) {
                } else {
#if defined( _STANDALONE_ )
                    AsmIntErr( 1 );
#endif
                    switch( op2_size ) {
                    case 1:
                        Code->mem_type = MT_BYTE;
#if defined( _STANDALONE_ )
                        if( ( Parse_Pass == PASS_1 ) && ( op2 & OP_I ) ) {
                            if( (Options.mode & MODE_IDEAL) == 0 ) {
                                AsmWarn( 1, ASSUMING_BYTE );
                            }
                        }
#endif
                        break;
                    case 2:
                        Code->mem_type = MT_WORD;
                        Code->info.opcode |= W_BIT;
#if defined( _STANDALONE_ )
                        if( ( Parse_Pass == PASS_1 ) && ( op2 & OP_I ) ) {
                            if( (Options.mode & MODE_IDEAL) == 0 ) {
                                AsmWarn( 1, ASSUMING_WORD );
                            }
                        }
#endif
                        if( Code->use32 )
                            SET_OPSIZ_ON( Code );
                        break;
                    case 4:
                        Code->mem_type = MT_DWORD;
                        Code->info.opcode |= W_BIT;
#if defined( _STANDALONE_ )
                        if( ( Parse_Pass == PASS_1 ) && ( op2 & OP_I ) ) {
                             if( (Options.mode & MODE_IDEAL) == 0 ) {
                                AsmWarn( 1, ASSUMING_DWORD );
                            }
                        }
#endif
                        break;
                    }
                }
            }
        }
    }
    return( state );
}

#if !defined( _STANDALONE_ )

void AsmInit( int use32, int cpu, int fpu, bool fpu_emu )
/*******************************************************/
{
    Code->use32 = ( use32 != 0 );
    switch( cpu ) {
    case 0: Code->info.cpu = P_86;   break;
    case 1: Code->info.cpu = P_186;  break;
    case 2: Code->info.cpu = P_286p; break;
    case 3: Code->info.cpu = P_386p; break;
    case 4: Code->info.cpu = P_486p; break;
    case 5: Code->info.cpu = P_586p | P_K3D | P_MMX; break;
    case 6: Code->info.cpu = P_686p | P_K3D | P_MMX | P_SSE | P_SSE2 | P_SSE3; break;
    }
    floating_point = ( fpu_emu ) ? DO_FP_EMULATION : NO_FP_EMULATION;
    switch( fpu ) {
    case 0: floating_point = NO_FP_ALLOWED; break;
    case 1: Code->info.cpu |= P_87;  break;
    case 2: Code->info.cpu |= P_287; break;
    case 3: Code->info.cpu |= P_387; break;
    }
}

static enum asm_cpu CPUinfo;

void AsmSaveCPUInfo( void )
{
    CPUinfo = Code->info.cpu;
}

void AsmRestoreCPUInfo( void )
{
    Code->info.cpu = CPUinfo;
}

#endif
