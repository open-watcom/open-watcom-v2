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
#include "dbgwind.h"
#include "dbgerr.h"
#include "guidlg.h"
#include "guipick.h"
#include "dlgamb.rh"
#include "dui.h"
#include "strutil.h"
#include "dbgutil.h"
#include "addarith.h"
#include "dbglkup.h"


static bool SymPick( const char *text, GUIPICKCALLBACK *pick_call_back, int *choice )
{
    dlg_pick    dlg;

    /* unused parameters */ (void)text;

    dlg.func = pick_call_back;
    dlg.choice = -1;
    ResDlgOpen( GUIPickGUIEventProc, &dlg, DIALOG_AMBIG );
    if( dlg.choice == -1 )
        return( false );
    *choice = dlg.choice;
    return( true );
}

static const char *SymPickText( const void *data_handle, int item )
{
    sym_list            *sym;
    unsigned            len;
    const char          *image_name;
    const ambig_info    *ambig = data_handle;

    sym = ambig->sym;
    while( item-- > 0 ) {
        sym = sym->next;
    }
    len = DIPSymName( SL2SH( sym ), ambig->lc, SNT_DEMANGLED, TxtBuff, TXT_LEN );
    if( len == 0 ) {
        len = DIPSymName( SL2SH( sym ), ambig->lc, SNT_SOURCE, TxtBuff, TXT_LEN );
    }
    image_name = ModImageName( DIPSymMod( SL2SH( sym ) ) );
    if( *image_name != NULLCHAR ) {
        Format( &TxtBuff[len], " [%s]", image_name );
    }
    return( TxtBuff );
}

bool DUIDisambiguate( const ambig_info *ambig, int num_items, int *choice )
{
    return( DlgPickWithRtn2( "", ambig, 0, SymPickText, num_items, SymPick, choice ) );
}
