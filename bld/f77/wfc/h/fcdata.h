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


//========================================================================
//
// DO NOT disturb the order of these variables as the IBM PC relies on
// this order for proper alignment.
//
//========================================================================

gbl_defn pointer                StkPtr;         // F-Code stack pointer
gbl_defn unsigned_32            CodeSize;       // size of program object code
gbl_defn label_id               LabelIdx;       // label index
gbl_defn sym_id                 WildLabel;      // label of a wild goto
gbl_defn pointer                LabelList;      // list of labels
gbl_defn obj_ptr                FormatList;     // pointer to format list
gbl_defn obj_ptr                PrologLabel;    // prologue location
gbl_defn label_id               EpilogLabel;    // epilogue label
gbl_defn sym_id                 CommonEntry;    // common entry point
gbl_defn sym_id                 EPValue;        // entry point identifier
gbl_defn sym_id                 ReturnValue;    // function return value
gbl_defn void         (* __FAR *FCTablePtr)(void);  // pointer to F-Code jump table
gbl_defn obj_ptr                DtConstList;    // DATA statement constants
gbl_defn PTYPE                  DtConstType;    // type of DATA constant
gbl_defn intstar4               DtRepCount;     // repitition count
gbl_defn sym_id                 DtConst;        // DATA constant
gbl_defn segment_id             DtSegment;      // DATA initialization segment
gbl_defn seg_offset             DtSegOffset;    // offset in DATA initialized
                                                // segment
gbl_defn intstar4               DtOffset;       // offset relative to variable
                                                // being initialized
gbl_defn unsigned_8             DtFlags;        // DATA intialization flags

gbl_defn int                    EndEqLabel;     // END= label
gbl_defn int                    ErrEqLabel;     // ERR= label
gbl_defn obj_ptr                AtEndFCode;     // pointer to ATEND F-Code

gbl_defn unsigned_32            MaxSegSize;     // maximum segment size

gbl_defn uint                   DataThreshold;  // data threshold

gbl_defn seg_offset             LDSegOffset;    // offset into LDATA
gbl_defn seg_offset             GSegOffset;     // offset into global segments

gbl_defn int                    NumSubProgs;    // # of subprograms compiled
