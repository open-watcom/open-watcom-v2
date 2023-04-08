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


static RcStatus readObjectTable( ExeFileInfo *exe )
/*************************************************/
{
    RcStatus        ret;
    unsigned        objects_size;
    pe_exe_header   *pehdr;

    pehdr = exe->u.PEInfo.WinHead;
    objects_size = pehdr->fheader.num_objects * sizeof( pe_object );
    exe->u.PEInfo.Objects = RESALLOC( objects_size );
    ret = SeekRead( exe->fp, exe->WinHeadOffset + PE_SIZE( *pehdr ), exe->u.PEInfo.Objects, objects_size );
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
    return( ret );
}

static int copyObjectTable( ExeFileInfo *old, ExeFileInfo *new )
/***************************************************************/
/* Copies the object table from old to new adding one more entry to new */
{
    uint_32         old_offset;     /* start of the image pages */
    uint_32         new_offset;
    uint_32         delta_offset;
    uint_32         res_offset;
    pe_va           old_rva;
    pe_va           new_rva;
    int             old_obj_size;
    int             new_obj_size;
    int             obj_num;
    int             old_num_objects;
    int             new_num_objects;
    pe_va           old_res_rva;
    pe_exe_header   *old_pehdr;
    pe_exe_header   *new_pehdr;

    /* check for a resource object in the old exe */
    old_pehdr = old->u.PEInfo.WinHead;
    old_res_rva = PE_DIRECTORY( *old_pehdr, PE_TBL_RESOURCE ).rva;
    new_num_objects = old_num_objects = old_pehdr->fheader.num_objects;
    for( obj_num = 0; obj_num < old_num_objects; obj_num++ ) {
        if( old_res_rva != 0 && old->u.PEInfo.Objects[obj_num].rva == old_res_rva ) {
            /* there already was a resource object */
            if( obj_num + 1 == old_num_objects ) {
                /* it is the last object so just ignore it */
                new_num_objects--;
                res_offset = old->u.PEInfo.Objects[obj_num].physical_offset + old->u.PEInfo.Objects[obj_num].physical_size;
                if( res_offset > old->DebugOffset ) {
                    old->DebugOffset = res_offset;
                }
                continue;
            } else {
                /* can't ignore it otherwise the rva's in the file will
                   be off*/
                RcWarning( ERR_OLD_RESOURCE_OBJECT );
            }
        }
    }
    if( !CmdLineParms.NoResFile ) {
        ++new_num_objects;
    }
    new_pehdr = new->u.PEInfo.WinHead;
    new_pehdr->fheader.num_objects = new_num_objects;
    new_obj_size = new_num_objects * sizeof( pe_object );
    old_obj_size = old_num_objects * sizeof( pe_object );
    if( !CmdLineParms.NoResFile ) {
        --new_num_objects;
    }
    old_offset = old->WinHeadOffset + PE_SIZE( *old_pehdr ) + old_obj_size;
    old_rva = ALIGN_VALUE( old_offset, PE( *old_pehdr, object_align ) );
    old_offset = ALIGN_VALUE( old_offset, PE( *old_pehdr, file_align ) );
    new_offset = new->WinHeadOffset + PE_SIZE( *new_pehdr ) + new_obj_size;
    new_rva = ALIGN_VALUE( new_offset, PE( *new_pehdr, object_align ) );
    new_offset = ALIGN_VALUE( new_offset, PE( *new_pehdr, file_align ) );

    delta_offset = new_offset - old_offset;

    if( new_rva != old_rva ) {
        /* This means that all the rva's in all image pages are now wrong. */
        /* Since this case is rare and the user can relink with a higher */
        /* object_align we just print an error messages and quit */
        RcError( ERR_PE_HEADER_SIZE_CHANGE );
        return( -1 );
    }

    new->u.PEInfo.Objects = RESALLOC( new_obj_size );

    for( obj_num = 0; obj_num < new_num_objects; obj_num++ ) {
        new->u.PEInfo.Objects[obj_num] = old->u.PEInfo.Objects[obj_num];
        new->u.PEInfo.Objects[obj_num].physical_offset += delta_offset;
    }

    if( new_num_objects == -1 ) {
        RcError( ERR_INTERNAL, INTERR_EXE_HAS_MINUS_1_SEGS );
    }
    return( new_num_objects );
}

/*
 * copyOneObject
 * if an error occurs this function MUST return without altering errno
 */
static RcStatus copyOneObject( FILE *old_fp, pe_object * old_obj,
                        FILE *new_fp, pe_object * new_obj )
/***************************************************************/
{
    /*
     * if this an uninitialized object (one for which there is not
     * data in the file) then don't copy it
     */
    if( (old_obj->flags & PE_OBJ_UNINIT_DATA) && ( old_obj->physical_offset == 0 ) ) {
        return( RS_OK );
    }
    if( RESSEEK( old_fp, old_obj->physical_offset, SEEK_SET ) )
        return( RS_READ_ERROR );
    if( RESSEEK( new_fp, new_obj->physical_offset, SEEK_SET ) )
        return( RS_WRITE_ERROR );

    return( CopyExeData( old_fp, new_fp, old_obj->physical_size ) );
}

bool CopyExeObjects( void )
/*************************/
{
    ExeFileInfo *   old;
    ExeFileInfo *   tmp;
    pe_object *     old_obj;
    pe_object *     tmp_obj;
    int             num_objs;
    RcStatus        ret;

    old = &Pass2Info.OldFile;
    tmp = &Pass2Info.TmpFile;

    if( readObjectTable( old ) != RS_OK ) {
        return( true );
    }
    num_objs = copyObjectTable( old, tmp );
    if( num_objs == -1 ) {
        return( true );
    }

    old_obj = old->u.PEInfo.Objects;
    tmp_obj = tmp->u.PEInfo.Objects;
    for( ; num_objs > 0; num_objs--, old_obj++, tmp_obj++ ) {
        ret = copyOneObject( old->fp, old_obj, tmp->fp, tmp_obj );
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
} /* CopyExeObjects */

uint_32 GetNextObjPhysOffset( PEExeInfo *peinfo )
/***********************************************/
/* This routine assumes the num_objects in the header include one for the */
/* resource object */
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
/**************************************/
/* This routine assumes the num_objects in the header include one for the */
/* resource object */
{
    uint_32         next_rva;
    pe_object       *last_obj;
    pe_exe_header   *pehdr;

    pehdr = peinfo->WinHead;
    last_obj = peinfo->Objects + pehdr->fheader.num_objects - 2;
/* This next line should work if the nt loader followed the PE spec but it */
/* doesn't so we can't use it */
//    next_rva = last_obj->rva + last_obj->virtual_size;
    next_rva = last_obj->rva + last_obj->physical_size;
    return( ALIGN_VALUE( next_rva, PE( *pehdr, object_align ) ) );
} /* GetNextObjRVA */
