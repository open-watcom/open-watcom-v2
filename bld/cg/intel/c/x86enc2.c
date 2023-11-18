/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Intel instruction encoding, part II. Processes labels,
*               jumps and the like.
*
****************************************************************************/


#include "_cgstd.h"
#include "coderep.h"
#include "optmain.h"
#include "cgmem.h"
#include "system.h"
#include "x86objd.h"
#include "pcencode.h"
#include "cgauxcc.h"
#include "cgauxinf.h"
#include "seldef.h"
#include "pccode.h"
#include "encode.h"
#include "data.h"
#include "types.h"
#include "utils.h"
#include "objout.h"
#include "object.h"
#include "intrface.h"
#include "targetin.h"
#include "targetdb.h"
#include "opttell.h"
#include "x87.h"
#include "x86esc.h"
#include "rgtbl.h"
#include "split.h"
#include "namelist.h"
#include "x86enc.h"
#include "x86enc2.h"
#include "feprotos.h"


typedef enum {
    UNSIGNED,               /* always an unsigned jump */
    SIGNED_86,              /* signed if 8086 instruction, else unsigned */
    SIGNED_87,              /* signed if 8087 instruction, else unsigned */
    SIGNED_BOTH             /* always signed */
} issigned;

static byte UCondTable[] = {
/***************************
 * The 8086 code for an unsigned jmp
 */
    5,              /* OP_BIT_TEST_TRUE */
    4,              /* OP_BIT_TEST_FALSE */
    4,              /* OP_CMP_EQUAL */
    5,              /* OP_CMP_NOT_EQUAL */
    7,              /* OP_CMP_GREATER */
    6,              /* OP_CMP_LESS_EQUAL */
    2,              /* OP_CMP_LESS */
    3               /* OP_CMP_GREATER_EQUAL */
};

static byte SCondTable[] = {
/***************************
 * The 8086 code for a signed jmp
 */
    5,              /* OP_BIT_TEST_TRUE */
    4,              /* OP_BIT_TEST_FALSE */
    4,              /* OP_CMP_EQUAL */
    5,              /* OP_CMP_NOT_EQUAL */
    15,             /* OP_CMP_GREATER */
    14,             /* OP_CMP_LESS_EQUAL */
    12,             /* OP_CMP_LESS */
    13              /* OP_CMP_GREATER_EQUAL */
};

static byte rev_condition[] = {
/******************************
 * Reverse the sense of an 8086 jmp (ie: ja -> jbe)
 * i.e. XOR 1
 */
    1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14
};

static issigned signed_type[] = {
/********************************
 * What kind of a jump does the instruction need following it
 */
    UNSIGNED,       /* U1*/
    SIGNED_BOTH,    /* I1*/
    UNSIGNED,       /* U2*/
    SIGNED_BOTH,    /* I2*/
    UNSIGNED,       /* U4*/
    SIGNED_BOTH,    /* I4*/
    UNSIGNED,       /* U8*/
    SIGNED_BOTH,    /* I8*/
    UNSIGNED,       /* CP*/
    UNSIGNED,       /* PT*/
    SIGNED_86,      /* FS*/
    SIGNED_86,      /* FD*/
    SIGNED_86,      /* FL*/
    UNSIGNED        /* XX*/
};

unsigned DepthAlign( unsigned depth )
/***********************************/
{
    static unsigned char    AlignArray[10] = { 0 };

    if( AlignArray[0] == 0 || depth == PROC_ALIGN ) {
        unsigned char *align_info_bytes = FEAuxInfo( NULL, FEINF_CODE_LABEL_ALIGNMENT );
        Copy( align_info_bytes, AlignArray, align_info_bytes[0] + 1 );
    }
    if( OptForSize > 0 )
        return( 1 );
    if( _CPULevel( CPU_486 ) ) {
        if( depth == PROC_ALIGN || depth == DEEP_LOOP_ALIGN )
            return( 16 );
        return( 1 );
    }
    if( _CPULevel( CPU_386 ) ) {
        if( depth == PROC_ALIGN || depth == DEEP_LOOP_ALIGN )
            return( 4 );
        return( 1 );
    }
    if( depth == PROC_ALIGN || depth == DEEP_LOOP_ALIGN ) {
        return( AlignArray[1] );
    }
    if( depth == 0 )
        depth = 1;
    if( depth >= AlignArray[0] ) {
        depth = AlignArray[0] - 1;
    }
    return( AlignArray[depth + 1] );
}

byte    CondCode( instruction *cond )
/************************************
 * Return the condition code number for the encoding, associated with "cond"
 */
{
    issigned        is_signed;

    if( _FPULevel( FPU_87 ) ) {
        is_signed = SIGNED_87;
    } else {
        is_signed = SIGNED_86;
    }
    if( is_signed & signed_type[cond->type_class] ) {
        return( SCondTable[cond->head.opcode - FIRST_CONDITION] );
    } else {
        return( UCondTable[cond->head.opcode - FIRST_CONDITION] );
    }
}

void    GenSetCC( instruction *cond )
/************************************
 * Given a conditional "cond", generate the correct setxx instruction
 */
{
    _Code;
    LayOpword( M_SETCC | CondCode( cond ) );
    if( cond->result->n.class == N_REGISTER ) {
        LayRMRegOp( cond->result );
    } else {
        LayModRM( cond->result );
    }
    AddToTemp( M_SECONDARY );
    _Emit;
}

byte    ReverseCondition( byte cond )
/************************************
 * Reverse the sense of a conditional jump (already encoded)
 */
{
    return( rev_condition[cond] );
}

void    DoCall( label_handle lbl, bool imported, bool big, bool pop )
/********************************************************************
 * Call routine "lbl".
 */
{
    oc_class        occlass;
    oc_dest_attr    destattr;

    /* unused parameters */ (void)imported;

    occlass = OC_CALL;
    if( pop )
        occlass |= OC_ATTR_POP;
    if( big )
        occlass |= OC_ATTR_FAR;
    if( !big ) {
        destattr = OC_DEST_NEAR;
    } else if( AskIfRTLabel( lbl )
            || imported //NYI:multi-code-segment, this can go when FORTRAN is fixed up
            || AskCodeSeg() != FESegID( AskForLblSym( lbl ) ) ) {
        destattr = OC_DEST_FAR;
    } else {
        destattr = OC_DEST_CHEAP;
    }
    CodeHandle( occlass, OptInsSize( occlass, destattr ), lbl );
}


static  void    CodeSequence( const byte *p, byte_seq_len len )
/**************************************************************
 * Dump an inline sequence, taking into account the floating fixups and
 * the "seg foo", "offset foo" sequences.
 */
{
    bool            first;
    const byte      *endp;
    const byte      *startp;
    byte            type;
    cg_sym_handle   sym = 0;
    offset          off = 0;
    fe_attr         attr = 0;
    name            *temp;

    first = false;
    endp = p + len;
    while( p < endp ) {
        _Code;
        startp = p;
        for( ; p < endp && ( p - startp ) < ( INSSIZE - 5 ); ) {
            if( p[0] == FLOATING_FIXUP_BYTE ) {
                type = p[1];
                switch( type ) {
                case FLOATING_FIXUP_BYTE:
                    ++p;
                    break;
                case FIX_SYM_OFFSET:
                case FIX_SYM_SEGMENT:
                case FIX_SYM_RELOFF:
                    p += 2;
                    sym = *(BYTE_SEQ_SYM *)p;
                    p += sizeof( BYTE_SEQ_SYM );
                    off = *(BYTE_SEQ_OFF *)p;
                    p += sizeof( BYTE_SEQ_OFF );
                    attr = FEAttr( sym );
                    switch( type ) {
                    case FIX_SYM_SEGMENT:
                        ILen += 2;
                        if( attr & (FE_STATIC | FE_GLOBAL) ) {
                            DoFESymRef( sym, CG_FE, off, FE_FIX_BASE );
                        } else {
                            FEMessage( FEMSG_ERROR, "aux seg used with local symbol" );
                        }
                        break;
                    case FIX_SYM_OFFSET:
                        ILen += WORD_SIZE;
                        if( attr & (FE_STATIC | FE_GLOBAL) ) {
                            DoFESymRef( sym, CG_FE, off, FE_FIX_OFF );
                        } else {
                            temp = DeAlias( AllocUserTemp( sym, U1 ) );
                            if( temp->t.location != NO_LOCATION ) {
                                EmitOffset( NewBase( temp ) - temp->v.offset + off );
                            } else {
                                FEMessage( FEMSG_ERROR, "aux offset used with register symbol" );
                            }
                        }
                        break;
                    case FIX_SYM_RELOFF:
                        ILen += WORD_SIZE;
                        if( attr & FE_PROC ) {
                            DoFESymRef( sym, CG_FE, off, FE_FIX_SELF );
                        } else {
                            FEMessage( FEMSG_ERROR, "aux reloff used with data symbol" );
                        }
                        break;
                    }
                    continue;
                default:
                    // floating point fixups
                    if( !first ) {
                        // ensure previous instructions be emited and
                        // start new FPU instruction
                        startp = p - INSSIZE;
                        first = true;
                        continue;
                    }
                    p += 2;
                    if( _IsEmulation() ) {
                        SetFPPatchType( type );
                        Used87 = true;
                    }
                    break;
                }
            }
            if( first ) {
                LayOpbyte( *p );
                first = false;
            } else {
                AddByte( *p );
            }
            ++p;
        }
        _Emit;
    }
}


static  void    GenNoReturn( void )
/**********************************
 * Generate a noreturn instruction (pseudo instruction)
 */
{
    any_oc          oc;

    oc.oc_entry.hdr.class = OC_NORET;
    oc.oc_entry.hdr.reclen = offsetof( oc_entry, data );
    oc.oc_entry.hdr.objlen = 0;
    InputOC( &oc );
}

static  void    Pushf( void )
/***************************/
{
    LayOpbyte( 0x9c ); /* PUSHF*/
    _Emit;
    _Code;
}


void    GenCall( instruction *ins )
/**********************************
 * Generate a call for "ins". (eg: call foo, or call far ptr foo)
 */
{
    name            *op;
    cg_sym_handle   sym;
    bool            imp;
    call_class      cclass;
    byte_seq        *code;
    label_handle    lbl;
    bool            far_call;

    if( ins->flags.call_flags & CALL_INTERRUPT ) {
        Pushf();
    }
    op = ins->operands[CALL_OP_ADDR];
    cclass = (call_class)(pointer_uint)FindAuxInfo( op, FEINF_CALL_CLASS );
    far_call = ( ((call_class_target)(pointer_uint)FindAuxInfo( op, FEINF_CALL_CLASS_TARGET ) & FECALL_X86_FAR_CALL) != 0 );
    code = FindAuxInfo( op, FEINF_CALL_BYTES );
    if( code != NULL ) {
        _Emit;
        if( code->relocs ) {
            CodeSequence( code->data, code->length );
        } else {
            CodeBytes( code->data, code->length );
        }
    } else if( (cclass & FECALL_GEN_ABORTS)
      && _IsntTargetModel( CGSW_X86_NEW_P5_PROFILING ) ) {
        sym = op->v.symbol;
        lbl = FEBack( sym )->lbl;
        if( far_call && (FEAttr( sym ) & FE_IMPORT) ) {
            CodeHandle( OC_JMP | OC_ATTR_FAR, OptInsSize( OC_JMP, OC_DEST_FAR ), lbl );
        } else {
            CodeHandle( OC_JMP, OptInsSize( OC_JMP, OC_DEST_NEAR ), lbl );
        }
        return;
    } else {
        sym = op->v.symbol;
        if( op->m.memory_type == CG_FE ) {
            lbl = FEBack( sym )->lbl;
            imp = ( (FEAttr( sym ) & (FE_COMMON | FE_IMPORT)) != 0 );
        } else {
            /*
             * handles mismatch Fix it!
             */
            lbl = (label_handle)sym;
            imp = true;
        }
        DoCall( lbl, imp, far_call, (ins->flags.call_flags & CALL_POPS_PARMS) != 0 );
    }
    if( (cclass & (FECALL_GEN_ABORTS | FECALL_GEN_NORETURN))
      && _IsntTargetModel( CGSW_X86_NEW_P5_PROFILING ) ) {
        GenNoReturn();
    }
}


void    GenCallIndirect( instruction *ins )
/******************************************
 * Generate an indirect call for "ins" (eg: call dword ptr [eax])
 */
{
    oc_class        occlass;
    gen_opcode      opcode;
    name            *op;

    if( ins->flags.call_flags & CALL_INTERRUPT ) {
        Pushf();
    }
    if( (ins->flags.call_flags & CALL_ABORTS)
      && _IsntTargetModel( CGSW_X86_NEW_P5_PROFILING ) ) {
        occlass = OC_JMPI;
    } else {
        occlass = OC_CALLI;
    }
    if( ins->flags.call_flags & CALL_POPS_PARMS ) {
        occlass |= OC_ATTR_POP;
    }
    op = ins->operands[CALL_OP_ADDR];
    opcode = M_CJINEAR;
    if( op->n.type_class == PT
      || op->n.type_class == CP ) {
        opcode = M_CJILONG;
        occlass |= OC_ATTR_FAR;
    }
    ReFormat( occlass );
    LayOpword( opcode );
    LayModRM( op );
    _Emit;
    if( (ins->flags.call_flags & CALL_NORETURN)
      && _IsntTargetModel( CGSW_X86_NEW_P5_PROFILING ) ) {
        GenNoReturn();
    }
}


void    GenCallRegister( instruction *ins )
/******************************************
 * Generate a call to a register (eg: call eax)
 */
{
    name            *op;
    oc_class        occlass;

    if( ins->flags.call_flags & CALL_INTERRUPT ) {
        Pushf();
    }
    op = ins->operands[CALL_OP_ADDR];
    occlass = OC_CALLI;
    if( ins->flags.call_flags & CALL_POPS_PARMS ) {
        occlass |= OC_ATTR_POP;
    }
    ReFormat( occlass );
    LayOpword( M_CJINEAR );
    LayRegRM( op->r.reg );
    _Emit;
    if( (ins->flags.call_flags & CALL_NORETURN)
      && _IsntTargetModel( CGSW_X86_NEW_P5_PROFILING ) ) {
        GenNoReturn();
    }
}


void    GenSelEntry( bool starts )
/*********************************
 * Dump a queue that a select table is starting/ending ("starts") into
 * the code segment queue.
 */
{
    any_oc          oc;

    oc.oc_select.hdr.class = OC_INFO + OC_INFO_SELECT;
    oc.oc_select.hdr.reclen = sizeof( oc_select );
    oc.oc_select.hdr.objlen = 0;
    oc.oc_select.starts = starts;
    InputOC( &oc );
}


void    Gen1ByteValue( byte value )
/**********************************
 * Drop an 8 bit integer into the queue.
 */
{
    _Code;
    AddByte( value );
    _Emit;
}


void    Gen2ByteValue( uint_16 value )
/*************************************
 * Drop a 16 bit integer into the queue.
 */
{
    _Code;
    AddByte( value & 0xFF );
    AddByte( ( value >> 8 ) & 0xFF );
    _Emit;
}


void    Gen4ByteValue( uint_32 value )
/*************************************
 * Drop a 32 bit integer into the queue.
 */
{
    _Code;
    AddByte( value & 0xFF );
    AddByte( ( value >> 8 ) & 0xFF );
    AddByte( ( value >> 16 ) & 0xFF );
    AddByte( ( value >> 24 ) & 0xFF );
    _Emit;
}


void    GenCodePtr( pointer label )
/**********************************
 * Dump a near reference to a label into the code segment.
 */
{
    CodeHandle( OC_LREF, TypeAddress( TY_NEAR_CODE_PTR )->length, label );
}


void    GenCallLabel( pointer label )
/************************************
 * Generate a call to a label within the procedure (near call)
 */
{
    DoCall( label, false, false, false );
}


void    GenLabelReturn( void )
/*****************************
 * Generate a return from CALL_LABEL instruction (near return)
 */
{
    GenReturn( 0, false );
}

void    GenReturn( int pop, bool is_long )
/*****************************************
 * Generate a return instruction
 */
{
    any_oc          oc;

    oc.oc_ret.hdr.class = OC_RET;
    oc.oc_ret.hdr.reclen = sizeof( oc_ret );
    oc.oc_ret.hdr.objlen = 1;
    if( pop != 0 ) {
        oc.oc_ret.hdr.class |= OC_ATTR_POP;
        oc.oc_ret.hdr.objlen += 2;
    }
    if( is_long ) {
        oc.oc_ret.hdr.class |= OC_ATTR_FAR;
    }
    oc.oc_ret.ref = NULL;
    oc.oc_ret.pops = pop;
    InputOC( &oc );
}

void    GenIRET( void )
/**********************
 * Generate a IRET instruction
 */
{
    any_oc          oc;

    oc.oc_ret.hdr.class = OC_RET | OC_ATTR_IRET;
    oc.oc_ret.hdr.reclen = sizeof( oc_ret );
    oc.oc_ret.hdr.objlen = 1;
    oc.oc_ret.ref = NULL;
    oc.oc_ret.pops = 0;
    InputOC( &oc );
}

void    GenJmpMemory( instruction *ins )
/***************************************
 * Generate a jump indirect through memory instruction.
 */
{
    label_handle    lbl;
    name            *base;

    if( ins->head.opcode != OP_SELECT
      && _IsTargetModel( CGSW_X86_BIG_CODE ) ) {
        ReFormat( OC_JMPI | OC_ATTR_FAR );
        LayOpword( M_CJILONG );
    } else {
        ReFormat( OC_JMPI );
        LayOpword( M_CJINEAR );
    }
    LayModRM( ins->operands[0] );
    if( ins->head.opcode == OP_SELECT &&
        ins->operands[0]->n.class == N_INDEXED ) {
        base = ins->operands[0]->i.base;
        if( base != NULL ) {
            lbl = AskForSymLabel( base->v.symbol, CG_TBL );
            if( AskAddress( lbl ) != ADDR_UNKNOWN ) {
                TellScrapLabel( lbl );
            }
        }
    }
}

static  void    JumpReg( instruction *ins, name *reg_name )
/**********************************************************
 * Generate a jump to register instruction (eg: jmp eax)
 */
{
    hw_reg_set      regs;

    /* unused parameters */ (void)ins;

    regs = reg_name->r.reg;
    if( reg_name->n.size > WORD_SIZE ) {
        /* fake up intersegment jump*/
        _Code;
        LayOpbyte( M_PUSH );
        LayRegAC( High32Reg( regs ) );
        _Next;
        LayOpbyte( M_PUSH );
        LayRegAC( Low32Reg( regs ) );
        _Emit;
        GenReturn( 0, true );
    } else {
        ReFormat( OC_JMPI );
        LayOpword( M_CJINEAR );
        LayRegRM( regs );
        _Emit;
    }
}

void    GenJmpRegister( instruction *ins )
/*****************************************
 * Generate a jump to register instruction (eg: jmp eax)
 */
{
    JumpReg( ins, ins->operands[0] );
}


static  void    DoCodeBytes( const void *src, byte_seq_len len, oc_class occlass )
/*********************************************************************************
 * Dump bytes "src" directly into the queue, for length "len".
 */
{
    any_oc          *oc;
    uint            addlen;

    oc = CGAlloc( offsetof( oc_entry, data ) + MAX_OBJ_LEN );
    oc->oc_entry.hdr.class = occlass;
    oc->oc_entry.hdr.reclen = offsetof( oc_entry, data ) + MAX_OBJ_LEN;
    oc->oc_entry.hdr.objlen = MAX_OBJ_LEN;
    addlen = 0;
    if( occlass == OC_IDATA ) {
        if( len > 255 )
            len = 255;
        addlen = 1;
    }
    while( len + addlen > MAX_OBJ_LEN ) {
        Copy( src, oc->oc_entry.data, MAX_OBJ_LEN );
        InputOC( oc );
        src = (char *)src + MAX_OBJ_LEN;
        len -= MAX_OBJ_LEN;
    }
    oc->oc_entry.hdr.reclen = offsetof( oc_entry, data ) + len + addlen;
    oc->oc_entry.hdr.objlen = len + addlen;
    Copy( src, oc->oc_entry.data, len );
    if( addlen )
        oc->oc_entry.data[len] = (byte)len;
    InputOC( oc );
    CGFree( oc );
}

void    CodeBytes( const void *src, byte_seq_len len )
/****************************************************/
{
    DoCodeBytes( src, len, OC_BDATA );
}

void    EyeCatchBytes( const void *src, byte_seq_len len )
/********************************************************/
{
    DoCodeBytes( src, len, OC_IDATA );
}
