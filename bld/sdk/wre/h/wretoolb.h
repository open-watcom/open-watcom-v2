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


#ifndef WRETOOLB_INCLUDED
#define WRETOOLB_INCLUDED

#include "toolbr.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef void *toolbar;

typedef struct {
    TOOLDISPLAYINFO dinfo;
    TOOLITEMINFO    *items;
    int             num_items;
} WREToolBarInfo;

typedef struct {
    toolbar         tbar;
    WREToolBarInfo  *info;
    HWND            parent;
    HWND            win;
    RECT            last_pos;
} WREToolBar;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern WREToolBar       *WRECreateToolBar( WREToolBarInfo *, HWND );
extern void             WREDestroyToolBar( WREToolBar * );
extern void             WREShutdownToolBars( void );
extern void             WREFreeToolBarInfo( WREToolBarInfo * );
extern WREToolBarInfo   *WREAllocToolBarInfo( int );
extern void             WRESetToolBarItemState( WREToolBar *, UINT, UINT );
extern bool             WRECloseToolBar( WREToolBar * );
extern void             WREFreeToolBar( WREToolBar * );

#endif
