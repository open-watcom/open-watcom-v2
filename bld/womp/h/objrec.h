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
* Description:  OMF record definitions.
*
****************************************************************************/


#ifndef OBJREC_H
#define OBJREC_H    1
typedef struct obj_rec      obj_rec;
typedef struct linnum_data  linnum_data;
typedef struct pubdef_data  pubdef_data;

#include "watcom.h"
#include "pcobj.h"
#if _WOMP_OPT & _WOMP_WOMP
#include "cansymb.h"
#endif
#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0
#include "fixup.h"
#include "namemgr.h"
#endif


#include "pushpck1.h"

struct coment_info {
    uint_8  attr;           /* attribute field from coment record       */
    uint_8  class;          /* class field from coment record           */
};
/*
    A COMENT record is created by filling in the above fields, and attaching
    any appropriate data with the Obj...() functions below.
*/


struct modend_info {
    uint_8  main_module :1; /* module is a main module                  */
#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0
    uint_8  start_addrs :1; /* module has start address                 */
    uint_8  is_logical  :1; /* is logical or physical reference         */
    logphys ref;            /* a logical or physical reference          */
#endif
};
/*
    A MODEND is described completely by the above information; no data
    should be attached to a MODEND.
*/


struct lnames_info {
    uint_16 first_idx;      /* index of first name in this record       */
    uint_16 num_names;      /* number of names in this record           */
};
/*
    LNAMES, EXTDEFs, and COMDEFs all use this structure.  The actual
    LNAMES/etc are in the data attached to the record.
*/


struct grpdef_info {
    uint_16 idx;            /* index of this grpdef record              */
};
/*
    The data that defines the GRPDEF should be attached to this record.
*/


enum segdef_align_values {
    SEGDEF_ALIGN_ABS        = 0,/* absolute segment - no alignment          */
    SEGDEF_ALIGN_BYTE       = 1,/* relocatable seg  - byte aligned          */
    SEGDEF_ALIGN_WORD       = 2,/*                  - word aligned          */
    SEGDEF_ALIGN_PARA       = 3,/*                  - para aligned          */
    SEGDEF_ALIGN_PAGE       = 4,/*                  - page aligned          */
    SEGDEF_ALIGN_DWORD      = 5,/*                  - dword aligned         */
    SEGDEF_ALIGN_4KPAGE     = 6 /*                  - 4k page aligned       */
    /* if more than 16 types then adjust bitfield width in segdef_info */
};

struct segdef_info {
    uint_16 idx;            /* index for this segment                   */
    uint_8      align       :4; /* align field (enum segdef_align_values)   */
    uint_8      combine     :4; /* combine field (values in pcobj.h)        */
#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0
    uint_8      use_32      :1; /* use_32 for this segment                  */
    uint_8      access_valid:1; /* does next field have valid value         */
    uint_8      access_attr :2; /* easy omf access attributes (see pcobj.h) */
    physref abs;            /* (conditional) absolute physical reference*/
#endif
    uint_32 seg_length;     /* length of this segment                   */
    uint_16 seg_name_idx;   /* name index of this segment               */
    uint_16 class_name_idx; /* class name index of this segment         */
#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0
    uint_16 ovl_name_idx;   /* overlay name index of this segment       */
#endif
};
/*
    All data necessary for a SEGDEF is defined in the above structure.  No
    data should be attached to the record.
*/


struct ledata_info {
    uint_16 idx;            /* index of segment the data belongs to     */
    uint_32 offset;         /* offset into segment of start of data     */
#if _WOMP_OPT & _WOMP_WATFOR
    void        *fixup;         /* ptr to linked list of fixups             */
    unsigned_8 num;             /* number of fixups                         */
#endif
};
/*
    LEDATAs and LIDATAs both use this structure.  The data that comprises the
    record should be attached.
*/


struct base_info {
    uint_16 grp_idx;        /* index of the group base                  */
    uint_16 seg_idx;        /* index of the segment                     */
    uint_16 frame;          /* valid if grp_idx == 0 && seg_idx == 0    */
};                          /* appears at beginning of appropriate recs */
/*
    This appears at the beginning of LINNUMs and PUBDEFs.  (see the
    appropriate structures.
*/


#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0

struct comdat_info {
    struct base_info base;
    uint_8      flags;
    uint_8      attributes;
    uint_8      align;
    uint_32     offset;
    uint_16     type_idx;
    uint_16     public_name_idx;
};
/*
    The data the comprises the record should be attached.
*/


struct fixup_info {
    obj_rec *data_rec;      /* ptr to the data record this belongs to   */
    fixup   *fixup;         /* linked list of processed fixups          */
};
/*
    No data should be attached to these records; all information is in
    the linked list of fixup records.
*/


struct linnum_info {
    union {
        struct base_info base;/* base information                       */
        struct {
            uint_8 flags;       /* for LINSYM records                   */
            uint_16 public_name_idx; /* for LINSYM records              */
        } linsym;
    } d;
    uint_16 num_lines;      /* number of elements in following array    */
    struct linnum_data {
        uint_16 number;     /* line number in source file               */
        uint_32 offset;     /* offset into segment                      */
    } *lines;               /* array of size num_lines                  */
};
/*
    No data should be attached to these records.  All necessary information
    is in the lines array.
*/


struct pubdef_info {
    struct base_info base;  /* base information                         */
    uint_16 num_pubs;       /* number of publics in following array     */
    struct pubdef_data {
        name_handle name;   /* name of this public                      */
        uint_32     offset; /* public offset                            */
        union {             /* see PUBDEF.h for more information        */
            uint_16     idx;/* Intel OMF type index                     */
#if _WOMP_OPT & _WOMP_WOMP
            symb_handle hdl;/* internal symbol handle...                */
#endif
        } type;
    } *pubs;                /* array of size num_pubs                   */
    uint_8 free_pubs : 1;   /* can we MemFree the pubs array?           */
    uint_8 processed : 1;   /* for use by dbg_generator (init'd to 0)   */
};
/*
    (This format for PUBDEFs is probably only useful for WOMP.)  No data
    should be attached to this record.  Everything is described by the
    pubs array.
*/
#endif


union objrec_info {
    struct coment_info  coment;
    struct modend_info  modend;
    struct lnames_info  lnames;
    struct lnames_info  llnames;
    struct lnames_info  extdef;
    struct lnames_info  comdef;
    struct lnames_info  cextdf;
    struct grpdef_info  grpdef;
    struct segdef_info  segdef;
    struct ledata_info  ledata;
    struct ledata_info  lidata;
    struct base_info    base;
#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0
    struct fixup_info   fixup;
    struct linnum_info  linnum;
    struct linnum_info  linsym;
    struct pubdef_info  pubdef;
    struct comdat_info  comdat;
#endif
};

struct obj_rec {
    obj_rec     *next;
    uint_16     length;     /* the length field for this record  (PRIVATE)  */
    uint_16     curoff;     /* offset of next read within record (PRIVATE)  */
    uint_8      *data;      /* data for this record              (PRIVATE)  */
    uint_8      command;    /* the command field for this record            */
    uint_8      is_phar : 1;/* is this a PharLap record                     */
    uint_8      is_32   : 1;/* is this a Microsoft 32bit record             */
    uint_8      free_data:1;/* should we MemFree( data )??       (PRIVATE)  */
    union objrec_info d;    /* data depending on record type                */
};

#include "poppck.h"

/*
    Nothing should rely on the data pointing to the same buffer all the time.
    i.e., any routine is allowed to ObjDetachData( objr ) and
        ObjAttachData( objr, ptr ) or ObjAllocData( objr, len )

    Most of the above structure is private to objrec.c (and the macros
    defined below).  See the following functions for instructions about
    manipulating the above structure.
*/


extern void         ObjRecInit( void );
extern void         ObjRecFini( void );
/*
    ObjRecInit must be called before any of the other routines in this
    module.  ObjRecFini free's all existing object records, and any
    memory used by the module.
*/


extern obj_rec      *ObjNewRec( uint_8 command );
/*
    Create an object record of type 'command'.  Does not allocate or attach
    any data to the record, or fill in any of the specific fields for each
    object record.
*/


extern void         ObjKillRec( obj_rec *objr );
/*
    Free's the memory used by an object record.  If the record had data
    allocated for it (ObjAllocData) or ObjCanFree was called on the record,
    then the data is free'd as well.  Records with extra memory (such as
    the fixup chain on FIXUPs, or the line number array on LINNUM) have the
    extra memory free'd as well.
*/


extern void         ObjAllocData( obj_rec *objr, uint_16 len );
/*
    Allocate a piece of memory of length len, attach it to the object
    record, and set the can_free bit.  This is the most common way to
    fill in data for object records.  Data allocated this way will be
    freed when ObjKillRec/ObjDetachData is called on the record.
*/


extern void         ObjAttachData( obj_rec *objr, uint_8 *data, uint_16 len );
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


extern void         ObjDetachData( obj_rec *objr );
/*
    Free's the data associated with an object record, but does not free
    the actual object record itself.  Called as part of ObjKillRec().
*/


extern void         ObjCanFree( obj_rec *objr );
/*
    Indicates that ObjDetachData or ObjKillRec can free the data associated
    with this record.  Not necessary if the data was allocated with
    ObjAllocData.
*/


extern uint_8       ObjGet8( obj_rec *objr );
extern uint_16      ObjGet16( obj_rec *objr );
extern uint_32      ObjGet32( obj_rec *objr );
extern uint_32      ObjGetEither( obj_rec *objr );
extern uint_16      ObjGetIndex( obj_rec *objr );
extern uint_8       *ObjGet( obj_rec *objr, uint_16 len );
extern int          ObjEOR( obj_rec *objr );
extern uint_16      ObjRTell( obj_rec *objr );
extern void         ObjRSeek( obj_rec *objr, uint_16 set );
extern uint_16      ObjRemain( obj_rec *objr );
extern void         ObjPut8( obj_rec *objr, uint_8 byte );
extern void         ObjPut16( obj_rec *objr, uint_16 word );
extern void         ObjPut32( obj_rec *objr, uint_32 dword );
extern void         ObjPutIndex( obj_rec *objr, uint_16 idx );
extern void         ObjPutEither( obj_rec *objr, uint_32 val );
extern void         ObjPut( obj_rec *objr, const uint_8 *data, uint_16 len );
extern void         ObjPutName( obj_rec *objr, const char *name, uint_8 len );
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


extern void         ObjTruncRec( obj_rec *objr );
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
