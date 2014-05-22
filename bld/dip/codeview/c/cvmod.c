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
* Description:  CodeView DIP module management.
*
****************************************************************************/


#include "cvinfo.h"


struct find_mod {
    IMP_MOD_WKR         *wk;
    void                *d;
    imp_mod_handle      im;
};

static walk_result FindMods( imp_image_handle *ii,
                                cv_directory_entry *cde, void *d )
{
    struct find_mod     *md = d;

    if( cde->subsection != sstModule ) return( WR_CONTINUE );
    return( md->wk( ii, cde->iMod, md->d ) );
}

walk_result     DIGENTRY DIPImpWalkModList( imp_image_handle *ii,
                        IMP_MOD_WKR *wk, void *d )
{
    struct find_mod     find;
    walk_result         wr;

    find.wk = wk;
    find.d  = d;
    wr = WalkDirList( ii, &FindMods, &find );
    if( wr == WR_CONTINUE ) wr = wk( ii, IMH_GBL, d );
    return( wr );
}

//NYI: should be OS && location sensitive
#define IS_PATH_CHAR( c ) ((c)=='\\'||(c)=='/'||(c)==':')
#define EXT_CHAR        '.'

#define GBL_NAME "__global"

unsigned        DIGENTRY DIPImpModName( imp_image_handle *ii,
                        imp_mod_handle im, char *buff, unsigned max )
{
    cv_directory_entry  *cde;
    cv_sst_module       *mp;
    char                *name;
    char                *start;
    char                *end;
    unsigned            len;

    if( im == IMH_GBL ) {
        return( NameCopy( buff, GBL_NAME, max, sizeof( GBL_NAME ) - 1 ) );
    }
    cde = FindDirEntry( ii, im, sstModule );

    mp = VMBlock( ii, cde->lfo, cde->cb );
    if( mp == NULL ) return( 0 );
    name = (char *)&mp->SegInfo[ mp->cSeg ];
    len = *(unsigned_8 *)name;
    ++name;
    start = name;
    end = name + len;
    for( ;; ) {
        if( len == 0 ) break;
        if( IS_PATH_CHAR( *name ) ) {
            start = name + 1;
            end = name + len;
        }
        if( *name == EXT_CHAR ) end = name;
        ++name;
        --len;
    }
    return( NameCopy( buff, start, max, end - start ) );
}

cs_compile *GetCompInfo( imp_image_handle *ii, imp_mod_handle im )
{
    cv_directory_entry  *cde;
    virt_mem            vm;
    s_compile           *rec;
    long                left;

    cde = FindDirEntry( ii, im, sstAlignSym );
    if( cde == NULL ) return( NULL );
    vm = cde->lfo + sizeof( unsigned_32 );
    left = cde->cb - sizeof( unsigned_32 );
    for( ;; ) {
        if( left <= 0 ) return( NULL );
        rec = VMRecord( ii, vm );
        if( rec->common.code == S_COMPILE ) return( &rec->f );
        vm += rec->common.length + sizeof( rec->common.length );
        left -= rec->common.length + sizeof( rec->common.length );
    }
}

char            *DIGENTRY DIPImpModSrcLang( imp_image_handle *ii, imp_mod_handle im )
{
    cs_compile  *comp_info;

    comp_info = GetCompInfo( ii, im );
    if( comp_info != NULL ) {
        switch( comp_info->language ) {
        case LANG_C:
            return( "c" );
        case LANG_CPP:
            return( "cpp" );
        case LANG_FORTRAN:
            return( "fortran" );
        }
    }
    return( "c" );
}

dip_status      DIGENTRY DIPImpModInfo( imp_image_handle *ii,
                                imp_mod_handle im, handle_kind hk )
{
    static const unsigned DmndType[] = {
        0,
        0, //sstGlobalTypes,
        sstSrcModule,
        sstAlignSym };
    unsigned            type;
    cv_directory_entry  *cde;

    type = DmndType[hk];
    if( type == 0 ) return( DS_FAIL );
    cde = FindDirEntry( ii, im, type );
    if( cde == NULL ) return( DS_FAIL );
    if( cde->cb == 0 ) return( DS_FAIL );
    return( DS_OK );
}

static walk_result FindAddr( imp_image_handle *ii, cv_directory_entry *cde,
                                void *d )
{
    struct find_mod     *md = d;
    address             *a;
    address             code;
    cv_sst_module       *mp;
    cv_seginfo          *sp;
    int                 left;

    if( cde->subsection != sstModule ) return( WR_CONTINUE );
    a = md->d;
    mp = VMBlock( ii, cde->lfo, cde->cb );
    if( mp == NULL ) return( WR_CONTINUE );
    if( mp->ovlNumber != a->sect_id ) return( WR_CONTINUE );
    sp =  &mp->SegInfo[0];
    left = mp->cSeg;
    for( ;; ) {
        if( left <= 0 ) return( WR_CONTINUE );
        code.mach.segment = sp->Seg;
        code.mach.offset  = sp->offset;
        MapLogical( ii, &code );
        if( code.mach.segment == a->mach.segment &&
            code.mach.offset <= a->mach.offset &&
            (code.mach.offset+sp->cbSeg) > a->mach.offset ) {
            md->im = cde->iMod;
            return( WR_STOP );
        }
        ++sp;
        --left;
    }
}

search_result ImpAddrMod( imp_image_handle *ii, address a, imp_mod_handle *im )
{
    int                 left;
    seg_desc            *map;
    struct find_mod     d;

    map = &ii->mapping[0];
    left = ii->map_count;
    for( ;; ) {
        if( left <= 0 ) return( SR_NONE );
        if( map->ovl == a.sect_id &&
            map->frame == a.mach.segment &&
            map->offset <= a.mach.offset &&
            (map->offset+map->cbseg) > a.mach.offset ) break;
        ++map;
        --left;
    }
    /*
        We know the address is in the image. If it's an executable
        segment, we can find the module by checking all the sstModule
        sections and look at the code section information. If it's a
        data segment, or we can't find it, return IMH_GBL.
    */
    *im = IMH_GBL;
    if( map->u.b.fExecute ) {
        d.d = &a;
        if( WalkDirList( ii, FindAddr, &d ) == WR_STOP ) {
            *im = d.im;
        }
    }
    return( SR_CLOSEST );
}

search_result   DIGENTRY DIPImpAddrMod( imp_image_handle *ii, address a,
                        imp_mod_handle *im )
{
    return( ImpAddrMod( ii, a, im ) );
}

address         DIGENTRY DIPImpModAddr( imp_image_handle *ii,
                                imp_mod_handle im )
{
    cv_sst_module       *mp;
    cv_directory_entry  *cde;
    address             addr;

    cde = FindDirEntry( ii, im, sstModule );
    if( cde == NULL ) return( NilAddr );
    mp = VMBlock( ii, cde->lfo, cde->cb );
    if( mp == NULL ) return( NilAddr );
    if( mp->cSeg == 0 ) return( NilAddr );
    addr.mach.segment = mp->SegInfo[0].Seg;
    addr.mach.offset  = mp->SegInfo[0].offset;
    MapLogical( ii, &addr );
    return( addr );
}

dip_status      DIGENTRY DIPImpModDefault( imp_image_handle *ii,
                imp_mod_handle im, default_kind dk, dip_type_info *ti )
{
    cs_compile  *comp_info;

    comp_info = GetCompInfo( ii, im );
    if( comp_info == NULL ) return( DS_FAIL );
    ti->kind = TK_POINTER;
    ti->modifier = TM_NEAR;
    ti->size = comp_info->flags.f.Mode32 ? sizeof( addr48_off ) : sizeof( addr32_off );
    switch( dk ) {
    case DK_INT:
        ti->kind = TK_INTEGER;
        ti->modifier = TM_SIGNED;
        /* size is OK */
        break;
    case DK_DATA_PTR:
        if( comp_info->flags.f.AmbientData != AMBIENT_NEAR ) {
            ti->modifier = TM_FAR;
            ti->size += sizeof( addr_seg );
        }
        break;
    case DK_CODE_PTR:
        if( comp_info->flags.f.AmbientCode != AMBIENT_NEAR ) {
            ti->modifier = TM_FAR;
            ti->size += sizeof( addr_seg );
        }
        break;
    }
    return( DS_OK );
}
