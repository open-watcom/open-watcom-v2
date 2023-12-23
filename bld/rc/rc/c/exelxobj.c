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


static bool readObjectAndPageTable( ExeFileInfo *src )
/****************************************************/
{
    RcStatus    ret;
    size_t      table_size;

    table_size = src->u.LXInfo.OS2Head.num_objects * sizeof( object_record );
    src->u.LXInfo.Objects = RESALLOC( table_size );
    ret = SeekRead( src->fp, src->WinHeadOffset + src->u.LXInfo.OS2Head.objtab_off,
                src->u.LXInfo.Objects, table_size );

    if( ret == RS_OK ) {
        table_size = src->u.LXInfo.OS2Head.num_pages * sizeof( lx_map_entry );
        src->u.LXInfo.Pages = RESALLOC( table_size );
        ret = SeekRead( src->fp, src->WinHeadOffset + src->u.LXInfo.OS2Head.objmap_off,
                    src->u.LXInfo.Pages, table_size );
    }
    switch( ret ) {
    case RS_OK:
        break;
    case RS_READ_ERROR:
        RcError( ERR_READING_EXE, src->name, strerror( errno ) );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, src->name );
        break;
    default:
        RcError( ERR_INTERNAL, INTERR_UNKNOWN_RCSTATUS );
        break;
    }
    CheckDebugOffset( src );
    return( ret != RS_OK );
}

static int copyObjectAndPageTable( ExeFileInfo *src, ExeFileInfo *dst )
/**********************************************************************
 * Copies the object/page table from src to dst sans resource objects
 */
{
    uint_32         src_obj_size;
    uint_32         dst_obj_size;
    uint_32         src_page_size;
    uint_32         dst_page_size;
    uint_32         align;
    int_32          dst_off;
    object_record   *src_obj;
    object_record   *dst_obj;
    lx_map_entry    *src_page;
    lx_map_entry    *dst_page;
    unsigned        obj_index;
    int             src_obj_index;
    int             src_num_objects;
    int             page_index;
    int             src_num_pages;
    uint_32         i;

    src_obj  = src->u.LXInfo.Objects;
    src_page = src->u.LXInfo.Pages;
    src_num_objects = src_num_pages = 0;
    /*
     * Figure out number of src objects/pages
     */
    for( obj_index = 0; obj_index < src->u.LXInfo.OS2Head.num_objects; obj_index++ ) {
        /*
         * Simply skip any existing resource objects
         */
        if( (src_obj[obj_index].flags & OBJ_RESOURCE) == 0 ) {
            ++src_num_objects;
            src_num_pages += src_obj[obj_index].mapsize;
        }
    }
    if( CmdLineParms.NoResFile ) {
        dst->u.LXInfo.OS2Head.num_objects = src_num_objects;
        dst->u.LXInfo.OS2Head.num_pages   = src_num_pages;
    } else {
        dst->u.LXInfo.OS2Head.num_objects = src_num_objects
                            + dst->u.LXInfo.Res.num_objects;
        dst->u.LXInfo.OS2Head.num_pages   = src_num_pages
                            + dst->u.LXInfo.Res.num_pages;
    }
    dst_obj_size = dst->u.LXInfo.OS2Head.num_objects * sizeof( object_record );
    src_obj_size = src->u.LXInfo.OS2Head.num_objects * sizeof( object_record );
    dst_page_size = dst->u.LXInfo.OS2Head.num_pages * sizeof( lx_map_entry );
    src_page_size = src->u.LXInfo.OS2Head.num_pages * sizeof( lx_map_entry );
    /*
     * Calculate dst offset of data pages
     */
    dst_off = dst_obj_size + dst_page_size - src_obj_size - src_page_size
            + dst->u.LXInfo.OS2Head.num_rsrcs * sizeof( flat_res_table )
            - src->u.LXInfo.OS2Head.num_rsrcs * sizeof( flat_res_table )
            + dst->u.LXInfo.OS2Head.num_pages * sizeof( uint_32 )
            - src->u.LXInfo.OS2Head.num_pages * sizeof( uint_32 );

    dst_off += src->u.LXInfo.OS2Head.page_off;
    align = 1 << src->u.LXInfo.OS2Head.l.page_shift;
    dst_off = (dst_off + align - 1) & ~(align - 1);
    dst->u.LXInfo.OS2Head.page_off = dst_off;
    /*
     * Allocate dst object/page table
     */
    dst_obj  = RESALLOC( dst_obj_size );
    dst_page = RESALLOC( dst_page_size );
    dst->u.LXInfo.Objects = dst_obj;
    dst->u.LXInfo.Pages   = dst_page;

    src_obj_index = page_index = 0;
    /*
     * Copy object and page records from src executable to dst
     */
    for( obj_index = 0; obj_index < src->u.LXInfo.OS2Head.num_objects; obj_index++ ) {
        if( (src_obj[obj_index].flags & OBJ_RESOURCE) == 0 ) {
            dst_obj[obj_index] = src_obj[src_obj_index];
            dst_obj[obj_index].mapidx = page_index + 1;
            for( i = 0; i < src_obj[src_obj_index].mapsize; ++i ) {
                dst_page[page_index] = src_page[src_obj[src_obj_index].mapidx + i - 1];
                ++page_index;
            }
            ++src_obj_index;
        }
    }
    /*
     * Mark the start of resource objects/pages
     */
    dst->u.LXInfo.FirstResObj  = src_obj_index;
    dst->u.LXInfo.FirstResPage = page_index;

    return( src_num_objects );
}


static RcStatus copyOneObject( ExeFileInfo *src, object_record *src_obj,
                               ExeFileInfo *dst, object_record *dst_obj )
/************************************************************************
 * if an error occurs this function MUST return without altering errno
 */
{
    RcStatus        ret;
    lx_map_entry    *src_map;
    lx_map_entry    *dst_map;
    uint_32         src_offset;
    uint_32         dst_offset;
    unsigned        i;

    for( i = 0; i < src_obj->mapsize; ++i ) {
        src_map = &src->u.LXInfo.Pages[src_obj->mapidx + i - 1];
        dst_map = &dst->u.LXInfo.Pages[dst_obj->mapidx + i - 1];

        // NB - page_offset is relative to start of executable, not to LX header!
        src_offset = (src_map->page_offset << src->u.LXInfo.OS2Head.l.page_shift)
                    + src->u.LXInfo.OS2Head.page_off;
        dst_offset = (dst_map->page_offset << dst->u.LXInfo.OS2Head.l.page_shift)
                    + dst->u.LXInfo.OS2Head.page_off;
        if( RESSEEK( src->fp, src_offset, SEEK_SET ) )
            return( RS_READ_ERROR );
        if( RESSEEK( dst->fp, dst_offset, SEEK_SET ) )
            return( RS_WRITE_ERROR );

        ret = CopyExeData( src->fp, dst->fp, src_map->data_size );
        if( ret != RS_OK ) {
            return( ret );
        }
    }
    return( RS_OK );
}


static RcStatus copyHeaderSections( ExeFileInfo *src, ExeFileInfo *dst )
/***********************************************************************
 * Copies parts of header and loader/fixup sections that won't be changing
 * if an error occurs this function MUST return without altering errno
 */
{
    RcStatus            ret;
    uint_32             src_pages;
    uint_32             offset;
    uint_32             length;
    uint_32             lx_off;
    os2_flat_header     *src_head;
    os2_flat_header     *dst_head;
    size_t              numread;

    lx_off = dst->WinHeadOffset;
    src_head = &src->u.LXInfo.OS2Head;
    dst_head = &dst->u.LXInfo.OS2Head;
    /*
     * Leave room for object table, object page table and resource
     * table to be written later
     */
    offset = sizeof( os2_flat_header )
           + dst_head->num_objects * sizeof( object_record )
           + dst_head->num_pages * sizeof( lx_map_entry )
           + dst_head->num_rsrcs * sizeof( flat_res_table );
    if( RESSEEK( dst->fp, lx_off + offset, SEEK_SET ) )
        return( RS_WRITE_ERROR );

    dst_head->signature    = src_head->signature;
    dst_head->byte_order   = src_head->byte_order;
    dst_head->word_order   = src_head->word_order;
    dst_head->level        = src_head->level;
    dst_head->cpu_type     = src_head->cpu_type;
    dst_head->os_type      = src_head->os_type;
    dst_head->version      = src_head->version;
    dst_head->flags        = src_head->flags;
    dst_head->start_obj    = src_head->start_obj;
    dst_head->eip          = src_head->eip;
    dst_head->stack_obj    = src_head->stack_obj;
    dst_head->esp          = src_head->esp;
    dst_head->page_size    = src_head->page_size;
    dst_head->l.page_shift = src_head->l.page_shift;
    dst_head->autodata_obj = src_head->autodata_obj;
    dst_head->heapsize     = src_head->heapsize;
    dst_head->stacksize    = src_head->stacksize;

    dst_head->objtab_off   = sizeof( os2_flat_header );
    dst_head->objmap_off   = dst_head->objtab_off
                           + dst_head->num_objects * sizeof( object_record );
    dst_head->rsrc_off     = dst_head->objmap_off
                           + dst_head->num_pages * sizeof( lx_map_entry );

    // copy resident names table if provided
    if( src_head->entry_off > src_head->resname_off ) {
        dst_head->resname_off  = dst_head->rsrc_off
                               + dst_head->num_rsrcs * sizeof( flat_res_table );
        if( RESSEEK( src->fp, lx_off + src_head->resname_off, SEEK_SET ) )
            return( RS_READ_ERROR );

        length = src_head->entry_off - src_head->resname_off;
        ret = CopyExeData( src->fp, dst->fp, length );
        if( ret != RS_OK ) {
            return( ret );
        }
    } else {
        dst_head->resname_off = 0;
        length = 0;
    }
    offset += length;

    // copy entry table if provided
    if( src_head->entry_off ) {
        dst_head->entry_off = offset;
        if( src_head->moddir_off ) {
            length = src_head->moddir_off - src_head->entry_off;
        } else {
            length = src_head->fixpage_off - src_head->entry_off;
        }
        if( RESSEEK( src->fp, lx_off + src_head->entry_off, SEEK_SET ) )
            return( RS_READ_ERROR );

        ret = CopyExeData( src->fp, dst->fp, length );
        if( ret != RS_OK ) {
            return( ret );
        }
    } else {
        dst_head->entry_off = 0;
        length = 0;
    }
    offset += length;

    // now we can determine loader section size
    dst_head->loader_size  = offset - sizeof( os2_flat_header );
    dst_head->loader_cksum = 0;

    // copy fixup section if provided
    if( src_head->fixup_size ) {
        int_32      delta;
        uint_32     last_fix_pg;
        uint_32     dst_fixpg_length;
        uint_32     src_fixpg_length;
        unsigned    i;

        // the fixup page table must be extended if resource pages were added
        src_pages = dst_head->num_pages - dst->u.LXInfo.Res.num_pages;
        length = sizeof( uint_32 ) * src_pages;
        if( RESSEEK( src->fp, lx_off + src_head->fixpage_off, SEEK_SET ) )
            return( RS_READ_ERROR );

        ret = CopyExeData( src->fp, dst->fp, length );
        if( ret != RS_OK )
            return( ret );

        // read the last entry of fixup page table
        numread = RESREAD( src->fp, &last_fix_pg, sizeof( last_fix_pg ) );
        if( numread != sizeof( last_fix_pg ) )
            return( RESIOERR( src->fp, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );

        // replicate for each added page (plus one for final entry)
        for( i = 0; i < dst->u.LXInfo.Res.num_pages + 1; i++ ) {
            if( RESWRITE( dst->fp, &last_fix_pg, sizeof( last_fix_pg ) ) != sizeof( last_fix_pg ) ) {
                return( RS_WRITE_ERROR );
            }
        }

        // calculate dst fixup table size
        src_fixpg_length = (src_head->num_pages + 1) * sizeof( uint_32 );
        dst_fixpg_length = (dst_head->num_pages + 1) * sizeof( uint_32 );
        dst_head->fixup_size   = src_head->fixup_size
                               - src_fixpg_length + dst_fixpg_length;
        dst_head->fixup_cksum  = 0;

        delta = offset - src_head->fixpage_off;
        dst_head->fixpage_off  = src_head->fixpage_off + delta;

        delta = delta - src_fixpg_length + dst_fixpg_length;
        dst_head->fixrec_off   = src_head->fixrec_off + delta;
        dst_head->impmod_off   = src_head->impmod_off + delta;
        dst_head->num_impmods  = src_head->num_impmods;
        dst_head->impproc_off  = src_head->impproc_off + delta;

        // copy the rest of fixup section
        offset += dst_fixpg_length;
        length = src_head->fixup_size - src_fixpg_length;
        if( RESSEEK( src->fp, lx_off + src_head->fixpage_off + src_fixpg_length, SEEK_SET ) )
            return( RS_READ_ERROR );

        ret = CopyExeData( src->fp, dst->fp, length );
        if( ret != RS_OK ) {
            return( ret );
        }
    } else {
        dst_head->fixup_size   = 0;
        dst_head->fixup_cksum  = 0;
        dst_head->fixpage_off  = 0;
        dst_head->fixrec_off   = 0;
        dst_head->impmod_off   = 0;
        dst_head->num_impmods  = 0;
        dst_head->impproc_off  = 0;
        length = 0;
    }
    offset += length;

    return( RS_OK );
}


bool CopyLXExeObjects( ExeFileInfo *src, ExeFileInfo *dst )
/*********************************************************/
{
    object_record   *src_obj;
    object_record   *dst_obj;
    int             num_objs;
    RcStatus        ret;

    if( readObjectAndPageTable( src ) ) {
        return( true );
    }
    num_objs = copyObjectAndPageTable( src, dst );
    if( num_objs == -1 ) {
        return( true );
    }
    /*
     * At this point we finally know how big all the tables in executable
     * header will be and can start copying data to the dst executable;
     * resources will be written and the headers finalized later.
     */
    if( copyHeaderSections( src, dst ) != RS_OK ) {
        return( true );
    }

    src_obj = src->u.LXInfo.Objects;
    dst_obj = dst->u.LXInfo.Objects;
    for( ; num_objs > 0; num_objs--, src_obj++, dst_obj++ ) {
        ret = copyOneObject( src, src_obj, dst, dst_obj );
        switch( ret ) {
        case RS_WRITE_ERROR:
            RcError( ERR_WRITTING_FILE, dst->name, strerror( errno ) );
            return( true );
        case RS_READ_ERROR:
            RcError( ERR_READING_EXE, src->name, strerror( errno ) );
            return( true );
        case RS_READ_INCMPLT:
            RcError( ERR_UNEXPECTED_EOF, src->name );
            return( true );
        default:
            break;
        }
    }
    CheckDebugOffset( src );
    CheckDebugOffset( dst );

    return( false );
} /* CopyLXExeObjects */
