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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "jvmdip.h"
#include "madjvm.h"


//NYI: handle methods with zero length line number table

static dip_status GetMethodBlock( imp_cue_handle *icueh, struct methodblock *mb )
{
    ji_ptr      mbp;

    mbp = GetPointer( icueh->cc + offsetof( ClassClass, methods ) );
    if( mbp == 0 )
        return( DS_FAIL );
    return( GetData( mbp + icueh->mb_idx * sizeof( *mb ), mb, sizeof( *mb ) ) );
}

walk_result DIPIMPENTRY( WalkFileList )( imp_image_handle *iih,
                    imp_mod_handle imh, DIP_IMP_CUE_WALKER *wk, imp_cue_handle *icueh,
                    void *d )
{
    //NYI: handle native methods at the front
    icueh->mb_idx = 0;
    icueh->ln_idx = 0;
    icueh->cc = iih->cc;
    return( wk( iih, icueh, d ) );
}

imp_mod_handle DIPIMPENTRY( CueMod )( imp_image_handle *iih, imp_cue_handle *icueh )
{
     return( IMH_JAVA );
}

size_t DIPIMPENTRY( CueFile )( imp_image_handle *iih,
                        imp_cue_handle *icueh, char *buff, size_t buff_size )
{
    ji_ptr      name;
    size_t      len;
    size_t      class_len = 0;

    /* Stick the package name on the front of the source file name */
    name = GetPointer( icueh->cc + offsetof( ClassClass, name ) );
    class_len = GetString( name, NameBuff, sizeof( NameBuff ) );
    for( ;; ) {
        if( class_len == 0 )
            break;
        if( NameBuff[class_len-1] == '/' )
            break;
        if( NameBuff[class_len-1] == '\\' )
            break;
        --class_len;
    }
    name = GetPointer( icueh->cc + offsetof( ClassClass, source_name ) );
    len = GetString( name, &NameBuff[class_len], sizeof( NameBuff ) - class_len );
    return( NameCopy( buff, NameBuff, buff_size, len + class_len ) );
}

cue_fileid DIPIMPENTRY( CueFileId )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)ii; (void)icueh;

    return( 1 );
}

static dip_status FillInLastIC( imp_cue_handle *icueh, unsigned mb_idx )
{
    struct methodblock  mb;
    dip_status          ds;

    icueh->mb_idx = mb_idx;
    ds = GetMethodBlock( icueh, &mb );
    if( ds != DS_OK )
        return( ds );
    icueh->ln_idx = mb.line_number_table_length - 1;
    return( DS_OK );
}

static void FillInFirstIC( imp_cue_handle *icueh, unsigned mb_idx )
{
    icueh->mb_idx = mb_idx;
    icueh->ln_idx = 0;
}

static unsigned GetNumMethods( imp_cue_handle *icueh )
{
    return( GetU16( icueh->cc + offsetof( ClassClass, methods_count ) ) );
}

dip_status DIPIMPENTRY( CueAdjust )( imp_image_handle *iih,
                imp_cue_handle *src_icueh, int adj, imp_cue_handle *dst_icueh )
{
    unsigned            mb_idx;
    dip_status          ds;
    struct methodblock  mb;
    unsigned            backup;

    ds = DS_OK;
    dst_icueh->mb_idx = src_icueh->mb_idx;
    dst_icueh->ln_idx = src_icueh->ln_idx;
    dst_icueh->cc = src_icueh->cc;
    if( adj < 0 ) {
        backup = dst_icueh->ln_idx;
        for( ;; ) {
            for( ;; ) {
                if( dst_icueh->ln_idx >= -adj ) {
                    dst_icueh->ln_idx += adj;
                    return( ds );
                }
                /* have to back up to previous method */
                adj += backup;
                mb_idx = dst_icueh->mb_idx;
                if( mb_idx == 0 )
                    break;
                FillInLastIC( dst_icueh, mb_idx - 1 );
                backup = dst_icueh->ln_idx + 1;
            }
            /* have to wrap around to last method */
            ds = DS_WRAPPED;
            FillInLastIC( dst_icueh, GetNumMethods( dst_icueh ) - 1 );
            backup = dst_icueh->ln_idx + 1;
        }
    } else {
        for( ;; ) {
            for( ;; ) {
                if( !GetMethodBlock( dst_icueh, &mb ) != DS_OK ) {
                    return( DS_ERR | DS_FAIL );
                }
                if( adj < (mb.line_number_table_length - dst_icueh->ln_idx) ) {
                    dst_icueh->ln_idx += adj;
                    return( ds );
                }
                adj -= mb.line_number_table_length - dst_icueh->ln_idx;
                /* advance to next method */
                mb_idx = dst_icueh->mb_idx;
                if( mb_idx >= GetNumMethods( dst_icueh ) )
                    break;
                FillInFirstIC( dst_icueh, mb_idx + 1 );
            }
            /* have to wrap around to first method */
            ds = DS_WRAPPED;
            FillInFirstIC( dst_icueh, 0 );
        }
    }
}

unsigned long DIPIMPENTRY( CueLine )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    struct methodblock  mb;

    if( GetMethodBlock( icueh, &mb ) != DS_OK )
        return( 0 );
    return( GetU32( (ji_ptr)mb.line_number_table
                + icueh->ln_idx * sizeof( struct lineno )
                + offsetof( struct lineno, line_number ) ) );
}

unsigned DIPIMPENTRY( CueColumn )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    return( 0 );
}

address DIPIMPENTRY( CueAddr )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    ji_ptr              ln_tbl;
    address             a;
    struct methodblock  mb;

    if( GetMethodBlock( icueh, &mb ) != DS_OK )
        return( NilAddr );

    a = DefCodeAddr;
    ln_tbl = (ji_ptr)mb.line_number_table;
    a.mach.offset = (ji_ptr)mb.code + GetU32( ln_tbl
                            + icueh->ln_idx * sizeof( struct lineno )
                            + offsetof( struct lineno, pc ) );
    return( a );
}

#define LN_CACHE_SIZE   100

search_result DIPIMPENTRY( LineCue )( imp_image_handle *iih,
                imp_mod_handle imh, cue_fileid file, unsigned long line,
                unsigned column, imp_cue_handle *icueh )
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

    icueh->cc = iih->cc;
    best_ln = ~0L;
    for( mb_idx = 0; mb_idx < iih->num_methods; ++mb_idx ) {
        ln_idx = 0;
        ln_tbl = (ji_ptr)iih->methods[mb_idx].line_number_table;
        for( left = iih->methods[mb_idx].line_number_table_length; left > 0; left -= get ) {
            get = left;
            if( get > LN_CACHE_SIZE )
                get = LN_CACHE_SIZE;
            if( GetData( ln_tbl, cache, get * sizeof( cache[0] ) ) != DS_OK ) {
                return( SR_NONE );
            }
            for( i = 0; i < get; ++i ) {
                curr_ln = cache[i].line_number;
                if( curr_ln == line ) {
                    icueh->mb_idx = mb_idx;
                    icueh->ln_idx = ln_idx + i;
                    return( SR_EXACT );
                }
                if( (curr_ln > line) && (curr_ln < best_ln) ) {
                    best_ln = curr_ln;
                    icueh->mb_idx = mb_idx;
                    icueh->ln_idx = ln_idx + i;
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

    acc.class_pointer = iih->cc;
    acc.line_num = line;
    ds = GetLineCue( &acc, &ret );
    if( ds != DS_OK ) {
        DCStatus( ds );
        return( SR_FAIL );
    }
    icueh->cc = ret.class_pointer;
    icueh->mb_idx = ret.method_index;
    icueh->ln_idx = ret.line_index;
    return( ret.ret );
}


search_result DIPIMPENTRY( AddrCue )( imp_image_handle *iih,
                imp_mod_handle imh, address addr, imp_cue_handle *icueh )
{
#if 0
    ji_ptr              ln_tbl;
    unsigned long       pc;
    long                lo;
    long                hi;
    long                target;

    switch( FindMBIndex( iih, addr.mach.offset, &icueh->mb_idx ) ) {
    case SR_EXACT:
    case SR_CLOSEST:
        break;
    default:
        return( SR_NONE );
    }
    icueh->cc = iih->cc;
    addr.mach.offset -= (ji_ptr)iih->methods[icueh->mb_idx].code;
    ln_tbl = (ji_ptr)iih->methods[icueh->mb_idx].line_number_table;
    hi = iih->methods[icueh->mb_idx].line_number_table_length - 1;
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
            icueh->ln_idx = target;
            return( SR_EXACT );
        }
    }
    if( hi < 0 )
        return( SR_NONE );
    icueh->ln_idx = hi;
    return( SR_CLOSEST );
#endif
    mad_jvm_findaddrcue_acc     acc;
    mad_jvm_findline_ret        ret;
    dip_status                  ds;

    acc.class_pointer = iih->cc;
    acc.addr = addr.mach.offset;
    ds = GetAddrCue( &acc, &ret );
    if( ds != DS_OK ) {
        DCStatus( ds );
        return( SR_FAIL );
    }
    icueh->cc = ret.class_pointer;
    icueh->mb_idx = ret.method_index;
    icueh->ln_idx = ret.line_index;
    return( ret.ret );
}

int DIPIMPENTRY( CueCmp )( imp_image_handle *iih, imp_cue_handle *icueh1, imp_cue_handle *icueh2 )
{
    if( icueh1->cc < icueh2->cc )
        return( -1 );
    if( icueh1->cc > icueh2->cc )
        return( 1 );
    if( icueh1->mb_idx < icueh2->mb_idx )
        return( -1 );
    if( icueh1->mb_idx > icueh2->mb_idx )
        return( 1 );
    if( icueh1->ln_idx < icueh2->ln_idx )
        return( -1 );
    if( icueh1->ln_idx > icueh2->ln_idx )
        return( 1 );
    return( 0 );
}
