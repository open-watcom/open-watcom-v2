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
#include <ctype.h>
#include "distypes.h"
#include "dis.h"

extern const dis_cpu_data       AXPData;
extern const dis_cpu_data       PPCData;
extern const dis_cpu_data       X86Data;
extern const dis_cpu_data       JVMData;
extern const dis_cpu_data       SPARCData;

extern const char               DisStringTable[];
extern const dis_selector       DisSelectorTable[];
extern const dis_ins_descript   DisInstructionTable[];
extern const unsigned short     DisRegisterTable[];
extern const unsigned short     DisRefTypeTable[];


long SEX( unsigned long v, unsigned bit )
{
    unsigned long chk;

    chk = 1UL << bit;
    if( v & chk ) {
        v |= ~(chk - 1);
    }
    return( v );
}

#define LENGTH_BIT      0x80

unsigned DisGetString( unsigned index, char *buff, int upper )
{
    unsigned            len;
    unsigned            i;
    const unsigned char *src;
    unsigned char       c;

    src = &DisStringTable[index];
    len = *src++ & ~LENGTH_BIT;
    i = len;
    for( ;; ) {
        if( i == 0 ) break;
        c = *src++;
        if( !(c & LENGTH_BIT) ) {
            if( upper ) c = toupper( c );
            *buff++ = c;
            --i;
        }
    }
    *buff = '\0';
    return( len );
}

dis_handler_return DisDummyHandler( dis_handle *h, void *d, dis_dec_ins *ins )
{
    return( DHR_INVALID );
}

dis_return DisInit( dis_cpu cpu, dis_handle *h )
// Perform all setup required
{
    h->cpu = cpu;
    switch( cpu ) {
    case DISCPU_axp:
        h->d = &AXPData;
        break;
    case DISCPU_ppc:
        h->d = &PPCData;
        break;
    case DISCPU_x86:
        h->d = &X86Data;
        break;
    case DISCPU_jvm:
        h->d = &JVMData;
        break;
    case DISCPU_sparc:
        h->d = &SPARCData;
        break;
    default:
        return( DR_FAIL );
    }
    if( h->d->range == NULL ) return( DR_FAIL );
    return( DR_OK );
}

void DisFini( dis_handle *h )
// Perform all shutdown required
{
    h = h;
}

void DisDecodeInit( dis_handle *h, dis_dec_ins *ins )
// Initialize a decoded instruction structure
{
    memset( ins, 0, sizeof( *ins ) );
    ins->num_ops = MAX_NUM_OPERANDS + 1;
    switch( h->cpu ) {
    case DISCPU_axp:
    case DISCPU_ppc:
    case DISCPU_sparc:
        ins->size = sizeof( unsigned_32 );
        break;
    }
}

static void BadOpcode( dis_handle *h, dis_dec_ins *ins )
{
    DisDecodeInit( h, ins );
    ins->type = DI_INVALID;
    ins->size = h->d->inssize_inc;
    ins->num_ops = 0;
}

dis_return DisDecode( dis_handle *h, void *d, dis_dec_ins *ins )
// Decode an instruction
{
    int                         curr;
    const dis_range             *table;
    dis_return                  dr;
    unsigned                    idx;
    unsigned                    start;
    dis_handler_return          hr;

    table = h->d->range;
    start = 0;
    for( ;; ) {
        dr = DisCliGetData( d, start, sizeof( ins->opcode ), &ins->opcode );
        if( dr != DR_OK ) return( dr );
        curr = 0;
        for( ;; ) {
            idx = (ins->opcode >> table[curr].shift) & table[curr].mask;
            curr = DisSelectorTable[idx + table[curr].index];
            if( curr >= 0 ) break;
            curr = -curr;
        }
        if( (DisInstructionTable[curr].mask & ins->opcode) != DisInstructionTable[curr].opcode ) {
            BadOpcode( h, ins );
            break;
        }
        ins->type = curr;
        hr = DisInstructionTable[curr].handler( h, d, ins );
        if( hr == DHR_DONE ) break;
        if( hr == DHR_INVALID ) {
            BadOpcode( h, ins );
            break;
        }
        start = ins->size;
    }
    if( ins->num_ops > MAX_NUM_OPERANDS ) {
        /* nobody's set the number of operands */
        for( idx = 0; idx < MAX_NUM_OPERANDS; ++idx ) {
            if( ins->op[idx].type == DO_NONE ) break;
        }
        ins->num_ops = idx;
    }
    return( DR_OK );
}

char *DisAddReg( dis_register reg, char *dst, dis_format_flags flags )
{
    return( &dst[ DisGetString( DisRegisterTable[reg], dst,
                        (flags & DFF_REG_UP) ) ] );
}

char *DisOpFormat( void *d, dis_dec_ins *ins, dis_format_flags flags,
                        unsigned i, char *p )
{
    p += DisCliValueString( d, ins, i, p );
    switch( ins->op[i].type & DO_MASK ) {
    case DO_REG:
        p = DisAddReg( ins->op[i].base, p, flags );
        break;
    case DO_ABSOLUTE:
    case DO_RELATIVE:
    case DO_MEMORY_ABS:
    case DO_MEMORY_REL:
        if( ins->op[i].base != DR_NONE || ins->op[i].index != DR_NONE ) {
            *p++ = '(';
            p = DisAddReg( ins->op[i].base, p, flags );
            if( ins->op[i].index != DR_NONE ) {
                *p++ = ',';
                p = DisAddReg( ins->op[i].index, p, flags );
                if( ins->op[i].scale != 1 ) {
                    *p++ = ',';
                    *p++ = '0' + ins->op[i].scale;
                }
            }
            *p++ = ')';
        }
        break;
    }
    return( p );
}

dis_return DisFormat( dis_handle *h, void *d, dis_dec_ins *ins_p,
                dis_format_flags flags, char *name, char *opers )
// Format up an instruction name or operands or both
{
    unsigned    i;
    dis_dec_ins ins;
    char        *p;
    unsigned    len;

    ins = *ins_p;       /* so we can fiddle it around */

    if( name != NULL ) name[0] = '\0';
    len = h->d->ins_hook( h, d, &ins, flags, name );
    if( name != NULL ) {
        if( len == 0 ) {
            DisGetString( DisInstructionTable[ins.type].name, name, 0 );
        }
        p = &name[ strlen( name ) ];
        h->d->flag_hook( h, d, &ins, flags, p );
        if( flags & DFF_INS_UP ) {
            strupr( name );
        }
    }
    if( opers != NULL ) {
        p = opers;
        for( i = 0; i < ins.num_ops; ++i ) {
            if( !(ins.op[i].type & DO_HIDDEN) ) {
                if( p != opers ) *p++ = ',';
                len = h->d->op_hook( h, d, &ins, flags, i, p );
                p += len;
                if( len == 0 ) {
                    p = DisOpFormat( d, &ins, flags, i, p );
                }
            }
        }
        *p = '\0';
    }
    return( DR_OK );
}

unsigned DisInsNameMax( dis_handle *h )
// Return how long (character length) the longest instruction name is
{
    return( *(h->d->max_insnamep) );
}

unsigned DisInsSizeInc( dis_handle *h )
// Return the number of bytes an instruction can grow by
// (LCM of instruction sizes).
{
    return( h->d->inssize_inc );
}
