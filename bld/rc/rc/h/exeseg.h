/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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


typedef enum {
    CPSEG_OK = 0,
    CPSEG_SEG_TOO_BIG,
    CPSEG_ERROR
} CpSegRc;

extern RcStatus AllocAndReadWINSegTables( ExeFileInfo *src, ExeFileInfo *dst, int *err_code );
extern RcStatus AllocAndReadOS2SegTables( ExeFileInfo *src, ExeFileInfo *dst, ResFileInfo *res, int *err_code );
extern uint_32 ComputeSegmentSize( FILE *fp, SegTable *, int shift_count );
extern CpSegRc CopyWINSegments( ExeFileInfo *src, ExeFileInfo *dst, uint_16 sect2mask, uint_16 sect2bits, bool sect2 );
extern CpSegRc CopyOS2Segments( ExeFileInfo *src, ExeFileInfo *dst );

#endif
