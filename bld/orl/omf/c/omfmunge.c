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
    omf_string_struct   *s1;
    omf_string_struct   *s2;

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
    return( stricmp( s1->string, s2->string ) );
}


static void *checkArraySize( omf_file_handle ofh, void *old_arr, long num, long inc, long elem )
{
    long        size;
    void        *new_arr;

    assert( ofh );
    assert( num >= 0 );
    assert( inc > 0 );
    assert( elem > 0 );

    if( !( num % inc ) ) {
        size = ( num + inc ) * elem;
        new_arr = _ClientAlloc( ofh, size );
        if( !new_arr )
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


static const omf_symbol_handle_struct *findExtDefSym( omf_file_handle ofh, omf_idx ext )
{
    omf_sec_handle                  sh;
    omf_string_struct               *extname;
    orl_hash_data_struct            *hd;
    const omf_symbol_handle_struct  *sym;

    assert( ofh );

    if( !ofh->extdefs )
        return( NULL );
    sh = ofh->extdefs;
    if( !ext || ( ext > sh->assoc.string.num ) )
        return( NULL );
    assert( sh->assoc.string.strings );
    extname = sh->assoc.string.strings[ext - 1];
    if( extname == NULL )
        return( NULL );

    for( hd = ORLHashTableQuery( ofh->symbol_table->assoc.sym.hash_tab, extname->string ); hd != NULL; hd = hd->next ) {
        sym = hd->data;
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

    if( !grp || ( grp > ofh->num_groups ) )
        return( NULL );
    assert( ofh->groups );
    return( ofh->groups[grp - 1] );
}


static omf_sec_handle   findComDat( omf_file_handle ofh, omf_idx seg )
{
    assert( ofh );

    if( !seg || ( seg > ofh->num_comdats ) )
        return( NULL );
    assert( ofh->comdats );
    return( ofh->comdats[seg - 1] );
}

static omf_sec_handle   findComDatByName( omf_file_handle ofh, omf_idx nameidx )
{
    unsigned    index;

    assert( ofh );
    assert( ofh->comdats );

    if( !nameidx )
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

    if( !seg || ( seg > ofh->num_segs ) )
        return( NULL );
    assert( ofh->segs );
    return( ofh->segs[seg - 1] );
}


static omf_symbol_handle newSymbol( omf_file_handle ofh, orl_symbol_type typ, char *buffer, omf_string_len len )
{
    omf_symbol_handle   sym;

    assert( ofh );
    assert( buffer );

    sym = _ClientAlloc( ofh, sizeof( omf_symbol_handle_struct ) + len );
    if( sym != NULL ) {
        memset( sym, 0, sizeof( omf_symbol_handle_struct ) + len );
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

    sh = _ClientAlloc( ofh, sizeof( omf_sec_handle_struct ) );
    if( !sh )
        return( sh );
    memset( sh, 0, sizeof( omf_sec_handle_struct ) );

    sh->file_format = ORL_OMF;
    sh->omf_file_hnd = ofh;
    sh->type = typ;
    sh->index = idx;

    if( ofh->first_sec ) {
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
    if( !ofh->comdats )
        return( NULL );

    sh = newSection( ofh, OMF_SEC_NEXT_AVAILABLE, ORL_SEC_TYPE_PROG_BITS );
    if( !sh )
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
    if( !ofh->segs )
        return( NULL );

    sh = newSection( ofh, OMF_SEC_NEXT_AVAILABLE, typ );
    if( !sh )
        return( sh );

    ofh->segs[ofh->num_segs] = sh;
    ofh->num_segs++;
    sh->assoc.seg.seg_id = ofh->num_segs;

    return( sh );
}


static omf_grp_handle   newGroup( omf_file_handle ofh )
{
    omf_grp_handle      gr;

    assert( ofh );

    ofh->groups = checkArraySize( ofh, ofh->groups, ofh->num_groups, STD_INC, sizeof( omf_grp_handle ) );
    if( !ofh->groups )
        return( NULL );

    gr = _ClientAlloc( ofh, sizeof( omf_grp_handle_struct ) );
    if( !gr )
        return( gr );
    memset( gr, 0, sizeof( omf_grp_handle_struct ) );

    ofh->groups[ofh->num_groups] = gr;
    ofh->num_groups++;
    gr->id = ofh->num_groups;
    gr->file_format = ORL_OMF;
    gr->omf_file_hnd = ofh;

    return( gr );
}


static orl_sec_offset   getUWord( omf_bytes buffer, int wordsize )
{
    orl_sec_offset      result = 0;

    assert( buffer );

    switch( wordsize ) {
    case( 4 ):
        result |= buffer[3] << 24;
        result |= buffer[2] << 16;
    case( 2 ):
        result |= buffer[1] << 8;
    default:
        result |= buffer[0];
    }
    return( result );
}


static omf_string_struct *getIdx2String( omf_sec_handle sh, omf_idx idx )
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
    if( sh ) {
        sh->flags = ORL_SEC_FLAG_REMOVE;
    }
    return( sh );
}


static orl_return       addString( omf_sec_handle sh, char *buffer, omf_string_len len )
{
    omf_string_struct   *str;
    omf_file_handle     ofh;

    assert( sh );
    assert( buffer );

    ofh = sh->omf_file_hnd;

    /* Check if we need to allocate more string table
     */
    sh->assoc.string.strings = checkArraySize( ofh, sh->assoc.string.strings,
                                               sh->assoc.string.num, STD_INC,
                                               sizeof( omf_string_struct * ) );
    if( !sh->assoc.string.strings )
        return( ORL_OUT_OF_MEMORY );

    str = _ClientAlloc( ofh, sizeof( omf_string_struct ) + len );
    if( str == NULL )
        return( ORL_OUT_OF_MEMORY );

    memset( str, 0, sizeof( omf_string_struct ) + len );
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

    assert( ofh );
    assert( sym );

    sh = ofh->symbol_table;
    if( !sh ) {
        sh = newSection( ofh, OMF_SEC_SYM_TABLE_INDEX, ORL_SEC_TYPE_SYM_TABLE );
        if( !sh )
            return( ORL_OUT_OF_MEMORY );
        ofh->symbol_table = sh;
        sh->flags |= ORL_SEC_FLAG_REMOVE;
        sh->assoc.sym.hash_tab = ORLHashTableCreate( ofh->omf_hnd->funcs, 257, ORL_HASH_STRING, (orl_hash_comparison_func)stricmp );
        if( !sh->assoc.sym.hash_tab ) {
            return( ORL_OUT_OF_MEMORY );
        }
    }
    assert( sh->assoc.sym.hash_tab );

    sh->assoc.sym.syms = checkArraySize( ofh, sh->assoc.sym.syms,
                                         sh->assoc.sym.num, STD_INC,
                                         sizeof( omf_symbol_handle ) );
    if( !sh->assoc.sym.syms )
        return( ORL_OUT_OF_MEMORY );

    sh->assoc.sym.syms[sh->assoc.sym.num] = sym;
    sh->assoc.sym.num++;
    return( ORLHashTableInsert( sh->assoc.sym.hash_tab, sym->name.string, sym ) );
}


static orl_return       addReloc( omf_file_handle ofh, omf_reloc_handle orh )
{
    omf_sec_handle      sh;

    assert( ofh );
    assert( orh );

    if( !ofh->relocs ) {
        ofh->relocs = newSection( ofh, OMF_SEC_RELOC_INDEX, ORL_SEC_TYPE_RELOCS );
        if( !ofh->relocs )
            return( ORL_OUT_OF_MEMORY );
        ofh->relocs->flags |= ORL_SEC_FLAG_REMOVE;
    }

    sh = ofh->relocs;
    sh->assoc.reloc.relocs = checkArraySize( ofh, sh->assoc.reloc.relocs,
                                             sh->assoc.reloc.num, STD_INC,
                                             sizeof( omf_reloc_handle ) );
    if( !sh->assoc.reloc.relocs )
        return( ORL_OUT_OF_MEMORY );

    sh->assoc.reloc.relocs[sh->assoc.reloc.num] = orh;
    sh->assoc.reloc.num++;

    return( ORL_OKAY );
}


static omf_tmp_fixup    findMatchingFixup( omf_tmp_fixup tf, int lo, int hi )
{
    if( hi < lo ) {
        hi = lo;
    }

    while( tf ) {
        if( ( tf->offset >= lo ) && ( tf->offset <= hi ) )
            break;
        tf = tf->next;
    }

    return( tf );
}


static orl_return   writeAndFixupLIData( omf_file_handle ofh, omf_sec_handle sh, omf_bytes buffer )
{
    int                 wordsize;
    long                tmp;
    uint_32             repeat;
    long                block;
    int                 size;
    int                 used;
    int                 hi;
    int                 lo;
    omf_bytes           ptr;
    orl_return          err = ORL_OKAY;
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

    repeat = getUWord( ptr, wordsize );
    ptr += wordsize;
    block = getUWord( ptr, 2 );
    ptr += 2;

    lo = used;
    hi = lo + tmp;
    size -= tmp;
    used += tmp;

    if( findMatchingFixup( ofh->lidata->first_fixup, lo, hi ) ) {
        return( ORL_ERROR );
    } else if( block ) {
        while( repeat ) {
            repeat--;
            ofh->lidata->used = used;
            ofh->lidata->size = size;
            for( x = 0; x < block; x++ ) {
                err = writeAndFixupLIData( ofh, sh, buffer );
                if( err != ORL_OKAY ) {
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
            while( ftr ) {
                ntr = _ClientAlloc( ofh, sizeof( omf_tmp_fixup_struct ) );
                if( !ntr )
                    return( ORL_OUT_OF_MEMORY );
                memcpy( ntr, ftr, sizeof( omf_tmp_fixup_struct ) );

                /* determine new offset as if this was an LEData we were
                 * doing a fixup for
                 */
                ntr->offset = ofh->lidata->offset - used + ftr->offset;

                /* insert into new fixup queue
                 */
                if( ofh->lidata->new_fixup ) {
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
        err = ORL_OKAY;
    }

    return( err );
}


static orl_return       expandPrevLIData( omf_file_handle ofh )
{
    omf_sec_handle      sh;
    int                 size;
    omf_bytes           buffer;
    orl_return          err = ORL_OKAY;
    unsigned char       tmp[1024];
    omf_tmp_fixup       ftr;
    orl_sec_offset      offset;

    assert( ofh );
    assert( ofh->work_sec );
    assert( ofh->lidata );

    sh = ofh->work_sec;
    size = ofh->lidata->size;

    if( size > 1024 ) {
        buffer = _ClientAlloc( ofh, size );
        if( !buffer ) {
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
        err = writeAndFixupLIData( ofh, sh, buffer );
        if( err != ORL_OKAY ) {
            return( err );
        }
    }
    sh->assoc.seg.cur_offset = offset;

    if( size > 1024 ) {
        _ClientFree( ofh, buffer );
    }

    assert( !( ofh->status & OMF_STATUS_ADD_MASK ) );

    /* Destroy original fixups
     */
    while( ofh->lidata->first_fixup ) {
        ftr = ofh->lidata->first_fixup;
        ofh->lidata->first_fixup = ftr->next;
        _ClientFree( ofh, ftr );
    }

    /* Add the generated fixups
     */
    while( ofh->lidata->new_fixup ) {
        ftr = ofh->lidata->new_fixup;
        err = OmfAddFixupp( ofh, ftr->is32, ftr->mode, ftr->location,
                            ftr->offset, ftr->fmethod, ftr->fidx, ftr->tmethod,
                            ftr->tidx, ftr->disp );
        if( err != ORL_OKAY )
            break;
        ofh->lidata->new_fixup = ftr->next;
        _ClientFree( ofh, ftr );
    }

    ofh->lidata->last_fixup = NULL;

    return( err );
}


static orl_return       applyBakpats( omf_file_handle ofh )
{
    omf_sec_handle      sh;
    orl_return          err = ORL_OKAY;
    omf_tmp_bkfix       tbf;
    uint_8              *pfix8;
    uint_16             *pfix16;
    uint_32             *pfix32;

    assert( ofh );
    assert( ofh->bakpat );

    ofh->bakpat->last_fixup = NULL;

    /* Go through all the backpatches and update the segment data. The BAKPATs
     * aren't normal fixups and may modify all sorts of instructions.
     */
    while( ofh->bakpat->first_fixup ) {
        tbf = ofh->bakpat->first_fixup;
        ofh->bakpat->first_fixup = tbf->next;

        assert( tbf->segidx || tbf->symidx );
        if( tbf->segidx )
            sh = findSegment( ofh, tbf->segidx );
        else 
            sh = findComDatByName( ofh, tbf->symidx );
        if( !sh ) {
            err = ORL_ERROR;
            break;
        }
        switch( tbf->reltype ) {
        case ORL_RELOC_TYPE_WORD_8:
            pfix8 = sh->contents + tbf->offset;
            *pfix8 += tbf->disp;
            break;
        case ORL_RELOC_TYPE_WORD_16:
            pfix16 = (uint_16 *)(sh->contents + tbf->offset);
            *pfix16 += tbf->disp;
            break;
        case ORL_RELOC_TYPE_WORD_32:
            pfix32 = (uint_32 *)(sh->contents + tbf->offset);
            *pfix32 += tbf->disp;
            break;
        default:
            assert( 0 );
            err = ORL_ERROR;
            break;
        }
        if( err != ORL_OKAY )
            break;

        _ClientFree( ofh, tbf );
    }

    assert( !(ofh->status & OMF_STATUS_ADD_BAKPAT) );
    /* Free the entire bakpat structure as well. */
    _ClientFree( ofh, ofh->bakpat );
    ofh->bakpat = NULL;

    return( err );
}


static orl_return       finishPrevWork( omf_file_handle ofh )
{
    orl_return  err = ORL_OKAY;

    assert( ofh );

    if( ofh->status & OMF_STATUS_ADD_LIDATA ) {
        ofh->status &= ~OMF_STATUS_ADD_LIDATA;
        err = expandPrevLIData( ofh );
    }
    /* NB: We're assuming that a BAKPAT/NBKPAT record always follows
     * the LEDATA (or possibly LIDATA) record it modifies. This is
     * not guaranteed by the TIS OMF spec, but the backpatch records
     * make no sense otherwise.
     */
    if( ofh->status & OMF_STATUS_ADD_BAKPAT ) {
        ofh->status &= ~OMF_STATUS_ADD_BAKPAT;
        err = applyBakpats( ofh );
    }
    assert( !( ofh->status & OMF_STATUS_ADD_MASK ) );
    return( err );
}


static orl_sec_offset   calcLIDataLength( int is32, omf_bytes *input, omf_rec_size *len )
{
    omf_rec_size    size;
    omf_bytes       buffer;
    int             wordsize;
    long            tmp;
    uint_32         repeat;
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
    repeat = getUWord( buffer, wordsize );
    buffer += wordsize;
    block = getUWord( buffer, 2 );
    buffer += 2;
    size -= tmp;

    if( block ) {
        while( block ) {
            tmp = calcLIDataLength( is32, &buffer, &size );
            if( !tmp )
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


static orl_return       checkSegmentLength( omf_sec_handle sh, uint_32 max )
{
    omf_bytes   conts;

    assert( sh );

    if( max > sh->size )
        return( ORL_ERROR );
    if( max > sh->assoc.seg.cur_size ) {
        max = ( max / STD_CODE_SIZE ) + 1;
        max *= STD_CODE_SIZE;
        conts = _ClientAlloc( sh->omf_file_hnd, max );
        if( !conts )
            return( ORL_OUT_OF_MEMORY );
        memset( conts, 0, max );
        if( sh->contents ) {
            memcpy( conts, sh->contents, sh->assoc.seg.cur_size );
            _ClientFree( sh->omf_file_hnd, sh->contents );
        }
        sh->contents = conts;
        sh->assoc.seg.cur_size = max;
    }
    return( ORL_OKAY );
}


static int strNUpper( char *str, omf_string_struct *name )
{
    int     i;

    assert( str );

    for( i = 0; i < name->len; ++i ) {
        *str++ = toupper( name->string[i] );
    }
    *str = '\0';
    return( i );
}


static orl_sec_flags    getSegSecFlags( omf_file_handle ofh, omf_idx name,
                                        omf_idx class, orl_sec_alignment align,
                                        int combine, int use32 )
{
    char                lname[257];
    orl_sec_flags       flags = 0;
    omf_string_struct   *str;
    int                 slen;

    align = align; combine = combine;
    assert( ofh );

    str = OmfGetLName( ofh->lnames, class );
    if( str != NULL ) {
        slen = strNUpper( lname, str );
        if( ( slen > 3 ) && ( !strcmp( "CODE", &lname[slen - 4] ) || !strcmp( "TEXT", &lname[slen - 4] ) ) ) {
            flags |= ORL_SEC_FLAG_EXEC | ORL_SEC_FLAG_EXECUTE_PERMISSION | ORL_SEC_FLAG_READ_PERMISSION;
        } else if( ( slen > 3 ) && !strcmp( "DATA", &lname[slen - 4] ) ) {
            flags |= ORL_SEC_FLAG_READ_PERMISSION;
            str = OmfGetLName( ofh->lnames, name );
            if( str != NULL ) {
                slen = strNUpper( lname, str );
                if( strstr( "CONST", lname ) ) {
                    flags |= ORL_SEC_FLAG_INITIALIZED_DATA;
                }
            }
        } else if( ( slen > 4 ) && !strcmp( "CONST", &lname[slen - 5] ) ) {
            flags |= ORL_SEC_FLAG_READ_PERMISSION | ORL_SEC_FLAG_INITIALIZED_DATA;
        } else if( ( slen > 2 ) && !strcmp( "BSS", &lname[slen - 3] ) ) {
            flags |= ORL_SEC_FLAG_READ_PERMISSION | ORL_SEC_FLAG_WRITE_PERMISSION | ORL_SEC_FLAG_UNINITIALIZED_DATA;
        } else if( ( slen > 4 ) && !strcmp( "STACK", &lname[slen - 5] ) ) {
            flags |= ORL_SEC_FLAG_READ_PERMISSION | ORL_SEC_FLAG_WRITE_PERMISSION | ORL_SEC_FLAG_UNINITIALIZED_DATA;
        } else {
            flags |= ORL_SEC_FLAG_READ_PERMISSION;
            str = OmfGetLName( ofh->lnames, name );
            slen = strNUpper( lname, str );
            if( ( slen > 3 ) && ( !strcmp( "CODE", &lname[slen - 4] ) || !strcmp( "TEXT", &lname[slen - 4] ) ) ) {
                flags |= ORL_SEC_FLAG_EXEC | ORL_SEC_FLAG_EXECUTE_PERMISSION;
            }
        }
    } else {
        flags |= ORL_SEC_FLAG_READ_PERMISSION;
    }

    if( use32 ) {
        flags |= ORL_SEC_FLAG_USE_32;
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


orl_return              OmfAddLIData( omf_file_handle ofh, int is32,
                                      omf_idx seg, orl_sec_offset offset,
                                      omf_bytes buffer, omf_rec_size len, int comdat )
{
    omf_sec_handle      sh;
    orl_return          err;
    orl_sec_offset      size;
    orl_sec_offset      tmpsize;
    omf_bytes           tmp;
    omf_rec_size        tmplen;

    assert( ofh );
    assert( buffer );
    assert( seg );

    err = finishPrevWork( ofh );
    if( err != ORL_OKAY )
        return( err );

    if( comdat ) {
        sh = findComDat( ofh, seg );
    } else {
        sh = findSegment( ofh, seg );
    }
    if( !sh )
        return( ORL_ERROR );
    if( len < 0 )
        return( ORL_ERROR );

    tmp = buffer;
    tmplen = len;
    size = 0;

    while( tmplen > 0 ) {
        if( ofh->status & OMF_STATUS_EASY_OMF ) {
            tmpsize = calcLIDataLength( 0, &tmp, &tmplen );
        } else {
            tmpsize = calcLIDataLength( is32, &tmp, &tmplen );
        }
        if( tmpsize == 0 )
            return( ORL_ERROR );
        size += tmpsize;
    }

    err = checkSegmentLength( sh, offset + size );
    if( err != ORL_OKAY )
        return( err );

    /* we put off expanding the lidata until all the fixups are in
     */
    if( !ofh->lidata ) {
        ofh->lidata = _ClientAlloc( ofh, sizeof( omf_tmp_lidata_struct ) );
        if( !ofh->lidata ) {
            return( ORL_OUT_OF_MEMORY );
        }
    }

    memset( ofh->lidata, 0, sizeof( omf_tmp_lidata_struct ) );
    ofh->lidata->size = len;
    ofh->lidata->is32 = is32;

    sh->assoc.seg.cur_offset = offset;
    memcpy( sh->contents + offset, buffer, len );

    ofh->status |= OMF_STATUS_ADD_LIDATA;
    ofh->work_sec = sh;

    return( err );
}


orl_return              OmfAddLEData( omf_file_handle ofh, int is32,
                                      omf_idx seg, orl_sec_offset offset,
                                      omf_bytes buffer, omf_rec_size len, int comdat )
{
    omf_sec_handle      sh;
    orl_return          err;

    is32 = is32;
    assert( ofh );
    assert( buffer );
    assert( seg );

    err = finishPrevWork( ofh );
    if( err != ORL_OKAY )
        return( err );

    if( comdat ) {
        sh = findComDat( ofh, seg );
    } else {
        sh = findSegment( ofh, seg );
    }
    if( !sh )
        return( ORL_ERROR );
    if( len < 0 )
        return( ORL_ERROR );

    err = checkSegmentLength( sh, offset + len );
    if( err != ORL_OKAY )
        return( err );

    sh->assoc.seg.cur_offset = offset;
    memcpy( sh->contents + offset, buffer, len );
    ofh->work_sec = sh;

    return( err );
}


orl_return  OmfAddLName( omf_file_handle ofh, char *buffer, omf_string_len len, omf_rectyp typ )
{
    assert( ofh );
    assert( buffer );

    typ = typ;
    if( !ofh->lnames ) {
        ofh->lnames = newStringTable( ofh, OMF_SEC_LNAME_INDEX );
        if( !ofh->lnames )
            return( ORL_OUT_OF_MEMORY );
        ofh->lnames->flags = ORL_SEC_FLAG_REMOVE;
    }

    return( addString( ofh->lnames, buffer, len ) );
}


orl_return  OmfAddExtName( omf_file_handle ofh, char *buffer, omf_string_len len, omf_rectyp typ )
{
    assert( ofh );
    assert( buffer );

    typ = typ;
    if( !ofh->extdefs ) {
        ofh->extdefs = newStringTable( ofh, OMF_SEC_IMPORT_INDEX );
        if( !ofh->extdefs )
            return( ORL_OUT_OF_MEMORY );
        ofh->extdefs->flags = ORL_SEC_FLAG_REMOVE;
    }

    return( addString( ofh->extdefs, buffer, len ) );
}


omf_string_struct   *OmfGetLastExtName( omf_file_handle ofh )
{
    omf_sec_handle sh = ofh->extdefs;

    assert( ofh );

    return( sh->assoc.string.strings[sh->assoc.string.num - 1] );
}


orl_return              OmfAddBakpat( omf_file_handle ofh, uint_8 loctype,
                                      orl_sec_offset location, omf_idx segidx,
                                      omf_idx symidx, orl_sec_offset disp )
{
    omf_tmp_bkfix       tbf;
    orl_reloc_type      reltype;

    assert( ofh );

    /* The BAKPAT records must be applied at the end. One of segidx/symidx
     * must be set to distinguish between BAKPAT and NBKPAT.
     */
    if( !ofh->bakpat ) {
        ofh->bakpat = _ClientAlloc( ofh, sizeof( omf_tmp_bakpat_struct ) );
        if( !ofh->bakpat ) 
            return( ORL_OUT_OF_MEMORY );
        memset( ofh->bakpat, 0, sizeof( omf_tmp_bakpat_struct ) );
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

    tbf = _ClientAlloc( ofh, sizeof( omf_tmp_bkfix_struct ) );
    if( !tbf )
        return( ORL_OUT_OF_MEMORY );
    memset( tbf, 0, sizeof( omf_tmp_bkfix_struct ) );

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


orl_return              OmfAddFixupp( omf_file_handle ofh, int is32, int mode,
                                      int location, orl_sec_offset offset,
                                      int fmethod, omf_idx fidx, int tmethod,
                                      omf_idx tidx, orl_sec_offset disp )
{
    omf_tmp_fixup       tfr;
    omf_reloc_handle    orel;
    omf_sec_handle      sh;
    omf_grp_handle      gr;

    assert( ofh );

    if( ofh->status & OMF_STATUS_ADD_LIDATA ) {
        assert( ofh->work_sec );
        assert( ofh->lidata );

        tfr = _ClientAlloc( ofh, sizeof( omf_tmp_fixup_struct ) );
        if( !tfr )
            return( ORL_OUT_OF_MEMORY );
        memset( tfr, 0, sizeof( omf_tmp_fixup_struct ) );

        if( fmethod == FRAME_LOC ) {
            fmethod = FRAME_SEG;
            fidx = ofh->work_sec->assoc.seg.seg_id;
        }

        tfr->is32 = is32;
        tfr->mode = mode;
        tfr->location = location;
        tfr->offset = offset;
        tfr->fmethod = fmethod;
        tfr->fidx = fidx;
        tfr->tmethod = tmethod;
        tfr->tidx = tidx;
        tfr->disp = disp;

        if( ofh->lidata->last_fixup ) {
            ofh->lidata->last_fixup->next = tfr;
        } else {
            ofh->lidata->first_fixup = tfr;
        }
        ofh->lidata->last_fixup = tfr;
        return( ORL_OKAY );
    }

    orel = _ClientAlloc( ofh, sizeof( omf_reloc_handle_struct ) );
    if( !orel )
        return( ORL_OUT_OF_MEMORY );
    memset( orel, 0, sizeof( omf_reloc_handle_struct ) );

    switch( location ) {
    case( LOC_OFFSET_LO ):              /* relocate lo byte of offset   */
        /* should be 8 rather then 16, fix later
         */
        if( mode ) {
            orel->type = ORL_RELOC_TYPE_WORD_8;
        } else {
            orel->type = ORL_RELOC_TYPE_REL_8;
        }
        break;
    case( LOC_OFFSET ):                 /* relocate offset              */
        if( mode ) {
            orel->type = ORL_RELOC_TYPE_WORD_16;
        } else {
            orel->type = ORL_RELOC_TYPE_REL_16;
        }
        break;
    case( LOC_BASE ):                   /* relocate segment             */
            orel->type = ORL_RELOC_TYPE_SEGMENT;
        break;
    case( LOC_BASE_OFFSET ):            /* relocate segment and offset  */
        if( mode ) {
            orel->type = ORL_RELOC_TYPE_WORD_16_SEG;
        } else {
            orel->type = ORL_RELOC_TYPE_REL_16_SEG;
        }
        break;
    case( LOC_OFFSET_HI ):              /* relocate hi byte of offset   */
        if( mode ) {
            orel->type = ORL_RELOC_TYPE_WORD_HI_8;
        } else {
            orel->type = ORL_RELOC_TYPE_REL_HI_8;
        }
        break;
    case( LOC_OFFSET_32 ):              /* relocate 32-bit offset       */
    case( LOC_MS_OFFSET_32 ):           /* MS 32-bit offset             */
    case( LOC_MS_LINK_OFFSET_32 ):      /* like OFFSET_32, ldr resolved */
        if( mode ) {
            orel->type = ORL_RELOC_TYPE_WORD_32;
        } else {
            orel->type = ORL_RELOC_TYPE_REL_32;
        }
        break;
    case( LOC_BASE_OFFSET_32 ):         /* relocate seg and 32bit offset*/
    case( LOC_MS_BASE_OFFSET_32 ):      /* MS 48-bit pointer            */
        if( mode ) {
            orel->type = ORL_RELOC_TYPE_WORD_32_SEG;
        } else {
            orel->type = ORL_RELOC_TYPE_REL_32_SEG;
        }
        break;
    default:
        return( ORL_ERROR );
    }

    /* no section for fixups to refer to
     */
    if( !ofh->work_sec )
        return( ORL_ERROR );

    orel->offset = offset + ofh->work_sec->assoc.seg.cur_offset;
    orel->section = (orl_sec_handle)(ofh->work_sec);
    orel->addend = disp;

    if( fmethod == FRAME_LOC ) {
        fmethod = FRAME_SEG;
        fidx = ofh->work_sec->assoc.seg.seg_id;
    }

    switch( tmethod ) {
    case( TARGET_SEGWD ):            /* segment index with displacement      */
    case( TARGET_SEG ):              /* segment index, no displacement       */
        sh = findSegment( ofh, tidx );
        if( !sh )
            return( ORL_ERROR );
        orel->symbol = (orl_symbol_handle)(sh->assoc.seg.sym);
        break;
    case( TARGET_GRPWD ):            /* group index with displacement        */
    case( TARGET_GRP ):              /* group index, no displacement         */
        gr = findGroup( ofh, tidx );
        if( !gr )
            return( ORL_ERROR );
        orel->symbol = (orl_symbol_handle)(gr->sym);
        break;
    case( TARGET_EXTWD ):            /* external index with displacement     */
    case( TARGET_EXT ):              /* external index, no displacement      */
        orel->symbol = (orl_symbol_handle)(findExtDefSym( ofh, tidx ));
        if( !orel->symbol )
            return( ORL_ERROR );
        break;
    case( TARGET_ABSWD ):            /* abs frame num with displacement      */
    case( TARGET_ABS ):              /* abs frame num, no displacement       */
        break;
    default:
        return( ORL_ERROR );
    }

    switch( fmethod ) {
    case( FRAME_SEG ):                      /* segment index                */
        sh = findSegment( ofh, fidx );
        if( !sh )
            return( ORL_ERROR );
        orel->frame = (orl_symbol_handle)(sh->assoc.seg.sym);
        break;
    case( FRAME_GRP ):                      /* group index                  */
        gr = findGroup( ofh, fidx );
        if( !gr )
            return( ORL_ERROR );
        orel->frame = (orl_symbol_handle)(gr->sym);
        break;
    case( FRAME_EXT ):                      /* external index               */
        orel->frame = (orl_symbol_handle)(findExtDefSym( ofh, fidx ));
        if( !orel->frame )
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
        assert( 0 );
    }

    return( addReloc( ofh, orel ) );
}


orl_return  OmfAddExtDef( omf_file_handle ofh, omf_string_struct *extname, omf_rectyp typ )
{
    omf_symbol_handle   sym;
    orl_symbol_type     styp;

    assert( ofh );
    assert( extname );

    typ = typ;
    styp = ORL_SYM_TYPE_OBJECT;
    if( ( typ == CMD_COMDEF ) || ( typ == CMD_LCOMDEF ) ) {
        styp |= ORL_SYM_TYPE_COMMON;
    } else {
        styp |= ORL_SYM_TYPE_UNDEFINED;
    }
    sym = _NewSymbol( ofh, styp, extname );
    if( !sym )
        return( ORL_OUT_OF_MEMORY );

    sym->idx = ofh->extdefs->assoc.string.num + 1;
    sym->rec_typ = typ;
    switch( typ ) {
    case( CMD_COMDEF ):
    case( CMD_LCOMDEF ):
        sym->flags |= OMF_SYM_FLAGS_COMDEF;
        sym->binding = ORL_SYM_BINDING_GLOBAL;
        if( typ == CMD_COMDEF ) {
            break;
        }
    case( CMD_LEXTDEF ):
    case( CMD_LEXTDEF32 ):
        sym->flags |= OMF_SYM_FLAGS_LOCAL;
        sym->binding = ORL_SYM_BINDING_LOCAL;
    }

    return( addToSymbolTable( ofh, sym ) );
}


orl_return              OmfAddComDat( omf_file_handle ofh, int is32, int flags,
                                      int attr, int align,
                                      orl_sec_offset offset, omf_idx seg,
                                      omf_idx group, omf_frame frame,
                                      omf_idx name, omf_bytes buffer, omf_rec_size len,
                                      omf_rectyp typ )
{
    orl_return          err;
    omf_sec_handle      sh;
    omf_symbol_handle   sym;
    orl_symbol_type     styp;
    long                size;
    omf_string_struct   *comname;

    typ = typ;
    assert( ofh );
    assert( buffer );

    err = finishPrevWork( ofh );
    if( err != ORL_OKAY )
        return( err );

    if( align == -1 ) {
        if( seg ) {
            sh = findSegment( ofh, seg );
            if( !sh )
                return( ORL_ERROR );
            align = sh->assoc.seg.alignment;
        } else {
            align = 0;  /* Use default for kinda-broken objects */
        }
    }

    if( flags & COMDAT_CONTINUE ) {
        sh = findComDatByName( ofh, name );
        if( !sh ) {
            return( ORL_ERROR );
        }
    } else {
        /* Create new Comdat
         */
        sh = newComDatSection( ofh );
        if( !sh )
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
            if( !sh->assoc.seg.comdat.assoc_seg )
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

        err = addToSymbolTable( ofh, sym );
        if( err != ORL_OKAY ) {
            return( err );
        }
    }

    size = offset + len;
    if( size > sh->size ) {
        sh->size = size;
    }

    if( flags & COMDAT_ITERATED ) {
        err = OmfAddLIData( ofh, is32, sh->assoc.seg.seg_id, offset, buffer, len, 1 );
    } else {
        err = OmfAddLEData( ofh, is32, sh->assoc.seg.seg_id, offset, buffer, len, 1 );
    }

    return( err );
}


extern orl_return       OmfAddLineNum( omf_sec_handle sh, unsigned_16 line,
                                       unsigned_32 offset )
{
    sh->assoc.seg.lines = checkArraySize( sh->omf_file_hnd, sh->assoc.seg.lines,
                                          sh->assoc.seg.num_lines, STD_INC,
                                          sizeof( orl_linnum ) );
    if( sh->assoc.seg.lines == NULL )
        return( ORL_OUT_OF_MEMORY );

    sh->assoc.seg.lines[ sh->assoc.seg.num_lines ].linnum = line;
    sh->assoc.seg.lines[ sh->assoc.seg.num_lines ].off = offset;
    sh->assoc.seg.num_lines++;

    return( ORL_OKAY );
}


orl_return              OmfAddSegDef( omf_file_handle ofh, int is32,
                                      orl_sec_alignment align, int combine,
                                      int use32, int max, orl_sec_frame frame,
                                      orl_sec_size size, omf_idx name,
                                      omf_idx class )
{
    omf_sec_handle      sh;
    omf_symbol_handle   sym;
    omf_string_struct   *segname;

    assert( ofh );

    sh = newSegSection( ofh, ORL_SEC_TYPE_PROG_BITS );
    if( !sh )
        return( ORL_OUT_OF_MEMORY );

    sh->assoc.seg.name = name;
    sh->assoc.seg.class = class;
    sh->assoc.seg.alignment = align;
    sh->assoc.seg.combine = getCombine( combine );
    sh->assoc.seg.frame = frame;

    if( is32 ) {
        sh->assoc.seg.seg_flags |= OMF_SEG_IS_32;
    }

    if( frame > ORL_SEC_NO_ABS_FRAME ) {
        sh->assoc.seg.seg_flags |= OMF_SEG_IS_ABS;
    }

    if( max ) {
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


orl_return              OmfAddPubDef( omf_file_handle ofh, int is32,
                                      omf_idx group, omf_idx seg,
                                      omf_frame frame, char *buffer, omf_string_len len,
                                      orl_sec_offset offset, omf_rectyp typ )
{
    omf_symbol_handle   sym;
    orl_symbol_type     styp;

    is32 = is32;
    assert( ofh );
    assert( buffer );

    if( !seg && group ) {
        /* we currently don't handle this, presumably it does not occur.
         */
        return( ORL_ERROR );
    } else if( !seg && !group ) {
        styp = ORL_SYM_TYPE_ABSOLUTE;
    } else {
        styp = ORL_SYM_TYPE_DEFINED;
    }
    styp |= ORL_SYM_TYPE_OBJECT;

    sym = newSymbol( ofh, styp, buffer, len );
    if( !sym )
        return( ORL_OUT_OF_MEMORY );

    sym->seg = seg;
    sym->frame = frame;
    sym->offset = offset;
    sym->rec_typ = typ;
    if( ( typ == CMD_LPUBDEF ) || ( typ == CMD_LPUBDEF32 ) ) {
        sym->flags |= OMF_SYM_FLAGS_LOCAL;
        sym->binding = ORL_SYM_BINDING_LOCAL;
    } else {
        sym->binding = ORL_SYM_BINDING_GLOBAL;
    }

    if( styp & ORL_SYM_TYPE_DEFINED ) {
        sym->section = findSegment( ofh, seg );
    }

    return( addToSymbolTable( ofh, sym ) );
}


orl_return  OmfAddGrpDef( omf_file_handle ofh, omf_idx name, omf_idx *segs, int size )
{
    omf_symbol_handle   sym;
    omf_sec_handle      sh;
    omf_grp_handle      gr;
    omf_string_struct   *grpname;

    assert( ofh );
    assert( segs );

    if( !name )
        return( ORL_ERROR );

    gr = newGroup( ofh );
    if( !gr )
        return( ORL_OUT_OF_MEMORY );

    gr->segs = segs;
    gr->name = name;
    gr->size = size;

    while( size ) {
        size--;
        sh = findSegment( ofh, segs[size] );
        if( !sh )
            return( ORL_ERROR );
        sh->flags |= ORL_SEC_FLAG_GROUPED;
        sh->assoc.seg.group = gr;
    }

    grpname = OmfGetLName( ofh->lnames, name );
    if( grpname == NULL )
        return( ORL_ERROR );

    sym = _NewSymbol( ofh, ORL_SYM_TYPE_GROUP, grpname );
    if( !sym )
        return( ORL_OUT_OF_MEMORY );

    sym->flags |= OMF_SYM_FLAGS_GRPDEF;
    sym->idx = gr->id;
    gr->sym = sym;

    return( addToSymbolTable( ofh, sym ) );
}


orl_return      OmfModEnd( omf_file_handle ofh )
{
    orl_return          err;

    assert( ofh );

    err = finishPrevWork( ofh );
    return( err );
}


orl_return  OmfAddComment( omf_file_handle ofh, uint_8 class, uint_8 flags, omf_bytes buff, omf_rec_size len )
{
    omf_sec_handle      sh;
    omf_comment_struct  *comment;

    assert( ofh );
    assert( buff );

    sh = ofh->comments;
    if( !sh ) {
        sh = newSection( ofh, OMF_SEC_COMMENT_INDEX, ORL_SEC_TYPE_NOTE );
        if( !sh )
            return( ORL_OUT_OF_MEMORY );
        sh->flags |= ORL_SEC_FLAG_REMOVE;
        ofh->comments = sh;
    }

    sh->assoc.comment.comments = checkArraySize(ofh, sh->assoc.comment.comments,
                                                sh->assoc.comment.num, STD_INC,
                                                sizeof( omf_comment_struct * ) );
    if( !sh->assoc.comment.comments )
        return( ORL_OUT_OF_MEMORY );

    comment = _ClientAlloc( ofh, sizeof( omf_comment_struct ) + len );
    if( !comment )
        return( ORL_OUT_OF_MEMORY );
    memset( comment, 0, sizeof( omf_comment_struct ) + len );

    comment->class = class;
    comment->flags = flags;
    comment->len = len;
    memcpy( comment->data, buff, len );
    comment->data[len] = 0;

    sh->assoc.comment.comments[sh->assoc.comment.num] = comment;
    sh->assoc.comment.num++;

    return( ORL_OKAY );
}


omf_string_struct *OmfGetLName( omf_sec_handle lnames, omf_idx idx )
{
    assert( lnames );
    assert( lnames->type == ORL_SEC_TYPE_STR_TABLE );

    if( idx == 0 ) {
        return( NULL );
    }

    return( getIdx2String( lnames, --idx ) );
}


omf_sec_handle          OmfFindSegOrComdat( omf_file_handle ofh, omf_idx seg,
                                            omf_idx comdat_lname )
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
    orl_return          err;
    omf_bytes           buffer;
    omf_string_len      len;

    err = finishPrevWork( ofh );
    if( err != ORL_OKAY )
        return( err );
    buffer = ofh->parsebuf;
    len = *buffer++;
    if( len > ofh->parselen )
        return( ORL_ERROR );
    return( OmfAddFileName( ofh, (char *)buffer, len ) );
}
