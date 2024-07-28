/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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

static void GUICALLBACK PickInit( gui_window *gui, gui_ctl_id list_id )
{
    GUIAddTextList( gui, list_id, _num_items, _data_handle, _getstring );
    GUISetCurrSelect( gui, list_id, _def_item );
}

bool WNDAPI DlgPickWithRtn2( const char *title, const void *data_handle, int def_item, GUIPICKGETTEXT *getstring, int num_items, WNDPICKER *pickfn, int *choice )
{
    _def_item = def_item;
    _data_handle = data_handle;
    _getstring = getstring;
    _num_items = num_items;
    return( pickfn( title, &PickInit, choice ) );
}

static void GUICALLBACK doDlgOpen( const char *title, gui_text_ord rows, gui_text_ord cols, gui_control_info *ctl, int num_controls, GUIEVCALLBACK *gui_call_back, void *extra )
{
    DlgOpen( title, rows, cols, ctl, num_controls, gui_call_back, extra );
}

static bool WNDCALLBACK doDlgPick( const char *title, GUIPICKCALLBACK *pickinit, int *choice )
{
    return( GUIDlgPickWithRtn( title, pickinit, doDlgOpen, choice ) );
}

bool WNDAPI DlgPickWithRtn( const char *title, const void *data_handle, int def_item, GUIPICKGETTEXT *getstring, int num_items, int *choice )
{
    return( DlgPickWithRtn2( title, data_handle, def_item, getstring, num_items, doDlgPick, choice ) );
}

static const char * GUICALLBACK doDlgPickText( const void *data_handle, int item )
{
    return( ((const char **)data_handle)[item] );
}

bool WNDAPI DlgPick( const char *title, const void *data_handle, int def_item, int num_items, int *choice )
{
    return( DlgPickWithRtn( title, data_handle, def_item, doDlgPickText, num_items, choice ) );
}
