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


#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include "watcom.h"
#include "exepe.h"
#include "wresall.h"
#include "wrmergdi.h"
#include "rcmem.h"
#include "pass2.h"
#include "rcstr.h"
#include "exeutil.h"
#include "exeobj.h"
#include "global.h"
#include "errors.h"
#include "exerespe.h"
#include "iortns.h"

#define RESOURCE_OBJECT_NAME ".rsrc"

#ifndef MAKELANGID
#define MAKELANGID(p, s)       ( ( ( (uint_16)(s) ) << 10 ) | (uint_16)(p) )
#endif

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

extern int RcPadFile( int, long );

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
        RcMemFree( old );
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

    new = RcMemMalloc( sizeof(QueueNode) );
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
    RcMemFree( old );
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
    entry->Children = RcMemMalloc( num_entries * sizeof(PEResEntry) );
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
    PEResDirEntryInit( &curr->u.Dir, num_sub_entries );

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
    curr->u.Data.Wind = wind;
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
    entry_num = currtype->u.Dir.Head.num_name_entries
                        + currtype->u.Dir.Head.num_id_entries - 1;
    currres = currtype->u.Dir.Children + entry_num;

    lang_id.IsName = FALSE;
    lang_id.ID.Num = MAKELANGID( langinfo->lang.lang, langinfo->lang.sublang );
    if( PEResDirAddData( &currres->u.Dir, &lang_id, wind,
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
    if( PEResDirAddDir( &currtype->u.Dir, &resinfo->ResName,
                        resinfo->NumResources, &res->String ) ) {
        return( TRUE );
    }

    return( FALSE );
}


static int PEResDirBuild( PEResDir * res, WResDir dir )
/*****************************************************/
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
        StringBlockBuild( &res->String, dir, TRUE );
        res->DirSize = sizeof(resource_dir_header);
        PEResDirEntryInit( &res->Root, WResGetNumTypes( dir ) );
        wind = WResFirstResource( dir );
        while( !WResIsEmptyWindow( wind ) ) {
            if( WResIsFirstResOfType( wind ) ) {
                if( AddType( res, WResGetTypeInfo( wind ) ) ) return( TRUE );
            }
            if( WResIsFirstLangOfRes( wind ) ) {
                if( AddRes( res, wind ) ) return( TRUE );
            }
            AddLang( res, wind );
            wind = WResNextResource( wind, dir );
        }
    }
    return( FALSE );
}

/*
 * traverseTree
 * NB when a visit function returns an error this function MUST return
 *    without altering errno
 */
static RcStatus traverseTree( PEResDir * dir, void * visit_data,
                    RcStatus (*visit)( PEResEntry *, void * visit_data ) )
/*******************************************************************/
/* Perfroms a level order traversal of a PEResDir tree calling visit at */
/* each entry */
{
    PEResEntry *    curr_entry;
    PEResEntry *    last_child;
    PEResDirEntry * curr_dir;
    DirEntryQueue   queue;
    RcStatus        ret;

    QueueInit( &queue );

    QueueAdd( &queue, &dir->Root );

    while( !QueueIsEmpty( &queue ) ) {
        curr_dir = QueueRemove( &queue );
        last_child = curr_dir->Children + curr_dir->Head.num_name_entries +
                        curr_dir->Head.num_id_entries;
        curr_entry = curr_dir->Children;
        while( curr_entry < last_child ) {
            ret = visit( curr_entry, visit_data );
            if( ret != RS_OK ) return( ret );
            if( curr_entry->IsDirEntry ) {
                QueueAdd( &queue, &curr_entry->u.Dir );
            }
            curr_entry++;
        }
    }

    QueueEmpty( &queue );

    return( RS_OK );
} /* traverseTree */

static RcStatus SetEntryOffset( PEResEntry * entry, uint_32 * curr_offset )
/********************************************************************/
{
    int     num_entries;

    if( entry->IsDirEntry ) {
        entry->Entry.entry_rva = *curr_offset | PE_RESOURCE_MASK_ON;
        num_entries = entry->u.Dir.Head.num_name_entries +
                            entry->u.Dir.Head.num_id_entries;
        *curr_offset += sizeof(resource_dir_header) +
                            num_entries * sizeof(resource_dir_entry);
    } else {
        entry->Entry.entry_rva = *curr_offset;
        *curr_offset += sizeof(resource_entry);
    }
    return( RS_OK );
} /* SetEntryOffset */

static RcStatus AdjustNameEntry( PEResEntry * entry, uint_32 * dir_size )
/******************************************************************/
{
    uint_32     str_offset;

    if( entry->Entry.id_name & PE_RESOURCE_MASK_ON ) {
        /* the id_name contains the offset into the string block */
        str_offset = entry->Entry.id_name & PE_RESOURCE_MASK;
        str_offset += *dir_size;
        entry->Entry.id_name = str_offset | PE_RESOURCE_MASK_ON;
    }

    return( RS_OK );
} /* AdjustNameEntry */

static int ComparePEResIdName( const PEResEntry * entry1,
                               const PEResEntry * entry2 )
/***********************************************************************/
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

static RcStatus SortDirEntry( PEResEntry * entry, void * dummy )
/*********************************************************/
{
    int     num_entries;

    dummy = dummy;

    if( entry->IsDirEntry ) {
        num_entries = entry->u.Dir.Head.num_name_entries +
                    entry->u.Dir.Head.num_id_entries;
        qsort( entry->u.Dir.Children, num_entries, sizeof(PEResEntry),
                    ComparePEResIdName );
    }
    return( RS_OK );
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

typedef struct CopyResInfo {
    int                 to_handle;
    ExeFileInfo         *file;  // for setting debugging offset
    ResFileInfo         *curres;
    ResFileInfo         *errres;
} CopyResInfo;

/*
 * copyDataEntry
 * NB when an error occurs this function MUST return without altering errno
 */
static RcStatus copyDataEntry( PEResEntry *entry, CopyResInfo *copy_info )
/*********************************************************************/
{
    WResLangInfo        *res_info;
    long                seek_rc;
    uint_32             diff;
    RcStatus            status;
    ResFileInfo         *info;
    int                 closefile;

    closefile = FALSE;
    if( !entry->IsDirEntry ) {
        info = WResGetFileInfo( entry->u.Data.Wind );
        if( copy_info->curres == NULL || copy_info->curres == info ) {
            res_info = WResGetLangInfo( entry->u.Data.Wind );
            seek_rc = RcSeek( info->Handle, res_info->Offset, SEEK_SET );
            if( seek_rc == -1 ) return( RS_READ_ERROR );
            status = CopyExeData( info->Handle, copy_info->to_handle,
                                  res_info->Length );
            if( status != RS_OK ) {
                copy_info->errres = info;
                return( status );
            }
            diff = ALIGN_VALUE( res_info->Length, sizeof(uint_32) );
            if( diff != res_info->Length ) {
                /* add the padding */
//              seek_rc = RcSeek( copy_info->to_handle, diff - res_info->Length,
//                                  SEEK_CUR );
                RcPadFile( copy_info->to_handle, diff - res_info->Length );
                if( seek_rc == -1 ) return( RS_WRITE_ERROR );
            }
            CheckDebugOffset( copy_info->file );
        }
    }

    return( RS_OK );
} /* copyDataEntry */

/*
 * copyPEResources
 * NB when an error occurs this function MUST return without altering errno
 */
static RcStatus copyPEResources( ExeFileInfo * tmp, ResFileInfo *resfiles,
                                int to_handle, int writebyfile,
                                ResFileInfo **errres )
/****************************************************************/
{
    CopyResInfo     copy_info;
    PEExeInfo *     info;
    pe_va           start_rva;
    uint_32         start_off;
    long            seek_rc;
    RcStatus        ret;
    int             tmpopened;

    info = &tmp->u.PEInfo;

    start_rva = info->Res.ResRVA + info->Res.DirSize +
                    info->Res.String.StringBlockSize;
    start_off = info->Res.ResOffset + info->Res.DirSize +
                    info->Res.String.StringBlockSize;

    copy_info.to_handle = to_handle;
    copy_info.errres = NULL;
    copy_info.file = tmp;       /* for tracking debugging info offset */
    start_off = ALIGN_VALUE( start_off, sizeof(uint_32) );

    seek_rc = RcSeek( to_handle, start_off, SEEK_SET );
    if( seek_rc == -1 ) return( RS_WRITE_ERROR );
    if( !writebyfile ) {
        copy_info.curres = NULL;
        ret = traverseTree( &info->Res, &copy_info, copyDataEntry );
        *errres = copy_info.errres;
    } else {
        while( resfiles != NULL ) {
            copy_info.curres = resfiles;
            if( resfiles->IsOpen ) {
                tmpopened = FALSE;
            } else {
                resfiles->Handle = ResOpenFileRO( resfiles->name );
                if( resfiles->Handle == -1 ) {
                    ret = RS_OPEN_ERROR;
                    *errres = resfiles;
                    break;
                }
                resfiles->IsOpen = TRUE;
                tmpopened = TRUE;
            }
            ret = traverseTree( &info->Res, &copy_info, copyDataEntry );
            if( tmpopened ) {
                ResCloseFile( resfiles->Handle );
                resfiles->Handle = -1;
                resfiles->IsOpen = FALSE;
            }
            if( ret != RS_OK ) {
                *errres = resfiles;
                break;
            }
            resfiles = resfiles->next;
        }
    }
    return( ret );
} /* copyPEResources */

/*
 * writeDirEntry -
 * NB when an error occurs this function MUST return without altering errno
 */
static RcStatus writeDirEntry( PEResDirEntry *entry, int handle )
/***************************************************************/
{
    int     num_wrote;
    int     child_num;

    num_wrote = RcWrite( handle, &entry->Head, sizeof(resource_dir_header) );
    if( num_wrote != sizeof(resource_dir_header) ) return( RS_WRITE_ERROR );

    for( child_num = 0; child_num < entry->Head.num_name_entries +
                    entry->Head.num_id_entries; child_num++ ) {
        num_wrote = RcWrite( handle, entry->Children + child_num,
                        sizeof(resource_dir_entry) );
        if( num_wrote != sizeof(resource_dir_entry) ) return( RS_WRITE_ERROR );
    }

    return( RS_OK );
} /* writeDirEntry */

/*
 * writeDataEntry -
 * NB when an error occurs this function MUST return without altering errno
 */
static int writeDataEntry( PEResDataEntry * entry, int handle )
/*************************************************************/
{
    int     num_wrote;

    num_wrote = RcWrite( handle, &entry->Entry, sizeof(resource_entry) );
    if( num_wrote != sizeof(resource_entry) ) return( RS_WRITE_ERROR );
    return( RS_OK );
} /* writeDataEntry */

typedef struct {
    ResFileInfo         *curfile;
    unsigned_32         *rva;
} DataEntryCookie;

/*
 * setDataEntry
 */
static RcStatus setDataEntry( PEResEntry *entry, DataEntryCookie *info )
/********************************************************************/
{
    WResLangInfo        *langinfo;
    ResFileInfo         *fileinfo;

    if( !entry->IsDirEntry ) {
        fileinfo = WResGetFileInfo( entry->u.Data.Wind );
        if( info->curfile == NULL || info->curfile == fileinfo ) {
            langinfo = WResGetLangInfo( entry->u.Data.Wind );
            entry->u.Data.Entry.data_rva = *info->rva;
            entry->u.Data.Entry.size = langinfo->Length;
            entry->u.Data.Entry.code_page = 0;    /* should this be the UNICODE page*/
            entry->u.Data.Entry.rsvd = 0;
            *info->rva = *info->rva +
                        ALIGN_VALUE( langinfo->Length, sizeof(uint_32) );
        }
    }
    return( RS_OK );
}

/*
 * writeEntry-
 * NB when an error occurs this function MUST return without altering errno
 */
static RcStatus writeEntry( PEResEntry * entry, int * handle )
/*******************************************************/
{
    if( entry->IsDirEntry ) {
        return( writeDirEntry( &entry->u.Dir, *handle ) );
    } else {
        return( writeDataEntry( &entry->u.Data, *handle ) );
    }
} /* writeEntry */

/*
 * writeDirectory
 * NB when an error occurs this function MUST return without altering errno
 */
static RcStatus writeDirectory( PEResDir * dir, int handle )
/********************************************************************/
{
    long        seek_rc;
    int         num_wrote;
    RcStatus    ret;

    seek_rc = RcSeek( handle, dir->ResOffset, SEEK_SET );
    if( seek_rc == -1 ) return( RS_WRITE_ERROR );

    /* write the root entry header */
    ret = writeDirEntry( &dir->Root, handle );
    if( ret != RS_OK ) return( ret );

    ret = traverseTree( dir, &handle, writeEntry );
    if( ret != RS_OK ) return( ret );

    if( dir->String.StringBlock != 0 ) {
        num_wrote = RcWrite( handle, dir->String.StringBlock,
                                dir->String.StringBlockSize );
        if( num_wrote != dir->String.StringBlockSize ) {
            return( RS_WRITE_ERROR );
        }
    }

    return( RS_OK );
} /* writeDirectory */

static void FreeSubDir( PEResDirEntry * subdir )
/**********************************************/
{
    int             num_children;
    PEResEntry *    last_child;
    PEResEntry *    curr;

    num_children = subdir->Head.num_id_entries + subdir->Head.num_name_entries;
    last_child = subdir->Children + num_children;
    curr = subdir->Children;
    while( curr < last_child ) {
        if( curr->IsDirEntry ) {
            FreeSubDir( &curr->u.Dir );
        }
        curr++;
    }

    RcMemFree( subdir->Children );
}

static void FreePEResDir( PEResDir * dir )
/****************************************/
{
    FreeSubDir( &dir->Root );
    RcMemFree( dir->String.StringBlock );
    RcMemFree( dir->String.StringList );
}

#ifndef INSIDE_WLINK
extern int RcPadFile( int handle, long pad )
/******************************************/
{
    char        zero = 0;

    if( pad > 0 ) {
        if( RcSeek( handle, pad - 1, SEEK_CUR ) == -1 ) {
            return( TRUE );
        }
        if( RcWrite( handle, &zero, 1 ) != 1 )  {
            return( TRUE );
        }
    }
    return( FALSE );
}
#endif

/*
 * padObject
 * NB when an error occurs this function MUST return without altering errno
 */
static int padObject( PEResDir *dir, ExeFileInfo *tmp, long size )
{
    long        pos;
    long        pad;

    pos = RcTell( tmp->Handle );
    if( pos == -1 ) return( TRUE );
    pad = dir->ResOffset + size - pos;
    if( pad > 0 ) {
        RcPadFile( tmp->Handle, pad );
    }
    CheckDebugOffset( tmp );
    return( FALSE );
#if(0)
    long        seek_rc;
    char        zero=0;

    seek_rc = RcSeek( tmp->Handle, dir->ResOffset, SEEK_SET );
    if( seek_rc == -1 ) return( TRUE );
    seek_rc = RcSeek( tmp->Handle, size-1, SEEK_CUR );
    if( seek_rc == -1 ) return( TRUE );
    if( RcWrite( tmp->Handle, &zero, 1 ) != 1 ) return( TRUE );
    CheckDebugOffset( tmp );
    return( FALSE );
#endif
}

static void fillResourceObj( pe_object *res_obj, PEResDir *dir,
                             uint_32 alignment )
/****************************************************************/
{
    strncpy( res_obj->name, RESOURCE_OBJECT_NAME, PE_OBJ_NAME_LEN );
    res_obj->virtual_size = 0;
    res_obj->rva = dir->ResRVA;
    res_obj->physical_size = ALIGN_VALUE( dir->ResSize, alignment );
    res_obj->physical_offset = dir->ResOffset;
    res_obj->relocs_rva = 0;
    res_obj->linnum_rva = 0;
    res_obj->num_relocs = 0;
    res_obj->num_linnums = 0;
    res_obj->flags = PE_OBJ_INIT_DATA | PE_OBJ_READABLE;
}

// merge the directories of all the res files into one large directory
// stored on the first resfileinfo node
int mergeDirectory( ResFileInfo *resfiles, WResMergeError **errs )
/******************************************************************/
{
    ResFileInfo         *cur;

    if( errs != NULL ) *errs = NULL;
    if( resfiles == NULL ) return( FALSE );
    cur = resfiles->next;
    while( cur != NULL ) {
        if( WResMergeDirs( resfiles->Dir, cur->Dir, errs ) ) {
            return( TRUE );
        }
        cur = cur->next;
    }
    return( FALSE );
}

static void setDataOffsets( PEResDir *dir, unsigned_32 *curr_rva,
                                ResFileInfo *resfiles, int writebyfile )
/****************************************************************/
{
    DataEntryCookie     cookie;

    cookie.rva = curr_rva;
    if( writebyfile ) {
        while( resfiles != NULL ) {
            cookie.curfile = resfiles;
            traverseTree( dir, &cookie, setDataEntry );
            resfiles = resfiles->next;
        }
    } else {
        cookie.curfile = NULL;
        traverseTree( dir, &cookie, setDataEntry );
    }
}

#define NAME_LEN        256
static void reportDuplicateResources( WResMergeError *errs )
/************************************************************/
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

extern int BuildResourceObject( ExeFileInfo *exeinfo, ResFileInfo *resinfo,
                                pe_object *res_obj, unsigned_32 rva,
                                unsigned_32 offset, int writebyfile )
/**************************************************************************/
{
    PEResDir *      dir;
    RcStatus        status;
    unsigned_32     curr_rva;
    WResMergeError  *errs;
    ResFileInfo     *errres;

    dir = &exeinfo->u.PEInfo.Res;

    mergeDirectory( resinfo, &errs );
    if( errs != NULL ) {
        reportDuplicateResources( errs );
        WResFreeMergeErrors( errs );
        return( TRUE );
    }
    if( PEResDirBuild( dir, resinfo->Dir ) ) {
        RcError( ERR_INTERNAL, INTERR_ERR_BUILDING_RES_DIR );
        return( TRUE );
    }
    CompleteTree( dir );
    exeinfo->u.PEInfo.Res.ResOffset = offset;
    exeinfo->u.PEInfo.Res.ResRVA = rva;
    curr_rva = rva + exeinfo->u.PEInfo.Res.DirSize
               + exeinfo->u.PEInfo.Res.String.StringBlockSize;
    curr_rva = ALIGN_VALUE( curr_rva, sizeof( uint_32 ) );
    setDataOffsets( dir, &curr_rva, resinfo, writebyfile );
    status = writeDirectory( dir, exeinfo->Handle );
    if( status != RS_OK ) {
        RcError( ERR_WRITTING_FILE, exeinfo->name, strerror( errno ) );
        return( TRUE );
    }

    status = copyPEResources( exeinfo, resinfo, exeinfo->Handle,
                              writebyfile, &errres );
    // warning - the file names output in these messages could be
    //          incorrect if the -fr switch is in use
    if( status != RS_OK  ) {
        switch( status ) {
        case RS_WRITE_ERROR:
            RcError( ERR_WRITTING_FILE, exeinfo->name, strerror( errno ) );
            break;
        case RS_READ_ERROR:
            RcError( ERR_READING_RES, errres->name, strerror( errno )  );
            break;
        case RS_OPEN_ERROR:
            RcError( ERR_CANT_OPEN_FILE, errres->name, strerror( errno ) );
        case RS_READ_INCMPLT:
            RcError( ERR_UNEXPECTED_EOF, errres->name );
            break;
        default:
            RcError( ERR_INTERNAL, INTERR_UNKNOWN_RCSTATUS );
            break;
        }
        return( TRUE );
    }

    exeinfo->u.PEInfo.Res.ResSize = curr_rva - rva;
    fillResourceObj( res_obj, dir, exeinfo->u.PEInfo.WinHead->file_align );
    if( padObject( dir, exeinfo, res_obj->physical_size ) ) {
        RcError( ERR_WRITTING_FILE, exeinfo->name, strerror( errno ) );
        return( TRUE );
    }

    /* set the resource element of the table in the header */
    exeinfo->u.PEInfo.WinHead->table[ PE_TBL_RESOURCE ].rva = res_obj->rva;
    exeinfo->u.PEInfo.WinHead->table[ PE_TBL_RESOURCE ].size =
                                        res_obj->physical_size;

    FreePEResDir( dir );

    return( FALSE );
} /* BuildResourceObject */


#ifndef INSIDE_WLINK
int RcBuildResourceObject( void ) {
/***********************************/

    pe_object           *res_obj;
    unsigned_32         rva;
    unsigned_32         offset;
    RcStatus            error;
    ExeFileInfo         *exeinfo;

    exeinfo = &Pass2Info.TmpFile;
    if( CmdLineParms.NoResFile ) {
        exeinfo->u.PEInfo.WinHead->table[ PE_TBL_RESOURCE ].rva = 0;
        exeinfo->u.PEInfo.WinHead->table[ PE_TBL_RESOURCE ].size = 0;
    } else {
        res_obj = exeinfo->u.PEInfo.Objects
                  + exeinfo->u.PEInfo.WinHead->num_objects - 1;
        rva = GetNextObjRVA( &exeinfo->u.PEInfo );
        offset = GetNextObjPhysOffset( &exeinfo->u.PEInfo );
        error = BuildResourceObject( exeinfo, Pass2Info.ResFiles,
                                     res_obj, rva, offset,
                                     !Pass2Info.AllResFilesOpen );
    }
    if( CmdLineParms.WritableRes ) {
        res_obj->flags |= PE_OBJ_WRITABLE;
    }
    return( error );
}
#endif
