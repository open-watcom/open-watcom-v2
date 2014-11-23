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


#include "jvmdip.h"
#include "madjvm.h"


//NYI: handle methods with zero length line number table

static dip_status GetMethodBlock( imp_cue_handle *ic, struct methodblock *mb )
{
    ji_ptr      mbp;

    mbp = GetPointer( ic->cc + offsetof( ClassClass, methods ) );
    if( mbp == 0 ) return( DS_FAIL );
    return( GetData( mbp + ic->mb_idx * sizeof( *mb ), mb, sizeof( *mb ) ) );
}

walk_result     DIGENTRY DIPImpWalkFileList( imp_image_handle *ii,
                    imp_mod_handle im, IMP_CUE_WKR *wk, imp_cue_handle *ic,
                    void *d )
{
    //NYI: handle native methods at the front
    ic->mb_idx = 0;
    ic->ln_idx = 0;
    ic->cc = ii->cc;
    return( wk( ii, ic, d ) );
}

imp_mod_handle  DIGENTRY DIPImpCueMod( imp_image_handle *ii,
                                imp_cue_handle *ic )
{
     return( IMH_JAVA );
}

unsigned        DIGENTRY DIPImpCueFile( imp_image_handle *ii,
                        imp_cue_handle *ic, char *buff, unsigned buff_size )
{
    ji_ptr      name;
    unsigned    len;
    unsigned    class_len = 0;

    /* Stick the package name on the front of the source file name */
    name = GetPointer( ic->cc + offsetof( ClassClass, name ) );
    class_len = GetString( name, NameBuff, sizeof( NameBuff ) );
    for( ;; ) {
        if( class_len == 0 ) break;
        if( NameBuff[class_len-1] == '/' ) break;
        if( NameBuff[class_len-1] == '\\' ) break;
        --class_len;
    }
    name = GetPointer( ic->cc + offsetof( ClassClass, source_name ) );
    len = GetString( name, &NameBuff[class_len], sizeof( NameBuff ) - class_len );
    return( NameCopy( buff, NameBuff, buff_size, len + class_len ) );
}

cue_fileid  DIGENTRY DIPImpCueFileId( imp_image_handle *ii,
                        imp_cue_handle *ic )
{
    return( 1 );
}

static dip_status FillInLastIC( imp_cue_handle *ic, unsigned mb_idx )
{
    struct methodblock  mb;
    dip_status          ds;

    ic->mb_idx = mb_idx;
    ds = GetMethodBlock( ic, &mb );
    if( ds != DS_OK ) return( ds );
    ic->ln_idx = mb.line_number_table_length - 1;
    return( DS_OK );
}

static void FillInFirstIC( imp_cue_handle *ic, unsigned mb_idx )
{
    ic->mb_idx = mb_idx;
    ic->ln_idx = 0;
}

static unsigned GetNumMethods( imp_cue_handle *ic )
{
    return( GetU16( ic->cc + offsetof( ClassClass, methods_count ) ) );
}

dip_status      DIGENTRY DIPImpCueAdjust( imp_image_handle *ii,
                imp_cue_handle *src, int adj, imp_cue_handle *dst )
{
    unsigned            mb_idx;
    dip_status          ds;
    struct methodblock  mb;
    unsigned            backup;

    ds = DS_OK;
    dst->mb_idx = src->mb_idx;
    dst->ln_idx = src->ln_idx;
    dst->cc = src->cc;
    if( adj < 0 ) {
        backup = dst->ln_idx;
        for( ;; ) {
            for( ;; ) {
                if( dst->ln_idx >= -adj ) {
                    dst->ln_idx += adj;
                    return( ds );
                }
                /* have to back up to previous method */
                adj += backup;
                mb_idx = dst->mb_idx;
                if( mb_idx == 0 ) break;
                FillInLastIC( dst, mb_idx - 1 );
                backup = dst->ln_idx + 1;
            }
            /* have to wrap around to last method */
            ds = DS_WRAPPED;
            FillInLastIC( dst, GetNumMethods( dst ) - 1 );
            backup = dst->ln_idx + 1;
        }
    } else {
        for( ;; ) {
            for( ;; ) {
                if( !GetMethodBlock( dst, &mb ) != DS_OK ) {
                    return( DS_ERR|DS_FAIL );
                }
                if( adj < (mb.line_number_table_length - dst->ln_idx) ) {
                    dst->ln_idx += adj;
                    return( ds );
                }
                adj -= mb.line_number_table_length - dst->ln_idx;
                /* advance to next method */
                mb_idx = dst->mb_idx;
                if( mb_idx >= GetNumMethods( dst ) ) break;
                FillInFirstIC( dst, mb_idx + 1 );
            }
            /* have to wrap around to first method */
            ds = DS_WRAPPED;
            FillInFirstIC( dst, 0 );
        }
    }
}

unsigned long   DIGENTRY DIPImpCueLine( imp_image_handle *ii,
                        imp_cue_handle *ic )
{
    struct methodblock  mb;

    if( GetMethodBlock( ic, &mb ) != DS_OK ) return( 0 );
    return( GetU32( (ji_ptr)mb.line_number_table
                + ic->ln_idx * sizeof( struct lineno )
                + offsetof( struct lineno, line_number ) ) );
}

unsigned        DIGENTRY DIPImpCueColumn( imp_image_handle *ii, imp_cue_handle *ic )
{
    return( 0 );
}

address         DIGENTRY DIPImpCueAddr( imp_image_handle *ii,
                        imp_cue_handle *ic )
{
    ji_ptr              ln_tbl;
    address             a;
    struct methodblock  mb;

    if( GetMethodBlock( ic, &mb ) != DS_OK ) return( NilAddr );

    a = DefCodeAddr;
    ln_tbl = (ji_ptr)mb.line_number_table;
    a.mach.offset = (ji_ptr)mb.code + GetU32( ln_tbl
                            + ic->ln_idx * sizeof( struct lineno )
                            + offsetof( struct lineno, pc ) );
    return( a );
}

#define LN_CACHE_SIZE   100

search_result   DIGENTRY DIPImpLineCue( imp_image_handle *ii,
                imp_mod_handle im, cue_fileid file, unsigned long line,
                unsigned column, imp_cue_handle *ic )
{
#if 0
    unsigned            mb_idx;
    unsigned            ln_idx;
    ji_ptr              ln_tbl;
    unsigned long       best_ln;
    unsigned long       curr_ln;
    struct lineno       cache[LN_CACHE_SIZE];
    unsigned            left;
    unsigned            get;
    unsigned            i;

    ic->cc = ii->cc;
    best_ln = ~0L;
    for( mb_idx = 0; mb_idx < ii->num_methods; ++mb_idx ) {
        ln_idx = 0;
        ln_tbl = (ji_ptr)ii->methods[mb_idx].line_number_table;
        for( left = ii->methods[mb_idx].line_number_table_length; left > 0; left -= get ) {
            get = left;
            if( get > LN_CACHE_SIZE ) get = LN_CACHE_SIZE;
            if( GetData( ln_tbl, cache, get * sizeof( cache[0] ) ) != DS_OK ) {
                return( SR_NONE );
            }
            for( i = 0; i < get; ++i ) {
                curr_ln = cache[i].line_number;
                if( curr_ln == line ) {
                    ic->mb_idx = mb_idx;
                    ic->ln_idx = ln_idx + i;
                    return( SR_EXACT );
                }
                if( (curr_ln > line) && (curr_ln < best_ln) ) {
                    best_ln = curr_ln;
                    ic->mb_idx = mb_idx;
                    ic->ln_idx = ln_idx + i;
                }
            }
            ln_idx += get;
            ln_tbl += get * sizeof( struct lineno );
        }
    }
    return( (best_ln == ~0L) ? SR_NONE : SR_CLOSEST );
#endif
    mad_jvm_findlinecue_acc     acc;
    mad_jvm_findline_ret        ret;
    dip_status                  ds;

    acc.class_pointer = ii->cc;
    acc.line_num = line;
    ds = GetLineCue( &acc, &ret );
    if( ds != DS_OK ) {
        DCStatus( ds );
        return( SR_FAIL );
    }
    ic->cc = ret.class_pointer;
    ic->mb_idx = ret.method_index;
    ic->ln_idx = ret.line_index;
    return( ret.ret );
}


search_result   DIGENTRY DIPImpAddrCue( imp_image_handle *ii,
                imp_mod_handle im, address addr, imp_cue_handle *ic )
{
#if 0
    ji_ptr              ln_tbl;
    unsigned long       pc;
    long                lo;
    long                hi;
    long                target;

    switch( FindMBIndex( ii, addr.mach.offset, &ic->mb_idx ) ) {
    case SR_EXACT:
    case SR_CLOSEST:
        break;
    default:
        return( SR_NONE );
    }
    ic->cc = ii->cc;
    addr.mach.offset -= (ji_ptr)ii->methods[ic->mb_idx].code;
    ln_tbl = (ji_ptr)ii->methods[ic->mb_idx].line_number_table;
    hi = ii->methods[ic->mb_idx].line_number_table_length - 1;
    lo = 0;
    while( lo <= hi ) {
        target = (lo + hi) >> 1;
        pc = GetU32( ln_tbl
                        + (target * sizeof( struct lineno ))
                        + offsetof( struct lineno, pc ) );
        if( addr.mach.offset < pc ) {
            hi = target - 1;
        } else if( addr.mach.offset > pc ) {
            lo = target + 1;
        } else {                  /* exact match */
            ic->ln_idx = target;
            return( SR_EXACT );
        }
    }
    if( hi < 0 ) return( SR_NONE );
    ic->ln_idx = hi;
    return( SR_CLOSEST );
#endif
    mad_jvm_findaddrcue_acc     acc;
    mad_jvm_findline_ret        ret;
    dip_status                  ds;

    acc.class_pointer = ii->cc;
    acc.addr = addr.mach.offset;
    ds = GetAddrCue( &acc, &ret );
    if( ds != DS_OK ) {
        DCStatus( ds );
        return( SR_FAIL );
    }
    ic->cc = ret.class_pointer;
    ic->mb_idx = ret.method_index;
    ic->ln_idx = ret.line_index;
    return( ret.ret );
}

int DIGENTRY DIPImpCueCmp( imp_image_handle *ii, imp_cue_handle *ic1,
                                imp_cue_handle *ic2 )
{
    if( ic1->cc     < ic2->cc     ) return( -1 );
    if( ic1->cc     > ic2->cc     ) return( +1 );
    if( ic1->mb_idx < ic2->mb_idx ) return( -1 );
    if( ic1->mb_idx > ic2->mb_idx ) return( +1 );
    if( ic1->ln_idx < ic2->ln_idx ) return( -1 );
    if( ic1->ln_idx > ic2->ln_idx ) return( +1 );
    return( 0 );
}
