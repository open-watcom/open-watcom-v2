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
* Description:  Stuff dealing with module handles.
*
****************************************************************************/


#include "dfdip.h"
#include "dfmod.h"
#include "dfmodinf.h"
#include "dfmodbld.h"
#include "dfld.h"
#include "dfaddr.h"
#include "dfaddsym.h"

//NYI: should be OS && location sensitive
#define IS_PATH_CHAR( c ) ((c)=='\\'||(c)=='/'||(c)==':')
#define EXT_CHAR        '.'
#define MAX_PATH        256

static void GetModName( const char *path, char *buff )
/****************************************************/
/* find the module name from the path         */
/**********************************************/
{
    const char  *start;
    const char  *end;
    bool        ext_found;

    start = end = path;
    ext_found = false;
    while( *path != '\0' ) {
        if( IS_PATH_CHAR( *path ) ) {
            start = path + 1;
        } else if( *path == EXT_CHAR ) {
            ext_found = true;
            end = path;
        } else if( *path == '(' ) {
            start = path + 1;
            while( *path != ')' )
                ++path;
            end = path;
            ext_found = true;
            goto do_copy;
        }
        ++path;
    }
    if( !ext_found ) {
        end = path;
    }
do_copy:
    while( start < end ) {
        *buff++ = *start++;
    }
    *buff = '\0';
}

static bool ModFill( void *_mod, drmem_hdl mod_handle )
/*****************************************************/
// fill in mod_handle for dip to dwarf mod map
// pick up general info about mod while here for later calls
{
    mod_list    *mod = _mod;
    char        fname[MAX_PATH];
    char        *name;
    char        *path;
    drmem_hdl   cu_tag;
    dr_model    model;
    mod_info    *modinfo;

    modinfo = NextModInfo( mod );
    modinfo->mod_handle = mod_handle;
    InitAddrSym( modinfo->addr_sym );
    modinfo->addr_size = DRGetAddrSize( mod_handle );
    cu_tag = DRGetCompileUnitTag( mod_handle );
    modinfo->cu_tag = cu_tag;
    modinfo->stmts = DRGetStmtList( cu_tag );
    path = DRGetName( cu_tag );
    if( path != NULL ) {
        GetModName( path, fname );
        DCFree( path );
        name = DCAlloc( strlen( fname ) + 1 );
        strcpy( name, fname );
    } else {
        name = NULL;
    }
    path = DRGetProducer( cu_tag );
    if( path != NULL ) {
        df_ver wat_producer_ver;

        if( memcmp( path, DWARF_WATCOM_PRODUCER_V3, sizeof( DWARF_WATCOM_PRODUCER_V3 ) - 1 ) == 0 ) {
            wat_producer_ver = VER_V3;
        } else if( memcmp( path, DWARF_WATCOM_PRODUCER_V2, sizeof( DWARF_WATCOM_PRODUCER_V2 ) - 1 ) == 0 ) {
            wat_producer_ver = VER_V2;
        } else if( memcmp( path, DWARF_WATCOM_PRODUCER_V1, sizeof( DWARF_WATCOM_PRODUCER_V1 ) - 1 ) == 0 ) {
            wat_producer_ver = VER_V1;
        } else {
            wat_producer_ver = VER_NONE;
        }
        if( mod->wat_producer_ver == VER_NONE ) {
            mod->wat_producer_ver = wat_producer_ver;
        } else if( mod->wat_producer_ver != wat_producer_ver ) {
            mod->wat_producer_ver = VER_ERROR;
        }
        DCFree( path );
    }
    modinfo->name = name;
    model = DRGetMemModelAT( cu_tag );
    if( DCCurrArch() == DIG_ARCH_X86 ) {
        switch( model ) {
        case DR_MODEL_NONE:
        case DR_MODEL_FLAT:
            modinfo->is_segment = false;
            break;
        default:
            modinfo->is_segment = true;
            break;
        }
    } else {
        modinfo->is_segment = false;
    }
    modinfo->model = model;
    modinfo->lang = DRGetLanguageAT( cu_tag );
    modinfo->dbg_pch = DRDebugPCHDef( cu_tag );
    modinfo->has_pubnames = false;
    return( true );
}

dip_status     InitModMap( imp_image_handle *iih )
/************************************************/
// Make the imp_mod_handle  to  drmem_hdl map
{
    mod_list    list;
    dip_status  ds;

    ds = DS_OK;
    InitModList( &list );
    InitAddrSym( iih->addr_sym );
    DRSetDebug( iih->dwarf->handle ); /* set dwarf to image */
    DRIterateCompileUnits( &list, ModFill );
    DRDbgClear( iih->dwarf->handle ); /* clear some mem */
    iih->mod_count = list.count;
    iih->mod_map = FiniModInfo( &list );
    if( list.wat_producer_ver == VER_V3 ) {
        DRDbgWatProducerVer( iih->dwarf->handle, VER_V3 );
    } else if( list.wat_producer_ver == VER_V2 ) {
        DRDbgWatProducerVer( iih->dwarf->handle, VER_V2 );
    } else if( list.wat_producer_ver == VER_V1 ) {
        DRDbgWatProducerVer( iih->dwarf->handle, VER_V1 );
    } else if( list.wat_producer_ver == VER_ERROR ) {
        DCStatus( DS_INFO_BAD_VERSION );
        ds = DS_FAIL | DS_INFO_BAD_VERSION;
    }
    return( ds );
}

bool    ClearMods( imp_image_handle *iih )
/****************************************/
// free any cached mem for modules
{
    im_idx      i;
    bool        ret;
    mod_info    *modinfo;

    ret = false;
    modinfo = iih->mod_map;
    for( i = 0; i < iih->mod_count; ++i ) {
        if( modinfo->addr_sym->head != NULL ) {
            FiniAddrSym( modinfo->addr_sym );
            ret = true;
        }
        ++modinfo;
    }
    return( ret );
}

void    FiniModMap( imp_image_handle *iih )
/*****************************************/
// Make the imp_mod_handle to drmem_hdl map
{
    im_idx      i;
    mod_info    *modinfo;

    ClearMods( iih );
    FiniAddrSym( iih->addr_sym );
    modinfo = iih->mod_map;
    for( i = 0; i < iih->mod_count; ++i ) {
        if( modinfo->name != NULL ) {
            DCFree( modinfo->name );
        }
        ++modinfo;
    }
    DCFree( iih->mod_map );
    iih->mod_map = NULL;
    iih->mod_count = 0;
}

imp_mod_handle   Dwarf2Mod( imp_image_handle *iih, drmem_hdl mod_handle )
/***********************************************************************/
// Look up mod_handle in mod_map
{
    im_idx      i;
    mod_info    *modinfo;

    modinfo = iih->mod_map;
    for( i = 0; i < iih->mod_count; ++i ) {
        if( mod_handle == modinfo->mod_handle ) {
            return( IMX2IMH( i ) );
        }
        ++modinfo;
    }
    return( IMH_NOMOD );
}

imp_mod_handle   DwarfMod( imp_image_handle *iih, drmem_hdl dr_sym )
/******************************************************************/
// find the imp_mod_handle where a dwarf dbginfo comes from
{
    im_idx      i;
    im_idx      last;
    mod_info    *modinfo;

    if( iih->mod_count == 0 )
        return( IMH_NOMOD );
    modinfo = iih->mod_map + 1;
    last = iih->mod_count - 1;
    for( i = 0; i < last; ++i ) {
        if( dr_sym < modinfo->mod_handle ) {
            break;
        }
        ++modinfo;
    }
    return( IMX2IMH( i ) );
}

imp_mod_handle   CuTag2Mod( imp_image_handle *iih, drmem_hdl cu_tag )
/*******************************************************************/
// Look up cu_tag in mod_map
{
    im_idx      i;
    mod_info    *modinfo;

    modinfo = iih->mod_map;
    for( i = 0; i < iih->mod_count; ++i ) {
        if( cu_tag == modinfo->cu_tag ) {
            return( IMX2IMH( i ) );
        }
        ++modinfo;
    }
    return( IMH_NOMOD );
}

walk_result DFWalkModList( imp_image_handle *iih, DIP_IMP_MOD_WALKER wk, void *d )
{
    im_idx          i;
    walk_result     ret;
    dr_dbg_handle   saved;

    ret = WR_CONTINUE;
    for( i = 0; i < iih->mod_count; ++i ) {
        saved = DRGetDebug();
        ret = wk( iih, IMX2IMH( i ), d );
        DRSetDebug( saved );
        if( ret != WR_CONTINUE ) {
            break;
        }
    }
    return( ret );
}

walk_result DFWalkModListSrc( imp_image_handle *iih, bool src, DIP_IMP_MOD_WALKER wk, void *d )
{
    im_idx          i;
    walk_result     ret;
    dr_dbg_handle   saved;
    mod_info        *modinfo;

    ret = WR_CONTINUE;
    modinfo = iih->mod_map;
    for( i = 0; i < iih->mod_count; ++i ) {
        if( src == modinfo->has_pubnames ) {
            saved = DRGetDebug();
            ret = wk( iih, IMX2IMH( i ), d );
            DRSetDebug( saved );
            if( ret != WR_CONTINUE ) {
                break;
            }
        }
        ++modinfo;
    }
    return( ret );
}

walk_result DIPIMPENTRY( WalkModList )( imp_image_handle *iih, DIP_IMP_MOD_WALKER *wk, void *d )
{
    im_idx          i;
    walk_result     ret;
    dr_dbg_handle   saved;

    ret = WR_CONTINUE;
    for( i = 0; i < iih->mod_count; ++i ) {
        saved = DRGetDebug();
        ret = wk( iih, IMX2IMH( i ), d );
        DRSetDebug( saved );
        if( ret != WR_CONTINUE ) {
            break;
        }
    }
    return( ret );
}

size_t DIPIMPENTRY( ModName )( imp_image_handle *iih,
                        imp_mod_handle imh, char *buff, size_t buff_size )
{
    char        *name;
    size_t      len;

    if( imh == IMH_NOMOD || (name = IMH2MODI( iih, imh )->name) == NULL ) {
        DCStatus( DS_FAIL );
        return( 0 );
    }
    len = NameCopy( buff, name, buff_size, 0 );
    return( len );
}

char *DIPIMPENTRY( ModSrcLang )( imp_image_handle *iih, imp_mod_handle imh )
{
    char       *ret = NULL;

    if( imh == IMH_NOMOD ) {
        DCStatus( DS_FAIL );
        return( NULL );
    }
    switch( IMH2MODI( iih, imh )->lang ) {
    case DR_LANG_UNKNOWN:
//      ret = "unknown";
        ret = "c";
        break;
    case DR_LANG_CPLUSPLUS:
        ret = "cpp";
        break;
    case DR_LANG_FORTRAN:
        ret = "fortran";
        break;
    case DR_LANG_C:
        ret = "c";
        break;
    }
    return( ret );
}


search_result DFAddrMod( imp_image_handle *iih, address a, imp_mod_handle *imh )
{
    off_info    *off;

    if(  a.mach.segment == iih->last.mach.segment
      && a.mach.offset  >= iih->last.mach.offset
      && a.mach.offset  <  iih->last.mach.offset + iih->last.len ) {
        *imh = iih->last.imh;
        if( a.mach.offset == iih->last.mach.offset ) {
            return( SR_EXACT );
        } else {
            return( SR_CLOSEST );
        }

    }
    off = FindMapAddr( iih->addr_map, &a );
    if( off != NULL ) {
        iih->last.mach.segment = a.mach.segment;
        iih->last.mach.offset = off->offset;
        iih->last.len = off->len;
        iih->last.imh = off->imh;
        *imh = off->imh;
        if( a.mach.offset == off->offset ) {
            return( SR_EXACT );
        } else {
            return( SR_CLOSEST );
        }
    }
    return( SR_NONE );
}

search_result DIPIMPENTRY( AddrMod )( imp_image_handle *iih, address a, imp_mod_handle *imh )
{
    imp_mod_handle  curr_imh;
    search_result   ret;

    ret = DFAddrMod( iih, a, &curr_imh );
    *imh = curr_imh;
    return( ret );
}


typedef struct {
    imp_image_handle    *iih;
    imp_mod_handle      imh;
    address             *ret;
} l_walk_info;

static bool AModAddr( void *_info, dr_line_data *curr )
/*****************************************************/
{
    l_walk_info         *info = _info;
    bool                ret;
    imp_image_handle    *iih;

    ret = true;
    if( curr->is_stmt ) {
        off_info *off;

        *info->ret = NilAddr;
        info->ret->mach.offset = curr->offset;
        info->ret->mach.segment = curr->seg;
        iih = info->iih;
        DCMapAddr( &info->ret->mach, iih->dcmap );
        off = FindMapAddr( iih->addr_map, info->ret );
        if( off->imh == info->imh ) {
            ret = false;
        }
    }
    return( ret );
}

static bool ALineCue( void *_info, dr_line_data *curr )
/*****************************************************/
{
    l_walk_info *info = _info;
    bool        ret;

    ret = true;
    if( curr->is_stmt ) {
        *info->ret = NilAddr;
        info->ret->mach.offset = curr->offset;
        info->ret->mach.segment = curr->seg;
        ret = false;
    }
    return( ret );
}

address DIPIMPENTRY( ModAddr )( imp_image_handle *iih, imp_mod_handle imh )
{
    l_walk_info walk;
    address     a;
    drmem_hdl   stmts;

    if( imh != IMH_NOMOD && (stmts = IMH2MODI( iih, imh )->stmts) != DRMEM_HDL_NULL ) {
        DRSetDebug( iih->dwarf->handle ); /* must do at each call into dwarf */
        walk.iih = iih;
        walk.imh = imh;
        walk.ret = &a;
        if( !DRWalkLines( stmts, SEG_CODE, AModAddr, &walk ) ) {
            // found
            return( a );
        }
    }
    DCStatus( DS_FAIL );
    return( NilAddr );
}

dip_status DIPIMPENTRY( ModInfo )( imp_image_handle *iih, imp_mod_handle imh, handle_kind hk )
{
    /*
        Return DS_OK if the module has the kind of information indicated
        by 'hk', DS_FAIL if it does not.
    */
    dip_status  ds;
    drmem_hdl   stmts;
    mod_info    *modinfo;

    ds = DS_FAIL;
    if( imh == IMH_NOMOD ) {
        DCStatus( ds );
        return( ds );
    }
    modinfo = IMH2MODI( iih, imh );
    switch( hk ) {
    case HK_IMAGE:
        break;
    case HK_TYPE:
        if( modinfo->stmts != DRMEM_HDL_NULL ) {
            ds = DS_OK;
        }
        break;
    case HK_CUE:
        stmts = modinfo->stmts;
        if( stmts != DRMEM_HDL_NULL ) {  // need to get rid of stmts for file with no cues
            l_walk_info walk;
            address     a;

            walk.iih = iih;
            walk.imh = imh;
            walk.ret = &a;
            DRSetDebug( iih->dwarf->handle ); /* set dwarf to image */
            if( !DRWalkLines( stmts, SEG_CODE, ALineCue, &walk ) ) {
                ds = DS_OK;
            }
        }
        break;
    case HK_SYM: /* if no lang assumed linker generated */
        if( modinfo->lang != DR_LANG_UNKNOWN ) {
            ds = DS_OK;
        }
        break;
    default:
        break;
    }
    return( ds );
}

dip_status DIPIMPENTRY( ModDefault )( imp_image_handle *iih, imp_mod_handle imh, default_kind dk, dig_type_info *ti )
{
    /*
        Return the default type information for indicated type. The
        client uses this to figure out how big a default 'int', code pointer,
        and data pointer should be. The information depends on whether
        the 16 or 32-bit compiler was used, and what memory model the
        source file was compiled with.
     */
    mod_info    *modinfo;

//TODO: finish
    modinfo = IMH2MODI( iih, imh );
    ti->size = modinfo->addr_size;
    ti->deref = false;
    switch( dk ) {
    case DK_INT:
        ti->kind = TK_INTEGER;
        ti->modifier = TM_SIGNED;
        break;
    case DK_DATA_PTR:
    case DK_CODE_PTR:
        ti->kind = TK_POINTER;
        switch( modinfo->model ) {
        case DR_MODEL_NONE:
        case DR_MODEL_FLAT:
        case DR_MODEL_SMALL:
            ti->modifier = TM_NEAR;
            break;
        case DR_MODEL_MEDIUM:
            if( dk == DK_CODE_PTR ) {
                ti->modifier = TM_FAR;
                ti->size += 2;
            } else {
                ti->modifier = TM_NEAR;
            }
            break;
        case DR_MODEL_COMPACT:
            if( dk == DK_CODE_PTR ) {
                ti->modifier = TM_NEAR;
            } else {
                ti->modifier = TM_FAR;
                ti->size += 2;
            }
            break;
        case DR_MODEL_LARGE:
            ti->modifier = TM_FAR;
            ti->size += 2;
            break;
        case DR_MODEL_HUGE:
            if( dk == DK_CODE_PTR ) {
                ti->modifier = TM_FAR;
            } else {
                ti->modifier = TM_HUGE;
            }
            ti->size += 2;
            break;
        }
    }
    return( DS_OK );
}

size_t NameCopy( char *buff, const char *from, size_t buff_size, size_t len )
/***************************************************************************/
{
    size_t      new_len;

    new_len = len + strlen( from );
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > new_len )
            buff_size = new_len;
        buff_size -= len;
        buff += len;
        while( buff_size-- > 0 ) {
            *buff++ = *from++;
        }
        *buff = '\0';
    }
    return( new_len );
}

void  SetModPubNames( imp_image_handle *iih, drmem_hdl mod_handle )
/*****************************************************************/
{
    imp_mod_handle imh;

    imh = Dwarf2Mod( iih, mod_handle );
    if( imh != IMH_NOMOD ) {
        IMH2MODI( iih, imh )->has_pubnames = true;
    }
}
