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


#ifndef WRLIST_INCLUDED
#define WRLIST_INCLUDED

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct list {
    HWND        hdlg;
    struct list *next;
    struct list *prev;
} LIST;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern void WRInsertObject( LIST **, HWND );
extern void WRRemoveObject( LIST **, HWND );
extern LIST *WRListCopy( LIST * );
extern void ListLastElt( LIST *, LIST ** );
extern HWND ListElement( LIST * );
extern void ListFree( LIST * );
extern LIST *ListNext( LIST * );
extern LIST *ListPrev( LIST * );
extern void ListAddElt( LIST **, HWND );
extern LIST *ListConsume( LIST * );
extern LIST *ListFindElt( LIST *, HWND );
extern LIST *ListCopy( LIST * );
extern int  ListCount( LIST * );
extern void ListInsertElt( LIST *, HWND );
extern void ListMerge( LIST **, LIST * );

#endif
