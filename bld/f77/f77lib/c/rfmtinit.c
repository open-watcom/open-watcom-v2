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
* Description:  Run-time FORMAT initialization routines
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "ftextvar.h"
#include "rundat.h"
#include "format.h"
#include "fmtdef.h"
#include "fmtdat.h"
#include "rtenv.h"
#include "fmttab.h"

static  void    FInit( string *fmt ) {
//====================================

    FmtEmStruct = &RFmtStruct;
    SetFmt( &FmtBuff[ 0 ] );
    IOCB->fmtlen = SCAN_STORAGE_SIZE;
    Fmt_charptr = fmt->strptr;
    Fmt_end = Fmt_charptr + fmt->len;
    Fmt_paren_level = 0;
    Fmt_revert.rt = &FmtBuff[ 0 ];
    Fmt_delimited = NO_DELIM;
    R_FEmInit();
}


static  void    FFinish( void ) {
//=========================

    R_FEmEnd();
    SetFmt( &FmtBuff[ 0 ] );
}


void    FmtScan( string *fmt, uint extend_format ) {
//==================================================

    _SetIOCB();
    if( extend_format ) {
        IOCB->flags |= IOF_EXTEND_FORMAT;
    }
    FInit( fmt );
    R_FDoSpec();
    FFinish();
}


void    FmtAScan( char PGM *array, long int num_elts, int elt_size,
                  uint extend_format ) {
//=================================================================

// Encode format specification from a character array.

    string      scb;

    _SetIOCB();
    scb.strptr = array;
    scb.len = num_elts * elt_size;
    FmtScan( &scb, extend_format );
}


