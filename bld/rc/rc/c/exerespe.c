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


#include <time.h>
#include "wio.h"
#include "global.h"
#include "rcstrblk.h"
#include "rcstr.h"
#include "errors.h"
#include "wrmergdi.h"
#include "rcrtns.h"
#include "rccore.h"
#include "exeobj.h"
#include "exeutil.h"
#include "exerespe.h"


#define RESOURCE_OBJECT_NAME ".rsrc"

#ifndef MAKELANGID
#define MAKELANGID(p, s)       ( ( ( (uint_16)(s) ) << 10 ) | (uint_16)(p) )
#endif

/* structures and routines to manipulate a queue of PEResDirEntry * */
/* This uses a linked list representation despite the overhead of the pointer */
/* since the total number of entries in the queue should be small and this is */
/* easier to code while still being dynamic */

typedef struct QueueNode {
    struct QueueNode    *next;
    PEResDirEntry       *entry;
} QueueNode;

typedef struct DirEntryQueue {
    QueueNode           *front;
    QueueNode           *back;
} DirEntryQueue;

static void QueueInit( DirEntryQueue *queue )
/*******************************************/
{
    queue->front = NULL;
    queue->back = NULL;
} /* QueueInit */

static void QueueEmpty( DirEntryQueue *queue )
/********************************************/
{
    QueueNode   *curr;
    QueueNode   *next;

    for( curr = queue->front; curr != NULL; curr = next ) {
        next = curr->next;
        RCFREE( curr );
    }

    QueueInit( queue );
} /* QueueEmpty */

static bool QueueIsEmpty( DirEntryQueue *queue )
/**********************************************/
{
    return( queue->front == NULL );
}

static void QueueAdd( DirEntryQueue *queue, PEResDirEntry *entry )
/****************************************************************/
{
    QueueNode       *new;

    new = RCALLOC( sizeof(QueueNode) );
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

static PEResDirEntry *QueueRemove( DirEntryQueue *queue )
/*******************************************************/
{
    QueueNode       *old;
    PEResDirEntry   *entry;

    old = queue->front;
    if( old == NULL ) {
        return( NULL );
    }

    queue->front = old->next;
    if( queue->front == NULL ) {
        queue->back = NULL;
    }

    entry = old->entry;
    RCFREE( old );
    return( entry );
} /* QueueRemove */

static void PEResDirEntryInit( PEResDirEntry *entry, int num_entries )
/********************************************************************/
{
    entry->Head.flags = 0;
    entry->Head.time_stamp = time( NULL );
    entry->Head.major = 0;
    entry->Head.minor = 0;
    entry->Head.num_name_entries = 0;
    entry->Head.num_id_entries = 0;
    entry->NumUnused = num_entries;
    entry->Children = RCALLOC( num_entries * sizeof(PEResEntry) );
}

static void PEResDirAdd( PEResDirEntry * entry, WResID * name,
                    StringsBlock *strings )
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


static bool PEResDirAddDir( PEResDirEntry * entry, WResID * name,
                    int num_sub_entries, StringsBlock *strings )
/***************************************************************/
{
    int             entry_num;
    PEResEntry      *curr;

    if( entry->NumUnused <= 0 )
        return( true );

    PEResDirAdd( entry, name, strings );

    entry_num = entry->Head.num_name_entries + entry->Head.num_id_entries - 1;
    curr = entry->Children + entry_num;
    curr->IsDirEntry = true;
    PEResDirEntryInit( &curr->u.Dir, num_sub_entries );

    return( false );
}

static bool PEResDirAddData( PEResDirEntry *entry, WResID *name,
                    WResDirWindow wind, StringsBlock *strings )
/***************************************************************/
{
    int             entry_num;
    PEResEntry      *curr;

    if( entry->NumUnused <= 0 )
        return( true );

    PEResDirAdd( entry, name, strings );

    entry_num = entry->Head.num_name_entries + entry->Head.num_id_entries - 1;
    curr = entry->Children + entry_num;
    curr->IsDirEntry = false;
    curr->u.Data.Wind = wind;
    /* The Data.Entry field will be filled in as the resource is writen */
    return( false );
}

static bool AddType( PEResDir *res, WResTypeInfo *type )
/******************************************************/
{
    return( PEResDirAddDir( &res->Root, &type->TypeName, type->NumResources, &res->String ) );
}

static bool AddLang( PEResDir *res, WResDirWindow wind )
/******************************************************/
{
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

    lang_id.IsName = false;
    lang_id.ID.Num = MAKELANGID( langinfo->lang.lang, langinfo->lang.sublang );
    if( PEResDirAddData( &currres->u.Dir, &lang_id, wind, &res->String ) ) {
        return( true );
    }
    return( false );
}

static bool AddRes( PEResDir *res, WResDirWindow wind )
/*****************************************************/
{
    int             entry_num;
    PEResEntry      *currtype;
    WResResInfo     *resinfo;

    resinfo = WResGetResInfo( wind );

    /* find the current type */
    entry_num = res->Root.Head.num_name_entries + res->Root.Head.num_id_entries;
    currtype = res->Root.Children + entry_num - 1;

    /* Add a directory level for the languages */
    if( PEResDirAddDir( &currtype->u.Dir, &resinfo->ResName, resinfo->NumResources, &res->String ) ) {
        return( true );
    }

    return( false );
}


static bool PEResDirBuild( PEResDir *res, WResDir dir )
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
        StringBlockBuild( &res->String, dir, true );
        res->DirSize = sizeof(resource_dir_header);
        PEResDirEntryInit( &res->Root, WResGetNumTypes( dir ) );
        for( wind = WResFirstResource( dir ); !WResIsEmptyWindow( wind ); wind = WResNextResource( wind, dir ) ) {
            if( WResIsFirstResOfType( wind ) ) {
                if( AddType( res, WResGetTypeInfo( wind ) ) ) {
                    return( true );
                }
            }
            if( WResIsFirstLangOfRes( wind ) ) {
                if( AddRes( res, wind ) ) {
                    return( true );
                }
            }
            AddLang( res, wind );
        }
    }
    return( false );
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
        last_child = curr_dir->Children + curr_dir->Head.num_name_entries + curr_dir->Head.num_id_entries;
        for( curr_entry = curr_dir->Children; curr_entry < last_child; curr_entry++ ) {
            ret = visit( curr_entry, visit_data );
            if( ret != RS_OK )
                return( ret );
            if( curr_entry->IsDirEntry ) {
                QueueAdd( &queue, &curr_entry->u.Dir );
            }
        }
    }

    QueueEmpty( &queue );

    return( RS_OK );
} /* traverseTree */

static RcStatus SetEntryOffset( PEResEntry * entry, void * _curr_offset )
/***********************************************************************/
{
    int     num_entries;
    uint_32 *curr_offset = _curr_offset;

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

static RcStatus AdjustNameEntry( PEResEntry * entry, void * _dir_size )
/*********************************************************************/
{
    uint_32     str_offset;
    uint_32    *dir_size = _dir_size;

    if( entry->Entry.id_name & PE_RESOURCE_MASK_ON ) {
        /* the id_name contains the offset into the string block */
        str_offset = entry->Entry.id_name & PE_RESOURCE_MASK;
        str_offset += *dir_size;
        entry->Entry.id_name = str_offset | PE_RESOURCE_MASK_ON;
    }

    return( RS_OK );
} /* AdjustNameEntry */

static int ComparePEResIdName( const void *e1, const void *e2 )
/*************************************************************/
{
#define PEE(x) ((const PEResEntry *)(x))
    if( PEE( e1 )->Name == NULL ) {
        if( PEE( e2 )->Name == NULL ) {
            if( PEE( e1 )->Entry.id_name > PEE( e2 )->Entry.id_name ) {
                return( 1 );
            } else if( PEE( e1 )->Entry.id_name < PEE( e2 )->Entry.id_name ) {
                return( -1 );
            } else {
                return( 0 );
            }
        } else {
            return( 1 );
        }
    } else {
        if( PEE( e2 )->Name == NULL ) {
            return( -1 );
        } else {
            return( CompareStringItems32( PEE( e1 )->Name, PEE( e2 )->Name ) );
        }
    }
#undef PEE
} /* ComparePEResIdName */

static RcStatus SortDirEntry( PEResEntry * entry, void * dummy )
/*********************************************************/
{
    int     num_entries;

    dummy = dummy;

    if( entry->IsDirEntry ) {
        num_entries = entry->u.Dir.Head.num_name_entries + entry->u.Dir.Head.num_id_entries;
        qsort( entry->u.Dir.Children, num_entries, sizeof(PEResEntry), ComparePEResIdName );
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
    WResFileID          to_handle;
    ExeFileInfo         *file;  // for setting debugging offset
    ResFileInfo         *curres;
    ResFileInfo         *errres;
} CopyResInfo;

/*
 * copyDataEntry
 * NB when an error occurs this function MUST return without altering errno
 */
static RcStatus copyDataEntry( PEResEntry *entry, void *_copy_info )
/******************************************************************/
{
    CopyResInfo         *copy_info = _copy_info;
    WResLangInfo        *res_info;
    uint_32             diff;
    RcStatus            ret;
    ResFileInfo         *info;
//    bool                closefile;

//    closefile = false;
    if( !entry->IsDirEntry ) {
        info = WResGetFileInfo( entry->u.Data.Wind );
        if( copy_info->curres == NULL || copy_info->curres == info ) {
            res_info = WResGetLangInfo( entry->u.Data.Wind );
            if( RCSEEK( info->Handle, res_info->Offset, SEEK_SET ) == -1 )
                return( RS_READ_ERROR );
            ret = CopyExeData( info->Handle, copy_info->to_handle, res_info->Length );
            if( ret != RS_OK ) {
                copy_info->errres = info;
                return( ret );
            }
            diff = ALIGN_VALUE( res_info->Length, sizeof(uint_32) );
            if( diff != res_info->Length ) {
                /* add the padding */
                if( RcPadFile( copy_info->to_handle, diff - res_info->Length ) ) {
                    return( RS_WRITE_ERROR );
                }
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
static RcStatus copyPEResources( ExeFileInfo *tmp, ResFileInfo *resfiles,
                                WResFileID to_handle, bool writebyfile,
                                ResFileInfo **errres )
/****************************************************************/
{
    CopyResInfo     copy_info;
//    pe_va           start_rva;
    uint_32         start_off;
    RcStatus        ret;
    bool            tmpopened;

//    start_rva = tmp->u.PEInfo.Res.ResRVA + tmp->u.PEInfo.Res.DirSize + tmp->u.PEInfo.Res.String.StringBlockSize;
    start_off = tmp->u.PEInfo.Res.ResOffset + tmp->u.PEInfo.Res.DirSize + tmp->u.PEInfo.Res.String.StringBlockSize;

    copy_info.to_handle = to_handle;
    copy_info.errres = NULL;
    copy_info.file = tmp;       /* for tracking debugging info offset */
    start_off = ALIGN_VALUE( start_off, sizeof(uint_32) );

    if( RCSEEK( to_handle, start_off, SEEK_SET ) == -1 )
        return( RS_WRITE_ERROR );
    if( !writebyfile ) {
        copy_info.curres = NULL;
        ret = traverseTree( &tmp->u.PEInfo.Res, &copy_info, copyDataEntry );
        *errres = copy_info.errres;
    } else {
        ret = RS_OK;
        while( resfiles != NULL ) {
            copy_info.curres = resfiles;
            if( resfiles->IsOpen ) {
                tmpopened = false;
            } else {
                resfiles->Handle = ResOpenFileRO( resfiles->name );
                if( resfiles->Handle == NIL_HANDLE ) {
                    ret = RS_OPEN_ERROR;
                    *errres = resfiles;
                    break;
                }
                resfiles->IsOpen = true;
                tmpopened = true;
            }
            ret = traverseTree( &tmp->u.PEInfo.Res, &copy_info, copyDataEntry );
            if( tmpopened ) {
                ResCloseFile( resfiles->Handle );
                resfiles->Handle = NIL_HANDLE;
                resfiles->IsOpen = false;
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
static RcStatus writeDirEntry( PEResDirEntry *entry, WResFileID handle )
/**********************************************************************/
{
    int     child_num;

    if( RCWRITE( handle, &entry->Head, sizeof(resource_dir_header) ) != sizeof(resource_dir_header) )
        return( RS_WRITE_ERROR );

    for( child_num = 0; child_num < entry->Head.num_name_entries +
                    entry->Head.num_id_entries; child_num++ ) {
        if( RCWRITE( handle, entry->Children + child_num, sizeof(resource_dir_entry) ) != sizeof(resource_dir_entry) ) {
            return( RS_WRITE_ERROR );
        }
    }

    return( RS_OK );
} /* writeDirEntry */

/*
 * writeDataEntry -
 * NB when an error occurs this function MUST return without altering errno
 */
static RcStatus writeDataEntry( PEResDataEntry * entry, WResFileID handle )
/*************************************************************************/
{
    if( RCWRITE( handle, &entry->Entry, sizeof(resource_entry) ) != sizeof(resource_entry) )
        return( RS_WRITE_ERROR );
    return( RS_OK );
} /* writeDataEntry */

typedef struct {
    ResFileInfo         *curfile;
    unsigned_32         *rva;
} DataEntryCookie;

/*
 * setDataEntry
 */
static RcStatus setDataEntry( PEResEntry *entry, void *_info )
/************************************************************/
{
    WResLangInfo        *langinfo;
    ResFileInfo         *fileinfo;
    DataEntryCookie     *info = _info;

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
static RcStatus writeEntry( PEResEntry * entry, void * _handle )
/**************************************************************/
{
    WResFileID *handle = _handle;

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
static RcStatus writeDirectory( PEResDir * dir, WResFileID handle )
/*****************************************************************/
{
    RcStatus    ret;

    if( RCSEEK( handle, dir->ResOffset, SEEK_SET ) == -1 )
        return( RS_WRITE_ERROR );

    /* write the root entry header */
    ret = writeDirEntry( &dir->Root, handle );
    if( ret != RS_OK )
        return( ret );

    ret = traverseTree( dir, &handle, writeEntry );
    if( ret != RS_OK )
        return( ret );

    if( dir->String.StringBlock != 0 ) {
        if( RCWRITE( handle, dir->String.StringBlock, dir->String.StringBlockSize ) != dir->String.StringBlockSize ) {
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
    for( curr = subdir->Children; curr < last_child; ++curr ) {
        if( curr->IsDirEntry ) {
            FreeSubDir( &curr->u.Dir );
        }
    }

    RCFREE( subdir->Children );
}

static void FreePEResDir( PEResDir * dir )
/****************************************/
{
    FreeSubDir( &dir->Root );
    if( dir->String.StringBlock != NULL )
        RCFREE( dir->String.StringBlock );
    if( dir->String.StringList != NULL ) {
        RCFREE( dir->String.StringList );
    }
}

#ifndef INSIDE_WLINK
extern bool RcPadFile( WResFileID handle, long pad )
/**************************************************/
{
    char        zero = 0;

    if( pad > 0 ) {
        if( RCSEEK( handle, pad - 1, SEEK_CUR ) == -1 ) {
            return( true );
        }
        if( RCWRITE( handle, &zero, 1 ) != 1 )  {
            return( true );
        }
    }
    return( false );
}
#endif

/*
 * padObject
 * NB when an error occurs this function MUST return without altering errno
 */
static bool padObject( PEResDir *dir, ExeFileInfo *tmp, long size )
{
    long        pos;
    long        pad;

    pos = RCTELL( tmp->Handle );
    if( pos == -1 )
        return( true );
    pad = dir->ResOffset + size - pos;
    if( pad > 0 ) {
        RcPadFile( tmp->Handle, pad );
    }
    CheckDebugOffset( tmp );
    return( false );
#if( 0)
    char        zero=0;

    if( RCSEEK( tmp->Handle, dir->ResOffset, SEEK_SET ) == -1 )
        return( true );
    if( RCSEEK( tmp->Handle, size-1, SEEK_CUR ) == -1 )
        return( true );
    if( RCWRITE( tmp->Handle, &zero, 1 ) != 1 )
        return( true );
    CheckDebugOffset( tmp );
    return( false );
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
static bool mergeDirectory( ResFileInfo *resfiles, WResMergeError **errs )
/***********************************************************************/
{
    ResFileInfo         *cur;

    if( errs != NULL )
        *errs = NULL;
    if( resfiles == NULL )
        return( false );
    for( cur = resfiles->next; cur != NULL; cur = cur->next ) {
        if( WResMergeDirs( resfiles->Dir, cur->Dir, errs ) ) {
            return( true );
        }
    }
    return( false );
}

static void setDataOffsets( PEResDir *dir, unsigned_32 *curr_rva,
                                ResFileInfo *resfiles, bool writebyfile )
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

    for( curerr = errs; curerr != NULL; curerr = curerr->next ) {
        resinfo = WResGetResInfo( curerr->dstres );
        typeinfo = WResGetTypeInfo( curerr->dstres );
        file1 = WResGetFileInfo( curerr->dstres );
        file2 = WResGetFileInfo( curerr->srcres );
        ReportDupResource( &resinfo->ResName, &typeinfo->TypeName, file1->name, file2->name, false );
    }
}

bool BuildPEResourceObject( ExeFileInfo *exe, ResFileInfo *resinfo,
                                pe_object *res_obj, unsigned_32 rva,
                                unsigned_32 offset, bool writebyfile )
/**************************************************************************/
{
    PEResDir            *dir;
    RcStatus            ret;
    unsigned_32         curr_rva;
    WResMergeError      *errs;
    ResFileInfo         *errres;
    unsigned_32         file_align;
    exe_pe_header       *pehdr;
    pe_hdr_table_entry  *table;

    dir = &exe->u.PEInfo.Res;

    mergeDirectory( resinfo, &errs );
    if( errs != NULL ) {
        reportDuplicateResources( errs );
        WResFreeMergeErrors( errs );
        return( true );
    }
    if( PEResDirBuild( dir, resinfo->Dir ) ) {
        RcError( ERR_INTERNAL, INTERR_ERR_BUILDING_RES_DIR );
        return( true );
    }
    CompleteTree( dir );
    exe->u.PEInfo.Res.ResOffset = offset;
    exe->u.PEInfo.Res.ResRVA = rva;
    curr_rva = rva + exe->u.PEInfo.Res.DirSize + exe->u.PEInfo.Res.String.StringBlockSize;
    curr_rva = ALIGN_VALUE( curr_rva, sizeof( uint_32 ) );
    setDataOffsets( dir, &curr_rva, resinfo, writebyfile );
    ret = writeDirectory( dir, exe->Handle );
    if( ret != RS_OK ) {
        RcError( ERR_WRITTING_FILE, exe->name, strerror( errno ) );
        return( true );
    }

    ret = copyPEResources( exe, resinfo, exe->Handle, writebyfile, &errres );
    // warning - the file names output in these messages could be
    //          incorrect if the -fr switch is in use
    if( ret != RS_OK  ) {
        switch( ret ) {
        case RS_WRITE_ERROR:
            RcError( ERR_WRITTING_FILE, exe->name, strerror( errno ) );
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
        return( true );
    }
    exe->u.PEInfo.Res.ResSize = curr_rva - rva;

    pehdr = exe->u.PEInfo.WinHead;
    if( IS_PE64( *pehdr ) ) {
        file_align = PE64( *pehdr ).file_align;
        table = PE64( *pehdr ).table;
    } else {
        file_align = PE32( *pehdr ).file_align;
        table = PE32( *pehdr ).table;
    }
    fillResourceObj( res_obj, dir, file_align );
    if( padObject( dir, exe, res_obj->physical_size ) ) {
        RcError( ERR_WRITTING_FILE, exe->name, strerror( errno ) );
        return( true );
    }

    /* set the resource element of the table in the header */
    table[PE_TBL_RESOURCE].rva = res_obj->rva;
    table[PE_TBL_RESOURCE].size = res_obj->physical_size;

    FreePEResDir( dir );

    return( false );
} /* BuildPEResourceObject */


#ifndef INSIDE_WLINK
bool RcBuildPEResourceObject( void )
/**********************************/
{
    pe_object           *res_obj;
    unsigned_32         rva;
    unsigned_32         offset;
    bool                error;
    ExeFileInfo         *exe;
    exe_pe_header       *pehdr;
    pe_hdr_table_entry  *table;

    exe = &Pass2Info.TmpFile;
    pehdr = exe->u.PEInfo.WinHead;
    if( CmdLineParms.NoResFile ) {
        if( IS_PE64( *pehdr ) ) {
            table = PE64( *pehdr ).table;
        } else {
            table = PE32( *pehdr ).table;
        }
        table[PE_TBL_RESOURCE].rva = 0;
        table[PE_TBL_RESOURCE].size = 0;
        error = false;
    } else {
        if( IS_PE64( *pehdr ) ) {
            res_obj = exe->u.PEInfo.Objects + PE64( *pehdr ).num_objects - 1;
        } else {
            res_obj = exe->u.PEInfo.Objects + PE32( *pehdr ).num_objects - 1;
        }
        rva = GetNextObjRVA( &exe->u.PEInfo );
        offset = GetNextObjPhysOffset( &exe->u.PEInfo );
        error = BuildPEResourceObject( exe, Pass2Info.ResFiles, res_obj, rva, offset, !Pass2Info.AllResFilesOpen );
// use of CmdLineParms.WritableRes has been commented out in param.c
// removed here too as it wasn't initialised anymore (Ernest ter Kuile 31 aug 2003)
//        if( CmdLineParms.WritableRes ) {
//            res_obj->flags |= PE_OBJ_WRITABLE;
//        }
    }
    return( error );
}
#endif
