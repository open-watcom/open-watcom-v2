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


#include "dipwat.h"
#include "dbcue.h"

extern void             *InfoLoad(imp_image_handle *, imp_mod_handle,unsigned,unsigned,void (*)());
extern void             InfoUnlock(void);
extern void             InfoSpecUnlock(void *);
extern unsigned         InfoSize(imp_image_handle *, imp_mod_handle,unsigned int, unsigned );
extern mem_block        FindSegBlock(imp_image_handle *, imp_mod_handle,unsigned long );
extern mod_info         *ModPointer( imp_image_handle *, imp_mod_handle );
extern unsigned         PrimaryCueFile( imp_image_handle *, imp_cue_handle *, char *, unsigned );
extern void             *FindSpecCueTable( imp_image_handle *, imp_mod_handle, void ** );

extern address          NilAddr;

static void     *LinStart;
static void     *LinEnd;
static byte     V2Lines;


#define NO_LINE         ((unsigned_16)-1)

typedef union {
    v2_line_segment     v2;
    v3_line_segment     v3;
} line_segment;

#define NEXT_SEG( ptr ) (V2Lines ? ((line_segment *)&((ptr)->v2.line[(ptr)->v2.num])) \
                                 : ((line_segment *)&((ptr)->v3.line[(ptr)->v3.num])))
#define LINE_SEG( ptr ) (V2Lines ? (ptr)->v2.segment : (ptr)->v3.segment)
#define LINE_NUM( ptr ) (V2Lines ? (ptr)->v2.num : (ptr)->v3.num)
#define LINE_LINE( ptr) (V2Lines ? (ptr)->v2.line : (ptr)->v3.line)

static int  CueFind( cue_state *base, cue_idx cue, cue_state *ret )
{
    cue_state  *curr;
    long        diff;
    unsigned_16 lo;
    unsigned_16 mid;
    int         ok;
    unsigned    hi;

    if( base == NULL ) return( 0 );
    hi = *(unsigned_16*)base;
    base =  (void *)((char *)base + 2);
    cue -= PRIMARY_RANGE;
    ok = 0;
    lo = 0;
    for(;;){
        mid = (lo + hi)/2;
        curr = &base[mid];      // compare keys
        diff = (long)cue - (long)(curr->cue);
        if( mid == lo )break;
        if( diff < 0 ){       // key < mid
            hi = mid;
        }else if( diff > 0 ){ // key > mid
            lo = mid;
        }else{                // key == mid
            break;
        }
    }
    if( diff >= 0 ){
        ok = 1;
    }
    *ret = *curr;
    ret->line += diff;
    ret->fno += 2; /* 0 => no file, 1 => primary file */
    return( ok );
}

static unsigned long SpecCueLine( imp_image_handle *ii, imp_cue_handle *ic,
                                  unsigned cue_id )
{
    cue_state           info;
    unsigned long       ret;
    void                *start;
    void                *base;

    ret = 0;
    start = FindSpecCueTable( ii, ic->im, &base );
    if( start != NULL ) {
        if( CueFind( start, cue_id, &info ) ){
            ret = info.line;
        }
        InfoSpecUnlock( base );
    }
    return( ret );
}

static unsigned SpecCueFileId( imp_image_handle *ii, imp_cue_handle *ic,
                                unsigned cue_id )
{
    cue_state           info;
    unsigned long       ret;
    void                *start;
    void                *base;

    ret = 0;
    start = FindSpecCueTable( ii, ic->im, &base );
    if( start != NULL ) {
        if( CueFind( start, cue_id, &info ) ){
            ret = info.fno;
        }
        InfoSpecUnlock( base );
    }
    return( ret );
}

static unsigned SpecCueFile( imp_image_handle *ii, imp_cue_handle *ic,
                    unsigned file_id, char *buff, unsigned max )
{
    unsigned_16         size;
    unsigned_16         len;
    unsigned_16         *index;
    char                *name;
    void                *start;
    void                *base;

    len = 0;
    start = FindSpecCueTable( ii, ic->im, &base );
    if( start != NULL ) {
        size  = *(unsigned_16*)start;
        start =  (char *)start + 2;
        start =  (char *)start +  size * sizeof( cue_state );
        size  = *(unsigned_16*)start;
        start =  (char *)start + 2;
        index = start;
        name = (char *)start + size * sizeof( unsigned_16 );
        name += index[file_id-2]; /* see comment in CueFind */
        len = strlen( name );
        if( max > 0 ) {
            --max;
            if( max > len ) max = len;
            memcpy( buff, name, max );
            buff[ max ] = '\0';
        }
        InfoSpecUnlock( base );
    }
    return( len );
}


/*
 * GetLineInfo -- get the line number infomation for a module
 */

static void UnlockLine( void )
{
    if( LinStart != NULL ) {
        InfoSpecUnlock( LinStart );
        LinStart = NULL; /* so we don't unlock the same section twice */
    }
}


static dip_status GetLineInfo( imp_image_handle *ii, imp_mod_handle im,
                                 unsigned entry )
{
    if( entry != 0 ) UnlockLine();
    LinStart = InfoLoad( ii, im, DMND_LINES, entry, NULL );
    if( LinStart == NULL ) return( DS_FAIL );
    LinEnd = (byte *)LinStart + InfoSize( ii, im, DMND_LINES, entry );
    V2Lines = ii->v2;
    return( DS_OK );
}

struct search_info {
    imp_cue_handle      ic;
    unsigned            num;
    addr_off            off;
    search_result       have;
    byte                found;
    void                *special_table;
    enum { ST_UNKNOWN, ST_NO, ST_YES } have_spec_table;
};


static line_info *FindLineOff( addr_off off, addr_off adj,
                        void *start, void *end,
                        struct search_info *close, imp_image_handle *ii )
{
    line_info   *ln_ptr;
    int         low, high, target;
    addr_off    chk;
    void        *dummy;

    low = 0;
    /* get number of entries minus one */
    high = ((char *)end - (char *)start) / sizeof( line_info ) - 1;
    /* point at first entry */
    ln_ptr = start;
    while( low <= high ) {
        target = (low + high) >> 1;
        chk = ln_ptr[ target ].code_offset + adj;
        if( off < chk ) {
            high = target - 1;
        } else if( off > chk ) {
            low = target + 1;
        } else {                  /* exact match */
            if( ln_ptr[ target ].line_number >= PRIMARY_RANGE ) {
                /* a special cue - have to make sure we have the table */
                if( close->have_spec_table == ST_UNKNOWN ) {
                    if( FindSpecCueTable( ii, close->ic.im, &dummy ) != NULL ) {
                        close->have_spec_table = ST_YES;
                    } else {
                        close->have_spec_table = ST_NO;
                    }
                }
                if( close->have_spec_table == ST_YES ) {
                    return( &ln_ptr[ target ] );
                }
            } else {
                return( &ln_ptr[ target ] );
            }
            /* if it's a special & we don't have the table, ignore entry */
            high = target - 1;
        }
    }
    if( high < 0 ) return( NULL );
    if( ln_ptr[ high ].line_number >= PRIMARY_RANGE ) {
        /* a special cue - have to make sure we have the table */
        if( close->have_spec_table == ST_UNKNOWN ) {
            if( FindSpecCueTable( ii, close->ic.im, &dummy ) != NULL ) {
                close->have_spec_table = ST_YES;
            } else {
                close->have_spec_table = ST_NO;
            }
        }
        if( close->have_spec_table == ST_NO ) {
            /* if it's a special & we don't have the table, ignore entry */
            for( ;; ) {
                --high;
                if( high < 0 ) return( NULL );
                if( ln_ptr[ high ].line_number < PRIMARY_RANGE ) break;
            }
        }
    }
    return( &ln_ptr[ high ] );
}

#define BIAS( p )       ((byte *)(p) - (byte *)LinStart)
#define UNBIAS( o )     ((void *)((byte *)LinStart + (o)))

static void SearchSection( imp_image_handle *ii,
                    struct search_info *close, address addr )
{
    line_segment        *seg;
    line_segment        *next;
    line_info           *info;
    mem_block           block;

    close->found = 0;
    for( seg = LinStart; seg < LinEnd; seg = next ) {
        next = NEXT_SEG( seg );
        block = FindSegBlock( ii, close->ic.im, LINE_SEG( seg ) );
        if( DCSameAddrSpace( block.start, addr ) != DS_OK ) continue;
        if( block.start.mach.offset > addr.mach.offset ) continue;
        if( block.start.mach.offset + block.len <= addr.mach.offset ) continue;
        info = FindLineOff( addr.mach.offset, block.start.mach.offset,
                            LINE_LINE( seg ), next, close, ii );
        if( info == NULL ) continue;
        if( close->have == SR_NONE
                || info->code_offset+block.start.mach.offset>close->off ) {
            close->found = 1;
            close->ic.seg_bias = BIAS( seg );
            close->ic.info_bias = BIAS( info );
            close->off = block.start.mach.offset + info->code_offset;
            if( close->off == addr.mach.offset ) {
                close->have = SR_EXACT;
            } else {
                close->have = SR_CLOSEST;
            }
        }
    }
}

search_result DIPENTRY DIPImpAddrCue( imp_image_handle *ii, imp_mod_handle im,
                address addr, imp_cue_handle *ic )
{
    struct search_info  close;
    unsigned            save_entry;

    close.ic.im = im;
    close.have = SR_NONE;
    close.ic.entry = 0;
    close.have_spec_table = ST_UNKNOWN;
    for( ;; ) {
        if( GetLineInfo( ii, close.ic.im, close.ic.entry ) != DS_OK ) break;
        SearchSection( ii, &close, addr );
        if( close.found ) save_entry = close.ic.entry;
        if( close.have == SR_EXACT ) break;
        ++close.ic.entry;
    }
    *ic = close.ic;
    ic->entry = save_entry;
    UnlockLine();
    return( close.have );
}

static void ScanSection( struct search_info *close, unsigned file_id,
                        unsigned line )
{
    line_info           *curr;
    line_segment        *ptr;
    void                *next;
    unsigned            num;
    cue_state           spec;

    close->found = 0;
    for( ptr = LinStart; ptr < LinEnd; ptr = next ) {
        next = NEXT_SEG( ptr );
        if( line == 0 ) {
            line = LINE_LINE( ptr )[0].line_number;
        }
        for( curr = LINE_LINE( ptr ); curr < next; ++curr ) {
            spec.fno = 1;
            num = curr->line_number;
            if( num >= PRIMARY_RANGE ) {
                if( !CueFind( close->special_table, num, &spec ) ) {
                    continue;
                }
                num = spec.line;
            }
            if( spec.fno != file_id ) continue;
            if( num > line ) continue;
            if( num <= close->num ) continue;
            close->found = 1;
            close->ic.seg_bias = BIAS( ptr );
            close->ic.info_bias = BIAS( curr );
            close->num = num;
            if( num == line ) {
                close->have = SR_EXACT;
                return;
            }
            close->have = SR_CLOSEST;
        }
    }
}

search_result DIPENTRY DIPImpLineCue( imp_image_handle *ii, imp_mod_handle im,
                        cue_file_id file, unsigned long line, unsigned col,
                        imp_cue_handle *ic )
{
    struct search_info  close;
    unsigned            save_entry;
    void                *base;

    col = col;
    if( file == 0 ) file = 1;
    close.ic.im = im;
    close.have = SR_NONE;
    close.num = 0;
    close.ic.entry = 0;
    close.special_table = FindSpecCueTable( ii, im, &base );
    for( ;; ) {
        if( GetLineInfo( ii, im, close.ic.entry ) != DS_OK ) break;
        ScanSection( &close, file, line );
        if( close.found ) save_entry = close.ic.entry;
        if( close.have == SR_EXACT ) break;
        ++close.ic.entry;
    }
    *ic = close.ic;
    ic->entry = save_entry;
    UnlockLine();
    if( base != NULL ) InfoSpecUnlock( base );
    return( close.have );
}

unsigned long DIPENTRY DIPImpCueLine( imp_image_handle *ii, imp_cue_handle *ic )
{
    line_info   *info;
    unsigned    num;

    LinStart = NULL;
    num = 0;
    if( ic->entry != NO_LINE && GetLineInfo( ii, ic->im, ic->entry ) == DS_OK ) {
        info = UNBIAS( ic->info_bias );
        num = info->line_number;
        UnlockLine();
        if( num >= PRIMARY_RANGE ) {
            return( SpecCueLine( ii, ic, num ) );
        }
    }
    return( num );
}

unsigned DIPENTRY DIPImpCueColumn( imp_image_handle *ii, imp_cue_handle *ic )
{
    ii = ii; ic = ic;
    return( 0 );
}

address DIPENTRY DIPImpCueAddr( imp_image_handle *ii, imp_cue_handle *ic )
{
    address             addr;
    line_info           *info;
    line_segment        *seg;

    LinStart = NULL;
    addr = NilAddr;
    if( ic->entry != NO_LINE && GetLineInfo( ii, ic->im, ic->entry ) == DS_OK ) {
        seg = UNBIAS( ic->seg_bias );
        addr = FindSegBlock( ii, ic->im, LINE_SEG( seg ) ).start;
        info = UNBIAS( ic->info_bias );
        addr.mach.offset += info->code_offset;
        UnlockLine();
    }
    return( addr );
}


walk_result DIPENTRY DIPImpWalkFileList( imp_image_handle *ii, imp_mod_handle im,
            IMP_CUE_WKR *wk, imp_cue_handle *ic, void *d )
{
    line_info           *curr;
    line_segment        *ptr;
    void                *next;
    walk_result         wr;

    //NYI: handle special cues
    ic->im = im;
    ic->entry = 0;
    for( ;; ) {
        if( GetLineInfo( ii, im, ic->entry ) != DS_OK ) break;
        for( ptr = LinStart; ptr < LinEnd; ptr = next ) {
            next = NEXT_SEG( ptr );
            ic->seg_bias = BIAS( ptr );
            for( curr = LINE_LINE( ptr ); curr < next; ++curr ) {
                ic->info_bias = BIAS( curr );
                if( curr->line_number < PRIMARY_RANGE ) {
                    wr = wk( ii, ic, d );
                    UnlockLine();
                    return( wr );
                }
            }
        }
        UnlockLine();
        ++ic->entry;
    }
    if( ic->entry == 0 ) {
        /* Module with no line cues. Fake one up. */
        ic->entry = NO_LINE;
        return( wk( ii, ic, d ) );
    }
    return( WR_CONTINUE );
}

imp_mod_handle DIPENTRY DIPImpCueMod( imp_image_handle *ii, imp_cue_handle *ic )
{
    ii = ii;
    return( ic->im );
}

cue_file_id DIPENTRY DIPImpCueFileId( imp_image_handle *ii, imp_cue_handle *ic )
{
    line_info   *info;
    unsigned    num;

    LinStart = NULL;
    if( ic->entry != NO_LINE && GetLineInfo( ii, ic->im, ic->entry ) == DS_OK ) {
        info = UNBIAS( ic->info_bias );
        num = info->line_number;
        UnlockLine();
        if( num >= PRIMARY_RANGE ) {
            return( SpecCueFileId( ii, ic, num ) );
        }
    }
    return( 1 );
}

unsigned DIPENTRY DIPImpCueFile( imp_image_handle *ii, imp_cue_handle *ic,
                        char *buff, unsigned max )
{
    cue_file_id         id;

    id = ImpInterface.cue_fyle_id( ii, ic );
    switch( id ) {
    case 0:
        return( 0 );
    case 1:
        return( PrimaryCueFile( ii, ic, buff, max ) );
    default:
        return( SpecCueFile( ii, ic, id, buff, max ) );
    }
}


static dip_status AdjForward( imp_image_handle *ii, imp_cue_handle *ic )
{
    line_info           *info;
    line_segment        *seg;
    unsigned            num_entries;
    dip_status          status;

    status = DS_OK;
    num_entries = ModPointer( ii, ic->im )->di[DMND_LINES].u.entries;
    for( ;; ) {
        status = GetLineInfo( ii, ic->im, ic->entry );
        if( status != DS_OK ) return( status );
        seg = UNBIAS( ic->seg_bias );
        info = UNBIAS( ic->info_bias );
        ++info;
        for( ;; ) {
            if( (void *)info < NEXT_SEG( seg ) ) {
                ic->seg_bias = BIAS( seg );
                ic->info_bias = BIAS( info );
                UnlockLine();
                return( DS_OK );
            }
            seg = NEXT_SEG( seg );
            if( seg >= LinEnd ) break;
            info = LINE_LINE( seg );
        }
        ic->entry++;
        if( ic->entry >= num_entries ) {
            ic->entry = 0;
            ic->seg_bias = BIAS( LinStart );
            info = LINE_LINE( (line_segment *)LinStart );
            ic->info_bias = BIAS( info );
            UnlockLine();
            return( DS_WRAPPED );
        }
        ic->seg_bias = BIAS( seg );
        ic->info_bias = BIAS( info );
    }
}

static line_segment *FindPrevSeg( line_segment *seg )
{
    line_segment        *curr;
    line_segment        *next;

    curr = LinStart;
    for( ;; ) {
        next = NEXT_SEG( curr );
        if( next >= seg ) return( curr );
        curr = next;
    }
}

static dip_status AdjBackward( imp_image_handle *ii, imp_cue_handle *ic )
{
    line_info           *info;
    line_segment        *seg;
    unsigned            num_entries;
    dip_status          status;

    LinStart = NULL;
    for( ;; ) {
        status = GetLineInfo( ii, ic->im, ic->entry );
        if( status != DS_OK ) return( status );
        seg = UNBIAS( ic->seg_bias );
        info = UNBIAS( ic->info_bias );
        --info;
        for( ;; ) {
            if( info >= LINE_LINE( seg ) ) {
                ic->seg_bias = BIAS( seg );
                ic->info_bias = BIAS( info );
                UnlockLine();
                return( DS_OK );
            }
            if( seg == LinStart ) break;
            seg = FindPrevSeg( seg );
            info = &LINE_LINE( seg )[ LINE_NUM( seg ) - 1 ];
        }
        if( ic->entry == 0 ) {
            num_entries = ModPointer( ii, ic->im )->di[DMND_LINES].u.entries;
            ic->entry = num_entries - 1;
            status = GetLineInfo( ii, ic->im, ic->entry );
            if( status != DS_OK ) return( status );
            seg = FindPrevSeg( seg );
            info = &LINE_LINE( seg )[ LINE_NUM( seg ) - 1 ];
            ic->seg_bias = BIAS( seg );
            ic->info_bias = BIAS( info );
            UnlockLine();
            return( DS_WRAPPED );
        }
        ic->entry--;
        if( ic->entry == 0 ) {
            /* special handling since we're walking backwards */
            UnlockLine();
        }
        ic->seg_bias = BIAS( seg );
        ic->info_bias = BIAS( info );
    }
}

dip_status DIPENTRY DIPImpCueAdjust( imp_image_handle *ii, imp_cue_handle *ic,
                        int adj, imp_cue_handle *aic )
{
    dip_status  status;
    dip_status  ok;

    if( ic->entry == NO_LINE ) return( DS_BAD_PARM );
    //NYI: handle special cues
    *aic = *ic;
    ok = DS_OK;
    while( adj > 0 ) {
        status = AdjForward( ii, aic );
        if( status & DS_ERR ) return( status );
        if( status != DS_OK ) ok = status;
        --adj;
    }
    while( adj < 0 ) {
        status = AdjBackward( ii, aic );
        if( status & DS_ERR ) return( status );
        if( status != DS_OK ) ok = status;
        ++adj;
    }
    return( ok );
}

int DIPENTRY DIPImpCueCmp( imp_image_handle *ii, imp_cue_handle *ic1,
                                imp_cue_handle *ic2 )
{
    ii = ii;
    if( ic1->im != ic2->im ) return( ic1->im - ic2->im );
    if( ic1->entry != ic2->entry ) return( ic1->entry - ic2->entry );
    return( ic1->info_bias - ic2->info_bias );
}
