/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
#include <errno.h>
#include "global.h"
#include "rcstrblk.h"
#include "rcstr.h"
#include "rcerrors.h"
#include "rcrtns.h"
#include "rccore_2.h"
#include "mergedir.h"
#include "exeobj.h"
#include "exeutil.h"
#include "exerespe.h"
#include "memfuncs.h"

#include "clibext.h"


#if !defined( INSIDE_WLINK ) || defined( _OS2 )

#define RESOURCE_OBJECT_NAME ".rsrc"

#ifndef MAKELANGID
#define MAKELANGID(p, s)       ( ( ( (uint_16)(s) ) << 10 ) | (uint_16)(p) )
#endif

/*
 * structures and routines to manipulate a queue of PEResDirEntry *
 * This uses a linked list representation despite the overhead of the pointer
 * since the total number of entries in the queue should be small and this is
 * easier to code while still being dynamic
 */

typedef struct QueueNode {
    struct QueueNode    *next;
    PEResDirEntry       *direntry;
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
        MemFree( curr );
    }

    QueueInit( queue );
} /* QueueEmpty */

static bool QueueIsEmpty( DirEntryQueue *queue )
/**********************************************/
{
    return( queue->front == NULL );
}

static void QueueAdd( DirEntryQueue *queue, PEResDirEntry *direntry )
/*******************************************************************/
{
    QueueNode       *new;

    new = MemAllocSafe( sizeof( QueueNode ) );
    new->direntry = direntry;
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
    PEResDirEntry   *direntry;

    old = queue->front;
    if( old == NULL ) {
        return( NULL );
    }

    queue->front = old->next;
    if( queue->front == NULL ) {
        queue->back = NULL;
    }

    direntry = old->direntry;
    MemFree( old );
    return( direntry );
} /* QueueRemove */

static void PEResDirEntryInit( PEResDirEntry *direntry, int num_entries )
/***********************************************************************/
{
    direntry->Head.flags = 0;
    direntry->Head.time_stamp = (unsigned_32)time( NULL );
    direntry->Head.major = 0;
    direntry->Head.minor = 0;
    direntry->Head.num_name_entries = 0;
    direntry->Head.num_id_entries = 0;
    direntry->NumUnused = num_entries;
    direntry->Children = MemAllocSafe( num_entries * sizeof( PEResEntry ) );
}

static void PEResDirAdd( PEResDirEntry *direntry, WResID *res_id, StringsBlock *strings )
/***************************************************************************************/
{
    int             entry_num;
    int_32          name_off;
    PEResEntry      *entry;


    entry_num = direntry->Head.num_name_entries + direntry->Head.num_id_entries;
    entry = direntry->Children + entry_num;
    if( res_id->IsName ) {
        name_off = StringBlockFind( strings, &res_id->ID.Name );
        if( name_off == -1 ) {
            /*
             * this case should not happen
             */
            entry->Entry.id_name = PE_RESOURCE_MASK_ON | 0;
            entry->Name = NULL;
        } else {
            /*
             * This value will be changed later when we know the size of the
             * the resource directory
             */
            entry->Entry.id_name = PE_RESOURCE_MASK_ON | name_off;
            entry->Name = strings->StringBlock + name_off;
        }
        direntry->Head.num_name_entries++;
    } else {
        entry->Entry.id_name = res_id->ID.Num;
        entry->Name = NULL;
        direntry->Head.num_id_entries++;
    }
    direntry->NumUnused--;
}


static bool PEResDirAddDir( PEResDirEntry *direntry, WResID *res_id,
                    int num_sub_entries, StringsBlock *strings )
/**************************************************************/
{
    int             entry_num;
    PEResEntry      *entry;

    if( direntry->NumUnused <= 0 )
        return( true );

    PEResDirAdd( direntry, res_id, strings );

    entry_num = direntry->Head.num_name_entries + direntry->Head.num_id_entries - 1;
    entry = direntry->Children + entry_num;
    entry->IsDirEntry = true;
    PEResDirEntryInit( &entry->u.Dir, num_sub_entries );

    return( false );
}

static bool PEResDirAddData( PEResDirEntry *direntry, WResID *res_id,
                    WResDirWindow wind, StringsBlock *strings )
/***************************************************************/
{
    int             entry_num;
    PEResEntry      *entry;

    if( direntry->NumUnused <= 0 )
        return( true );

    PEResDirAdd( direntry, res_id, strings );

    entry_num = direntry->Head.num_name_entries + direntry->Head.num_id_entries - 1;
    entry = direntry->Children + entry_num;
    entry->IsDirEntry = false;
    entry->u.Data.Wind = wind;
    /*
     * The Data.Entry field will be filled in as the resource is writen
     */
    return( false );
}

static bool AddType( PEResDir *pedir, WResTypeInfo *typeinfo )
/************************************************************/
{
    return( PEResDirAddDir( &pedir->Root, &typeinfo->TypeName, typeinfo->NumResources, &pedir->String ) );
}

static bool AddLang( PEResDir *pedir, WResDirWindow wind )
/********************************************************/
{
    int                 entry_num;
    PEResEntry          *entry_res;
    PEResEntry          *entry_type;
    WResLangInfo        *langinfo;
    WResID              lang_id;

    langinfo = WResGetLangInfo( wind );
    /*
     * find the current type
     */
    entry_num = pedir->Root.Head.num_name_entries
                + pedir->Root.Head.num_id_entries - 1;
    entry_type = pedir->Root.Children + entry_num;
    /*
     * find the current resource
     */
    entry_num = entry_type->u.Dir.Head.num_name_entries
                        + entry_type->u.Dir.Head.num_id_entries - 1;
    entry_res = entry_type->u.Dir.Children + entry_num;

    lang_id.IsName = false;
    lang_id.ID.Num = MAKELANGID( langinfo->lang.lang, langinfo->lang.sublang );
    if( PEResDirAddData( &entry_res->u.Dir, &lang_id, wind, &pedir->String ) ) {
        return( true );
    }
    return( false );
}

static bool AddRes( PEResDir *pedir, WResDirWindow wind )
/*******************************************************/
{
    int             entry_num;
    PEResEntry      *entry_type;
    WResResInfo     *resinfo;

    resinfo = WResGetResInfo( wind );
    /*
     * find the current type
     */
    entry_num = pedir->Root.Head.num_name_entries + pedir->Root.Head.num_id_entries;
    entry_type = pedir->Root.Children + entry_num - 1;
    /*
     * Add a directory level for the languages
     */
    if( PEResDirAddDir( &entry_type->u.Dir, &resinfo->ResName, resinfo->NumResources, &pedir->String ) ) {
        return( true );
    }

    return( false );
}


static bool PEResDirBuild( PEResDir *pedir, WResDir dir )
/*******************************************************/
{
    WResDirWindow   wind;

    if( WResIsEmpty( dir ) ) {
        pedir->Root.Head.num_name_entries = 0;
        pedir->Root.Head.num_id_entries = 0;
        pedir->Root.Children = NULL;
        pedir->ResRVA = 0;
        pedir->ResOffset = 0;
        pedir->ResSize = 0;
    } else {
        StringIDNamesBlockBuild( &pedir->String, dir, true );
        pedir->DirSize = sizeof( resource_dir_header );
        PEResDirEntryInit( &pedir->Root, dir->NumTypes );
        for( wind = WResFirstResource( dir ); !WResIsEmptyWindow( wind ); wind = WResNextResource( wind, dir ) ) {
            if( WResIsFirstResOfType( wind ) ) {
                if( AddType( pedir, WResGetTypeInfo( wind ) ) ) {
                    return( true );
                }
            }
            if( WResIsFirstLangOfRes( wind ) ) {
                if( AddRes( pedir, wind ) ) {
                    return( true );
                }
            }
            AddLang( pedir, wind );
        }
    }
    return( false );
}

static RcStatus traverseTree( PEResDir *pedir, void *visit_data,
                    RcStatus (*visit)( PEResEntry *entry, void *visit_data ) )
/*****************************************************************************
 * NB when a visit function returns an error this function MUST return
 *    without altering errno
 *
 * Perfroms a level order traversal of a PEResDir tree calling visit at
 * each entry
 */
{
    PEResEntry      *entry;
    PEResEntry      *entry_last;
    PEResDirEntry   *direntry;
    DirEntryQueue   queue;
    RcStatus        ret;

    QueueInit( &queue );

    QueueAdd( &queue, &pedir->Root );

    while( !QueueIsEmpty( &queue ) ) {
        direntry = QueueRemove( &queue );
        entry_last = direntry->Children + direntry->Head.num_name_entries + direntry->Head.num_id_entries;
        for( entry = direntry->Children; entry < entry_last; entry++ ) {
            ret = visit( entry, visit_data );
            if( ret != RS_OK )
                return( ret );
            if( entry->IsDirEntry ) {
                QueueAdd( &queue, &entry->u.Dir );
            }
        }
    }

    QueueEmpty( &queue );

    return( RS_OK );
} /* traverseTree */

static RcStatus SetEntryOffset( PEResEntry *entry, void *_curr_offset )
/*********************************************************************/
{
    int     num_entries;
    uint_32 *curr_offset = _curr_offset;

    if( entry->IsDirEntry ) {
        entry->Entry.entry_rva = *curr_offset | PE_RESOURCE_MASK_ON;
        num_entries = entry->u.Dir.Head.num_name_entries + entry->u.Dir.Head.num_id_entries;
        *curr_offset += sizeof( resource_dir_header ) + num_entries * sizeof( resource_dir_entry );
    } else {
        entry->Entry.entry_rva = *curr_offset;
        *curr_offset += sizeof( resource_entry );
    }
    return( RS_OK );
} /* SetEntryOffset */

static RcStatus AdjustNameEntry( PEResEntry *entry, void *_dir_size )
/*******************************************************************/
{
    uint_32     str_offset;
    uint_32    *dir_size = _dir_size;

    if( entry->Entry.id_name & PE_RESOURCE_MASK_ON ) {
        /*
         * the id_name contains the offset into the string block
         */
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

static RcStatus SortDirEntry( PEResEntry *entry, void *dummy )
/************************************************************/
{
    int     num_entries;

    /* unused parameters */ (void)dummy;

    if( entry->IsDirEntry ) {
        num_entries = entry->u.Dir.Head.num_name_entries + entry->u.Dir.Head.num_id_entries;
        qsort( entry->u.Dir.Children, num_entries, sizeof( PEResEntry ), ComparePEResIdName );
    }
    return( RS_OK );
} /* SortDirEntry */

static void CompleteTree( PEResDir *pedir )
/*****************************************/
{
    uint_32     curr_offset;
    int         num_entries;

    num_entries = pedir->Root.Head.num_name_entries + pedir->Root.Head.num_id_entries;
    /*
     * sort the entries at each level
     */
    qsort( pedir->Root.Children, num_entries, sizeof( PEResEntry ), ComparePEResIdName );
    traverseTree( pedir, NULL, SortDirEntry );
    /*
     * Set curr_offset to the size of the root entry
     */
    curr_offset = sizeof( resource_dir_header ) + num_entries * sizeof( resource_dir_entry );

    traverseTree( pedir, &curr_offset, SetEntryOffset );
    pedir->DirSize = curr_offset;

    traverseTree( pedir, &curr_offset, AdjustNameEntry );
} /* CompleteTree */

typedef struct CopyResInfo {
    FILE                *to_fp;
    ExeFileInfo         *file;  /* for setting debugging offset */
    ResFileInfo         *curres;
    ResFileInfo         *errres;
} CopyResInfo;

static RcStatus copyDataEntry( PEResEntry *entry, void *_copy_info )
/*******************************************************************
 * NB when an error occurs this function MUST return without altering errno
 */
{
    CopyResInfo         *copy_info = _copy_info;
    WResLangInfo        *langinfo;
    uint_32             diff;
    RcStatus            ret;
    ResFileInfo         *info;
//    bool                closefile;

//    closefile = false;
    if( !entry->IsDirEntry ) {
        info = WResGetFileInfo( entry->u.Data.Wind );
        if( copy_info->curres == NULL
          || copy_info->curres == info ) {
            langinfo = WResGetLangInfo( entry->u.Data.Wind );
            if( RESSEEK( info->fp, langinfo->Offset, SEEK_SET ) )
                return( RS_READ_ERROR );
            ret = CopyExeData( info->fp, copy_info->to_fp, langinfo->Length );
            if( ret != RS_OK ) {
                copy_info->errres = info;
                return( ret );
            }
            diff = ALIGN_VALUE( langinfo->Length, sizeof( uint_32 ) );
            if( diff > langinfo->Length ) {
                /*
                 * add the padding
                 */
                if( RcPadFile( copy_info->to_fp, (size_t)( diff - langinfo->Length ) ) ) {
                    return( RS_WRITE_ERROR );
                }
            }
            CheckDebugOffset( copy_info->file );
        }
    }

    return( RS_OK );
} /* copyDataEntry */

static RcStatus copyPEResources( ExeFileInfo *dst, ResFileInfo *resfiles,
                                FILE *to_fp, bool writebyfile,
                                ResFileInfo **errres )
/************************************************************************
 * NB when an error occurs this function MUST return without altering errno
 */
{
    CopyResInfo     copy_info;
//    pe_va           start_rva;
    uint_32         start_off;
    RcStatus        ret;

//    start_rva = dst->u.PEInfo.Res.ResRVA + dst->u.PEInfo.Res.DirSize + dst->u.PEInfo.Res.String.StringBlockSize;
    start_off = dst->u.PEInfo.Res.ResOffset + dst->u.PEInfo.Res.DirSize + dst->u.PEInfo.Res.String.StringBlockSize;

    copy_info.to_fp = to_fp;
    copy_info.errres = NULL;
    copy_info.file = dst;       /* for tracking debugging info offset */
    start_off = ALIGN_VALUE( start_off, sizeof( uint_32 ) );

    if( RESSEEK( to_fp, start_off, SEEK_SET ) )
        return( RS_WRITE_ERROR );
    if( !writebyfile ) {
        copy_info.curres = NULL;
        ret = traverseTree( &dst->u.PEInfo.Res, &copy_info, copyDataEntry );
        *errres = copy_info.errres;
    } else {
        ret = RS_OK;
        for( ; resfiles != NULL; resfiles = resfiles->next ) {
            copy_info.curres = resfiles;
            if( resfiles->fp != NULL ) {
                ret = traverseTree( &dst->u.PEInfo.Res, &copy_info, copyDataEntry );
            } else {
                ret = RS_OPEN_ERROR;
                resfiles->fp = ResOpenFileRO( resfiles->name );
                if( resfiles->fp != NULL ) {
                    ret = traverseTree( &dst->u.PEInfo.Res, &copy_info, copyDataEntry );
                    RCCloseFile( &(resfiles->fp) );
                }
            }
            if( ret != RS_OK ) {
                *errres = resfiles;
                break;
            }
        }
    }
    return( ret );
} /* copyPEResources */

static RcStatus writeDirEntry( PEResDirEntry *direntry, FILE *fp )
/*****************************************************************
 * NB when an error occurs this function MUST return without altering errno
 */
{
    int     child_num;

    if( RESWRITE( fp, &direntry->Head, sizeof( resource_dir_header ) ) != sizeof( resource_dir_header ) )
        return( RS_WRITE_ERROR );

    for( child_num = 0; child_num < direntry->Head.num_name_entries + direntry->Head.num_id_entries; child_num++ ) {
        if( RESWRITE( fp, direntry->Children + child_num, sizeof( resource_dir_entry ) ) != sizeof( resource_dir_entry ) ) {
            return( RS_WRITE_ERROR );
        }
    }

    return( RS_OK );
} /* writeDirEntry */

static RcStatus writeDataEntry( PEResDataEntry *entry, FILE *fp )
/****************************************************************
 * NB when an error occurs this function MUST return without altering errno
 */
{
    if( RESWRITE( fp, &entry->Entry, sizeof( resource_entry ) ) != sizeof( resource_entry ) )
        return( RS_WRITE_ERROR );
    return( RS_OK );
} /* writeDataEntry */

typedef struct {
    ResFileInfo         *curfile;
    unsigned_32         *rva;
} DataEntryCookie;

static RcStatus setDataEntry( PEResEntry *entry, void *_info )
/************************************************************/
{
    WResLangInfo        *langinfo;
    ResFileInfo         *fileinfo;
    DataEntryCookie     *info = _info;

    if( !entry->IsDirEntry ) {
        fileinfo = WResGetFileInfo( entry->u.Data.Wind );
        if( info->curfile == NULL
          || info->curfile == fileinfo ) {
            langinfo = WResGetLangInfo( entry->u.Data.Wind );
            entry->u.Data.Entry.data_rva = *info->rva;
            entry->u.Data.Entry.size = langinfo->Length;
            entry->u.Data.Entry.code_page = 0;    /* should this be the UNICODE page*/
            entry->u.Data.Entry.rsvd = 0;
            *info->rva = *info->rva + ALIGN_VALUE( langinfo->Length, sizeof( uint_32 ) );
        }
    }
    return( RS_OK );
}

static RcStatus writeEntry( PEResEntry *entry, void *fp )
/********************************************************
 * NB when an error occurs this function MUST return without altering errno
 */
{
    if( entry->IsDirEntry ) {
        return( writeDirEntry( &entry->u.Dir, *(FILE **)fp ) );
    } else {
        return( writeDataEntry( &entry->u.Data, *(FILE **)fp ) );
    }
} /* writeEntry */

static RcStatus writeDirectory( PEResDir *pedir, FILE *fp )
/**********************************************************
 * NB when an error occurs this function MUST return without altering errno
 */
{
    RcStatus    ret;

    if( RESSEEK( fp, pedir->ResOffset, SEEK_SET ) )
        return( RS_WRITE_ERROR );
    /*
     * write the root entry header
     */
    ret = writeDirEntry( &pedir->Root, fp );
    if( ret != RS_OK )
        return( ret );

    ret = traverseTree( pedir, &fp, writeEntry );
    if( ret != RS_OK )
        return( ret );

    if( pedir->String.StringBlock != NULL ) {
        if( RESWRITE( fp, pedir->String.StringBlock, pedir->String.StringBlockSize ) != pedir->String.StringBlockSize ) {
            return( RS_WRITE_ERROR );
        }
    }

    return( RS_OK );
} /* writeDirectory */

static void FreeSubDir( PEResDirEntry *direntry )
/***********************************************/
{
    int             num_children;
    PEResEntry      *entry_last;
    PEResEntry      *entry;

    num_children = direntry->Head.num_id_entries + direntry->Head.num_name_entries;
    entry_last = direntry->Children + num_children;
    for( entry = direntry->Children; entry < entry_last; ++entry ) {
        if( entry->IsDirEntry ) {
            FreeSubDir( &entry->u.Dir );
        }
    }

    MemFree( direntry->Children );
}

static void FreePEResDir( PEResDir *pedir )
/*****************************************/
{
    FreeSubDir( &pedir->Root );
    if( pedir->String.StringBlock != NULL )
        MemFree( pedir->String.StringBlock );
    if( pedir->String.StringList != NULL ) {
        MemFree( pedir->String.StringList );
    }
}

#if !defined( INSIDE_WLINK )
bool RcPadFile( FILE *fp, size_t pad )
/************************************/
{
    char        zero = 0;

    if( pad > 0 ) {
        if( RESSEEK( fp, pad - 1, SEEK_CUR ) ) {
            return( true );
        }
        if( RESWRITE( fp, &zero, 1 ) != 1 )  {
            return( true );
        }
    }
    return( false );
}
#endif

static bool padObject( PEResDir *pedir, ExeFileInfo *dst, long size )
/********************************************************************
 * NB when an error occurs this function MUST return without altering errno
 */
{
    long        pos;
    long        pad;

    pos = RESTELL( dst->fp );
    if( pos == -1 )
        return( true );
    pad = pedir->ResOffset + size - pos;
    if( pad > 0 ) {
        RcPadFile( dst->fp, (size_t)pad );
    }
    CheckDebugOffset( dst );
    return( false );
#if( 0)
    char        zero=0;

    if( RESSEEK( dst->fp, pedir->ResOffset, SEEK_SET ) )
        return( true );
    if( RESSEEK( dst->fp, size - 1, SEEK_CUR ) )
        return( true );
    if( RESWRITE( dst->fp, &zero, 1 ) != 1 )
        return( true );
    CheckDebugOffset( dst );
    return( false );
#endif
}

static void fillResourceObj( pe_object *res_obj, PEResDir *pedir,
                             uint_32 alignment )
/****************************************************************/
{
    strncpy( res_obj->name, RESOURCE_OBJECT_NAME, PE_OBJ_NAME_LEN );
    res_obj->virtual_size = 0;
    res_obj->rva = pedir->ResRVA;
    res_obj->physical_size = ALIGN_VALUE( pedir->ResSize, alignment );
    res_obj->physical_offset = pedir->ResOffset;
    res_obj->relocs_rva = 0;
    res_obj->linnum_rva = 0;
    res_obj->num_relocs = 0;
    res_obj->num_linnums = 0;
    res_obj->flags = PE_OBJ_INIT_DATA | PE_OBJ_READABLE;
}

static void setDataOffsets( PEResDir *pedir, unsigned_32 *curr_rva,
                                ResFileInfo *resfiles, bool writebyfile )
/****************************************************************/
{
    DataEntryCookie     cookie;

    cookie.rva = curr_rva;
    if( writebyfile ) {
        for( ; resfiles != NULL; resfiles = resfiles->next ) {
            cookie.curfile = resfiles;
            traverseTree( pedir, &cookie, setDataEntry );
        }
    } else {
        cookie.curfile = NULL;
        traverseTree( pedir, &cookie, setDataEntry );
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

bool BuildPEResourceObject( ExeFileInfo *dst, ResFileInfo *resfiles,
                                pe_object *res_obj, unsigned_32 rva,
                                unsigned_32 offset, bool writebyfile )
/*********************************************************************
 * here is linker entry point for building resources in executable
 * image
 */
{
    PEResDir            *pedir;
    RcStatus            ret;
    unsigned_32         curr_rva;
    WResMergeError      *errs;
    ResFileInfo         *errres;
    pe_exe_header       *pehdr;

    pedir = &dst->u.PEInfo.Res;

    MergeDirectory( resfiles, &errs );
    if( errs != NULL ) {
        reportDuplicateResources( errs );
        WResFreeMergeErrors( errs );
        return( true );
    }
    if( PEResDirBuild( pedir, resfiles->Dir ) ) {
        RcError( ERR_INTERNAL, INTERR_ERR_BUILDING_RES_DIR );
        return( true );
    }
    CompleteTree( pedir );
    dst->u.PEInfo.Res.ResOffset = offset;
    dst->u.PEInfo.Res.ResRVA = rva;
    curr_rva = rva + dst->u.PEInfo.Res.DirSize + dst->u.PEInfo.Res.String.StringBlockSize;
    curr_rva = ALIGN_VALUE( curr_rva, sizeof( uint_32 ) );
    setDataOffsets( pedir, &curr_rva, resfiles, writebyfile );
    ret = writeDirectory( pedir, dst->fp );
    if( ret != RS_OK ) {
        RcError( ERR_WRITTING_FILE, dst->name, strerror( errno ) );
        return( true );
    }

    ret = copyPEResources( dst, resfiles, dst->fp, writebyfile, &errres );
    /*
     * warning - the file names output in these messages could be
     *          incorrect if the -fr switch is in use
     */
    if( ret != RS_OK  ) {
        if( ret == RS_READ_ERROR ) {
            ret = RS_READ_ERROR_RES;
        }
        RcIOError( ret, errres->name, dst->name, errno );
        return( true );
    }
    dst->u.PEInfo.Res.ResSize = curr_rva - rva;

    pehdr = dst->u.PEInfo.WinHead;
    fillResourceObj( res_obj, pedir, PE( *pehdr, file_align ) );
    if( padObject( pedir, dst, res_obj->physical_size ) ) {
        RcError( ERR_WRITTING_FILE, dst->name, strerror( errno ) );
        return( true );
    }
    /*
     * set the resource element of the table in the header
     */
    PE_DIRECTORY( *pehdr, PE_TBL_RESOURCE ).rva = res_obj->rva;
    PE_DIRECTORY( *pehdr, PE_TBL_RESOURCE ).size = res_obj->physical_size;

    FreePEResDir( pedir );

    return( false );
} /* BuildPEResourceObject */


#if !defined( INSIDE_WLINK )
bool RcBuildPEResourceObject( ExeFileInfo *dst, ResFileInfo *resfiles )
/*********************************************************************/
{
    pe_object           *res_obj;
    unsigned_32         rva;
    unsigned_32         offset;
    bool                error;
    pe_exe_header       *pehdr;

    pehdr = dst->u.PEInfo.WinHead;
    if( CmdLineParms.NoResFile ) {
        PE_DIRECTORY( *pehdr, PE_TBL_RESOURCE ).rva = 0;
        PE_DIRECTORY( *pehdr, PE_TBL_RESOURCE ).size = 0;
        error = false;
    } else {
        res_obj = dst->u.PEInfo.Objects + pehdr->fheader.num_objects - 1;
        rva = GetNextObjRVA( &dst->u.PEInfo );
        offset = GetNextObjPhysOffset( &dst->u.PEInfo );
        error = BuildPEResourceObject( dst, resfiles, res_obj, rva, offset, !Pass2Info.AllResFilesOpen );
        /*
         * use of CmdLineParms.WritableRes has been commented out in param.c
         * removed here too as it wasn't initialised anymore (Ernest ter Kuile 31 aug 2003)
         */
//        if( CmdLineParms.WritableRes ) {
//            res_obj->flags |= PE_OBJ_WRITABLE;
//        }
    }
    return( error );
}
#endif  /* !defined( INSIDE_WLINK ) */

#endif  /* !defined( INSIDE_WLINK ) || defined( _OS2 ) */
