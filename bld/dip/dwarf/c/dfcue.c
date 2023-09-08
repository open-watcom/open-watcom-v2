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
* Description:  DWARF DIP source line cues.
*
****************************************************************************/


#include "dfdip.h"
#include <ctype.h>
#include "dfld.h"
#include "dfaddr.h"
#include "dfline.h"
#include "dfmod.h"
#include "dfmodinf.h"
#include "dfcue.h"


/************************
 *** cue cache **********
 ************************/

void InitImpCueInfo( imp_image_handle *iih )
/******************************************/
{
    cue_list    *list;

    list = iih->cue_map;
    InitCueList( list );
    list->imh = IMH_NOMOD;
    list->last.mach.segment = 0;
    list->last.mach.offset = 0;
    list->last.next_offset = 0;
}


static void ResetCueInfo( cue_list *list )
/****************************************/
{
    list->imh = IMH_NOMOD;
    list->last.mach.segment = 0;
    list->last.mach.offset = 0;
    list->last.next_offset = 0;
    FiniCueInfo( list );
}


bool FiniImpCueInfo( imp_image_handle *iih )
/******************************************/
{
    bool        ret;
    cue_list    *list;

    list = iih->cue_map;
    if( list->imh != IMH_NOMOD ) {
        ResetCueInfo( list );
        ret = true;
    } else {
        ret = false;
    }
    return( ret );
}


imp_mod_handle DIPIMPENTRY( CueMod )( imp_image_handle *iih, imp_cue_handle *icueh )
/***********************************************************************************
 * Return the module the source cue comes from.
 */
{
    /* unused parameters */ (void)iih;

    return( icueh->imh );
}


typedef struct {
    uint_16         index;
    char            *ret;
    uint_16         num_dirs;
    dr_line_dir     *dirs;
} file_walk_name;

static bool ACueFile( void *_info, dr_line_file *curr )
/*****************************************************/
{
    file_walk_name  *info = _info;
    bool            cont;
    int             i;

    cont = true;
    if( info->index == curr->index ) {
        if( curr->name != NULL ) {
            if( curr->dir != 0) {
                for( i = 0; i < info->num_dirs; i++ ) {
                    if( info->dirs[i].index == curr->dir ) {
                        break;
                    }
                }
                if( i < info->num_dirs ) {
                    info->ret = DCAlloc( strlen( curr->name ) + strlen( info->dirs[i].name ) + 2 );
                    strcpy( info->ret, info->dirs[i].name );
                    strcat( info->ret, "/");
                    strcat( info->ret, curr->name );
                } else {
                    /*
                     * This should be an error, but it isn't fatal as we should
                     * never get here in practice.
                     */
                    info->ret = DCAlloc( strlen( curr->name ) + 1 );
                    strcpy( info->ret, curr->name );
                }
            } else {
                info->ret = DCAlloc( strlen( curr->name ) + 1 );
                strcpy( info->ret, curr->name );
            }
        } else {
            info->ret = NULL;
        }
        cont = false;
    }
    return( cont );
}


static bool ACueDir( void *_info, dr_line_dir *curr )
/***************************************************/
{
    file_walk_name  *info = _info;

    if( info != NULL ) {
        info->dirs = DCRealloc( info->dirs, sizeof( dr_line_dir ) * (info->num_dirs + 1) );
        info->dirs[info->num_dirs].index = curr->index;
        info->dirs[info->num_dirs].name = DCAlloc( strlen( curr->name ) + 1 );
        strcpy( info->dirs[info->num_dirs].name, curr->name );
        info->num_dirs++;
    }
    return( true );
}


static bool IsRelPathname( const char *name )
/*******************************************/
{
    /*
     * Detect UNIX or DOS style relative pathnames
     */
    if( (name[0] == '/') || (name[0] == '\\') ) {
        return( false );
    }
    if( isalpha( name[0] ) && (name[1] == ':')
      && ((name[2] == '/') || (name[2] == '\\')) ) {
        return( false );
    }
    return( true );
}


size_t DIPIMPENTRY( CueFile )( imp_image_handle *iih, imp_cue_handle *icueh, char *buff, size_t buff_size )
/*********************************************************************************************************/
{
    char            *name;
    file_walk_name  wlk;
    size_t          len;
    drmem_hdl       stmts;
    drmem_hdl       cu_tag;
    int             i;
    mod_info        *modinfo;

    DRSetDebug( iih->dwarf->handle );    /* must do at each call into dwarf */
    modinfo = IMH2MODI( iih, icueh->imh );
    stmts = modinfo->stmts;
    cu_tag = modinfo->cu_tag;
    if( stmts == DRMEM_HDL_NULL || cu_tag == DRMEM_HDL_NULL ) {
        DCStatus( DS_FAIL );
        return( 0 );
    }
    wlk.index = icueh->fno;
    wlk.ret = NULL;
    wlk.num_dirs = 0;
    wlk.dirs = NULL;
    DRWalkLFiles( stmts, ACueFile, &wlk, ACueDir, &wlk );
    name = wlk.ret;
    /*
     * Free directory and file table information
     */
    for( i = 0; i < wlk.num_dirs; i++ ) {
        DCFree( wlk.dirs[i].name );
    }
    DCFree( wlk.dirs );

    if( name == NULL ) {
        DCStatus( DS_FAIL );
        return( 0 );
    }
    /*
     * If compilation unit has a DW_AT_comp_dir attribute, we need to
     * stuff that in front of the file pathname, unless that is absolute
     */
    len = DRGetCompDirBuff( cu_tag, buff, buff_size );
    if( ( len > 1 ) && IsRelPathname( name ) ) {  // Ignore empty comp dirs
        len = NameCopy( buff, "/", buff_size, len );
    } else {
        len = 0;
    }
    len = NameCopy( buff, name, buff_size, len );
    DCFree( name );
    return( len );
}


typedef struct {
    uint_16      fno;
    dr_line_data first;
} first_cue_wlk;

static bool TheFirstCue( void *_wlk, dr_line_data *curr )
/*******************************************************/
{
    first_cue_wlk   *wlk = _wlk;

    if( wlk->fno == curr->file ) {
        wlk->first = *curr;
        return( false );
    }
    return( true );
}


static bool FirstCue( drmem_hdl stmts, uint_16 fno, imp_cue_handle *icueh )
/*************************************************************************/
{
    bool            cont;
    first_cue_wlk   wlk;

    wlk.fno = fno;
    cont = DRWalkLines( stmts, SEG_CODE, TheFirstCue, &wlk );
    if( cont == false ) {
        icueh->fno = wlk.first.file;
        icueh->line = wlk.first.line;
//        icueh->col = wlk.first.col;
        icueh->col = 0;
        icueh->a = NilAddr;
        icueh->a.mach.segment = wlk.first.seg;
        icueh->a.mach.offset = wlk.first.offset;
    }
    return( cont );
}


typedef struct {
    drmem_hdl           stmts;
    imp_image_handle    *iih;
    imp_mod_handle      imh;
    DIP_IMP_CUE_WALKER  *wk;
    imp_cue_handle      *icueh;
    void                *d;
    walk_result         wr;
} file_walk_cue;

static bool ACueFileNum( void *_fc, dr_line_file *curr )
/******************************************************/
{
    file_walk_cue   *fc = _fc;
    bool            cont;
    imp_cue_handle  *icueh;
    dr_dbg_handle   saved;

    icueh = fc->icueh;
    icueh->a = NilAddr;
    icueh->imh = fc->imh;
    if( FirstCue( fc->stmts, curr->index, icueh ) ) {
        icueh->fno = curr->index;
        icueh->line = 1;
        icueh->col = 0;
    }
    saved = DRGetDebug();
    fc->wr = fc->wk( fc->iih, icueh, fc->d );
    DRSetDebug( saved );
    if( fc->wr == WR_CONTINUE ) {
        cont = true;
    } else {
        cont = false;
    }
    return( cont  );
}


walk_result DIPIMPENTRY( WalkFileList )( imp_image_handle *iih, imp_mod_handle imh,
                          DIP_IMP_CUE_WALKER *wk, imp_cue_handle *icueh, void *d )
/********************************************************************************/
{
    file_walk_cue   wlk;
    drmem_hdl       stmts;

    DRSetDebug( iih->dwarf->handle );    /* must do at each call into DWARF */
    stmts = IMH2MODI( iih, imh )->stmts;
    if( stmts == DRMEM_HDL_NULL ) {
        DCStatus( DS_FAIL );
        return( WR_CONTINUE );
    }
    wlk.stmts = stmts;
    wlk.iih = iih;
    wlk.imh = imh;
    wlk.wk = wk;
    wlk.icueh = icueh;
    wlk.d = d;
    wlk.wr = WR_CONTINUE;
    DRWalkLFiles( stmts, ACueFileNum, &wlk, ACueDir, NULL );
    return( wlk.wr );
}


cue_fileid DIPIMPENTRY( CueFileId )( imp_image_handle *iih, imp_cue_handle *icueh )
/*********************************************************************************/
{
    /* unused parameters */ (void)iih;

    return( icueh->fno );
}

/*******************************
 *** Load Cue map find cues  ***
 *******************************/

typedef struct {
    address         ret;
    cue_list        *list;
    seg_cue         *curr_seg;
} la_walk_info;

static bool ACueAddr( void *_info, dr_line_data *curr )
/*****************************************************/
{
    la_walk_info    *info = _info;

    if( curr->addr_set ) {  /* a set address */
        info->curr_seg = InitSegCue( info->list, curr->seg, curr->offset );
    }
    AddCue( info->curr_seg, curr );
    return( true );
}


static void LoadCueMap( drmem_hdl stmts, address *addr, cue_list *list )
/**********************************************************************/
{
    la_walk_info    wlk;

    wlk.ret = *addr;
    wlk.list = list;
    wlk.curr_seg = NULL;
    DRWalkLines( stmts, SEG_CODE, ACueAddr, &wlk );
}


dip_status DIPIMPENTRY( CueAdjust )( imp_image_handle *iih, imp_cue_handle *src_icueh,
                                                int adj, imp_cue_handle *dst_icueh )
/*************************************************************************************
 * Adjust the 'src' cue by 'adj' amount and return the result in 'dst'.
 * That is, If you get called with "DIPImpCueAdjust( iih, src, 1, dst )",
 * the 'dst' handle should be filled in with implementation cue handle
 * representing the source cue immediately following the 'src' cue.
 * Passing in an 'adj' of -1 will get the immediately preceeding source
 * cue. The list of source cues for each file are considered a ring,
 * so if 'src' is the first cue in a file, an 'adj' of -1 will return
 * the last source cue FOR THAT FILE. The cue adjust never crosses a
 * file boundry. Also, if 'src' is the last cue in a file, and 'adj' of
 * 1 will get the first source cue FOR THAT FILE. If an adjustment
 * causes a wrap from begining to end or vice versa, you should return
 * DS_WRAPPED status (NOTE: DS_ERR should *not* be or'd in, nor should
 * DCStatus be called in this case). Otherwise DS_OK should be returned
 * unless an error occurred.
 */
{
    drmem_hdl       stmts;
    dfline_search   start_state;
    dfline_find     find;
    dip_status      ds;
    cue_item        cue;
    cue_list        *cue_map;
    address         map_addr;

    DRSetDebug( iih->dwarf->handle );    /* must do at each call into DWARF */
    stmts = IMH2MODI( iih, src_icueh->imh )->stmts;
    if( stmts == DRMEM_HDL_NULL ) {
        DCStatus( DS_FAIL );
        return( DS_ERR | DS_FAIL  );
    }
    cue_map = iih->cue_map;
    if( cue_map->imh != src_icueh->imh ) {
        ResetCueInfo( cue_map );
        cue_map->imh = src_icueh->imh;
        map_addr = NilAddr;
        LoadCueMap( stmts, &map_addr, cue_map );
    }
    if( adj < 0 ) {
        start_state = LOOK_LOW;
        adj = -adj;
    } else {
        start_state = LOOK_HIGH;
    }
    cue.fno = src_icueh->fno;
    cue.line = src_icueh->line;
    cue.col = src_icueh->col;
    cue.mach = src_icueh->a.mach;
    find = LINE_NOT;
    while( 0 != adj ) {
        find = FindCue( cue_map, &cue, start_state );
        if( find == LINE_NOT )
            break;
        --adj;
    }
    dst_icueh->imh = src_icueh->imh;
    dst_icueh->fno = cue.fno;
    dst_icueh->line = cue.line;
    dst_icueh->col = cue.col;
    dst_icueh->a.mach = cue.mach;
    ds = DS_FAIL;
    switch( find ) {
    case LINE_NOT:
        DCStatus( DS_FAIL );
        ds = DS_ERR | DS_FAIL;
        break;
    case LINE_WRAPPED:
        ds = DS_WRAPPED;
        break;
    case LINE_FOUND:
        ds = DS_OK;
        break;
    }
    return( ds );
}


unsigned long DIPIMPENTRY( CueLine )( imp_image_handle *iih, imp_cue_handle *icueh )
/**********************************************************************************/
{
    /* unused parameters */ (void)iih;

    return( icueh->line );
}


unsigned DIPIMPENTRY( CueColumn )( imp_image_handle *iih, imp_cue_handle *icueh )
/********************************************************************************
 * Return the column number of source cue. Return zero if there
 * is no column number associated with the cue, or an error occurs in
 * getting the information.
 */
{
    /* unused parameters */ (void)iih;

    return( icueh->col );
}


address DIPIMPENTRY( CueAddr )( imp_image_handle *iih, imp_cue_handle *icueh )
/*****************************************************************************
 * Return the address of source cue. Return NilAddr if there
 * is no address associated with the cue, or an error occurs in
 * getting the information.
 */
{
    address     ret;

    ret = icueh->a;
    DCMapAddr( &ret.mach, iih->dcmap );
    return( ret );
}


search_result DIPIMPENTRY( AddrCue )( imp_image_handle *iih,
                imp_mod_handle imh, address addr, imp_cue_handle *icueh )
/************************************************************************
 * Search for the closest cue in the given module that has an address
 * less then or equal to the given address. If there is no such cue
 * return SR_NONE. If there is one exactly at the address return
 * SR_EXACT. Otherwise, return SR_CLOSEST.
 */
{
    address         map_addr;
    search_result   ret;
    cue_list        *cue_map;
    cue_item        cue;
    drmem_hdl       stmts;

    if( imh == IMH_NOMOD ) {
        DCStatus( DS_FAIL );
        return( SR_NONE );
    }
    DRSetDebug( iih->dwarf->handle ); /* must do at each call into dwarf */
    stmts = IMH2MODI( iih, imh )->stmts;
    if( stmts == DRMEM_HDL_NULL ) {
        return( SR_NONE );
    }
    map_addr = addr;
    cue_map = iih->cue_map;
    Real2Map( iih->addr_map, &map_addr );
    if( cue_map->imh != imh ) {
        ResetCueInfo( cue_map );
        cue_map->imh = imh;
        LoadCueMap( stmts, &map_addr, cue_map );
    }
    icueh->imh = imh;
    icueh->fno = 0;
    icueh->line = 0;
    icueh->col = 0;
    icueh->a = NilAddr;
    ret = SR_NONE;
    if( map_addr.mach.segment == cue_map->last.mach.segment
      && map_addr.mach.offset  >= cue_map->last.mach.offset
      && map_addr.mach.offset  <  cue_map->last.next_offset ) {
        icueh->fno = cue_map->last.fno;
        icueh->line = cue_map->last.line;
        icueh->col = cue_map->last.col;
        icueh->a.mach = cue_map->last.mach;
        if( cue_map->last.mach.offset == map_addr.mach.offset ) {
            ret = SR_EXACT;
        } else {
            ret = SR_CLOSEST;
        }
        return( ret );
     }
    if( FindCueOffset( cue_map, &map_addr.mach, &cue ) ) {
        icueh->fno = cue.fno;
        icueh->line = cue.line;
        icueh->col = cue.col;
        icueh->a.mach = cue.mach;
        if( cue.mach.offset == map_addr.mach.offset ) {
            ret = SR_EXACT;
        } else {
            ret = SR_CLOSEST;
        }
        cue_map->last = cue;
    }
    return( ret );
}


search_result DIPIMPENTRY( LineCue )( imp_image_handle *iih,
                imp_mod_handle imh, cue_fileid file, unsigned long line,
                unsigned column, imp_cue_handle *icueh )
/*********************************************************************/
{
    search_result   ret;
    dfline_find     find;
    dfline_search   what;
    drmem_hdl       stmts;
    cue_list        *cue_map;
    address         map_addr;
    cue_item        cue;

    if( imh == IMH_NOMOD ) {
        DCStatus( DS_FAIL );
        return( SR_NONE );
    }
    DRSetDebug( iih->dwarf->handle );    /* must do at each call into DWARF */
    stmts = IMH2MODI( iih, imh )->stmts;
    if( stmts == DRMEM_HDL_NULL ) {
        return( SR_NONE );
    }
    cue_map = iih->cue_map;
    if( cue_map->imh != imh ) {
        ResetCueInfo( cue_map );
        cue_map->imh = imh;
        map_addr = NilAddr;
        LoadCueMap( stmts, &map_addr, cue_map );
    }
    if( file == 0 ) {   // primary file
        cue.fno = 1;
    } else {
        cue.fno = (uint_16)file;
    }
    cue.line = line;
    cue.col = (uint_16)column;
    cue.mach.offset = 0;
    cue.mach.segment = 0;
    icueh->a = NilAddr;
    if( line == 0 ) {
        what = LOOK_FILE;
    } else {
        what = LOOK_CLOSEST;
    }
    find = FindCue( cue_map, &cue, what );
    icueh->imh = imh;
    icueh->fno = cue.fno;
    icueh->line = cue.line;
    icueh->col = cue.col;
    icueh->a.mach = cue.mach;
    ret = SR_NONE;
    switch( find ) {
    case LINE_CLOSEST:
        ret = SR_CLOSEST;
        break;
    case LINE_FOUND:
        ret = SR_EXACT;
        break;
    case LINE_NOT:
        ret = SR_NONE;
        break;
    }
    return( ret );
}


int DIPIMPENTRY( CueCmp )( imp_image_handle *iih, imp_cue_handle *icueh1, imp_cue_handle *icueh2 )
/*************************************************************************************************
 * Compare two cue handles and return 0 if they refer to the same
 * information. If they refer to differnt things return either a
 * positive or negative value to impose an 'order' on the information.
 * The value should obey the following constraints.
 * Given three handles H1, H2, H3:
 *         - if H1 < H2 then H1 is always < H2
 *         - if H1 < H2 and H2 < H3 then H1 is < H3
 * The reason for the constraints is so that a client can sort a
 * list of handles and binary search them.
 */
{
    /* unused parameters */ (void)iih;

    if( icueh1->imh < icueh2->imh )
        return( -1 );
    if( icueh1->imh > icueh2->imh )
        return( 1 );
    if( icueh1->fno < icueh2->fno )
        return( -1 );
    if( icueh1->fno > icueh2->fno )
        return( 1 );
    if( icueh1->line < icueh2->line )
        return( -1 );
    if( icueh1->line > icueh2->line )
        return( 1 );
    if( icueh1->col < icueh2->col )
        return( -1 );
    if( icueh1->col > icueh2->col )
        return( 1 );
    return( 0 );
}
