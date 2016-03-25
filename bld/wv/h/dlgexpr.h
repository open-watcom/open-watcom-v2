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


extern bool     DlgGetLong( gui_window *gui, gui_ctl_id id, long *value );
extern bool     DlgGetCodeAddr( gui_window *gui, gui_ctl_id id, address *value );
extern bool     DlgGetDataAddr( gui_window *gui, gui_ctl_id id, address *value );
extern void     DlgSetLong( gui_window *gui, gui_ctl_id id, long value );
extern bool     DlgLongExpr( const char *title, long *value );
extern bool     DlgAnyExpr( const char *title, char *buff, unsigned buff_len );
extern bool     DlgCodeAddr( const char *title, address *value );
extern bool     DlgDataAddr( const char *title, address *value );
extern bool     DlgGivenAddr( const char *title, address *value );
extern bool     DlgModName( const char *title, mod_handle *mod );
extern bool     DlgString( const char *title, char *buff );
extern bool     DlgMadTypeExpr( const char *title, item_mach *value, mad_type_handle th );
