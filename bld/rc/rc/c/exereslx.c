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
* Description:  LX resource manipulation routines.
*
****************************************************************************/


#include <assert.h>
#include <time.h>
#include "global.h"
#include "rcstrblk.h"
#include "rcerrors.h"
#include "os2res.h"
#include "rcrtns.h"
#include "rccore.h"
#include "mergedir.h"
#include "exeutil.h"
#include "exereslx.h"


static int CompareLXResIdName( const void *e1, const void *e2 )
/*************************************************************/
{
#define LXE(x) ((const LXResEntry *)(x))
    if( LXE( e1 )->resource.type_id == LXE( e2 )->resource.type_id ) {
        return( LXE( e1 )->resource.name_id - LXE( e2 )->resource.name_id );
    } else {
        return( LXE( e1 )->resource.type_id - LXE( e2 )->resource.type_id );
    }
#undef LXE
} /* CompareLXResIdName */


static bool addRes( LXResTable *res, WResDirWindow wind )
/*******************************************************/
{
    LXResEntry      *new_entry;
    WResResInfo     *resinfo;
    WResLangInfo    *res_lang;
    WResTypeInfo    *res_type;

    res_type = WResGetTypeInfo( wind );

    // RT_DEFAULTICON is not written into the executable, ignore
    if( res_type->TypeName.ID.Num == OS2_RT_DEFAULTICON )
        return( false );

    /* realloc resource table if necessary (with 32 entries per increment
     * we won't need to realloc too often)
     */
    res->res_count++;
    if( sizeof( LXResEntry ) * res->res_count > res->table_size ) {
        LXResEntry      *curr_table;

        curr_table = res->resources;
        res->resources = RCREALLOC( res->resources, ( res->res_count + 32 ) * sizeof( LXResEntry ) );
        if( res->resources == NULL ) {
            res->resources = curr_table;
            return( true );
        }
    }

    resinfo  = WResGetResInfo( wind );
    res_lang = WResGetLangInfo( wind );

    /* add new resource entry into table */
    new_entry = &res->resources[res->res_count - 1];

    new_entry->wind = wind;
    new_entry->mem_flags = res_lang->MemoryFlags;
    new_entry->assigned = false;
    new_entry->resource.res_size = res_lang->Length;

    assert( !resinfo->ResName.IsName );
    assert( !res_type->TypeName.IsName );

    new_entry->resource.type_id = res_type->TypeName.ID.Num;
    new_entry->resource.name_id = resinfo->ResName.ID.Num;
    new_entry->resource.object = 0;
    new_entry->resource.offset = res_lang->Offset;

    return( false );
}


static bool LXResTableBuild( LXResTable *res, WResDir dir )
/*********************************************************/
{
    WResDirWindow   wind;

    res->num_objects = 0;
    res->num_pages   = 0;
    if( WResIsEmpty( dir ) ) {
        res->resources  = NULL;
        res->table_size = 0;
        res->res_count  = 0;
    } else {
        for( wind = WResFirstResource( dir ); !WResIsEmptyWindow( wind ); wind = WResNextResource( wind, dir ) ) {
            if( addRes( res, wind ) ) {
                return( true );
            }
        }
    }
    return( false );
}


static void reportDuplicateResources( WResMergeError *errs )
/**********************************************************/
{
    WResMergeError  *curerr;
    ResFileInfo     *file1;
    ResFileInfo     *file2;
    WResResInfo     *resinfo;
    WResTypeInfo    *typeinfo;

    for( curerr = errs; curerr != NULL; curerr = curerr->next ) {
        resinfo = WResGetResInfo( curerr->dstres );
        typeinfo = WResGetTypeInfo( curerr->dstres );
        file1 = WResGetFileInfo( curerr->dstres );
        file2 = WResGetFileInfo( curerr->srcres );
        ReportDupResource( &resinfo->ResName, &typeinfo->TypeName, file1->name, file2->name, false );
    }
}


RcStatus WriteLXResourceObjects( ExeFileInfo *exe, ResFileInfo *info )
/********************************************************************/
{
    RcStatus        ret;
    WResLangInfo    *res_info;
    LXResTable      *dir;
    LXResEntry      *entry;
    lx_map_entry    *map;
    object_record   *object;
    uint_32         file_offset;
    uint_32         page_offset;
    uint_32         page_index;
    uint_32         obj_index;
    uint_32         padded_size;
    uint_32         padded_res_size;
    int             page_shift;
    unsigned        i;

    dir = &exe->u.LXInfo.Res;

    obj_index  = (uint_32)-1;
    page_index = exe->u.LXInfo.FirstResPage;
    page_shift = exe->u.LXInfo.OS2Head.l.page_shift;

    // Determine starting offset - expects that DebugOffset is pointing where
    // resources should be (current end of executable)
    file_offset = exe->DebugOffset;

    page_offset = 0;
    padded_size = 0;
    object      = NULL;
    map         = NULL;

    for( i = 0; i < exe->u.LXInfo.OS2Head.num_rsrcs; ++i ) {
        entry = &dir->resources[i];

        // Fill in new object
        if( obj_index != entry->resource.object ) {
            // Align page in output executable
            if( file_offset != ((file_offset >> page_shift) << page_shift) ) {
                file_offset = ((file_offset >> page_shift) + 1) << page_shift;
            }
            obj_index = entry->resource.object;
            object = &exe->u.LXInfo.Objects[exe->u.LXInfo.FirstResObj + obj_index];
            object->size     = 0;
            object->addr     = 0;
            object->flags    = OBJ_READABLE | OBJ_RESOURCE | OBJ_DISCARDABLE
                             | OBJ_BIG | OBJ_SHARABLE;
            object->mapidx   = page_index + 1;
            object->mapsize  = 1;
            object->reserved = 0;

            // Point to associated page table entry
            map = &exe->u.LXInfo.Pages[page_index];
            padded_size = 0;
            page_offset = file_offset;
        }
        entry->resource.object += exe->u.LXInfo.FirstResObj + 1;

        // Copy resource data
        if( RESSEEK( exe->fp, file_offset, SEEK_SET ) )
            return( RS_WRITE_ERROR );

        res_info = WResGetLangInfo( entry->wind );
        if( RESSEEK( info->fp, res_info->Offset, SEEK_SET ) )
            return( RS_READ_ERROR );

        ret = CopyExeData( info->fp, exe->fp, res_info->Length );
        if( ret != RS_OK ) {
            return( ret );
        }

        padded_res_size = ALIGN_VALUE( entry->resource.res_size, sizeof( uint_32 ) );
        padded_size  += padded_res_size;
        file_offset  += padded_res_size;
        object->size += padded_res_size;

        // Write padding if necessary (this is critical)
        if( padded_res_size > entry->resource.res_size ) {
            RcPadFile( exe->fp, padded_res_size - entry->resource.res_size );
        }

        // Update page table
        map->page_offset = (page_offset - exe->u.LXInfo.OS2Head.page_off) >> page_shift;
        map->flags       = 0;
        while( padded_size > OSF_DEF_PAGE_SIZE ) {
            map->page_offset = (page_offset - exe->u.LXInfo.OS2Head.page_off) >> page_shift;
            map->data_size   = OSF_DEF_PAGE_SIZE;

            padded_size -= OSF_DEF_PAGE_SIZE;
            page_offset += OSF_DEF_PAGE_SIZE;
            ++map;
            object->mapsize++;
            map->page_offset = (page_offset - exe->u.LXInfo.OS2Head.page_off) >> page_shift;
            map->flags       = 0;
        }
        map->data_size = padded_size;
    }
    CheckDebugOffset( exe );
    return( RS_OK );
}


bool BuildLXResourceObjects( ExeFileInfo *exeinfo, ResFileInfo *resinfo,
                                   object_record *res_obj, unsigned_32 rva,
                                   unsigned_32 offset, bool writebyfile )
/**************************************************************************/
{
    LXResTable      *dir;
    WResMergeError  *errs;
    unsigned        i;
    LXResEntry      *entry;
    unsigned_32     total = 0;
    unsigned_32     curr_total;
    unsigned_32     curr_offset;

    /* unused parameters */ (void)res_obj; (void)rva; (void)offset; (void)writebyfile;

    dir = &exeinfo->u.LXInfo.Res;

    MergeDirectory( resinfo, &errs );
    if( errs != NULL ) {
        reportDuplicateResources( errs );
        WResFreeMergeErrors( errs );
        return( true );
    }
    if( LXResTableBuild( dir, resinfo->Dir ) ) {
        RcError( ERR_INTERNAL, INTERR_ERR_BUILDING_RES_DIR );
        return( true );
    }

    /* OS/2 2.x requires resources to be sorted */
    qsort( dir->resources, dir->res_count, sizeof( LXResEntry ), CompareLXResIdName );

    /* Assign resources to objects/pages and figure out exactly
     * how many we'll need, in order to determine the exact size
     * of executable's header section.
     */
    curr_total = curr_offset = 0;
    dir->num_objects++;
    exeinfo->u.LXInfo.OS2Head.num_rsrcs = dir->res_count;
    for( i = 0; i < dir->res_count; ++i ) {
        unsigned_32     res_size;

        entry = &dir->resources[i];

        // IBM's RC appears to align resources on a 32-bit boundary
        res_size = ALIGN_VALUE( entry->resource.res_size, sizeof( uint_32 ) );
        total += res_size;
        curr_total += res_size;
        for( ; curr_total > OSF_DEF_PAGE_SIZE; curr_total -= OSF_DEF_PAGE_SIZE ) {
            dir->num_pages++;
        }

#ifndef NDEBUG
        printf( "    %d.%d (%d bytes)\n", entry->resource.name_id,
            entry->resource.type_id, entry->resource.res_size );
#endif

        /* FIXME? - we cheat and stuff everything in a single resource object */
        /* Maybe that's not a problem though. */
        entry->resource.object = 0;
        entry->resource.offset = curr_offset;
        entry->assigned = true;

        curr_offset += res_size;
    }
    if( curr_total )
        dir->num_pages++;

#ifndef NDEBUG
    printf( "total size: %d bytes in %d page(s)\n", total, dir->num_pages );
#endif

    return( false );
} /* BuildLXResourceObjects */


#ifndef INSIDE_WLINK
bool RcBuildLXResourceObjects( void )
/***************************************/
{
    object_record       *res_objects;
    bool                ret;
    ExeFileInfo         *exeinfo;

    exeinfo = &Pass2Info.TmpFile;
    if( CmdLineParms.NoResFile ) {
        exeinfo->u.LXInfo.OS2Head.num_rsrcs = 0;
        ret = false;
    } else {
        res_objects = exeinfo->u.LXInfo.Objects;
        ret = BuildLXResourceObjects( exeinfo, Pass2Info.ResFile,
                                        res_objects, 0, 0, //rva, offset,
                                        !Pass2Info.AllResFilesOpen );
    }
    if( CmdLineParms.WritableRes ) {
        // Not sure if setting the resource objects writable would work?
    }
    return( ret );
}

RcStatus RcWriteLXResourceObjects( void )
/***************************************/
{
    RcStatus            ret;
    ExeFileInfo         *exeinfo;

    exeinfo = &Pass2Info.TmpFile;
    if( CmdLineParms.NoResFile ) {
        // Nothing to do
        ret = RS_OK;
    } else {
        ret = WriteLXResourceObjects( exeinfo, Pass2Info.ResFile );
    }
    return( ret );
}
#endif
