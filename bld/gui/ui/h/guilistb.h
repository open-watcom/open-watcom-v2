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


#ifndef _GUILISTB_H_
#define _GUILISTB_H_

extern void GUIFreeList( a_list *list, bool free_list );
extern a_list *GUICreateListBox( void );
extern a_list *GUICreateEditMLE( const char *text );
extern bool GUIListBoxAddText( a_list *, const char *, gui_ctl_idx choice );
extern bool GUIListBoxAddTextList( a_list *, int items, const void *data_handle, GUIPICKGETTEXT *getstring );
extern bool GUIListBoxDeleteItem( a_list *list, gui_ctl_idx choice );
extern bool GUIAddListBox( gui_control *control, gui_control_info *info,
                           gui_colour_set *plain, gui_colour_set *standout );
extern bool GUIListBox( gui_control *control , EVENT ev, gui_ord row, gui_ord col );
extern char *GUIGetListBoxText( a_list *list, gui_ctl_idx choice, bool get_curr );
extern bool GUIListCurr( a_list *list, gui_ctl_idx choice, bool set, gui_ctl_idx *ret );
extern bool GUIFillInListBox( a_list *list );
extern bool GUIClearListBox( a_list *list );
extern a_list *GUIGetListFromControl( gui_control *contr );
extern gui_ctl_idx GUIListSize( a_list *list );
extern bool GUIListBoxTopIndex( a_list  *list, gui_ctl_idx choice, bool set, gui_ctl_idx *ret );

#endif // _GUILISTB_H_
