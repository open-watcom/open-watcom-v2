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
* Description:  DWARF debug information loading.
*
****************************************************************************/


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

#include "clibext.h"


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

static uint Lookup_section_name( const char *name )
/******************************************/
{
    uint        sect;

    for( sect = 0 ; sect < DR_DEBUG_NUM_SECTS; sect++ ) {
        if( stricmp( SectionNames[sect], name ) == 0 ) {
            return sect;
        }
    }
    return( sect );
}


static void ByteSwapShdr( Elf32_Shdr *elf_sec, bool byteswap )
/************************************************************/
{
    if( byteswap ) {
        SWAP_32( elf_sec->sh_name );
        SWAP_32( elf_sec->sh_type );
        SWAP_32( elf_sec->sh_flags );
        SWAP_32( elf_sec->sh_addr );
        SWAP_32( elf_sec->sh_offset );
        SWAP_32( elf_sec->sh_size );
        SWAP_32( elf_sec->sh_link );
        SWAP_32( elf_sec->sh_info );
        SWAP_32( elf_sec->sh_addralign );
        SWAP_32( elf_sec->sh_entsize );
    }
}


static dip_status GetSectInfo( FILE *fp, unsigned long *sizes, unsigned long *bases, bool *byteswap )
/****************************************************************************************************
 * Fill in the starting offset & length of the dwarf sections
 */
{
    TISTrailer          dbg_head;
    Elf32_Ehdr          elf_head;
    Elf32_Shdr          elf_sec;
    unsigned long       offset;
    unsigned long       start;
    char                *string_table;
    int                 i;
    uint                sect;

    // Find TIS header seek to elf header
    if( DCSeek( fp, DIG_SEEK_POSBACK( sizeof( dbg_head ) ), DIG_SEEK_END ) )
        return( DS_FAIL );
    start = DCTell( fp );
    for( ;; ) {
        if( DCRead( fp, &dbg_head, sizeof( dbg_head ) ) != sizeof( dbg_head ) ) {
            return( DS_FAIL );
        }
        if( dbg_head.signature != TIS_TRAILER_SIGNATURE ) {
            /*
             * Seek to start of file and hope it's in ELF format
             */
            start = 0;
            DCSeek( fp, 0, DIG_SEEK_ORG );
            break;
        }
        start -= dbg_head.size - sizeof( dbg_head );
        DCSeek( fp, start, DIG_SEEK_ORG );
        if( dbg_head.vendor == TIS_TRAILER_VENDOR_TIS && dbg_head.type == TIS_TRAILER_TYPE_TIS_DWARF ) {
            break;
        }
    }
    // read elf header find dwarf info
    if( DCRead( fp, &elf_head, sizeof( elf_head ) ) != sizeof( elf_head ) ) {
        return( DS_FAIL );
    }
    if( memcmp( elf_head.e_ident, ELF_SIGNATURE, ELF_SIGNATURE_LEN ) != 0 ) {
        return( DS_FAIL );
    }
    if( elf_head.e_ident[EI_CLASS] == ELFCLASS64 ) {
        // no support yet
        return( DS_FAIL );
    }

    *byteswap = false;
#ifdef __BIG_ENDIAN__
    if( elf_head.e_ident[EI_DATA] == ELFDATA2LSB ) {
#else
    if( elf_head.e_ident[EI_DATA] == ELFDATA2MSB ) {
#endif
        *byteswap = true;
        SWAP_16( elf_head.e_type );
        SWAP_16( elf_head.e_machine );
        SWAP_32( elf_head.e_version );
        SWAP_32( elf_head.e_entry );
        SWAP_32( elf_head.e_phoff );
        SWAP_32( elf_head.e_shoff );
        SWAP_32( elf_head.e_flags );
        SWAP_16( elf_head.e_ehsize );
        SWAP_16( elf_head.e_phentsize );
        SWAP_16( elf_head.e_phnum );
        SWAP_16( elf_head.e_shentsize );
        SWAP_16( elf_head.e_shnum );
        SWAP_16( elf_head.e_shstrndx );
    }

    // grab the string table, if it exists
    if( !elf_head.e_shstrndx ) {
        return( DS_FAIL );  // no strings no DWARF
    }
    if( elf_head.e_shnum == 0 ) {
        return( DS_FAIL );  // no sections no DWARF
    }
    memset( bases, 0, DR_DEBUG_NUM_SECTS * sizeof( unsigned long ) );
    memset( sizes, 0, DR_DEBUG_NUM_SECTS * sizeof( unsigned long ) );
    offset = elf_head.e_shoff + elf_head.e_shstrndx * elf_head.e_shentsize + start;
    DCSeek( fp, offset, DIG_SEEK_ORG );
    DCRead( fp, &elf_sec, sizeof( Elf32_Shdr ) );
    ByteSwapShdr( &elf_sec, *byteswap );
    string_table = DCAlloc( elf_sec.sh_size );
    DCSeek( fp, elf_sec.sh_offset + start, DIG_SEEK_ORG );
    DCRead( fp, string_table, elf_sec.sh_size );
    for( i = 0; i < elf_head.e_shnum; i++ ) {
        DCSeek( fp, elf_head.e_shoff + i * elf_head.e_shentsize + start, DIG_SEEK_ORG );
        DCRead( fp, &elf_sec, sizeof( Elf32_Shdr ) );
        ByteSwapShdr( &elf_sec, *byteswap );
        sect = Lookup_section_name( &string_table[elf_sec.sh_name] );
        if( sect < DR_DEBUG_NUM_SECTS ) {
            bases[sect] = elf_sec.sh_offset + start;
            sizes[sect] = elf_sec.sh_size;
        }
    }
    DCFree( string_table );
    if( sizes[DR_DEBUG_INFO] == 0
      || sizes[DR_DEBUG_ABBREV] == 0
      || sizes[DR_DEBUG_ARANGES] == 0 ) {
        /*
         * NOTE: aranges shouldn't be required to work, but currently is.
         */
        return( DS_FAIL );
    }
    return( DS_OK );
}


static void DWRRead( void *_f, dr_section sect, void *buff, size_t size )
/***********************************************************************/
{
    imp_image       f = _f;
//    unsigned long   base;

    /* unused parameters */ (void)sect;

//    base = f->dwarf->sect_offsets[sect];
    DCRead( f->sym_fp, buff, size );
}


static void DWRSeek( void *_f, dr_section sect, long offs )
/*********************************************************/
{
    imp_image   f = _f;
    long        base;

    base = f->dwarf->sect_offsets[sect];
    DCSeek( f->sym_fp, offs + base, DIG_SEEK_ORG );
}


static void *DWRAlloc( size_t size )
/**********************************/
{
    return( DCAlloc( size ) );
}


static void *DWRRealloc( void *what, size_t size )
/************************************************/
{
    return( DCRealloc( what, size ) );
}


static void DWRFree( void *what )
/*******************************/
{
    DCFree( what );
}


static void DWRErr( dr_except code )
/**********************************/
{
#if !defined( NDEBUG ) && defined( __WATCOMC__ ) && defined( __386__ )
    /*
     * for easier debugging
     */
    __asm int 3;
#endif
    switch( code ) {
    case DREXCEP_OUT_OF_VM:
        DCStatus( DS_ERR | DS_NO_READ_MEM );
        break;
    case DREXCEP_BAD_DBG_VERSION:
        DCStatus( DS_ERR | DS_INFO_BAD_VERSION );
        break;
    case DREXCEP_BAD_DBG_INFO:
        DCStatus( DS_ERR | DS_INFO_INVALID );
        break;
    case DREXCEP_OUT_OF_MMEM:        // DWRMALLOC or DWRREALLOC failed
        DCStatus( DS_ERR | DS_NO_MEM );
        break;
    case DREXCEP_DWARF_LIB_FAIL:     // bug in the DWARF library
        DCStatus( DS_ERR  );
        break;
    }
}

DWRSetRtns( DWRRead, DWRSeek, DWRAlloc, DWRRealloc, DWRFree, DWRErr );

static dip_status InitDwarf( imp_image_handle *iih )
/**************************************************/
{
    unsigned long   sect_sizes[DR_DEBUG_NUM_SECTS];
    dwarf_info      *dwarf;
    dip_status      ds;

    ds = DS_ERR | DS_NO_MEM;
    dwarf = DCAlloc( sizeof( *dwarf ) );
    if( dwarf != NULL ) {
        iih->dwarf = dwarf;
        ds = GetSectInfo( iih->sym_fp, sect_sizes, dwarf->sect_offsets, &iih->is_byteswapped );
        if( ds == DS_OK ) {
            dwarf->handle = DRDbgInitNFT( iih, sect_sizes, iih->is_byteswapped );
            if( dwarf->handle != NULL ) {
                iih->has_pubnames = ( sect_sizes[DR_DEBUG_PUBNAMES] > 0 );
                return( ds );
            }
            ds = DS_ERR | DS_NO_MEM;
        }
    }
    if( dwarf != NULL ) {
        DCFree( dwarf );
    }
    iih->dwarf = NULL;
    DCStatus( ds );
    return( ds );
}


static void FiniDwarf( imp_image_handle *iih )
/********************************************/
{
    dwarf_info      *dwarf;

    dwarf = iih->dwarf;
    if( dwarf != NULL ) {
        DRDbgDone( dwarf->handle ); /* free the sections */
        DRDbgFini( dwarf->handle );
        DCFree( dwarf );
        iih->dwarf = NULL;
//      DRFini();
    }
}

/*
 * Loading/unloading symbolic information.
 */

static bool APubName( void *_iih, dr_pubname_data *curr )
/********************************************************
 * Add name from pubdefs to global name hash
 */
{
    imp_image_handle    *iih = _iih;

    if( curr->is_start ) {
        SetModPubNames( iih, curr->dbg_cu );
    }
    AddHashName( iih->name_map, curr->name, curr->dbg_handle );
    return( true );
}


static bool AModHash( drmem_hdl sym, void *_iih, dr_search_context *cont )
/*************************************************************************
 * Add any global symbol to the hash
 */
{
    imp_image_handle    *iih = _iih;
//    unsigned            len;
    char                buff[256];

    /* unused parameters */ (void)cont;

    if( !DRIsStatic( sym ) ) {
//        len = DRGetNameBuff( sym, buff, sizeof( buff ) );
        DRGetNameBuff( sym, buff, sizeof( buff ) );
        AddHashName( iih->name_map, buff, sym );
    }
    return( true );
}


static walk_result ModGlbSymHash( imp_image_handle *iih, imp_mod_handle imh, void *d )
/*************************************************************************************
 * Add module's global syms to the name hash
 */
{
    /* unused parameters */ (void)d;

    DRWalkModFunc( IMH2MODI( iih, imh )->cu_tag, false, AModHash, iih );   /* load hash */
    return( WR_CONTINUE );
}


static void LoadGlbHash( imp_image_handle *iih )
/***********************************************
 * Load a name hash of all the gobal symbols
 */
{
    DRSetDebug( iih->dwarf->handle );    /* must do at each interface */
    if( iih->has_pubnames ) {
        DRWalkPubName( APubName, iih );
        DFWalkModListSrc( iih, false, ModGlbSymHash, NULL );
    } else {                            /* big load up */
        DFWalkModList( iih, ModGlbSymHash, NULL );
    }
}


dip_status DIPIMPENTRY( LoadInfo )( FILE *fp, imp_image_handle *iih )
/*******************************************************************/
{
    dip_status          ds;

    iih->sym_fp = fp;
    ds = InitDwarf( iih );
    if( ds == DS_OK ) {
        ds = InitModMap( iih );
        if( ds == DS_OK ) {
            InitImpCueInfo( iih );
            iih->name_map = InitHashName();
            LoadGlbHash( iih );
            iih->dcmap = NULL;
            InitScope( &iih->scope );
            DFAddImage( iih );
            return( ds );
        }
    }
    iih->sym_fp = NULL;
    return( ds );
}


typedef struct {
    imp_image_handle    *iih;
    addr_off            low_pc;
    addr_off            high_pc;
    imp_mod_handle      imh;
} a_walk_info;

static bool ARangeItem( void *_info, dr_arange_data *arange )
/***********************************************************/
{
    a_walk_info         *info = _info;
    off_info            addr_info;
    uint_16             seg;
    imp_image_handle    *iih;
    mod_info            *modinfo;

    iih = info->iih;
    if( arange->is_start ) {
        info->imh = Dwarf2Mod( iih, arange->dbg );
        if( info->imh == IMH_NOMOD ) {
            return( false );
        }
    }
    modinfo = IMH2MODI( iih, info->imh );
    if( arange->is_start ) {
        if( modinfo->is_segment ) {
            info->low_pc = 0;
            info->high_pc = 0;
        } else {
            DRGetLowPc( modinfo->cu_tag, &info->low_pc );
            DRGetHighPc( modinfo->cu_tag, &info->high_pc );
        }
    }
    if( arange->seg_size != 0 ) { /* reset because we know better */
        modinfo->is_segment = true;
    }
    if( modinfo->is_segment ) {
        seg = arange->seg;
    } else {
        seg = SEG_FLAT;
    }
    addr_info.imh = info->imh;
    addr_info.map_seg = seg;
    addr_info.map_offset = arange->addr;
    addr_info.len = arange->len;
    AddMapAddr( iih->addr_map, iih->dcmap, &addr_info );
    return( true );
}


void DIPIMPENTRY( MapInfo )( imp_image_handle *iih, void *d )
/************************************************************
 * Read in address ranges and build map
 */
{
    a_walk_info     info;

    iih->dcmap = d;
    DRSetDebug( iih->dwarf->handle );    /* set DWARF to image */
    InitAddrInfo( iih->addr_map );
    info.iih = iih;
    info.low_pc = 0;
    info.high_pc = 0;
    DRWalkARange( ARangeItem, &info );
    SortMapAddr( iih->addr_map );
    DRDbgClear( iih->dwarf->handle );    /* clear some memory */
    iih->last.len = 0;
    iih->last.imh = IMH_NOMOD;
    iih->last.mach.segment = 0;
    iih->last.mach.offset = 0;
}


void DIPIMPENTRY( UnloadInfo )( imp_image_handle *iih )
/*****************************************************/
{
    FiniDwarf( iih );
    FiniAddrInfo( iih->addr_map );
    FiniImpCueInfo( iih );
    FiniModMap( iih );
    FiniHashName( iih->name_map );
    FiniScope( &iih->scope );
    DFFreeImage( iih );
}
