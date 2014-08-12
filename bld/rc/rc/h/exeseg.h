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
* Description:  NE segment table manipulation prototypes and structures.
*
****************************************************************************/


#ifndef EXESEG_INCLUDED
#define EXESEG_INCLUDED

#include "rctypes.h"

/* NB: NumOS2ResSegs is a subset of total segments (must be <= NumSegs) but
 * is not always equal to number of resources, because resources > 64K will
 * be split into multiple segments! Only applicable to OS/2 NE, not Windows.
 */
typedef struct SegTable {
    uint_16             NumSegs;        /* Total number of segments */
    uint_16             NumOS2ResSegs;  /* Number of resource segments */
    segment_record      *Segments;      /* array of size NumSegs */
} SegTable;

typedef enum {
    CPSEG_OK = 0,
    CPSEG_SEG_TOO_BIG,
    CPSEG_ERROR
} CpSegRc;

extern RcStatus AllocAndReadWINSegTables( int *err_code );
extern RcStatus AllocAndReadOS2SegTables( int *err_code );
extern uint_32 ComputeSegmentSize( WResFileID handle, SegTable *, int shift_count );
extern CpSegRc CopyWINSegments( uint_16 sect2mask, uint_16 sect2bits, bool sect2 );
extern CpSegRc CopyOS2Segments( void );

#endif
