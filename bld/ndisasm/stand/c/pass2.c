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


#include <stdio.h>
#include <string.h>
#include "global.h"
#include "dis.h"
#include "pass2.h"
#include "buffer.h"
#include "srcmix.h"
#include "formasm.h"
#include "init.h"
#include "main.h"
#include "print.h"

#define COMMENT_TAB_POS 4
#define OPS_REP_TAB_POS 10
#define DIS_X86_SEG_OR  ( DIF_X86_CS | DIF_X86_DS | DIF_X86_ES | DIF_X86_FS | \
                          DIF_X86_GS | DIF_X86_SS )

struct pass2 {
    orl_sec_offset      loop;
    orl_sec_size        size;
    num_errors          disassembly_errors;
    ref_entry           r_entry;
};

extern wd_options               Options;
extern char                     LabelChar;
extern dis_handle               DHnd;
extern bool                     Prettify;
extern section_list_struct      Sections;
extern hash_table               HandleToSectionTable;
extern hash_table               SymbolToLabelTable;
extern dis_format_flags         DFormat;
extern bool                     source_mix;
extern char *                   CommentString;

static orl_sec_offset   routineBase = 0;
static orl_sec_size     routineSize = 0;

dis_return DisCliGetData( void *d, unsigned off, unsigned size, void *buff )
{
    //NYI: need to handle going beyond the end of the section
    memcpy( buff, (char *)d + off, size );
    return( DR_OK );
}

static label_entry handleLabels( char *sec_name, orl_sec_offset offset, label_entry l_entry,
                        orl_sec_size size )
// handle any labels at this offset
{
    for( ; l_entry != NULL && l_entry->offset <= offset; l_entry = l_entry->next ) {
        switch( l_entry->type ) {
        case LTYP_SECTION:
        case LTYP_NAMED:
            if( strcmp( l_entry->label.name, sec_name ) == 0 ) continue;
            break;
        case LTYP_FUNC_INFO:
            continue;
        }
        if( !( DFormat & DFF_ASM ) ) {
            if( l_entry->type == LTYP_NAMED && offset != 0 &&
                l_entry->binding == ORL_SYM_BINDING_GLOBAL ) {
                routineSize = offset - routineBase;
                BufferConcatNL();
                BufferMsg( ROUTINE_SIZE );
                BufferStore(" %d ", routineSize );
                BufferMsg( BYTES );
                BufferConcat(",    ");
                BufferMsg( ROUTINE_BASE );
                BufferStore(" %s + %04X\n\n", sec_name, routineBase );
                routineBase = offset;
            }
            PrintLinePrefix( NULL, offset, size, 1, 0 );
        }
        if( l_entry->type == LTYP_NAMED || l_entry->type == LTYP_SECTION ) {
            BufferStore( "%s:\n", l_entry->label.name );
        } else {
            BufferStore( "%c$%d:\n", LabelChar, l_entry->label.number );
        }
        BufferPrint();
    }
    return( l_entry );
}

static return_val referenceString( ref_entry r_entry, orl_sec_size size,
                                   char *ext_pref, char *int_pref, char *post,
                                   char *buff, ref_flags flags )
{
    label_entry         l_entry;
    char                *sep = ":";
    char                *frame;

    frame = r_entry->frame;
    if( !frame || ( flags & RFLAG_NO_FRAME ) ) {
        frame = "";
        sep = "";
    }

    l_entry = r_entry->label;
    if( Options & METAWARE_COMPATIBLE || (ext_pref[0]==0 && int_pref[0]==0) ) {
        switch( l_entry->type ) {
            case LTYP_UNNAMED:
                sprintf( buff, "%s%s%c$%d%s", frame, sep, LabelChar,
                         l_entry->label.number, post );
                break;
            default:
                sprintf( buff, "%s%s%s%s", frame, sep, l_entry->label.name,
                         post );
                break;
        }
    } else {
        switch( l_entry->type ) {
            case LTYP_EXTERNAL_NAMED:
                sprintf( buff, "%s%s%s%s", ext_pref, frame, sep,
                         l_entry->label.name );
                break;
            case LTYP_NAMED:
            case LTYP_SECTION:
            case LTYP_GROUP:
                sprintf( buff, "%s%s%s%s", int_pref, frame, sep,
                         l_entry->label.name );
                break;
            default:
                sprintf( buff, "%s%s%s%c$%d", int_pref, frame, sep,
                         LabelChar, l_entry->label.number );
                if( l_entry->offset > size ) {
                    return( ERROR );
                }
                break;
        }
    }
    return( OKAY );
}

unsigned HandleAReference( dis_value value, int ins_size, ref_flags flags,
                           orl_sec_offset offset, orl_sec_size sec_size,
                           ref_entry * r_entry, char *buff )
// handle any references at this offset
{
    return_val          error;
    dis_value           nvalue;
    char                *p;

    buff[0] = '\0';
    while( *r_entry && (*r_entry)->offset == offset ) {
        if( (*r_entry)->no_val ) {
            nvalue = 0;
        } else if( (*r_entry)->addend ) {
            nvalue = HandleAddend( *r_entry );
        } else {
            nvalue = value;
        }
        switch( (*r_entry)->type ) {
        case ORL_RELOC_TYPE_JUMP:
        case ORL_RELOC_TYPE_REL_21_SH:
            error = referenceString( *r_entry, sec_size, "j^", "", "",
                                     buff, flags );
            if( error != OKAY ) {
                // label is defined to be beyond the boundaries of the section!
                if( !(DFormat & DFF_ASM) ){
                    BufferStore("\t     %04X", offset );
                    BufferAlignToTab( COMMENT_TAB_POS );
                } else {
                    BufferConcat("\t" );
                }
                BufferConcat( CommentString );
                BufferMsg( LABEL_BEYOND_SECTION );
                BufferConcatNL();
                BufferPrint();
                *r_entry = (*r_entry)->next;
                return( 0 );
            }
            *r_entry = (*r_entry)->next;
            continue; // Don't print addend
            break;
        case ORL_RELOC_TYPE_SEC_REL:
            referenceString( *r_entry, sec_size, "s^", "s^", "@s", buff,
                             flags );
            break;
        case ORL_RELOC_TYPE_HALF_HI:
            referenceString( *r_entry, sec_size, "h^", "h^", "@h", buff,
                             flags );
            break;
        case ORL_RELOC_TYPE_HALF_HA:
            referenceString( *r_entry, sec_size, "h^", "h^", "@ha", buff,
                             flags );
            break;
        case ORL_RELOC_TYPE_HALF_LO:
            referenceString( *r_entry, sec_size, "l^", "l^", "@l", buff,
                             flags );
            break;
        case ORL_RELOC_TYPE_REL_14:
        case ORL_RELOC_TYPE_REL_24:
        case ORL_RELOC_TYPE_WORD_14:
        case ORL_RELOC_TYPE_WORD_24:
            nvalue &= ~0x3;
        case ORL_RELOC_TYPE_WORD_16:
        case ORL_RELOC_TYPE_WORD_32:
        case ORL_RELOC_TYPE_WORD_64:
            if( ( (*r_entry)->label->type != LTYP_GROUP ) &&
              ( flags & RFLAG_IS_IMMED ) && IsMasmOutput() ) {
                referenceString( *r_entry, sec_size, "offset ", "offset ",
                                 "", buff, flags );
            } else {
                referenceString( *r_entry, sec_size, "", "", "", buff,
                                 flags );
            }
            break;
        case ORL_RELOC_TYPE_REL_16:
            if( IsIntelx86() ) {
                nvalue -= ins_size;
            }
            if( ( (*r_entry)->label->type != LTYP_GROUP ) &&
              ( flags & RFLAG_IS_IMMED ) && IsMasmOutput() ) {
                referenceString( *r_entry, sec_size, "offset ", "offset ",
                                 "", buff, flags );
            } else {
                referenceString( *r_entry, sec_size, "", "", "", buff,
                                 flags  );
            }
            break;
        case ORL_RELOC_TYPE_WORD_8:
        case ORL_RELOC_TYPE_WORD_16_SEG:
        case ORL_RELOC_TYPE_WORD_HI_8:
        case ORL_RELOC_TYPE_WORD_32_SEG:
            // Keep these seperate because they are OMF specific
            referenceString( *r_entry, sec_size, "", "", "", buff, flags );
            break;
        case ORL_RELOC_TYPE_SEGMENT:
            if( ( (*r_entry)->label->type != LTYP_GROUP ) &&
              ( flags & RFLAG_IS_IMMED ) && IsMasmOutput() ) {
                referenceString( *r_entry, sec_size, "seg ", "seg ", "",
                                 buff, flags );
            } else {
                referenceString( *r_entry, sec_size, "", "", "", buff,
                                 flags );
            }
            break;
        case ORL_RELOC_TYPE_REL_8:
        case ORL_RELOC_TYPE_REL_16_SEG:
        case ORL_RELOC_TYPE_REL_HI_8:
        case ORL_RELOC_TYPE_REL_32_SEG:
        case ORL_RELOC_TYPE_REL_32:
            // For some reason we add the instruction size to the value
            // of the displacement in a relative call and get a bad
            // offset, due to CORE implementation
            nvalue -= ins_size;
            if( GetMachineType() == ORL_MACHINE_TYPE_I386 && GetFormat() == ORL_ELF ) {
                // this is a little kluge because Brian's ELF files seem to have
                // -4 in the implicit addend for calls and such BBB May 09, 1997
                nvalue += 4;
            }
            referenceString( *r_entry, sec_size, "", "", "", buff, flags );
            break;
        case ORL_RELOC_TYPE_TOCREL_14:
            nvalue &= ~0x3;
        case ORL_RELOC_TYPE_TOCREL_16:
            referenceString( *r_entry, sec_size, "[toc]", "[toc]", "@toc",
                             buff, flags );
            break;
        case ORL_RELOC_TYPE_TOCVREL_14:
            nvalue &= ~0x3;
        case ORL_RELOC_TYPE_TOCVREL_16:
            referenceString( *r_entry, sec_size, "[tocv]", "[tocv]", "@tocv",
                             buff, flags );
            break;
        case ORL_RELOC_TYPE_GOT_16:
            referenceString( *r_entry, sec_size, "", "", "@got", buff, flags );
            break;
        case ORL_RELOC_TYPE_GOT_16_HI:
            referenceString( *r_entry, sec_size, "", "", "@got@h", buff, flags );
            break;
        case ORL_RELOC_TYPE_GOT_16_HA:
            referenceString( *r_entry, sec_size, "", "", "@got@ha", buff, flags );
            break;
        case ORL_RELOC_TYPE_GOT_16_LO:
            referenceString( *r_entry, sec_size, "", "", "@got@l", buff, flags );
            break;
        case ORL_RELOC_TYPE_PLTREL_24:
        case ORL_RELOC_TYPE_PLTREL_32:
        case ORL_RELOC_TYPE_PLT_32:
            referenceString( *r_entry, sec_size, "", "", "@plt", buff, flags );
            break;
        case ORL_RELOC_TYPE_PLT_16_HI:
            referenceString( *r_entry, sec_size, "", "", "@plt@h", buff, flags );
            break;
        case ORL_RELOC_TYPE_PLT_16_HA:
            referenceString( *r_entry, sec_size, "", "", "@plt@ha", buff, flags );
            break;
        case ORL_RELOC_TYPE_PLT_16_LO:
            referenceString( *r_entry, sec_size, "", "", "@plt@l", buff, flags );
            break;
        default:
            *r_entry = (*r_entry)->next;
            continue;
        }
        // LTYP_UNNAMED labels are always at the correct location
        // if( nvalue != 0 && (*r_entry)->label->type != LTYP_UNNAMED ) {
        // not so - BBB Oct 28, 1996
        if( nvalue != 0 ) {
            p = &buff[strlen(buff)];
            *p++ = '+';
            FmtHexNum( p, 0, nvalue );
        }
        *r_entry = (*r_entry)->next;
    }
    return( strlen( buff ) );
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

unsigned DisCliValueString( void *d, dis_dec_ins *ins, unsigned op_num,
                            char *buff )
{
    struct pass2        *pd = d;
    unsigned            len;
    dis_operand         *op;
    ref_flags           rf;

    buff[0] = '\0';

    rf = RFLAG_DEFAULT;
    op = &ins->op[op_num];
    switch( op->type & DO_MASK ) {
    case DO_RELATIVE:
    case DO_MEMORY_REL:
    case DO_ABSOLUTE:
    case DO_MEMORY_ABS:
        if( pd->r_entry != NULL ) {
            /* if there is an override we must avoid the frame
             */
            if( ( ins->flags & DIS_X86_SEG_OR ) && IsIntelx86() ) {
                rf |= RFLAG_NO_FRAME;
            }
            len = HandleAReference( op->value, ins->size, rf,
                        pd->loop + op->op_position, pd->size, &pd->r_entry,
                        buff );
            if( len != 0 ) return( len );
        }
        switch( op->type & DO_MASK ) {
            case DO_RELATIVE:
            case DO_MEMORY_REL:
                op->value += pd->loop;
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
        if( pd->r_entry != NULL ) {
            rf |= RFLAG_IS_IMMED;
            len = HandleAReference( op->value, ins->size, rf,
                        pd->loop + op->op_position, pd->size, &pd->r_entry,
                        buff );
            if( len != 0 ) return( len );
        }
        FmtSizedHexNum( buff, ins, op_num );
        break;
    }
    return( strlen( buff ) );
}

static void processDataInCode( char *contents, struct pass2 *data,
                               orl_sec_size size, label_entry *l_entry )
{
    orl_sec_size        offset;

    offset = data->loop + size;
    if( DFormat & DFF_ASM ) {
        DumpASMDataFromSection( contents, data->loop, offset, l_entry,
                                &(data->r_entry), NULL );
        BufferPrint();
    } else {
        DumpDataFromSection( contents, data->loop, offset, l_entry,
                             &(data->r_entry), NULL );
    }
    while( data->r_entry && ( data->r_entry->offset < offset ) ) {
        data->r_entry = data->r_entry->next;
    }
    data->loop = offset;
}

num_errors DoPass2( section_ptr sec, char *contents, orl_sec_size size,
                    label_list sec_label_list, ref_list sec_ref_list )
// perform pass 2 on one section
{
    struct pass2        data;
    label_entry         l_entry;
    dis_dec_ins         decoded;
    char                name[ MAX_INS_NAME ];
    char                ops[ MAX_OBJ_NAME + 24 ];       // at most 1 label/relocation per instruction, plus room for registers, brackets and other crap
    unsigned            flags;
    scantab_ptr         st;
    int                 is_intel;

    routineBase = 0;
    st = sec->scan;
    data.size = size;
    if( sec_label_list != NULL ) {
        l_entry = sec_label_list->first;
    }
    if( sec_ref_list != NULL ) {
        data.r_entry = sec_ref_list->first;
    } else {
        data.r_entry = NULL;
    }
    data.disassembly_errors = 0;

    if( source_mix ) {
        GetSourceFile( sec );
    }

    PrintHeader( sec );
    PrintAssumeHeader( sec );
    flags = 0;
    if( GetMachineType() == ORL_MACHINE_TYPE_I386 ) {
        if( ( GetFormat() != ORL_OMF ) ||
            ( ORLSecGetFlags( sec->shnd ) & ORL_SEC_FLAG_USE_32 ) ) {
            flags = DIF_X86_USE32_FLAGS;
        }
        is_intel = 1;
    } else {
        is_intel = IsIntelx86();
    }
    for( data.loop = 0; data.loop < size; data.loop += decoded.size ) {

        // process data in code segment
        while( st && ( data.loop > st->end ) ) {
            st = st->next;
        }
        if( st && ( data.loop >= st->start ) ) {
            decoded.size = 0;
            processDataInCode( contents, &data, st->end - data.loop, &l_entry );
            st = st->next;
            continue;
        }
        // data may not be listed in scan table, but a fixup at this offset will
        // give it away
        while( data.r_entry &&
              ( (data.r_entry->offset < data.loop) || SkipRef(data.r_entry)) ) {
            data.r_entry = data.r_entry->next;
        }
        if( data.r_entry && ( data.r_entry->offset == data.loop ) ) {
            if( is_intel || IsDataReloc( data.r_entry ) ) {
                // we just skip the data
                decoded.size = 0;
                processDataInCode( contents, &data, RelocSize( data.r_entry ),
                                   &l_entry );
                continue;
            }
        }

        if( source_mix ) {
            MixSource( data.loop );
        }
        DisDecodeInit( &DHnd, &decoded );
        decoded.flags |= flags;
        DisDecode( &DHnd, &contents[data.loop], &decoded );
        if( sec_label_list ) {
            if( l_entry != NULL &&
                l_entry->offset > data.loop &&
                l_entry->offset < ( data.loop + decoded.size ) ) {
                /*
                    If we have a label planted in the middle of this
                    instruction (see inline memchr for example), put
                    out a couple of data bytes, and then restart decode
                    and label process from offset of actual label.
                */
                processDataInCode( contents, &data, l_entry->offset - data.loop, &l_entry );
                data.loop = l_entry->offset;
                DisDecodeInit( &DHnd, &decoded );
                decoded.flags |= flags;
                DisDecode( &DHnd, &contents[data.loop], &decoded );
            }
            l_entry = handleLabels( sec->name, data.loop, l_entry, size );
        }
        DisFormat( &DHnd, &data, &decoded, DFormat, name, ops );
        if( !(DFormat & DFF_ASM) ) {
             PrintLinePrefix( contents, data.loop, size,
                                DisInsSizeInc( &DHnd ), decoded.size );
        }
        BufferStore( "    %*s %s", -DisInsNameMax( &DHnd ), name, ops );
        BufferStore("\n");
        BufferPrint();
    }
    if( sec_label_list ) {
        l_entry = handleLabels( sec->name, size, l_entry, size );
    }
    if( !(DFormat & DFF_ASM) ) {
        routineSize = data.loop - routineBase;
        BufferConcatNL();
        BufferMsg( ROUTINE_SIZE );
        BufferStore(" %d ", routineSize );
        BufferMsg( BYTES );
        BufferConcat(",    ");
        BufferMsg( ROUTINE_BASE );
        BufferStore(" %s + %04X\n\n", sec->name, routineBase );
        BufferPrint();
    }
    if( source_mix ) {
        EndSourceMix();
    }
    PrintTail( sec );
    return( data.disassembly_errors );
}
