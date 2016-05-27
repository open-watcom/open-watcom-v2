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
* Description:  Macros for converting segment and offset data 
*               into linear address and subtracting and comparing
*
****************************************************************************/


extern segment Find16MSeg( segment );

#define SUB_REAL_ADDR( l, r ) ((((offset)(l).seg-(offset)(r).seg) << FmtData.SegShift) + ((l).off-(r).off))

#define SUB_16M_ADDR( l, r ) (((offset)Find16MSeg((l).seg)-(offset)Find16MSeg((r).seg))*16+((l).off-(r).off))

#define LESS_THAN_ADDR( l, r ) (MK_REAL_ADDR((l).seg,(l).off) < MK_REAL_ADDR((r).seg,(r).off))

#define MK_REAL_ADDR( seg, off )  ( ((offset)(seg) << FmtData.SegShift) + (off) )

#define SEG_GROUP_DELTA(x)  SUB_REAL_ADDR( (x)->seg_addr, (x)->group->grp_addr )

#define GROUP_SECTION_DELTA(x) SUB_REAL_ADDR( (x)->grp_addr, (x)->section->sect_addr )

#define GROUP_FILE_LOC(x) ( SUB_REAL_ADDR( (x)->grp_addr, (x)->section->sect_addr ) + (x)->section->u.file_loc )
