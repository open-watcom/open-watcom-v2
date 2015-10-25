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


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbglit.h"
#include "dbgerr.h"
#include "dlgamb.h"
#include "dui.h"
#include "strutil.h"
#include "dbgutil.h"
#include "addarith.h"
#include "dbglkup.h"


sym_list *Disambiguate( sym_list *sym, location_context *lc )
{
    sym_list    *chosen;
    sym_list    *next,*curr,*other;
    int         count;
    int         new;
    ambig_info  ambig;

    if( _IsOn( SW_AMBIGUITY_FATAL ) ) Error( ERR_NONE, LIT_ENG( ERR_NO_ACCESS ) );
    ambig.sym = sym;
    ambig.lc = lc;
    count = 0;
    for( curr = sym; curr != NULL; curr = curr->next ) {
        if( SymLocation( SL2SH( curr ), lc, &curr->loc ) != DS_OK ||
            curr->loc.num != 1 || curr->loc.e[0].type != LT_ADDR ) {
            curr->loc.num = 0;
        }
        ++count;
    }
    for( curr = sym; curr != NULL; curr = next ) {
        next = curr->next;
        if( curr->loc.num == 1 && IS_NIL_ADDR( curr->loc.e[0].u.addr ) ) {
            --count;
            FreeSymHandle( curr );
            continue;
        }
        for( other = sym; other != curr; other = other->next ) {
            if( other->loc.num == 1 && curr->loc.num == 1 &&
                AddrComp( curr->loc.e[0].u.addr, other->loc.e[0].u.addr ) == 0 ) {
                FreeSymHandle( curr );
                --count;
                break;
            }
        }
    }

    if( count == 0 ) Error( ERR_NONE, LIT_ENG( ERR_NO_ACCESS ) ); // phooey!
    if( count == 1 ) return( sym );

    if( _IsOn( SW_NO_DISAMBIGUATOR ) ) {
        new = 0;
    } else {
        new = DUIDisambiguate( &ambig, count );
    }
    if( new == -1 )
        Error( ERR_SILENT, LIT_ENG( Empty ) ); // goodbye cruel world!
    chosen = NULL;
    while( sym != NULL ) {
        next = sym->next;
        if( new == 0 ) {
            chosen = sym;
        } else {
            FreeSymHandle( sym );
        }
        --new;
        sym = next;
    }
    return( chosen );
}
