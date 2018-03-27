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


#include "_aui.h"

static int              _def_item;
static const void       *_data_handle;
static GUIPICKGETTEXT   *_getstring;
static int              _num_items;

static void PickInit( gui_window *gui, gui_ctl_id list_id )
{
    GUIAddTextList( gui, list_id, _num_items, _data_handle, _getstring );
    GUISetCurrSelect( gui, list_id, _def_item );
}

bool DlgPickWithRtn2( const char *title, const void *data_handle, int def_item, GUIPICKGETTEXT *getstring, int num_items, WNDPICKER *pickfn, int *choice )
{
    _def_item = def_item;
    _data_handle = data_handle;
    _getstring = getstring;
    _num_items = num_items;
    return( pickfn( title, &PickInit, choice ) );
}


static bool DoDlgPick( const char *title, GUIPICKCALLBACK *pickinit, int *choice )
{
    return( GUIDlgPickWithRtn( title, pickinit, DlgOpen, choice ) );
}


bool DlgPickWithRtn( const char *title, const void *data_handle, int def_item, GUIPICKGETTEXT *getstring, int num_items, int *choice )
{
    return( DlgPickWithRtn2( title, data_handle, def_item, getstring, num_items, DoDlgPick, choice ) );
}

static const char *DlgPickText( const void *data_handle, int item )
{
    return( ((const char **)data_handle)[item] );
}

bool DlgPick( const char *title, const void *data_handle, int def_item, int num_items, int *choice )
{
    return( DlgPickWithRtn( title, data_handle, def_item, DlgPickText, num_items, choice ) );
}
