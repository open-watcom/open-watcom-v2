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


#ifndef PRSOBJ_H
#define PRSOBJ_H    1
#include <stddef.h>
#include "objrec.h"
#include "watcom.h"
#include "pcobj.h"
#include "queue.h"
#include "objio.h"

/*
    The object file parser performs a pass over the object file.  For
    each record a series of steps are taken, and these steps may be
    intercepted mid-stream by filters outside of the object file
    parser.  This works in the following manner:

    1) The parser reads a record.
    2) If the record is a record which affects the state of the parser
       (i.e., LNAMES, EXTDEF, SEGDEF, ...) then the parser updates its
       state accordingly.
    3) The record is passed to the each filter in the list for this command
       in reverse order of their registration.  (i.e., last registered
       gets the record first).  If any of the routines returns -1, then
       the record is not passed any further.
    4) If the record was MODEND, then quit
    5) Go to step 1

    EACH FILTER MUST CONFORM TO THESE GUIDELINES!
    In particular, if a filter must move the record pointer it should
    return it to its proper spot.  This may entail a simple ObjRSeek( objr, 0 )
    or it might take a combo:  offset = ObjRTell( objr ); ...work...;
    ObjRSeek( objr, offset ).

    A filter MAY ASSUME that the following are true (by command):

    COMENT:
        objr->d.coment.attr = ObjGet8( objr );
        objr->d.coment.class = ObjGet8( objr );
        Record pointer is at first character after the attr and class bytes.
        CMT_EASY_OMF and CMT_MS_OMF are detected and are not passed on.

    MODEND:
        objr->d.modend has been initialized
        ObjDetachData( objr ) has been performed.

    LNAMES:
        objr->d.lnames.first_idx = index of first name in record
        objr->d.lnames.num_names = number of names in record
        record pointer is at end of record

    SEGDEF:
        objr->d.segdef.idx = index of this segdef
        the remainder of objr->d.segdef has been preprocessed
        ObjDetachData( objr ) has been performed; thus, no attempt should
        be made to access data in the record.

    EXTDEF, STATIC_EXTDEF:
        objr->d.extdef.first_idx = index of first name in record
        objr->d.extdef.num_names = number of names in record
        record pointer is at end of record

    COMDEF, STATIC_COMDEF:
        objr->d.comdef.first_idx = index of first name in record
        objr->d.comdef.num_names = number of names in record
        record pointer is at end of record

    FIXUP:
        objr->d.fixup.idx_last_data = seg index from most recent le/lidata
        objr->d.fixup.offset_last_data = offset from most recent le/lidata
        objr->d.fixup.fixup = linked list of processed fixups
            The fixups can be "stolen" if the record is not going to be
            passed on.  i.e., the WATPRS FIXUP filter steals all the fixups
            for the $$TYPES and $$SYMBOLS segments.  This eliminates the
            need to use FixDup and saves memory.
        ObjDetachData( objr ) has been performed; don't read data.
        Thread fixups don't exist internally; any fixup record containing
        just threads is not passed on from the preprocessor.

    LEDATA, LIDATA:
        objr->d.ledata.idx = segment index from record
        objr->d.ledata.offset = offset from record
        record pointer is just after data offset field (first byte of data)

    GRPDEF:
        objr->d.grpdef.idx = index of this grpdef
        record pointer is at beginning of record

    LINNUM:
        objr->d.linnum.base contains the base descriptor
        objr->d.linnum.num_lines contains the number of lines in the record
        objr->d.linnum.lines points to an array of num_lines linnum_datas.
        ObjDetachData( objr ) has been performed.

    PUBDEF:
        objr->d.pubdef.base contains the base descriptor
        objr->d.pubdef.num_pubs contains the number of pubs in the array
        objr->d.pubdef.pubs is an array of num_pubs pubdef_datas
        ObjDetachData( objr ) has been performed.

    All other record types:
        No preprocessing is done.  Record pointer is at beginning of record.
        This is subject to change as there is need to preprocess more record
        types.

    It is safe to modify the record before passing it on.

    Note that certain records must always be passed on (i.e., SEGDEF, EXTDEF,
    GRPDEF, LNAMES and their 32-bit counterparts) in order to maintain the
    index ordering of the original object file.  If you want to "eliminate"
    one of these records, then you should just munge the fields to some non-
    obtrusive values.  i.e., for SEGDEF you should make it combinable
    (COMB_ADDOFF) and set the seg_length to zero.  Any filter may assume that
    an obj_rec pointer passed for one of these records will not be ObjKillRec'd
    until all passes are complete.

    During READ_PASS, if all filters return 0, then the record is stored in a
    linked list of all records in the file.  The records used during WRITE_PASS
    come from this linked list of records left over from READ_PASS.
    ObjKillRec( objr ) is performed on all records regardless of the return
    values from the filters during WRITE_PASS.

    The parser assumes during READ_PASS that if a filter returns -1, then it
    has "taken over" the obj_rec. i.e., the filter will ensure that the obj_rec
    is ObjKillRec'd at an appropriate time.

    An LEDATA can't be ObjKillRec'd until all FIXUPs for that LEDATA have been
    read.  i.e., an LEDATA can't be ObjKillRec'd until the next LE/LIDATA.
    Further, those FIXUPs belong to the LEDATA and must be killed if the
    LEDATA is killed (sound like fun?).  The same applies to LIDATAs.

    Filters are allowed to unregister themselves, or any filter that is called
    before they are in the chain.

    The MS386 32-bit record types (LIDA32, LEDA32, ...) are passed as their
    16-bit counterparts, with the is_32 bit set in the obj_rec structure.

    During the WRITE_PASS, this is the ordering of the records:
        THEADR
        ( LNAMES | LLNAMES | SEGDEF | GRPDEF | COMENT ) *
        ( EXTDEF | STATIC_EXTDEF | COMDEF | STATIC_COMDEF | COMENT | CEXTDF ) *
        ( PUBDEF | STATIC_PUBDEF | COMENT ) *
        -- LAST_DEFN is called here
        ( LEDATA | LIDATA | FIXUP | COMENT | LINNUM |
            COMDAT | BAKPAT | LINSYM ) *
        MODEND

*/

typedef struct {
    uint_8      pass;           /* current pass number                      */
    OBJ_WFILE   *file_out;      /* output file pointer                      */
} pobj_state;

typedef int (*pobj_filter)( obj_rec *objr, pobj_state *state );

/* for lists of filters... */
typedef struct {
    uint_8      command;
    uint_8      pass;
    pobj_filter func;
} pobj_list;
#define POBJ_ALL_PASSES     0   /* invoke filter on all passes          */
#define POBJ_READ_PASS      1   /* invoke filter on the read pass only  */
#define POBJ_WRITE_PASS     2   /* invoke filter on the write pass only */

enum extra_commands {
        /* our effective minimum command (should be == 0(mod2)) */
    CMD_POBJ_MIN_CMD = ( CMD_MIN_CMD -             1  ) & ~1,
        /* the number of extra commands goes here ^^^ */
    CMD_LAST_DEFN = CMD_POBJ_MIN_CMD
        /* each extra command should be == 0 (mod2), so add 2 to previous */
};

#if _WOMP_OPT & _WOMP_EXTRAS
typedef struct {
    size_t      page_len;
    uint_8      is_lib : 1;
} pobj_lib_info;
#endif

extern uint_16 ObjDGROUPIndex;
extern uint_16 ObjFLATIndex;

#if _WOMP_OPT & _WOMP_EXTRAS
extern void PObjInit( void );
extern int PObj( OBJ_RFILE *file_in, OBJ_WFILE *file_out, pobj_lib_info *pli );
#else
extern void PObjInit( OBJ_WFILE *file_out );
#endif
extern void PObjFini( void );
extern void PObjRegister( uint_8 command, uint_8 pass, pobj_filter func );
extern void PObjRegList( const pobj_list *list, size_t len );
extern void PObjUnRegister( uint_8 command, uint_8 pass, pobj_filter func );
extern void PObjUnRegList( const pobj_list *list, size_t len );

/*
    PObjEnqueue will add a record between the current one and the next one.
    Actually, it maintains a queue that is added when all filters have seen
    the current record.  This function is only valid during POBJ_WRITE_PASS.
*/
extern void PObjEnqueue( obj_rec *objr );
extern void PObjJoinQueue( qdesc *q );

/*
    The following PObjNew functions are used to enqueue new records that are
    position dependant in the object file.  The call will reserve 1 (or more)
    LNAMES, SEGDEFs, etc., PObjEnqueue the record, and return you a pointer.
    Thus they are only valid during pass 2.  Furthermore, you must ensure that
    you call these functions AFTER the last LNAMES, SEGDEF, etc. has been
    processed.  (It is safe to call if the last LNAME, etc is the current
    record.)  See CAN2MS0.C for an example.
*/
extern obj_rec *PObjNewLnames( uint_16 num );
extern obj_rec *PObjNewSegdef( void );
extern obj_rec *PObjNewGrpdef( void );

#if _WOMP_OPT & _WOMP_EXTRAS
/*
    Used by pubdef.c
*/
extern qdesc *PObjPubdefQueue( void );
#endif

#endif
