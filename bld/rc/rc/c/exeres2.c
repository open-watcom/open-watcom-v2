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
* Description:  NE resource manipulation routines, OS/2 version.
*
****************************************************************************/


#include "global.h"
#include "errors.h"
#include "rcmem.h"
#include "exeutil.h"
#include "iortns.h"
#include "os2res.h"


/* Note: IBM's OS/2 RC accepts string resource IDs/types but quietly
 * replaces all strings with zeros when writing out the resources to
 * NE modules. Strange thing to do, but we'll do the same.
 */


/* Build OS/2 NE style resource table. Each resource gets one entry, and
 * resources > 64K will get an additional entry for each 64K chunk.
 */
static void buildOS2ResTable( OS2ResTable *res_tbl, WResDir dir )
/***************************************************************/
{
    WResDirWindow   wind;
    WResLangInfo    *lang;
    OS2ResEntry     *entry;
    WResTypeInfo    *res_type;
    WResResInfo     *resinfo;
    uint_16         type;
    uint_16         id;
    int_32          length;

    entry = res_tbl->resources;

    /* Walk through the WRes directory */
    wind = WResFirstResource( dir );
    while( !WResIsEmptyWindow( wind ) ) {
        lang = WResGetLangInfo( wind );
        res_type = WResGetTypeInfo( wind );
        resinfo  = WResGetResInfo( wind );

        // RT_DEFAULTICON is not written into the executable, ignore
        if( res_type->TypeName.ID.Num == OS2_RT_DEFAULTICON ) {
            wind = WResNextResource( wind, dir );
            continue;
        }

        if( res_type->TypeName.IsName )
            type = 0;
        else
            type = res_type->TypeName.ID.Num;

        if( resinfo->ResName.IsName )
            id = 0;
        else
            id = resinfo->ResName.ID.Num;

        length = lang->Length;

        /* Fill in resource entries */
        entry->res_type   = type;
        entry->res_id     = id;
        entry->wind       = wind;
        entry->mem_flags  = lang->MemoryFlags;
        entry->seg_length = 0;  /* Zero means 64K */
        entry->first_part = TRUE;

        while( length > 0x10000 ) {
            length -= 0x10000;
            entry++;
            entry->res_type   = type;
            entry->res_id     = id;
            entry->wind       = wind;
            entry->mem_flags  = lang->MemoryFlags;
            entry->seg_length = 0;
            entry->first_part = FALSE;
        }
        entry->seg_length = lang->Length % 0x10000;

        wind = WResNextResource( wind, dir );
        entry++;
    }
}


static int compareOS2ResTypeId( const void * _entry1, const void * _entry2 )
/*************************************************************************/
{
    const LXResEntry *entry1 = _entry1;
    const LXResEntry *entry2 = _entry2;

    if( entry1->resource.type_id == entry2->resource.type_id ) {
        return( entry1->resource.name_id - entry2->resource.name_id );
    } else {
        return( entry1->resource.type_id - entry2->resource.type_id );
    }
} /* compareOS2ResTypeId */


extern RcStatus InitOS2ResTable( int *err_code )
/**********************************************/
{
    OS2ResTable         *res;
    WResDir             dir;

    res = &(Pass2Info.TmpFile.u.NEInfo.OS2Res);
    dir = Pass2Info.ResFiles->Dir;

    if( CmdLineParms.NoResFile ) {
        res->resources    = NULL;
        res->num_res_segs = 0;
        res->table_size   = 0;
    } else {
        res->num_res_segs = ComputeOS2ResSegCount( dir );
        /* One resource type/id record per resource segment, 16-bits each */
        res->table_size   = res->num_res_segs * 2 * sizeof( uint_16 );

        res->resources = RcMemMalloc( res->num_res_segs * sizeof( res->resources[0] ) );
        if( res->resources == NULL ) {
            *err_code = errno;
            return( RS_NO_MEM );
        }
        buildOS2ResTable( res, dir );

        /* OS/2 requires resources to be sorted */
        qsort( res->resources, res->num_res_segs,
                sizeof( res->resources[0] ), compareOS2ResTypeId );

    }
    return( RS_OK );
} /* InitOS2ResTable */


/* Compute the number of resource segments in an OS/2 NE module. Each
 * resource gets its own segment and resources > 64K will be split into
 * as many segments as necessary.
 */
extern uint_32 ComputeOS2ResSegCount( WResDir dir )
/*************************************************/
{
    uint_32         length;
    WResDirWindow   wind;
    WResTypeInfo    *res_type;
    WResLangInfo    *res;
    uint_32         num_res_segs;

    num_res_segs = 0;
    wind = WResFirstResource( dir );
    while( !WResIsEmptyWindow( wind ) ) {
        res = WResGetLangInfo( wind );
        res_type = WResGetTypeInfo( wind );

        // RT_DEFAULTICON is not written into the executable, ignore
        if( res_type->TypeName.ID.Num != OS2_RT_DEFAULTICON ) {
            ++num_res_segs;
            length = res->Length;
            while( length > 0x10000 ) {
                length -= 0x10000;
                ++num_res_segs;
            }
        }
        wind = WResNextResource( wind, dir );
    }
    return( num_res_segs );
} /* ComputeOS2ResSegCount */


/* NB: We copy resources in one go even if they span multiple segments.
 * This is fine because all segments but the last one are 64K big, and
 * hence will be nicely aligned.
 */
static RcStatus copyOneResource( WResLangInfo *lang, WResFileID reshandle,
            WResFileID outhandle, int shift_count, int *err_code )
/************************************************************************/
{
    RcStatus            error;
    long                out_offset;
    long                align_amount;

    /* align the output file to a boundary for shift_count */
    error = RS_OK;
    out_offset = RcTell( outhandle );
    if( out_offset == -1 ) {
        error = RS_WRITE_ERROR;
        *err_code = errno;
    }
    if( error == RS_OK ) {
        align_amount = AlignAmount( out_offset, shift_count );
        if( RcSeek( outhandle, align_amount, SEEK_CUR ) == -1 ) {
            error = RS_WRITE_ERROR;
            *err_code = errno;
        }
        out_offset += align_amount;
    }

    if( error == RS_OK ) {
        if( RcSeek( reshandle, lang->Offset, SEEK_SET ) == -1 ) {
            error = RS_READ_ERROR;
            *err_code = errno;
        }
    }
    if( error == RS_OK ) {
        error = CopyExeData( reshandle, outhandle, lang->Length );
        *err_code = errno;
    }
    if( error == RS_OK ) {
        align_amount = AlignAmount( RcTell( outhandle ), shift_count );
        error = PadExeData( outhandle, align_amount );
        *err_code = errno;
    }

    return( error );
} /* copyOneResource */


extern int CopyOS2Resources( void )
{
    OS2ResEntry         *entry;
    WResDirWindow       wind;
    OS2ResTable         *restab;
    WResLangInfo        *lang;
    int                 tmphandle;
    int                 reshandle;
    RcStatus            error;
    int                 err_code;
    int                 shift_count;
    int                 currseg;
    segment_record      *tmpseg;
    uint_32             seg_offset;
    long                align_amount;
    int                 i;

    restab    = &(Pass2Info.TmpFile.u.NEInfo.OS2Res);
    tmphandle = Pass2Info.TmpFile.Handle;
    reshandle = Pass2Info.ResFiles->Handle;
    tmpseg    = Pass2Info.TmpFile.u.NEInfo.Seg.Segments;
    currseg   = Pass2Info.OldFile.u.NEInfo.Seg.NumSegs
                - Pass2Info.OldFile.u.NEInfo.Seg.NumOS2ResSegs;
    entry     = restab->resources;
    error     = RS_OK;
    err_code  = 0;

    tmpseg += currseg;
    shift_count = Pass2Info.TmpFile.u.NEInfo.WinHead.align;
    seg_offset = 0;     // shut up gcc

    /* We may need to add padding before the first resource segment */
    align_amount = AlignAmount( RcTell( tmphandle ), shift_count );
    if( align_amount ) {
        error = PadExeData( tmphandle, align_amount );
        err_code = errno;
    }

    /* Walk through the resource entries */
    for( i = 0; i < restab->num_res_segs; i++, entry++, tmpseg++ ) {
        wind = entry->wind;
        lang = WResGetLangInfo( wind );

        if( entry->first_part ) {
            seg_offset = RcTell( tmphandle );
        } else {
            seg_offset += 0x10000;
        }

        /* Fill in segment structure */
        tmpseg->address = seg_offset >> shift_count;
        tmpseg->size    = entry->seg_length;
        tmpseg->min     = entry->seg_length;
        tmpseg->info    = SEG_DATA | SEG_READ_ONLY | SEG_LEVEL_3;
        if( entry->mem_flags & MEMFLAG_MOVEABLE )
            tmpseg->info |= SEG_MOVABLE;
        if( entry->mem_flags & MEMFLAG_PURE )
            tmpseg->info |= SEG_PURE;
        if( entry->mem_flags & MEMFLAG_PRELOAD )
            tmpseg->info |= SEG_PRELOAD;
        if( entry->mem_flags & MEMFLAG_DISCARDABLE )
            tmpseg->info |= SEG_DISCARD;

        /* For non-last segment of a resource, there's nothing to copy */
        if( !entry->first_part )
            continue;

        /* Copy resource data */
        error = copyOneResource( lang, reshandle, tmphandle,
                                shift_count, &err_code );

        if( error != RS_OK )
            break;

        CheckDebugOffset( &(Pass2Info.TmpFile) );
    }

    switch( error ) {
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_FILE, Pass2Info.TmpFile.name,
                 strerror( err_code ) );
        break;
    case RS_READ_ERROR:
        RcError( ERR_READING_RES, CmdLineParms.OutResFileName,
                 strerror( err_code ) );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, CmdLineParms.OutResFileName );
        break;
    default:
        break;
    }
    return( error );
} /* CopyOS2Resources */


/*
 * WriteOS2ResTable
 * NB when an error occurs this function must return without altering errno
 */
extern RcStatus WriteOS2ResTable( int handle, OS2ResTable *restab, int *err_code )
/********************************************************************************/
{
    int                         num_wrote;
    int                         error;
    uint_16                     res_type;
    uint_16                     res_id;
    int                         i;

    error = RS_OK;
    for( i = 0; i < restab->num_res_segs && error == RS_OK; i++ ) {
        res_type = restab->resources[i].res_type;
        res_id   = restab->resources[i].res_id;

        num_wrote = RcWrite( handle, &res_type, sizeof( uint_16 ) );
        if( num_wrote != sizeof( uint_16 ) ) {
            error = RS_WRITE_ERROR;
        } else {
            num_wrote = RcWrite( handle, &res_id, sizeof( uint_16 ) );
            if( num_wrote != sizeof( uint_16 ) ) {
                error = RS_WRITE_ERROR;
            }
        }
    }

    *err_code = errno;
    if( restab->resources != NULL ) {
        RcMemFree( restab->resources );
    }

    return( error );
} /* WriteOS2ResTable */
