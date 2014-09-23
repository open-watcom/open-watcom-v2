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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "auipvt.h"

static int          DlgChosen;
static void         *_data_handle;
static PICKGETTEXT  *_getstring;
static int          _items;

static void PickInit( gui_window *gui, unsigned list_ctrl )
{
    GUIAddTextList( gui, list_ctrl, _items, _data_handle, _getstring );
    GUISetCurrSelect( gui, list_ctrl, DlgChosen );
}

int DlgPickWithRtn2( char *text, void *data_handle, int def, PICKGETTEXT *getstring, int items, GUIPICKER *pick )
{
    DlgChosen = def;
    _data_handle = data_handle;
    _getstring = getstring;
    _items = items;
    return( pick( text, &PickInit ) );
}


int DoDlgPick( char *text, PICKCALLBACK *pickinit )
{
    return( GUIDlgPickWithRtn( text, pickinit, DlgOpen ) );
}


int DlgPickWithRtn( char *text, void *data_handle, int def, PICKGETTEXT *getstring, int items )
{
    return( DlgPickWithRtn2( text, data_handle, def, getstring, items, DoDlgPick ) );
}

char *DlgPickText( void *data_handle, int item )
{
    return( ((char **)data_handle)[item] );
}

int DlgPick( char *text, void *data_handle, int def, int items )
{
    return( DlgPickWithRtn( text, data_handle, def, DlgPickText, items ) );
}
