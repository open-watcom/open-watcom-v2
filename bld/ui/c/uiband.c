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


#include "uidef.h"
#include "uibox.h"
#include "uigchar.h"

static UI_WINDOW        BandWnd;
static SAREA            BandArea;
static ATTR             Attr;


static void drawband( SAREA area, void *dummy )
{
    drawbox( &UIData->screen, BandArea, (char *)&UiGChar[ UI_SBOX_TOP_LEFT ],
             Attr, FALSE );
}

void UIAPI uibandinit( SAREA start, ATTR attr )
{
    Attr = attr;
    start.width++;
    start.height++;
    BandArea = start;
    start.width = 0;
    start.height = 0;
    BandWnd.area = start;
    BandWnd.priority = P_UNBUFFERED;
    BandWnd.update = drawband;
    BandWnd.parm = NULL;
    openwindow( &BandWnd );
    BandWnd.dirty = BandArea;
}


void UIAPI uibandmove( SAREA new )
{
    new.width++;
    new.height++;
    uidirty( BandArea );
    BandArea = new;
    uidirty( new );
    BandWnd.dirty = BandArea;
}


void UIAPI uibandfini( void )
{
    uidirty( BandArea );
    closewindow( &BandWnd );
    BandWnd.update = NULL;
}
