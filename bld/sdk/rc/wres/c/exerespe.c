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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "exerespe.h"

#define RESOURCE_OBJECT_NAME ".rsrc"

#ifndef MAKELANGID
#define MAKELANGID(p, s)       ( ( ( (uint_16)(s) ) << 10 ) | (uint_16)(p) )
#endif

/* align should be a power of 2 */
/* without the casts the macro reads: (value + (align-1)) & ~(align-1) */
#define ALIGN_VALUE( value, align ) ( (uint_32)( \
        ( (uint_32)(value) + ( (uint_32)(align) - 1 ) ) \
        & ~( (uint_32)(align) - 1 ) ) )

WResStatus WritePad(int handle, int size) {
    static char blank[200]; // automatically initialized to 0 by compiler
    for (; size > 200; size -= 200) {
        if (WRESWRITE(handle, blank, 200) != 200) {
            return WRS_WRITE_FAILED;
        }
    }
    if (WRESWRITE(handle, blank, size) != size) {
        return WRS_WRITE_FAILED;
    } else {
        return WRS_OK;
    }
}

WResStatus CopyData( int from, int to, int len ) {
    static char buff[200];
    for (; len > 200; len -= 200) {
        if (WRESREAD(from, buff, 200) != 200) return WRS_READ_FAILED;
        if (WRESWRITE(to, buff, 200) != 200) return WRS_WRITE_FAILED;
    }
    if (WRESREAD(from, buff, len) != len) return WRS_READ_FAILED;
    if (WRESWRITE(to, buff, len) != len) return WRS_WRITE_FAILED;
    return WRS_OK;
}


/* structures and routines to manipulate a queue of PEResDirEntry * */
/* This uses a linked list representation despite the overhead of the pointer */
/* since the total number of entries in the queue should be small and this is */
/* easier to code while still being dynamic */

typedef struct QueueNode {
    struct QueueNode *  next;
    PEResDirEntry *     entry;
} QueueNode;

typedef struct DirEntryQueue {
    QueueNode *     front;
    QueueNode *     back;
} DirEntryQueue;

static void QueueInit( DirEntryQueue * queue )
/********************************************/
{
    queue->front = NULL;
    queue->back = NULL;
} /* QueueInit */

static void QueueEmpty( DirEntryQueue * queue )
/*********************************************/
{
    QueueNode * curr;
    QueueNode * old;

    curr = queue->front;
    while( curr != NULL ) {
        old = curr;
        curr = curr->next;
        WRESFREE( old );
    }

    QueueInit( queue );
} /* QueueEmpty */

static int QueueIsEmpty( DirEntryQueue * queue )
/**********************************************/
{
    return( queue->front == NULL );
}

static void QueueAdd( DirEntryQueue * queue, PEResDirEntry * entry )
/******************************************************************/
{
    QueueNode *     new;

    new = WRESALLOC( sizeof(QueueNode) );
    new->entry = entry;
    new->next = NULL;
    if( queue->front == NULL ) {
        queue->front = new;
        queue->back = new;
    } else {
        queue->back->next = new;
        queue->back = new;
    }
} /* QueueAdd */

static PEResDirEntry * QueueRemove( DirEntryQueue * queue )
/*********************************************************/
{
    QueueNode *     old;
    PEResDirEntry * entry;

    old = queue->front;
    if( old == NULL ) {
        return( NULL );
    }

    queue->front = old->next;
    if( queue->front == NULL ) {
        queue->back = NULL;
    }

    entry = old->entry;
    WRESFREE( old );
    return( entry );
} /* QueueRemove */



static void PEResDirEntryInit( PEResDirEntry * entry, int num_entries )
/*********************************************************************/
{
    entry->Head.flags = 0;
    entry->Head.time_stamp = time( NULL );
    entry->Head.major = 0;
    entry->Head.minor = 0;
    entry->Head.num_name_entries = 0;
    entry->Head.num_id_entries = 0;
    entry->NumUnused = num_entries;
    entry->Children = WRESALLOC( num_entries * sizeof(PEResEntry) );
}

static void PEResDirAdd( PEResDirEntry * entry, WResID * name,
                    StringBlock * strings )
/***********************************************************/
{
    int             entry_num;
    int_32          name_off;
    PEResEntry *    curr;


    entry_num = entry->Head.num_name_entries + entry->Head.num_id_entries;
    curr = entry->Children + entry_num;
    if( name->IsName ) {
        name_off = StringBlockFind( strings, &name->ID.Name );
        if( name_off == -1 ) {
            /* this case should not happen */
            curr->Entry.id_name = PE_RESOURCE_MASK_ON | 0;
            curr->Name = NULL;
        } else {
            /* This value will be changed later when we know the size of the */
            /* the resource directory */
            curr->Entry.id_name = PE_RESOURCE_MASK_ON | name_off;
            curr->Name = (char *)strings->StringBlock + name_off;
        }
        entry->Head.num_name_entries++;
    } else {
        curr->Entry.id_name = name->ID.Num;
        curr->Name = NULL;
        entry->Head.num_id_entries++;
    }
    entry->NumUnused--;
}


static int PEResDirAddDir( PEResDirEntry * entry, WResID * name,
                    int num_sub_entries, StringBlock * strings )
/***************************************************************/
{
    int             entry_num;
    PEResEntry *    curr;

    if( entry->NumUnused <= 0 ) return( TRUE );

    PEResDirAdd( entry, name, strings );

    entry_num = entry->Head.num_name_entries + entry->Head.num_id_entries - 1;
    curr = entry->Children + entry_num;
    curr->IsDirEntry = TRUE;
    PEResDirEntryInit( &curr->Dir, num_sub_entries );

    return( FALSE );
}

static int PEResDirAddData( PEResDirEntry * entry, WResID * name,
                    WResDirWindow wind, StringBlock * strings )
/***************************************************************/
{
    int             entry_num;
    PEResEntry *    curr;

    if( entry->NumUnused <= 0 ) return( TRUE );

    PEResDirAdd( entry, name, strings );

    entry_num = entry->Head.num_name_entries + entry->Head.num_id_entries - 1;
    curr = entry->Children + entry_num;
    curr->IsDirEntry = FALSE;
    curr->Data.Wind = wind;
    /* The Data.Entry field will be filled in as the resource is writen */
    return( FALSE );
}

static int AddType( PEResDir * res, WResTypeInfo * type )
/*******************************************************/
{
    return( PEResDirAddDir( &res->Root, &type->TypeName, type->NumResources,
                                &res->String ) );
}

static int AddLang( PEResDir *res, WResDirWindow wind ) {
/*******************************************************/

    int                 entry_num;
    PEResEntry          *currres;
    PEResEntry          *currtype;
    WResLangInfo        *langinfo;
    WResID              lang_id;

    langinfo = WResGetLangInfo( wind );
    /* find the current type */
    entry_num = res->Root.Head.num_name_entries
                + res->Root.Head.num_id_entries - 1;
    currtype = res->Root.Children + entry_num;

    /* find the current resource */
    entry_num = currtype->Dir.Head.num_name_entries
                        + currtype->Dir.Head.num_id_entries - 1;
    currres = currtype->Dir.Children + entry_num;

    lang_id.IsName = FALSE;
    lang_id.ID.Num = MAKELANGID( langinfo->lang.lang, langinfo->lang.sublang );
    if( PEResDirAddData( &currres->Dir, &lang_id, wind,
                        &res->String ) ) {
        return( TRUE );
    }
    return( FALSE );
}

static int AddRes( PEResDir * res, WResDirWindow wind )
/*****************************************************/
{
    int             entry_num;
    PEResEntry *    currtype;
    WResResInfo *   resinfo;

    resinfo = WResGetResInfo( wind );

    /* find the current type */
    entry_num = res->Root.Head.num_name_entries + res->Root.Head.num_id_entries;
    currtype = res->Root.Children + entry_num - 1;

    /* Add a directory level for the languages */
    if( PEResDirAddDir( &currtype->Dir, &resinfo->ResName,
                        resinfo->NumResources, &res->String ) ) {
        return( TRUE );
    }

    return( FALSE );
}

/*
 * traverseTree
 * NB when a visit function returns an error this function MUST return
 *    without altering errno
 */
static WResStatus traverseTree( PEResDir * dir, void * visit_data,
                   WResStatus (*visit)( PEResEntry *, void * visit_data ) )
/*******************************************************************/
/* Perfroms a level order traversal of a PEResDir tree calling visit at */
/* each entry */
{
    PEResEntry *    curr_entry;
    PEResEntry *    last_child;
    PEResDirEntry * curr_dir;
    DirEntryQueue   queue;
    WResStatus      ret;

    QueueInit( &queue );

    QueueAdd( &queue, &dir->Root );

    while( !QueueIsEmpty( &queue ) ) {
        curr_dir = QueueRemove( &queue );
        last_child = curr_dir->Children + curr_dir->Head.num_name_entries +
                        curr_dir->Head.num_id_entries;
        curr_entry = curr_dir->Children;
        while( curr_entry < last_child ) {
            ret = visit( curr_entry, visit_data );
            if( ret != WRS_OK ) return( ret );
            if( curr_entry->IsDirEntry ) {
                QueueAdd( &queue, &curr_entry->Dir );
            }
            curr_entry++;
        }
    }

    QueueEmpty( &queue );

    return( WRS_OK );
} /* traverseTree */

static WResStatus SetEntryOffset( PEResEntry * entry, uint_32 * curr_offset )
/********************************************************************/
{
    int     num_entries;

    if( entry->IsDirEntry ) {
        entry->Entry.entry_rva = *curr_offset | PE_RESOURCE_MASK_ON;
        num_entries = entry->Dir.Head.num_name_entries +
                            entry->Dir.Head.num_id_entries;
        *curr_offset += sizeof(resource_dir_header) +
                            num_entries * sizeof(resource_dir_entry);
    } else {
        entry->Entry.entry_rva = *curr_offset;
        *curr_offset += sizeof(resource_entry);
    }
    return( WRS_OK );
} /* SetEntryOffset */

static WResStatus AdjustNameEntry( PEResEntry * entry, uint_32 * dir_size )
/******************************************************************/
{
    uint_32     str_offset;

    if( entry->Entry.id_name & PE_RESOURCE_MASK_ON ) {
        /* the id_name contains the offset into the string block */
        str_offset = entry->Entry.id_name & PE_RESOURCE_MASK;
        str_offset += *dir_size;
        entry->Entry.id_name = str_offset | PE_RESOURCE_MASK_ON;
    }

    return( WRS_OK );
} /* AdjustNameEntry */

static int ComparePEResIdName( PEResEntry * entry1, PEResEntry * entry2 )
{
    if( entry1->Name == NULL ) {
        if( entry2->Name == NULL ) {
            if( entry1->Entry.id_name > entry2->Entry.id_name ) {
                return( 1 );
            } else if( entry1->Entry.id_name < entry2->Entry.id_name ) {
                return( -1 );
            } else {
                return( 0 );
            }
        } else {
            return( 1 );
        }
    } else {
        if( entry2->Name == NULL ) {
            return( -1 );
        } else {
            return( CompareStringItems32( entry1->Name, entry2->Name ) );
        }
    }
} /* ComparePEResIdName */

static WResStatus SortDirEntry( PEResEntry * entry, void * dummy )
/*********************************************************/
{
    int     num_entries;

    dummy = dummy;

    if( entry->IsDirEntry ) {
        num_entries = entry->Dir.Head.num_name_entries +
                    entry->Dir.Head.num_id_entries;
        qsort( entry->Dir.Children, num_entries, sizeof(PEResEntry),
                    ComparePEResIdName );
    }
    return( WRS_OK );
} /* SortDirEntry */

static void CompleteTree( PEResDir * dir )
/****************************************/
{
    uint_32     curr_offset;
    int         num_entries;

    num_entries = dir->Root.Head.num_name_entries +
                                dir->Root.Head.num_id_entries;

    /* sort the entries at each level */
    qsort( dir->Root.Children, num_entries, sizeof(PEResEntry),
                ComparePEResIdName );
    traverseTree( dir, NULL, SortDirEntry );

    /* Set curr_offset to the size of the root entry */
    curr_offset = sizeof(resource_dir_header) +
                        num_entries * sizeof(resource_dir_entry);

    traverseTree( dir, &curr_offset, SetEntryOffset );
    dir->DirSize = curr_offset;


    traverseTree( dir, &curr_offset, AdjustNameEntry );
} /* CompleteTree */


int BuildPEResDir( PEResDir * res, WResDir dir )
{
    WResDirWindow   wind;

    if( WResIsEmpty( dir ) ) {
        res->Root.Head.num_name_entries = 0;
        res->Root.Head.num_id_entries = 0;
        res->Root.Children = NULL;
        res->ResRVA = 0;
        res->ResOffset = 0;
        res->ResSize = 0;
    } else {
        StringBlockBuild( &res->String, dir, -1 );
        res->DirSize = sizeof(resource_dir_header);
        PEResDirEntryInit( &res->Root, WResGetNumTypes( dir ) );
        wind = WResFirstResource( dir );
        while( !WResIsEmptyWindow( wind ) ) {
            if( WResIsFirstResOfType( wind ) ) {
                if( AddType( res, WResGetTypeInfo( wind ) ) ) return( -1 );
            }
            if( WResIsFirstLangOfRes( wind ) ) {
                if( AddRes( res, wind ) ) return( -1 );
            }
            AddLang( res, wind );
            wind = WResNextResource( wind, dir );
        }
    }
    CompleteTree( res );
    return( WRS_OK );
}

typedef struct CopyResInfo {
    int             to_handle;
    int             from_handle;
    pe_va           curr_rva;
} CopyResInfo;

/*
 * copyDataEntry: used by PutPEResources
 * NB when an error occurs this function MUST return without altering errno
 */
static WResStatus copyDataEntry( PEResEntry *entry, CopyResInfo *copy_info )
/*********************************************************************/
{
    WResLangInfo        *res_info;
    long                seekret;
    uint_32             diff;
    WResStatus          status;


    if( !entry->IsDirEntry ) {
        res_info = WResGetLangInfo( entry->Data.Wind );
        seekret = WRESSEEK( copy_info->from_handle, res_info->Offset, SEEK_SET );
        if( seekret == -1 ) return( WRS_SEEK_FAILED );
        status = CopyData( copy_info->from_handle, copy_info->to_handle,
                              res_info->Length );
        if( status != WRS_OK ) {
            return( status );
        }
        entry->Data.Entry.data_rva = copy_info->curr_rva;
        entry->Data.Entry.size = res_info->Length;
        entry->Data.Entry.code_page = 0;    /* should this be the UNICODE page*/
        entry->Data.Entry.rsvd = 0;
        diff = ALIGN_VALUE( res_info->Length, sizeof(uint_32) );
        copy_info->curr_rva += diff;
        if( diff != res_info->Length ) {
            /* add the padding */
            if (WritePad(copy_info->to_handle, diff-res_info->Length) != WRS_OK)
            {
                return WRS_WRITE_FAILED;
            }
        }
    }

    return( 0 );
} /* copyDataEntry */

/*
 * NB: info->rva and info->offset must be aligned to info->align!
 * NB: when an error occurs this function MUST return without altering errno
 */
WResStatus CopyPEResData(pResInOutInfo info, PEResDir *outRes) {
    CopyResInfo     copy_info;
    long            seekret;
    WResStatus      ret;
    long            rdirSize;

    rdirSize = outRes->DirSize + outRes->String.StringBlockSize;
    rdirSize = ALIGN_VALUE(rdirSize, sizeof(uint_32));
    outRes->ResOffset = info->offset;
    outRes->ResRVA = info->rva;

    copy_info.from_handle = info->res;
    copy_info.to_handle = info->obj;
    copy_info.curr_rva = info->rva + rdirSize;

    seekret = WRESSEEK( info->obj, info->offset+rdirSize, SEEK_SET );
    if( seekret == -1 ) return( WRS_SEEK_FAILED );
    ret = traverseTree( outRes, &copy_info, copyDataEntry );
    if( ret ) return( ret );  // error occured

    outRes->ResSize = copy_info.curr_rva - outRes->ResRVA;

    return( WRS_OK );
}

/*
 * writeDirEntry -
 * NB when an error occurs this function MUST return without altering errno
 */
WResStatus writeDirEntry( PEResDirEntry *entry, int handle )
/***************************************************************/
{
    int     num_wrote;
    int     child_num;

    num_wrote = WRESWRITE( handle, &entry->Head, sizeof(resource_dir_header) );
    if( num_wrote != sizeof(resource_dir_header) ) return( WRS_WRITE_FAILED );

    for( child_num = 0; child_num < entry->Head.num_name_entries +
                    entry->Head.num_id_entries; child_num++ ) {
        num_wrote = WRESWRITE( handle, entry->Children + child_num,
                        sizeof(resource_dir_entry) );
        if( num_wrote != sizeof(resource_dir_entry) ) return( WRS_WRITE_FAILED );
    }

    return( WRS_OK );
} /* writeDirEntry */

/*
 * writeDataEntry -
 * NB when an error occurs this function MUST return without altering errno
 */
static int writeDataEntry( PEResDataEntry * entry, int handle )
/*************************************************************/
{
    int     num_wrote;

    num_wrote = WRESWRITE( handle, &entry->Entry, sizeof(resource_entry) );
    if( num_wrote != sizeof(resource_entry) ) return( WRS_WRITE_FAILED );
    return( WRS_OK );
} /* writeDataEntry */

/*
 * writeEntry-
 * NB when an error occurs this function MUST return without altering errno
 */
static WResStatus writeEntry( PEResEntry * entry, int * handle )
{
    if( entry->IsDirEntry ) {
        return( writeDirEntry( &entry->Dir, *handle ) );
    } else {
        return( writeDataEntry( &entry->Data, *handle ) );
    }
} /* writeEntry */

/*
 * writeDirectory
 * NB when an error occurs this function MUST return without altering errno
 */
static WResStatus WriteDirectory( PEResDir * dir, int handle )
/********************************************************************/
{
    long        seekret;
    int         num_wrote;
    WResStatus  ret;

    seekret = WRESSEEK( handle, dir->ResOffset, SEEK_SET );
    if( seekret == -1 ) return( WRS_WRITE_FAILED );

    /* write the root entry header */
    ret = writeDirEntry( &dir->Root, handle );
    if( ret != WRS_OK ) return( ret );

    ret = traverseTree( dir, &handle, writeEntry );
    if( ret != WRS_OK ) return( ret );

    if( dir->String.StringBlock != 0 ) {
        num_wrote = WRESWRITE( handle, dir->String.StringBlock,
                                dir->String.StringBlockSize );
        if( num_wrote != dir->String.StringBlockSize ) {
            return( WRS_WRITE_FAILED );
        }
    }

    return( WRS_OK );
} /* writeDirectory */

void SetPEResObjRecord(pe_object *res_obj, pe_hdr_table_entry *resTblEntry,
                       PEResDir *outRes) {
    /* fill in the object record for the resource object */
    strncpy( res_obj->name, RESOURCE_OBJECT_NAME, PE_OBJ_NAME_LEN );
    res_obj->virtual_size = 0;
    res_obj->rva = outRes->ResRVA;
    res_obj->physical_size = outRes->ResSize;
    res_obj->physical_offset = outRes->ResOffset;
    res_obj->relocs_rva = 0;
    res_obj->linnum_rva = 0;
    res_obj->num_relocs = 0;
    res_obj->num_linnums = 0;
    res_obj->flags = PE_OBJ_INIT_DATA | PE_OBJ_READABLE;

    /* set the resource element of the table in the header */
    resTblEntry->rva = res_obj->rva;
    resTblEntry->size = res_obj->physical_size;

}

static void FreeSubDir( PEResDirEntry * subdir )
{
    int             num_children;
    PEResEntry *    last_child;
    PEResEntry *    curr;

    num_children = subdir->Head.num_id_entries + subdir->Head.num_name_entries;
    last_child = subdir->Children + num_children;
    curr = subdir->Children;
    while( curr < last_child ) {
        if( curr->IsDirEntry ) {
            FreeSubDir( &curr->Dir );
        }
        curr++;
    }

    WRESFREE( subdir->Children );
}

void FreePEResDir( PEResDir * dir )
{
    FreeSubDir( &dir->Root );
    WRESFREE( dir->String.StringBlock );
    WRESFREE( dir->String.StringList );
}

WResStatus PutPEResources(pResInOutInfo info) {

    WResDir dir = WResInitDir();
    PEResDir outRes;
    int status;
    unsigned long newsize;
    int dup_discarded;

    if (ALIGN_VALUE(info->rva, info->alignRva) != info->rva ||
        ALIGN_VALUE(info->offset, info->alignFile) != info->offset)
    {
        return WRS_BAD_PARAMETER;
    }

    status = WResReadDir( info->res, dir, &dup_discarded );
    if( status ) {
        return(status);
    }

    if( BuildPEResDir( &outRes, dir) ) {
        return(-1);
    }

    status = CopyPEResData( info, &outRes );
    if (status) {
        return(status);
    }
    status = WriteDirectory( &outRes, info->obj );
    if (status) {
        return(status);
    }

    newsize = ALIGN_VALUE(outRes.ResSize + info->offset, info->alignFile)
                          - info->offset;

    status = WRESSEEK(info->obj, info->offset + outRes.ResSize, SEEK_SET );
    if( status == -1 ) return( WRS_SEEK_FAILED );
    status = WritePad(info->obj, newsize - outRes.ResSize);
    if (status != WRS_OK) return status;

    outRes.ResSize = newsize;

    SetPEResObjRecord(info->res_obj, info->resTblEntry, &outRes);

    FreePEResDir( &outRes );
    WResFreeDir(dir);

    return WRS_OK;
}
