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


#include <string.h>
#include <stdlib.h>
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

static void GetModName( char *path, char *buff ){
/************************************************/
/* find the module name from the path           */
/************************************************/
    char *end, *start;
    int ext_found;
    start = path;
    ext_found = FALSE;
    while( *path != '\0' ){
        if( IS_PATH_CHAR( *path ) ){
            start = path+1;
        }else if( *path == EXT_CHAR ){
            ext_found = TRUE;
            end = path;
        }else if( *path == '(' ){
            start = path+1;
            while( *path != ')' )++path;
            end = path;
            ext_found = TRUE;
            goto do_copy;
        }
        ++path;
    }
    if( !ext_found ){
        end = path;
    }
do_copy:
    while( start < end ){
        *buff = *start;
        ++start;
        ++buff;
    }
    *buff = '\0';
}

static bool ModFill( void *_mod, dr_handle mod_handle ){
/**************************************************************/
// fill in mod_handle for dip to dwarf mod map
// pick up general info about mod while here for later calls
    mod_list *mod = _mod;
    char    fname[MAX_PATH];
    char   *name;
    char   *path;
    dr_handle   cu_tag;
    dr_model    model;
    mod_info   *curr;

    curr = NextModInfo( mod );
    curr->mod_handle = mod_handle;
    InitAddrSym( curr->addr_sym );
    curr->addr_size = DRGetAddrSize( mod_handle );
    cu_tag = DRGetCompileUnitTag( mod_handle );
    curr->cu_tag = cu_tag;
    curr->stmts = DRGetStmtList( cu_tag );
    path = DRGetName( cu_tag );
    if( path != NULL ){
        GetModName( path, fname );
        DCFree( path );
        name = DCAlloc( strlen( fname )+1 );
        strcpy( name, fname );
    }else{
        name = NULL;
    }
    path = DRGetProducer( cu_tag );
    if( path != NULL ){

        df_ver version;

        if( strcmp( path, "V2.0 WATCOM" ) == 0 ) {
            version = VER_V3;
        } else if( strcmp( path, "V1.0 WATCOM" ) == 0 ) {
            version = VER_V2;
        }else if( strcmp( path, "WATCOM" ) == 0 ){
            version = VER_V1;
        }else{
            version = VER_NONE;
        }
        if( mod->version == VER_NONE ){
            mod->version = version;
        }else if( mod->version != version ){
            mod->version = VER_ERROR;
        }
        DCFree( path );
    }
    curr->name = name;
    model = DRGetMemModelAT( cu_tag );
    if( DCCurrMAD() == MAD_X86 ) {
        switch( model ){
        case DR_MODEL_NONE:
        case DR_MODEL_FLAT:
            curr->is_segment = FALSE;
            break;
        default:
            curr->is_segment = TRUE;
            break;
        }
    } else {
        curr->is_segment = FALSE;
    }
    curr->model = model;
    curr->lang  = DRGetLanguageAT( cu_tag );
    curr->dbg_pch = DRDebugPCHDef( cu_tag );
    curr->has_pubnames = FALSE;
    return( TRUE );
}

extern  dip_status     InitModMap( imp_image_handle *ii ){
/**************************************************/
// Make the imp_mod_handle  to  dr_handle map
    mod_list list;
    dip_status ret;

    ret = DS_OK;
    ii->mod_count = 0;
    InitModList( &list );
    InitAddrSym( ii->addr_sym );
    DRSetDebug( ii->dwarf->handle ); /* set dwarf to image */
    DRIterateCompileUnits( &list, ModFill );
    DRDbgClear( ii->dwarf->handle ); /* clear some mem */
    ii->mod_count = list.count;
    ii->mod_map = FiniModInfo( &list );
    if( list.version == VER_V1 ) {
        DRDbgOldVersion( ii->dwarf->handle, 1 );
    } else if( list.version == VER_V2 ) {
        DRDbgOldVersion( ii->dwarf->handle, 2 );
    } else if( list.version == VER_ERROR ) {
        DCStatus( DS_INFO_BAD_VERSION );
        ret = DS_FAIL | DS_INFO_BAD_VERSION;
    }
    return( ret );
}

extern  bool    ClearMods( imp_image_handle *ii ){
/********************************/
// free any cached mem for modules
    int i;
    int ret;

    ret = FALSE;
    if( ii->mod_map != NULL ){
        for( i = 0; i < ii->mod_count; ++i ){
            if( ii->mod_map[i].addr_sym->head != NULL ){
                FiniAddrSym( ii->mod_map[i].addr_sym );
                ret = TRUE;
            }
        }
    }
   return( ret );
}

extern  void    FiniModMap( imp_image_handle *ii ){
/**************************************************/
// Make the imp_mod_handle  to  dr_handle map
    int i;

    ClearMods( ii );
    FiniAddrSym( ii->addr_sym );
    if( ii->mod_map != NULL ){
        for( i = 0; i < ii->mod_count; ++i ){
            if( ii->mod_map[i].name != NULL ){
                DCFree( ii->mod_map[i].name );
            }
        }
        DCFree( ii->mod_map  );
        ii->mod_map = NULL;
    }
}

extern  im_idx  Dwarf2ModIdx( imp_image_handle *ii, dr_handle mod_handle ){
/******************************************************************************/
// Look up mod_handle in mod_map
    im_idx i;

    for( i = FIRST_IMX; i < ii->mod_count; ++i ){
        if( mod_handle == ii->mod_map[i].mod_handle ){
            return( i );
        }
    }
    return( INVALID_IMX );
}

extern  im_idx  DwarfModIdx( imp_image_handle *ii, dr_handle mod_handle ){
/************************************************************************/
// find the im_idx where a dwarf dbginfo comes from
    im_idx i;
    im_idx last;

    if( ii->mod_count == 0 ){
        return( INVALID_IMX );
    }
    last = ii->mod_count - 1;
    for( i = 0; i < last; ++i ){
        if( mod_handle < ii->mod_map[i+1].mod_handle ){
            return( i );
        }
    }
    return( i );
}

extern  im_idx  CuTag2ModIdx( imp_image_handle *ii, dr_handle cu_handle ){
/******************************************************************************/
// Look up mod_handle in mod_map
    im_idx i;

    for( i = FIRST_IMX; i < ii->mod_count; ++i ){
        if( cu_handle == ii->mod_map[i].cu_tag ){
            return( i );
        }
    }
    return( INVALID_IMX );
}

extern walk_result DFWalkModList( imp_image_handle *ii,
                        MY_MOD_WKR wk, void *d )
{
    im_idx imx;
    walk_result ret;
    dr_dbg_handle  saved;

    ret = WR_CONTINUE;
    for( imx = FIRST_IMX; imx < ii->mod_count; ++imx ){
        saved = DRGetDebug();
        ret = wk( ii, imx, d );
        DRSetDebug( saved );
        if( ret != WR_CONTINUE )break;
    }
    return( ret );
}

extern walk_result DFWalkModListSrc( imp_image_handle *ii,
                                 int src,
                                 MY_MOD_WKR wk, void *d )
{
    im_idx          imx;
    int             hassrc;
    walk_result ret;
    dr_dbg_handle  saved;

    ret = WR_CONTINUE;
    for( imx = FIRST_IMX; imx < ii->mod_count; ++imx ){
        hassrc =  ii->mod_map[imx].has_pubnames;
        if( src == hassrc ){
            saved = DRGetDebug();
            ret = wk( ii, imx, d );
            DRSetDebug( saved );
            if( ret != WR_CONTINUE )break;
        }
    }
    return( ret );
}

walk_result DIPENTRY DIPImpWalkModList( imp_image_handle *ii,
                        IMP_MOD_WKR wk, void *d )
{
    int im;
    walk_result ret;
    dr_dbg_handle  saved;

    ret = WR_CONTINUE;
    for( im = 1; im <= ii->mod_count; ++im ){
        saved = DRGetDebug();
        ret = wk( ii, im, d );
        DRSetDebug( saved );
        if( ret != WR_CONTINUE )break;
    }
    return( ret );

}

unsigned        DIPENTRY DIPImpModName( imp_image_handle *ii,
                        imp_mod_handle im, char *buff, unsigned max )
{
    char        *name;
    unsigned    len;
    im_idx      imx;


    if( im == 0 ){
        DCStatus( DS_FAIL );
        return( 0 );
    }
    imx = IM2IMX( im );
    name = ii->mod_map[imx].name;
    if( name == NULL ){
        DCStatus( DS_FAIL );
        return( 0 );
    }
    len = NameCopy( buff, name, max );
    return( len );
}

char            *DIPENTRY DIPImpModSrcLang( imp_image_handle *ii, imp_mod_handle im )
{
    char       *ret;
    im_idx      imx;

    if( im == 0 ){
        DCStatus( DS_FAIL );
        return( 0 );
    }
    imx = IM2IMX( im );
    switch( ii->mod_map[imx].lang ){
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


extern search_result DFAddrMod( imp_image_handle *ii, address a,
                im_idx *imx )
{
    off_info   *off;

    if(  a.mach.segment == ii->last.mach.segment
      && a.mach.offset  >= ii->last.mach.offset
      && a.mach.offset  <  ii->last.mach.offset+ii->last.len ){
        *imx = ii->last.imx;
        if( a.mach.offset == ii->last.mach.offset ){
            return( SR_EXACT );
        }else{
            return( SR_CLOSEST );
        }

    }
    off = FindMapAddr( ii->addr_map, &a );
    if( off != NULL ){
        ii->last.mach.segment = a.mach.segment;
        ii->last.mach.offset  = off->offset;
        ii->last.len = off->len;
        ii->last.imx = off->imx;
        *imx =  off->imx;
        if( a.mach.offset == off->offset ){
            return( SR_EXACT );
        }else{
            return( SR_CLOSEST );
        }
    }
    return( SR_NONE );
}

search_result   DIPENTRY DIPImpAddrMod( imp_image_handle *ii, address a,
                imp_mod_handle *im )
{
    im_idx imx;
    search_result ret;

    ret = DFAddrMod( ii, a, &imx );
    *im = IMX2IM( imx );
    return( ret );
}


typedef struct{
    imp_image_handle  *ii;
    im_idx            imx;
    address           *ret;
}l_walk_info;

static int AModAddr( void *_info, dr_line_data *curr ){
/*************************************************************/
    l_walk_info *info = _info;
    int ret;
    imp_image_handle  *ii;

    ret = TRUE;
    if( curr->is_stmt ){
        off_info *off;

        *info->ret = NilAddr;
        info->ret->mach.offset = curr->offset;
        info->ret->mach.segment = curr->seg;
        ii = info->ii;
        DCMapAddr( &info->ret->mach, ii->dcmap );
        off = FindMapAddr( ii->addr_map, info->ret );
        if( off->imx == info->imx ){
            ret = FALSE;
        }
    }
    return( ret );
}

static int ALineCue( void *_info, dr_line_data *curr ){
/*************************************************************/
    l_walk_info *info = _info;
    int ret;

    ret = TRUE;
    if( curr->is_stmt ){
        *info->ret = NilAddr;
        info->ret->mach.offset = curr->offset;
        info->ret->mach.segment = curr->seg;
        ret = FALSE;
    }
    return( ret );
}

address         DIPENTRY DIPImpModAddr( imp_image_handle *ii,
                                imp_mod_handle im )
{
    l_walk_info walk;
    address     a;
    im_idx      imx;
    dr_handle   stmts;

    if( im == 0 ){
        DCStatus( DS_FAIL );
        return( NilAddr );
    }
    imx = IM2IMX( im );
    stmts =  ii->mod_map[imx].stmts;
    if( stmts == 0 ) {
        DCStatus( DS_FAIL );
        return( NilAddr );
    }
    DRSetDebug( ii->dwarf->handle ); /* must do at each call into dwarf */
    walk.ii = ii;
    walk.imx = imx;
    walk.ret = &a;
    if( DRWalkLines( stmts, SEG_CODE, AModAddr, &walk ) ){ // nothing found
        a = NilAddr;
        DCStatus( DS_FAIL );
    }
    return( a );
}

dip_status      DIPENTRY DIPImpModInfo( imp_image_handle *ii,
                                imp_mod_handle im, handle_kind hk )
{
    /*
        Return DS_OK if the module has the kind of information indicated
        by 'hk', DS_FAIL if it does not.
    */
    dip_status  ret;
    dr_handle   stmts;
    im_idx      imx;

    if( im == 0 ){
        DCStatus( DS_FAIL );
        return( 0 );
    }
    imx = IM2IMX( im );
    stmts =  ii->mod_map[imx].stmts;
    ret = DS_FAIL;
    switch( hk ){
    case HK_IMAGE:
        ret = DS_FAIL;
        break;
    case HK_TYPE:
        if( stmts != 0 ) {
            ret = DS_OK;
        }
        break;
    case HK_CUE:
        if( stmts != 0 ) {  // need to get rid of stmts for file with no cues
            l_walk_info walk;
            address     a;

            walk.ii = ii;
            walk.imx = imx;
            walk.ret = &a;
            DRSetDebug( ii->dwarf->handle ); /* set dwarf to image */
            if( !DRWalkLines( stmts, SEG_CODE, ALineCue, &walk ) ){
                ret = DS_OK;
            }
        }
        break;
    case HK_SYM: /* if no lang assumed linker generated */
        if( ii->mod_map[imx].lang != DR_LANG_UNKNOWN ){
            ret = DS_OK;
        }
        break;
    default:
        ret = DS_FAIL;
        break;
    }
    return( ret );
}

dip_status      DIPENTRY DIPImpModDefault( imp_image_handle *ii,
                imp_mod_handle im, default_kind dk, type_info *ti )
{
    /*
        Return the default type information for indicated type. The
        client uses this to figure out how big a default 'int', code pointer,
        and data pointer should be. The information depends on whether
        the 16 or 32-bit compiler was used, and what memory model the
        source file was compiled with.
     */
//TODO: finish
    mod_info   *curr;
    im_idx      imx;
    int         size;
    ii=ii;
    im=im;
    dk=dk;
    ti=ti;

    imx = IM2IMX( im );
    curr = &ii->mod_map[imx];
    size = curr->addr_size;
    switch( dk ){
    case DK_INT:
        ti->kind = TK_INTEGER;
        ti->modifier = TM_SIGNED;
        ti->size = size;
        break;
    case DK_DATA_PTR:
    case DK_CODE_PTR:
        ti->kind = TK_POINTER;
        switch( curr->model ){
        case DR_MODEL_NONE:
        case DR_MODEL_FLAT:
        case DR_MODEL_SMALL:
            ti->modifier = TM_NEAR;
            break;
        case DR_MODEL_MEDIUM:
            if( dk == DK_CODE_PTR ){
                ti->modifier = TM_FAR;
                size += 2;
            }else{
                ti->modifier = TM_NEAR;
            }
            break;
        case DR_MODEL_COMPACT:
            if( dk == DK_CODE_PTR ){
                ti->modifier = TM_NEAR;
            }else{
                ti->modifier = TM_FAR;
                size += 2;
            }
            break;
        case DR_MODEL_LARGE:
            ti->modifier = TM_FAR;
            size += 2;
            break;
        case DR_MODEL_HUGE:
            if( dk == DK_CODE_PTR ){
                ti->modifier = TM_FAR;
            }else{
                ti->modifier = TM_HUGE;
            }
            size += 2;
            break;
        }
    }
    ti->size = size;
    return( DS_OK );
}

extern unsigned NameCopy( char *to, char *from, unsigned max )
/************************************************************/
{
    unsigned    len;

    len = strlen( from );
    if( max > 0 ) {
        if( len < max ) {
            max = len;
        } else {
            max = max - 1;
        }
        if( max > 0 ) {     // Check max to prevent underflow
            do {
                *to = *from;
                ++to;
                ++from;
            } while( --max > 0 );
        }
        *to = '\0';
    }
    return( len );
}

extern  void  SetModPubNames( imp_image_handle *ii, dr_handle mod_handle ){
/************************************************************************/
    im_idx imx;

    imx  = Dwarf2ModIdx( ii, mod_handle );
    if( imx != INVALID_IMX ){
        ii->mod_map[imx].has_pubnames = TRUE;
    }
}
