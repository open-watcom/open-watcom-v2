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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <string.h>
#include "dfdip.h"
#include "dfld.h"
#include "dfaddr.h"
#include "dfaddsym.h"
#include "dfmod.h"
#include "dfmodinf.h"
#include "dfcue.h"
#include "dfscope.h"
#include "dfmisc.h"
#include "dfhash.h"
#include "exeelf.h"
#include "tistrail.h"


typedef struct imp_image_handle *imp_image;

static char const * const SectionNames[DR_DEBUG_NUM_SECTS] = {
    ".debug_info",
    ".debug_pubnames",
    ".debug_aranges",
    ".debug_line",
    ".debug_loc",
    ".debug_abbrev",
    ".debug_macinfo",
    ".debug_str",
    ".WATCOM_references"
};

uint Lookup_section_name( const char *name ){
/******************************************/
    uint        sect;

    for( sect = 0 ; sect < DR_DEBUG_NUM_SECTS; sect++ ) {
        if( stricmp( SectionNames[sect], name ) == 0 ) {
            return sect;
        }
    }
    return( sect );
}

static dip_status GetSectInfo( dig_fhandle f, uint_32 *sizes, uint_32 *bases ){
/*****************************************************************/
// Fill in the starting offset & length of the dwarf sections

    TISTrailer          dbg_head;
    Elf32_Ehdr          elf_head;
    Elf32_Shdr          elf_sec;
    unsigned_32         offset;
    unsigned_32         start;
    char               *string_table;
    int                 i;
    uint                sect;

    // Find TIS header seek to elf header
    start = DCSeek( f, -(int)sizeof( dbg_head ), DIG_END );
    for(;;){
        if( DCRead( f, &dbg_head, sizeof(dbg_head)) != sizeof(dbg_head) ) {
            return( DS_FAIL );
        }
        if( dbg_head.signature != TIS_TRAILER_SIGNATURE ) {
            /* Seek to start of file and hope it's in ELF format */
            start = 0;
            DCSeek( f, 0, DIG_ORG );
            break;
        }
        start += sizeof( dbg_head );
        start  -= dbg_head.size;
        DCSeek( f, start, DIG_ORG );
        if( dbg_head.vendor == TIS_TRAILER_VENDOR_TIS
         && dbg_head.type == TIS_TRAILER_TYPE_TIS_DWARF )break;
    }
    // read elf header find dwarf info
    if( DCRead( f, &elf_head, sizeof( elf_head )) != sizeof( elf_head ) ){
        return( DS_FAIL );
    }
    if( memcmp( elf_head.e_ident, ELF_SIGNATURE, ELF_SIGNATURE_LEN ) ) {
        return( DS_FAIL );
    }

    // grab the string table, if it exists
    if( !elf_head.e_shstrndx ){
        return( DS_FAIL );  // no strings no dwarf
    }
    if( elf_head.e_shnum == 0 ){
        return( DS_FAIL ); // no sections no dwarf
    }
    memset( bases, 0, DR_DEBUG_NUM_SECTS * sizeof(unsigned_32) );
    memset( sizes, 0, DR_DEBUG_NUM_SECTS * sizeof(unsigned_32) );
    offset = elf_head.e_shoff
           + elf_head.e_shstrndx * elf_head.e_shentsize+start;
    DCSeek( f, offset, DIG_ORG );
    DCRead( f, &elf_sec, sizeof( Elf32_Shdr ) );
    string_table = DCAlloc( elf_sec.sh_size );
    DCSeek( f, elf_sec.sh_offset + start, DIG_ORG );
    DCRead( f, string_table, elf_sec.sh_size );
    for( i = 0; i < elf_head.e_shnum; i++ ) {
        DCSeek( f, elf_head.e_shoff + i * elf_head.e_shentsize + start, DIG_ORG );
        DCRead( f, &elf_sec, sizeof( Elf32_Shdr ) );
        if( elf_sec.sh_type == SHT_PROGBITS ){
            sect = Lookup_section_name( &string_table[elf_sec.sh_name] );
            if ( sect < DR_DEBUG_NUM_SECTS ){
                bases[sect] = elf_sec.sh_offset + start;
                sizes[sect] = elf_sec.sh_size;
            }
        }
    }
    DCFree( string_table );
    if( sizes[DR_DEBUG_INFO] == 0
     || sizes[DR_DEBUG_ABBREV] == 0
     || sizes[DR_DEBUG_ARANGES] == 0 ) {
        /* NOTE: aranges shouldn't be required to work, but currently is. */
        return( DS_FAIL );
    }
    return( DS_OK );
}

static void DWRRead( imp_image f,  uint sect,  void *buff, int size ) {
/******************************************************************/
    uint_32    base;

    base = f->dwarf->sect_offsets[sect];
    DCRead( f->sym_file, buff, size );

}

static void DWRSeek( imp_image  f, uint sect, long offs ) {
/******************************************************/
    long base;

    base = f->dwarf->sect_offsets[sect];
    DCSeek( f->sym_file, offs+base, DIG_ORG );
}

static void *DWRAlloc( size_t size ){
/***********************************/
    return( DCAlloc( size ) );
}

static void *DWRRealloc( void *what, size_t size ){
/***********************************/
    return( DCRealloc( what, size ) );
}

static void DWRFree( void *what ){
/*********************************/
    DCFree( what );
}


static void DWRErr( dr_except code ){
/************************************/
    switch( code ){
    case DREXCEP_OUT_OF_VM:
        DCStatus( DS_ERR | DS_NO_READ_MEM );
        break;
    case DREXCEP_BAD_DBG_VERSION:
        DCStatus( DS_ERR |DS_INFO_BAD_VERSION );
        break;
    case DREXCEP_BAD_DBG_INFO:
        DCStatus( DS_ERR |DS_INFO_INVALID );
        break;
    case DREXCEP_OUT_OF_MMEM:        // DWRMALLOC or DWRREALLOC failed
        DCStatus( DS_ERR | DS_NO_MEM );
        break;
    case DREXCEP_DWARF_LIB_FAIL:     // bug in the dwarf library
        DCStatus( DS_ERR  );
        break;
    }
}

DWRSetRtns( DWRRead, DWRSeek, DWRAlloc, DWRRealloc, DWRFree, DWRErr );

static dip_status InitDwarf( imp_image_handle *ii ){
/**************************************************/
    unsigned long    sect_sizes[DR_DEBUG_NUM_SECTS];
    dwarf_info      *dwarf;
    dip_status       ret;

    dwarf = DCAlloc( sizeof( *dwarf ) );
    ii->dwarf = dwarf;
    if( dwarf == NULL ){
        ret = DS_ERR | DS_NO_MEM;
        DCStatus( ret );
        goto error_exit;
    }
    ret = GetSectInfo( ii->sym_file, sect_sizes, dwarf->sect_offsets );
    if( ret != DS_OK )goto error_exit;
    dwarf->handle = DRDbgInitNFT( ii, sect_sizes );
    if( dwarf->handle == NULL ){
        ret = DS_ERR | DS_NO_MEM;
        DCStatus( ret );
        goto error_exit;
    }
    if( sect_sizes[DR_DEBUG_PUBNAMES] > 0 ){
        ii->has_pubnames = TRUE;
    }else{
        ii->has_pubnames = FALSE;
    }
    return( ret );
error_exit:
    if( dwarf != NULL ){
        DCFree( dwarf );
        ii->dwarf = NULL;
    }
    return( ret );
}

static void FiniDwarf( imp_image_handle *ii ){
/**************************************************/
    dwarf_info      *dwarf;

    dwarf = ii->dwarf;
    if( dwarf != NULL ){
        DRDbgDone( dwarf->handle ); /* free the sections */
        DRDbgFini( dwarf->handle );
        DCFree( dwarf );
        ii->dwarf = NULL;
//      DRFini();
    }
}
/*
        Loading/unloading symbolic information.
*/

static int APubName( imp_image_handle *ii,
                     dr_pubname_data  *curr ){
//*************************************************
// Add name from pubdefs to global name hash
//*************************************************
    if( curr->is_start ){
        SetModPubNames( ii, curr->dbg_cu );
    }
    AddHashName( ii->name_map, curr->name, curr->dbg_handle );
    return( TRUE );
}

static int AModHash( dr_handle           sym,
                     imp_image_handle *ii,
                     dr_search_context  *cont ){
//*******************************************
// Add any global symbol to the hash
//******************************************
    int     len;
    char    buff[256];

    cont = cont;
    if( !DRIsFunctionStatic( sym ) ){
        len =  DRGetNameBuff( sym, buff, sizeof(buff) );
        AddHashName( ii->name_map, buff, sym );
    }
    return( TRUE );
}

static walk_result ModGlbSymHash( imp_image_handle *ii,
                          im_idx            imx,
                          void             *d   ){
//*******************************************
// Add module's global syms to the name hash
//*******************************************
    dr_handle       cu_tag;

    d=d;
    cu_tag = ii->mod_map[imx].cu_tag;
    DRWalkModFunc( cu_tag, FALSE, AModHash, ii ); /* load hash */
    return( WR_CONTINUE );
}

static void LoadGlbHash( imp_image_handle *ii ){
//**********************************************
// Load a name hash of all the gobal symbols
//**********************************************
    DRSetDebug( ii->dwarf->handle ); /* must do at each interface */
    if( ii->has_pubnames ){
        DRWalkPubName( APubName, ii );
        DFWalkModListSrc( ii, FALSE, ModGlbSymHash, NULL );
    }else{  /* big load up */
        DFWalkModList( ii, ModGlbSymHash, NULL );
    }
}

dip_status      DIPENTRY DIPImpLoadInfo( dig_fhandle file, imp_image_handle *ii )
{
    dip_status          ret;

    if( file == DIG_NIL_HANDLE ) {
        DCStatus( DS_ERR|DS_FOPEN_FAILED );
        return( DS_ERR|DS_FOPEN_FAILED );
    }
    ii->sym_file = file;
    ret = InitDwarf( ii );
    if( ret == DS_OK ){
        ret = InitModMap( ii );
        if( ret == DS_OK ){
            InitImpCueInfo( ii );
            ii->name_map = InitHashName();
            LoadGlbHash( ii );
            ii->dcmap = NULL;
            InitScope( &ii->scope );
            DFAddImage( ii );
        }
    }
    return( ret );
}

typedef struct{
    imp_image_handle  *ii;
    addr_off        low_pc;
    addr_off        high_pc;
    im_idx imx;
}a_walk_info;

static int ARangeItem( a_walk_info  *info, dr_arange_data *curr ){
    off_info addr_info;
    uint_16  seg;
    imp_image_handle *ii;

    ii = info->ii;
    if( curr->is_start ){
        info->imx  = Dwarf2ModIdx( ii, curr->dbg );
        if( info->imx == INVALID_IMX ){
            return( TRUE );
        }
        if( ii->mod_map[info->imx].is_segment ){
            info->low_pc = 0;
            info->high_pc = 0;
        }else{
            DRGetLowPc( ii->mod_map[info->imx].cu_tag , &info->low_pc );
            DRGetHighPc( ii->mod_map[info->imx].cu_tag , &info->high_pc );
        }
    }
    if( curr->seg_size != 0 ){ /* reset because we know better */
        ii->mod_map[info->imx].is_segment = TRUE;
    }
    if( ii->mod_map[info->imx].is_segment ){
        seg = curr->seg;
    }else{
        seg = SEG_FLAT;
    }
    addr_info.imx  = info->imx;
    addr_info.map_seg =  seg;
    addr_info.map_offset =  curr->addr;
    addr_info.len = curr->len;
    AddMapAddr( ii->addr_map, ii->dcmap, &addr_info );
    return( TRUE );
}


void    DIPENTRY DIPImpMapInfo( imp_image_handle *ii, void *d ){
//*************************************************************
//  Read in a ranges build map
    a_walk_info info;

    ii->dcmap = d;
    DRSetDebug( ii->dwarf->handle ); /* set dwarf to image */
    InitAddrInfo( ii->addr_map );
    info.ii = ii;
    info.low_pc = 0;
    info.high_pc = 0;
    DRWalkARange( ARangeItem, &info );
    SortMapAddr( ii->addr_map );
    DRDbgClear( ii->dwarf->handle ); /* clear some memory */
    ii->last.len = 0;
    ii->last.imx = 0;
    ii->last.mach.segment = 0;
    ii->last.mach.offset = 0;
}

void            DIPENTRY DIPImpUnloadInfo( imp_image_handle *ii )
{
    FiniDwarf( ii );
    DCClose( ii->sym_file );

    FiniAddrInfo( ii->addr_map );
    FiniImpCueInfo( ii );
    FiniModMap( ii );
    FiniHashName( ii->name_map );
    FiniScope( &ii->scope );
    DFFreeImage( ii );
}
