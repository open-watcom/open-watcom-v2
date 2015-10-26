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
// NAMELIST     : NAMELIST statement processor
//

#include "ftnstd.h"
#include "global.h"
#include "errcod.h"
#include "namecod.h"
#include "recog.h"
#include "insert.h"
#include "utility.h"

extern  sym_id          LkSym(void);
extern  sym_id          LkNameList(void);
extern  grp_entry       *STGroupEntry(void);


void    CpNameList( void ) {
//====================

// Process NAMELIST statement.
//      NAMELIST  /namelist/ sym {,sym} { /namelist/ sym ,{sym} }

    sym_id      name_list;
    sym_id      sym;
    grp_entry   *last;
    grp_entry   *ge;

    StmtExtension( SP_STRUCTURED_EXT );
    ReqNOpn();
    AdvanceITPtr();
    for(;;) {
        ReqDiv();
        if( ReqName( NAME_GROUP ) ) {
            name_list = LkNameList();
        } else {
            name_list = NULL;
        }
        AdvanceITPtr();
        ReqDiv();
        for(;;) {
            if( ReqName( NAME_VAR_OR_ARR ) ) {
                sym = LkSym();
            } else {
                sym = NULL;
            }
            if( ( name_list != NULL ) && ( sym != NULL ) ) {
                ge = STGroupEntry();
                ge->sym = sym;
                last = name_list->u.nl.group_list;
                if( last == NULL ) {    // if empty name list
                    name_list->u.nl.group_list = ge;
                } else {
                    for(;;) {
                        if( last->sym == sym ) {
                            NameErr( VA_SAME_NAMELIST, sym );
                        }
                        if( last->link == NULL ) break;
                        last = last->link;
                    }
                    last->link = ge;
                }
            }
            AdvanceITPtr();
            if( RecComma() && RecNOpn() ) {
                AdvanceITPtr();
            }
            if( !RecComma() ) break;
        }
        if( !RecDiv() ) break;
    }
    ReqEOS();
}
