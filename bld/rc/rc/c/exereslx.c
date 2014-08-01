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
#include "wio.h"
#include "global.h"
#include "rcstrblk.h"
#include "errors.h"
#include "os2res.h"
#include "wrmergdi.h"
#include "rcrtns.h"
#include "clibext.h"
#include "rccore.h"
#include "exeobj.h"
#include "exeutil.h"
#include "exereslx.h"


extern int RcPadFile( int, long );

static int CompareLXResIdName( const void * _entry1, const void * _entry2 )
/*************************************************************************/
{
    const LXResEntry *entry1 = _entry1;
    const LXResEntry *entry2 = _entry2;

    if( entry1->resource.type_id == entry2->resource.type_id ) {
        return( entry1->resource.name_id - entry2->resource.name_id );
    } else {
        return( entry1->resource.type_id - entry2->resource.type_id );
    }
} /* CompareLXResIdName */


static int addRes( LXResTable *res, WResDirWindow wind )
/******************************************************/
{
    LXResEntry      *new_entry;
    WResResInfo     *resinfo;
    WResLangInfo    *res_lang;
    WResTypeInfo    *res_type;

    res_type = WResGetTypeInfo( wind );

    // RT_DEFAULTICON is not written into the executable, ignore
    if( res_type->TypeName.ID.Num == OS2_RT_DEFAULTICON )
        return( FALSE );

    /* realloc resource table if necessary (with 32 entries per increment
     * we won't need to realloc too often)
     */
    res->res_count++;
    if( sizeof( LXResEntry ) * res->res_count > res->table_size ) {
        LXResEntry      *curr_table;

        curr_table = res->resources;
        res->resources = RCREALLOC( res->resources,
                            (res->res_count + 32) * sizeof( LXResEntry ) );
        if( res->resources == NULL ) {
            res->resources = curr_table;
            return( TRUE );
        }
    }

    resinfo  = WResGetResInfo( wind );
    res_lang = WResGetLangInfo( wind );

    /* add new resource entry into table */
    new_entry = &res->resources[res->res_count - 1];

    new_entry->wind = wind;
    new_entry->mem_flags = res_lang->MemoryFlags;
    new_entry->assigned  = FALSE;
    new_entry->resource.res_size = res_lang->Length;

    assert( !resinfo->ResName.IsName );
    assert( !res_type->TypeName.IsName );

    new_entry->resource.type_id = res_type->TypeName.ID.Num;
    new_entry->resource.name_id = resinfo->ResName.ID.Num;
    new_entry->resource.object = 0;
    new_entry->resource.offset = res_lang->Offset;

    return( FALSE );
}


static int LXResTableBuild( LXResTable *res, WResDir dir )
/********************************************************/
{
    WResDirWindow   wind;

    res->num_objects = 0;
    res->num_pages   = 0;
    if( WResIsEmpty( dir ) ) {
        res->resources  = NULL;
        res->table_size = 0;
        res->res_count  = 0;
    } else {
        wind = WResFirstResource( dir );
        while( !WResIsEmptyWindow( wind ) ) {
            if( addRes( res, wind ) )
                return( TRUE );
            wind = WResNextResource( wind, dir );
        }
    }
    return( FALSE );
}


static void reportDuplicateResources( WResMergeError *errs )
/**********************************************************/
{
    WResMergeError  *curerr;
    ResFileInfo     *file1;
    ResFileInfo     *file2;
    WResResInfo     *resinfo;
    WResTypeInfo    *typeinfo;

    curerr = errs;
    while( curerr != NULL ) {
        resinfo = WResGetResInfo( curerr->dstres );
        typeinfo = WResGetTypeInfo( curerr->dstres );
        file1 = WResGetFileInfo( curerr->dstres );
        file2 = WResGetFileInfo( curerr->srcres );
        ReportDupResource( &resinfo->ResName, &typeinfo->TypeName,
                           file1->name, file2->name, FALSE );
        curerr = curerr->next;
    }
}


// merge the directories of all the res files into one large directory
// stored on the first resfileinfo node
static int mergeDirectory( ResFileInfo *resfiles, WResMergeError **errs )
/***********************************************************************/
{
    ResFileInfo         *cur;

    if( errs != NULL )
        *errs = NULL;
    if( resfiles == NULL )
        return( FALSE );
    cur = resfiles->next;
    while( cur != NULL ) {
        if( WResMergeDirs( resfiles->Dir, cur->Dir, errs ) )
            return( TRUE );
        cur = cur->next;
    }
    return( FALSE );
}


extern int WriteLXResourceObjects( ExeFileInfo *exe, ResFileInfo *info )
/**********************************************************************/
{
    long            seek_rc;
    int             copy_rc;
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
    int             i;

    dir = &exe->u.LXInfo.Res;

    obj_index  = -1;
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
            object = &exe->u.LXInfo.Objects[ exe->u.LXInfo.FirstResObj + obj_index ];
            object->size     = 0;
            object->addr     = 0;
            object->flags    = OBJ_READABLE | OBJ_RESOURCE | OBJ_DISCARDABLE
                             | OBJ_BIG | OBJ_SHARABLE;
            object->mapidx   = page_index + 1;
            object->mapsize  = 1;
            object->reserved = 0;

            // Point to associated page table entry
            map = &exe->u.LXInfo.Pages[ page_index ];
            padded_size = 0;
            page_offset = file_offset;
        }
        entry->resource.object += exe->u.LXInfo.FirstResObj + 1;

        // Copy resource data
        seek_rc = RCSEEK( exe->Handle, file_offset, SEEK_SET );
        if( seek_rc == -1 )
            return( RS_WRITE_ERROR );

        res_info = WResGetLangInfo( entry->wind );
        seek_rc = RCSEEK( info->Handle, res_info->Offset, SEEK_SET );
        if( seek_rc == -1 )
            return( RS_READ_ERROR );

        copy_rc = CopyExeData( info->Handle, exe->Handle, res_info->Length );
        if( copy_rc != RS_OK ) {
            return( copy_rc );
        }

        padded_res_size = ALIGN_VALUE( entry->resource.res_size, sizeof( uint_32 ) );
        padded_size  += padded_res_size;
        file_offset  += padded_res_size;
        object->size += padded_res_size;

        // Write padding if necessary (this is critical)
        if( padded_res_size > entry->resource.res_size ) {
            RcPadFile( exe->Handle, padded_res_size - entry->resource.res_size );
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


extern int BuildLXResourceObjects( ExeFileInfo *exeinfo, ResFileInfo *resinfo,
                                   object_record *res_obj, unsigned_32 rva,
                                   unsigned_32 offset, int writebyfile )
/**************************************************************************/
{
    LXResTable      *dir;
    WResMergeError  *errs;
    int             i;
    LXResEntry      *entry;
    unsigned_32     total = 0;
    unsigned_32     curr_total;
    unsigned_32     curr_offset;

    res_obj = res_obj; rva = rva; offset = offset; writebyfile = writebyfile;

    dir = &exeinfo->u.LXInfo.Res;

    mergeDirectory( resinfo, &errs );
    if( errs != NULL ) {
        reportDuplicateResources( errs );
        WResFreeMergeErrors( errs );
        return( TRUE );
    }
    if( LXResTableBuild( dir, resinfo->Dir ) ) {
        RcError( ERR_INTERNAL, INTERR_ERR_BUILDING_RES_DIR );
        return( TRUE );
    }

    /* OS/2 2.x requires resources to be sorted */
    qsort( dir->resources, dir->res_count, sizeof( LXResEntry ),
            CompareLXResIdName );

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
        while( curr_total > OSF_DEF_PAGE_SIZE ) {
            dir->num_pages++;
            curr_total -= OSF_DEF_PAGE_SIZE;
        }

#ifndef NDEBUG
        printf( "    %d.%d (%d bytes)\n", entry->resource.name_id,
            entry->resource.type_id, entry->resource.res_size );
#endif

        /* FIXME? - we cheat and stuff everything in a single resource object */
        /* Maybe that's not a problem though. */
        entry->resource.object = 0;
        entry->resource.offset = curr_offset;
        entry->assigned = TRUE;

        curr_offset += res_size;
    }
    if( curr_total )
        dir->num_pages++;

#ifndef NDEBUG
    printf( "total size: %d bytes in %d page(s)\n", total, dir->num_pages );
#endif

    return( FALSE );
} /* BuildLXResourceObjects */


#ifndef INSIDE_WLINK
int RcBuildLXResourceObjects( void )
/**********************************/
{
    object_record       *res_objects;
    RcStatus            error;
    ExeFileInfo         *exeinfo;

    exeinfo = &Pass2Info.TmpFile;
    if( CmdLineParms.NoResFile ) {
        exeinfo->u.LXInfo.OS2Head.num_rsrcs = 0;
        error = RS_OK;
    } else {
        res_objects = exeinfo->u.LXInfo.Objects;
        error = BuildLXResourceObjects( exeinfo, Pass2Info.ResFiles,
                                        res_objects, 0, 0, //rva, offset,
                                        !Pass2Info.AllResFilesOpen );
    }
    if( CmdLineParms.WritableRes ) {
        // Not sure if setting the resource objects writable would work?
    }
    return( error );
}


int RcWriteLXResourceObjects( void )
/**********************************/
{
    RcStatus            error;
    ExeFileInfo         *exeinfo;

    exeinfo = &Pass2Info.TmpFile;
    if( CmdLineParms.NoResFile ) {
        // Nothing to do
        error = RS_OK;
    } else {
        error = WriteLXResourceObjects( exeinfo, Pass2Info.ResFiles );
    }
    return( error );
}
#endif
