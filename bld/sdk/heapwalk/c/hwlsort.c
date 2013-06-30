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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "heapwalk.h"

extern LOCALENTRY       **LocalHeapList;
extern unsigned         LocalHeapCount;


/*
 * SortByLocalType is in hwlist.c
 */

static int SortByAddr( LOCALENTRY **t1, LOCALENTRY **t2 ) {

    return( ( *t1 )->wAddress - ( *t2 )->wAddress );
}

static int SortBySize( LOCALENTRY **t1, LOCALENTRY **t2 ) {

    return( ( *t1 )->wSize - ( *t2 )->wSize );
}

static int SortByHandle( LOCALENTRY **t1, LOCALENTRY **t2 ) {

    return( (UINT)(( *t1 )->hHandle) - (UINT)(( *t2 )->hHandle ));
}

void SortLocalHeapList( HWND hwnd, WORD type ) {

    void        *fn;
    HMENU       mh;

    mh = GetMenu( hwnd );
    CheckMenuItem( mh, LSortType, MF_UNCHECKED | MF_BYCOMMAND );
    CheckMenuItem( mh, type, MF_CHECKED | MF_BYCOMMAND );
    LSortType = type;
    switch( type ) {
    case HEAPMENU_SORT_ADDR:
        fn = SortByAddr;
        break;
    case HEAPMENU_SORT_SIZE:
        fn = SortBySize;
        break;
    case HEAPMENU_SORT_TYPE:
        fn = SortByLocalType;
        break;
    case HEAPMENU_SORT_HANDLE:
        fn = SortByHandle;
        break;
    default:
        fn = SortByHandle;
        break;
    }
    qsort( LocalHeapList, LocalHeapCount, sizeof( LOCALENTRY * ), fn );
}
