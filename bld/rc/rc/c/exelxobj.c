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
* Description:  LX objects and object table manipulation routines.
*
****************************************************************************/


#include "global.h"
#include <errno.h>
#include "rcerrors.h"
#include "rcrtns.h"
#include "rccore_2.h"
#include "exelxobj.h"
#include "exeutil.h"


static bool readObjectAndPageTable( ExeFileInfo *exe )
/****************************************************/
{
    RcStatus    ret;
    size_t      table_size;

    table_size = exe->u.LXInfo.OS2Head.num_objects * sizeof( object_record );
    exe->u.LXInfo.Objects = RESALLOC( table_size );
    ret = SeekRead( exe->fp, exe->WinHeadOffset + exe->u.LXInfo.OS2Head.objtab_off,
                exe->u.LXInfo.Objects, table_size );

    if( ret == RS_OK ) {
        table_size = exe->u.LXInfo.OS2Head.num_pages * sizeof( lx_map_entry );
        exe->u.LXInfo.Pages = RESALLOC( table_size );
        ret = SeekRead( exe->fp, exe->WinHeadOffset + exe->u.LXInfo.OS2Head.objmap_off,
                    exe->u.LXInfo.Pages, table_size );
    }
    switch( ret ) {
    case RS_OK:
        break;
    case RS_READ_ERROR:
        RcError( ERR_READING_EXE, exe->name, strerror( errno ) );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, exe->name );
        break;
    default:
        RcError( ERR_INTERNAL, INTERR_UNKNOWN_RCSTATUS );
        break;
    }
    CheckDebugOffset( exe );
    return( ret != RS_OK );
}

static int copyObjectAndPageTable( ExeFileInfo *old, ExeFileInfo *new )
/*********************************************************************/
/* Copies the object/page table from old to new sans resource objects */
{
    uint_32         old_obj_size;
    uint_32         new_obj_size;
    uint_32         old_page_size;
    uint_32         new_page_size;
    uint_32         align;
    int_32          new_off;
    object_record   *old_obj;
    object_record   *new_obj;
    lx_map_entry    *old_page;
    lx_map_entry    *new_page;
    unsigned        obj_index;
    int             old_obj_index;
    int             old_num_objects;
    int             page_index;
    int             old_num_pages;
    uint_32         i;

    old_obj  = old->u.LXInfo.Objects;
    old_page = old->u.LXInfo.Pages;
    old_num_objects = old_num_pages = 0;
    /* Figure out number of old objects/pages */
    for( obj_index = 0; obj_index < old->u.LXInfo.OS2Head.num_objects; obj_index++ ) {
        /* Simply skip any existing resource objects */
        if( (old_obj[obj_index].flags & OBJ_RESOURCE) == 0 ) {
            ++old_num_objects;
            old_num_pages += old_obj[obj_index].mapsize;
        }
    }
    if( CmdLineParms.NoResFile ) {
        new->u.LXInfo.OS2Head.num_objects = old_num_objects;
        new->u.LXInfo.OS2Head.num_pages   = old_num_pages;
    } else {
        new->u.LXInfo.OS2Head.num_objects = old_num_objects
                            + new->u.LXInfo.Res.num_objects;
        new->u.LXInfo.OS2Head.num_pages   = old_num_pages
                            + new->u.LXInfo.Res.num_pages;
    }
    new_obj_size = new->u.LXInfo.OS2Head.num_objects * sizeof( object_record );
    old_obj_size = old->u.LXInfo.OS2Head.num_objects * sizeof( object_record );
    new_page_size = new->u.LXInfo.OS2Head.num_pages * sizeof( lx_map_entry );
    old_page_size = old->u.LXInfo.OS2Head.num_pages * sizeof( lx_map_entry );

    /* Calculate new offset of data pages */
    new_off = new_obj_size + new_page_size - old_obj_size - old_page_size
            + new->u.LXInfo.OS2Head.num_rsrcs * sizeof( flat_res_table )
            - old->u.LXInfo.OS2Head.num_rsrcs * sizeof( flat_res_table )
            + new->u.LXInfo.OS2Head.num_pages * sizeof( uint_32 )
            - old->u.LXInfo.OS2Head.num_pages * sizeof( uint_32 );

    new_off += old->u.LXInfo.OS2Head.page_off;
    align = 1 << old->u.LXInfo.OS2Head.l.page_shift;
    new_off = (new_off + align - 1) & ~(align - 1);
    new->u.LXInfo.OS2Head.page_off = new_off;

    /* Allocate new object/page table */
    new_obj  = RESALLOC( new_obj_size );
    new_page = RESALLOC( new_page_size );
    new->u.LXInfo.Objects = new_obj;
    new->u.LXInfo.Pages   = new_page;

    old_obj_index = page_index = 0;

    /* Copy object and page records from old executable to new */
    for( obj_index = 0; obj_index < old->u.LXInfo.OS2Head.num_objects; obj_index++ ) {
        if( (old_obj[obj_index].flags & OBJ_RESOURCE) == 0 ) {
            new_obj[obj_index] = old_obj[old_obj_index];
            new_obj[obj_index].mapidx = page_index + 1;
            for( i = 0; i < old_obj[old_obj_index].mapsize; ++i ) {
                new_page[page_index] = old_page[old_obj[old_obj_index].mapidx + i - 1];
                ++page_index;
            }
            ++old_obj_index;
        }
    }

    /* Mark the start of resource objects/pages */
    new->u.LXInfo.FirstResObj  = old_obj_index;
    new->u.LXInfo.FirstResPage = page_index;

    return( old_num_objects );
}


/*
 * copyOneObject
 * if an error occurs this function MUST return without altering errno
 */
static RcStatus copyOneObject( ExeFileInfo *old, object_record *old_obj,
                               ExeFileInfo *new, object_record *new_obj )
/***********************************************************************/
{
    RcStatus        ret;
    lx_map_entry    *old_map;
    lx_map_entry    *new_map;
    uint_32         old_offset;
    uint_32         new_offset;
    unsigned        i;

    for( i = 0; i < old_obj->mapsize; ++i ) {
        old_map = &old->u.LXInfo.Pages[old_obj->mapidx + i - 1];
        new_map = &new->u.LXInfo.Pages[new_obj->mapidx + i - 1];

        // NB - page_offset is relative to start of executable, not to LX header!
        old_offset = (old_map->page_offset << old->u.LXInfo.OS2Head.l.page_shift)
                    + old->u.LXInfo.OS2Head.page_off;
        new_offset = (new_map->page_offset << new->u.LXInfo.OS2Head.l.page_shift)
                    + new->u.LXInfo.OS2Head.page_off;
        if( RESSEEK( old->fp, old_offset, SEEK_SET ) )
            return( RS_READ_ERROR );
        if( RESSEEK( new->fp, new_offset, SEEK_SET ) )
            return( RS_WRITE_ERROR );

        ret = CopyExeData( old->fp, new->fp, old_map->data_size );
        if( ret != RS_OK ) {
            return( ret );
        }
    }
    return( RS_OK );
}


/*
 * copyHeaderSections
 * if an error occurs this function MUST return without altering errno
 */
static RcStatus copyHeaderSections( ExeFileInfo *old, ExeFileInfo *new )
/**********************************************************************/
/* Copies parts of header and loader/fixup sections that won't be changing */
{
    RcStatus            ret;
    uint_32             old_pages;
    uint_32             offset;
    uint_32             length;
    uint_32             lx_off;
    os2_flat_header     *old_head;
    os2_flat_header     *new_head;
    size_t              numread;

    lx_off = new->WinHeadOffset;
    old_head = &old->u.LXInfo.OS2Head;
    new_head = &new->u.LXInfo.OS2Head;

    /* Leave room for object table, object page table and resource
     * table to be written later
     */
    offset = sizeof( os2_flat_header )
           + new_head->num_objects * sizeof( object_record )
           + new_head->num_pages * sizeof( lx_map_entry )
           + new_head->num_rsrcs * sizeof( flat_res_table );
    if( RESSEEK( new->fp, lx_off + offset, SEEK_SET ) )
        return( RS_WRITE_ERROR );

    new_head->signature    = old_head->signature;
    new_head->byte_order   = old_head->byte_order;
    new_head->word_order   = old_head->word_order;
    new_head->level        = old_head->level;
    new_head->cpu_type     = old_head->cpu_type;
    new_head->os_type      = old_head->os_type;
    new_head->version      = old_head->version;
    new_head->flags        = old_head->flags;
    new_head->start_obj    = old_head->start_obj;
    new_head->eip          = old_head->eip;
    new_head->stack_obj    = old_head->stack_obj;
    new_head->esp          = old_head->esp;
    new_head->page_size    = old_head->page_size;
    new_head->l.page_shift = old_head->l.page_shift;
    new_head->autodata_obj = old_head->autodata_obj;
    new_head->heapsize     = old_head->heapsize;
    new_head->stacksize    = old_head->stacksize;

    new_head->objtab_off   = sizeof( os2_flat_header );
    new_head->objmap_off   = new_head->objtab_off
                           + new_head->num_objects * sizeof( object_record );
    new_head->rsrc_off     = new_head->objmap_off
                           + new_head->num_pages * sizeof( lx_map_entry );

    // copy resident names table if provided
    if( old_head->entry_off > old_head->resname_off ) {
        new_head->resname_off  = new_head->rsrc_off
                               + new_head->num_rsrcs * sizeof( flat_res_table );
        if( RESSEEK( old->fp, lx_off + old_head->resname_off, SEEK_SET ) )
            return( RS_READ_ERROR );

        length = old_head->entry_off - old_head->resname_off;
        ret = CopyExeData( old->fp, new->fp, length );
        if( ret != RS_OK ) {
            return( ret );
        }
    } else {
        new_head->resname_off = 0;
        length = 0;
    }
    offset += length;

    // copy entry table if provided
    if( old_head->entry_off ) {
        new_head->entry_off = offset;
        if( old_head->moddir_off ) {
            length = old_head->moddir_off - old_head->entry_off;
        } else {
            length = old_head->fixpage_off - old_head->entry_off;
        }
        if( RESSEEK( old->fp, lx_off + old_head->entry_off, SEEK_SET ) )
            return( RS_READ_ERROR );

        ret = CopyExeData( old->fp, new->fp, length );
        if( ret != RS_OK ) {
            return( ret );
        }
    } else {
        new_head->entry_off = 0;
        length = 0;
    }
    offset += length;

    // now we can determine loader section size
    new_head->loader_size  = offset - sizeof( os2_flat_header );
    new_head->loader_cksum = 0;

    // copy fixup section if provided
    if( old_head->fixup_size ) {
        int_32      delta;
        uint_32     last_fix_pg;
        uint_32     new_fixpg_length;
        uint_32     old_fixpg_length;
        unsigned    i;

        // the fixup page table must be extended if resource pages were added
        old_pages = new_head->num_pages - new->u.LXInfo.Res.num_pages;
        length = sizeof( uint_32 ) * old_pages;
        if( RESSEEK( old->fp, lx_off + old_head->fixpage_off, SEEK_SET ) )
            return( RS_READ_ERROR );

        ret = CopyExeData( old->fp, new->fp, length );
        if( ret != RS_OK )
            return( ret );

        // read the last entry of fixup page table
        numread = RESREAD( old->fp, &last_fix_pg, sizeof( last_fix_pg ) );
        if( numread != sizeof( last_fix_pg ) )
            return( RESIOERR( old->fp, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );

        // replicate for each added page (plus one for final entry)
        for( i = 0; i < new->u.LXInfo.Res.num_pages + 1; i++ ) {
            if( RESWRITE( new->fp, &last_fix_pg, sizeof( last_fix_pg ) ) != sizeof( last_fix_pg ) ) {
                return( RS_WRITE_ERROR );
            }
        }

        // calculate new fixup table size
        old_fixpg_length = (old_head->num_pages + 1) * sizeof( uint_32 );
        new_fixpg_length = (new_head->num_pages + 1) * sizeof( uint_32 );
        new_head->fixup_size   = old_head->fixup_size
                               - old_fixpg_length + new_fixpg_length;
        new_head->fixup_cksum  = 0;

        delta = offset - old_head->fixpage_off;
        new_head->fixpage_off  = old_head->fixpage_off + delta;

        delta = delta - old_fixpg_length + new_fixpg_length;
        new_head->fixrec_off   = old_head->fixrec_off + delta;
        new_head->impmod_off   = old_head->impmod_off + delta;
        new_head->num_impmods  = old_head->num_impmods;
        new_head->impproc_off  = old_head->impproc_off + delta;

        // copy the rest of fixup section
        offset += new_fixpg_length;
        length = old_head->fixup_size - old_fixpg_length;
        if( RESSEEK( old->fp, lx_off + old_head->fixpage_off + old_fixpg_length, SEEK_SET ) )
            return( RS_READ_ERROR );

        ret = CopyExeData( old->fp, new->fp, length );
        if( ret != RS_OK ) {
            return( ret );
        }
    } else {
        new_head->fixup_size   = 0;
        new_head->fixup_cksum  = 0;
        new_head->fixpage_off  = 0;
        new_head->fixrec_off   = 0;
        new_head->impmod_off   = 0;
        new_head->num_impmods  = 0;
        new_head->impproc_off  = 0;
        length = 0;
    }
    offset += length;

    return( RS_OK );
}


bool CopyLXExeObjects( void )
/***************************/
{
    ExeFileInfo     *old;
    ExeFileInfo     *tmp;
    object_record   *old_obj;
    object_record   *tmp_obj;
    int             num_objs;
    RcStatus        ret;

    old = &Pass2Info.OldFile;
    tmp = &Pass2Info.TmpFile;

    if( readObjectAndPageTable( old ) ) {
        return( true );
    }
    num_objs = copyObjectAndPageTable( old, tmp );
    if( num_objs == -1 ) {
        return( true );
    }

    /* At this point we finally know how big all the tables in executable
     * header will be and can start copying data to the new executable;
     * resources will be written and the headers finalized later.
     */
    if( copyHeaderSections( old, tmp ) != RS_OK ) {
        return( true );
    }

    old_obj = old->u.LXInfo.Objects;
    tmp_obj = tmp->u.LXInfo.Objects;
    for( ; num_objs > 0; num_objs--, old_obj++, tmp_obj++ ) {
        ret = copyOneObject( old, old_obj, tmp, tmp_obj );
        switch( ret ) {
        case RS_WRITE_ERROR:
            RcError( ERR_WRITTING_FILE, tmp->name, strerror( errno ) );
            return( true );
        case RS_READ_ERROR:
            RcError( ERR_READING_EXE, old->name, strerror( errno ) );
            return( true );
        case RS_READ_INCMPLT:
            RcError( ERR_UNEXPECTED_EOF, old->name );
            return( true );
        default:
            break;
        }
        CheckDebugOffset( old );
        CheckDebugOffset( tmp );
    }

    return( false );
} /* CopyLXExeObjects */
