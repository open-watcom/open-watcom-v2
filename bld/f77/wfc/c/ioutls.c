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
// IOUTLS    : utilities for io processing
//

#include "ftnstd.h"
#include "iodefs.h"
#include "iortncon.h"
#include "global.h"
#include "ioperm.h"
#include "ioutls.h"
#include "rtconst.h"
#include "fcodes.h"
#include "gio.h"


void    InitIO(void) {
//================

    IOData = 0;
    GSetIOCB();
}


void    FiniIO(void) {
//================

}


uint    IOIndex(void) {
//=================

    switch( StmtProc ) {
    case PR_READ:       return( EX_READ );
    case PR_PRINT:      return( EX_WRITE );
    case PR_WRITE:      return( EX_WRITE );
    case PR_OPEN:       return( EX_OPEN );
    case PR_CLOSE:      return( EX_CLOSE );
    case PR_BKSP:       return( EX_BK_SPACE );
    case PR_EFILE:      return( EX_END_FILE );
    case PR_REW:        return( EX_REWIND );
    }
    return( EX_INQUIRE );
}


uint    AuxIOStmt(void) {
//===================

    switch( StmtProc ) {
    case PR_READ:
    case PR_PRINT:
    case PR_WRITE:
        return( FALSE );
    }
    return( TRUE );
}


bool    NotFormatted(void) {
//======================

    return( !Already( IO_FMT ) );
}
