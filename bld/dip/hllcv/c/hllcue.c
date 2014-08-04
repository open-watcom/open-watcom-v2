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
* Description:  HLL/CV source line cue support.
*
****************************************************************************/


#include "hllinfo.h"
#include <stdint.h>
#include "walloca.h"


/*
 * Gets the size of a line number entry.
 */
static unsigned hllLinnumSize( imp_cue_handle *ic )
{
    switch( ic->style ) {
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
static void hllLinnumDtor( imp_image_handle *ii, void *entries)
{
    int i;
    for( i = 0; i < HLLINFO_NUM_SORTED; i++ ) {
        if( ii->sorted_linnum_blocks[i] == entries ) {
            ii->sorted_linnum_blocks[i] = NULL;
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
static void *hllGetSortedLinnums( imp_image_handle *ii, imp_cue_handle *ic )
{
    const unsigned  entry_size = hllLinnumSize( ic );
    void           *entries;
    int             i;

    entries = VMBlock( ii, ic->lines, ic->num_lines * entry_size );
    if( !entries ) {
        return( NULL );
    }

    /* already sorted? */
    for( i = 0; i < HLLINFO_NUM_SORTED; i++ ) {
        if( ii->sorted_linnum_blocks[i] == entries ) {
            return( entries );
        }
    }

    /* sort and register a destructor */
    switch( ic->style ) {
    case HLL_LINE_STYLE_SRC_LINES:
    case HLL_LINE_STYLE_SRC_LINES_SEG_16:
        qsort( entries, ic->num_lines, entry_size, hllLinnumCmp16CV3 );
        break;
    case HLL_LINE_STYLE_SRC_LINES_SEG_32:
        qsort( entries, ic->num_lines, entry_size, hllLinnumCmp32CV3 );
        break;
    case HLL_LINE_STYLE_HLL_01:
    case HLL_LINE_STYLE_HLL_02:
        qsort( entries, ic->num_lines, entry_size, hllLinnumCmpHL1 );
        break;
    default:
        hllConfused();
    case HLL_LINE_STYLE_HLL_03:
    case HLL_LINE_STYLE_HLL_04:
        qsort( entries, ic->num_lines, entry_size, hllLinnumCmpHL3 );
        break;
    }

    VMAddDtor( ii, ic->lines, hllLinnumDtor, entries );

    for( i = 0; i < HLLINFO_NUM_SORTED; i++ ) {
        if( !ii->sorted_linnum_blocks[i] ) {
            ii->sorted_linnum_blocks[i] = entries;
            return( entries );
        }
    }

    i = ((uintptr_t)entries >> 7) % HLLINFO_NUM_SORTED;
    ii->sorted_linnum_blocks[i] = entries;
    return( entries );
}


/*
 * Walk the list of source files of a module.
 */
walk_result DIGENTRY DIPImpWalkFileList( imp_image_handle *ii,
                                         imp_mod_handle im, IMP_CUE_WKR *wk,
                                         imp_cue_handle *ic, void *d )
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
    hde = hllFindDirEntry( ii, im, hll_sstSrcLines );
    if( hde ) {
        /*
         *  while (more data) {
         *      +0      DB  filename len
         *      +1..n   DB  filename
         *      n       DW  number of entries
         *      while (more entries) {
         *          n + 2   DW  line number
         *          n + 4   DW  code offset
         *      }
         * }
         */
        unsigned_32         pos;
        const unsigned_32   entry_size = sizeof( cv3_linnum_entry_16 );

        ic->style       = HLL_LINE_STYLE_SRC_LINES;
        ic->im          = im;
        ic->cur_line    = 0;
        ic->segment     = 1;
        pos = 0;
        while( pos < hde->cb ) {
            unsigned_8  name_len;

            /* read the header */
            ic->num_lines = 0;
            if( !VMSsGetU8( ii, hde, pos, &name_len )
             || !VMSsGetU16( ii, hde, pos + 1 + name_len, (unsigned_16 *)&ic->num_lines )
                ) {
                return( WR_FAIL );
            }

            ic->file    = hde->lfo + pos;
            ic->lines   = ic->file + 1 + name_len + 2;

            pos += 1 + name_len + 2 + ic->num_lines * entry_size;
            if( pos > hde->cb ) {
                return( WR_FAIL );
            }

            if( ic->num_lines > 0  ) {
                wr = wk( ii, ic, d );
                if( wr != WR_CONTINUE ) {
                    return( wr );
                }
            }
        }
    }

    /*
     * Old line numbers.
     */
    hde = hllFindDirEntry( ii, im, hll_sstSrcLnSeg );
    if( hde ) {
        /*
         * while (more data) {
         *      +0      DB  filename len
         *      +1..n   DB  filename
         *      n       DW  segment index
         *      n + 2   DW  number of entries
         *      while (more entries) {
         *          if (32-bit) {
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

        if( ii->format_lvl == HLL_LVL_NB00
         || ii->format_lvl == HLL_LVL_NB02 ) {
            entry_size = sizeof( cv3_linnum_entry_16 );
            ic->style  = HLL_LINE_STYLE_SRC_LINES_SEG_16;
        } else {
            entry_size = sizeof( cv3_linnum_entry_32 );;
            ic->style  = HLL_LINE_STYLE_SRC_LINES_SEG_32;
        }
        ic->im         = im;
        ic->cur_line   = 0;
        pos = 0;
        while( pos < hde->cb ) {
            unsigned_8  name_len;

            /* read the header */
            ic->num_lines = 0;
            if( !VMSsGetU8( ii, hde, pos, &name_len )
             || !VMSsGetU16( ii, hde, pos + 1 + name_len, &ic->segment )
             || !VMSsGetU16( ii, hde, pos + 1 + name_len + 2, (unsigned_16 *)&ic->num_lines )
                ) {
                return( WR_FAIL );
            }

            ic->file   = hde->lfo + pos;
            ic->lines  = ic->file + 1 + name_len + 4;

            pos += 1 + name_len + 4 + ic->num_lines * entry_size;
            if( pos > hde->cb ) {
                return( WR_FAIL );
            }

            if( ic->num_lines > 0  ) {
                wr = wk( ii, ic, d );
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
    hde = hllFindDirEntry( ii, im, hll_sstHLLSrc );
    if( hde )  {
        unsigned_32     pos = 0;
        unsigned_32     num_files;
        unsigned_32     end_files;
        unsigned_32     first_sfi;
        unsigned        i;

        ic->im          = im;
        ic->cur_line    = 0;
        ic->lines       = 0;
        ic->file        = 0;
        ic->u.hll.base_offset = 0;
        ic->u.hll.first = 0;
        ic->u.hll.last  = 0;

        /*
         * Find the two tables.
         */
        switch( hllGetModStyle( ii, im ) ) {
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

                ic->style  = HLL_LINE_STYLE_HLL_01;

                /* get the 'first' record. */
                first = VMSsBlock( ii, hde, pos, sizeof( *first ) );
                if( !first ) {
                    return( WR_FAIL );
                }
                if( first->entry_type != HLL_LNE_TYPE_SOURCE ) {
                    hllConfused(); /* see ASSUMPTION above */
                    return( WR_FAIL );
                }

                end_files = hde->cb;
                ic->segment = 1;
                ic->lines = pos + sizeof( *first );
                ic->num_lines = first->num_line_entries;
                ic->file = ic->lines
                         + ic->num_lines * sizeof( hl1_linnum_entry )
                         + first->num_path_entries * sizeof( hl1_pathtab_entry );

                /* get the file table header */
                hdr = VMSsBlock( ii, hde, ic->file, sizeof( *hdr ) );
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
            ic->style  = HLL_LINE_STYLE_HLL_02;
            while ( pos < hde->cb && ( !ic->lines || !ic->file ) ) {
                hl2_linnum_first   *first;
                hl1_filetab_hdr    *hdr;
                unsigned_32         pos_next;

                /* get the 'first' record. */
                first = VMSsBlock( ii, hde, pos, sizeof( *first ) );
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
                    ic->segment = first->seg;
                    ic->lines = pos + sizeof( *first );
                    ic->num_lines = first->num_line_entries;
                }

                if ( first->file_tab_size > sizeof( hl1_filetab_hdr ) ) {
                    ic->file = ic->lines
                             + ic->num_lines * sizeof( hl1_linnum_entry );
                    end_files = ic->file + first->file_tab_size;

                    /* get the file table header */
                    hdr = VMSsBlock( ii, hde, ic->file, sizeof( *hdr ) );
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
            ic->style  = HLL_LINE_STYLE_HLL_03;
            while ( pos < hde->cb && ( !ic->lines || !ic->file ) ) {
                hl2_linnum_first   *first;
                hl1_filetab_hdr    *hdr;
                unsigned_32         pos_next;

                /* get the 'first' record. */
                first = VMSsBlock( ii, hde, pos, sizeof( *first ) );
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
                    ic->segment = first->seg;
                    ic->lines = pos + sizeof( *first );
                    ic->num_lines = first->num_line_entries;
                }

                if ( first->file_tab_size > sizeof( hl1_filetab_hdr ) ) {
                    ic->file = pos + sizeof( *first )
                             + ic->num_lines * sizeof( hl3_linnum_entry );
                    end_files = ic->file + first->file_tab_size;

                    /* get the file table header */
                    hdr = VMSsBlock( ii, hde, ic->file, sizeof( *hdr ) );
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
            ic->style  = HLL_LINE_STYLE_HLL_04;
            while ( pos < hde->cb && ( !ic->lines || !ic->file ) ) {
                union {
                    hl4_linnum_first_core   core;
                    hl4_linnum_first_files  files;
                    hl4_linnum_first_lines  lines;
                }                   *first;
                hl1_filetab_hdr     *hdr;
                unsigned_32         pos_next;

                /* get the 'first' record. */
                first = VMSsBlock( ii, hde, pos, sizeof( *first ) );
                if( !first ) {
                    return( WR_FAIL );
                }
                switch( first->core.entry_type ) {
                case HLL_LNE_TYPE_SOURCE:
                    ic->segment = first->lines.seg;
                    ic->lines = pos + sizeof( *first );
                    ic->num_lines = first->lines.num_line_entries;
                    ic->u.hll.base_offset = first->lines.base_offset;
                    if( ic->segment
                     && ii->seg_count > ic->segment
                     && ic->u.hll.base_offset >= ii->segments[ic->segment-1].address ){
                        ic->u.hll.base_offset -= ii->segments[ic->segment-1].address;
                    }
                    pos_next = pos
                             + first->lines.num_line_entries * sizeof( hl3_linnum_entry );
                    break;

                case HLL_LNE_TYPE_FILE_TABLE:
                    ic->file = pos + sizeof( *first );
                    end_files = ic->file + first->files.file_tab_size;

                    /* get the file table header */
                    hdr = VMSsBlock( ii, hde, ic->file, sizeof( *hdr ) );
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
        ic->lines += hde->lfo;
        ic->u.hll.first = 0;
        ic->u.hll.last  = 0;

        first_sfi++; /* one based */

        /*
         * Iterate the two tables in parallel.
         */
        pos = ic->file + sizeof( hl1_filetab_hdr );
        for( i = 0; i < num_files && pos < end_files; i++ ) {
            union {
                hl1_linnum_entry hl1;
                hl3_linnum_entry hl3;
            }              *entries;
            unsigned_16     sfi;
            unsigned_8      file_len;

            /* get the line numbers (again). */
            entries = hllGetSortedLinnums( ii, ic );
            if( !entries ) {
                return( WR_FAIL );
            }
            sfi = ic->style < HLL_LINE_STYLE_HLL_03
                ? entries[ic->u.hll.first].hl1.sfi
                : entries[ic->u.hll.first].hl3.sfi;
            if( sfi == i + first_sfi) {
                /* find the end of the line numbers for this file. */
                ic->u.hll.last = ic->u.hll.first + 1;
                while( ic->u.hll.last < ic->num_lines
                    && (  ic->style < HLL_LINE_STYLE_HLL_03
                        ? entries[ic->u.hll.last].hl1.sfi
                        : entries[ic->u.hll.last].hl3.sfi )
                       == sfi ) {
                    ic->u.hll.last++;
                }
                ic->u.hll.last--;
                ic->file = hde->lfo + pos;

                /* do the callback */
                wr = wk( ii, ic, d );
                if( wr != WR_CONTINUE ) {
                    return( wr );
                }

                /* advance the current (hll) line number. */
                if( ++ic->u.hll.last >= ic->num_lines ) {
                    break;
                }
                ic->cur_line = ic->u.hll.first = ic->u.hll.last;
            }

            /* next file name */
            if (!VMSsGetU8( ii, hde, pos, &file_len )) {
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
imp_mod_handle DIGENTRY DIPImpCueMod( imp_image_handle *ii, imp_cue_handle *ic )
{
    ii = ii;
    return( ic->im );
}

/*
 * Get the filename of a cue.
 */
unsigned DIGENTRY DIPImpCueFile( imp_image_handle *ii, imp_cue_handle *ic,
                                 char *buf, unsigned max )
{
    unsigned_8  name_len;
    const char *file_name;

    if( VMGetU8( ii, ic->file, &name_len)
     && (file_name = VMBlock( ii, ic->file + 1, name_len ) ) != NULL ) {
        return( hllNameCopy( buf, file_name, max, name_len ) );
    }

    return( 0 );
}

/*
 * Get the 'file id'.
 */
cue_fileid  DIGENTRY DIPImpCueFileId( imp_image_handle *ii,
                                      imp_cue_handle *ic )
{
    ii = ii;
    return( ic->file );
}

/*
 * Get the linenumber.
 */
unsigned long   DIGENTRY DIPImpCueLine( imp_image_handle *ii,
                                        imp_cue_handle *ic )
{
    unsigned_8 * entries;

    /* Load and sort the line number. */
    entries = hllGetSortedLinnums( ii, ic );
    if( !entries ) {
        return( DS_ERR | DS_FAIL );
    }

    switch( ic->style ) {
    case HLL_LINE_STYLE_SRC_LINES:
    case HLL_LINE_STYLE_SRC_LINES_SEG_16:
        {
            cv3_linnum_entry_16 *linnum;
            linnum = (cv3_linnum_entry_16 *)( entries + ic->cur_line * sizeof( *linnum ) );
            return( linnum->line );
        }
        break;

    case HLL_LINE_STYLE_SRC_LINES_SEG_32:
        {
            cv3_linnum_entry_32 *linnum;
            linnum = (cv3_linnum_entry_32 *)( entries + ic->cur_line * sizeof( *linnum ) );
            return( linnum->line );
        }
        break;

    case HLL_LINE_STYLE_HLL_01:
    case HLL_LINE_STYLE_HLL_02:
        {
            hl1_linnum_entry *linnum;
            linnum = (hl1_linnum_entry *)( entries + ic->cur_line * sizeof( *linnum ) );
            return( linnum->line );
        }
        break;

    case HLL_LINE_STYLE_HLL_03:
    case HLL_LINE_STYLE_HLL_04:
        {
            hl3_linnum_entry *linnum;
            linnum = (hl3_linnum_entry *)( entries + ic->cur_line * sizeof( *linnum ) );
            return( linnum->line );
        }
        break;
    }
    return( 0 );
}

/*
 * Get the column number - we've got no such information.
 */
unsigned DIGENTRY DIPImpCueColumn( imp_image_handle *ii, imp_cue_handle *ic )
{
    ii = ii; ic = ic;
    return( 0 );
}

/*
 * Get the address.
 */
address DIGENTRY DIPImpCueAddr( imp_image_handle *ii, imp_cue_handle *ic )
{
    unsigned_8 *entries;
    address     addr;

    /* Load and sort the line number. */
    entries = hllGetSortedLinnums( ii, ic );
    if( !entries ) {
        return( NilAddr );
    }

    /* get addr.offset */
    switch( ic->style ) {
    case HLL_LINE_STYLE_SRC_LINES:
    case HLL_LINE_STYLE_SRC_LINES_SEG_16:
        {
            cv3_linnum_entry_16 *linnum;
            linnum = (cv3_linnum_entry_16 *)( entries + ic->cur_line * sizeof( *linnum ) );
            addr.mach.offset = linnum->offset;
        }
        break;

    case HLL_LINE_STYLE_SRC_LINES_SEG_32:
        {
            cv3_linnum_entry_32 *linnum;
            linnum = (cv3_linnum_entry_32 *)( entries + ic->cur_line * sizeof( *linnum ));
            addr.mach.offset = linnum->offset;
        }
        break;

    case HLL_LINE_STYLE_HLL_01:
    case HLL_LINE_STYLE_HLL_02:
        {
            hl1_linnum_entry *linnum;
            linnum = (hl1_linnum_entry *)( entries + ic->cur_line * sizeof( *linnum ) );
            addr.mach.offset = linnum->offset;
        }
        break;

    case HLL_LINE_STYLE_HLL_03:
    case HLL_LINE_STYLE_HLL_04:
        {
            hl3_linnum_entry *linnum;
            linnum = (hl3_linnum_entry *)( entries + ic->cur_line * sizeof( *linnum ) );
            addr.mach.offset = linnum->offset + ic->u.hll.base_offset;
        }
        break;
    }

    /* Construct the address and return. */
    addr.indirect = 0;
    addr.sect_id = 0;
    addr.mach.segment = ic->segment;
    hllMapLogical( ii, &addr );
    return( addr );
}

/*
 * Adjust the 'src' cue by 'adj' amount and return the result in 'dst'.
 *
 * That is, If you get called with "DIPImpCueAdjust( ii, src, 1, dst )",
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
dip_status DIGENTRY DIPImpCueAdjust( imp_image_handle *ii, imp_cue_handle *ic,
                                     int adj, imp_cue_handle *aic )
{
    dip_status rc;
    HLL_LOG(( "DIPImpCueAdjust: ic=%p:{cur_line=%lu file=%#lx} adj=%d aic=%p",
              ic, (long)ic->cur_line, (long)ic->file, adj, aic ));

    /*
     * Since we sort the linnumbers this walking is extremely simple.
     */
    rc = DS_OK;
    *aic = *ic;

    switch( ic->style ) {
    case HLL_LINE_STYLE_SRC_LINES:
    case HLL_LINE_STYLE_SRC_LINES_SEG_16:
    case HLL_LINE_STYLE_SRC_LINES_SEG_32:
        while( adj > 0 ) {
            aic->cur_line++;
            if( aic->cur_line >= aic->num_lines ) {
                aic->cur_line = 0;
                rc = DS_WRAPPED;
            }
            adj--;
        }
        while( adj < 0 ) {
            if( aic->cur_line != 0 ) {
                aic->cur_line--;
            } else {
                aic->cur_line = aic->num_lines - 1;
                rc = DS_WRAPPED;
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
            aic->cur_line++;
            if( aic->cur_line > aic->u.hll.last ) {
                aic->cur_line = aic->u.hll.first;
                rc = DS_WRAPPED;
            }
            adj--;
        }
        while( adj < 0 ) {
            if( aic->cur_line > aic->u.hll.first ) {
                aic->cur_line--;
            } else {
                aic->cur_line = aic->u.hll.last;
                rc = DS_WRAPPED;
            }
            adj++;
        }
        break;
    }

    HLL_LOG(( " -> %d (%ld)\n", rc, (long)aic->cur_line ));
    return( rc );
}

/* line search state data */
typedef struct {
    cue_fileid      file;               /* The file we're looking for. */
    unsigned_16     line;               /* The line we're looking for. */
    imp_cue_handle  best;               /* The best cue so far. */
    search_result   rc;                 /* The search result. */
} hll_find_line_cue_in_file;


/*
 * Search a file for a line number in a file.
 */
static walk_result hllFindLineCueInFile( imp_image_handle *ii, imp_cue_handle *ic, void *_state )
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
    if( ic->file != state->file && state->file != 0 ) {
        return( WR_CONTINUE );
    }

    /*
     * Simple first line query?
     */
    if( state->line == 0 ) {
        state->best = *ic;
        state->rc = SR_EXACT;
        return( WR_STOP );
    }

    /*
     * Do a binary search for the line number.
     *
     * We exploit the fact that all the linnum entry structs starts with
     * the a 16-bit linenumber member.
     */
    entries = hllGetSortedLinnums( ii, ic );
    if( !entries ) {
        return( WR_FAIL );
    }

    entry_size = hllLinnumSize( ic );
    if( ic->style >= HLL_LINE_STYLE_HLL_01 ) {
        start = ic->u.hll.first;
        last = ic->u.hll.last;
    } else {
        start = 0;
        last = ic->num_lines - 1;
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
    if( ic->style >= HLL_LINE_STYLE_HLL_01 ) {
        start = ic->u.hll.first;
        last = ic->u.hll.last;
    } else {
        start = 0;
        last = ic->num_lines - 1;
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
        state->best = *ic;
        state->best.cur_line = i;
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
 * Fill in the '*ic' handle with the result. If there was a cue at
 * exactly the file/line/column specified return SR_EXACT. If there
 * are cues with in the file with a line/column less than the given
 * values, return the largest cue possible that is less then the
 * passed in values and return SR_CLOSEST. If there are no cues with
 * the proper characteristics, return SR_NONE
 */
search_result DIGENTRY DIPImpLineCue( imp_image_handle *ii, imp_mod_handle im,
                                      cue_fileid file, unsigned long line,
                                      unsigned column, imp_cue_handle *ic )
{
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
    if( state.line > UINT16_MAX )
        state.line = UINT16_MAX;
    state.rc   = SR_NONE;
    walk_rc = DIPImpWalkFileList( ii, im, hllFindLineCueInFile, ic, &state );
    HLL_LOG(( "DIPImpLineCue: mod=%x file=%lx line=%lx -> %d (%lx)\n",
              im, file, line, state.rc, state.best.cur_line ));
    if( walk_rc == WR_FAIL) {
        return( SR_FAIL );
    }
    if( state.rc != SR_NONE ) {
        *ic = state.best;
    }
    return( state.rc );
}


/* Address search state data */
typedef struct {
    address         addr;                   /* what we're looking for. */
    unsigned_32     best_delta;             /* The delta between addr and the best cue. */
    imp_cue_handle  best;                   /* The best cue so far. */
} hll_find_addr_cue_in_file;


/*
 * Search a file for the line number closes to a given address.
 */
static walk_result hllFindAddrCueInFile( imp_image_handle *ii, imp_cue_handle *ic, void *_state )
{
    hll_find_addr_cue_in_file  *state = _state;
    void                       *entries;
    address                     addr = {0};
    unsigned_32                 map_offset;
    unsigned_16                 i;

    /*
     * Check if this segment is the right one, and get the mapping offset of the segment.
     */
    addr.mach.segment = ic->segment;
    hllMapLogical( ii, &addr );
    if( addr.mach.segment != state->addr.mach.segment ) {
        return( WR_CONTINUE );
    }
    /* logical mapping offset. */
    if( ic->segment - 1 < ii->seg_count ) {
        map_offset = ii->segments[ic->segment - 1].map.offset;
    } else {
        map_offset = 0;
    }

    /*
     * Traverse the line numbers.
     */
    entries = hllGetSortedLinnums( ii, ic );
    if( !entries ) {
        return( WR_FAIL );
    }
    i = ic->num_lines;
    switch( ic->style ) {
    case HLL_LINE_STYLE_SRC_LINES:
    case HLL_LINE_STYLE_SRC_LINES_SEG_16:
        {
            cv3_linnum_entry_16 *linnum = entries;
            while( i-- > 0 ) {
                signed_32 delta = state->addr.mach.offset - ( linnum[i].offset + map_offset );
                if( delta >= 0 && delta <= state->best_delta ) {
                    state->best_delta = delta;
                    state->best = *ic;
                    state->best.cur_line = i;
                    if( !delta ) {
                        return( WR_STOP );
                    }
                }
            }
        }
        break;

    case HLL_LINE_STYLE_SRC_LINES_SEG_32:
        {
            cv3_linnum_entry_32 *linnum = entries;
            while( i-- > 0 ) {
                signed_32 delta = state->addr.mach.offset - ( linnum[i].offset + map_offset );
                if( delta >= 0 && delta <= state->best_delta ) {
                    state->best_delta = delta;
                    state->best = *ic;
                    state->best.cur_line = i;
                    if( !delta ) {
                        return( WR_STOP );
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
            map_offset += ic->u.hll.base_offset;
            while( i-- > 0 ) {
                signed_32 delta = state->addr.mach.offset - ( linnum[i].offset + map_offset );
                if( delta >= 0 && delta <= state->best_delta ) {
                    state->best_delta = delta;
                    state->best = *ic;
                    state->best.cur_line = i;
                    if( !delta ) {
                        return( WR_STOP );
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
search_result DIGENTRY DIPImpAddrCue( imp_image_handle *ii, imp_mod_handle im,
                                      address addr, imp_cue_handle *ic )
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
    walk_rc = DIPImpWalkFileList( ii, im, hllFindAddrCueInFile, ic, &state );
    if( walk_rc != WR_FAIL) {
        if( state.best_delta != INT32_MAX ) {
            *ic = state.best;
            rc = walk_rc == WR_STOP ? SR_EXACT : SR_CLOSEST;
        } else {
            rc = SR_NONE;
        }
    } else {
        rc = SR_FAIL;
    }
    HLL_LOG(( "DIPImpAddrCue: mod=%x addr=%04x:%08lx -> %d (%lx)\n",
              im, addr.mach.segment, (long)addr.mach.offset, rc, state.best.cur_line ));
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
int DIGENTRY DIPImpCueCmp( imp_image_handle *ii, imp_cue_handle *ic1,
                           imp_cue_handle *ic2 )
{
    ii = ii;
    if( ic1->im != ic2->im )
        return( ic1->im - ic2->im );
    if( ic1->lines < ic2->lines )
        return( -1 );
    if( ic1->lines > ic2->lines )
        return( +1 );
    return( ic1->cur_line - ic2->cur_line );
}

