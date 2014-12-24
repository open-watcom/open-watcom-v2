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
#include "dlgayn.h"
#include "strutil.h"


OVL_EXTERN bool AynEvent( gui_window * gui, gui_event gui_ev, void *param )
{
    dlg_ayn     *ayn;
    unsigned    id;

    ayn = GUIGetExtra( gui );
    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        Format( TxtBuff, LIT( WARN_Program_Will_Slow ), ayn->mult );
        GUISetText( gui, CTL_AYN_NO_WAY, TxtBuff );
        GUISetFocus( gui, CTL_AYN_NO );
        return( TRUE );
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_AYN_YES:
            ayn->as_a_fruitcake = TRUE;
            /* fall through */
        case CTL_AYN_NO:
            GUICloseDialog( gui );
            return( TRUE );
        }
        return( FALSE );
    }
    return( FALSE );
}


bool DlgAreYouNuts( unsigned long mult )
{
    dlg_ayn     ayn;

    ayn.as_a_fruitcake = FALSE;
    ayn.mult = mult;
    ResDlgOpen( &AynEvent, &ayn, DIALOG_AYN );
    return( ayn.as_a_fruitcake );
}
