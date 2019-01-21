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
* Description:  HLL/CV module support.
*
****************************************************************************/


#include "hllinfo.h"


/* Arguments passed to GlueModWalk. */
struct glue_mod_walk {
    DIP_IMP_MOD_WALKER  *wk;
    void                *d;
    imp_mod_handle      imh;
};

/*
 * Glue function for the module walk.
 */
static walk_result GlueModWalk( imp_image_handle *iih, hll_dir_entry *hdd, void *d )
{
    struct glue_mod_walk *md = d;
    return( md->wk( iih, hdd->iMod, md->d ) );
}

/*
 * Walk modules.
 */
walk_result DIPIMPENTRY( WalkModList )( imp_image_handle *iih, DIP_IMP_MOD_WALKER *wk, void *d )
{
    struct glue_mod_walk    md;
    walk_result             wr;

    md.wk = wk;
    md.d  = d;
    wr = hllWalkDirList( iih, hll_sstModule, &GlueModWalk, &md );
    if( wr == WR_CONTINUE ) {
        wr = wk( iih, IMH_GBL, d );
    }
    return( wr );
}

//NYI: should be OS && location sensitive
#define IS_PATH_CHAR( c )   ( (c) == '\\' || (c) == '/' || (c) == ':')
#define EXT_CHAR            '.'

/* The name of the fictive global module. */
#define GBL_NAME            "__global"


/*
 * Strips path and extension from the name and copies into the specified buffer.
 */
static size_t StripAndCopyName( unsigned_8 *name, char *buff, size_t buff_size )
{
    char        *start = (char *)&name[1];
    char        *end   = start + *name;
    char        *cur   = end - 1;
    /* FIXME there should be generic functions for stripping this stuff! */

    /* strip path */
    for( cur = end - 1; cur >= start && !IS_PATH_CHAR( *cur ); cur-- )
        /* nothing */;
    start = cur + 1;

    /* strip extension */
    for( cur = end - 1; cur > start; cur-- ) {
        if( *cur == EXT_CHAR ) {
            end = cur;
            break;
        }
    }
    return( hllNameCopy( buff, start, buff_size, end - start ) );
}

/*
 * Gets the module name.
 */
size_t DIPIMPENTRY( ModName )( imp_image_handle *iih, imp_mod_handle imh,
                                 char *buff, size_t buff_size )
{
    hll_dir_entry *hdd;

    /* the fictive global module. */
    if( imh == IMH_GBL ) {
        return( hllNameCopy( buff, GBL_NAME, buff_size, sizeof( GBL_NAME ) - 1 ) );
    }

    hdd = hllFindDirEntry( iih, imh, hll_sstModule );
    if( hdd != NULL ) {
        void *mp = VMBlock( iih, hdd->lfo, hdd->cb );
        if( mp != NULL ) {
            switch ( iih->format_lvl ) {
            case HLL_LVL_NB00:
            case HLL_LVL_NB02://testme
                return( StripAndCopyName( &((cv3_module_16 *)mp)->name_len, buff, buff_size ) );
            case HLL_LVL_NB00_32BIT:
                return( StripAndCopyName( &((cv3_module_32 *)mp)->name_len, buff, buff_size ) );
            case HLL_LVL_NB04:
                return( StripAndCopyName( &((hll_module *)mp)->name_len, buff, buff_size ) );
            default:
                hllConfused();
            }
        }
    }
    return( 0 );
}

/*
 * Gets the compiler unit infomation record (unit = module, so for a module).
 */
hll_ssr_cuinfo *hllGetCompInfo( imp_image_handle *iih, imp_mod_handle imh )
{
    hll_dir_entry *hdd = hllFindDirEntry( iih, imh, hll_sstSymbols );
    if( hdd != NULL ) {
        virt_mem        pos = hdd->lfo;
        const virt_mem  end = pos + hdd->cb;
        while( pos < end ) {
            hll_ssr_cuinfo *cuinfo = VMRecord( iih, pos, &pos, NULL );
            if( cuinfo != NULL
             || cuinfo->common.code == HLL_SSR_CU_INFO ) {
                //FIXME: check CV3 format!
                return cuinfo;
            }
        }
    }
    return( NULL );
}

/*
 * Gets the source language for a module.
 */
char *DIPIMPENTRY( ModSrcLang )( imp_image_handle *iih, imp_mod_handle imh )
{
    hll_ssr_cuinfo *cuinfo = hllGetCompInfo( iih, imh );
    if( cuinfo != NULL ) {
        switch( cuinfo->language ) {
        case HLL_LANG_C:        return( "c" );
        case HLL_LANG_CPP:      return( "cpp" );
#if 1 /* additional */
        case HLL_LANG_PLX86:    return( "plx86" );
        case HLL_LANG_PL1:      return( "pl1" );
        case HLL_LANG_RPG:      return( "rpg" );
        case HLL_LANG_COBOL:    return( "cobol" );
        case HLL_LANG_ALP:      return( "asm" );
        case HLL_LANG_JAVA:     return( "java" );
#endif
        }
    }
    return( "c" );
}

/*
 * Checks if the specified information is available for this module.
 */
dip_status DIPIMPENTRY( ModInfo )( imp_image_handle *iih, imp_mod_handle imh,
                                   handle_kind hk )
{
    hll_dir_entry   *hdd;

    switch( hk ) {
    case HK_TYPE:
        hdd = hllFindDirEntry( iih, imh, hll_sstTypes );
        break;
    case HK_CUE:
        hdd = hllFindDirEntry( iih, imh, hll_sstHLLSrc );
        if( !hdd || !hdd->cb ) {
            hdd = hllFindDirEntry( iih, imh, hll_sstSrcLnSeg );
        }
        if( !hdd || !hdd->cb ) {
            hdd = hllFindDirEntry( iih, imh, hll_sstSrcLines );
        }
        break;
    case HK_SYM:
        hdd = hllFindDirEntry( iih, imh, hll_sstSymbols );
        break;

    case HK_IMAGE:
    default:
        return( DS_FAIL );
    }
    return( hdd && hdd->cb ? DS_OK : DS_FAIL );
}


/* Arguments passed to FindModNB04. */
struct find_mod {
    address         *a;
    imp_mod_handle  imh;
};

/*
 * ImpAddrMod worker called for each module subsection.
 */
static walk_result FindModNB04( imp_image_handle *iih, hll_dir_entry *hdd, void *d )
{
    struct find_mod     *args = d;
    address             *a = args->a;
    hll_module          *mp;
    hll_seginfo         *sp;
    unsigned             seg;

    mp = VMBlock( iih, hdd->lfo, hdd->cb );
    if( mp == NULL )
        return( WR_CONTINUE );

    if( mp->ovlNumber != a->sect_id )
        return( WR_CONTINUE );

    sp = &( mp->SegInfo );
    for( seg = 0; seg < mp->cSeg; ++seg ) {
        address code;
        code.mach.segment = sp->Seg;
        code.mach.offset  = sp->offset;
        hllMapLogical( iih, &code );
        if( code.mach.segment == a->mach.segment && a->mach.offset - code.mach.offset <= sp->cbSeg ) {
            args->imh = hdd->iMod;
            return( WR_STOP );
        }
        if( seg == 0 ) {
            sp = &( ((hll_module *)( (char *)mp + sizeof( hll_module ) + mp->name_len ))->SegInfo );
        } else {
            ++sp;
        }
    }

    return( WR_CONTINUE );
}

/*
 * Finds the module which contains the address 'a'.
 */
search_result hllAddrMod( imp_image_handle *iih, address a, imp_mod_handle *imh )
{
    unsigned    seg;

    /*
     * Check that the address is within the segment table.
     */
    for( seg = 0; seg < iih->seg_count; ++seg ) {
        if( iih->segments[seg].ovl == a.sect_id
          && iih->segments[seg].map.segment == a.mach.segment
          && a.mach.offset - iih->segments[seg].map.offset < iih->segments[seg].size ) {

            /*
             * Find the address among the module sections.
             * If not found in any, we return the global handle since
             * we know that the address is within the bounds of this image.
             */
            *imh = IMH_GBL;
            if( iih->segments[seg].is_executable ) {
                struct find_mod args;
                args.a = &a;
                if( hllWalkDirList( iih, hll_sstModule, FindModNB04, &args ) == WR_STOP ) {
                    *imh = args.imh;
                }
            }
            return( SR_CLOSEST );
        }
    }
    return( SR_NONE );
}

/*
 * Finds the module which contains the address 'a'.
 */
search_result DIPIMPENTRY( AddrMod )( imp_image_handle *iih, address a, imp_mod_handle *imh )
{
    return( hllAddrMod( iih, a, imh ) );
}

/*
 * Gets the module address.
 */
address DIPIMPENTRY( ModAddr )( imp_image_handle *iih, imp_mod_handle imh )
{
    hll_dir_entry *hdd = hllFindDirEntry( iih, imh, hll_sstModule );
    if( hdd != NULL) {
        union {
            void            *pv;
            hll_module      *hll;
            cv3_module_16   *cv3_16;
            cv3_module_32   *cv3_32;
        } m;
        m.pv = VMBlock( iih, hdd->lfo, hdd->cb );
        if( m.pv ) {
            address addr;

            switch( iih->format_lvl ) {
            case HLL_LVL_NB00:
            case HLL_LVL_NB02:
                if( m.cv3_16->cSeg > 0 ) {
                    addr.mach.segment = m.cv3_16->SegInfo.Seg;
                    addr.mach.offset  = m.cv3_16->SegInfo.offset;
                    hllMapLogical( iih, &addr );
                    return( addr );
                }
                break;

            case HLL_LVL_NB00_32BIT:
                if( m.cv3_16->cSeg > 0 ) {
                    addr.mach.segment = m.cv3_16->SegInfo.Seg;
                    addr.mach.offset  = m.cv3_16->SegInfo.offset;
                    hllMapLogical( iih, &addr );
                    return( addr );
                }
                break;

            case HLL_LVL_NB04:
                if( m.hll->cSeg > 0 ) {
                    addr.mach.segment = m.hll->SegInfo.Seg;
                    addr.mach.offset  = m.hll->SegInfo.offset;
                    hllMapLogical( iih, &addr );
                    return( addr );
                }
                break;
            }
        }
    }
    return( NilAddr );
}

/*
 * Construct default dig_type_info for a module.
 */
dip_status DIPIMPENTRY( ModDefault )( imp_image_handle *iih, imp_mod_handle imh,
                                      default_kind dk, dig_type_info *ti )
{
    /*
     * Get the module entry and figure if it's a 16-bit or 32-bit module.
     *
     * FIXME: We need to record the kind executable image, segment type and
     *        whatever else which could give of heuristics to decide here.
     */
    hll_dir_entry *hdd = hllFindDirEntry( iih, imh, hll_sstModule );
    if( hdd == NULL && imh != IMH_GBL ) {
        return( DS_FAIL );
    }
    /* ASSUMES everything is 32-bit for now */

    ti->kind = TK_POINTER;
    ti->size = sizeof( addr48_off );  /*FIXME: 16-bit: sizeof( addr32_off )*/
    ti->modifier = TM_NEAR;
    ti->deref = false;
    switch( dk ) {
    case DK_INT:
        ti->kind = TK_INTEGER;
        ti->modifier = TM_SIGNED;
        /* size is OK */
        break;
    case DK_DATA_PTR:
        /*if( comp_info->flags.f.AmbientData != AMBIENT_NEAR ) {
            ti->modifier = TM_FAR;
            ti->size += sizeof( addr_seg );
        } */
        break;
    case DK_CODE_PTR:
        /*if( comp_info->flags.f.AmbientCode != AMBIENT_NEAR ) {
            ti->modifier = TM_FAR;
            ti->size += sizeof( addr_seg );
        }*/
        break;
    }
    return( DS_OK );
}


/*
 * Gets the debug style of a module.
 */
hll_style hllGetModStyle( imp_image_handle *iih, imp_mod_handle imh )
{
    hll_dir_entry *hdd = hllFindDirEntry( iih, imh, hll_sstModule );
    if( hdd ) {
        hll_module *mp = VMBlock( iih, hdd->lfo, hdd->cb );
        if( mp != NULL ) {
            switch( mp->Style ) {
            case HLL_DEBUG_STYLE_HL:
                switch( mp->Version ) {
                case 0x100: return( HLL_STYLE_HL01 );
                case 0x200: return( HLL_STYLE_HL02 );
                case 0x300: return( HLL_STYLE_HL03 );
                case 0x400: return( HLL_STYLE_HL04 );
                case 0x500: return( HLL_STYLE_HL05 );
                case 0x600: return( HLL_STYLE_HL06 );
                default: hllConfused();
                }
                break;
            case HLL_DEBUG_STYLE_CV:
                switch( mp->Version ) {
                case 0x000: return( HLL_STYLE_CV00 );
                case 0x200: return( HLL_STYLE_HL02 );
                default: hllConfused();
                }
                break;
            }
        }
    }
    return( iih->is_32bit ? HLL_STYLE_HL04 : HLL_STYLE_CV00 );
}

