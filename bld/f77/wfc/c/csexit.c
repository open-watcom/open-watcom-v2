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
// CSEXIT    : processing for EXIT statement
//

#include "ftnstd.h"
#include "errcod.h"
#include "global.h"
#include "insert.h"
#include "utility.h"

extern  void            BlockLabel(void);
extern  void            CSExtn(void);
extern  itnode          *GetBlockLabel(void);
extern  void            GBranch(label_id);


void    CpExit(void) {
//================

// Compile the EXIT statement.

    itnode      *block_label;
    csnode      *csblock;

    CSExtn();
    block_label = GetBlockLabel();
    csblock = CSHead;
    for(;;) {
        if( ( csblock->typ == CS_DO ) ||
            ( csblock->typ == CS_DO_WHILE ) ||
            ( csblock->typ == CS_WHILE ) ||
            ( csblock->typ == CS_LOOP ) ||
            ( csblock->typ == CS_GUESS ) ||
            ( csblock->typ == CS_ADMIT ) ||
            ( csblock->typ == CS_REMOTEBLOCK ) ) {
            if( block_label->opnd_size == 0 ) break;
            if( CmpNode2Str( block_label, &csblock->label ) ) break;
        }
        if( csblock->link == NULL ) break;
        csblock = csblock->link;
    }
    if( csblock->typ == CS_EMPTY_LIST ) {
        StmtErr( SP_BAD_QUIT );
    } else {
        GBranch( csblock->bottom );
    }
    BlockLabel();
}
