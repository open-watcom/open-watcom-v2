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
#include <errno.h>
#include "digcli.h"
#include "exeqnx.h"
#include "i86.h"
#include "sys/lmf.h"
#include "sys/seginfo.h"

typedef struct {
    unsigned long       sig;
    unsigned_8          init_rtn[1];    /* offset is start of routine */
} supp_header;

static unsigned short   *SuppSegs;

union any_rec {
    struct _lmf_definition  defn;
    struct _lmf_data        data;
    struct _lmf_seg_fixup   sfix;
    struct _lmf_eof         eof;
};

static int ProcDefn( struct _lmf_definition *defn, unsigned long *seg, unsigned count )
{
    unsigned    i;

    if( defn->version_no != 400 )
        return( EINVAL );
    SuppSegs = DIGCli( Alloc )( sizeof( *SuppSegs ) * (count+1));
    if( SuppSegs == NULL )
        return( ENOMEM );
    for( i = 0; i < count; ++i ) {
        SuppSegs[i] = qnx_segment_alloc( seg[i] & 0xfffffff );
        if( SuppSegs[i] == -1 ) {
            return( ENOMEM );
        }
    }
    SuppSegs[i] = -1;
    return( EOK );
}

static int ProcData( dig_lhandle lfh, struct _lmf_data *data, unsigned nbytes )
{
    void        *pos;

    pos = MK_FP( SuppSegs[data->segment_index], data->offset );
    nbytes -= sizeof( struct _lmf_data );
    if( DIGLoadRead( lfh, pos, nbytes ) )
        return( EIO );
    return( EOK );
}

static int ProcFixup( dig_lhandle lfh, int size )
{
    struct fixups   a_fix;
    unsigned short  *fix;

    for( ; size > 0; size -= sizeof( a_fix ) ) {
        if( DIGLoadRead( lfh, &a_fix, sizeof( a_fix ) ) )
            return( EIO );
        fix = MK_FP( SuppSegs[a_fix.fixup_seg_index], a_fix.fixup_offset );
        *fix = SuppSegs[*fix >> 3];
    }
    return( EOK );
}

static supp_header *ReadSupp( dig_lhandle lfh )
{
    struct _lmf_header  head;
    unsigned            count;
    unsigned            size;
    unsigned            i;
    unsigned long       segs[10];
    union  any_rec      rec;

    if( DIGLoadRead( lfh, &head, sizeof( head ) ) )
        return( NULL );
    if( head.rec_type != _LMF_DEFINITION_REC )
        return( NULL );
    if( head.data_nbytes <= sizeof( struct _lmf_definition ) )
        return( NULL );
    if( DIGLoadRead( lfh, &rec, sizeof( rec.defn ) ) )
        return( NULL );
    size = head.data_nbytes - sizeof( struct _lmf_definition );
    if( size > sizeof( segs ) )
        return( NULL );
    if( DIGLoadRead( lfh, segs, size ) )
        return( NULL );
    count = size / sizeof( unsigned long );
    if( ProcDefn( &rec.defn, segs, count ) != EOK )
        return( NULL );
    for( ;; ) {
        if( DIGLoadRead( lfh, &head, sizeof( head ) ) )
            return( NULL );
        switch( head.rec_type ) {
        case _LMF_COMMENT_REC:
        case _LMF_RESOURCE_REC:
        case _LMF_ENDDATA_REC:
        case _LMF_FIXUP_80X87_REC:
            DIGLoadSeek( lfh, head.data_nbytes, DIG_CUR );
            break;
        case _LMF_DATA_REC:
            if( DIGLoadRead( lfh, &rec, sizeof( rec.data ) ) )
                return( NULL );
            if( ProcData( lfh, &rec.data, head.data_nbytes ) != EOK )
                return( NULL );
            break;
        case _LMF_FIXUP_SEG_REC:
            if( ProcFixup( lfh, head.data_nbytes ) != EOK )
                return( NULL );
            break;
        case _LMF_EOF_REC:
            for( i = 0; i < count; ++i ) {
                if( (segs[i] & 0xf0000000) == ((unsigned long)_LMF_CODE << 28) ) {
                    if( qnx_segment_flags( SuppSegs[i], _PMF_CODE_RX|_PMF_MODIFY ) == -1 ) {
                        return( NULL );
                    }
                }
            }
            return( MK_FP( SuppSegs[0], 0 ) );
        default:
            return( NULL );
        }
    }
}

static void UnloadSupp( unsigned short *p )
{
    unsigned short      *start;

    if( p != NULL ) {
        start = p;
        while( *p != -1 ) {
            qnx_segment_free( *p );
            ++p;
        }
        DIGCli( Free )( start );
    }
}
