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
* Description:  Disassembler pass 2.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include "dis.h"
#include "global.h"
#include "pass2.h"
#include "buffer.h"
#include "srcmix.h"
#include "formasm.h"
#include "init.h"
#include "main.h"
#include "print.h"
#include "labproc.h"

#define COMMENT_TAB_POS 4
#define OPS_REP_TAB_POS 10
#define DIS_X86_SEG_OR  (DIF_X86_CS | DIF_X86_DS | DIF_X86_ES | DIF_X86_FS | DIF_X86_GS | DIF_X86_SS)

struct pass2 {
    dis_sec_offset      loop;
    dis_sec_size        size;
    num_errors          disassembly_errors;
    ref_entry           r_entry;
};

static dis_sec_offset   routineBase = 0;
static dis_sec_size     routineSize = 0;

dis_return DisCliGetData( void *d, unsigned off, size_t size, void *buff )
{
    sa_disasm   pd = d;

    // Check for overrruns, return 0xFFs for reads beyond the end of section
    if( pd->offs + off + size <= pd->last ) {
        memcpy( buff, pd->data + pd->offs + off, size );
    } else if( pd->offs + off <= pd->last ) {
        size_t  valid = pd->last - pd->offs - off + 1;

        memcpy( buff, pd->data + pd->offs + off, valid );
        memset( (char *)buff + valid, 0xFF, size - valid );
    } else {
        memset( (char *)buff, 0xFF, size );
    }
    return( DR_OK );
}

static label_entry handleLabels( const char *sec_name, dis_sec_offset offset, dis_sec_offset end,
                             label_entry l_entry, dis_sec_size size )
// handle any labels at this offset and skip all unused non-label symbols
{
    bool    is32bit;

    is32bit = ( size >= 0x10000 );
    for( ; ( l_entry != NULL ) && ( l_entry->offset < end ); l_entry = l_entry->next ) {
        switch( l_entry->type ) {
        case LTYP_SECTION:
        case LTYP_NAMED:
            if( strcmp( l_entry->label.name, sec_name ) == 0 )
                continue;
            /* fall through */
        case LTYP_UNNAMED:
            if( l_entry->offset > offset )
                return( l_entry );
            break;
        case LTYP_ABSOLUTE:
        case LTYP_FUNC_INFO:
        default:
            continue;
        }
        switch( l_entry->type ) {
        case LTYP_NAMED:
            if( (DFormat & DFF_ASM) == 0 ) {
                if( offset != 0 && l_entry->binding == ORL_SYM_BINDING_GLOBAL ) {
                    routineSize = offset - routineBase;
                    BufferConcatNL();
                    BufferMsg( ROUTINE_SIZE );
                    BufferConcatChar( ' ' );
                    BufferDecimal( routineSize );
                    BufferConcatChar( ' ' );
                    BufferMsg( BYTES );
                    BufferConcat( ",    " );
                    BufferMsg( ROUTINE_BASE );
                    BufferConcatChar( ' ' );
                    BufferConcat( sec_name );
                    BufferConcat( " + " );
                    BufferHex4( routineBase );
                    BufferConcatNL();
                    BufferConcatNL();
                    routineBase = offset;
                }
            }
            /* fall through */
        case LTYP_SECTION:
        case LTYP_UNNAMED:
            if( (DFormat & DFF_ASM) == 0 ) {
                BufferLinePrefixAddress( offset, is32bit );
                BufferAlignToTab( PREFIX_SIZE_TABS );
            }
            if( l_entry->type == LTYP_UNNAMED ) {
                BufferLabelNum( l_entry->label.number );
            } else {
                BufferQuoteName( l_entry->label.name );
            }
            BufferConcatChar( ':' );
            BufferConcatNL();
            break;
        }
        BufferPrint();
    }
    return( l_entry );
}

static void quoteName( const char *name, char *buff )
{
    if( NeedsQuoting( name ) ) {
        strcat( buff, "`" );
        strcat( buff, name );
        strcat( buff, "`" );
    } else {
        strcat( buff, name );
    }
}

static return_val referenceString( ref_entry r_entry, dis_sec_size size,
                                   char *ext_pref, char *int_pref, char *post,
                                   char *buff, ref_flags flags )
{
    label_entry         l_entry;
    const char          *frame_sep;
    const char          *frame;
    char                temp[20];
    dis_value           value;

    frame_sep = ":";
    frame = r_entry->frame;
    if( frame == NULL || (flags & RFLAG_NO_FRAME) ) {
        frame = "";
        frame_sep = "";
    }

    value.u._32[I64HI32] = 0;
    l_entry = r_entry->label;
    if( (Options & METAWARE_COMPATIBLE) || ( ext_pref[0] == '\0' && int_pref[0] == '\0' ) ) {
        switch( l_entry->type ) {
        case LTYP_ABSOLUTE:
            value.u._32[I64LO32] = l_entry->offset;
            FmtHexNum( temp, 0, value );
            if( r_entry->frame == NULL && (flags & RFLAG_NO_FRAME) == 0 )
                frame = "ds:";
            sprintf( buff, "%s%s[%s]", frame, frame_sep, temp);
            break;
        case LTYP_UNNAMED:
            FmtLabelNum( temp, l_entry->label.number );
            sprintf( buff, "%s%s%s%s", frame, frame_sep, temp, post );
            break;
        default:
            sprintf( buff, "%s%s", frame, frame_sep );
            quoteName( l_entry->label.name, buff );
            strcat( buff, post );
            break;
        }
    } else {
        switch( l_entry->type ) {
        case LTYP_EXTERNAL_NAMED:
            sprintf( buff, "%s%s%s", ext_pref, frame, frame_sep );
            quoteName( l_entry->label.name, buff );
            break;
        case LTYP_NAMED:
        case LTYP_SECTION:
        case LTYP_GROUP:
            sprintf( buff, "%s%s%s", int_pref, frame, frame_sep );
            quoteName( l_entry->label.name, buff );
            break;

        case LTYP_ABSOLUTE:
            value.u._32[I64LO32] = l_entry->offset;
            FmtHexNum( temp, 0, value );
            if( *frame == '\0' && ( (flags & RFLAG_NO_FRAME) == 0 ) )
                frame = "ds:";
            sprintf( buff, "%s%s%s[%s]", int_pref, frame, frame_sep, temp);
            break;

        default:
            FmtLabelNum( temp, l_entry->label.number );
            sprintf( buff, "%s%s%s%s", int_pref, frame, frame_sep, temp );
            if( l_entry->offset > size ) {
                return( RC_ERROR );
            }
            break;
        }
    }
    return( RC_OKAY );
}

size_t HandleAReference( dis_value value, int ins_size, ref_flags flags,
                           dis_sec_offset offset, dis_sec_size sec_size,
                           ref_entry *reference_entry, char *buff )
// handle any references at this offset
{
    return_val          error;
    dis_sec_addend      nvalue;
    char                *p;
    ref_entry           r_entry;

    r_entry = *reference_entry;
    buff[0] = '\0';
    for( ; r_entry != NULL && r_entry->offset == offset; r_entry = r_entry->next ) {
        if( r_entry->has_val ) {
            nvalue = value.u._32[I64LO32];
        } else if( r_entry->addend ) {
            nvalue = HandleAddend( r_entry );
        } else {
            nvalue = 0;
        }
        switch( r_entry->type ) {
        case ORL_RELOC_TYPE_MAX + 1:
        case ORL_RELOC_TYPE_JUMP:
        case ORL_RELOC_TYPE_REL_21_SH:
        case ORL_RELOC_TYPE_WORD_26:
            error = referenceString( r_entry, sec_size, "j^", "", "", buff, flags );
            if( error != RC_OKAY ) {
                // label is defined to be beyond the boundaries of the section!
                BufferConcatChar( '\t' );
                if( (DFormat & DFF_ASM) == 0 ) {
                    BufferConcat( "     " );
                    BufferHexU32( 4, offset );
                    BufferAlignToTab( COMMENT_TAB_POS );
                }
                BufferConcat( CommentString );
                BufferMsg( LABEL_BEYOND_SECTION );
                BufferConcatNL();
                BufferPrint();
                *reference_entry = r_entry->next;
                return( 0 );
            }
            continue; // Don't print addend
        case ORL_RELOC_TYPE_SEC_REL:
            referenceString( r_entry, sec_size, "s^", "s^", "@s", buff, flags );
            break;
        case ORL_RELOC_TYPE_HALF_HI:
            referenceString( r_entry, sec_size, "h^", "h^", "@h", buff, flags );
            break;
        case ORL_RELOC_TYPE_HALF_HA:
            referenceString( r_entry, sec_size, "ha^", "ha^", "@ha", buff, flags );
            break;
        case ORL_RELOC_TYPE_HALF_LO:
            referenceString( r_entry, sec_size, "l^", "l^", "@l", buff, flags );
            break;
        case ORL_RELOC_TYPE_REL_14:
        case ORL_RELOC_TYPE_REL_24:
        case ORL_RELOC_TYPE_WORD_14:
        case ORL_RELOC_TYPE_WORD_24:
            nvalue &= ~0x3;
            /* fall through */
        case ORL_RELOC_TYPE_WORD_16:
        case ORL_RELOC_TYPE_WORD_32:
        case ORL_RELOC_TYPE_WORD_64:
            if( ( r_entry->label->type != LTYP_GROUP ) && (flags & RFLAG_IS_IMMED) && IsMasmOutput() ) {
                referenceString( r_entry, sec_size, "offset ", "offset ", "", buff, flags );
            } else {
                referenceString( r_entry, sec_size, "", "", "", buff, flags );
            }
            break;
        case ORL_RELOC_TYPE_REL_16:
            if( IsIntelx86() && r_entry->has_val ) {
                nvalue -= ins_size;
            }
            if( ( r_entry->label->type != LTYP_GROUP ) && (flags & RFLAG_IS_IMMED) && IsMasmOutput() ) {
                referenceString( r_entry, sec_size, "offset ", "offset ", "", buff, flags );
            } else {
                referenceString( r_entry, sec_size, "", "", "", buff, flags  );
            }
            break;
        case ORL_RELOC_TYPE_WORD_8:
        case ORL_RELOC_TYPE_WORD_16_SEG:
        case ORL_RELOC_TYPE_WORD_HI_8:
        case ORL_RELOC_TYPE_WORD_32_SEG:
            // Keep these seperate because they are OMF specific
            referenceString( r_entry, sec_size, "", "", "", buff, flags );
            break;
        case ORL_RELOC_TYPE_SEGMENT:
            if( ( r_entry->label->type != LTYP_GROUP )
                && ( r_entry->label->type != LTYP_SECTION )
                && (flags & RFLAG_IS_IMMED)
                && IsMasmOutput() ) {
                referenceString( r_entry, sec_size, "seg ", "seg ", "", buff, flags );
            } else {
                referenceString( r_entry, sec_size, "", "", "", buff, flags );
            }
            break;

        case ORL_RELOC_TYPE_REL_32_NOADJ:
            // this is a little kluge because Brian's ELF files seem to have
            // -4 in the implicit addend for calls and such BBB May 09, 1997
            nvalue += 4;
            /* fall through */
        case ORL_RELOC_TYPE_REL_8:
        case ORL_RELOC_TYPE_REL_16_SEG:
        case ORL_RELOC_TYPE_REL_HI_8:
        case ORL_RELOC_TYPE_REL_32_SEG:
        case ORL_RELOC_TYPE_REL_32:
        case ORL_RELOC_TYPE_REL_32_ADJ5:
        case ORL_RELOC_TYPE_REL_32_ADJ4:
        case ORL_RELOC_TYPE_REL_32_ADJ3:
        case ORL_RELOC_TYPE_REL_32_ADJ2:
        case ORL_RELOC_TYPE_REL_32_ADJ1:
            // For some reason we add the instruction size to the value
            // of the displacement in a relative call and get a bad
            // offset, due to CORE implementation
            //
            // Main reason :
            // instruction size with displacement and with addend is correct for
            // relative addresses without relocate
            //
            // in amd64 code the instruction size will be added in pass1.c!
            if( r_entry->has_val && !( GetMachineType() == ORL_MACHINE_TYPE_AMD64 ) ) {
                nvalue -= ins_size;
            }
            referenceString( r_entry, sec_size, "", "", "", buff, flags );
            break;
        case ORL_RELOC_TYPE_TOCREL_14:
            nvalue &= ~0x3;
            /* fall through */
        case ORL_RELOC_TYPE_TOCREL_16:
            referenceString( r_entry, sec_size, "[toc]", "[toc]", "@toc", buff, flags );
            break;
        case ORL_RELOC_TYPE_TOCVREL_14:
            nvalue &= ~0x3;
            /* fall through */
        case ORL_RELOC_TYPE_TOCVREL_16:
            referenceString( r_entry, sec_size, "[tocv]", "[tocv]", "@tocv", buff, flags );
            break;
        case ORL_RELOC_TYPE_GOT_16:
            referenceString( r_entry, sec_size, "", "", "@got", buff, flags );
            break;
        case ORL_RELOC_TYPE_GOT_16_HI:
            referenceString( r_entry, sec_size, "", "", "@got@h", buff, flags );
            break;
        case ORL_RELOC_TYPE_GOT_16_HA:
            referenceString( r_entry, sec_size, "", "", "@got@ha", buff, flags );
            break;
        case ORL_RELOC_TYPE_GOT_16_LO:
            referenceString( r_entry, sec_size, "", "", "@got@l", buff, flags );
            break;
        case ORL_RELOC_TYPE_PLTREL_24:
        case ORL_RELOC_TYPE_PLTREL_32:
        case ORL_RELOC_TYPE_PLT_32:
            referenceString( r_entry, sec_size, "", "", "@plt", buff, flags );
            break;
        case ORL_RELOC_TYPE_PLT_16_HI:
            referenceString( r_entry, sec_size, "", "", "@plt@h", buff, flags );
            break;
        case ORL_RELOC_TYPE_PLT_16_HA:
            referenceString( r_entry, sec_size, "", "", "@plt@ha", buff, flags );
            break;
        case ORL_RELOC_TYPE_PLT_16_LO:
            referenceString( r_entry, sec_size, "", "", "@plt@l", buff, flags );
            break;
        default:
            continue;
        }
        // LTYP_UNNAMED labels are always at the correct location
        // if( nvalue != 0 && r_entry->label->type != LTYP_UNNAMED ) {
        // not so - BBB Oct 28, 1996
        if( r_entry->has_val && nvalue != 0 ) {
            p = &buff[strlen(buff)];
            if( nvalue < 0 ) {
                *p++ = '-';
                nvalue = -nvalue;
            } else {
                *p++ = '+';
            }
            value.u._32[I64HI32] = 0;
            value.u._32[I64LO32] = nvalue;
            FmtHexNum( p, 0, value );
        }
    }
    *reference_entry = r_entry;
    return( strlen( buff ) );
}

static void FmtSizedHexNum( char *buff, dis_dec_ins *ins, unsigned op_num )
{
    unsigned            size;
    unsigned            len;
    unsigned            i;
    dis_value           mask;
    dis_value           value;

    mask.u._32[I64HI32] = 0;
    mask.u._32[I64LO32] = 0;
    value = ins->op[op_num].value;
    switch( ins->op[op_num].ref_type ) {
    case DRT_SPARC_BYTE:
    case DRT_X86_BYTE:
    case DRT_X64_BYTE:
        size = 2;
        mask.u._32[I64LO32] = 0x000000ff;
        break;
    case DRT_SPARC_HALF:
    case DRT_X86_WORD:
    case DRT_X64_WORD:
        size = 4;
        mask.u._32[I64LO32] = 0x0000ffff;
        break;
    case DRT_SPARC_WORD:
    case DRT_SPARC_SFLOAT:
    case DRT_X86_DWORD:
    case DRT_X86_DWORDF:
    case DRT_X64_DWORD:
        size = 8;
        mask.u._32[I64LO32] = 0xffffffff;
        break;
    case DRT_X64_QWORD:
    case DRT_SPARC_DWORD:
    case DRT_SPARC_DFLOAT:
        size = 16;
        mask.u._32[I64HI32] = 0xffffffff;
        mask.u._32[I64LO32] = 0xffffffff;
        break;
    default:
        size = 0;
        for( i = 0; i < ins->num_ops; i++ ) {
            switch( ins->op[i].ref_type ) {
            case DRT_X86_BYTE:
            case DRT_X64_BYTE:
                len = 2;
                if ( len > size ) {
                    size = len;
                    mask.u._32[I64LO32] = 0x000000ff;
                }
                break;
            case DRT_X86_WORD:
            case DRT_X64_WORD:
                len = 4;
                if ( len > size ) {
                    size = len;
                    mask.u._32[I64LO32] = 0x0000ffff;
                }
                break;
            case DRT_X86_DWORD:
            case DRT_X86_DWORDF:
            case DRT_X64_DWORD:
                len = 8;
                if ( len > size ) {
                    size = len;
                    mask.u._32[I64LO32] = 0xffffffff;
                }
                break;
            case DRT_X64_QWORD:
                len = 16;
                if ( len > size ) {
                    size = len;
                    mask.u._32[I64HI32] = 0xffffffff;
                    mask.u._32[I64LO32] = 0xffffffff;
                }
                break;
            default:
                break;
            }
        }
        if ( size == 0 ) {
            size = 8;
            mask.u._32[I64LO32] = 0xffffffff;
        }
        break;
    }
    value.u._32[I64HI32] &= mask.u._32[I64HI32];
    value.u._32[I64LO32] &= mask.u._32[I64LO32];
    FmtHexNum( buff, size, value );
}

size_t DisCliValueString( void *d, dis_dec_ins *ins, unsigned op_num, char *buff, size_t buff_len )
{
    struct pass2        *pd = d;
    size_t              len;
    dis_operand         *op;
    ref_flags           rf;
    dis_value           value;

    /* unused parameters */ (void)buff_len;

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
            if( (ins->flags.u.x86 & DIS_X86_SEG_OR) && IsIntelx86() ) {
                rf |= RFLAG_NO_FRAME;
            }
            len = HandleAReference( op->value, ins->size, rf,
                        pd->loop + op->op_position, pd->size, &pd->r_entry, buff );
            if( len != 0 ) {
                return( len );
            }
        }
        switch( op->type & DO_MASK ) {
        case DO_RELATIVE:
        case DO_MEMORY_REL:
            op->value.u._32[I64LO32] += pd->loop;
            break;
        }
        if( op->base == DR_NONE && op->index == DR_NONE ) {
            FmtSizedHexNum( buff, ins, op_num );
        } else if( op->value.s._32[I64LO32] > 0 ) {
            FmtHexNum( buff, 0, op->value );
        } else if( op->value.s._32[I64LO32] < 0 ) {
            buff[0] = '-';
            value.s._32[I64HI32] = 0;
            value.s._32[I64LO32] = -op->value.s._32[I64LO32];
            FmtHexNum( &buff[1], 0, value );
        }
        break;
    case DO_IMMED:
        if( pd->r_entry != NULL ) {
            rf |= RFLAG_IS_IMMED;
            len = HandleAReference( op->value, ins->size, rf,
                        pd->loop + op->op_position, pd->size, &pd->r_entry, buff );
            if( len != 0 ) {
                return( len );
            }
        }
        FmtSizedHexNum( buff, ins, op_num );
        break;
    }
    return( strlen( buff ) );
}

static void processDataInCode( section_ptr section, unsigned_8 *contents, struct pass2 *data,
                               dis_sec_size size, label_entry *l_entry )
{
    dis_sec_size        offset;

    offset = data->loop + size;
    if( DFormat & DFF_ASM ) {
        DumpASMDataFromSection( contents, data->loop, offset, l_entry, &(data->r_entry), section );
        BufferPrint();
    } else {
        DumpDataFromSection( contents, data->loop, offset, l_entry, &(data->r_entry), section );
    }
    for( ; data->r_entry != NULL; data->r_entry = data->r_entry->next ) {
        if( data->r_entry->offset >= offset ) {
            break;
        }
    }
    data->loop = offset;
}

static ref_entry processFpuEmulatorFixup( ref_entry r_entry, dis_sec_offset loop, const char **pfixup )
{
    const char  *fpu_fixup;

    if( r_entry != NULL && r_entry->offset == loop ) {
        fpu_fixup = SkipRef( r_entry );
        if( fpu_fixup != NULL ) {
            r_entry = r_entry->next;
            // there can be second fixup per instruction with 1 byte offset
            // it must be skipped too, displayed is first only
            // first one is significant, second one is segment override only
            if( r_entry != NULL && SkipRef( r_entry ) != NULL && ( r_entry->offset == loop + 1 ) ) {
                r_entry = r_entry->next;
            }
            if( (Options & PRINT_FPU_EMU_FIXUP) == 0 ) {
                fpu_fixup = NULL;
            }
        }
    } else {
        fpu_fixup = NULL;
    }
    *pfixup = fpu_fixup;
    return( r_entry );
}

num_errors DoPass2( section_ptr section, unsigned_8 *contents, dis_sec_size size,
                    label_list sec_label_list, ref_list sec_ref_list )
// perform pass 2 on one section
{
    struct pass2        data;
    label_entry         l_entry;
    dis_dec_ins         decoded;
    char                name[ MAX_INS_NAME ];
    char                ops[ MAX_OBJ_NAME + 24 ];       // at most 1 label/relocation per instruction, plus room for registers, brackets and other crap
    dis_inst_flags      flags;
    scantab_ptr         st;
    bool                is_intel;
    sa_disasm_struct    sds;
    const char          *FPU_fixup;
    int                 pos_tabs;
    bool                is32bit;
    orl_table_index     currline;
    orl_table_index     lineInFile;
    state_lines         ls;

    routineBase = 0;
    st = section->scan;
    data.size = size;
    sds.data = contents;
    sds.last = size - 1;
    l_entry = NULL;
    if( sec_label_list != NULL ) {
        l_entry = sec_label_list->first;
    }
    if( sec_ref_list != NULL ) {
        data.r_entry = sec_ref_list->first;
    } else {
        data.r_entry = NULL;
    }
    data.disassembly_errors = 0;
    currline = 0;
    lineInFile = 0;
    ls.lines = NULL;
    ls.numlines = 0;
    ls.currlinesize = 0;
    if( source_mix ) {
        GetSourceFile( &ls, section );
    }

    PrintHeader( section );
    if( size && sec_label_list )
        PrintAssumeHeader( section );
    flags.u.all = DIF_NONE;
    if( GetMachineType() == ORL_MACHINE_TYPE_I386 ) {
        if( ( GetFormat() != ORL_OMF ) || (ORLSecGetFlags( section->shnd ) & ORL_SEC_FLAG_USE_32) ) {
            flags.u.x86 = DIF_X86_USE32_FLAGS;
        }
        is_intel = true;
    } else {
        is_intel = IsIntelx86();
    }
    is32bit = ( size >= 0x10000 );
    for( data.loop = 0; data.loop < size; data.loop += decoded.size ) {

        // process data in code segment
        for( ; st != NULL; st = st->next ) {
            if( st->end >= data.loop ) {
                break;
            }
        }
        if( st != NULL && ( data.loop >= st->start ) ) {
            decoded.size = 0;
            processDataInCode( section, contents, &data, st->end - data.loop, &l_entry );
            st = st->next;
            continue;
        }
        // data may not be listed in scan table, but a fixup at this offset will
        // give it away
        for( ; data.r_entry != NULL; data.r_entry = data.r_entry->next ) {
            if( data.r_entry->offset >= data.loop ) {
                break;
            }
        }
        data.r_entry = processFpuEmulatorFixup( data.r_entry, data.loop, &FPU_fixup );
        if( data.r_entry != NULL && ( data.r_entry->offset == data.loop ) ) {
            if( is_intel || IsDataReloc( data.r_entry ) ) {
                // we just skip the data
                decoded.size = 0;
                processDataInCode( section, contents, &data, RelocSize( data.r_entry ), &l_entry );
                continue;
            }
        }

        if( source_mix ) {
            MixSource( &ls, data.loop, &currline, &lineInFile );
        }
        DisDecodeInit( &DHnd, &decoded );
        decoded.flags.u.all |= flags.u.all;
        sds.offs = data.loop;
        DisDecode( &DHnd, &sds, &decoded );
        if( sec_label_list ) {
            l_entry = handleLabels( section->name, data.loop, data.loop + decoded.size, l_entry, size );
            if( ( l_entry != NULL )
                && ( l_entry->offset > data.loop )
                && ( l_entry->offset < data.loop + decoded.size ) ) {
                /*
                    If we have a label planted in the middle of this
                    instruction (see inline memchr for example), put
                    out a couple of data bytes, and then restart decode
                    and label process from offset of actual label.
                */
                decoded.size = 0;
                processDataInCode( section, contents, &data, l_entry->offset - data.loop, &l_entry );
                continue;
            }
        }
        DisFormat( &DHnd, &data, &decoded, DFormat, name, sizeof( name ), ops, sizeof( ops ) );
        if( FPU_fixup != NULL ) {
            if( (DFormat & DFF_ASM) == 0 ) {
                BufferAlignToTab( PREFIX_SIZE_TABS );
            }
            BufferConcatChar( '\t' );
            BufferConcat( CommentString );
            BufferConcat( "FPU fixup " );
            BufferConcat( FPU_fixup );
            BufferConcatNL();
        }
        if( (DFormat & DFF_ASM) == 0 ) {
            unsigned_64     *tmp_64;
            unsigned_32     *tmp_32;
            unsigned_16     *tmp_16;

            tmp_64 = (unsigned_64 *)(contents + data.loop);
            tmp_32 = (unsigned_32 *)(contents + data.loop);
            tmp_16 = (unsigned_16 *)(contents + data.loop);
            if( DHnd.need_bswap ) {
                switch( DisInsSizeInc( &DHnd ) ) {
                //case 8:
                //    SWAP_64( *tmp_64 );
                //    break;
                case 4:
                    SWAP_32( *tmp_32 );
                    break;
                case 2:
                    SWAP_16( *tmp_16 );
                    break;
                default:
                    break;
                }
            }
            BufferLinePrefixAddress( data.loop, is32bit );
            BufferLinePrefixData( contents, data.loop, size, DisInsSizeInc( &DHnd ), decoded.size );
            BufferAlignToTab( PREFIX_SIZE_TABS );
        }
        BufferConcatChar( '\t' );
        BufferConcat( name );
        if( *ops != '\0' ) {
            pos_tabs = ( DisInsNameMax( &DHnd ) + TAB_WIDTH ) / TAB_WIDTH + 1;
            if( (DFormat & DFF_ASM) == 0 ) {
                pos_tabs += PREFIX_SIZE_TABS;
            }
            BufferAlignToTab( pos_tabs );
            BufferConcat( ops );
        }
        BufferConcatNL();
        BufferPrint();
    }
    if( sec_label_list ) {
        l_entry = handleLabels( section->name, size, (dis_sec_offset)-1, l_entry, size );
    }
    if( (DFormat & DFF_ASM) == 0 ) {
        routineSize = data.loop - routineBase;
        BufferConcatNL();
        BufferMsg( ROUTINE_SIZE );
        BufferConcatChar( ' ' );
        BufferDecimal( routineSize );
        BufferConcatChar( ' ' );
        BufferMsg( BYTES );
        BufferConcat( ",    " );
        BufferMsg( ROUTINE_BASE );
        BufferConcatChar( ' ' );
        BufferConcat( section->name );
        BufferConcat( " + " );
        BufferHex4( routineBase );
        BufferConcatNL();
        BufferConcatNL();
        BufferPrint();
    }
    if( source_mix ) {
        EndSourceMix( &ls );
    }
    PrintTail( section );
    return( data.disassembly_errors );
}
