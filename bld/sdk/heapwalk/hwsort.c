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
#include "jdlg.h"

/*
 * SortByGlobType - is in hwlist.c
 */

static int SortByModule( heap_list **p1, heap_list **p2 )
{
    int rc;
    if( !ListingDPMI ) {
        rc = strcmp( (*p1)->szModule,(*p2)->szModule );
        if( !rc ) {
            return( SortByGlobType( p1, p2 ) );
        }
        return( rc );
    }
    return( 0 );

} /* SortByModule */

static int SortByAddr( heap_list **p1, heap_list **p2 )
{
    DWORD       b1,b2;

    if( !ListingDPMI ) {
        b1 = (*p1)->info.ge.dwAddress;
        b2 = (*p2)->info.ge.dwAddress;
    } else {
        descriptor      *d1,*d2;

        d1 = &((*p1)->info.mem.desc);
        d2 = &((*p2)->info.mem.desc);

        b1 = GET_DESC_BASE( *d1 );
        b2 = GET_DESC_BASE( *d2 );
    }
    if( b1 < b2 ) return( -1 );
    if( b1 > b2 ) return( 1 );
    return( 0 );

} /* SortByAddr */

static int SortBySize( heap_list **p1, heap_list **p2 )
{
    DWORD       s1,s2;

    if( !ListingDPMI ) {
        s1 = (*p1)->info.ge.dwBlockSize;
        s2 = (*p2)->info.ge.dwBlockSize;
    } else {
        descriptor      *d1,*d2;

        d1 = &((*p1)->info.mem.desc);
        d2 = &((*p2)->info.mem.desc);

        s1 = GET_DESC_LIMIT( *d1 );
        s2 = GET_DESC_LIMIT( *d2 );
    }
    if( s1 < s2 ) return( -1 );
    if( s1 > s2 ) return( 1 );
    return( 0 );

} /* SortBySize */

static int SortByHandle( heap_list **p1, heap_list **p2 )
{
    WORD        h1,h2;
    if( !ListingDPMI ) {
        h1 = (*p1)->info.ge.hBlock;
        h2 = (*p2)->info.ge.hBlock;
    } else {
        h1 = (*p1)->info.mem.sel;
        h2 = (*p2)->info.mem.sel;
    }
    if( h1 < h2 ) return( -1 );
    if( h1 > h2 ) return( 1 );
    return( 0 );

} /* SortByHandle */


static int SortByGran( heap_list **p1, heap_list **p2 )
{
    return( (*p1)->info.mem.desc.granularity - (*p2)->info.mem.desc.granularity );
} /* SortByGran */

static int SortByDPL( heap_list **p1, heap_list **p2 )
{
    return( (*p1)->info.mem.desc.dpl - (*p2)->info.mem.desc.dpl );
} /* SortByDPL */

static int SortByFlag( heap_list **p1, heap_list **p2 )
{
    return( (*p1)->flag - (*p2)->flag );
} /* SortByFlag */

static int SortByLRU( heap_list **p1, heap_list **p2 )
{
    return( (*p1)->lru_pos - (*p2)->lru_pos );
} /* SortByLRU */

#if(0)
BOOL __export FAR PASCAL ExtendedSortProc( HWND hwnd, WORD msg, WORD wparam,
                                    DWORD lparam )
{
    char        buf[50];
    char        test[50];
    WORD        i;

    switch( msg ) {
    case WM_INITDIALOG:
        break;
    case WM_SYSCOLORCHANGE:
        Ctl3dColorChange();
        break;
    case WM_COMMAND:
        break;
    case BN_CLICKED:
        switch( wparam ) {
        case SORT_BT_1:
        case SORT_BT_2:
        case SORT_BT_3:
        case SORT_BT_4:
        case SORT_BT_5:
        case SORT_BT_6:
            GetDlgItemText( hwnd, wparam, buf, 50 );
            i = SORT_FLD_1;
            GetDlgItemText( hwnd, i, test, 50 );
            while( test[0] != '\0' ) {
                i++;
                GetDlgItemText( hwnd, i, test, 50 );
                if( i == SORT_FLD_LAST ) break;
            }
            SetDlgItemText( hwnd, i, buf );
            break;
        case SORT_OK:
        case SORT_CANCEL:
            EndDialog( hwnd, 0 );
            break;
        default:
            return( 0 );
        }
    case WM_CLOSE:
        EndDialog( hwnd, 0 );
        break;
    default:
        return( 0 );
    }
    return( 1 );
}


static ExtendedSort() {

    FARPROC     dialproc;

    dialproc = MakeProcInstance( ExtendedSortProc, Instance );
    JDialogBox( Instance, "EXTEND_SORT", HeapWalkMainWindow, dialproc );
    FreeProcInstance( dialproc );
} /* ExtendedSort */
#endif

/*
 * SortHeapList - sort the heap list
 */

void SortHeapList( void )
{
    void *fn;


    switch( GSortType ) {
    case HEAPMENU_SORT_ADDR: fn = SortByAddr; break;
    case HEAPMENU_SORT_HANDLE: fn = SortByHandle; break;
    case HEAPMENU_SORT_MODULE: fn = SortByModule; break;
    case HEAPMENU_SORT_SIZE: fn = SortBySize; break;
    case HEAPMENU_SORT_TYPE: fn = SortByGlobType; break;
    case HEAPMENU_SORT_GRAN: fn = SortByGran; break;
    case HEAPMENU_SORT_DPL: fn = SortByDPL; break;
    case HEAPMENU_SORT_FLAG: fn = SortByFlag; break;
    case HEAPMENU_SORT_LRU: fn = SortByLRU; break;
    default:
        fn=SortByHandle;
        GSortType = HEAPMENU_SORT_HANDLE;
        break;
    }
    qsort( HeapList, HeapListSize, sizeof(heap_list *), fn );

} /* SortHeapList */
