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


#include <string.h>
#include "dbgdefn.h"
#include "dbgwind.h"
#include "dbginfo.h"
#include "dbgerr.h"
#include "guidlg.h"
#include "guipick.h"
#include "dbgtoggl.h"
#include "dlgamb.h"
#include "ambigsym.h"
#include <string.h>

extern void             FreeSymHandle( sym_list * );
extern int              AddrComp( address a, address b );
extern char             *ModImageName( mod_handle handle );
extern char             *Format( char *buff, char *fmt, ... );

extern char             *TxtBuff;

extern GUIPICKER SymPick;
DLGPICKTEXT SymPickText;

extern int DUIDisambiguate( ambig_info *ambig, int count )
{
    return( DlgPickWithRtn2( "", ambig, 0, SymPickText, count, SymPick ) );
}

static int SymPick( char *text, PICKCALLBACK * PickInit )
{
    dlg_pick    dlg;

    text=text;
    dlg.func = PickInit;
    dlg.chosen = -1;
    ResDlgOpen( &GUIPickEvent, &dlg, DIALOG_AMBIG );
    return( dlg.chosen );
}
