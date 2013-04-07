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


#include "wio.h"
#include "global.h"
#include "rcmem.h"
#include "errors.h"
#include "exeutil.h"
#include "exeobj.h"
#include "iortns.h"

static int readObjectTable( ExeFileInfo *exe )
/********************************************/
{
    RcStatus        error;
    unsigned        objects_size;
    long            file_offset;
    exe_pe_header   *pehdr;

    pehdr = exe->u.PEInfo.WinHead;
    if( IS_PE64( *pehdr ) ) {
        objects_size = PE64( *pehdr ).num_objects * sizeof( pe_object );
        file_offset = exe->WinHeadOffset + sizeof( pe_header64 );
    } else {
        objects_size = PE32( *pehdr ).num_objects * sizeof( pe_object );
        file_offset = exe->WinHeadOffset + sizeof( pe_header );
    }
    exe->u.PEInfo.Objects = RcMemMalloc( objects_size );
    error = SeekRead( exe->Handle, file_offset, exe->u.PEInfo.Objects, objects_size );
    switch( error ) {
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
    return( error != RS_OK );
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
    exe_pe_header   *old_pehdr;
    exe_pe_header   *new_pehdr;

    /* check for a resource object in the old exe */
    old_pehdr = old->u.PEInfo.WinHead;
    if( IS_PE64( *old_pehdr ) ) {
        old_res_rva = PE64( *old_pehdr ).table[PE_TBL_RESOURCE].rva;
        old_num_objects = PE64( *old_pehdr ).num_objects;
    } else {
        old_res_rva = PE32( *old_pehdr ).table[PE_TBL_RESOURCE].rva;
        old_num_objects = PE32( *old_pehdr ).num_objects;
    }
    new_num_objects = old_num_objects;
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
    if( IS_PE64( *new_pehdr ) ) {
        PE64( *new_pehdr ).num_objects = new_num_objects;
    } else {
        PE32( *new_pehdr ).num_objects = new_num_objects;
    }
    new_obj_size = new_num_objects * sizeof( pe_object );
    old_obj_size = old_num_objects * sizeof( pe_object );
    if( !CmdLineParms.NoResFile ) {
        --new_num_objects;
    }
    if( IS_PE64( *old_pehdr ) ) {
        old_offset = old->WinHeadOffset + sizeof( pe_header64 ) + old_obj_size;
        old_rva = ALIGN_VALUE( old_offset, PE64( *old_pehdr ).object_align );
        old_offset = ALIGN_VALUE( old_offset, PE64( *old_pehdr ).file_align );
    } else {
        old_offset = old->WinHeadOffset + sizeof( pe_header ) + old_obj_size;
        old_rva = ALIGN_VALUE( old_offset, PE32( *old_pehdr ).object_align );
        old_offset = ALIGN_VALUE( old_offset, PE32( *old_pehdr ).file_align );
    }
    if( IS_PE64( *new_pehdr ) ) {
        new_offset = new->WinHeadOffset + sizeof( pe_header64 ) + new_obj_size;
        new_rva = ALIGN_VALUE( new_offset, PE64( *new_pehdr ).object_align );
        new_offset = ALIGN_VALUE( new_offset, PE64( *new_pehdr ).file_align );
    } else {
        new_offset = new->WinHeadOffset + sizeof( pe_header ) + new_obj_size;
        new_rva = ALIGN_VALUE( new_offset, PE32( *new_pehdr ).object_align );
        new_offset = ALIGN_VALUE( new_offset, PE32( *new_pehdr ).file_align );
    }

    delta_offset = new_offset - old_offset;

    if( new_rva != old_rva ) {
        /* This means that all the rva's in all image pages are now wrong. */
        /* Since this case is rare and the user can relink with a higher */
        /* object_align we just print an error messages and quit */
        RcError( ERR_PE_HEADER_SIZE_CHANGE );
        return( -1 );
    }

    new->u.PEInfo.Objects = RcMemMalloc( new_obj_size );

    for( obj_num = 0; obj_num < new_num_objects; obj_num++ ) {
        new->u.PEInfo.Objects[ obj_num ] = old->u.PEInfo.Objects[ obj_num ];
        new->u.PEInfo.Objects[ obj_num ].physical_offset += delta_offset;
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
static RcStatus copyOneObject( int old_handle, pe_object * old_obj,
                        int new_handle, pe_object * new_obj )
/************************************************************/
{
    /*
     * if this an uninitialized object (one for which there is not
     * data in the file) then don't copy it
     */
    if( ( old_obj->flags & PE_OBJ_UNINIT_DATA ) &&
        ( old_obj->physical_offset == 0 ) ) {
        return( RS_OK );
    }
    if( RcSeek( old_handle, old_obj->physical_offset, SEEK_SET ) == -1 )
        return( RS_READ_ERROR );
    if( RcSeek( new_handle, new_obj->physical_offset, SEEK_SET ) == -1 )
        return( RS_WRITE_ERROR );

    return( CopyExeData( old_handle, new_handle, old_obj->physical_size ) );
}

extern int CopyExeObjects( void )
/*******************************/
{
    ExeFileInfo *   old;
    ExeFileInfo *   tmp;
    pe_object *     old_obj;
    pe_object *     tmp_obj;
    int             num_objs;
    RcStatus        status;

    old = &Pass2Info.OldFile;
    tmp = &Pass2Info.TmpFile;

    if( readObjectTable( old ) ) {
        return( TRUE );
    }
    num_objs = copyObjectTable( old, tmp );
    if( num_objs == -1 ) {
        return( TRUE );
    }

    old_obj = old->u.PEInfo.Objects;
    tmp_obj = tmp->u.PEInfo.Objects;
    for( ; num_objs > 0; num_objs--, old_obj++, tmp_obj++ ) {
        status = copyOneObject( old->Handle, old_obj, tmp->Handle, tmp_obj );
        switch( status ) {
        case RS_WRITE_ERROR:
            RcError( ERR_WRITTING_FILE, tmp->name, strerror( errno ) );
            return( TRUE );
        case RS_READ_ERROR:
            RcError( ERR_READING_EXE, old->name, strerror( errno ) );
            return( TRUE );
        case RS_READ_INCMPLT:
            RcError( ERR_UNEXPECTED_EOF, old->name );
            return( TRUE );
        default:
            break;
        }
        CheckDebugOffset( old );
        CheckDebugOffset( tmp );
    }

    return( FALSE );
} /* CopyExeObjects */

extern uint_32 GetNextObjPhysOffset( PEExeInfo *peinfo )
/******************************************************/
/* This routine assumes the num_objects in the header include one for the */
/* resource object */
{
    uint_32         next_off;
    pe_object       *last_obj;
    unsigned_32     file_align;
    exe_pe_header   *pehdr;

    pehdr = peinfo->WinHead;
    if( IS_PE64( *pehdr ) ) {
        last_obj = peinfo->Objects + PE64( *pehdr ).num_objects - 2;
        file_align = PE64( *pehdr ).file_align;
    } else {
        last_obj = peinfo->Objects + PE32( *pehdr ).num_objects - 2;
        file_align = PE32( *pehdr ).file_align;
    }
    next_off = last_obj->physical_offset + last_obj->physical_size;
    return( ALIGN_VALUE( next_off, file_align ) );
} /* GetNextObjPhysOffset */

extern pe_va GetNextObjRVA( PEExeInfo *peinfo )
/*********************************************/
/* This routine assumes the num_objects in the header include one for the */
/* resource object */
{
    uint_32         next_rva;
    pe_object       *last_obj;
    unsigned_32     object_align;
    exe_pe_header   *pehdr;

    pehdr = peinfo->WinHead;
    if( IS_PE64( *pehdr ) ) {
        last_obj = peinfo->Objects + PE64( *pehdr ).num_objects - 2;
        object_align = PE64( *pehdr ).object_align;
    } else {
        last_obj = peinfo->Objects + PE32( *pehdr ).num_objects - 2;
        object_align = PE32( *pehdr ).object_align;
    }
/* This next line should work if the nt loader followed the PE spec but it */
/* doesn't so we can't use it */
//    next_rva = last_obj->rva + last_obj->virtual_size;
    next_rva = last_obj->rva + last_obj->physical_size;

    return( ALIGN_VALUE( next_rva, object_align ) );
} /* GetNextObjRVA */
