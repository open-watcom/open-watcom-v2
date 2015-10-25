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


//
// FmtInit      : FORMAT statement initialization routines
//

#include "ftnstd.h"
#include "fmterr.h"
#include "fmtdef.h"
#include "fmtdat.h"
#include "format.h"
#include "global.h"
#include "fmttab.h"
#include "cfmttab.h"
#include "fmtemit.h"
#include "fmtinit.h"
#include "fcodes.h"
#include "gflow.h"
#include "cgformat.h"

extern  void            R_FDoSpec(void);

extern  const FmtElements CFmtStruct;


static  void    FInit( int fmt_length, char *fmt_string ) {
//=========================================================

    Fmt_start = fmt_string;
    Fmt_end = fmt_string + fmt_length;
    Fmt_paren_level = 0;
    Fmt_charptr = fmt_string;
    Fmt_delimited = NO_DELIM;
    FmtEmStruct = &CFmtStruct;
}


static  void    FFinish(void) {
//=========================

    if( StmtProc == PR_FMT ) {
        for(;;) {
            if( *Fmt_charptr != ' ' ) break;
            ++Fmt_charptr;
            if( Fmt_charptr >= Fmt_end ) break;
        }
        if( ( Fmt_charptr < Fmt_end ) && ( *Fmt_charptr != '!' ) ) {
            FmtError( FM_NO_EOS );
        }
    }
}

void    FScan( int fmt_length, char *fmt_string, cs_label fmt_label ) {
//=====================================================================

// FORMAT statement parsing (only compile-time).


    FInit( fmt_length, fmt_string );
    StartFmt( fmt_label );
    R_FDoSpec();
    FFinish();
    GFEmEnd();
    EndFmt();
}

