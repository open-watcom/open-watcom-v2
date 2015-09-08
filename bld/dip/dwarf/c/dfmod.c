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
    int         ext_found;

    start = end = path;
    ext_found = FALSE;
    while( *path != '\0' ) {
        if( IS_PATH_CHAR( *path ) ) {
            start = path + 1;
        } else if( *path == EXT_CHAR ) {
            ext_found = TRUE;
            end = path;
        } else if( *path == '(' ) {
            start = path + 1;
            while( *path != ')' )
                ++path;
            end = path;
            ext_found = TRUE;
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

static bool ModFill( void *_mod, dr_handle mod_handle )
/*****************************************************/
// fill in mod_handle for dip to dwarf mod map
// pick up general info about mod while here for later calls
{
    mod_list    *mod = _mod;
    char        fname[MAX_PATH];
    char        *name;
    char        *path;
    dr_handle   cu_tag;
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
    if( DCCurrMAD() == MAD_X86 ) {
        switch( model ) {
        case DR_MODEL_NONE:
        case DR_MODEL_FLAT:
            modinfo->is_segment = FALSE;
            break;
        default:
            modinfo->is_segment = TRUE;
            break;
        }
    } else {
        modinfo->is_segment = FALSE;
    }
    modinfo->model = model;
    modinfo->lang = DRGetLanguageAT( cu_tag );
    modinfo->dbg_pch = DRDebugPCHDef( cu_tag );
    modinfo->has_pubnames = FALSE;
    return( TRUE );
}

dip_status     InitModMap( imp_image_handle *ii )
/***********************************************/
// Make the imp_mod_handle  to  dr_handle map
{
    mod_list    list;
    dip_status  ret;

    ret = DS_OK;
    InitModList( &list );
    InitAddrSym( ii->addr_sym );
    DRSetDebug( ii->dwarf->handle ); /* set dwarf to image */
    DRIterateCompileUnits( &list, ModFill );
    DRDbgClear( ii->dwarf->handle ); /* clear some mem */
    ii->mod_count = list.count;
    ii->mod_map = FiniModInfo( &list );
    if( list.wat_producer_ver == VER_V3 ) {
        DRDbgWatProducerVer( ii->dwarf->handle, VER_V3 );
    } else if( list.wat_producer_ver == VER_V2 ) {
        DRDbgWatProducerVer( ii->dwarf->handle, VER_V2 );
    } else if( list.wat_producer_ver == VER_V1 ) {
        DRDbgWatProducerVer( ii->dwarf->handle, VER_V1 );
    } else if( list.wat_producer_ver == VER_ERROR ) {
        DCStatus( DS_INFO_BAD_VERSION );
        ret = DS_FAIL | DS_INFO_BAD_VERSION;
    }
    return( ret );
}

bool    ClearMods( imp_image_handle *ii )
/***************************************/
// free any cached mem for modules
{
    im_idx      i;
    bool        ret;
    mod_info    *modinfo;

    ret = FALSE;
    modinfo = ii->mod_map;
    for( i = 0; i < ii->mod_count; ++i ) {
        if( modinfo->addr_sym->head != NULL ) {
            FiniAddrSym( modinfo->addr_sym );
            ret = TRUE;
        }
        ++modinfo;
    }
    return( ret );
}

void    FiniModMap( imp_image_handle *ii )
/****************************************/
// Make the imp_mod_handle to dr_handle map
{
    im_idx      i;
    mod_info    *modinfo;

    ClearMods( ii );
    FiniAddrSym( ii->addr_sym );
    modinfo = ii->mod_map;
    for( i = 0; i < ii->mod_count; ++i ) {
        if( modinfo->name != NULL ) {
            DCFree( modinfo->name );
        }
        ++modinfo;
    }
    DCFree( ii->mod_map );
    ii->mod_map = NULL;
    ii->mod_count = 0;
}

imp_mod_handle   Dwarf2Mod( imp_image_handle *ii, dr_handle mod_handle )
/**********************************************************************/
// Look up mod_handle in mod_map
{
    im_idx      i;
    mod_info    *modinfo;

    modinfo = ii->mod_map;
    for( i = 0; i < ii->mod_count; ++i ) {
        if( mod_handle == modinfo->mod_handle ) {
            return( IMX2IMH( i ) );
        }
        ++modinfo;
    }
    return( IMH_NOMOD );
}

imp_mod_handle   DwarfMod( imp_image_handle *ii, dr_handle dr_sym )
/*****************************************************************/
// find the imp_mod_handle where a dwarf dbginfo comes from
{
    im_idx      i;
    im_idx      last;
    mod_info    *modinfo;

    if( ii->mod_count == 0 )
        return( IMH_NOMOD );
    modinfo = ii->mod_map + 1;
    last = ii->mod_count - 1;
    for( i = 0; i < last; ++i ) {
        if( dr_sym < modinfo->mod_handle ) {
            break;
        }
        ++modinfo;
    }
    return( IMX2IMH( i ) );
}

imp_mod_handle   CuTag2Mod( imp_image_handle *ii, dr_handle cu_tag )
/******************************************************************/
// Look up cu_tag in mod_map
{
    im_idx      i;
    mod_info    *modinfo;

    modinfo = ii->mod_map;
    for( i = 0; i < ii->mod_count; ++i ) {
        if( cu_tag == modinfo->cu_tag ) {
            return( IMX2IMH( i ) );
        }
        ++modinfo;
    }
    return( IMH_NOMOD );
}

walk_result DFWalkModList( imp_image_handle *ii, IMP_MOD_WKR wk, void *d )
{
    im_idx          i;
    walk_result     ret;
    dr_dbg_handle   saved;

    ret = WR_CONTINUE;
    for( i = 0; i < ii->mod_count; ++i ) {
        saved = DRGetDebug();
        ret = wk( ii, IMX2IMH( i ), d );
        DRSetDebug( saved );
        if( ret != WR_CONTINUE ) {
            break;
        }
    }
    return( ret );
}

walk_result DFWalkModListSrc( imp_image_handle *ii, bool src, IMP_MOD_WKR wk, void *d )
{
    im_idx          i;
    walk_result     ret;
    dr_dbg_handle   saved;
    mod_info        *modinfo;

    ret = WR_CONTINUE;
    modinfo = ii->mod_map;
    for( i = 0; i < ii->mod_count; ++i ) {
        if( src == modinfo->has_pubnames ) {
            saved = DRGetDebug();
            ret = wk( ii, IMX2IMH( i ), d );
            DRSetDebug( saved );
            if( ret != WR_CONTINUE ) {
                break;
            }
        }
        ++modinfo;
    }
    return( ret );
}

walk_result DIGENTRY DIPImpWalkModList( imp_image_handle *ii, IMP_MOD_WKR *wk, void *d )
{
    im_idx          i;
    walk_result     ret;
    dr_dbg_handle   saved;

    ret = WR_CONTINUE;
    for( i = 0; i < ii->mod_count; ++i ) {
        saved = DRGetDebug();
        ret = wk( ii, IMX2IMH( i ), d );
        DRSetDebug( saved );
        if( ret != WR_CONTINUE ) {
            break;
        }
    }
    return( ret );
}

unsigned    DIGENTRY DIPImpModName( imp_image_handle *ii,
                        imp_mod_handle im, char *buff, unsigned buff_size )
{
    char        *name;
    unsigned    len;

    if( im == IMH_NOMOD || (name = IMH2MODI( ii, im )->name) == NULL ) {
        DCStatus( DS_FAIL );
        return( 0 );
    }
    len = NameCopy( buff, name, buff_size, 0 );
    return( len );
}

char    *DIGENTRY DIPImpModSrcLang( imp_image_handle *ii, imp_mod_handle im )
{
    char       *ret = NULL;

    if( im == IMH_NOMOD ) {
        DCStatus( DS_FAIL );
        return( NULL );
    }
    switch( IMH2MODI( ii, im )->lang ) {
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


search_result DFAddrMod( imp_image_handle *ii, address a, imp_mod_handle *im )
{
    off_info    *off;

    if(  a.mach.segment == ii->last.mach.segment
      && a.mach.offset  >= ii->last.mach.offset
      && a.mach.offset  <  ii->last.mach.offset+ii->last.len ) {
        *im = ii->last.im;
        if( a.mach.offset == ii->last.mach.offset ) {
            return( SR_EXACT );
        } else {
            return( SR_CLOSEST );
        }

    }
    off = FindMapAddr( ii->addr_map, &a );
    if( off != NULL ) {
        ii->last.mach.segment = a.mach.segment;
        ii->last.mach.offset = off->offset;
        ii->last.len = off->len;
        ii->last.im = off->im;
        *im = off->im;
        if( a.mach.offset == off->offset ) {
            return( SR_EXACT );
        } else {
            return( SR_CLOSEST );
        }
    }
    return( SR_NONE );
}

search_result   DIGENTRY DIPImpAddrMod( imp_image_handle *ii, address a, imp_mod_handle *im )
{
    imp_mod_handle  cim;
    search_result   ret;

    ret = DFAddrMod( ii, a, &cim );
    *im = cim;
    return( ret );
}


typedef struct {
    imp_image_handle    *ii;
    imp_mod_handle      im;
    address             *ret;
} l_walk_info;

static bool AModAddr( void *_info, dr_line_data *curr )
/*****************************************************/
{
    l_walk_info         *info = _info;
    bool                ret;
    imp_image_handle    *ii;

    ret = TRUE;
    if( curr->is_stmt ) {
        off_info *off;

        *info->ret = NilAddr;
        info->ret->mach.offset = curr->offset;
        info->ret->mach.segment = curr->seg;
        ii = info->ii;
        DCMapAddr( &info->ret->mach, ii->dcmap );
        off = FindMapAddr( ii->addr_map, info->ret );
        if( off->im == info->im ) {
            ret = FALSE;
        }
    }
    return( ret );
}

static bool ALineCue( void *_info, dr_line_data *curr )
/*****************************************************/
{
    l_walk_info *info = _info;
    bool        ret;

    ret = TRUE;
    if( curr->is_stmt ) {
        *info->ret = NilAddr;
        info->ret->mach.offset = curr->offset;
        info->ret->mach.segment = curr->seg;
        ret = FALSE;
    }
    return( ret );
}

address DIGENTRY DIPImpModAddr( imp_image_handle *ii, imp_mod_handle im )
{
    l_walk_info walk;
    address     a;
    dr_handle   stmts;

    if( im != IMH_NOMOD && (stmts = IMH2MODI( ii, im )->stmts) != DR_HANDLE_NUL ) {
        DRSetDebug( ii->dwarf->handle ); /* must do at each call into dwarf */
        walk.ii = ii;
        walk.im = im;
        walk.ret = &a;
        if( !DRWalkLines( stmts, SEG_CODE, AModAddr, &walk ) ) {
            // found
            return( a );
        }
    }
    DCStatus( DS_FAIL );
    return( NilAddr );
}

dip_status  DIGENTRY DIPImpModInfo( imp_image_handle *ii,
                                imp_mod_handle im, handle_kind hk )
{
    /*
        Return DS_OK if the module has the kind of information indicated
        by 'hk', DS_FAIL if it does not.
    */
    dip_status  ret;
    dr_handle   stmts;
    mod_info    *modinfo;

    ret = DS_FAIL;
    if( im == IMH_NOMOD ) {
        DCStatus( ret );
        return( ret );
    }
    modinfo = IMH2MODI( ii, im );
    switch( hk ) {
    case HK_IMAGE:
        break;
    case HK_TYPE:
        if( modinfo->stmts != DR_HANDLE_NUL ) {
            ret = DS_OK;
        }
        break;
    case HK_CUE:
        stmts = modinfo->stmts;
        if( stmts != DR_HANDLE_NUL ) {  // need to get rid of stmts for file with no cues
            l_walk_info walk;
            address     a;

            walk.ii = ii;
            walk.im = im;
            walk.ret = &a;
            DRSetDebug( ii->dwarf->handle ); /* set dwarf to image */
            if( !DRWalkLines( stmts, SEG_CODE, ALineCue, &walk ) ) {
                ret = DS_OK;
            }
        }
        break;
    case HK_SYM: /* if no lang assumed linker generated */
        if( modinfo->lang != DR_LANG_UNKNOWN ) {
            ret = DS_OK;
        }
        break;
    default:
        break;
    }
    return( ret );
}

dip_status  DIGENTRY DIPImpModDefault( imp_image_handle *ii,
                imp_mod_handle im, default_kind dk, dip_type_info *ti )
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
    modinfo = IMH2MODI( ii, im );
    ti->size = modinfo->addr_size;
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

unsigned    NameCopy( char *buff, const char *from, unsigned buff_size, unsigned len )
/************************************************************************************/
{
    unsigned    new_len;

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

void  SetModPubNames( imp_image_handle *ii, dr_handle mod_handle )
/****************************************************************/
{
    imp_mod_handle im;

    im = Dwarf2Mod( ii, mod_handle );
    if( im != IMH_NOMOD ) {
        IMH2MODI( ii, im )->has_pubnames = TRUE;
    }
}
