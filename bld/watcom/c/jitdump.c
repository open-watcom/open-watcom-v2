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
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "watcom.h"
#include "jit.h"
#include "dis.h"

static const char       *archTypes[ JIT_ARCH_LAST ] = {
        "JIT_ARCH_I86",
        "JIT_ARCH_SPARC" };

static const char       *relocTypes[ JIT_RELOC_LAST ] = {
    #define _str( x ) #x
    #define JitReloc(x) _str( JIT_RELOC_##x ),
    JitRelocs
    #undef JitReloc
    #undef _str
};

static const char       *helperTypes[ JIT_RTC_end ] = {
    #define JitRtc(x) #x,
    JitRtcs
    #undef JitRtc
};

static void myPrintf( void (*output)( char ), char *fmt, ... )
{
    va_list     args;
    char        buffer[ 1024 ];
    char        *ptr;

    va_start( args, fmt );
    vsprintf( &buffer[ 0 ], fmt, args );
    va_end( args );
    ptr = &buffer[ 0 ];
    while( *ptr ) {
        output( *ptr++ );
    }
}

static void FmtHexNum( char *buff, unsigned prec, unsigned long value )
{
    sprintf( buff, "0x%*.*lx", prec, prec, value );
}

static void FmtSizedHexNum( char *buff, dis_dec_ins *ins, unsigned op_num )
{
    unsigned            size;
    unsigned            i;
    unsigned            len;

    static const unsigned long mask[] = {
        0x00000000, 0x000000ff, 0x0000ffff, 0x00ffffff, 0xffffffff
    };

    size = 0;
    for( i = 0; i < ins->num_ops; ++i ) {
        switch( ins->op[i].ref_type ) {
        case DRT_X86_BYTE:
            len = 1;
            break;
        case DRT_X86_WORD:
            len = 2;
            break;
        case DRT_X86_DWORD:
        case DRT_X86_DWORDF:
            len = 4;
            break;
        default:
            len = 0;
            break;
        }
        if( len > size ) size = len;
    }
    if( size == 0 ) size = 4;
    FmtHexNum( buff, size * 2, mask[size] & ins->op[op_num].value );
}

typedef struct {
    char const  *start;
    int         offset;
    int         length;
    dis_cpu     cpu;
}code_buff;

extern dis_return DisCliGetData( void *d, unsigned off, size_t size, void *data )
{
    code_buff   *header;

    header = (code_buff *)d;
    if( ( off + size + header->offset ) > header->length ) {
        size = header->length - ( header->offset + off );
    }
    memcpy( data, header->start + header->offset + off, size );
    return( DR_OK );
}

extern unsigned DisCliValueString( void *d, dis_dec_ins *ins, unsigned op_num, char *buff )
{
    dis_operand         *op;
    code_buff   *header;

    header = (code_buff *)d;
    buff[0] = '\0';
    op = &ins->op[op_num];
    switch( op->type & DO_MASK ) {
    case DO_RELATIVE:
    case DO_MEMORY_REL:
    case DO_ABSOLUTE:
    case DO_MEMORY_ABS:
        switch( op->type & DO_MASK ) {
            case DO_RELATIVE:
            case DO_MEMORY_REL:
                op->value += header->offset;
                break;
        }
        if( op->base == DR_NONE && op->index == DR_NONE ) {
            FmtSizedHexNum( buff, ins, op_num );
        } else if( op->value > 0 ) {
            FmtHexNum( buff, 0, op->value );
        } else if( op->value < 0 ) {
            buff[0] = '-';
            FmtHexNum( &buff[1], 0, -op->value );
        }
        break;
    case DO_IMMED:
        // FmtSizedHexNum( buff, ins, op_num );
        if( op->value < 0 ) {
            buff[0] = '-';
            FmtHexNum( &buff[1], 0, -op->value );
        } else {
            FmtHexNum( buff, 0, op->value );
        }
        break;
    }
    return( strlen( buff ) );
}

static void dumpSection( void (*output)( char ), char *type, jit_data_sec *section )
{
    myPrintf( output, "\t%s.offset\t=%d\n", type, section->offset );
    myPrintf( output, "\t%s.size  \t=%d\n", type, section->size );
}

static void dumpOpcodes( void (*output)( char ), char const *what, unsigned size )
{
    unsigned            i;

    myPrintf( output, "\t" );
    for( i = 0; i < size; i++ ) {
        myPrintf( output, "%2.2x ", what[i] );
    }
    while( i < 10 ) {
        myPrintf( output, "   " );
        i++;
    }
}

#define MAX_INS_NAME    24
#define MAX_OBJ_NAME    1024

static void dumpCodeBuff( void (*output)( char ),
                          code_buff *buff )
{
    dis_handle          handle;
    dis_dec_ins         ins;
    char                name[ MAX_INS_NAME ];
    char                ops[ MAX_OBJ_NAME + 24 ];

    DisInit( buff->cpu, &handle );
    while( buff->offset < buff->length ){
        DisDecodeInit( &handle, &ins );
        if( buff->cpu == DISCPU_x86 ) {
            ins.flags |= DIF_X86_USE32_FLAGS;
        }
        DisDecode( &handle, buff, &ins );
        DisFormat( &handle, buff, &ins, DFF_AXP_SYMBOLIC_REG|DFF_PSEUDO, &name, &ops );
        myPrintf( output, "\t%4.4x:", buff->offset );
        dumpOpcodes( output, buff->start+buff->offset, ins.size );
        myPrintf( output, "\t%s\t%s\n", name, ops );
        buff->offset += ins.size;
    }
    DisFini( &handle );
}

static void dumpCode( void (*output)( char ), jit_code_str *header )
{
    code_buff           buff;

    buff.cpu = DISCPU_x86;
    if( header->arch == JIT_ARCH_SPARC ) {
        buff.cpu = DISCPU_sparc;
    }
    buff.offset = 0;
    buff.length = header->code.size;
    buff.start = (char *)header+header->code.offset;
    dumpCodeBuff( output, &buff );
}

extern void DumpCodeStr(
             char const *buffer,
             unsigned   len,
             void       (*output)( char ) )
{
    code_buff           buff;

    buff.cpu = DISCPU_x86;
    buff.offset = 0;
    buff.length = len;
    buff.start = buffer;
    dumpCodeBuff( output, &buff );
}

static void dumpData( void (*output)( char ), jit_code_str *header )
{
    int offset;
    char *curr;
    curr = (char *)header + header->data.offset;
    for( offset = 0; offset < header->data.size; offset += 4 ) {
        myPrintf( output, "\t%4.4x:%4.4x\n", offset, *(long*)curr );
        curr+= 4;
    }
}
static void dumpOneReloc( void (*output)( char ), jit_code_str *header, jit_reloc *reloc )
{
    assert( reloc->type < JIT_RELOC_LAST );
    myPrintf( output, "\ttype:   %d (%s)\n", reloc->type, relocTypes[ reloc->type ] );
    myPrintf( output, "\toffset: 0x%4.4x\n", reloc->location - header->code.offset );
    myPrintf( output, "\ttarget: " );
    switch( reloc->type ) {
    case JIT_RELOC_METHOD_OFFSET:
    case JIT_RELOC_FIELD_OFFSET:
    case JIT_RELOC_METHOD_ADDR:
    case JIT_RELOC_FIELD_ADDR:
        myPrintf( output, "cp_index=%d\n", reloc->u.cp_index );
        break;
    case JIT_RELOC_DATA_REF:
        myPrintf( output, "addend=%d\n", reloc->u.addend );
        break;
    case JIT_RELOC_HELPER_FUNC:
        myPrintf( output, "helper_func=%s\n",helperTypes[ reloc->u.helper_index] );
        break;
    case JIT_RELOC_RT_FUNC:
        myPrintf( output, "rt_func=%d\n", reloc->u.rt_index );
        break;
    case JIT_RELOC_METHOD_HANDLE:
        myPrintf( output, "method=%d\n", reloc->u.cp_index );
    case JIT_RELOC_CLASS_HANDLE:
    case JIT_RELOC_INTERFACE_HANDLE:
        myPrintf( output, "class=%d\n", reloc->u.cp_index );
        break;
    }
    myPrintf( output, "\t*******************************\n" );
}

static void dumpRelocs( void (*output)( char ), jit_code_str *header )
{
    int                 i;
    int                 count;
    jit_reloc           *curr;

    curr = (jit_reloc *)((char *)header + header->relocs.offset);
    count = header->relocs.size / sizeof( jit_reloc );
    for( i = 0; i < count; i++, curr++ ) {
        dumpOneReloc( output, header, curr );
    }
}

static void dumpEhTable( void (*output)( char ), jit_code_str *header )
{
    int                 i;
    int                 count;
    jit_eh_table       *curr;
    int offset;

    curr = (jit_eh_table *)((char *)header + header->eh_table.offset);
    count = header->eh_table.size / sizeof( jit_eh_table );
    offset = 0;
    myPrintf( output, "\toffset:beg end handler class\n" );
    for( i = 0; i < count; i++, curr++ ) {
        myPrintf( output, "\t  %4.4x:%4.4x %4.4x %4.4x    %4.4x\n", offset,
            curr->beg,curr->end,curr->handler,curr->cp_index );
        offset += sizeof( jit_eh_table );
    }
}

static void dumpPData( void (*output)( char ), jit_code_str *header )
{
    jit_pdata           *pdata;

    pdata = (jit_pdata *)( (char *) header + header->pdata.offset );
    myPrintf( output, "\troutine beg    = %4.4x\n", pdata->rtn_beg );
    myPrintf( output, "\tprolog end     = %4.4x\n", pdata->pro_end );
    myPrintf( output, "\tepilog beg     = %4.4x\n", pdata->epi_beg );
    myPrintf( output, "\troutine end    = %4.4x\n", pdata->rtn_end );
    myPrintf( output, "\tparms          = %4.4x\n", pdata->parms );
    myPrintf( output, "\tlocals         = %4.4x\n", pdata->locals );
}

void DumpJitCodeStr( const char *buffer, unsigned len, void (*output)( char ) )
{
    jit_code_str        *header;

    header = (jit_code_str *)buffer;
    switch( header->arch ) {
    case JIT_ARCH_I86:
    case JIT_ARCH_SPARC:
        myPrintf( output, "arch: %d (%s)\n", header->arch, archTypes[ header->arch ] );
        dumpSection( output, "code", &header->code );
        dumpSection( output, "data", &header->data );
        dumpSection( output, "eh_table", &header->eh_table );
        dumpSection( output, "pdata", &header->pdata );
        dumpSection( output, "relocs", &header->relocs );
        myPrintf( output, "code:\n" );
        dumpCode( output, header );
        myPrintf( output, "data:\n" );
        dumpData( output, header );
        myPrintf( output, "eh_table:\n" );
        dumpEhTable( output, header );
        myPrintf( output, "pdata:\n" );
        dumpPData( output, header );
        myPrintf( output, "relocs:\n" );
        dumpRelocs( output, header );
        break;
    default:
        myPrintf( output, "not a valid code string: arch = %d\n", header->arch );
    }
}
