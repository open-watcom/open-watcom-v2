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
// FMTERR -- issue error and extension messages for format statements
//

#include "ftnstd.h"
#include "fmterr.h"
#include "format.h"
#include "fmtdef.h"
#include "fmtdat.h"
#include "global.h"
#include "ferror.h"
#include "cfmttab.h"


static  unsigned_16     SetCaret(void) {
//==================================

// Set position of caret in format string.

    int         column;
    int         contline;
    int         scanned;
    unsigned_16 opnpos;

    opnpos = CITNode->opnpos;
    column = opnpos & 0xff;
    if( StmtProc != PR_FMT ) {
        column++;     // column points to starting quote of format literal
    }
    contline = opnpos >> 8;
    scanned = Fmt_charptr - Fmt_start;
    if( scanned > LastColumn - column + 1 ) {
        scanned -= LastColumn - column + 1;
        contline++;
        column = CONT_COL + 1;
        while( scanned >= LastColumn - CONT_COL ) {
            scanned -= LastColumn - CONT_COL;
            contline++;
        }
    }
    column += scanned;
    CITNode->opnpos = ( contline << 8 ) + column;
    return( opnpos );
}


void    FmtError( int err_code ) {
//================================

// Issue a format statement error message.

    unsigned_16 opnpos;

    opnpos = SetCaret();
    Error( err_code );
    if( err_code != FM_DELIM ) {
        for(;;) {
            if( *Fmt_charptr == ',' ) break;
            if( *Fmt_charptr == '/' ) break;
            if( *Fmt_charptr == ':' ) break;
            if( *Fmt_charptr == ')' ) break;
            if( *Fmt_charptr == '(' ) break;
            ++Fmt_charptr;
            if( Fmt_charptr >= Fmt_end ) break;
        }
    }
    CITNode->opnpos = opnpos;
}


void    FmtExtension( int ext_code ) {
//====================================

// Issue a format statement extension message.

    unsigned_16 opnpos;

    opnpos = SetCaret();
    Extension( ext_code );
    CITNode->opnpos = opnpos;
}
