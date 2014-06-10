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


#ifndef WEDIT_INCLUDED
#define WEDIT_INCLUDED

#include "w_menu.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern void WInitEditWindows( HINSTANCE );
extern void WFiniEditWindows( void );
extern void WInitEditDlg( HINSTANCE inst, HWND parent );
extern bool WCreateMenuEditWindow( WMenuEditInfo *, HINSTANCE );
extern bool WResizeMenuEditWindow( WMenuEditInfo *, RECT * );
extern bool WSetEditWinResName( WMenuEditInfo * );
extern bool WInitEditWindowListBox( WMenuEditInfo * );
extern void WHandleSelChange( WMenuEditInfo * );
extern void WDoHandleSelChange( WMenuEditInfo *, bool, bool );
extern bool WResetEditWindowFlags( HWND dlg );

extern void WSetEditWindowControls( WMenuEditInfo *, WMenuEntry * );
extern bool WSetEditWindowMenuEntry( WMenuEditInfo *, WMenuEntry * );
extern bool WGetEditWindowMenuEntry( WMenuEditInfo *, WMenuEntry *, bool, bool * );

extern bool WSetEditWindowText( HWND, MenuFlags, char * );
extern bool WSetEditWindowID( HWND, uint_16, bool, char * );
extern bool WSetEditWindowFlags( HWND, MenuFlags, bool );

extern bool WGetEditWindowText( HWND, char ** );
extern bool WGetEditWindowID( HWND, char **, uint_16 *, WRHashTable *, bool );
extern bool WGetEditWindowFlags( HWND, MenuFlags * );
extern bool WClipMenuItem( WMenuEditInfo *einfo, bool cut );
extern bool WPasteMenuItem( WMenuEditInfo *einfo );

#endif
