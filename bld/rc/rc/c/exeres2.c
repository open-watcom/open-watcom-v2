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
* Description:  NE resource manipulation routines, OS/2 version.
*
****************************************************************************/


#include "global.h"
#include <errno.h>
#include "rcerrors.h"
#include "os2res.h"
#include "rcrtns.h"
#include "rccore_2.h"
#include "exeutil.h"
#include "exeres.h"


/*
 * Note: IBM's OS/2 RC accepts string resource IDs/types but quietly
 * replaces all strings with zeros when writing out the resources to
 * NE modules. Strange thing to do, but we'll do the same.
 */

static void buildOS2ResTable( OS2ResTable *restab, WResDir dir )
/***************************************************************
 * Build OS/2 NE style resource table. Each resource gets one entry, and
 * resources > 64K will get an additional entry for each 64K chunk.
 */
{
    WResDirWindow   wind;
    WResLangInfo    *langinfo;
    OS2ResEntry     *entry;
    WResTypeInfo    *typeinfo;
    WResResInfo     *resinfo;
    uint_16         type_id;
    uint_16         name_id;
    int_32          length;

    /*
     * Walk through the WRes directory
     */
    entry = restab->resources;
    for( wind = WResFirstResource( dir ); !WResIsEmptyWindow( wind ); wind = WResNextResource( wind, dir ) ) {
        langinfo = WResGetLangInfo( wind );
        typeinfo = WResGetTypeInfo( wind );
        resinfo  = WResGetResInfo( wind );
        /*
         * RT_DEFAULTICON is not written into the executable, ignore
         */
        if( typeinfo->TypeName.ID.Num == OS2_RT_DEFAULTICON ) {
            continue;
        }

        type_id = 0;
        if( !typeinfo->TypeName.IsName )
            type_id = typeinfo->TypeName.ID.Num;

        name_id = 0;
        if( !resinfo->ResName.IsName )
            name_id = resinfo->ResName.ID.Num;
        /*
         * Fill in resource entries
         */
        entry->res_type   = type_id;
        entry->res_name   = name_id;
        entry->wind       = wind;
        entry->mem_flags  = langinfo->MemoryFlags;
        entry->seg_length = 0;  /* Zero means 64K */
        entry->first_part = true;

        for( length = langinfo->Length; length > 0x10000; length -= 0x10000 ) {
            entry++;
            entry->res_type   = type_id;
            entry->res_name   = name_id;
            entry->wind       = wind;
            entry->mem_flags  = langinfo->MemoryFlags;
            entry->seg_length = 0;
            entry->first_part = false;
        }
        entry->seg_length = langinfo->Length % 0x10000;
        entry++;
    }
}


static int compareOS2ResTypeId( const void *e1, const void *e2 )
/**************************************************************/
{
#define LXRE(e) ((LXResEntry *)(e))
    if( LXRE(e1)->resource.type_id == LXRE(e2)->resource.type_id ) {
        return( LXRE(e1)->resource.name_id - LXRE(e2)->resource.name_id );
    } else {
        return( LXRE(e1)->resource.type_id - LXRE(e2)->resource.type_id );
    }
#undef LXRE
} /* compareOS2ResTypeId */


RcStatus InitOS2ResTable( ExeFileInfo *dst, ResFileInfo *res, int *err_code )
/***************************************************************************/
{
    OS2ResTable         *restab;
    WResDir             dir;

    restab = &(dst->u.NEInfo.OS2Res);
    dir = res->Dir;

    if( CmdLineParms.NoResFile ) {
        restab->resources    = NULL;
        restab->num_res_segs = 0;
        restab->table_size   = 0;
    } else {
        restab->num_res_segs = ComputeOS2ResSegCount( dir );
        /*
         * One resource type/id record per resource segment
         */
        restab->table_size   = restab->num_res_segs * sizeof( resource_table_record );

        restab->resources = RESALLOC( restab->num_res_segs * sizeof( restab->resources[0] ) );
        if( restab->resources == NULL ) {
            *err_code = errno;
            return( RS_NO_MEM );
        }
        buildOS2ResTable( restab, dir );
        /*
         * OS/2 requires resources to be sorted
         */
        qsort( restab->resources, restab->num_res_segs, sizeof( restab->resources[0] ), compareOS2ResTypeId );
    }
    return( RS_OK );
} /* InitOS2ResTable */


uint_32 ComputeOS2ResSegCount( WResDir dir )
/*******************************************
 * Compute the number of resource segments in an OS/2 NE module. Each
 * resource gets its own segment and resources > 64K will be split into
 * as many segments as necessary.
 */
{
    uint_32         length;
    WResDirWindow   wind;
    WResTypeInfo    *typeinfo;
    WResLangInfo    *langinfo;
    uint_32         num_res_segs;

    num_res_segs = 0;
    for( wind = WResFirstResource( dir ); !WResIsEmptyWindow( wind ); wind = WResNextResource( wind, dir ) ) {
        langinfo = WResGetLangInfo( wind );
        typeinfo = WResGetTypeInfo( wind );
        /*
         * RT_DEFAULTICON is not written into the executable, ignore
         */
        if( typeinfo->TypeName.ID.Num != OS2_RT_DEFAULTICON ) {
            ++num_res_segs;
            for( length = langinfo->Length; length > 0x10000; length -= 0x10000 ) {
                ++num_res_segs;
            }
        }
    }
    return( num_res_segs );
} /* ComputeOS2ResSegCount */


static RcStatus copyOneResource( WResLangInfo *langinfo, FILE *res_fp,
            FILE *dst_fp, int shift_count, int *err_code )
/*********************************************************************
 * NB: We copy resources in one go even if they span multiple segments.
 * This is fine because all segments but the last one are 64K big, and
 * hence will be nicely aligned.
 */
{
    RcStatus            ret;
    long                dst_offset;
    long                align_amount;

    /*
     * align the output file to a boundary for shift_count
     */
    ret = RS_OK;
    dst_offset = RESTELL( dst_fp );
    if( dst_offset == -1 ) {
        ret = RS_WRITE_ERROR;
        *err_code = errno;
    }
    if( ret == RS_OK ) {
        align_amount = AlignAmount( dst_offset, shift_count );
        if( RESSEEK( dst_fp, align_amount, SEEK_CUR ) ) {
            ret = RS_WRITE_ERROR;
            *err_code = errno;
        }
//        dst_offset += align_amount;
    }

    if( ret == RS_OK ) {
        if( RESSEEK( res_fp, langinfo->Offset, SEEK_SET ) ) {
            ret = RS_READ_ERROR;
            *err_code = errno;
        }
    }
    if( ret == RS_OK ) {
        ret = CopyExeData( res_fp, dst_fp, langinfo->Length );
        *err_code = errno;
    }
    if( ret == RS_OK ) {
        align_amount = AlignAmount( RESTELL( dst_fp ), shift_count );
        ret = PadExeData( dst_fp, align_amount );
        *err_code = errno;
    }

    return( ret );
} /* copyOneResource */


RcStatus CopyOS2Resources( ExeFileInfo *dst, ResFileInfo *res )
{
    OS2ResEntry         *entry;
    WResDirWindow       wind;
    WResLangInfo        *langinfo;
    RcStatus            ret;
    int                 err_code;
    int                 shift_count;
    segment_record      *dst_seg;
    uint_32             seg_offset;
    long                align_amount;
    int                 i;

    ret       = RS_OK;
    err_code  = 0;
    shift_count = dst->u.NEInfo.WinHead.align;
    /*
     * We may need to add padding before the first resource segment
     */
    align_amount = AlignAmount( RESTELL( dst->fp ), shift_count );
    if( align_amount ) {
        ret = PadExeData( dst->fp, align_amount );
        err_code = errno;
    }
    /*
     * Walk through the resource entries
     */
    seg_offset = 0;
    entry = dst->u.NEInfo.OS2Res.resources;
    dst_seg = dst->u.NEInfo.Seg.Segments + ( dst->u.NEInfo.Seg.NumSegs - dst->u.NEInfo.Seg.NumOS2ResSegs );
    for( i = 0; i < dst->u.NEInfo.OS2Res.num_res_segs; i++, entry++, dst_seg++ ) {
        wind = entry->wind;
        langinfo = WResGetLangInfo( wind );

        if( entry->first_part ) {
            seg_offset = RESTELL( dst->fp );
        } else {
            seg_offset += 0x10000;
        }
        /*
         * Fill in segment structure
         */
        dst_seg->address = seg_offset >> shift_count;
        dst_seg->size    = entry->seg_length;
        dst_seg->min     = entry->seg_length;
        dst_seg->info    = SEG_DATA | SEG_READ_ONLY | SEG_LEVEL_3;
        if( entry->mem_flags & MEMFLAG_MOVEABLE )
            dst_seg->info |= SEG_MOVABLE;
        if( entry->mem_flags & MEMFLAG_PURE )
            dst_seg->info |= SEG_PURE;
        if( entry->mem_flags & MEMFLAG_PRELOAD )
            dst_seg->info |= SEG_PRELOAD;
        if( entry->mem_flags & MEMFLAG_DISCARDABLE )
            dst_seg->info |= SEG_DISCARD;
        /*
         * For non-last segment of a resource, there's nothing to copy
         */
        if( !entry->first_part )
            continue;
        /*
         * Copy resource data
         */
        ret = copyOneResource( langinfo, res->fp, dst->fp, shift_count, &err_code );
        if( ret != RS_OK ) {
            break;
        }
    }
    CheckDebugOffset( dst );

    switch( ret ) {
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_FILE, dst->name, strerror( err_code ) );
        break;
    case RS_READ_ERROR:
        RcError( ERR_READING_RES, CmdLineParms.OutResFileName, strerror( err_code ) );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, CmdLineParms.OutResFileName );
        break;
    default:
        break;
    }
    return( ret );
} /* CopyOS2Resources */


RcStatus WriteOS2ResTable( FILE *fp, OS2ResTable *restab, int *err_code )
/************************************************************************
 * NB when an error occurs this function must return without altering errno
 */
{
    RcStatus                ret;
    resource_table_record   res_tab;
    int                     i;

    ret = RS_OK;
    for( i = 0; i < restab->num_res_segs && ret == RS_OK; i++ ) {
        res_tab.type = restab->resources[i].res_type;
        res_tab.name = restab->resources[i].res_name;
        if( RESWRITE( fp, &res_tab, sizeof( res_tab ) ) != sizeof( res_tab ) ) {
            ret = RS_WRITE_ERROR;
        }
    }

    *err_code = errno;
    if( restab->resources != NULL ) {
        RESFREE( restab->resources );
    }

    return( ret );
} /* WriteOS2ResTable */
