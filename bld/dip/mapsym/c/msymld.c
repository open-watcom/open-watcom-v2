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
* Description:  Mapsym DIP loading/unloading of symbolic information.
*
****************************************************************************/


#include "msym.h"
#include "roundmac.h"
#include "exedos.h"
#include "exeos2.h"
#include "exeflat.h"


/*
 * Implementation notes:
 *
 * - Symbol files may contain multiple maps. This is not supported because
 *   I've never seen such a .sym file and I don't even know how to produce one.
 */

#if defined( __WATCOMC__ ) && defined( __386__ )
/*
 * WD looks for this symbol to determine module bitness
 */
int __nullarea;
#pragma aux __nullarea "*";
#endif

static struct {
    unsigned long       fpos;
    unsigned            len;
    unsigned            off;
    unsigned_8          data[4096];
} Buff;

static unsigned long BSeek( FILE *fp, unsigned long p, dig_seek where )
{
    unsigned long       bpos;
    unsigned long       npos = 0;

    bpos = Buff.fpos - Buff.len;
    switch( where ) {
    case DIG_SEEK_END:
        return( DIG_SEEK_ERROR ); /* unsupported */
    case DIG_SEEK_CUR:
        npos = bpos + p + Buff.off;
        break;
    case DIG_SEEK_ORG:
        npos = p;
        break;
    }
    if( npos >= bpos && npos < (bpos+Buff.len) ) {
        Buff.off = npos - bpos;
        return( npos );
    }
    DCSeek( fp, npos, DIG_SEEK_ORG );
    Buff.fpos = DCTell( fp );
    Buff.off = 0;
    Buff.len = 0;
    return( Buff.fpos );
}

static size_t BRead( FILE *fp, void *b, size_t s )
{
    size_t      got;
    size_t      want;

    if( s > sizeof( Buff.data ) ) {
        DCSeek( fp, Buff.fpos + Buff.off - Buff.len, DIG_SEEK_ORG );
        Buff.fpos = DCTell( fp );
        Buff.len = 0;
        Buff.off = 0;
        if( Buff.fpos == DIG_SEEK_ERROR )
            return( 0 );
        got = DCRead( fp, b, s );
        Buff.fpos += got;
        return( got );
    }
    want = s;
    got = Buff.len - Buff.off;
    if( got > want )
        got = want;
    memcpy( b, &Buff.data[Buff.off], got );
    Buff.off += got;
    want -= got;
    if( want > 0 ) {
        size_t  len;

        len = DCRead( fp, &Buff.data[0], sizeof( Buff.data ) );
        if( len == DIG_RW_ERROR ) {
            Buff.fpos = DIG_SEEK_ERROR;
            Buff.off = 0;
            Buff.len = 0;
            return( DIG_RW_ERROR );
        }
        Buff.len = len;
        Buff.fpos += Buff.len;
        b = (unsigned_8 *)b + got;
        memcpy( b, &Buff.data[0], want );
        Buff.off = want;
    }
    return( s );
}

static void *HunkAlloc( imp_image_handle *iih, size_t size )
{
    msym_hunk   *hunk;
    size_t      alloc;

    size = __ROUND_UP_SIZE( size, sizeof( void * ) );
    hunk = iih->hunks;
    if( hunk == NULL || size > hunk->left ) {
        alloc = HUNK_SIZE;
        if( alloc < size )
            alloc = size;
        hunk = DCAlloc( (sizeof( *hunk ) - HUNK_SIZE) + alloc );
        if( hunk == NULL )
            return( NULL );
        hunk->next = iih->hunks;
        iih->hunks = hunk;
        hunk->left = alloc;
    }
    hunk->left -= size;
    return( &hunk->data[hunk->left] );
}

static void ImpUnloadInfo( imp_image_handle *iih )
{
    msym_hunk   *curr;
    msym_hunk   *next;

    for( curr = iih->hunks; curr != NULL; curr = next ) {
        next = curr->next;
        DCFree( curr );
    }
    iih->hunks = NULL;
}

msym_block *FindAddrBlock( imp_image_handle *iih, addr_ptr addr )
{
    msym_block  *b;

    for( b = iih->addr; b != NULL; b = b->next ) {
        if( SameAddrSpace( b->start, addr )
          && b->start.offset <= addr.offset
          && (b->start.offset + b->len) > addr.offset ) {
            return( b );
        }
    }
    return( NULL );
}

static dip_status AddName( imp_image_handle *iih, unsigned len, char *name )
{
    char        *start;
    char        *end;

    end = NULL;
    start = name;
    for( ; len > 0; --len ) {
        switch( *name ) {
        case ':':
        case '\\':
        case '/':
            start = name + 1;
            end = NULL;
            break;
        case '.':
            end = name;
            break;
        }
        ++name;
    }
    if( end == NULL )
        end = name;
    iih->len = end - start;
    iih->name = HunkAlloc( iih, iih->len );
    if( iih->name == NULL )
        return( DS_ERR | DS_NO_MEM );
    memcpy( iih->name, start, iih->len );
    return( DS_OK );
}

static dip_status AddBlock( imp_image_handle *iih, addr_seg seg, addr_off off,
                        unsigned_32 len, unsigned_8 code )
{
    msym_block  *new;

    new = HunkAlloc( iih, sizeof( *new ) );
    if( new == NULL )
        return( DS_ERR | DS_NO_MEM );
    new->start.segment = seg;
    new->start.offset = off;
    new->len = len;
    new->code = code;
    new->next = iih->addr;
    iih->addr = new;
    return( DS_OK );
}

static dip_status AddSymbol( imp_image_handle *iih, addr_seg seg, addr_off off,
                        unsigned len, char *name )
{
    msym_sym    *new;

    new = HunkAlloc( iih, (sizeof( *new ) - 1) + len );
    if( new == NULL )
        return( DS_ERR | DS_NO_MEM );
    new->addr.segment = seg;
    new->addr.offset = off;
    new->len = len;
    memcpy( new->name, name, len );
    new->next = iih->gbl;
    iih->gbl = new;
    return( DS_OK );
}

static dip_status CheckSymFile( FILE *fp )
/*
 * Heuristics to determine whether given file is a MAPSYM .sym file
 */
{
    sym_endmap          end_map;
    unsigned long       pos;

    /*
     * seek to the end, read and check end map record
     */
    if( DCSeek( fp, DIG_SEEK_POSBACK( sizeof( end_map ) ), DIG_SEEK_END ) ) {
        return( DS_ERR | DS_FSEEK_FAILED );
    }
    pos = DCTell( fp );
    /*
     * the endmap record must be 16-byte aligned
     */
    if( pos % 16 ) {
        return( DS_FAIL );
    }
    if( DCRead( fp, &end_map, sizeof( end_map ) ) != sizeof( end_map ) ) {
        return( DS_ERR | DS_FREAD_FAILED );
    }
    if( end_map.zero != 0 ) {
        return( DS_FAIL );
    }
    /*
     * Check .sym file version. The version number seems to correspond to
     * the linker version. Versions 3.10, 4.0, 5.10, 5.11, 6.0, 6.10 have
     * been seen. Version 5.1 seems to be identical to 4.0 with added
     * support for 32-bit symbols.
     */
    if( (end_map.major_ver < 3) || (end_map.major_ver > 6) || (end_map.minor_ver > 11) ) {
        return( DS_FAIL );
    }
    /*
     * looks like the right sort of .sym file
     */
    return( DS_OK );
}

static dip_status ReadString( FILE *fp, char *buf, unsigned *len_ptr )
/*********************************************************************
 * Read a Pascal style string - limited to 255 chars max length
 */
{
    unsigned_8  str_len;

    if( BRead( fp, &str_len, sizeof( str_len ) ) != sizeof( str_len ) ) {
        return( DS_ERR | DS_FREAD_FAILED );
    }
    if( BRead( fp, buf, str_len ) != str_len ) {
        return( DS_ERR | DS_FREAD_FAILED );
    }
    buf[str_len] = '\0';            // NUL terminate string
    if( len_ptr != NULL )
        *len_ptr = str_len;
    return( DS_OK );
}

static dip_status LoadSymTable( FILE *fp, imp_image_handle *iih, unsigned count,
                        unsigned long base_ofs, unsigned_32 table_ofs,
                        addr_seg seg, int big_syms )
/*******************************************************************************
 * Load symbols for a segment
 */
{
    dip_status      ds;
    unsigned_16     *sym_tbl;
    size_t          tbl_size;
    sym_symdef      sym;
    sym_symdef_32   sym_32;
    char            name[256];
    unsigned        name_len;
    int             i;

    tbl_size = count * sizeof( unsigned_16 );
    sym_tbl = DCAlloc( tbl_size );
    if( sym_tbl == NULL ) {
        return( DS_ERR | DS_NO_MEM );
    }
    if( BSeek( fp, base_ofs + table_ofs, DIG_SEEK_ORG ) == DIG_SEEK_ERROR ) {
        ds = DS_ERR | DS_FSEEK_FAILED;
        goto done;
    }
    if( BRead( fp, sym_tbl, tbl_size ) != tbl_size ) {
        ds = DS_ERR | DS_FREAD_FAILED;
        goto done;
    }

    sym.offset = 0;
    sym_32.offset = 0;
    for( i = 0; i < count; ++i ) {
        if( BSeek( fp, base_ofs + sym_tbl[i], DIG_SEEK_ORG ) == DIG_SEEK_ERROR ) {
            ds = DS_ERR | DS_FSEEK_FAILED;
            goto done;
        }
        if( big_syms ) {
            if( BRead( fp, &sym_32, SYM_SYMDEF_32_FIXSIZE ) != SYM_SYMDEF_32_FIXSIZE ) {
                ds = DS_ERR | DS_FREAD_FAILED;
                goto done;
            }
            ds = ReadString( fp, name, &name_len );
            if( ds != DS_OK )
                goto done;
            ds = AddSymbol( iih, seg, sym_32.offset, name_len, name );
            if( ds != DS_OK ) {
                goto done;
            }
        } else {
            if( BRead( fp, &sym, SYM_SYMDEF_FIXSIZE ) != SYM_SYMDEF_FIXSIZE ) {
                ds = DS_ERR | DS_FREAD_FAILED;
                goto done;
            }
            ds = ReadString( fp, name, &name_len );
            if( ds != DS_OK )
                goto done;
            ds = AddSymbol( iih, seg, sym.offset, name_len, name );
            if( ds != DS_OK ) {
                goto done;
            }
        }
    }

    // The .sym file doesn't say how big a segment is. Since the symbols are
    // sorted by offset, just use the last offset as segment size. Should be
    // close enough.
    if( big_syms ) {
        iih->addr->len = sym_32.offset;
    } else {
        iih->addr->len = sym.offset;
    }

    ds = DS_OK;

done:
    DCFree( sym_tbl );
    return( ds );
}

static dip_status LoadSegments( FILE *fp, imp_image_handle *iih, int count )
/***************************************************************************
 * Load all segments for a map
 */
{
    dip_status      ds;
    sym_segdef      seg;
    char            name[256];
    unsigned        name_len;
    unsigned long   seg_start;
    int             i;
    int             is_code;

    for( i = 0; i < count; ++i ) {
        seg_start = BSeek( fp, 0, DIG_SEEK_CUR );
        if( seg_start == DIG_SEEK_ERROR ) {
            return( DS_ERR | DS_FSEEK_FAILED );
        }
        if( BRead( fp, &seg, SYM_SEGDEF_FIXSIZE ) != SYM_SEGDEF_FIXSIZE ) {
            return( DS_ERR | DS_FREAD_FAILED );
        }
        ds = ReadString( fp, name, &name_len );
        if( ds != DS_OK )
            return( ds );
        /*
         * There's no good way to tell whether segment is code or data. Try
         * to guess what a segment is based on its name.
         */
        if( strcmp( name, "DGROUP" ) == 0 ) {
            is_code = 0;
        } else {
            is_code = 1;
        }
        ds = AddBlock( iih, seg.load_addr, 0, 0, is_code );
        if( ds != DS_OK )
            return( ds );

        LoadSymTable( fp, iih, seg.num_syms, seg_start, seg.sym_tab_ofs,
            seg.load_addr, (seg.sym_type & SYM_FLAG_32BIT) != 0 );

        if( BSeek( fp, SYM_PTR_TO_OFS( seg.next_ptr ), DIG_SEEK_ORG ) == DIG_SEEK_ERROR ) {
            return( DS_ERR | DS_FSEEK_FAILED );
        }
    }
    return( DS_OK );
}

static dip_status LoadSymFile( FILE *fp, imp_image_handle *iih )
/***************************************************************
 * Load all symbols in a .sym file
 */
{
    dip_status      ds;
    sym_mapdef      map;
    char            name[256];
    unsigned        name_len;

    if( BSeek( fp, 0, DIG_SEEK_ORG ) == DIG_SEEK_ERROR ) {
        return( DS_ERR | DS_FSEEK_FAILED );
    }
    /*
     * Read the first map and use its name as the module name
     */
    if( BRead( fp, &map, SYM_MAPDEF_FIXSIZE ) != SYM_MAPDEF_FIXSIZE ) {
        return( DS_ERR | DS_FREAD_FAILED );
    }

    ds = ReadString( fp, name, &name_len );
    if( ds != DS_OK )
        return( ds );

    ds = AddName( iih, name_len, name );
    if( ds != DS_OK )
        return( ds );

    if( BSeek( fp, SYM_PTR_TO_OFS( map.seg_ptr ), DIG_SEEK_ORG ) == DIG_SEEK_ERROR ) {
        return( DS_ERR | DS_FSEEK_FAILED );
    }
    ds = LoadSegments( fp, iih, map.num_segs );
    if( ds != DS_OK )
        return( ds );

    return( DS_OK );
}

dip_status DIPIMPENTRY( LoadInfo )( FILE *fp, imp_image_handle *iih )
{
    dip_status  ds;

    iih->gbl = NULL;
    iih->addr = NULL;
    iih->name = NULL;
    iih->hunks = NULL;
    Buff.len = 0;
    Buff.off = 0;

    ds = CheckSymFile( fp );
    if( ds == DS_OK )
        ds = LoadSymFile( fp, iih );

    if( ds != DS_OK ) {
        DCStatus( ds );
        /*
         * clean up any allocations
         */
        ImpUnloadInfo( iih );
        return( ds );
    }
    return( DS_OK );
}

void DIPIMPENTRY( MapInfo )( imp_image_handle *iih, void *d )
{
    msym_block  *b;
    msym_sym    *s;

    for( s = iih->gbl; s != NULL; s = s->next ) {
        DCMapAddr( &s->addr, d );
    }
    for( b = iih->addr; b != NULL; b = b->next ) {
        DCMapAddr( &b->start, d );
    }
}

void DIPIMPENTRY( UnloadInfo )( imp_image_handle *iih )
{
    ImpUnloadInfo( iih );
}
