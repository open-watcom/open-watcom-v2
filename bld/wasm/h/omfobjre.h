/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  OMF record manipulation function definitions.
*
****************************************************************************/


#ifndef OMFOBJRE_H
#define OMFOBJRE_H

#include "omfrec.h"


extern void             ObjRecInit( void );
extern void             ObjRecFini( void );
/*
    ObjRecInit must be called before any of the other routines in this
    module.  ObjRecFini free's all existing object records, and any
    memory used by the module.
*/


extern obj_rec_handle   ObjNewRec( uint_8 command );
/*
    Create an object record of type 'command'.  Does not allocate or attach
    any data to the record, or fill in any of the specific fields for each
    object record.
*/


extern void             ObjKillRec( obj_rec_handle objr );
/*
    Free's the memory used by an object record.  If the record had data
    allocated for it (ObjAllocData) or ObjCanFree was called on the record,
    then the data is free'd as well.  Records with extra memory (such as
    the fixup chain on FIXUPs, or the line number array on LINNUM) have the
    extra memory free'd as well.
*/


extern void         ObjAllocData( obj_rec_handle objr, uint_16 len );
/*
    Allocate a piece of memory of length len, attach it to the object
    record, and set the can_free bit.  This is the most common way to
    fill in data for object records.  Data allocated this way will be
    freed when ObjKillRec/ObjDetachData is called on the record.
*/


extern void         ObjAttachData( obj_rec_handle objr, uint_8 *data, uint_16 len );
/*
    This is useful for attaching constants to an object record.  For example,
    when creating the 80386 comment record for pharlap OMF, you could do the
    following:

        objr = ObjNewRec( CMD_COMENT );
        objr->d.coment.attr = 0x80;
        coment->d.coment.class = CMT_EASY_OMF;
        ObjAttachData( coment, "80386", 5 );

    Memory attached this way is not free'd by ObjKillRec or ObjDetachData.
*/


extern void         ObjDetachData( obj_rec_handle objr );
/*
    Free's the data associated with an object record, but does not free
    the actual object record itself.  Called as part of ObjKillRec().
*/


extern void         ObjCanFree( obj_rec_handle objr );
/*
    Indicates that ObjDetachData or ObjKillRec can free the data associated
    with this record.  Not necessary if the data was allocated with
    ObjAllocData.
*/


extern uint_8       ObjGet8( obj_rec_handle objr );
extern uint_16      ObjGet16( obj_rec_handle objr );
extern uint_32      ObjGet32( obj_rec_handle objr );
extern uint_32      ObjGetEither( obj_rec_handle objr );
extern uint_16      ObjGetIndex( obj_rec_handle objr );
extern uint_8       *ObjGet( obj_rec_handle objr, uint_16 len );
extern int          ObjEOR( obj_rec_handle objr );
extern uint_16      ObjRTell( obj_rec_handle objr );
extern void         ObjRSeek( obj_rec_handle objr, uint_16 set );
extern uint_16      ObjRemain( obj_rec_handle objr );
extern void         ObjPut8( obj_rec_handle objr, uint_8 byte );
extern void         ObjPut16( obj_rec_handle objr, uint_16 word );
extern void         ObjPut32( obj_rec_handle objr, uint_32 dword );
extern void         ObjPutIndex( obj_rec_handle objr, uint_16 idx );
extern void         ObjPutEither( obj_rec_handle objr, uint_32 val );
extern void         ObjPut( obj_rec_handle objr, const uint_8 *data, uint_16 len );
extern void         ObjPutName( obj_rec_handle objr, const char *name, uint_8 len );
/*
    Notes:

    The data attached/allocated for an object record is treated like a
    small file.  Initially the "file" pointer points 0 characters into
    the data.  The following functions are used to read and write data
    and modify the "file" pointer.

    ObjGet8         return uint_8 at pointer, and bump pointer by 1
    ObjGet16        return uint_16 at pointer, and bump pointer by 2
    ObjGet32        return uint_32 at pointer, and bump pointer by 4
    ObjGetEither    if record is_32 then ObjGet32 else ObjGet16
    ObjGetIndex     return the intel index at pointer, bump ptr by 1 or 2
    ObjGet          return ptr to len bytes, bump ptr by len.
                    The ptr to the entire data record is returned by
                    the call ObjGet( rec, 0 );
    ObjEOR          returns TRUE (non-zero) if pointer is at end of record
    ObjRTell        returns the offset of the pointer into the data
    ObjRSeek        sets the offset of the pointer into the data
    ObjRemain       how many bytes left in record
    ObjPut8         write uint_8 at pointer, and bump pointer by 1
    ObjPut16        write uint_16 at pointer, and bump pointer by 2
    ObjPut32        write uint_32 at pointer, and bump pointer by 4
    ObjPutEither    if record is_32 then ObjPut32 else ObjPut16
    ObjPutIndex     return the intel index at pointer, bump ptr by 1 or 2
    ObjPut          put len bytes of data at pointer, and bump ptr by len
    ObjPutName      ObjPut8( len ) then ObjPut( name, len )
*/


extern void         ObjTruncRec( obj_rec_handle objr );
/*
    The generator filters (genmsomf.c, genphar.c) write data starting from
    the "file" pointer up to the point where ObjTruncRec was called.  For
    example,

        objr = ObjNewRec( CMD_COMENT );
        objr->d.comment.attr = 0x80;
        objr->d.comment.class = 0xff;
        ObjAllocData( objr, 100 );
        ObjRSeek( objr, 5 );
        ObjPut( objr, "willy_wonka", 11 );
        ObjTruncRec( objr );

    The object record constructed by this would only have the bytes starting
    at offset 5, and ending at offset 15 written out to the object file.
    This is useful when creating records that contain Intel indicies...
    since the indicies are variable length you have to either precalculate
    the length of the record, or make an 'upper-bound' guess.  I chose to
    make the upper-bound guess, then use ObjTruncRec to truncate the
    record after writing all the data to it.
*/


/*
    The following macros are just for speed.
*/

#define ObjEOR(objr)            ( (objr)->curoff >= (objr)->length )
#define ObjRTell(objr)          ( (objr)->curoff )
#define ObjRSeek(objr,set)      (void)( (objr)->curoff = set )
#define ObjRemain(objr)         ( (objr)->length - (objr)->curoff )
#define ObjTruncRec(objr)       (void)( (objr)->length = (objr)->curoff )
#define ObjCanFree(objr)        (void)( (objr)->free_data = 1 )
#endif
