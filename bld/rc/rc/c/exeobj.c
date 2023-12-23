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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "global.h"
#include <errno.h>
#include "rcerrors.h"
#include "rcrtns.h"
#include "rccore_2.h"
#include "exeutil.h"
#include "exeobj.h"


static RcStatus readObjectTable( ExeFileInfo *src )
/*************************************************/
{
    RcStatus        ret;
    unsigned        objects_size;
    pe_exe_header   *pehdr;

    pehdr = src->u.PEInfo.WinHead;
    objects_size = pehdr->fheader.num_objects * sizeof( pe_object );
    src->u.PEInfo.Objects = RESALLOC( objects_size );
    ret = SeekRead( src->fp, src->WinHeadOffset + PE_SIZE( *pehdr ), src->u.PEInfo.Objects, objects_size );
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
    return( ret );
}

static int copyObjectTable( ExeFileInfo *src, ExeFileInfo *dst )
/***************************************************************
 * Copies the object table from src to dst adding one more entry to dst
 */
{
    uint_32         src_offset;     /* start of the image pages */
    uint_32         dst_offset;
    uint_32         delta_offset;
    uint_32         res_offset;
    pe_va           src_rva;
    pe_va           dst_rva;
    int             src_obj_size;
    int             dst_obj_size;
    int             obj_num;
    int             src_num_objects;
    int             dst_num_objects;
    pe_va           src_resource_rva;
    pe_exe_header   *src_pehdr;
    pe_exe_header   *dst_pehdr;

    /*
     * check for a resource object in the src exe
     */
    src_pehdr = src->u.PEInfo.WinHead;
    src_resource_rva = PE_DIRECTORY( *src_pehdr, PE_TBL_RESOURCE ).rva;
    dst_num_objects = src_num_objects = src_pehdr->fheader.num_objects;
    for( obj_num = 0; obj_num < src_num_objects; obj_num++ ) {
        if( src_resource_rva != 0 && src->u.PEInfo.Objects[obj_num].rva == src_resource_rva ) {
            /*
             * there already was a resource object
             */
            if( obj_num + 1 == src_num_objects ) {
                /*
                 * it is the last object so just ignore it
                 */
                dst_num_objects--;
                res_offset = src->u.PEInfo.Objects[obj_num].physical_offset + src->u.PEInfo.Objects[obj_num].physical_size;
                if( src->DebugOffset < res_offset ) {
                    src->DebugOffset = res_offset;
                }
                continue;
            } else {
                /*
                 * can't ignore it otherwise the rva's in the file will be off
                 */
                RcWarning( ERR_OLD_RESOURCE_OBJECT );
            }
        }
    }
    if( !CmdLineParms.NoResFile ) {
        ++dst_num_objects;
    }
    dst_pehdr = dst->u.PEInfo.WinHead;
    dst_pehdr->fheader.num_objects = dst_num_objects;
    dst_obj_size = dst_num_objects * sizeof( pe_object );
    src_obj_size = src_num_objects * sizeof( pe_object );
    if( !CmdLineParms.NoResFile ) {
        --dst_num_objects;
    }
    src_offset = src->WinHeadOffset + PE_SIZE( *src_pehdr ) + src_obj_size;
    src_rva = ALIGN_VALUE( src_offset, PE( *src_pehdr, object_align ) );
    src_offset = ALIGN_VALUE( src_offset, PE( *src_pehdr, file_align ) );
    dst_offset = dst->WinHeadOffset + PE_SIZE( *dst_pehdr ) + dst_obj_size;
    dst_rva = ALIGN_VALUE( dst_offset, PE( *dst_pehdr, object_align ) );
    dst_offset = ALIGN_VALUE( dst_offset, PE( *dst_pehdr, file_align ) );

    delta_offset = dst_offset - src_offset;

    if( dst_rva != src_rva ) {
        /*
         * This means that all the rva's in all image pages are now wrong.
         * Since this case is rare and the user can relink with a higher
         * object_align we just print an error messages and quit
         */
        RcError( ERR_PE_HEADER_SIZE_CHANGE );
        return( -1 );
    }

    dst->u.PEInfo.Objects = RESALLOC( dst_obj_size );

    for( obj_num = 0; obj_num < dst_num_objects; obj_num++ ) {
        dst->u.PEInfo.Objects[obj_num] = src->u.PEInfo.Objects[obj_num];
        dst->u.PEInfo.Objects[obj_num].physical_offset += delta_offset;
    }

    if( dst_num_objects == -1 ) {
        RcError( ERR_INTERNAL, INTERR_EXE_HAS_MINUS_1_SEGS );
    }
    return( dst_num_objects );
}

static RcStatus copyOneObject( FILE *src_fp, pe_object *src_obj,
                        FILE *dst_fp, pe_object *dst_obj )
/***************************************************************
 *
 * copyOneObject
 * if an error occurs this function MUST return without altering errno
 *
 * if this an uninitialized object (one for which there is not data in the file)
 * then don't copy it
 */
{
    if( (src_obj->flags & PE_OBJ_UNINIT_DATA) && ( src_obj->physical_offset == 0 ) ) {
        return( RS_OK );
    }
    if( RESSEEK( src_fp, src_obj->physical_offset, SEEK_SET ) )
        return( RS_READ_ERROR );
    if( RESSEEK( dst_fp, dst_obj->physical_offset, SEEK_SET ) )
        return( RS_WRITE_ERROR );

    return( CopyExeData( src_fp, dst_fp, src_obj->physical_size ) );
}

bool CopyExeObjects( ExeFileInfo *src, ExeFileInfo *dst )
/*******************************************************/
{
    pe_object       *src_obj;
    pe_object       *dst_obj;
    int             num_objs;
    RcStatus        ret;

    if( readObjectTable( src ) != RS_OK ) {
        return( true );
    }
    num_objs = copyObjectTable( src, dst );
    if( num_objs == -1 ) {
        return( true );
    }

    src_obj = src->u.PEInfo.Objects;
    dst_obj = dst->u.PEInfo.Objects;
    for( ; num_objs > 0; num_objs--, src_obj++, dst_obj++ ) {
        ret = copyOneObject( src->fp, src_obj, dst->fp, dst_obj );
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
} /* CopyExeObjects */

uint_32 GetNextObjPhysOffset( PEExeInfo *peinfo )
/************************************************
 * This routine assumes the num_objects in the header include one for the
 * resource object
 */
{
    uint_32         next_off;
    pe_object       *last_obj;
    pe_exe_header   *pehdr;

    pehdr = peinfo->WinHead;
    last_obj = peinfo->Objects + pehdr->fheader.num_objects - 2;
    next_off = last_obj->physical_offset + last_obj->physical_size;
    return( ALIGN_VALUE( next_off, PE( *pehdr, file_align ) ) );
} /* GetNextObjPhysOffset */

pe_va GetNextObjRVA( PEExeInfo *peinfo )
/***************************************
 * This routine assumes the num_objects in the header include one for the
 * resource object
 */
{
    uint_32         next_rva;
    pe_object       *last_obj;
    pe_exe_header   *pehdr;

    pehdr = peinfo->WinHead;
    last_obj = peinfo->Objects + pehdr->fheader.num_objects - 2;
/*
 * This next line should work if the nt loader followed the PE spec but it
 * doesn't so we can't use it
 */
//    next_rva = last_obj->rva + last_obj->virtual_size;
    next_rva = last_obj->rva + last_obj->physical_size;
    return( ALIGN_VALUE( next_rva, PE( *pehdr, object_align ) ) );
} /* GetNextObjRVA */
