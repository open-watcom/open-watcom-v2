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


#ifndef WDEFORDR_INCLUDED
#define WDEFORDR_INCLUDED

#include "fmedit.def"
#include "wderesin.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef enum {
    WdeSelect = 0,
    WdeSetOrder,
    WdeSetTabs,
    WdeSetGroups
} WdeOrderMode;

typedef struct {
    OBJPTR        obj;
    Bool          present;
    int           pos;
    WdeOrderMode  mode;
    Bool          pos_set;
    Bool          tab_set;
    Bool          grp_set;
    HWND          tag;
} WdeOrderedEntry;

typedef struct {
    LIST             *newlist;
    LIST             *oldlist;
} WdeSetOrderLists;

typedef struct {
    WdeOrderedEntry     *new_oe;
    WdeOrderedEntry     *old_oe;
    WdeSetOrderLists    *lists;
    WdeResInfo          *res_info;
} WdeSetOrderStruct;

extern char WdeTagClass[];

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern void WdeFreeOrderedList( LIST * );
extern LIST *WdeCopyOrderedList( LIST * );
extern Bool WdeAddOrderedEntry( LIST **, OBJPTR );
extern Bool WdeRemoveOrderedEntry( LIST *, OBJPTR );
extern Bool WdeCleanOrderedList( LIST ** );
extern Bool WdeGetNextChild( LIST **, OBJPTR *, Bool );
extern void WdeFiniOrderStuff( void );
extern Bool WdeRegisterTagClass( HINSTANCE );
extern void WdeTagPressed( WdeSetOrderStruct * );
extern void WdeDestroyTag( HWND tag );
extern HWND WdeCreateTag( HWND, WdeSetOrderStruct * );
extern void WdeReorderTags( WdeSetOrderLists *, Bool );
extern LIST *WdeFindOrderedEntry( LIST *, OBJPTR );
extern WdeSetOrderStruct    *WdeGetTagInfo( HWND );

#endif
