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
* Description:  CPU independent instruction decoding core.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "dis.h"
#include "distypes.h"
#include "distbls.gh"

#include "clibext.h"


#if DISCPU & DISCPU_axp
extern const dis_cpu_data       AXPData;
#endif
#if DISCPU & DISCPU_ppc
extern const dis_cpu_data       PPCData;
#endif
#if DISCPU & DISCPU_x86
extern const dis_cpu_data       X86Data;
#endif
#if DISCPU & DISCPU_x64
extern const dis_cpu_data       X64Data;
#endif
#if DISCPU & DISCPU_jvm
extern const dis_cpu_data       JVMData;
#endif
#if DISCPU & DISCPU_sparc
extern const dis_cpu_data       SPARCData;
#endif
#if DISCPU & DISCPU_mips
extern const dis_cpu_data       MIPSData;
#endif

long DisSEX( unsigned long v, unsigned bit )
{
    unsigned long chk;

    chk = 1UL << bit;
    if( v & chk ) {
        v |= ~(chk - 1);
    }
    return( v );
}

#define LENGTH_BIT      0x80

size_t DisGetString( size_t index, char *buff, bool to_upper )
{
    size_t              len;
    size_t              i;
    const unsigned char *src;
    int                 c;

    src = &DisStringTable[index];
    len = *src++ & ~LENGTH_BIT;
    i = len;
    for( ;; ) {
        if( i == 0 ) break;
        c = *src++;
        if( !(c & LENGTH_BIT) ) {
            if( to_upper )
                c = toupper( c );
            *buff++ = (char)c;
            --i;
        }
    }
    *buff = '\0';
    return( len );
}

dis_return DisInit( dis_cpu cpu, dis_handle *h, bool swap_bytes )
// Perform all setup required
{
    h->cpu = cpu;
    switch( cpu ) {
#if DISCPU & DISCPU_axp
    case DISCPU_axp:
        h->d = &AXPData;
        break;
#endif
#if DISCPU & DISCPU_ppc
    case DISCPU_ppc:
        h->d = &PPCData;
        break;
#endif
#if DISCPU & DISCPU_x86
    case DISCPU_x86:
        h->d = &X86Data;
        break;
#endif
#if DISCPU & DISCPU_x64
    case DISCPU_x64:
        h->d = &X64Data;
        break;
#endif
#if DISCPU & DISCPU_jvm
    case DISCPU_jvm:
        h->d = &JVMData;
        break;
#endif
#if DISCPU & DISCPU_sparc
    case DISCPU_sparc:
        h->d = &SPARCData;
        break;
#endif
#if DISCPU & DISCPU_mips
    case DISCPU_mips:
        h->d = &MIPSData;
        break;
#endif
    default:
        return( DR_FAIL );
    }
    if( h->d->range == NULL ) return( DR_FAIL );
    h->need_bswap = swap_bytes;
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
    case DISCPU_mips:
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
    int                     curr;
    const dis_range         *table;
    dis_return              dr;
    unsigned char           idx;
    unsigned                start;
    dis_handler_return      hr;
    int                     page;
    int const               *pos;
    int                     offs;

    start = 0;
    curr  = 0;
    table = h->d->range;
    for( ;; ) {
        dr = DisCliGetData( d, start, sizeof( ins->opcode ), &ins->opcode );
        if( dr != DR_OK ) {
            ins->num_ops = 0;   /* must reset num_ops before returning! */
            return( dr );
        }
        h->d->preproc_hook( h, d, ins );
        page = 0;
        for( pos = h->d->range_pos ; *pos != -1 ; ++pos, ++page ) {
            if( h->d->decode_check( page, ins ) != DHR_DONE )
                continue;
            offs = *pos;
            curr = 0;
            for( ;; ) {
                idx = (ins->opcode >> table[curr+offs].shift) & table[curr+offs].mask;
                curr = DisSelectorTable[idx + table[curr+offs].index];
                if( curr >= 0 ) break;
                curr = -curr;
            }
            if( (DisInstructionTable[curr].mask & ins->opcode) == DisInstructionTable[curr].opcode ) {
                break;
            }
        }
        if( *pos == -1 ) {
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
    return( &dst[ DisGetString( DisRegisterTable[reg], dst, (flags & DFF_REG_UP) != 0 ) ] );
}

char *DisOpFormat( dis_handle *h, void *d, dis_dec_ins *ins, dis_format_flags flags,
                        unsigned i, char *p, size_t buff_len )
{
    const char chLbrac = ( h->cpu == DISCPU_sparc ) ? '[' : '(';
    const char chRbrac = ( h->cpu == DISCPU_sparc ) ? ']' : ')';
    char       *end = p + buff_len;


    // BartoszP 23.10.2005
    // for SPARC architecture DO_IMMED value could not be emited before
    // other arguments (registers) so we should dissassembly like:
    //   [ %reg + offset ]
    // not like x86:
    //   offset[reg]
    if( h->cpu != DISCPU_sparc ) {
        p += DisCliValueString( d, ins, i, p, end - p );
    }
    switch( ins->op[i].type & DO_MASK ) {
    case DO_IMMED:
        if( h->cpu == DISCPU_sparc ) {
            p += DisCliValueString( d, ins, i, p, end - p );
        }
        break;
    case DO_REG:
        p = DisAddReg( ins->op[i].base, p, flags );
        break;
    case DO_ABSOLUTE:
    case DO_RELATIVE:
        if( h->cpu == DISCPU_sparc ) {
            p += DisCliValueString( d, ins, i, p, end - p );
            break;
        }
    case DO_MEMORY_ABS:
    case DO_MEMORY_REL:
        if( ins->op[i].base != DR_NONE || ins->op[i].index != DR_NONE ) {
            *p++ = chLbrac;
            p = DisAddReg( ins->op[i].base, p, flags );
            if( h->cpu != DISCPU_sparc ) {
                if( ins->op[i].index != DR_NONE ) {
                    *p++ = ',';
                    p = DisAddReg( ins->op[i].index, p, flags );
                    if( ins->op[i].scale != 1 ) {
                        *p++ = ',';
                        *p++ = '0' + ins->op[i].scale;
                    }
                }
            } else {
                // SPARC stuff
                if( ins->op[i].index != DR_NONE
                     && ( DO_MEMORY_ABS == (ins->op[i].type & DO_MASK ) ) ) {
                    // ASI stuff
                } else if( ins->op[i].index == DR_NONE
                            && ins->op[i].value != 0 ) {
                    // always add offset to base reg
                    // offset > 0 dissassembles like %reg + offset
                    // offset < 0 dissassembles like %reg + -offset
                    *p++ = ' '; *p++ = '+'; *p++ = ' ';
                    p += DisCliValueString( d, ins, i, p, end - p );
                } else {
                    // who knows ??
                    *p++ = '/'; *p++ = '*';
                    *p++ = '?'; *p++ = '?'; *p++ = '?';
                    *p++ = '*'; *p++ = '/';
                }
            }
            *p++ = chRbrac;
        }
        break;
    }
    return( p );
}

dis_return DisFormat( dis_handle *h, void *d, dis_dec_ins *ins_p,
                dis_format_flags flags, char *name, size_t name_len, char *opers, size_t opers_len )
// Format up an instruction name or operands or both
{
    unsigned    i;
    dis_dec_ins ins;
    char        *p;
    size_t      len;
    char        *end;

    name_len = name_len;
    ins = *ins_p;       /* so we can fiddle it around */

    if( name != NULL ) name[0] = '\0';
    len = h->d->ins_hook( h, d, &ins, flags, name );
    if( name != NULL ) {
        if( len == 0 ) {
            DisGetString( DisInstructionTable[ins.type].name, name, FALSE );
        }
        p = &name[ strlen( name ) ];
        h->d->flag_hook( h, d, &ins, flags, p );
        if( flags & DFF_INS_UP ) {
            strupr( name );
        }
    }
    if( opers != NULL ) {
        p = opers;
        end = opers + opers_len;
        for( i = 0; i < ins.num_ops; ++i ) {
            if( !(ins.op[i].type & DO_HIDDEN) ) {
                if( p != opers )
                    *p++ = ',';
                len = h->d->op_hook( h, d, &ins, flags, i, p, end - p );
                p += len;
                if( len == 0 ) {
                    p = DisOpFormat( h, d, &ins, flags, i, p, end - p );
                }
            }
        }
        if( p != opers )
            *p++ = ' ';
        len = h->d->post_op_hook( h, d, &ins, flags, i, p, end - p );
        if( len ) {
            p += len;
        } else if( p != opers ) {
            --p;
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
