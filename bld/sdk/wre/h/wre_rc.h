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


#ifndef WRE_RC_INCLUDED
#define WRE_RC_INCLUDED

#include "del.h"
#include "renam.h"
#include "res.h"
#include "paste.h"

/****************************************************************************/
/* Menu Identifiers                                                         */
/****************************************************************************/

/****************************************************************************/
/* File Menu                                                                */
/****************************************************************************/
#define IDM_NEW               200
#define IDM_OPEN              201
#define IDM_SAVE              202
#define IDM_SAVEAS            203
#define IDM_EXIT              204
#define WRE_FATAL_EXIT        205

/****************************************************************************/
/* Edit Menu                                                                */
/****************************************************************************/
#define IDM_CUT                 210
#define IDM_COPY                211
#define IDM_PASTE               212
#define IDM_DELETE              213
#define IDM_OPTIONS             214
#define IDM_LOAD_SYMBOLS        215
#define IDM_EDIT_SYMBOLS        216
#define IDM_SAVE_SYMBOLS        217

/****************************************************************************/
/* Resource Menu                                                            */
/****************************************************************************/
#define IDM_NEW_CURSOR        220
#define IDM_NEW_BITMAP        221
#define IDM_NEW_ICON          222
#define IDM_NEW_MENU          223
#define IDM_NEW_DIALOG        224
#define IDM_NEW_STRING        225
#define IDM_NEW_FONT          226
#define IDM_NEW_ACCELERATOR   227
#define IDM_NEW_RCDATA        228
#define IDM_RES_SAVEAS        230
#define IDM_RES_SAVE_INTO     231
#define IDM_RES_EDIT          232
#define IDM_RES_RENAME        233
#define IDM_RES_MEM_FLAGS     234

/****************************************************************************/
/* Window Menu                                                              */
/****************************************************************************/
#define IDM_MDI_CASCADE       250
#define IDM_MDI_TILEV         251
#define IDM_MDI_TILEH         252
#define IDM_MDI_ARRANGE       253
#define IDM_SHOW_RIBBON       254

/****************************************************************************/
/* Help Menu                                                                */
/****************************************************************************/
#define IDM_ABOUT             260
#define IDM_HELP              261
#define IDM_HELP_SEARCH       262
#define IDM_HELP_ON_HELP      263

/****************************************************************************/
/* Misc. Menu & control id's                                                */
/****************************************************************************/
#define IDM_NOINPUT        -1

/* NOTE: The following value defines the menu id that identifies the
 *       lowest MDI window menu identifier. Hints will be synthesized
 *       at and after this id
 */
#define WRE_MDI_FIRST          600

#endif
