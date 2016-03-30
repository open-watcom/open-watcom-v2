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
// USOTHER   : target dependant upscan routines
//

#include "ftnstd.h"
#include "astype.h"
#include "opn.h"
#include "global.h"
#include "stmtfunc.h"
#include "upscan.h"
#include "usother.h"
#include "exprutil.h"


extern  void            GCallNoArgs(void);
extern  void            GCallWithArgs(void);


void    EndExpr( void ) {
//=================

    // If we get an error during UPSCAN process, the argument list may
    // not have been detached.
    if( !AError ) {
        if( ASType & AST_CNA ) {
            GCallNoArgs();
        } else if( StmtProc == PR_CALL ) {
            GCallWithArgs();
        }
    }
    if( ( ( ASType & ( AST_DIM | AST_CEX ) ) == 0 ) &&
        ( CITNode->opn.us == USOPN_CON ) ) {
        AddConst( CITNode );
    }
    // We don't want to finish off the expression if an error
    // occurred during upscan process.
    if( !AError &&
        ( ( ASType & ( AST_DIM | AST_IO | AST_SUB | AST_CEX ) ) == 0 ) ) {
        EmExprDone();
    }
    if( ASType & AST_ASF ) {
        SFEpilogue();    // epilogue for stmt function
    }
    ASType = AST_OFF;
}
