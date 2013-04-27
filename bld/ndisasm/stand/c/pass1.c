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
* Description:  Disassembler pass 1.
*
****************************************************************************/


#include <string.h>
#include "global.h"
#include "dis.h"
#include "pass1.h"
#include "labproc.h"
#include "refproc.h"
#include "init.h"
#include "formasm.h"
#include "hashtabl.h"

extern dis_handle       DHnd;


ref_entry DoPass1Relocs( unsigned_8 *contents, ref_entry r_entry,
                         orl_sec_offset start, orl_sec_offset end )
{
    long                                value;
    unnamed_label_return_struct         rs;

    if( !IsIntelx86() )
        return( r_entry );

    while( r_entry && ( r_entry->offset < start ) ) {
        r_entry = r_entry->next;
    }

    while( r_entry && ( r_entry->offset < end ) ) {
        if( r_entry->label->shnd && ( r_entry->label->type == LTYP_SECTION ) ) {
            if( r_entry->addend ) {
                value = HandleAddend( r_entry );
            } else {
                switch( RelocSize( r_entry ) ) {
                case( 6 ):
                case( 4 ):
                    value = *((long *)&(contents[ r_entry->offset ]));
                    break;
                case( 2 ):
                    value = *((short *)&(contents[ r_entry->offset ]));
                    break;
                case( 1 ):
                    value = *((signed char *)&(contents[ r_entry->offset ]));
                    break;
                default:
                    value = 0;
                }
            }
            CreateUnnamedLabel( r_entry->label->shnd, value, &rs );
            if( rs.error == OKAY ) {
                r_entry->label = rs.entry;
                r_entry->no_val = 1;
            }
        }
        r_entry = r_entry->next;
    }
    return( r_entry );
}

static int isSelfReloc( ref_entry r_entry )
{
    switch( r_entry->type ) {
    case ORL_RELOC_TYPE_REL_16:
    case ORL_RELOC_TYPE_REL_8:
    case ORL_RELOC_TYPE_REL_16_SEG:
    case ORL_RELOC_TYPE_REL_HI_8:
    case ORL_RELOC_TYPE_REL_32_SEG:
    case ORL_RELOC_TYPE_REL_32:
    case ORL_RELOC_TYPE_REL_32_NOADJ:
    case ORL_RELOC_TYPE_REL_32_ADJ1:
    case ORL_RELOC_TYPE_REL_32_ADJ2:
    case ORL_RELOC_TYPE_REL_32_ADJ3:
    case ORL_RELOC_TYPE_REL_32_ADJ4:
    case ORL_RELOC_TYPE_REL_32_ADJ5:
        return( 1 );
    }
    return( 0 );
}

return_val DoPass1( orl_sec_handle shnd, unsigned_8 *contents, orl_sec_size size,
                    ref_list sec_ref_list, scantab_ptr stl )
// perform pass 1 on one section
{
    orl_sec_offset                      loop;
    dis_dec_ins                         decoded;
    dis_value                           value;
    dis_return                          dr;
    unnamed_label_return_struct         rs;
    return_val                          error;
    unsigned                            i;
    ref_entry                           r_entry;
    dis_inst_flags                      flags;
    orl_sec_offset                      op_pos;
    int                                 is_intel;
    int                                 adjusted;
    sa_disasm_struct                    sds;

    sds.data = contents;
    sds.last = size - 1;
    if( sec_ref_list != NULL ) {
        r_entry = sec_ref_list->first;
    } else {
        r_entry = NULL;
    }

    flags.u.all = 0;
    if( GetMachineType() == ORL_MACHINE_TYPE_I386 ) {
        if( ( GetFormat() != ORL_OMF ) ||
            ( ORLSecGetFlags( shnd ) & ORL_SEC_FLAG_USE_32 ) ) {
            flags.u.all = DIF_X86_USE32_FLAGS;
        }
        is_intel = 1;
    } else {
        is_intel = IsIntelx86();
    }

    for( loop = 0; loop < size; loop += decoded.size ) {

        // skip data in code segment
        while( stl && ( loop > stl->end ) ) {
            stl = stl->next;
        }
        if( stl && ( loop >= stl->start ) ) {
            decoded.size = 0;
            if( is_intel ) {
                r_entry = DoPass1Relocs( contents, r_entry, loop, stl->end );
            }
            loop = stl->end;
            stl = stl->next;
            continue;
        }

        // data may not be listed in scan table, but a fixup at this offset will
        // give it away
        while( r_entry && ( ( r_entry->offset < loop ) || SkipRef(r_entry) ) ) {
            r_entry = r_entry->next;
        }
        if( r_entry && ( r_entry->offset == loop ) ) {
            if( is_intel || IsDataReloc( r_entry ) ) {
                // we just skip the data
                op_pos = loop;
                decoded.size = 0;
                loop += RelocSize( r_entry );
                r_entry = DoPass1Relocs( contents, r_entry, op_pos, loop );
                continue;
            }
        }

        DisDecodeInit( &DHnd, &decoded );
        decoded.flags.u.all |= flags.u.all;
        sds.offs = loop;
        dr = DisDecode( &DHnd, &sds, &decoded );
        // if an invalid instruction was found, there is nothing we can do.
        if( dr != DR_OK )
            return( ERROR );

        for( i = 0; i < decoded.num_ops; ++i ) {
            adjusted = 0;
            op_pos = loop + decoded.op[i].op_position;
            switch( decoded.op[i].type & DO_MASK ) {
            case DO_IMMED:
                if( !is_intel )
                    break;
                /* fall through */
            case DO_RELATIVE:
            case DO_MEMORY_REL:
                if( ( decoded.op[i].type &  DO_MASK ) != DO_IMMED ) {
                    decoded.op[i].value += loop;
                    adjusted = 1;
                }
                /* fall through */
            case DO_ABSOLUTE:
            case DO_MEMORY_ABS:
                // Check for reloc at this location
                while( r_entry && r_entry->offset < op_pos ) {
                    r_entry = r_entry->next;
                }
                if( r_entry && ( r_entry->offset == op_pos ) ) {
                    if( is_intel && r_entry->label->shnd
                        && ( r_entry->type != ORL_RELOC_TYPE_SEGMENT )
                        && ( r_entry->label->type == LTYP_SECTION ) ) {
                        /* For section offsets under intel we MUST generate a
                         * local label because the offset might change when the
                         * code is re-assembled
                         */
                        if( r_entry->addend ) {
                            r_entry->no_val = 0;
                            CreateUnnamedLabel( r_entry->label->shnd,
                                                HandleAddend( r_entry ), &rs );
                        } else {
                            r_entry->no_val = 1;
                            if( adjusted && isSelfReloc( r_entry ) &&
                                ( r_entry->label->type == LTYP_SECTION ) ) {
                                /* This is a kludgy reloc done under OMF
                                 */
                                decoded.op[i].value -= loop;
                                decoded.op[i].value -= decoded.size;
                                switch( RelocSize( r_entry ) ) {
                                case( 2 ):
                                    decoded.op[i].value =
                                        (uint_16)(decoded.op[i].value);
                                case( 1 ):
                                    decoded.op[i].value =
                                        (uint_8)(decoded.op[i].value);
                                }
                            }
                            value = decoded.op[i].value;
                            if( value < 0 || value > ORLSecGetSize( r_entry->label->shnd ) ) {
                                // can't fold it into the label position - BBB Oct 28, 1996
                                value = 0;
                                r_entry->no_val = 0;
                            }
                            CreateUnnamedLabel( r_entry->label->shnd, value, &rs );
                        }
                        if( rs.error != OKAY )
                            return( rs.error );
                        r_entry->label = rs.entry;
                    } else {
                        // fixme: got to handle other types of relocs here
                    }
                } else if( ( decoded.op[i].type &  DO_MASK ) != DO_IMMED ) {
                    if( decoded.op[i].base == DR_NONE &&
                        decoded.op[i].index == DR_NONE ) {
                        switch( decoded.op[i].type & DO_MASK ) {
                        case DO_MEMORY_REL:
                        case DO_MEMORY_ABS:
                            // use decoded instruction size for absolute memory on amd64.
                            // the cpu will reference rip _after_ the instruction is
                            // completely fetched and decoded.
                            // relocations in pass2 are not applied because they break
                            // relative memory references if no relocation is present!
                            if( GetMachineType() == ORL_MACHINE_TYPE_AMD64 ) {
                                decoded.op[i].value += decoded.size;

                                // I don't know if this is neccessary, but it will generate
                                // labels for memory references if no symbol is present
                                // (ex: executable file)
                                CreateUnnamedLabel( shnd, decoded.op[i].value, &rs );
                                if( rs.error != OKAY )
                                    return( rs.error );
                                error = CreateUnnamedLabelRef( shnd, rs.entry, op_pos );
                            } else {
                                // create an LTYP_ABSOLUTE label
                                CreateAbsoluteLabel( shnd, decoded.op[i].value, &rs );
                                if( rs.error != OKAY )
                                    return( rs.error );
                                error = CreateAbsoluteLabelRef( shnd, rs.entry, op_pos );
                            }
                            break;
                        default:
                            // create an LTYP_UNNAMED label
                            CreateUnnamedLabel( shnd, decoded.op[i].value, &rs );
                            if( rs.error != OKAY )
                                return( rs.error );
                            error = CreateUnnamedLabelRef( shnd, rs.entry, op_pos );
                            break;
                        }
                        if( error != OKAY ) {
                            return( error );
                        }
                    }
                }
                break;
            }
        }
    }
    return( OKAY );
}
