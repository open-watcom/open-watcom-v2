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
// INCLUDE      : INCLUDE statement processor
//

#include "ftnstd.h"
#include "global.h"
#include "opn.h"
#include "errcod.h"
#include "recog.h"
#include "ferror.h"
#include "insert.h"
#include "comio.h"
#include "inout.h"
#include "utility.h"


void    CpInclude(void) {
//===================

// Process INCLUDE statement.
//      INCLUDE 'file-name'

    char        *lit;

    StmtExtension( SP_STRUCTURED_EXT );
    if( RecLiteral() ) {
        lit = MkNodeStr( CITNode );
        AdvanceITPtr();
        if( ReqEOS() ) {
            Include( lit );
            ComRead();
        }
        FrNodeStr( lit );
    } else {
        Error( SX_EXPECT_CHAR_CONST );
    }
}
