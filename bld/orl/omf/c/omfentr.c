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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <assert.h>

#include "omfentr.h"
#include "omfload.h"
#include "omfmunge.h"
#include "omfflhn.h"
#include "omforl.h"
#include "omfdrctv.h"
#include "orlhash.h"


#define _IsSegType( t )     ( ( t == ORL_SEC_TYPE_PROG_BITS ) || ( t == ORL_SEC_TYPE_NO_BITS ) )

omf_handle OMFENTRY OmfInit( orl_funcs *funcs )
{
    omf_handle          oh;

    assert( funcs );

    oh = ORL_CLI_ALLOC( funcs, ORL_STRUCT_SIZEOF( omf_handle ) );
    if( oh != NULL ) {
        oh->funcs = funcs;
        oh->first_file_hnd = NULL;
    }
    return( oh );
}


orl_return OMFENTRY OmfFini( omf_handle oh )
{
    orl_return          return_val;

    assert( oh );

    while( oh->first_file_hnd != NULL ) {
        return_val = OmfRemoveFileLinks( oh->first_file_hnd );
        if( return_val != ORL_OKAY ) {
            return( return_val );
        }
    }
    ORL_PTR_FREE( oh, oh );
    return( ORL_OKAY );
}


orl_return OMFENTRY OmfFileInit( omf_handle oh, FILE *fp, omf_file_handle *pofh )
{
    omf_file_handle     ofh;
    orl_return          return_val;

    assert( oh );

    ofh = ORL_PTR_ALLOC( oh, ORL_STRUCT_SIZEOF( omf_file_handle ) );
    if( ofh == NULL )
        return( ORL_OUT_OF_MEMORY );

    memset( ofh, 0, ORL_STRUCT_SIZEOF( omf_file_handle ) );
    ofh->fp = fp;

    OmfAddFileLinks( oh, ofh );
    return_val = OmfLoadFileStructure( ofh );
    if( return_val != ORL_OKAY ) {
        OmfRemoveFileLinks( ofh );
        ofh = NULL;
    }
    *pofh = ofh;
    return( return_val );
}


orl_return OMFENTRY OmfFileFini( omf_file_handle ofh )
{
    assert( ofh );

    return( OmfRemoveFileLinks( ofh ) );
}


orl_return OMFENTRY OmfFileScan( omf_file_handle ofh, const char *desired, orl_sec_return_func return_func )
{
    orl_hash_data_entry                 data_entry;
    omf_sec_handle                      sh;
    omf_symbol_handle                   sym;
    orl_return                          return_val;
    orl_hash_key                        h_key;

    assert( ofh );
    assert( return_func );

    if( desired == NULL ) {
        /* global request */
        for( sh = ofh->first_sec; sh != NULL; sh = sh->next ) {
            // string tables are internal sections which are not seen by
            // the user code
            if( sh->type != ORL_SEC_TYPE_STR_TABLE ) {
                return_val = return_func( (orl_sec_handle)sh );
                if( return_val != ORL_OKAY ) {
                    return( return_val );
                }
            }
        }
    } else if( ofh->symbol_table != NULL ) {
        assert( ofh->symbol_table->assoc.sym.hash_tab );
        h_key.u.string = desired;
        for( data_entry = ORLHashTableQuery( ofh->symbol_table->assoc.sym.hash_tab, h_key ); data_entry != NULL; data_entry = data_entry->next ) {
            sym = (omf_symbol_handle)data_entry->data.u.sym_handle;
            if( ( sym->typ == ORL_SYM_TYPE_SECTION ) && (sym->flags & OMF_SYM_FLAGS_GRPDEF) == 0 ) {
                return_val = return_func( (orl_sec_handle)sym->section );
                if( return_val != ORL_OKAY ) {
                    return( return_val );
                }
            }
        }
    }
    return( ORL_OKAY );
}


orl_machine_type OMFENTRY OmfFileGetMachineType( omf_file_handle ofh )
{
    assert( ofh );

    return( ofh->machine_type );
}


orl_file_flags OMFENTRY OmfFileGetFlags( omf_file_handle ofh )
{
    assert( ofh );

    return( ofh->flags );
}


orl_file_type OMFENTRY OmfFileGetType( omf_file_handle ofh )
{
    assert( ofh );

    return( ofh->type );
}


orl_file_size OMFENTRY OmfFileGetSize( omf_file_handle ofh )
{
    assert( ofh );

    return( ofh->size );
}


omf_sec_handle OMFENTRY OmfFileGetSymbolTable( omf_file_handle ofh )
{
    assert( ofh );

    return( ofh->symbol_table );
}


const char * OMFENTRY OmfSecGetName( omf_sec_handle sh )
{
    assert( sh );

    if( _IsSegType( sh->type ) ) {
        assert( sh->assoc.seg.sym );
        return( sh->assoc.seg.sym->name.string );
    }
    return( NULL );
}


omf_sec_base OMFENTRY OmfSecGetBase( omf_sec_handle sh )
{
    /* unused parameters */ (void)sh;

    assert( sh );

    // Ask Jim
    return( 0 );
}


omf_sec_size OMFENTRY OmfSecGetSize( omf_sec_handle sh )
{
    assert( sh );

    return( sh->size );
}


orl_sec_type OMFENTRY OmfSecGetType( omf_sec_handle sh )
{
    assert( sh );

    return( sh->type );
}


orl_sec_flags OMFENTRY OmfSecGetFlags( omf_sec_handle sh )
{
    assert( sh );

    return( sh->flags );
}


orl_sec_alignment OMFENTRY OmfSecGetAlignment( omf_sec_handle sh )
{
    assert( sh );

    if( _IsSegType( sh->type ) ) {
        return( sh->assoc.seg.alignment );
    }
    return( 0 );
}


const char * OMFENTRY OmfSecGetClassName( omf_sec_handle sh )
{
    omf_string      class;

    assert( sh );

    if( _IsSegType( sh->type ) ) {
        class = OmfGetLName( sh->omf_file_hnd->lnames, sh->assoc.seg.class );
        if( class != NULL ) {
            return( class->string );
        }
    }
    return( NULL );
}


orl_sec_combine OMFENTRY OmfSecGetCombine( omf_sec_handle sh )
{
    assert( sh );

    if( _IsSegType( sh->type ) ) {
        return( sh->assoc.seg.combine );
    }
    return( ORL_SEC_COMBINE_NONE );
}


orl_sec_frame OMFENTRY OmfSecGetAbsFrame( omf_sec_handle sh )
{
    assert( sh );

    if( _IsSegType( sh->type ) && (sh->assoc.seg.seg_flags & OMF_SEG_IS_ABS) ) {
        return( sh->assoc.seg.frame );
    }
    return( ORL_SEC_NO_ABS_FRAME );
}


orl_sec_handle OMFENTRY OmfSecGetAssociated( omf_sec_handle sh )
{
    assert( sh );

    if( _IsSegType( sh->type ) && ( sh->flags & ORL_SEC_FLAG_COMDAT ) ) {
        return( (orl_sec_handle)(sh->assoc.seg.comdat.assoc_seg) );
    }
    return( ORL_NULL_HANDLE );
}


orl_group_handle OMFENTRY OmfSecGetGroup( omf_sec_handle sh )
{
    assert( sh );

    if( _IsSegType( sh->type ) ) {
        if( sh->flags & ORL_SEC_FLAG_COMDAT ) {
            return( (orl_group_handle)(sh->assoc.seg.comdat.group) );
        } else if( sh->flags & ORL_SEC_FLAG_GROUPED ) {
            return( (orl_group_handle)(sh->assoc.seg.group) );
        }
    }
    return( ORL_NULL_HANDLE );
}


omf_sec_handle OMFENTRY OmfSecGetStringTable( omf_sec_handle sh )
{
    assert( sh );
    assert( sh->omf_file_hnd );

    if( sh == sh->omf_file_hnd->lnames ) {
        return( sh );
    } else if( sh == sh->omf_file_hnd->extdefs ) {
        return( sh );
    }
    return( NULL );
}


omf_sec_handle OMFENTRY OmfSecGetSymbolTable( omf_sec_handle sh )
{
    assert( sh );
    assert( sh->omf_file_hnd );

    return( sh->omf_file_hnd->symbol_table );
}


omf_sec_handle OMFENTRY OmfSecGetRelocTable( omf_sec_handle sh )
{
    assert( sh );
    assert( sh->omf_file_hnd );

    if( sh->type == ORL_SEC_TYPE_PROG_BITS ) {
        return( sh->omf_file_hnd->relocs );
    }
    return( NULL );
}


orl_return OMFENTRY OmfSecGetContents( omf_sec_handle sh, unsigned_8 **buffer )
{
    orl_return  return_val;

    assert( sh );

    if( ( sh->contents != NULL ) || ( sh->type == ORL_SEC_TYPE_PROG_BITS ) ) {
        return_val = OmfExportSegmentContents( sh );
        if( return_val != ORL_OKAY )
            return( return_val );
        *buffer = sh->contents;
        return( ORL_OKAY );
    }
    return( ORL_ERROR );
}


static orl_return OMFENTRY relocScan( omf_sec_handle sh, omf_sec_offset sec_offset,
                                      orl_reloc_return_func return_func, bool check )
{
    unsigned_32         x;
    unsigned_32         num;
    orl_reloc           *relocs;
    orl_return          return_val;
    int                 global;
    omf_sec_handle      rsh;

    assert( sh );
    assert( sh->omf_file_hnd );
    assert( return_func );

    if( sh->omf_file_hnd->relocs == NULL )
        return( ORL_FALSE );
    relocs = sh->omf_file_hnd->relocs->assoc.reloc.relocs;
    num = sh->omf_file_hnd->relocs->assoc.reloc.num;
    if( num ) {
        assert( relocs );
    } else {
        return( ORL_FALSE );
    }
    global = ( sh->index == sh->omf_file_hnd->relocs->index );

    for( x = 0; x < num; x++ ) {
        rsh = (omf_sec_handle)(relocs[x]->section);
        if( global || ( sh->index == rsh->index ) ) {
            if( !check || ( relocs[x]->offset == sec_offset ) ) {
                return_val = return_func( relocs[x] );
                if( return_val != ORL_OKAY ) {
                    return( return_val );
                }
            }
        }
    }
    return( ORL_TRUE );
}


orl_return OMFENTRY OmfSecQueryReloc( omf_sec_handle sh, omf_sec_offset sec_offset, orl_reloc_return_func return_func )
{
    assert( sh );
    assert( return_func );

    if( sh->type != ORL_SEC_TYPE_PROG_BITS )
        return( ORL_ERROR );
    return( relocScan( sh, sec_offset, return_func, true ) );
}


orl_return OMFENTRY OmfSecScanReloc( omf_sec_handle sh, orl_reloc_return_func return_func )
{
    assert( sh );
    assert( return_func );

    if( sh->type != ORL_SEC_TYPE_PROG_BITS )
        return( ORL_ERROR );
    return( relocScan( sh, 0, return_func, false ) );
}

// ask jim for next 2 calls
// One of these should be implimented O(1), but the section handles
// get reordered in OmfLoad.
orl_table_index OMFENTRY OmfCvtSecHdlToIdx( omf_sec_handle sh )
{
    assert( sh );

    return( sh->index );
}


omf_sec_handle OMFENTRY OmfCvtIdxToSecHdl( omf_file_handle ofh, orl_table_index idx )
{
    omf_sec_handle      sh;

    assert( ofh );

    for( sh = ofh->first_sec; sh != NULL; sh = sh->next ) {
        if( sh->index == idx ) {
            break;
        }
    }
    return( sh );
}


orl_return OMFENTRY OmfRelocSecScan( omf_sec_handle sh, orl_reloc_return_func return_func )
{
    assert( sh );
    assert( return_func );

    if( sh->type != ORL_SEC_TYPE_RELOCS )
        return( ORL_ERROR );
    return( relocScan( sh, 0, return_func, false ) );
}


orl_return OMFENTRY OmfSymbolSecScan( omf_sec_handle sh, orl_symbol_return_func return_func )
{
    int                     x;
    orl_return              return_val;
    omf_symbol_handle       *syms;

    assert( sh );
    assert( return_func );

    if( sh->type != ORL_SEC_TYPE_SYM_TABLE )
        return( ORL_ERROR );
    syms = sh->assoc.sym.syms;
    if( syms == NULL )
        return( ORL_ERROR );

    for( x = 0; x < sh->assoc.sym.num; x++ ) {
        return_val = return_func( (orl_symbol_handle)syms[x] );
        if( return_val != ORL_OKAY ) {
            return( return_val );
        }
    }
    return( ORL_OKAY );
}


orl_table_index OMFENTRY OmfSecGetNumLines( omf_sec_handle sh )
{
    assert( sh );

    if( sh->type == ORL_SEC_TYPE_PROG_BITS ) {
        return( sh->assoc.seg.num_lines );
    } else {
        return( 0 );
    }
}


orl_linnum OMFENTRY OmfSecGetLines( omf_sec_handle sh )
{
    assert( sh );

    if( sh->type == ORL_SEC_TYPE_PROG_BITS ) {
        return( sh->assoc.seg.lines );
    } else {
        return( NULL );
    }
}


const char * OMFENTRY OmfSymbolGetName( omf_symbol_handle sym )
{
    assert( sym );

    return( sym->name.string );
}


omf_symbol_value OMFENTRY OmfSymbolGetValue( omf_symbol_handle sym )
{
    assert( sym );
    return( sym->offset );
}


orl_symbol_binding OMFENTRY OmfSymbolGetBinding( omf_symbol_handle sym )
{
    assert( sym );

    return( sym->binding );
}


orl_symbol_type OMFENTRY OmfSymbolGetType( omf_symbol_handle sym )
{
    assert( sym );

    return( sym->typ );
}


unsigned char OMFENTRY OmfSymbolGetRawInfo( omf_symbol_handle sym )
{
    /* unused parameters */ (void)sym;

    assert( sym );

    return( 0 );
}


omf_sec_handle OMFENTRY OmfSymbolGetSecHandle( omf_symbol_handle sym )
{
    assert( sym );

    return( sym->section );
}


orl_return OMFENTRY OmfNoteSecScan( omf_sec_handle hnd, orl_note_callbacks *cbs, void *cookie )
{
    assert( hnd );
    assert( cbs );

    if( hnd->type != ORL_SEC_TYPE_NOTE )
        return( ORL_ERROR );
    return( OmfParseComments( hnd, cbs, cookie ) );
}


orl_return OMFENTRY OmfGroupsScan( omf_file_handle hnd, orl_group_return_func return_func )
{
    orl_table_index     idx;
    orl_return          return_val;

    assert( hnd );
    assert( return_func );

    return_val = ORL_OKAY;
    for( idx = 0; idx < hnd->num_groups; idx++ ) {
        assert( hnd->groups );
        return_val = return_func( (orl_group_handle)hnd->groups[idx] );
        if( return_val != ORL_OKAY ) {
            break;
        }
    }

    return( return_val );
}


const char * OMFENTRY OmfGroupName( omf_grp_handle hnd )
{
    assert( hnd );
    assert( hnd->sym );

    return( hnd->sym->name.string );
}


orl_table_index OMFENTRY OmfGroupSize( omf_grp_handle hnd )
{
    assert( hnd );

    return( hnd->num_segs );
}


const char * OMFENTRY OmfGroupMember( omf_grp_handle hnd, orl_table_index idx )
{
    omf_sec_handle      sh;

    assert( hnd );

    if( ( hnd->num_segs > 0 ) && ( idx < hnd->num_segs ) ) {
        assert( hnd->segs );
        if( hnd->segs[idx] ) {
            sh = OmfFindSegOrComdat( hnd->omf_file_hnd, hnd->segs[idx], 0 );
            if( sh != NULL ) {
                assert( sh->assoc.seg.sym );
                return( sh->assoc.seg.sym->name.string );
            }
        }
    }
    return( NULL );
}
