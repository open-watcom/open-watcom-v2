/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Munge OMF objects into something resembling a sensible
*               object format.
*
****************************************************************************/


#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "omfload.h"
#include "omfmunge.h"
#include "omforl.h"
#include "orlhash.h"

#include "clibext.h"


/* Local definitions
 */
#define STD_INC         256
#define STD_CODE_SIZE   4096

#define _IsSymPub( t )      ( t & ( ORL_SYM_TYPE_DEFINED | ORL_SYM_TYPE_ABSOLUTE ) )

#define _NewSymbol(h,t,n)   newSymbol( h, t, n->string, n->len )

static orl_sec_combine getCombine( int combine )
{
    switch( combine ) {
    case( COMB_INVALID ):
        return( ORL_SEC_COMBINE_PRIVATE );
    case( COMB_ADDOFF ):
    case( COMB_FOUR ):
        return( ORL_SEC_COMBINE_PUBLIC );
    case( COMB_STACK ):
        return( ORL_SEC_COMBINE_STACK );
    case( COMB_COMMON ):
        return( ORL_SEC_COMBINE_COMMON );
    default:
        return( ORL_SEC_COMBINE_NONE );
    }
}

static int nameCmp( omf_file_handle ofh, omf_idx n1, omf_idx n2 )
{
    omf_string          s1;
    omf_string          s2;

    assert( ofh );
    assert( n1 );
    assert( n2 );

    if( n1 == n2 )
        return( 0 );
    s1 = OmfGetLName( ofh->lnames, n1 );
    if( s1 == NULL )
        return( 1 );
    s2 = OmfGetLName( ofh->lnames, n2 );
    if( s2 == NULL )
        return( 1 );
    if( s1->len != s2->len )
        return( 1 );
    return( stricmp( s1->string, s2->string ) != 0 );
}


static void *checkArraySize( omf_file_handle ofh, void *old_arr, long num, long inc, long elem )
{
    long        size;
    void        *new_arr;

    assert( ofh );
    assert( num >= 0 );
    assert( inc > 0 );
    assert( elem > 0 );

    if( (num % inc) == 0 ) {
        size = ( num + inc ) * elem;
        new_arr = _ClientAlloc( ofh, size );
        if( new_arr == NULL )
            return( NULL );
        memset( new_arr, 0, size );
        if( num ) {
            assert( old_arr );
            size = num * elem;
            memcpy( new_arr, old_arr, size );
            _ClientFree( ofh, old_arr );
        }
        old_arr = new_arr;
    }
    return( old_arr );
}


static omf_symbol_handle findExtDefSym( omf_file_handle ofh, omf_idx ext )
{
    omf_sec_handle                  sh;
    omf_string                      extname;
    orl_hash_data_entry             data_entry;
    omf_symbol_handle               sym;
    orl_hash_key                    h_key;

    assert( ofh );

    if( ofh->extdefs == NULL )
        return( NULL );
    sh = ofh->extdefs;
    if( ext == 0 || ( ext > sh->assoc.string.num ) )
        return( NULL );
    assert( sh->assoc.string.strings );
    extname = sh->assoc.string.strings[ext - 1];
    if( extname == NULL )
        return( NULL );

    h_key.u.string = extname->string;
    for( data_entry = ORLHashTableQuery( ofh->symbol_table->assoc.sym.hash_tab, h_key ); data_entry != NULL; data_entry = data_entry->next ) {
        sym = (omf_symbol_handle)data_entry->data.u.sym_handle;
        if( sym != NULL ) {
            if( sym->typ & ( ORL_SYM_TYPE_UNDEFINED | ORL_SYM_TYPE_COMMON ) ) {
                return( sym );
            }
        }
    }
    return( NULL );
}


static omf_grp_handle   findGroup( omf_file_handle ofh, omf_idx grp )
{
    assert( ofh );

    if( grp == 0 || ( grp > ofh->num_groups ) )
        return( NULL );
    assert( ofh->groups );
    return( ofh->groups[grp - 1] );
}


static omf_sec_handle   findComDat( omf_file_handle ofh, omf_idx seg )
{
    assert( ofh );

    if( seg == 0 || ( seg > ofh->num_comdats ) )
        return( NULL );
    assert( ofh->comdats );
    return( ofh->comdats[seg - 1] );
}

static omf_sec_handle   findComDatByName( omf_file_handle ofh, omf_idx nameidx )
{
    unsigned    index;

    assert( ofh );
    assert( ofh->comdats );

    if( nameidx == 0 )
        return( NULL );
    for( index = 0; index < ofh->num_comdats; index++ ) {
        if( ofh->comdats[index]->assoc.seg.name == nameidx ) {
            return( ofh->comdats[index] );
        }
    }
    return( NULL );
}


static omf_sec_handle   findSegment( omf_file_handle ofh, omf_idx seg )
{
    assert( ofh );

    if( seg == 0 || ( seg > ofh->num_segs ) )
        return( NULL );
    assert( ofh->segs );
    return( ofh->segs[seg - 1] );
}


static omf_symbol_handle newSymbol( omf_file_handle ofh, orl_symbol_type typ, const char *buffer, omf_string_len len )
{
    omf_symbol_handle   sym;

    assert( ofh );
    assert( buffer );

    sym = _ClientAlloc( ofh, ORL_STRUCT_SIZEOF( omf_symbol_handle ) + len );
    if( sym != NULL ) {
        memset( sym, 0, ORL_STRUCT_SIZEOF( omf_symbol_handle ) + len );
        sym->typ = typ;
        sym->file_format = ORL_OMF;
        sym->omf_file_hnd = ofh;
        memcpy( sym->name.string, buffer, len );
        sym->name.string[len] = '\0';
        sym->name.len = len;
    }
    return( sym );
}

static omf_sec_handle   newSection( omf_file_handle ofh, omf_quantity idx, orl_sec_type typ )
{
    omf_sec_handle      sh;

    assert( ofh );
    assert( idx >= OMF_SEC_NEXT_AVAILABLE );

    if( idx == OMF_SEC_NEXT_AVAILABLE ) {
        if( ofh->next_idx < OMF_SEC_DATA_CODE_START ) {
            ofh->next_idx = OMF_SEC_DATA_CODE_START;
        }
        idx = ofh->next_idx;
        ofh->next_idx++;
    } else if( idx >= ofh->next_idx ) {
        ofh->next_idx = idx + 1;
    }

    sh = _ClientAlloc( ofh, ORL_STRUCT_SIZEOF( omf_sec_handle ) );
    if( sh == NULL )
        return( sh );
    memset( sh, 0, ORL_STRUCT_SIZEOF( omf_sec_handle ) );

    sh->file_format = ORL_OMF;
    sh->omf_file_hnd = ofh;
    sh->type = typ;
    sh->index = idx;

    if( ofh->first_sec != NULL ) {
        ofh->last_sec->next = sh;
    } else {
        ofh->first_sec = sh;
    }
    ofh->last_sec = sh;
    ofh->num_sections++;

    return( sh );
}


static omf_sec_handle   newComDatSection( omf_file_handle ofh )
{
    omf_sec_handle      sh;

    assert( ofh );

    ofh->comdats = checkArraySize( ofh, ofh->comdats, ofh->num_comdats, STD_INC, sizeof( omf_sec_handle ) );
    if( ofh->comdats == NULL )
        return( NULL );

    sh = newSection( ofh, OMF_SEC_NEXT_AVAILABLE, ORL_SEC_TYPE_PROG_BITS );
    if( sh == NULL )
        return( sh );

    ofh->comdats[ofh->num_comdats] = sh;
    ofh->num_comdats++;
    sh->assoc.seg.seg_id = ofh->num_comdats;

    return( sh );
}


static omf_sec_handle   newSegSection( omf_file_handle ofh, orl_sec_type typ )
{
    omf_sec_handle      sh;

    assert( ofh );

    ofh->segs = checkArraySize( ofh, ofh->segs, ofh->num_segs, STD_INC, sizeof( omf_sec_handle ) );
    if( ofh->segs == NULL )
        return( NULL );

    sh = newSection( ofh, OMF_SEC_NEXT_AVAILABLE, typ );
    if( sh == NULL )
        return( sh );

    ofh->segs[ofh->num_segs] = sh;
    ofh->num_segs++;
    sh->assoc.seg.seg_id = ofh->num_segs;

    return( sh );
}


static omf_grp_handle   newGroup( omf_file_handle ofh )
{
    omf_grp_handle      grp;

    assert( ofh );

    ofh->groups = checkArraySize( ofh, ofh->groups, ofh->num_groups, STD_INC, sizeof( omf_grp_handle ) );
    if( ofh->groups == NULL )
        return( NULL );

    grp = _ClientAlloc( ofh, ORL_STRUCT_SIZEOF( omf_grp_handle ) );
    if( grp == NULL )
        return( grp );
    memset( grp, 0, ORL_STRUCT_SIZEOF( omf_grp_handle ) );

    ofh->groups[ofh->num_groups] = grp;
    ofh->num_groups++;
    grp->idx = ofh->num_groups;
    grp->file_format = ORL_OMF;
    grp->omf_file_hnd = ofh;

    return( grp );
}


static omf_string getIdx2String( omf_sec_handle sh, omf_idx idx )
{
    assert( sh );

    if( idx < sh->assoc.string.num ) {
        return( sh->assoc.string.strings[idx] );
    }
    return( NULL );
}


static omf_sec_handle   newStringTable( omf_file_handle ofh, omf_quantity idx )
{
    omf_sec_handle      sh;

    assert( ofh );

    sh = newSection( ofh, idx, ORL_SEC_TYPE_STR_TABLE );
    if( sh != NULL ) {
        sh->flags = ORL_SEC_FLAG_REMOVE;
    }
    return( sh );
}


static orl_return       addString( omf_sec_handle sh, const char *buffer, omf_string_len len )
{
    omf_string          str;
    omf_file_handle     ofh;

    assert( sh );
    assert( buffer );

    ofh = sh->omf_file_hnd;

    /* Check if we need to allocate more string table
     */
    sh->assoc.string.strings = checkArraySize( ofh, sh->assoc.string.strings, sh->assoc.string.num, STD_INC, sizeof( omf_string ) );
    if( sh->assoc.string.strings == NULL )
        return( ORL_OUT_OF_MEMORY );

    str = (omf_string)_ClientAlloc( ofh, ORL_STRUCT_SIZEOF( omf_string ) + len );
    if( str == NULL )
        return( ORL_OUT_OF_MEMORY );

    memset( str, 0, ORL_STRUCT_SIZEOF( omf_string ) + len );
    memcpy( str->string, buffer, len );
    str->string[len] = '\0';
    str->len = len;

    sh->assoc.string.strings[sh->assoc.string.num] = str;
    sh->assoc.string.num++;
    return( ORL_OKAY );
}


static orl_return       addToSymbolTable( omf_file_handle ofh, omf_symbol_handle sym )
{
    omf_sec_handle      sh;
    orl_hash_key        h_key;
    orl_hash_data       h_data;

    assert( ofh );
    assert( sym );

    sh = ofh->symbol_table;
    if( sh == NULL ) {
        sh = newSection( ofh, OMF_SEC_SYM_TABLE_INDEX, ORL_SEC_TYPE_SYM_TABLE );
        if( sh == NULL )
            return( ORL_OUT_OF_MEMORY );
        ofh->symbol_table = sh;
        sh->flags |= ORL_SEC_FLAG_REMOVE;
        sh->assoc.sym.hash_tab = ORLHashTableCreate( ofh->omf_hnd->funcs, STD_HASH_TABLE_SIZE, ORL_HASH_STRING_IGNORECASE );
        if( sh->assoc.sym.hash_tab == NULL ) {
            return( ORL_OUT_OF_MEMORY );
        }
    }
    assert( sh->assoc.sym.hash_tab );

    sh->assoc.sym.syms = checkArraySize( ofh, sh->assoc.sym.syms, sh->assoc.sym.num, STD_INC, sizeof( omf_symbol_handle ) );
    if( sh->assoc.sym.syms == NULL )
        return( ORL_OUT_OF_MEMORY );

    sh->assoc.sym.syms[sh->assoc.sym.num] = sym;
    sh->assoc.sym.num++;
    h_key.u.string = sym->name.string;
    h_data.u.sym_handle = (orl_symbol_handle)sym;
    return( ORLHashTableInsert( sh->assoc.sym.hash_tab, h_key, h_data ) );
}


static orl_return       addReloc( omf_file_handle ofh, orl_reloc orel )
{
    omf_sec_handle      sh;

    assert( ofh );
    assert( orel );

    if( ofh->relocs == NULL ) {
        ofh->relocs = newSection( ofh, OMF_SEC_RELOC_INDEX, ORL_SEC_TYPE_RELOCS );
        if( ofh->relocs == NULL )
            return( ORL_OUT_OF_MEMORY );
        ofh->relocs->flags |= ORL_SEC_FLAG_REMOVE;
    }

    sh = ofh->relocs;
    sh->assoc.reloc.relocs = checkArraySize( ofh, (void *)sh->assoc.reloc.relocs, sh->assoc.reloc.num, STD_INC, sizeof( orl_reloc ) );
    if( sh->assoc.reloc.relocs == NULL )
        return( ORL_OUT_OF_MEMORY );

    sh->assoc.reloc.relocs[sh->assoc.reloc.num] = orel;
    sh->assoc.reloc.num++;

    return( ORL_OKAY );
}


static omf_tmp_fixup findMatchingFixup( omf_tmp_fixup ftr, omf_sec_offset lo, omf_sec_offset hi )
{
    if( hi < lo ) {
        hi = lo;
    }

    while( ftr != NULL ) {
        if( ( ftr->offset >= lo ) && ( ftr->offset <= hi ) )
            break;
        ftr = ftr->next;
    }

    return( ftr );
}


static orl_return   writeAndFixupLIData( omf_file_handle ofh, omf_sec_handle sh, omf_bytes buffer )
{
    int                 wordsize;
    omf_rec_size        tmp;
    unsigned_32         repeat;
    long                block;
    omf_rec_size        size;
    omf_sec_offset      used;
    omf_sec_offset      hi;
    omf_sec_offset      lo;
    omf_bytes           ptr;
    orl_return          return_val;
    omf_tmp_fixup       ftr;
    omf_tmp_fixup       ntr;
    int                 x;

    assert( ofh );
    assert( ofh->lidata );
    assert( sh );
    assert( sh->contents );
    assert( buffer );

    if( ofh->status & OMF_STATUS_EASY_OMF ) {
        wordsize = OmfGetWordSize( 0 );
    } else {
        wordsize = OmfGetWordSize( ofh->lidata->is32 );
    }
    tmp = wordsize + 2;

    if( ofh->lidata->size < tmp )
        return( ORL_ERROR );
    used = ofh->lidata->used;
    size = ofh->lidata->size;
    ptr = buffer + used;

    repeat = OmfGetUWord( ptr, wordsize );
    ptr += wordsize;
    block = OmfGetU16( ptr );
    ptr += 2;

    lo = used;
    hi = lo + tmp;
    size -= tmp;
    used += tmp;

    return_val = ORL_OKAY;
    if( findMatchingFixup( ofh->lidata->first_fixup, lo, hi ) ) {
        return( ORL_ERROR );
    } else if( block ) {
        while( repeat ) {
            repeat--;
            ofh->lidata->used = used;
            ofh->lidata->size = size;
            for( x = 0; x < block; x++ ) {
                return_val = writeAndFixupLIData( ofh, sh, buffer );
                if( return_val != ORL_OKAY ) {
                    break;
                }
            }
        }
    } else {
        tmp = *ptr;
        ptr++;
        used++;
        size--;

        lo = used;
        hi = used + tmp - 1;

        while( repeat ) {
            buffer = sh->contents + sh->assoc.seg.cur_offset;
            memcpy( buffer, ptr, tmp );

            /* create appropriate fixup records
             */
            ftr = findMatchingFixup( ofh->lidata->first_fixup, lo, hi );
            while( ftr != NULL ) {
                ntr = _ClientAlloc( ofh, ORL_STRUCT_SIZEOF( omf_tmp_fixup ) );
                if( ntr == NULL )
                    return( ORL_OUT_OF_MEMORY );
                memcpy( ntr, ftr, ORL_STRUCT_SIZEOF( omf_tmp_fixup ) );

                /* determine new offset as if this was an LEData we were
                 * doing a fixup for
                 */
                ntr->offset = ofh->lidata->offset - used + ftr->offset;

                /* insert into new fixup queue
                 */
                if( ofh->lidata->new_fixup != NULL ) {
                    ofh->lidata->last_fixup->next = ntr;
                } else {
                    ofh->lidata->new_fixup = ntr;
                }
                ofh->lidata->last_fixup = ntr;

                ftr = findMatchingFixup( ftr->next, lo, hi );
            }

            ofh->lidata->offset += tmp;
            sh->assoc.seg.cur_offset += tmp;
            repeat--;
        }

        ofh->lidata->size = size - tmp;
        ofh->lidata->used = used + tmp;
    }

    return( return_val );
}


static orl_return       expandPrevLIData( omf_file_handle ofh )
{
    omf_sec_handle      sh;
    int                 size;
    omf_bytes           buffer;
    orl_return          return_val = ORL_OKAY;
    unsigned char       tmp[1024];
    omf_tmp_fixup       ftr;
    omf_sec_offset      offset;

    assert( ofh );
    assert( ofh->work_sec );
    assert( ofh->lidata );

    sh = ofh->work_sec;
    size = ofh->lidata->size;

    if( size > 1024 ) {
        buffer = _ClientAlloc( ofh, size );
        if( buffer == NULL ) {
            return( ORL_OUT_OF_MEMORY );
        }
    } else {
        buffer = tmp;
    }

    /* we must remember to save the current offset of the lidata and then
     * restore it
     */
    ofh->lidata->last_fixup = NULL;
    offset = sh->assoc.seg.cur_offset;

    /* make a working copy of the LIDATA
     */
    memcpy( buffer, sh->contents + offset, size );

    while( ofh->lidata->size > 0 ) {
        return_val = writeAndFixupLIData( ofh, sh, buffer );
        if( return_val != ORL_OKAY ) {
            return( return_val );
        }
    }
    sh->assoc.seg.cur_offset = offset;

    if( size > 1024 ) {
        _ClientFree( ofh, buffer );
    }

    assert( (ofh->status & OMF_STATUS_ADD_MASK) == 0 );

    /* Destroy original fixups
     */
    while( ofh->lidata->first_fixup != NULL ) {
        ftr = ofh->lidata->first_fixup;
        ofh->lidata->first_fixup = ftr->next;
        _ClientFree( ofh, ftr );
    }

    /* Add the generated fixups
     */
    while( ofh->lidata->new_fixup != NULL ) {
        ftr = ofh->lidata->new_fixup;
        return_val = OmfAddFixupp( ofh, ftr->is32, ftr->mode, ftr->fix_loc, ftr->offset,
                            &ftr->fthread, &ftr->tthread, ftr->disp );
        if( return_val != ORL_OKAY )
            break;
        ofh->lidata->new_fixup = ftr->next;
        _ClientFree( ofh, ftr );
    }

    ofh->lidata->last_fixup = NULL;

    return( return_val );
}


static orl_return       applyBakpats( omf_file_handle ofh )
{
    omf_sec_handle      sh;
    orl_return          return_val = ORL_OKAY;
    omf_tmp_bkfix       tbf;
    unsigned_8          *pfix8;
    unsigned_16         *pfix16;
    unsigned_32         *pfix32;

    assert( ofh );
    assert( ofh->bakpat );

    ofh->bakpat->last_fixup = NULL;

    /* Go through all the backpatches and update the segment data. The BAKPATs
     * aren't normal fixups and may modify all sorts of instructions.
     */
    while( ofh->bakpat->first_fixup != NULL ) {
        tbf = ofh->bakpat->first_fixup;
        ofh->bakpat->first_fixup = tbf->next;

        assert( tbf->segidx || tbf->symidx );
        if( tbf->segidx ) {
            sh = findSegment( ofh, tbf->segidx );
        } else {
            sh = findComDatByName( ofh, tbf->symidx );
        }
        if( sh == NULL ) {
            return_val = ORL_ERROR;
            break;
        }
        switch( tbf->reltype ) {
        case ORL_RELOC_TYPE_WORD_8:
            pfix8 = sh->contents + tbf->offset;
            *pfix8 += (unsigned_8)tbf->disp;
            break;
        case ORL_RELOC_TYPE_WORD_16:
            pfix16 = (unsigned_16 *)(sh->contents + tbf->offset);
            *pfix16 += (unsigned_16)tbf->disp;
            break;
        case ORL_RELOC_TYPE_WORD_32:
            pfix32 = (unsigned_32 *)(sh->contents + tbf->offset);
            *pfix32 += tbf->disp;
            break;
        default:
            assert( 0 );
            return_val = ORL_ERROR;
            break;
        }
        if( return_val != ORL_OKAY )
            break;

        _ClientFree( ofh, tbf );
    }

    assert( (ofh->status & OMF_STATUS_ADD_BAKPAT) == 0 );
    /* Free the entire bakpat structure as well. */
    _ClientFree( ofh, ofh->bakpat );
    ofh->bakpat = NULL;

    return( return_val );
}


static orl_return       finishPrevWork( omf_file_handle ofh )
{
    orl_return  return_val = ORL_OKAY;

    assert( ofh );

    if( ofh->status & OMF_STATUS_ADD_LIDATA ) {
        ofh->status &= ~OMF_STATUS_ADD_LIDATA;
        return_val = expandPrevLIData( ofh );
    }
    /* NB: We're assuming that a BAKPAT/NBKPAT record always follows
     * the LEDATA (or possibly LIDATA) record it modifies. This is
     * not guaranteed by the TIS OMF spec, but the backpatch records
     * make no sense otherwise.
     */
    if( ofh->status & OMF_STATUS_ADD_BAKPAT ) {
        ofh->status &= ~OMF_STATUS_ADD_BAKPAT;
        return_val = applyBakpats( ofh );
    }
    assert( (ofh->status & OMF_STATUS_ADD_MASK) == 0 );
    return( return_val );
}


static omf_sec_offset   calcLIDataLength( bool is32, omf_bytes *input, omf_rec_size *len )
{
    omf_rec_size    size;
    omf_bytes       buffer;
    int             wordsize;
    long            tmp;
    unsigned_32     repeat;
    long            block;
    long            result = 0;

    assert( input );
    assert( *input );
    assert( len );

    buffer = *input;
    size = *len;
    wordsize = OmfGetWordSize( is32 );
    tmp = wordsize + 2;

    if( size < tmp )
        return( 0 );
    repeat = OmfGetUWord( buffer, wordsize );
    buffer += wordsize;
    block = OmfGetU16( buffer );
    buffer += 2;
    size -= tmp;

    if( block ) {
        while( block ) {
            tmp = calcLIDataLength( is32, &buffer, &size );
            if( tmp == 0 )
                return( 0 );
            result += tmp;
            block--;
        }
    } else {
        result = buffer[0];
        size -= ( result + 1 );
        if( size < 0 )
            return( 0 );
        buffer += result + 1;
    }

    result *= repeat;
    *input = buffer;
    *len = size;

    return( result );
}


static orl_return       checkSegmentLength( omf_sec_handle sh, unsigned_32 max )
{
    omf_bytes   conts;

    assert( sh );

    if( max > sh->size )
        return( ORL_ERROR );
    if( max > sh->assoc.seg.cur_size ) {
        max = ( max / STD_CODE_SIZE ) + 1;
        max *= STD_CODE_SIZE;
        conts = _ClientAlloc( sh->omf_file_hnd, max );
        if( conts == NULL )
            return( ORL_OUT_OF_MEMORY );
        memset( conts, 0, max );
        if( sh->contents != NULL ) {
            memcpy( conts, sh->contents, sh->assoc.seg.cur_size );
            _ClientFree( sh->omf_file_hnd, sh->contents );
        }
        sh->contents = conts;
        sh->assoc.seg.cur_size = max;
    }
    return( ORL_OKAY );
}


static int strNUpper( char *str, omf_string name )
{
    int     i;

    assert( str );

    for( i = 0; i < name->len; ++i ) {
        *str++ = (char)toupper( (unsigned char)name->string[i] );
    }
    *str = '\0';
    return( i );
}


static orl_sec_flags getSegSecFlags( omf_file_handle ofh, omf_idx name, omf_idx class,
                                    orl_sec_alignment align, int combine, bool use32 )
{
    char                lname[257];
    orl_sec_flags       flags;
    omf_string          str;
    int                 slen;

    /* unused parameters */ (void)align; (void)combine;

    assert( ofh );

    flags = ORL_SEC_FLAG_NONE;
    str = OmfGetLName( ofh->lnames, class );
    if( str != NULL ) {
        slen = strNUpper( lname, str );
        if( ( slen > 3 ) && ( strcmp( "CODE", &lname[slen - 4] ) == 0 || strcmp( "TEXT", &lname[slen - 4] ) == 0 ) ) {
            flags |= ORL_SEC_FLAG_EXEC | ORL_SEC_FLAG_EXECUTE_PERMISSION | ORL_SEC_FLAG_READ_PERMISSION;
        } else if( ( slen > 3 ) && strcmp( "DATA", &lname[slen - 4] ) == 0 ) {
            flags |= ORL_SEC_FLAG_READ_PERMISSION;
            str = OmfGetLName( ofh->lnames, name );
            if( str != NULL ) {
                slen = strNUpper( lname, str );
                if( strstr( "CONST", lname ) ) {
                    flags |= ORL_SEC_FLAG_INITIALIZED_DATA;
                }
            }
        } else if( ( slen > 4 ) && strcmp( "CONST", &lname[slen - 5] ) == 0 ) {
            flags |= ORL_SEC_FLAG_READ_PERMISSION | ORL_SEC_FLAG_INITIALIZED_DATA;
        } else if( ( slen > 2 ) && strcmp( "BSS", &lname[slen - 3] ) == 0 ) {
            flags |= ORL_SEC_FLAG_READ_PERMISSION | ORL_SEC_FLAG_WRITE_PERMISSION | ORL_SEC_FLAG_UNINITIALIZED_DATA;
        } else if( ( slen > 4 ) && strcmp( "STACK", &lname[slen - 5] ) == 0 ) {
            flags |= ORL_SEC_FLAG_READ_PERMISSION | ORL_SEC_FLAG_WRITE_PERMISSION | ORL_SEC_FLAG_UNINITIALIZED_DATA;
        } else {
            flags |= ORL_SEC_FLAG_READ_PERMISSION;
            str = OmfGetLName( ofh->lnames, name );
            slen = strNUpper( lname, str );
            if( ( slen > 3 ) && ( strcmp( "CODE", &lname[slen - 4] ) == 0 || strcmp( "TEXT", &lname[slen - 4] ) == 0 ) ) {
                flags |= ORL_SEC_FLAG_EXEC | ORL_SEC_FLAG_EXECUTE_PERMISSION;
            }
        }
    } else {
        flags |= ORL_SEC_FLAG_READ_PERMISSION;
    }

    if( use32 ) {
        flags |= ORL_SEC_FLAG_USE_32;
    } else {
        flags |= ORL_SEC_FLAG_USE_16;
    }

    return( flags );
}


static orl_return   OmfAddFileName( omf_file_handle ofh, char *buffer, omf_string_len len )
{
    omf_symbol_handle   sym;

    sym = newSymbol( ofh, ORL_SYM_TYPE_FILE, buffer, len );
    if( sym == NULL ) {
        return( ORL_OUT_OF_MEMORY );
    }
    return( addToSymbolTable( ofh, sym ) );
}


orl_return OmfAddLIData( omf_file_handle ofh, bool is32, omf_idx seg, omf_sec_offset offset,
                            omf_bytes buffer, omf_rec_size len, bool comdat )
{
    omf_sec_handle      sh;
    orl_return          return_val;
    omf_sec_offset      size;
    omf_sec_offset      tmpsize;
    omf_bytes           tmp;
    omf_rec_size        tmplen;

    assert( ofh );
    assert( buffer );
    assert( seg );

    return_val = finishPrevWork( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );

    if( comdat ) {
        sh = findComDat( ofh, seg );
    } else {
        sh = findSegment( ofh, seg );
    }
    if( sh == NULL )
        return( ORL_ERROR );
    if( len < 0 )
        return( ORL_ERROR );

    tmp = buffer;
    tmplen = len;
    size = 0;

    while( tmplen > 0 ) {
        if( ofh->status & OMF_STATUS_EASY_OMF ) {
            tmpsize = calcLIDataLength( false, &tmp, &tmplen );
        } else {
            tmpsize = calcLIDataLength( is32, &tmp, &tmplen );
        }
        if( tmpsize == 0 )
            return( ORL_ERROR );
        size += tmpsize;
    }

    return_val = checkSegmentLength( sh, offset + size );
    if( return_val != ORL_OKAY )
        return( return_val );

    /* we put off expanding the lidata until all the fixups are in
     */
    if( ofh->lidata == NULL ) {
        ofh->lidata = _ClientAlloc( ofh, ORL_STRUCT_SIZEOF( omf_tmp_lidata ) );
        if( ofh->lidata == NULL ) {
            return( ORL_OUT_OF_MEMORY );
        }
    }

    memset( ofh->lidata, 0, ORL_STRUCT_SIZEOF( omf_tmp_lidata ) );
    ofh->lidata->size = len;
    ofh->lidata->is32 = is32;

    sh->assoc.seg.cur_offset = offset;
    memcpy( sh->contents + offset, buffer, len );

    ofh->status |= OMF_STATUS_ADD_LIDATA;
    ofh->work_sec = sh;

    return( return_val );
}


orl_return OmfAddLEData( omf_file_handle ofh, bool is32, omf_idx seg, omf_sec_offset offset,
                            omf_bytes buffer, omf_rec_size len, bool comdat )
{
    omf_sec_handle      sh;
    orl_return          return_val;

    /* unused parameters */ (void)is32;

    assert( ofh );
    assert( buffer );
    assert( seg );

    return_val = finishPrevWork( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );

    if( comdat ) {
        sh = findComDat( ofh, seg );
    } else {
        sh = findSegment( ofh, seg );
    }
    if( sh == NULL )
        return( ORL_ERROR );
    if( len < 0 )
        return( ORL_ERROR );

    return_val = checkSegmentLength( sh, offset + len );
    if( return_val != ORL_OKAY )
        return( return_val );

    sh->assoc.seg.cur_offset = offset;
    memcpy( sh->contents + offset, buffer, len );
    ofh->work_sec = sh;

    return( return_val );
}


orl_return OmfAddLName( omf_file_handle ofh, const char *buffer, omf_string_len len )
{
    assert( ofh );
    assert( buffer );

    if( ofh->lnames == NULL ) {
        ofh->lnames = newStringTable( ofh, OMF_SEC_LNAME_INDEX );
        if( ofh->lnames == NULL ) {
            return( ORL_OUT_OF_MEMORY );
        }
    }
    return( addString( ofh->lnames, buffer, len ) );
}


omf_string OmfGetLastExtName( omf_file_handle ofh )
{
    omf_sec_handle sh = ofh->extdefs;

    assert( ofh );

    return( sh->assoc.string.strings[sh->assoc.string.num - 1] );
}


orl_return OmfAddBakpat( omf_file_handle ofh, unsigned_8 loctype, omf_sec_offset location,
                            omf_idx segidx, omf_idx symidx, omf_sec_addend disp )
{
    omf_tmp_bkfix       tbf;
    orl_reloc_type      reltype;

    assert( ofh );

    /* The BAKPAT records must be applied at the end. One of segidx/symidx
     * must be set to distinguish between BAKPAT and NBKPAT.
     */
    if( ofh->bakpat == NULL ) {
        ofh->bakpat = _ClientAlloc( ofh, ORL_STRUCT_SIZEOF( omf_tmp_bakpat ) );
        if( ofh->bakpat == NULL )
            return( ORL_OUT_OF_MEMORY );
        memset( ofh->bakpat, 0, ORL_STRUCT_SIZEOF( omf_tmp_bakpat ) );
        ofh->status |= OMF_STATUS_ADD_BAKPAT;
    }

    assert( ofh->status & OMF_STATUS_ADD_BAKPAT );

    /* Translate the location type. */
    switch( loctype ) {
    case 0:
        reltype = ORL_RELOC_TYPE_WORD_8;
        break;
    case 1:
        reltype = ORL_RELOC_TYPE_WORD_16;
        break;
    case 2:
    case 9:
        reltype = ORL_RELOC_TYPE_WORD_32;
        break;
    default:
        return( ORL_ERROR );
    }

    tbf = _ClientAlloc( ofh, ORL_STRUCT_SIZEOF( omf_tmp_bkfix ) );
    if( tbf == NULL )
        return( ORL_OUT_OF_MEMORY );
    memset( tbf, 0, ORL_STRUCT_SIZEOF( omf_tmp_bkfix ) );

    tbf->reltype = reltype;
    tbf->segidx  = segidx;
    tbf->symidx  = symidx;
    tbf->offset  = location;
    tbf->disp    = disp;

    if( ofh->bakpat->last_fixup ) {
        ofh->bakpat->last_fixup->next = tbf;
    } else {
        ofh->bakpat->first_fixup = tbf;
    }
    ofh->bakpat->last_fixup = tbf;

    return( ORL_OKAY );
}


orl_return OmfAddFixupp( omf_file_handle ofh, bool is32, bool mode, omf_fix_loc fix_loc,
                        omf_sec_offset offset, ORL_STRUCT( omf_thread_fixup ) *fthread,
                        ORL_STRUCT( omf_thread_fixup ) *tthread, omf_sec_addend disp )
{
    omf_tmp_fixup           ftr;
    ORL_STRUCT( orl_reloc ) *orel;
    omf_sec_handle          sh;
    omf_grp_handle          grp;
    ORL_STRUCT( omf_thread_fixup ) frame;

    assert( ofh );

    /*
     * remap FRAME_LOC to FRAME_SEG of curr segment
     */
    if( fthread->method == FRAME_LOC ) {
        frame.method = FRAME_SEG;
        frame.idx = ofh->work_sec->assoc.seg.seg_id;
    } else {
        frame = *fthread;
    }

    if( ofh->status & OMF_STATUS_ADD_LIDATA ) {
        assert( ofh->work_sec );
        assert( ofh->lidata );

        ftr = _ClientAlloc( ofh, ORL_STRUCT_SIZEOF( omf_tmp_fixup ) );
        if( ftr == NULL )
            return( ORL_OUT_OF_MEMORY );
        memset( ftr, 0, ORL_STRUCT_SIZEOF( omf_tmp_fixup ) );

        ftr->is32 = is32;
        ftr->mode = mode;
        ftr->fix_loc = fix_loc;
        ftr->offset = offset;
        ftr->fthread = frame;
        ftr->tthread = *tthread;
        ftr->disp = disp;

        if( ofh->lidata->last_fixup ) {
            ofh->lidata->last_fixup->next = ftr;
        } else {
            ofh->lidata->first_fixup = ftr;
        }
        ofh->lidata->last_fixup = ftr;
        return( ORL_OKAY );
    }

    orel = _ClientAlloc( ofh, ORL_STRUCT_SIZEOF( orl_reloc ) );
    if( orel == NULL )
        return( ORL_OUT_OF_MEMORY );
    memset( orel, 0, ORL_STRUCT_SIZEOF( orl_reloc ) );

    switch( fix_loc ) {
    case( LOC_OFFSET_LO ):
        /*
         * relocate lo byte of offset
         */
        if( mode ) {
            orel->type = ORL_RELOC_TYPE_WORD_8;
        } else {
            orel->type = ORL_RELOC_TYPE_REL_8;
        }
        break;
    case( LOC_OFFSET ):
        /*
         * relocate 16-bit offset
         */
        if( mode ) {
            orel->type = ORL_RELOC_TYPE_WORD_16;
        } else {
            orel->type = ORL_RELOC_TYPE_REL_16;
        }
        break;
    case( LOC_BASE ):
        /*
         * relocate segment
         */
        orel->type = ORL_RELOC_TYPE_SEGMENT;
        break;
    case( LOC_BASE_OFFSET ):
        /*
         * relocate segment and 16-bit offset
         */
        if( mode ) {
            orel->type = ORL_RELOC_TYPE_WORD_16_SEG;
        } else {
            orel->type = ORL_RELOC_TYPE_REL_16_SEG;
        }
        break;
    case( LOC_OFFSET_HI ):
        /*
         * relocate hi byte of offset
         */
        if( mode ) {
            orel->type = ORL_RELOC_TYPE_WORD_HI_8;
        } else {
            orel->type = ORL_RELOC_TYPE_REL_HI_8;
        }
        break;
    case( LOC_OFFSET_LOADER ):
        if( ofh->status & OMF_STATUS_EASY_OMF ) {
            /*
             * Pharlap, relocate 32-bit offset
             */
            if( mode ) {
                orel->type = ORL_RELOC_TYPE_WORD_32;
            } else {
                orel->type = ORL_RELOC_TYPE_REL_32;
            }
            break;
        }
        /*
         * relocate 16-bit offset, loader resolved
         */
        if( mode ) {
            orel->type = ORL_RELOC_TYPE_WORD_16;
        } else {
            orel->type = ORL_RELOC_TYPE_REL_16;
        }
        break;
    case( LOC_OFFSET_32 ):
    case( LOC_OFFSET_32_LOADER ):
        /*
         * relocate 32-bit offset
         * relocate 32-bit offset, loader resolved
         */
        if( mode ) {
            orel->type = ORL_RELOC_TYPE_WORD_32;
        } else {
            orel->type = ORL_RELOC_TYPE_REL_32;
        }
        break;
    case( LOC_PHARLAP_BASE_OFFSET_32 ):
        /*
         * Pharlap, relocate segment and 32-bit offset
         */
        if( (ofh->status & OMF_STATUS_EASY_OMF) == 0 ) {
            return( ORL_ERROR );
        }
        /* fall through */
    case( LOC_BASE_OFFSET_32 ):         /* relocate 48-bit pointer            */
        if( mode ) {
            orel->type = ORL_RELOC_TYPE_WORD_32_SEG;
        } else {
            orel->type = ORL_RELOC_TYPE_REL_32_SEG;
        }
        break;
    default:
        return( ORL_ERROR );
    }
    /*
     * no section for fixups to refer to
     */
    if( ofh->work_sec == NULL )
        return( ORL_ERROR );

    orel->offset = offset + ofh->work_sec->assoc.seg.cur_offset;
    orel->section = (orl_sec_handle)(ofh->work_sec);
    orel->addend = disp;

    switch( tthread->method ) {
    case TARGET_SEG:            /* segment index      */
        sh = findSegment( ofh, tthread->idx );
        if( sh == NULL )
            return( ORL_ERROR );
        orel->symbol = (orl_symbol_handle)(sh->assoc.seg.sym);
        break;
    case TARGET_GRP:            /* group index        */
        grp = findGroup( ofh, tthread->idx );
        if( grp == NULL )
            return( ORL_ERROR );
        orel->symbol = (orl_symbol_handle)(grp->sym);
        break;
    case TARGET_EXT:            /* external index     */
        orel->symbol = (orl_symbol_handle)(findExtDefSym( ofh, tthread->idx ));
        if( orel->symbol == NULL )
            return( ORL_ERROR );
        break;
    case TARGET_ABS:            /* abs frame num      */
        break;
    default:
        return( ORL_ERROR );
    }

    switch( frame.method ) {
    case( FRAME_SEG ):                      /* segment index                */
        sh = findSegment( ofh, frame.idx );
        if( sh == NULL )
            return( ORL_ERROR );
        orel->frame = (orl_symbol_handle)(sh->assoc.seg.sym);
        break;
    case( FRAME_GRP ):                      /* group index                  */
        grp = findGroup( ofh, frame.idx );
        if( grp == NULL )
            return( ORL_ERROR );
        orel->frame = (orl_symbol_handle)(grp->sym);
        break;
    case( FRAME_EXT ):                      /* external index               */
        orel->frame = (orl_symbol_handle)(findExtDefSym( ofh, frame.idx ));
        if( orel->frame == NULL )
            return( ORL_ERROR );
        break;
    case( FRAME_ABS ):                      /* absolute frame number        */
        /* fix this up to do somehting later */
        orel->frame = NULL;
        break;
    case( FRAME_TARG ):                     /* frame same as target         */
        orel->frame = orel->symbol;
        break;
    case( FRAME_LOC ):                      /* frame containing location    */
        /*
         * FRAME_LOC is already remap to FRAME_SEG of curr segment
         */
        assert( 0 );
    }

    return( addReloc( ofh, orel ) );
}


orl_return  OmfAddExtDef( omf_file_handle ofh, const char *extname, omf_string_len len, omf_rectyp typ )
{
    omf_symbol_handle   sym;
    orl_symbol_type     styp;
    orl_return          return_val;

    assert( ofh );
    assert( extname );

    if( ofh->extdefs == NULL ) {
        ofh->extdefs = newStringTable( ofh, OMF_SEC_IMPORT_INDEX );
        if( ofh->extdefs == NULL ) {
            return( ORL_OUT_OF_MEMORY );
        }
    }
    return_val = addString( ofh->extdefs, extname, len );
    if( return_val == ORL_OKAY ) {
        styp = ORL_SYM_TYPE_OBJECT;
        if( ( typ == CMD_COMDEF ) || ( typ == CMD_LCOMDEF ) ) {
            styp |= ORL_SYM_TYPE_COMMON;
        } else {
            styp |= ORL_SYM_TYPE_UNDEFINED;
        }
        return_val = ORL_OUT_OF_MEMORY;
        sym = newSymbol( ofh, styp, extname, len );
        if( sym != NULL ) {
            sym->idx = ofh->extdefs->assoc.string.num;
            sym->rec_typ = typ;
            switch( typ ) {
            case( CMD_COMDEF ):
                sym->flags |= OMF_SYM_FLAGS_COMDEF;
                sym->binding = ORL_SYM_BINDING_GLOBAL;
                break;
            case( CMD_LCOMDEF ):
                sym->flags |= OMF_SYM_FLAGS_COMDEF;
                /* fall through */
            case( CMD_LEXTDEF ):
            case( CMD_LEXTDEF32 ):
                sym->flags |= OMF_SYM_FLAGS_LOCAL;
                sym->binding = ORL_SYM_BINDING_LOCAL;
                break;
            }
            return_val = addToSymbolTable( ofh, sym );
        }
    }
    return( return_val );
}


orl_return OmfAddComDat( omf_file_handle ofh, bool is32, int flags, int attr, int align,
                        omf_sec_offset offset, omf_idx seg, omf_idx group, omf_frame frame,
                        omf_idx name, omf_bytes buffer, omf_rec_size len, omf_rectyp typ )
{
    orl_return          return_val;
    omf_sec_handle      sh;
    omf_symbol_handle   sym;
    orl_symbol_type     styp;
    omf_sec_offset      size;
    omf_string          comname;

    /* unused parameters */ (void)typ;

    assert( ofh );
    assert( buffer );

    return_val = finishPrevWork( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );

    if( align == -1 ) {
        if( seg ) {
            sh = findSegment( ofh, seg );
            if( sh == NULL )
                return( ORL_ERROR );
            align = sh->assoc.seg.alignment;
        } else {
            align = 0;  /* Use default for kinda-broken objects */
        }
    }

    if( flags & COMDAT_CONTINUE ) {
        sh = findComDatByName( ofh, name );
        if( sh == NULL ) {
            return( ORL_ERROR );
        }
    } else {
        /* Create new Comdat
         */
        sh = newComDatSection( ofh );
        if( sh == NULL )
            return( ORL_OUT_OF_MEMORY );

        sh->assoc.seg.name = name;
        sh->assoc.seg.comdat.frame = frame;
        sh->assoc.seg.alignment = align;

        if( is32 ) {
            sh->assoc.seg.seg_flags |= OMF_SEG_IS_32;
        }

        sh->flags |= ORL_SEC_FLAG_COMDAT | ORL_SEC_FLAG_READ_PERMISSION |
                     ORL_SEC_FLAG_INITIALIZED_DATA;
        sh->assoc.seg.combine = ORL_SEC_COMBINE_COMDAT;

        switch( attr & COMDAT_ALLOC_MASK ) {
        case( COMDAT_EXPLICIT ):        /* in given segment */
            sh->assoc.seg.comdat.assoc_seg = findSegment( ofh, seg );
            if( sh->assoc.seg.comdat.assoc_seg == NULL )
                return( ORL_ERROR );
            sh->flags |= sh->assoc.seg.comdat.assoc_seg->flags;
            sh->assoc.seg.combine |= ORL_SEC_COMBINE_COMDAT_ALLOC_EXPLIC;
            if( group ) {
                sh->assoc.seg.comdat.group = findGroup( ofh, group );
            }
            break;
        case( COMDAT_FAR_CODE ):        /* allocate CODE use16 segment */
            sh->flags |= ORL_SEC_FLAG_EXEC | ORL_SEC_FLAG_EXECUTE_PERMISSION;
            sh->assoc.seg.combine |= ORL_SEC_COMBINE_COMDAT_ALLOC_CODE16;
            break;
        case( COMDAT_CODE32 ):          /* allocate CODE use32 segment */
            sh->flags |= ORL_SEC_FLAG_EXEC | ORL_SEC_FLAG_EXECUTE_PERMISSION;
            sh->assoc.seg.combine |= ORL_SEC_COMBINE_COMDAT_ALLOC_CODE32;
            break;
        case( COMDAT_FAR_DATA ):        /* allocate DATA use16 segment */
            sh->flags |= ORL_SEC_FLAG_WRITE_PERMISSION;
            sh->assoc.seg.combine |= ORL_SEC_COMBINE_COMDAT_ALLOC_DATA16;
            break;
        case( COMDAT_DATA32 ):          /* allocate DATA use32 segment */
            sh->flags |= ORL_SEC_FLAG_WRITE_PERMISSION;
            sh->assoc.seg.combine |= ORL_SEC_COMBINE_COMDAT_ALLOC_DATA32;
            break;
        }

        /* Create symbol for section using its name, when looking up we will
         * need to match the indexes for proper matching
         */
        styp = ORL_SYM_TYPE_SECTION | ORL_SYM_TYPE_COMMON;
        switch( attr & COMDAT_MATCH_MASK ) {
        case( COMDAT_MATCH_NONE ):              /* don't match anyone */
            styp |= ORL_SYM_CDAT_NODUPLICATES;
            sh->assoc.seg.combine |= ORL_SEC_COMBINE_COMDAT_PICK_NONE;
            break;
        case( COMDAT_MATCH_ANY ):               /* pick any instance */
            styp |= ORL_SYM_CDAT_ANY;
            sh->assoc.seg.combine |= ORL_SEC_COMBINE_COMDAT_PICK_ANY;
            break;
        case( COMDAT_MATCH_SAME ):              /* must be same size */
            styp |= ORL_SYM_CDAT_SAME_SIZE;
            sh->assoc.seg.combine |= ORL_SEC_COMBINE_COMDAT_PICK_SAME;
            break;
        case( COMDAT_MATCH_EXACT ):             /* must be exact match */
            styp |= ORL_SYM_CDAT_EXACT;
            sh->assoc.seg.combine |= ORL_SEC_COMBINE_COMDAT_PICK_EXACT;
            break;
        }

        comname = OmfGetLName( ofh->lnames, name );
        if( comname == NULL )
            return( ORL_ERROR );
        sym = _NewSymbol( ofh, styp, comname );

        sym->seg = name;
        sym->idx = sh->assoc.seg.seg_id;
        sh->assoc.seg.sym = sym;
        sym->section = sh;
        if( flags & COMDAT_LOCAL ) {
            sym->binding = ORL_SYM_BINDING_LOCAL;
            sym->flags |= OMF_SYM_FLAGS_LOCAL;
        } else {
            sym->binding = ORL_SYM_BINDING_GLOBAL;
        }

        return_val = addToSymbolTable( ofh, sym );
        if( return_val != ORL_OKAY ) {
            return( return_val );
        }
    }

    size = offset + len;
    if( sh->size < size ) {
        sh->size = size;
    }

    if( flags & COMDAT_ITERATED ) {
        return_val = OmfAddLIData( ofh, is32, sh->assoc.seg.seg_id, offset, buffer, len, true );
    } else {
        return_val = OmfAddLEData( ofh, is32, sh->assoc.seg.seg_id, offset, buffer, len, true );
    }

    return( return_val );
}


extern orl_return OmfAddLineNum( omf_sec_handle sh, unsigned_16 line, unsigned_32 offset )
{
    sh->assoc.seg.lines = checkArraySize( sh->omf_file_hnd, (void *)sh->assoc.seg.lines, sh->assoc.seg.num_lines, STD_INC, ORL_STRUCT_SIZEOF( orl_linnum ) );
    if( sh->assoc.seg.lines == NULL )
        return( ORL_OUT_OF_MEMORY );

    ((ORL_STRUCT( orl_linnum ) *)sh->assoc.seg.lines)[sh->assoc.seg.num_lines].linnum = line;
    ((ORL_STRUCT( orl_linnum ) *)sh->assoc.seg.lines)[sh->assoc.seg.num_lines].off = offset;
    sh->assoc.seg.num_lines++;

    return( ORL_OKAY );
}


orl_return OmfAddSegDef( omf_file_handle ofh, bool is32, orl_sec_alignment align,
                        int combine, bool use32, bool max_size, bool abs_frame,
                        omf_frame frame, omf_sec_size size, omf_idx name, omf_idx class )
{
    omf_sec_handle      sh;
    omf_symbol_handle   sym;
    omf_string          segname;

    assert( ofh );

    sh = newSegSection( ofh, ORL_SEC_TYPE_PROG_BITS );
    if( sh == NULL )
        return( ORL_OUT_OF_MEMORY );

    sh->assoc.seg.name = name;
    sh->assoc.seg.class = class;
    sh->assoc.seg.alignment = align;
    sh->assoc.seg.combine = getCombine( combine );
    sh->assoc.seg.frame = frame;

    if( is32 ) {
        sh->assoc.seg.seg_flags |= OMF_SEG_IS_32;
    }

    if( abs_frame ) {
        sh->assoc.seg.seg_flags |= OMF_SEG_IS_ABS;
    }

    if( max_size ) {
        sh->assoc.seg.seg_flags |= OMF_SEG_IS_BIG;
        if( is32 ) {
            sh->size = 0xffffffff;
        } else {
            sh->size = 0x10000;
        }
    } else {
        sh->size = size;
    }

    sh->flags |= getSegSecFlags( ofh, name, class, align, combine, use32 );

    if( sh->flags & ORL_SEC_FLAG_UNINITIALIZED_DATA ) {
        sh->type = ORL_SEC_TYPE_NO_BITS;
    }

    /* Create symbol for section using its name, when looking up we will
     * need to match the indexes for proper matching
     */
    segname = OmfGetLName( ofh->lnames, name );
    if( segname == NULL )
        return( ORL_ERROR );
    sym = _NewSymbol( ofh, ORL_SYM_TYPE_SECTION, segname );

    sym->seg = name;
    sym->idx = sh->assoc.seg.seg_id;
    sh->assoc.seg.sym = sym;
    sym->section = sh;

    return( addToSymbolTable( ofh, sym ) );
}


orl_return OmfAddPubDef( omf_file_handle ofh, bool is32, omf_idx group, omf_idx seg, omf_frame frame,
                            const char *buffer, omf_string_len len, omf_sec_offset offset, omf_rectyp typ )
{
    omf_symbol_handle   sym;
    orl_symbol_type     styp;

    /* unused parameters */ (void)is32;

    assert( ofh );
    assert( buffer );

    if( seg == 0 && group != 0 ) {
        /* we currently don't handle this, presumably it does not occur.
         */
        return( ORL_ERROR );
    }
    styp = ORL_SYM_TYPE_OBJECT;
    if( seg == 0 && group == 0 ) {
        styp |= ORL_SYM_TYPE_ABSOLUTE;
    } else {
        styp |= ORL_SYM_TYPE_DEFINED;
    }
    sym = newSymbol( ofh, styp, buffer, len );
    if( sym == NULL )
        return( ORL_OUT_OF_MEMORY );

    sym->seg = seg;
    sym->frame = frame;
    sym->offset = offset;
    sym->rec_typ = typ;
    switch( typ ) {
    case( CMD_LPUBDEF ):
    case( CMD_LPUBDEF32 ):
        sym->flags |= OMF_SYM_FLAGS_LOCAL;
        sym->binding = ORL_SYM_BINDING_LOCAL;
        break;
    default:
        sym->binding = ORL_SYM_BINDING_GLOBAL;
        break;
    }

    if( styp & ORL_SYM_TYPE_DEFINED ) {
        sym->section = findSegment( ofh, seg );
    }

    return( addToSymbolTable( ofh, sym ) );
}


orl_return  OmfAddGrpDef( omf_file_handle ofh, omf_idx name, omf_idx *segs, unsigned num_segs )
{
    omf_symbol_handle   sym;
    omf_sec_handle      sh;
    omf_grp_handle      grp;
    omf_string          grpname;

    assert( ofh );
    assert( segs );

    if( name == 0 )
        return( ORL_ERROR );

    grp = newGroup( ofh );
    if( grp == NULL )
        return( ORL_OUT_OF_MEMORY );

    grp->num_segs = num_segs;
    grp->segs = segs;
    grp->name = name;

    while( num_segs > 0 ) {
        num_segs--;
        sh = findSegment( ofh, segs[num_segs] );
        if( sh == NULL )
            return( ORL_ERROR );
        sh->flags |= ORL_SEC_FLAG_GROUPED;
        sh->assoc.seg.group = grp;
    }

    grpname = OmfGetLName( ofh->lnames, name );
    if( grpname == NULL )
        return( ORL_ERROR );

    sym = _NewSymbol( ofh, ORL_SYM_TYPE_GROUP, grpname );
    if( sym == NULL )
        return( ORL_OUT_OF_MEMORY );

    sym->flags |= OMF_SYM_FLAGS_GRPDEF;
    sym->idx = grp->idx;
    grp->sym = sym;

    return( addToSymbolTable( ofh, sym ) );
}


orl_return      OmfModEnd( omf_file_handle ofh )
{
    orl_return          return_val;

    assert( ofh );

    return_val = finishPrevWork( ofh );
    return( return_val );
}


orl_return  OmfAddComment( omf_file_handle ofh, unsigned_8 class, unsigned_8 flags, omf_bytes buff, omf_rec_size len )
{
    omf_sec_handle      sh;
    omf_comment         comment;

    assert( ofh );
    assert( buff );

    sh = ofh->comments;
    if( sh == NULL ) {
        sh = newSection( ofh, OMF_SEC_COMMENT_INDEX, ORL_SEC_TYPE_NOTE );
        if( sh == NULL )
            return( ORL_OUT_OF_MEMORY );
        sh->flags |= ORL_SEC_FLAG_REMOVE;
        ofh->comments = sh;
    }

    sh->assoc.comment.comments = checkArraySize( ofh, sh->assoc.comment.comments, sh->assoc.comment.num, STD_INC, sizeof( omf_comment ) );
    if( sh->assoc.comment.comments == NULL )
        return( ORL_OUT_OF_MEMORY );

    comment = (omf_comment)_ClientAlloc( ofh, ORL_STRUCT_SIZEOF( omf_comment ) + len );
    if( comment == NULL )
        return( ORL_OUT_OF_MEMORY );
    memset( comment, 0, ORL_STRUCT_SIZEOF( omf_comment ) + len );

    comment->class = class;
    comment->flags = flags;
    comment->len = len;
    memcpy( comment->data, buff, len );
    comment->data[len] = 0;

    sh->assoc.comment.comments[sh->assoc.comment.num] = comment;
    sh->assoc.comment.num++;

    return( ORL_OKAY );
}


omf_string OmfGetLName( omf_sec_handle lnames, omf_idx idx )
{
    assert( lnames );
    assert( lnames->type == ORL_SEC_TYPE_STR_TABLE );

    if( idx == 0 ) {
        return( NULL );
    }

    return( getIdx2String( lnames, --idx ) );
}


omf_sec_handle OmfFindSegOrComdat( omf_file_handle ofh, omf_idx seg, omf_idx comdat_lname )
{
    omf_quantity        x;

    assert( ofh );

    if( seg ) {
        return( findSegment( ofh, seg ) );
    } else {
        assert( comdat_lname );

        for( x = 0; x < ofh->num_comdats; x++ ) {
            if( nameCmp(ofh, ofh->comdats[x]->assoc.seg.name, comdat_lname) == 0 ) {
                return( ofh->comdats[x] );
            }
        }
    }
    return( NULL );
}


orl_return      OmfExportSegmentContents( omf_sec_handle sh )
{
    assert( sh );

    if( sh->size > 0 ) {
        return( checkSegmentLength( sh, sh->size ) );
    }
    return( ORL_OKAY );
}


orl_return      OmfTheadr( omf_file_handle ofh )
{
    orl_return          return_val;
    omf_bytes           buffer;
    omf_string_len      len;

    return_val = finishPrevWork( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );
    buffer = ofh->parsebuf;
    len = *buffer++;
    if( len > ofh->parselen )
        return( ORL_ERROR );
    return( OmfAddFileName( ofh, (char *)buffer, len ) );
}
