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


#ifndef SYS_RC_INCLUDED
#define SYS_RC_INCLUDED

/****************************************************************************/
/* System independent identifiers                                           */
/****************************************************************************/
#include "w_rc.h"

/****************************************************************************/
/* System dependent identifiers                                             */
/****************************************************************************/
#include "menedit.h"

/****************************************************************************/
/* Menu Identifiers                                                         */
/****************************************************************************/

/****************************************************************************/
/* Menu Menu ID's                                                           */
/****************************************************************************/
#define IDM_MENU_CLEAR          100
#define IDM_MENU_UPDATE         101
#define IDM_MENU_OPEN           102
#define IDM_MENU_SAVE           103
#define IDM_MENU_SAVEAS         104
#define IDM_MENU_EXIT           105

#define IDM_MENU_CUT            110
#define IDM_MENU_COPY           111
#define IDM_MENU_PASTE          112
#define IDM_MENU_DELETE         113
#define IDM_MENU_LOAD_SYMBOLS   114
#define IDM_MENU_SYMBOLS        115
#define IDM_MENU_SHOWRIBBON     116

#define IDM_MENU_RENAME         120
#define IDM_MENU_MEM_FLAGS      121
#define IDM_MENU_SAVEINTO       122

#define IDM_MENU_NEWITEM        130
#define IDM_MENU_NEWPOPUP       131
#define IDM_MENU_NEWSEPARATOR   132
#define IDM_MENU_INSERTTOGGLE   133
#define IDM_MENU_INSERTAFTER    134
#define IDM_MENU_INSERTBEFORE   135
#define IDM_MENU_INSERTSUBITEMS 136

#define IDM_MENU_ABOUT          140

#endif
