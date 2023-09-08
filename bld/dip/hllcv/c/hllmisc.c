/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Miscellaneous routines for HLL/CV debugging.
*
****************************************************************************/


#include "hllinfo.h"

/*
 * FIXME: kick out these!
 */
#include "cv4w.h"

enum {
#define _CVREG( name, num )     CV_X86_##name = num,
#include "cv4intl.h"
#undef _CVREG
#define _CVREG( name, num )     CV_AXP_##name = num,
#include "cv4axp.h"
CV_LAST_REG
};

const char      DIPImp( Name )[] = "HLL/CV";

unsigned DIPIMPENTRY( HandleSize )( handle_kind hk )
/***************************************************
 * Get the size of a handle.
 */
{
    static unsigned_8 Sizes[] = {
        #define pick(enum,hsize,ihsize,wvihsize,cvdmndtype,wdmndtype)   ihsize,
        #include "diphndls.h"
        #undef pick
    };

    return( Sizes[hk] );
}

dip_status DIPIMPENTRY( MoreMem )( size_t size )
/***********************************************
 * Free memory.
 */
{
    size = size;
    return( (VMShrink() != 0) ? DS_OK : DS_FAIL );
}

dip_status DIPImp( Startup )( void )
/***********************************
 * Module startup.
 */
{
    return( DS_OK );
}

void DIPIMPENTRY( Shutdown )( void )
/***********************************
 * Module shutdown.
 */
{
}

void DIPIMPENTRY( Cancel )( void )
/*********************************
 * ?
 */
{
}

size_t hllNameCopy( char *buff, const char *src, size_t buff_size, size_t len )
/******************************************************************************
 * Creates a zero terminated string.
 */
{
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > len )
            buff_size = len;
        memcpy( buff, src, buff_size );
        buff[buff_size] = '\0';
    }
    return( len );
}

hll_dir_entry *hllFindDirEntry( imp_image_handle *iih, imp_mod_handle imh, hll_sst sst )
/***************************************************************************************
 * Finds a subsection directory entry for a specific module.
 */
{
    unsigned            i;
    unsigned            block;
    unsigned            full_blocks;
    unsigned            remainder;
    hll_dir_entry       *p;

    full_blocks = BLOCK_FACTOR( iih->dir_count, DIRECTORY_BLOCK_ENTRIES ) - 1;
    for( block = 0; block < full_blocks; ++block ) {
        for( i = 0; i < DIRECTORY_BLOCK_ENTRIES; ++i ) {
            p = &iih->directory[block][i];
            if( p->iMod == imh && p->subsection == sst ) {
                return( p );
            }
        }
    }
    remainder = iih->dir_count - (full_blocks * DIRECTORY_BLOCK_ENTRIES);
    for( i = 0; i < remainder; ++i ) {
        p = &iih->directory[block][i];
        if( p->iMod == imh && p->subsection == sst ) {
            return( p );
        }
    }
    return( NULL );
}

walk_result hllWalkDirList( imp_image_handle *iih, hll_sst sst, DIP_DIR_WALKER *wk, void *d )
/********************************************************************************************
 * Walks the subsection directory.
 *
 * Use 'sst' to limit the callbacks to one specific type. A 'sst' of 0
 * means everything.
 */
{
    unsigned            i;
    unsigned            block;
    unsigned            full_blocks;
    unsigned            remainder;
    walk_result         wr;
    hll_dir_entry       *p;

    full_blocks = BLOCK_FACTOR( iih->dir_count, DIRECTORY_BLOCK_ENTRIES ) - 1;
    for( block = 0; block < full_blocks; ++block ) {
        for( i = 0; i < DIRECTORY_BLOCK_ENTRIES; ++i ) {
            p = &iih->directory[block][i];
            if( p->subsection == sst || sst == 0) {
                wr = wk( iih, p, d );
                if( wr != WR_CONTINUE ) {
                    return( wr );
                }
            }
        }
    }
    remainder = iih->dir_count - (full_blocks * DIRECTORY_BLOCK_ENTRIES);
    for( i = 0; i < remainder; ++i ) {
        p = &iih->directory[block][i];
        if( p->subsection == sst || sst == 0) {
            wr = wk( iih, p, d );
            if( wr != WR_CONTINUE ) {
                return( wr );
            }
        }
    }
    return( WR_CONTINUE );
}

static const struct {
    unsigned_8  k;
    unsigned_8  size;
} LeafInfo[] = {
    { TK_INTEGER,       1 },
    { TK_INTEGER,       2 },
    { TK_INTEGER,       2 },
    { TK_INTEGER,       4 },
    { TK_INTEGER,       4 },
    { TK_REAL,          4 },
    { TK_REAL,          8 },
    { TK_REAL,          10 },
    { TK_REAL,          16 },
    { TK_INTEGER,       8 },
    { TK_INTEGER,       8 },
    { TK_REAL,          6 },
    { TK_COMPLEX,       8 },
    { TK_COMPLEX,       16 },
    { TK_COMPLEX,       20 },
    { TK_COMPLEX,       32 },
    { TK_STRING,        0 },
};

const void *hllGetNumLeaf( const void *p, numeric_leaf *v )
{
    unsigned            key;

    key = *(unsigned_16 *)p;
    if( key < LF_NUMERIC ) {
        v->k = TK_INTEGER;
        v->size = sizeof( unsigned_16 );
        v->valp = p;
        v->int_val = key;
    } else {
        v->valp = (const unsigned_8 *)p + sizeof( unsigned_16 );
        v->size = LeafInfo[key - LF_NUMERIC].size;
        v->k = LeafInfo[key - LF_NUMERIC].k;
        switch( key ) {
        case LF_CHAR:
            v->int_val = *(signed_8 *)v->valp;
            break;
        case LF_SHORT:
            v->int_val = *(signed_16 *)v->valp;
            break;
        case LF_USHORT:
            v->int_val = *(unsigned_16 *)v->valp;
            break;
        case LF_LONG:
        case LF_ULONG:
            v->int_val = *(unsigned_32 *)v->valp;
            break;
        case LF_VARSTRING:
            v->size = *(unsigned_16 *)v->valp;
            v->valp = (const unsigned_8 *)v->valp + sizeof( unsigned_16 );
            break;
        }
    }
    return( (const unsigned_8 *)v->valp + v->size );
}


dip_status hllDoIndirection( imp_image_handle *iih, dig_type_info *ti,
                             location_context *lc, location_list *ll )
{
    union {
        unsigned_8      u8;
        unsigned_16     u16;
        unsigned_32     u32;
        addr32_off      ao32;
        addr48_off      ao48;
        addr32_ptr      ap32;
        addr48_ptr      ap48;
    }                   tmp;
    location_list       dst;
    dip_status          ds;

    iih = iih;
    hllLocationCreate( &dst, LT_INTERNAL, &tmp );
    ds = DCAssignLocation( &dst, ll, ti->size );
    if( ds != DS_OK )
        return( ds );
    ds = DCItemLocation( lc, CI_DEF_ADDR_SPACE, ll );
    if( ds != DS_OK )
        return( ds );
    if( ti->modifier == TM_NEAR ) {
        if( ti->size == sizeof( addr32_off ) ) {
            ll->e[0].u.addr.mach.offset = tmp.ao32;
        } else {
            ll->e[0].u.addr.mach.offset = tmp.ao48;
        }
    } else {
        if( ti->size == sizeof( addr32_ptr ) ) {
            ll->e[0].u.addr.mach.offset = tmp.ap32.offset;
            ll->e[0].u.addr.mach.segment = tmp.ap32.segment;
        } else {
            ll->e[0].u.addr.mach.offset = tmp.ap48.offset;
            ll->e[0].u.addr.mach.segment = tmp.ap48.segment;
        }
    }
    return( DS_OK );
}

void hllConfused( void )
/***********************
 * Don't know what's happening.
 */
{
    volatile int a = 0;
    volatile int b = 0;
    /*
     * cause a fault
     */
    a /= b;
}
