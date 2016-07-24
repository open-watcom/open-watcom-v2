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
* Description:  Intel instruction encoding, part II. Processes labels,
*               jumps and the like.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "ocentry.h"
#include "optmain.h"
#include "cgmem.h"
#include "system.h"
#include "escape.h"
#include "pcencode.h"
#include "cgauxinf.h"
#include "cgdefs.h"
#include "seldef.h"
#include "pccode.h"
#include "fppatch.h"
#include "cypfunc.h"
#include "encode.h"
#include "data.h"
#include "types.h"
#include "utils.h"
#include "objout.h"
#include "object.h"
#include "i86enc2.h"
#include "intrface.h"
#include "targetin.h"
#include "targetdb.h"
#include "opttell.h"
#include "feprotos.h"


extern  hw_reg_set      Low32Reg(hw_reg_set);
extern  void            EjectInst( void );
extern  void            LayRegAC(hw_reg_set);
extern  hw_reg_set      High32Reg(hw_reg_set);
extern  void            LayOpbyte(gen_opcode);
extern  void            Format(oc_class);
extern  void            LayRegRM(hw_reg_set);
extern  void            LayRMRegOp(name*);
extern  void            LayModRM(name*);
extern  void            LayOpword(gen_opcode);
extern  void            ReFormat(oc_class);
extern  void            Finalize( void );
extern  void            AddByte(byte);
extern  obj_length      OptInsSize(oc_class,oc_dest_attr);
extern  void            AddToTemp(byte);
extern  void            DoFESymRef( cg_sym_handle, cg_class, offset, fe_fixup_types);
extern  void            FlipCond(instruction*);
extern  name            *DeAlias(name*);
extern  name            *AllocUserTemp(pointer,type_class_def);
extern  void            EmitOffset(offset);

static  void            JumpReg( instruction *ins, name *reg_name );
static  void            Pushf(void);

extern  fp_patches      FPPatchType;
extern  bool            Used87;


static byte UCondTable[] = {
/***************************
    the 8086 code for an unsigned jmp
*/
        5,              /* OP_BIT_TEST_TRUE */
        4,              /* OP_BIT_TEST_FALSE */
        4,              /* OP_CMP_EQUAL */
        5,              /* OP_CMP_NOT_EQUAL */
        7,              /* OP_CMP_GREATER */
        6,              /* OP_CMP_LESS_EQUAL */
        2,              /* OP_CMP_LESS */
        3 };            /* OP_CMP_GREATER_EQUAL */

static byte SCondTable[] = {
/***************************
    the 8086 code for a signed jmp
*/
        5,              /* OP_BIT_TEST_TRUE */
        4,              /* OP_BIT_TEST_FALSE */
        4,              /* OP_CMP_EQUAL */
        5,              /* OP_CMP_NOT_EQUAL */
        15,             /* OP_CMP_GREATER */
        14,             /* OP_CMP_LESS_EQUAL */
        12,             /* OP_CMP_LESS */
        13 };           /* OP_CMP_GREATER_EQUAL */

static byte RevCond[] = {
/************************
    reverse the sense of an 8086 jmp (ie: ja -> jbe)
*/
        1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14 };        /* i.e. XOR 1*/

typedef enum {
        UNSIGNED,               /* always an unsigned jump */
        SIGNED_86,              /* signed if 8086 instruction, else unsigned */
        SIGNED_87,              /* signed if 8087 instruction, else unsigned */
        SIGNED_BOTH             /* always signed */
} issigned;

static issigned Signed[] = {
/***************************
    what kind of a jump does the instruction need following it
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
        UNSIGNED };     /* XX*/


unsigned DepthAlign( unsigned depth )
/***********************************/
{
    static unsigned char AlignArray[10] = { 0 };

    if( AlignArray[0] == 0 || depth == PROC_ALIGN ) {
        unsigned char *align_info_bytes = FEAuxInfo( NULL, CODE_LABEL_ALIGNMENT );
        Copy( align_info_bytes, AlignArray, align_info_bytes[0] + 1 );
    }
    if( OptForSize )
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

byte    CondCode( instruction *cond ) {
/**************************************
    Return the condition code number for the encoding, associated with "cond"
*/

    issigned            is_signed;

    if( _FPULevel( FPU_87 ) ) {
        is_signed = SIGNED_87;
    } else {
        is_signed = SIGNED_86;
    }
    if( is_signed & Signed[cond->type_class] ) {
        return( SCondTable[cond->head.opcode - FIRST_CONDITION] );
    } else {
        return( UCondTable[cond->head.opcode - FIRST_CONDITION] );
    }
}

void    GenSetCC( instruction *cond ) {
/**************************************
    given a conditional "cond", generate the correct setxx instruction
*/

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

byte    ReverseCondition( byte cond ) {
/**************************************
    reverse the sense of a conditional jump (already encoded)
*/

    return( RevCond[cond] );
}

void    DoCall( label_handle lbl, bool imported, bool big, bool pop )
/********************************************************************
    call routine "lbl".
*/
{
    oc_class    occlass;
    obj_length  len;

    imported = imported;
    occlass = OC_CALL;
    if( pop )
        occlass |= ATTR_POP;
    if( big )
        occlass |= ATTR_FAR;
    if( !big ) {
        len = OptInsSize( OC_CALL, OC_DEST_NEAR );
    } else if( AskIfRTLabel( lbl )
            || imported //NYI:multi-code-segment, this can go when FORTRAN is fixed up
            || AskCodeSeg() != FESegID( AskForLblSym( lbl ) ) ) {
        len = OptInsSize( OC_CALL, OC_DEST_FAR );
    } else {
        len = OptInsSize( OC_CALL, OC_DEST_CHEAP );
    }
    CodeHandle( occlass, len, lbl );
}


static  void    CodeSequence( const byte *p, byte_seq_len len )
/**************************************************************
    Dump an inline sequence, taking into account the floating fixups and
    the "seg foo", "offset foo" sequences.
*/
{
    bool        first;
    const byte  *endp;
    const byte  *startp;
    byte        type;
    cg_sym_handle sym = 0;
    offset      off = 0;
    fe_attr     attr = 0;
    name        *temp;

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
                            FEMessage( MSG_ERROR, "aux seg used with local symbol" );
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
                                FEMessage( MSG_ERROR, "aux offset used with register symbol" );
                            }
                        }
                        break;
                    case FIX_SYM_RELOFF:
                        ILen += WORD_SIZE;
                        if( attr & FE_PROC ) {
                            DoFESymRef( sym, CG_FE, off, FE_FIX_SELF );
                        } else {
                            FEMessage( MSG_ERROR, "aux reloff used with data symbol" );
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
                        FPPatchType = type;
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


static  void    GenNoReturn( void ) {
/************************************
    Generate a noreturn instruction (pseudo instruction)
*/

    any_oc      oc;

    oc.oc_ret.hdr.class = OC_RET | ATTR_NORET;
    oc.oc_ret.hdr.reclen = sizeof( oc_ret );
    oc.oc_ret.hdr.objlen = 0;
    oc.oc_ret.ref = NULL;
    oc.oc_ret.pops = 0;
    InputOC( &oc );
}

void    GenCall( instruction *ins ) {
/************************************
    Generate a call for "ins". (eg: call foo, or call far ptr foo)
*/

    name                *op;
    cg_sym_handle       sym;
    bool                imp;
    call_class          cclass;
    byte_seq            *code;
    label_handle        lbl;

    if( ins->flags.call_flags & CALL_INTERRUPT ) {
        Pushf();
    }
    op = ins->operands[CALL_OP_ADDR];
    cclass = *(call_class *)FindAuxInfo( op, CALL_CLASS );
    code = FindAuxInfo( op, CALL_BYTES );
    if( code != NULL ) {
        _Emit;
        if( code->relocs ) {
            CodeSequence( code->data, code->length );
        } else {
            CodeBytes( code->data, code->length );
        }
        if( cclass & SUICIDAL ) {
            GenNoReturn();
        }
    } else if( ( cclass & SUICIDAL ) && _IsntTargetModel( NEW_P5_PROFILING ) ) {
        sym = op->v.symbol;
        lbl = FEBack( sym )->lbl;
        if( (cclass & FAR_CALL) && (FEAttr( sym ) & FE_IMPORT) ) {
            CodeHandle( OC_JMP | ATTR_FAR, OptInsSize( OC_JMP, OC_DEST_FAR ), lbl );
        } else {
            CodeHandle( OC_JMP, OptInsSize( OC_JMP, OC_DEST_NEAR ), lbl );
        }
    } else {
        sym = op->v.symbol;
        if( op->m.memory_type == CG_FE ) {
            lbl = FEBack( sym )->lbl;
            imp = (FEAttr( sym ) & (FE_COMMON | FE_IMPORT)) != 0;
        } else {
            // handles mismatch Fix it!
            lbl = (label_handle)sym;
            imp = true;
        }
        DoCall( lbl, imp, (cclass & FAR_CALL) != 0, (ins->flags.call_flags & CALL_POPS_PARMS) != 0 );
    }
}


void    GenICall( instruction *ins ) {
/*************************************
    Generate an indirect call for "ins" (eg: call dword ptr [eax])
*/

    oc_class    entry;
    gen_opcode  opcode;

    if( ins->flags.call_flags & CALL_INTERRUPT ) {
        Pushf();
    }
    entry = 0;
    if( ins->flags.call_flags & CALL_POPS_PARMS ) {
        entry |= ATTR_POP;
    }
    if( ( ins->flags.call_flags & CALL_ABORTS ) && _IsntTargetModel( NEW_P5_PROFILING ) ) {
        entry |= OC_JMPI;
    } else {
        entry |= OC_CALLI;
    }
    if( ins->operands[CALL_OP_ADDR]->n.name_class == PT
     || ins->operands[CALL_OP_ADDR]->n.name_class == CP ) {
        entry |= ATTR_FAR;
        opcode = M_CJILONG;
    } else {
        opcode = M_CJINEAR;
    }
    ReFormat( entry );
    LayOpword( opcode );
    LayModRM( ins->operands[CALL_OP_ADDR] );
    _Emit;
}


void    GenRCall( instruction *ins )
/*****************************************
    generate a call to a register (eg: call eax)
*/
{
    name                *op;

    if( ins->flags.call_flags & CALL_INTERRUPT ) {
        Pushf();
    }
    ReFormat( (ins->flags.call_flags & CALL_POPS_PARMS) != 0 ? OC_CALLI | ATTR_POP : OC_CALLI );
    LayOpword( M_CJINEAR );
    op = ins->operands[CALL_OP_ADDR];
    LayRegRM( op->r.reg );
    _Emit;
}


static  void    Pushf( void ) {
/***********************/

    LayOpbyte( 0x9c ); /* PUSHF*/
    _Emit;
    _Code;
}


void    GenSelEntry( bool starts )
/*******************************************
    dump a queue that a select table is starting/ending ("starts") into
    the code segment queue.
*/
{
    any_oc      oc;

    oc.oc_select.hdr.class = OC_INFO + INFO_SELECT;
    oc.oc_select.hdr.reclen = sizeof( oc_select );
    oc.oc_select.hdr.objlen = 0;
    oc.oc_select.starts = starts;
    InputOC( &oc );
}


void    Gen1ByteValue( byte value )
/****************************************
    drop an 8 bit integer into the queue.
*/
{
    _Code;
    AddByte( value );
    _Emit;
}


void    Gen2ByteValue( unsigned_16 value ) {
/***************************************************
    drop a 16 bit integer into the queue.
*/

    _Code;
    AddByte( value & 0xFF );
    AddByte( ( value >> 8 ) & 0xFF );
    _Emit;
}


void    Gen4ByteValue( unsigned_32 value ) {
/***************************************************
    drop a 32 bit integer into the queue.
*/

    _Code;
    AddByte( value & 0xFF );
    AddByte( ( value >> 8 ) & 0xFF );
    AddByte( ( value >> 16 ) & 0xFF );
    AddByte( ( value >> 24 ) & 0xFF );
    _Emit;
}


void    GenCodePtr( pointer label ) {
/********************************************
    Dump a near reference to a label into the code segment.
*/

    CodeHandle( OC_LREF, TypeAddress( TY_NEAR_CODE_PTR )->length, label );
}


void    GenCallLabel( pointer label )
/************************************
    generate a call to a label within the procedure (near call)
*/
{
    DoCall( label, false, false, false );
}


void    GenLabelReturn( void ) {
/*********************************
    generate a return from CALL_LABEL instruction (near return)
*/

    GenReturn( 0, false, false );
}

void    GenReturn( int pop, bool is_long, bool iret ) {
/**************************************************************
    Generate a return instruction
*/

    any_oc      oc;

    oc.oc_ret.hdr.class = OC_RET;
    if( pop != 0 ) {
        oc.oc_ret.hdr.class |= ATTR_POP;
    }
    if( is_long ) {
        oc.oc_ret.hdr.class |= ATTR_FAR;
    }
    if( iret ) {
        oc.oc_ret.hdr.class |= ATTR_IRET;
    }
    oc.oc_ret.hdr.reclen = sizeof( oc_ret );
    oc.oc_ret.hdr.objlen = 1;
    if( pop != 0 ) {
        oc.oc_ret.hdr.objlen += 2;
    }
    oc.oc_ret.ref = NULL;
    oc.oc_ret.pops = pop;
    InputOC( &oc );
}

void    GenMJmp( instruction *ins ) {
/********************************************
    Generate a jump indirect through memory instruction.
*/

    label_handle    lbl;
    name            *base;

    if( ins->head.opcode != OP_SELECT && _IsTargetModel( BIG_CODE ) ) {
        ReFormat( OC_JMPI | ATTR_FAR );
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

void    GenRJmp( instruction *ins ) {
/********************************************
    Generate a jump to register instruction (eg: jmp eax)
*/

    JumpReg( ins, ins->operands[0] );
}


static  void    JumpReg( instruction *ins, name *reg_name ) {
/************************************************************
    Generate a jump to register instruction (eg: jmp eax)
*/

    hw_reg_set  regs;

    ins = ins;
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
        GenReturn( 0, true, false );
    } else {
        ReFormat( OC_JMPI );
        LayOpword( M_CJINEAR );
        LayRegRM( regs );
        _Emit;
    }
}

static  void    DoCodeBytes( const void *src, byte_seq_len len, oc_class class ) {
/*******************************************************************************
    Dump bytes "src" directly into the queue, for length "len".
*/
    any_oc    *oc;
    uint      addlen;

    oc = CGAlloc( offsetof( oc_entry, data ) + MAX_OBJ_LEN );
    oc->oc_entry.hdr.class = class;
    oc->oc_entry.hdr.reclen = offsetof( oc_entry, data ) + MAX_OBJ_LEN;
    oc->oc_entry.hdr.objlen = MAX_OBJ_LEN;
    addlen = 0;
    if( class == OC_IDATA ) {
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
