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
// CGFORMAT  : FORMAT processing
//

#include "ftnstd.h"
#include "global.h"
#include "fcodes.h"
#include "wf77defs.h"
#include "fcgbls.h"
#include "cg.h"
#include "fmthdr.h"
#include "emitobj.h"


void    StartFmt( cs_label fmt_label ) {
//======================================

// Start format processing.

    obj_ptr     new_fmt;

    EmitOp( FC_FCODE_SEEK );
    new_fmt = ObjTell();
    OutU16( 0 );
    OutObjPtr( FormatList );
    FormatList = new_fmt;
    if( StmtProc == PR_FMT ) {
        if( fmt_label.st_label == NULL ) {
            // FORMAT statement with no statement label
            OutU16( 0 );
        } else {
            OutU16( fmt_label.st_label->u.st.address );
        }
    } else {
        OutU16( fmt_label.g_label );
    }
}


void    EndFmt( void ) {
//================

// Finish format processing.

    obj_ptr     fmt_ptr;
    unsigned_16 fmt_len;

    fmt_len = ObjOffset( FormatList ) - sizeof( unsigned_16 );
    fmt_ptr = ObjSeek( FormatList );
    OutU16( fmt_len );
    ObjSeek( fmt_ptr );
}


void    InitFormatList( void ) {
//========================

// Initialize format list.

    FormatList = 0;
}


void    FiniFormatList( void ) {
//========================

// Finish format list.

}
