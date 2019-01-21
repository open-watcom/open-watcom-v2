/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  HLL/CV source line cue support.
*
****************************************************************************/


#include "hllinfo.h"
#include <stdint.h>
#include "walloca.h"


/*
 * Gets the size of a line number entry.
 */
static unsigned hllLinnumSize( imp_cue_handle *icueh )
{
    switch( icueh->style ) {
    case HLL_LINE_STYLE_SRC_LINES:
    case HLL_LINE_STYLE_SRC_LINES_SEG_16:
        return( sizeof( cv3_linnum_entry_16 ) );
    case HLL_LINE_STYLE_SRC_LINES_SEG_32:
        return( sizeof( cv3_linnum_entry_32 ) );
    case HLL_LINE_STYLE_HLL_01:
    case HLL_LINE_STYLE_HLL_02:
        return( sizeof( hl1_linnum_entry ) );
    default:
        hllConfused();
    case HLL_LINE_STYLE_HLL_03:
    case HLL_LINE_STYLE_HLL_04:
        return( sizeof( hl3_linnum_entry ) );
    }
}

static int hllLinnumCmp16CV3(const void *_e1, const void *_e2)
{
    const cv3_linnum_entry_16 *e1 = _e1;
    const cv3_linnum_entry_16 *e2 = _e2;
    if( e1->line > e2->line)
        return 1;
    if( e1->line < e2->line)
        return -1;
    if( e1->offset > e2->offset)
        return -1;
    return 0;
}

static int hllLinnumCmp32CV3(const void *_e1, const void *_e2)
{
    const cv3_linnum_entry_32 *e1 = _e1;
    const cv3_linnum_entry_32 *e2 = _e2;
    if( e1->line > e2->line)
        return 1;
    if( e1->line < e2->line)
        return -1;
    if( e1->offset > e2->offset)
        return -1;
    return 0;
}

static int hllLinnumCmpHL1(const void *_e1, const void *_e2)
{
    const hl1_linnum_entry *e1 = _e1;
    const hl1_linnum_entry *e2 = _e2;
    if( e1->sfi > e2->sfi)
        return 1;
    if( e1->sfi < e2->sfi)
        return -1;
    if( e1->line > e2->line)
        return 1;
    if( e1->line < e2->line)
        return -1;
    if( e1->offset > e2->offset)
        return -1;
    return 0;
}

static int hllLinnumCmpHL3(const void *_e1, const void *_e2)
{
    const hl3_linnum_entry *e1 = _e1;
    const hl3_linnum_entry *e2 = _e2;
    if( e1->sfi > e2->sfi)
        return 1;
    if( e1->sfi < e2->sfi)
        return -1;
    if( e1->line > e2->line)
        return 1;
    if( e1->line < e2->line)
        return -1;
    if( e1->offset > e2->offset)
        return -1;
    return 0;
}

/*
 * VM destructure callback for tracking sorted linnum blocks.
 */
static void hllLinnumDtor( imp_image_handle *iih, void *entries )
{
    int i;
    for( i = 0; i < HLLINFO_NUM_SORTED; i++ ) {
        if( iih->sorted_linnum_blocks[i] == entries ) {
            iih->sorted_linnum_blocks[i] = NULL;
            break;
        }
    }
}

/*
 * Load and sort the linenumbers in walk order.
 *
 * We should not assume that the line numbers are sorted in any helpful
 * way in the debug info we load from file. (It might be sorted by segment
 * offset but that's not very helpful.) However, we are supposed to
 * return them as if they were ordered by [sfi,] line. So, since I'm lazy
 * (don't sue me) we'll do that.
 */
static void *hllGetSortedLinnums( imp_image_handle *iih, imp_cue_handle *icueh )
{
    const unsigned  entry_size = hllLinnumSize( icueh );
    void           *entries;
    int             i;

    entries = VMBlock( iih, icueh->lines, icueh->num_lines * entry_size );
    if( !entries ) {
        return( NULL );
    }

    /* already sorted? */
    for( i = 0; i < HLLINFO_NUM_SORTED; i++ ) {
        if( iih->sorted_linnum_blocks[i] == entries ) {
            return( entries );
        }
    }

    /* sort and register a destructor */
    switch( icueh->style ) {
    case HLL_LINE_STYLE_SRC_LINES:
    case HLL_LINE_STYLE_SRC_LINES_SEG_16:
        qsort( entries, icueh->num_lines, entry_size, hllLinnumCmp16CV3 );
        break;
    case HLL_LINE_STYLE_SRC_LINES_SEG_32:
        qsort( entries, icueh->num_lines, entry_size, hllLinnumCmp32CV3 );
        break;
    case HLL_LINE_STYLE_HLL_01:
    case HLL_LINE_STYLE_HLL_02:
        qsort( entries, icueh->num_lines, entry_size, hllLinnumCmpHL1 );
        break;
    default:
        hllConfused();
    case HLL_LINE_STYLE_HLL_03:
    case HLL_LINE_STYLE_HLL_04:
        qsort( entries, icueh->num_lines, entry_size, hllLinnumCmpHL3 );
        break;
    }

    VMAddDtor( iih, icueh->lines, hllLinnumDtor, entries );

    for( i = 0; i < HLLINFO_NUM_SORTED; i++ ) {
        if( !iih->sorted_linnum_blocks[i] ) {
            iih->sorted_linnum_blocks[i] = entries;
            return( entries );
        }
    }

    i = ((uintptr_t)entries >> 7) % HLLINFO_NUM_SORTED;
    iih->sorted_linnum_blocks[i] = entries;
    return( entries );
}


/*
 * Walk the list of source files of a module.
 */
walk_result DIPIMPENTRY( WalkFileList )( imp_image_handle *iih, imp_mod_handle imh,
                       DIP_IMP_CUE_WALKER *wk, imp_cue_handle *icueh, void *d )
{
    hll_dir_entry                       *hde;
    //hl4_linnum_first                    *hdr4;
    //hl4_filetab_entry                   *fp4;
    //unsigned                            file_tab_count;
    //unsigned                            i;
    walk_result                         wr;

    /*
     * Really old line numbers.
     */
    hde = hllFindDirEntry( iih, imh, hll_sstSrcLines );
    if( hde ) {
        /*
         *  while( more data ) {
         *      +0      DB  filename len
         *      +1..n   DB  filename
         *      n       DW  number of entries
         *      while( more entries ) {
         *          n + 2   DW  line number
         *          n + 4   DW  code offset
         *      }
         * }
         */
        unsigned_32         pos;
        const unsigned_32   entry_size = sizeof( cv3_linnum_entry_16 );

        icueh->style = HLL_LINE_STYLE_SRC_LINES;
        icueh->imh = imh;
        icueh->cur_line = 0;
        icueh->segment = 1;
        pos = 0;
        while( pos < hde->cb ) {
            unsigned_8  name_len;

            /* read the header */
            icueh->num_lines = 0;
            if( !VMSsGetU8( iih, hde, pos, &name_len )
             || !VMSsGetU16( iih, hde, pos + 1 + name_len, (unsigned_16 *)&icueh->num_lines )
                ) {
                return( WR_FAIL );
            }

            icueh->file = hde->lfo + pos;
            icueh->lines = icueh->file + 1 + name_len + 2;

            pos += 1 + name_len + 2 + icueh->num_lines * entry_size;
            if( pos > hde->cb ) {
                return( WR_FAIL );
            }

            if( icueh->num_lines > 0  ) {
                wr = wk( iih, icueh, d );
                if( wr != WR_CONTINUE ) {
                    return( wr );
                }
            }
        }
    }

    /*
     * Old line numbers.
     */
    hde = hllFindDirEntry( iih, imh, hll_sstSrcLnSeg );
    if( hde ) {
        /*
         * while( more data ) {
         *      +0      DB  filename len
         *      +1..n   DB  filename
         *      n       DW  segment index
         *      n + 2   DW  number of entries
         *      while( more entries ) {
         *          if( 32-bit ) {
         *              n + 2   DW  line number
         *              n + 4   DD  code offset
         *          } else {
         *              n + 2   DW  line number
         *              n + 4   DW  code offset
         *          }
         *      }
         * }
         */
        unsigned_32 pos;
        unsigned_32 entry_size;

        if( iih->format_lvl == HLL_LVL_NB00
         || iih->format_lvl == HLL_LVL_NB02 ) {
            entry_size = sizeof( cv3_linnum_entry_16 );
            icueh->style  = HLL_LINE_STYLE_SRC_LINES_SEG_16;
        } else {
            entry_size = sizeof( cv3_linnum_entry_32 );;
            icueh->style  = HLL_LINE_STYLE_SRC_LINES_SEG_32;
        }
        icueh->imh = imh;
        icueh->cur_line = 0;
        pos = 0;
        while( pos < hde->cb ) {
            unsigned_8  name_len;

            /* read the header */
            icueh->num_lines = 0;
            if( !VMSsGetU8( iih, hde, pos, &name_len )
             || !VMSsGetU16( iih, hde, pos + 1 + name_len, &icueh->segment )
             || !VMSsGetU16( iih, hde, pos + 1 + name_len + 2, (unsigned_16 *)&icueh->num_lines )
                ) {
                return( WR_FAIL );
            }

            icueh->file = hde->lfo + pos;
            icueh->lines = icueh->file + 1 + name_len + 4;

            pos += 1 + name_len + 4 + icueh->num_lines * entry_size;
            if( pos > hde->cb ) {
                return( WR_FAIL );
            }

            if( icueh->num_lines > 0  ) {
                wr = wk( iih, icueh, d );
                if( wr != WR_CONTINUE ) {
                    return( wr );
                }
            }
        }
    }

    /*
     * HLL line numbers - 4 variations.
     *
     * This code ASSUMES that there is only ONE file names table and ONE line
     * number table. Any compiler emitting anything else will not work right
     * with the code in this file! I.e. path tables, listing, and source + listing
     * blocks are completely ignored.
     */
    hde = hllFindDirEntry( iih, imh, hll_sstHLLSrc );
    if( hde )  {
        unsigned_32     pos = 0;
        unsigned_32     num_files;
        unsigned_32     end_files;
        unsigned_32     first_sfi;
        unsigned        i;

        icueh->imh         = imh;
        icueh->cur_line    = 0;
        icueh->lines       = 0;
        icueh->file        = 0;
        icueh->u.hll.base_offset = 0;
        icueh->u.hll.first = 0;
        icueh->u.hll.last  = 0;

        num_files = 0;
        end_files = 0;
        first_sfi = 0;

        /*
         * Find the two tables.
         */
        switch( hllGetModStyle( iih, imh ) ) {
            /*
             * In HLL V1, there is only one block which starts with
             * line numbers, continues with paths and finishes off
             * with files.
             * FIXME: Not 100% sure about the number of blocks. Needs testing.
             */
        case HLL_STYLE_HL01:
            {
                hl1_linnum_first   *first;
                hl1_filetab_hdr    *hdr;

                icueh->style  = HLL_LINE_STYLE_HLL_01;

                /* get the 'first' record. */
                first = VMSsBlock( iih, hde, pos, sizeof( *first ) );
                if( !first ) {
                    return( WR_FAIL );
                }
                if( first->entry_type != HLL_LNE_TYPE_SOURCE ) {
                    hllConfused(); /* see ASSUMPTION above */
                    return( WR_FAIL );
                }

                end_files = hde->cb;
                icueh->segment = 1;
                icueh->lines = pos + sizeof( *first );
                icueh->num_lines = first->num_line_entries;
                icueh->file = icueh->lines
                         + icueh->num_lines * sizeof( hl1_linnum_entry )
                         + first->num_path_entries * sizeof( hl1_pathtab_entry );

                /* get the file table header */
                hdr = VMSsBlock( iih, hde, icueh->file, sizeof( *hdr ) );
                if( !hdr ) {
                    return( WR_FAIL );
                }
                if( !hdr->num_files ) {
                    return( WR_CONTINUE );
                }
                num_files = hdr->num_files;
                first_sfi = hdr->first_sfi;
            }
            break;

        /*
         * We got segments in the header now and it's possible to
         * have several chunks of linenumbers and paths. We will
         * only deal with the first chunk.
         * FIXME: Not 100% sure about > 1 blocks and entry types. Needs testing.
         */
        case HLL_STYLE_HL02:
            icueh->style  = HLL_LINE_STYLE_HLL_02;
            while( pos < hde->cb && ( !icueh->lines || !icueh->file ) ) {
                hl2_linnum_first   *first;
                hl1_filetab_hdr    *hdr;
                unsigned_32         pos_next;

                /* get the 'first' record. */
                first = VMSsBlock( iih, hde, pos, sizeof( *first ) );
                if( !first ) {
                    return( WR_FAIL );
                }
                if( first->entry_type != HLL_LNE_TYPE_SOURCE ) {
                    hllConfused(); /* see ASSUMPTION above */
                    return( WR_FAIL );
                }

                pos_next = pos
                         + first->num_line_entries * sizeof( hl1_linnum_entry )
                         + first->file_tab_size;

                if( first->num_line_entries ) {
                    icueh->segment = first->seg;
                    icueh->lines = pos + sizeof( *first );
                    icueh->num_lines = first->num_line_entries;
                }

                if( first->file_tab_size > sizeof( hl1_filetab_hdr ) ) {
                    icueh->file = icueh->lines
                             + icueh->num_lines * sizeof( hl1_linnum_entry );
                    end_files = icueh->file + first->file_tab_size;

                    /* get the file table header */
                    hdr = VMSsBlock( iih, hde, icueh->file, sizeof( *hdr ) );
                    if( !hdr ) {
                        return( WR_FAIL );
                    }

                    num_files = hdr->num_files;
                    first_sfi = hdr->first_sfi;
                }

                pos = pos_next;
            }
            break;

            /*
             * Like HLL V2 except that the line number entry has changed.
             * FIXME: Not 100% sure about > 1 blocks and entry types. Needs testing.
             */
        case HLL_STYLE_HL03:
            icueh->style  = HLL_LINE_STYLE_HLL_03;
            while( pos < hde->cb && ( !icueh->lines || !icueh->file ) ) {
                hl2_linnum_first   *first;
                hl1_filetab_hdr    *hdr;
                unsigned_32         pos_next;

                /* get the 'first' record. */
                first = VMSsBlock( iih, hde, pos, sizeof( *first ) );
                if( !first ) {
                    return( WR_FAIL );
                }
                if( first->entry_type != HLL_LNE_TYPE_SOURCE ) {
                    hllConfused(); /* see ASSUMPTION above */
                    return( WR_FAIL );
                }

                pos_next = pos
                         + first->num_line_entries * sizeof( hl3_linnum_entry )
                         + first->file_tab_size;

                if( first->num_line_entries ) {
                    icueh->segment = first->seg;
                    icueh->lines = pos + sizeof( *first );
                    icueh->num_lines = first->num_line_entries;
                }

                if( first->file_tab_size > sizeof( hl1_filetab_hdr ) ) {
                    icueh->file = pos + sizeof( *first )
                             + icueh->num_lines * sizeof( hl3_linnum_entry );
                    end_files = icueh->file + first->file_tab_size;

                    /* get the file table header */
                    hdr = VMSsBlock( iih, hde, icueh->file, sizeof( *hdr ) );
                    if( !hdr ) {
                        return( WR_FAIL );
                    }

                    num_files = hdr->num_files;
                    first_sfi = hdr->first_sfi;
                }

                pos = pos_next;
            }
            break;

        default:
            hllConfused();
            /*
             * With HLL V4 the different tables are separated and the
             * path table is back in (and with a shorter first record).
             * We believe there was no changes to the these records after v4.
             */
        case HLL_STYLE_HL04:
        case HLL_STYLE_HL05:
        case HLL_STYLE_HL06:
            icueh->style  = HLL_LINE_STYLE_HLL_04;
            while( pos < hde->cb && ( !icueh->lines || !icueh->file ) ) {
                union {
                    hl4_linnum_first_core   core;
                    hl4_linnum_first_files  files;
                    hl4_linnum_first_lines  lines;
                }                   *first;
                hl1_filetab_hdr     *hdr;
                unsigned_32         pos_next;

                /* get the 'first' record. */
                first = VMSsBlock( iih, hde, pos, sizeof( *first ) );
                if( !first ) {
                    return( WR_FAIL );
                }
                switch( first->core.entry_type ) {
                case HLL_LNE_TYPE_SOURCE:
                    icueh->segment = first->lines.seg;
                    icueh->lines = pos + sizeof( *first );
                    icueh->num_lines = first->lines.num_line_entries;
                    icueh->u.hll.base_offset = first->lines.base_offset;
                    if( icueh->segment
                     && iih->seg_count > icueh->segment
                     && icueh->u.hll.base_offset >= iih->segments[icueh->segment-1].address ) {
                        icueh->u.hll.base_offset -= iih->segments[icueh->segment-1].address;
                    }
                    pos_next = pos
                             + first->lines.num_line_entries * sizeof( hl3_linnum_entry );
                    break;

                case HLL_LNE_TYPE_FILE_TABLE:
                    icueh->file = pos + sizeof( *first );
                    end_files = icueh->file + first->files.file_tab_size;

                    /* get the file table header */
                    hdr = VMSsBlock( iih, hde, icueh->file, sizeof( *hdr ) );
                    if( !hdr ) {
                        return( WR_FAIL );
                    }

                    num_files = hdr->num_files;
                    first_sfi = hdr->first_sfi;
                    pos_next = end_files;
                    break;

                default:
                    hllConfused(); /* see ASSUMPTION above */
                    return( WR_FAIL );
                }

                pos = pos_next;
            }
            break;
        }

        /*
         * Check and adjust the offsets
         */
        if( end_files > hde->cb ) {
            hllConfused();
            end_files = hde->cb;
        }
        icueh->lines += hde->lfo;
        icueh->u.hll.first = 0;
        icueh->u.hll.last  = 0;

        first_sfi++; /* one based */

        /*
         * Iterate the two tables in parallel.
         */
        pos = icueh->file + sizeof( hl1_filetab_hdr );
        for( i = 0; i < num_files && pos < end_files; i++ ) {
            union {
                hl1_linnum_entry hl1;
                hl3_linnum_entry hl3;
            }              *entries;
            unsigned_16     sfi;
            unsigned_8      file_len;

            /* get the line numbers (again). */
            entries = hllGetSortedLinnums( iih, icueh );
            if( !entries ) {
                return( WR_FAIL );
            }
            sfi = icueh->style < HLL_LINE_STYLE_HLL_03
                ? entries[icueh->u.hll.first].hl1.sfi
                : entries[icueh->u.hll.first].hl3.sfi;
            if( sfi == i + first_sfi) {
                /* find the end of the line numbers for this file. */
                icueh->u.hll.last = icueh->u.hll.first + 1;
                while( icueh->u.hll.last < icueh->num_lines
                    && ( icueh->style < HLL_LINE_STYLE_HLL_03
                        ? entries[icueh->u.hll.last].hl1.sfi
                        : entries[icueh->u.hll.last].hl3.sfi )
                       == sfi ) {
                    icueh->u.hll.last++;
                }
                icueh->u.hll.last--;
                icueh->file = hde->lfo + pos;

                /* do the callback */
                wr = wk( iih, icueh, d );
                if( wr != WR_CONTINUE ) {
                    return( wr );
                }

                /* advance the current (hll) line number. */
                if( ++icueh->u.hll.last >= icueh->num_lines ) {
                    break;
                }
                icueh->cur_line = icueh->u.hll.first = icueh->u.hll.last;
            }

            /* next file name */
            if( !VMSsGetU8( iih, hde, pos, &file_len ) ) {
                return( WR_FAIL );
            }
            pos += 1 + file_len;
        }

    }

    return( WR_CONTINUE );
}

/*
 * Get the module of a cue.
 */
imp_mod_handle DIPIMPENTRY( CueMod )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    iih = iih;
    return( icueh->imh );
}

/*
 * Get the filename of a cue.
 */
size_t DIPIMPENTRY( CueFile )( imp_image_handle *iih, imp_cue_handle *icueh,
                                 char *buff, size_t buff_size )
{
    unsigned_8  name_len;
    const char *file_name;

    if( VMGetU8( iih, icueh->file, &name_len)
     && (file_name = VMBlock( iih, icueh->file + 1, name_len ) ) != NULL ) {
        return( hllNameCopy( buff, file_name, buff_size, name_len ) );
    }

    return( 0 );
}

/*
 * Get the 'file id'.
 */
cue_fileid DIPIMPENTRY( CueFileId )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih;

    return( icueh->file );
}

/*
 * Get the linenumber.
 */
unsigned long DIPIMPENTRY( CueLine )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    unsigned_8 * entries;

    /* Load and sort the line number. */
    entries = hllGetSortedLinnums( iih, icueh );
    if( !entries ) {
        return( DS_ERR | DS_FAIL );
    }

    switch( icueh->style ) {
    case HLL_LINE_STYLE_SRC_LINES:
    case HLL_LINE_STYLE_SRC_LINES_SEG_16:
        {
            cv3_linnum_entry_16 *linnum;
            linnum = (cv3_linnum_entry_16 *)( entries + icueh->cur_line * sizeof( *linnum ) );
            return( linnum->line );
        }
        break;

    case HLL_LINE_STYLE_SRC_LINES_SEG_32:
        {
            cv3_linnum_entry_32 *linnum;
            linnum = (cv3_linnum_entry_32 *)( entries + icueh->cur_line * sizeof( *linnum ) );
            return( linnum->line );
        }
        break;

    case HLL_LINE_STYLE_HLL_01:
    case HLL_LINE_STYLE_HLL_02:
        {
            hl1_linnum_entry *linnum;
            linnum = (hl1_linnum_entry *)( entries + icueh->cur_line * sizeof( *linnum ) );
            return( linnum->line );
        }
        break;

    case HLL_LINE_STYLE_HLL_03:
    case HLL_LINE_STYLE_HLL_04:
        {
            hl3_linnum_entry *linnum;
            linnum = (hl3_linnum_entry *)( entries + icueh->cur_line * sizeof( *linnum ) );
            return( linnum->line );
        }
        break;
    }
    return( 0 );
}

/*
 * Get the column number - we've got no such information.
 */
unsigned DIPIMPENTRY( CueColumn )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih; (void)icueh;

    return( 0 );
}

/*
 * Get the address.
 */
address DIPIMPENTRY( CueAddr )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    unsigned_8 *entries;
    address     addr;

    /* Load and sort the line number. */
    entries = hllGetSortedLinnums( iih, icueh );
    if( !entries ) {
        return( NilAddr );
    }

    /* get addr.offset */
    switch( icueh->style ) {
    case HLL_LINE_STYLE_SRC_LINES:
    case HLL_LINE_STYLE_SRC_LINES_SEG_16:
        {
            cv3_linnum_entry_16 *linnum;
            linnum = (cv3_linnum_entry_16 *)( entries + icueh->cur_line * sizeof( *linnum ) );
            addr.mach.offset = linnum->offset;
        }
        break;

    case HLL_LINE_STYLE_SRC_LINES_SEG_32:
        {
            cv3_linnum_entry_32 *linnum;
            linnum = (cv3_linnum_entry_32 *)( entries + icueh->cur_line * sizeof( *linnum ));
            addr.mach.offset = linnum->offset;
        }
        break;

    case HLL_LINE_STYLE_HLL_01:
    case HLL_LINE_STYLE_HLL_02:
        {
            hl1_linnum_entry *linnum;
            linnum = (hl1_linnum_entry *)( entries + icueh->cur_line * sizeof( *linnum ) );
            addr.mach.offset = linnum->offset;
        }
        break;

    case HLL_LINE_STYLE_HLL_03:
    case HLL_LINE_STYLE_HLL_04:
        {
            hl3_linnum_entry *linnum;
            linnum = (hl3_linnum_entry *)( entries + icueh->cur_line * sizeof( *linnum ) );
            addr.mach.offset = linnum->offset + icueh->u.hll.base_offset;
        }
        break;
    }

    /* Construct the address and return. */
    addr.indirect = 0;
    addr.sect_id = 0;
    addr.mach.segment = icueh->segment;
    hllMapLogical( iih, &addr );
    return( addr );
}

/*
 * Adjust the 'src' cue by 'adj' amount and return the result in 'dst'.
 *
 * That is, If you get called with "DIPImpCueAdjust( iih, src, 1, dst )",
 * the 'dst' handle should be filled in with implementation cue handle
 * representing the source cue immediately following the 'src' cue.
 * Passing in an 'adj' of -1 will get the immediately preceeding source
 * cue. The list of source cues for each file are considered a ring,
 * so if 'src' is the first cue in a file, an 'adj' of -1 will return
 * the last source cue FOR THAT FILE. The cue adjust never crosses a
 * file boundry. Also, if 'src' is the last cue in a file, and 'adj' of
 * 1 will get the first source cue FOR THAT FILE. If an adjustment
 * causes a wrap from begining to end or vis-versa, you should return
 * DS_WRAPPED status (NOTE: DS_ERR should *not* be or'd in, nor should
 * DCStatus be called in this case). Otherwise DS_OK should be returned
 * unless an error occurred.
 *
 * Reminder: A cue is a source file location. For HLL/CV it's a line for
 *           which code was generated.
 */
dip_status DIPIMPENTRY( CueAdjust )( imp_image_handle *iih, imp_cue_handle *src_icueh,
                                     int adj, imp_cue_handle *dst_icueh )
{
    dip_status ds;

    /* unused parameters */ (void)iih;

    HLL_LOG(( "DIPImpCueAdjust: ic=%p:{cur_line=%lu file=%#lx} adj=%d aic=%p",
              src_icueh, (long)ic->cur_line, (long)src_icueh->file, adj, dst_icueh ));

    /*
     * Since we sort the linnumbers this walking is extremely simple.
     */
    ds = DS_OK;
    *dst_icueh = *src_icueh;

    switch( src_icueh->style ) {
    case HLL_LINE_STYLE_SRC_LINES:
    case HLL_LINE_STYLE_SRC_LINES_SEG_16:
    case HLL_LINE_STYLE_SRC_LINES_SEG_32:
        while( adj > 0 ) {
            dst_icueh->cur_line++;
            if( dst_icueh->cur_line >= dst_icueh->num_lines ) {
                dst_icueh->cur_line = 0;
                ds = DS_WRAPPED;
            }
            adj--;
        }
        while( adj < 0 ) {
            if( dst_icueh->cur_line != 0 ) {
                dst_icueh->cur_line--;
            } else {
                dst_icueh->cur_line = dst_icueh->num_lines - 1;
                ds = DS_WRAPPED;
            }
            adj++;
        }
        break;

    default:
        hllConfused();
    case HLL_LINE_STYLE_HLL_01:
    case HLL_LINE_STYLE_HLL_02:
    case HLL_LINE_STYLE_HLL_03:
    case HLL_LINE_STYLE_HLL_04:
        while( adj > 0 ) {
            dst_icueh->cur_line++;
            if( dst_icueh->cur_line > dst_icueh->u.hll.last ) {
                dst_icueh->cur_line = dst_icueh->u.hll.first;
                ds = DS_WRAPPED;
            }
            adj--;
        }
        while( adj < 0 ) {
            if( dst_icueh->cur_line > dst_icueh->u.hll.first ) {
                dst_icueh->cur_line--;
            } else {
                dst_icueh->cur_line = dst_icueh->u.hll.last;
                ds = DS_WRAPPED;
            }
            adj++;
        }
        break;
    }

    HLL_LOG(( " -> %d (%ld)\n", ds, (long)dst_icueh->cur_line ));
    return( ds );
}

/* line search state data */
typedef struct {
    cue_fileid      file;               /* The file we're looking for. */
    unsigned_16     line;               /* The line we're looking for. */
    imp_cue_handle  best_icueh;         /* The best cue so far. */
    search_result   rc;                 /* The search result. */
} hll_find_line_cue_in_file;


/*
 * Search a file for a line number in a file.
 */
static walk_result hllFindLineCueInFile( imp_image_handle *iih, imp_cue_handle *icueh, void *_state )
{
    hll_find_line_cue_in_file  *state = _state;
    unsigned_16                 entry_size;
    unsigned_8                 *entries;
    unsigned_16                 i;
    unsigned_16                 start;
    unsigned_16                 last;
    unsigned_16                *line_num;

    /*
     * Does the file id match?
     */
    if( icueh->file != state->file && state->file != 0 ) {
        return( WR_CONTINUE );
    }

    /*
     * Simple first line query?
     */
    if( state->line == 0 ) {
        state->best_icueh = *icueh;
        state->rc = SR_EXACT;
        return( WR_STOP );
    }

    /*
     * Do a binary search for the line number.
     *
     * We exploit the fact that all the linnum entry structs starts with
     * the a 16-bit linenumber member.
     */
    entries = hllGetSortedLinnums( iih, icueh );
    if( !entries ) {
        return( WR_FAIL );
    }

    entry_size = hllLinnumSize( icueh );
    if( icueh->style >= HLL_LINE_STYLE_HLL_01 ) {
        start = icueh->u.hll.first;
        last = icueh->u.hll.last;
    } else {
        start = 0;
        last = icueh->num_lines - 1;
    }
    for( ;; ) {
        i = start + ((last - start) / 2);
        line_num = (unsigned_16 *)(entries + i * entry_size);
        if( start >= last ) {
            break;
        }
        if( *line_num == state->line ) {
            break;
        }
        if( *line_num < state->line ) {
            start = i + 1;
        } else {
            last = i ? i - 1 : 0;
        }
    }

    /*
     * We adjust the result, partly because I didn't proove that the search code
     * is 100% exact, and mainly because there can be multiplentries for the same line.
     * We always return the first cue for a line, even if the match isn't exact.
     */
    if( icueh->style >= HLL_LINE_STYLE_HLL_01 ) {
        start = icueh->u.hll.first;
        last = icueh->u.hll.last;
    } else {
        start = 0;
        last = icueh->num_lines - 1;
    }

    while( i < last ) {
        unsigned_16 *next = (unsigned_16 *)(entries + (i + 1) * entry_size);
        if( *next > state->line ) {
            break;
        }
        line_num = next;
        i++;
    }

    while( i > start ) {
        unsigned_16 *prev = (unsigned_16 *)(entries + (i - 1) * entry_size);
        if( *line_num <= state->line
         && *prev < *line_num ) {
            break;
        }
        line_num = prev;
        i--;
    }

    /* Set the return values on success. */
    if( *line_num <= state->line ) {
        state->rc = *line_num == state->line ? SR_EXACT : SR_CLOSEST;
        state->best_icueh = *icueh;
        state->best_icueh.cur_line = i;
    }
    return( WR_STOP );
}

/*
 * Search for a source cue in the given module.
 *
 * If 'file' is zero, use the main source file of the module.
 * If 'line' is zero, use the first line with a source cue
 * in the given file.
 * If 'column' is zero, use the first column with a source cue
 * in the given line.
 *
 * Fill in the '*icueh' handle with the result. If there was a cue at
 * exactly the file/line/column specified return SR_EXACT. If there
 * are cues with in the file with a line/column less than the given
 * values, return the largest cue possible that is less then the
 * passed in values and return SR_CLOSEST. If there are no cues with
 * the proper characteristics, return SR_NONE
 */
search_result DIPIMPENTRY( LineCue )( imp_image_handle *iih, imp_mod_handle imh,
                                      cue_fileid file, unsigned long line,
                                      unsigned column, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)column;

    /*
     * Do it the simple way, i.e. use DIPImpWalkFileList.
     *
     * We're ASSUMING, potentially incorrectly, that the first source file
     * we encounter is the primary one. We'll address that later if it
     * becomes a problem.
     */
    walk_result                 walk_rc;
    hll_find_line_cue_in_file   state = {0};
    state.file = file;
    state.line = line;
//    if( state.line > UINT16_MAX )
//        state.line = UINT16_MAX;
    state.rc   = SR_NONE;
    walk_rc = DIPImpWalkFileList( iih, imh, hllFindLineCueInFile, icueh, &state );
    HLL_LOG(( "DIPImpLineCue: mod=%x file=%lx line=%lx -> %d (%lx)\n",
              imh, file, line, state.rc, state.best_icueh.cur_line ));
    if( walk_rc == WR_FAIL) {
        return( SR_FAIL );
    }
    if( state.rc != SR_NONE ) {
        *icueh = state.best_icueh;
    }
    return( state.rc );
}


/* Address search state data */
typedef struct {
    address         addr;                   /* what we're looking for. */
    unsigned_32     best_delta;             /* The delta between addr and the best cue. */
    imp_cue_handle  best_icueh;             /* The best cue so far. */
} hll_find_addr_cue_in_file;


/*
 * Search a file for the line number closes to a given address.
 */
static walk_result hllFindAddrCueInFile( imp_image_handle *iih, imp_cue_handle *icueh, void *_state )
{
    hll_find_addr_cue_in_file  *state = _state;
    void                       *entries;
    address                     addr = {0};
    unsigned_32                 map_offset;
    unsigned_16                 i;

    /*
     * Check if this segment is the right one, and get the mapping offset of the segment.
     */
    addr.mach.segment = icueh->segment;
    hllMapLogical( iih, &addr );
    if( addr.mach.segment != state->addr.mach.segment ) {
        return( WR_CONTINUE );
    }
    /* logical mapping offset. */
    if( icueh->segment <= iih->seg_count ) {
        map_offset = iih->segments[icueh->segment - 1].map.offset;
    } else {
        map_offset = 0;
    }

    /*
     * Traverse the line numbers.
     */
    entries = hllGetSortedLinnums( iih, icueh );
    if( !entries ) {
        return( WR_FAIL );
    }
    i = icueh->num_lines;
    switch( icueh->style ) {
    case HLL_LINE_STYLE_SRC_LINES:
    case HLL_LINE_STYLE_SRC_LINES_SEG_16:
        {
            cv3_linnum_entry_16 *linnum = entries;
            while( i-- > 0 ) {
                if( state->addr.mach.offset >= ( linnum[i].offset + map_offset ) ) {
                    unsigned_32 delta = state->addr.mach.offset - ( linnum[i].offset + map_offset );
                    if( state->best_delta >= delta ) {
                        state->best_delta = delta;
                        state->best_icueh = *icueh;
                        state->best_icueh.cur_line = i;
                        if( delta == 0 ) {
                            return( WR_STOP );
                        }
                    }
                }
            }
        }
        break;

    case HLL_LINE_STYLE_SRC_LINES_SEG_32:
        {
            cv3_linnum_entry_32 *linnum = entries;
            while( i-- > 0 ) {
                if( state->addr.mach.offset >= ( linnum[i].offset + map_offset ) ) {
                    unsigned_32 delta = state->addr.mach.offset - ( linnum[i].offset + map_offset );
                    if( state->best_delta >= delta ) {
                        state->best_delta = delta;
                        state->best_icueh = *icueh;
                        state->best_icueh.cur_line = i;
                        if( delta == 0 ) {
                            return( WR_STOP );
                        }
                    }
                }
            }
        }
        break;

        /* Being kind of lazy, we realize that hl1_linnum_entry and hl3_linnum_entry
         * only differs in the file index size and the flag field. So, since we'll
         * be accessing the offset only, we can use hl1_linnum_entry for all the HLL
         * stuff.
         */
    default:
        hllConfused();
    case HLL_LINE_STYLE_HLL_01:
    case HLL_LINE_STYLE_HLL_02:
    case HLL_LINE_STYLE_HLL_03:
    case HLL_LINE_STYLE_HLL_04:
        {
            hl1_linnum_entry *linnum = entries;
            map_offset += icueh->u.hll.base_offset;
            while( i-- > 0 ) {
                if( state->addr.mach.offset >= ( linnum[i].offset + map_offset ) ) {
                    unsigned_32 delta = state->addr.mach.offset - ( linnum[i].offset + map_offset );
                    if( state->best_delta >= delta ) {
                        state->best_delta = delta;
                        state->best_icueh = *icueh;
                        state->best_icueh.cur_line = i;
                        if( delta == 0 ) {
                            return( WR_STOP );
                        }
                    }
                }
            }
        }
        break;
    }

    return( WR_CONTINUE );
}

/*
 * Search for the closest cue in the given module that has an address
 * less then or equal to the given address. If there is no such cue
 * return SR_NONE. If there is one exactly at the address return
 * SR_EXACT. Otherwise, return SR_CLOSEST.
 */
search_result DIPIMPENTRY( AddrCue )( imp_image_handle *iih, imp_mod_handle imh,
                                      address addr, imp_cue_handle *icueh )
{
    /*
     * It's likely that the line number information of a module, at least HLL,
     * origianlly is sorted by address. That could enable us to do a binary
     * search of the table.
     *
     * However, since we're resorting the line numbers for greatly
     * simplifying the walking done by DIPImpCueAdjust, we'll have to do
     * a linear search here. Should this function prove more frequently
     * used and suffer heavily performance wise from this, we should
     * reconsider the DIPImpCueAdjust and DIPImpWalkFileList implementations.
     */
    walk_result                 walk_rc;
    search_result               rc;
    hll_find_addr_cue_in_file   state = {0};
    state.addr = addr;
    state.best_delta = INT32_MAX;
    walk_rc = DIPImpWalkFileList( iih, imh, hllFindAddrCueInFile, icueh, &state );
    if( walk_rc != WR_FAIL) {
        if( state.best_delta != INT32_MAX ) {
            *icueh = state.best_icueh;
            rc = ( walk_rc == WR_STOP ) ? SR_EXACT : SR_CLOSEST;
        } else {
            rc = SR_NONE;
        }
    } else {
        rc = SR_FAIL;
    }
    HLL_LOG(( "DIPImpAddrCue: mod=%x addr=%04x:%08lx -> %d (%lx)\n",
              imh, addr.mach.segment, (long)addr.mach.offset, rc, state.best_icueh.cur_line ));
    return( rc );
}

/*
 * Compare two cue handles and return 0 if they refer to the same information.
 *
 * If they refer to differnt things return either a positive or negative value
 * to impose an 'order' on the information. The value should obey the following
 * constraints. Given three handles H1, H2, H3:
 *      - if H1 < H2 then H1 is always < H2
 *      - if H1 < H2 and H2 < H3 then H1 is < H3
 * The reason for the constraints is so that a client can sort a list of handles
 * and binary search them.
 */
int DIPIMPENTRY( CueCmp )( imp_image_handle *iih, imp_cue_handle *icueh1, imp_cue_handle *icueh2 )
{
    /* unused parameters */ (void)iih;

    if( icueh1->imh < icueh2->imh )
        return( -1 );
    if( icueh1->imh > icueh2->imh )
        return( 1 );
    if( icueh1->lines < icueh2->lines )
        return( -1 );
    if( icueh1->lines > icueh2->lines )
        return( 1 );
    if( icueh1->cur_line < icueh2->cur_line )
        return( -1 );
    if( icueh1->cur_line > icueh2->cur_line )
        return( 1 );
    return( 0 );
}
