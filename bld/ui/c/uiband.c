/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#include "uidef.h"
#include "uibox.h"
#include "uigchar.h"

static UI_WINDOW        BandWnd;
static SAREA            BandArea;
static ATTR             BandAttr;


static void drawband_update_fn( SAREA area, void *dummy )
{
    /* unused parameters */ (void)area; (void)dummy;

    drawbox( &UIData->screen, BandArea, SBOX_CHARS(), BandAttr, false );
}

void UIAPI uibandinit( SAREA area, ATTR attr )
{
    BandAttr = attr;
    area.width++;
    area.height++;
    BandArea = area;
    area.width = 0;
    area.height = 0;
    BandWnd.area = area;
    BandWnd.priority = P_UNBUFFERED;
    BandWnd.update_func = drawband_update_fn;
    BandWnd.update_parm = NULL;
    openwindow( &BandWnd );
    BandWnd.dirty_area = BandArea;
}


void UIAPI uibandmove( SAREA area )
{
    area.width++;
    area.height++;
    uidirty( BandArea );
    BandArea = area;
    uidirty( area );
    BandWnd.dirty_area = BandArea;
}


void UIAPI uibandfini( void )
{
    uidirty( BandArea );
    closewindow( &BandWnd );
    BandWnd.update_func = NULL;
}
