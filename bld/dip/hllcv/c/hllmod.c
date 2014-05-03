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
    IMP_MOD_WKR         *wk;
    void                *d;
    imp_mod_handle      im;
};

/*
 * Glue function for the module walk.
 */
static walk_result GlueModWalk( imp_image_handle *ii, hll_dir_entry *hdd,
                                void *d )
{
    struct glue_mod_walk *md = d;
    return( md->wk( ii, hdd->iMod, md->d ) );
}

/*
 * Walk modules.
 */
walk_result DIPENTRY DIPImpWalkModList( imp_image_handle *ii, IMP_MOD_WKR *wk,
                                        void *d )
{
    struct glue_mod_walk    md;
    walk_result             wr;

    md.wk = wk;
    md.d  = d;
    wr = hllWalkDirList( ii, hll_sstModule, &GlueModWalk, &md );
    if( wr == WR_CONTINUE ) {
        wr = wk( ii, MH_GBL, d );
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
static unsigned StripAndCopyName( unsigned_8 *name, char *buf, unsigned max )
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
    return( hllNameCopy( buf, start, max, end - start ) );
}

/*
 * Gets the module name.
 */
unsigned DIPENTRY DIPImpModName( imp_image_handle *ii, imp_mod_handle im,
                                 char *buf, unsigned max )
{
    hll_dir_entry *hdd;

    /* the fictive global module. */
    if( im == MH_GBL ) {
        return( hllNameCopy( buf, GBL_NAME, max, sizeof( GBL_NAME ) - 1 ) );
    }

    hdd = hllFindDirEntry( ii, im, hll_sstModule );
    if( hdd != NULL ) {
        void *mp = VMBlock( ii, hdd->lfo, hdd->cb );
        if( mp != NULL ) {
            switch ( ii->format_lvl ) {
            case HLL_LVL_NB00:
            case HLL_LVL_NB02://testme
                return( StripAndCopyName( &((cv3_module_16 *)mp)->name_len, buf, max ) );
            case HLL_LVL_NB00_32BIT:
                return( StripAndCopyName( &((cv3_module_32 *)mp)->name_len, buf, max ) );
            case HLL_LVL_NB04:
                return( StripAndCopyName( &((hll_module *)mp)->name_len, buf, max ) );
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
hll_ssr_cuinfo *GetCompInfo( imp_image_handle *ii, imp_mod_handle im )
{
    hll_dir_entry *hdd = hllFindDirEntry( ii, im, hll_sstSymbols );
    if( hdd != NULL ) {
        virt_mem        pos = hdd->lfo;
        const virt_mem  end = pos + hdd->cb;
        while( pos < end ) {
            hll_ssr_cuinfo *cuinfo = VMRecord( ii, pos, &pos, NULL );
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
char *DIPENTRY DIPImpModSrcLang( imp_image_handle *ii, imp_mod_handle im )
{
    hll_ssr_cuinfo *cuinfo = GetCompInfo( ii, im );
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
dip_status DIPENTRY DIPImpModInfo( imp_image_handle *ii, imp_mod_handle im,
                                   handle_kind hk )
{
    hll_dir_entry   *hdd;

    switch( hk ) {
    case HK_TYPE:
        hdd = hllFindDirEntry( ii, im, hll_sstTypes );
        break;
    case HK_CUE:
        hdd = hllFindDirEntry( ii, im, hll_sstHLLSrc );
        if( !hdd || !hdd->cb ) {
            hdd = hllFindDirEntry( ii, im, hll_sstSrcLnSeg );
        }
        if( !hdd || !hdd->cb ) {
            hdd = hllFindDirEntry( ii, im, hll_sstSrcLines );
        }
        break;
    case HK_SYM:
        hdd = hllFindDirEntry( ii, im, hll_sstSymbols );
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
    imp_mod_handle   im;
};

/*
 * ImpAddrMod worker called for each module subsection.
 */
static walk_result FindModNB04( imp_image_handle *ii, hll_dir_entry *hdd, void *d )
{
    struct find_mod     *args = d;
    address             *a = args->a;
    hll_module          *mp;
    hll_seginfo         *sp;
    unsigned             seg;

    mp = VMBlock( ii, hdd->lfo, hdd->cb );
    if( mp == NULL )
        return( WR_CONTINUE );

    if( mp->ovlNumber != a->sect_id )
        return( WR_CONTINUE );

    seg = 0;
    sp = &mp->SegInfo;
    for( ;; ) {
        address code;
        code.mach.segment = sp->Seg;
        code.mach.offset  = sp->offset;
        hllMapLogical( ii, &code );
        if( code.mach.segment == a->mach.segment
         && a->mach.offset - code.mach.offset <= sp->cbSeg) {
            args->im = hdd->iMod;
            return( WR_STOP );
        }

        /* next */
        if( ++seg >= mp->cSeg ) {
            break;
        }
        if( seg == 1 ) {
            sp = (hll_seginfo *)&mp->name[mp->name_len];
        } else {
            ++sp;
        }
    }

    return( WR_CONTINUE );
}

/*
 * Finds the module which contains the address 'a'.
 */
search_result hllAddrMod( imp_image_handle *ii, address a, imp_mod_handle *im )
{
    int             seg;

    /*
     * Check that the address is within the segment table.
     */
    for( seg = 0; seg < ii->seg_count; seg++ ) {
        if( ii->segments[seg].ovl == a.sect_id
         && ii->segments[seg].map.segment == a.mach.segment
         &&   a.mach.offset - ii->segments[seg].map.offset
            < ii->segments[seg].size ) {

            /*
             * Find the address among the module sections.
             * If not found in any, we return the global handle since
             * we know that the address is within the bounds of this image.
             */
            *im = MH_GBL;
            if( ii->segments[seg].is_executable ) {
                struct find_mod args;
                args.a = &a;
                if( hllWalkDirList( ii, hll_sstModule, FindModNB04, &args ) == WR_STOP ) {
                    *im = args.im;
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
search_result DIPENTRY DIPImpAddrMod( imp_image_handle *ii, address a,
                                      imp_mod_handle *im )
{
    return( hllAddrMod( ii, a, im ) );
}

/*
 * Gets the module address.
 */
address DIPENTRY DIPImpModAddr( imp_image_handle *ii, imp_mod_handle im )
{
    hll_dir_entry *hdd = hllFindDirEntry( ii, im, hll_sstModule );
    if( hdd != NULL) {
        union {
            void            *pv;
            hll_module      *hll;
            cv3_module_16   *cv3_16;
            cv3_module_32   *cv3_32;
        } m;
        m.pv = VMBlock( ii, hdd->lfo, hdd->cb );
        if( m.pv ) {
            address addr;

            switch( ii->format_lvl ) {
            case HLL_LVL_NB00:
            case HLL_LVL_NB02:
                if( m.cv3_16->cSeg > 0 ) {
                    addr.mach.segment = m.cv3_16->SegInfo.Seg;
                    addr.mach.offset  = m.cv3_16->SegInfo.offset;
                    hllMapLogical( ii, &addr );
                    return( addr );
                }
                break;

            case HLL_LVL_NB00_32BIT:
                if( m.cv3_16->cSeg > 0 ) {
                    addr.mach.segment = m.cv3_16->SegInfo.Seg;
                    addr.mach.offset  = m.cv3_16->SegInfo.offset;
                    hllMapLogical( ii, &addr );
                    return( addr );
                }
                break;

            case HLL_LVL_NB04:
                if( m.hll->cSeg > 0 ) {
                    addr.mach.segment = m.hll->SegInfo.Seg;
                    addr.mach.offset  = m.hll->SegInfo.offset;
                    hllMapLogical( ii, &addr );
                    return( addr );
                }
                break;
            }
        }
    }
    return( NilAddr );
}

/*
 * Construct default dip_type_info for a module.
 */
dip_status DIPENTRY DIPImpModDefault( imp_image_handle *ii, imp_mod_handle im,
                                      default_kind dk, dip_type_info *ti )
{
    /*
     * Get the module entry and figure if it's a 16-bit or 32-bit module.
     *
     * FIXME: We need to record the kind executable image, segment type and
     *        whatever else which could give of heuristics to decide here.
     */
    hll_dir_entry *hdd = hllFindDirEntry( ii, im, hll_sstModule );
    if( hdd == NULL && im != MH_GBL ) {
        return( DS_FAIL );
    }
    /* ASSUMES everything is 32-bit for now */

    ti->kind = TK_POINTER;
    ti->modifier = TM_NEAR;
    ti->size = sizeof( addr48_off );  /*FIXME: 16-bit: sizeof( addr32_off )*/
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
hll_style hllGetModStyle( imp_image_handle *ii, imp_mod_handle im )
{
    hll_dir_entry *hdd = hllFindDirEntry( ii, im, hll_sstModule );
    if( hdd ) {
        hll_module *mp = VMBlock( ii, hdd->lfo, hdd->cb );
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
    return( ii->is_32bit ? HLL_STYLE_HL04 : HLL_STYLE_CV00 );
}

