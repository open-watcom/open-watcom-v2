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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "heapwalk.h"

extern msglist Display_types[];
extern msglist DPMI_Sort_types[];
extern msglist Sort_types[];

/*
 * PutOutGlobalInfo
 */

static void PutOutGlobalInfo( FILE *fptr ) {

    GLOBALINFO  meminfo;

    memset( &meminfo, 0, sizeof( GLOBALINFO ) );
    meminfo.dwSize = sizeof( GLOBALINFO );
    if( GlobalInfo( &meminfo ) != 0 ) {
        RCfprintf( fptr, STR_GBL_HEAPINFO );
        RCfprintf( fptr, STR_GBL_INFO_UNDERLINE );
        RCfprintf( fptr, STR_GBL_TOT_ITEMS, meminfo.wcItems );
        RCfprintf( fptr, STR_GBL_LRU_ITEMS, meminfo.wcItemsLRU );
        RCfprintf( fptr, STR_GBL_FREE_ITEMS, meminfo.wcItemsFree );
    }
}

/*
 * PutOutMemManInfo
 */

static void PutOutMemManInfo( FILE *fptr ) {

    MEMMANINFO          info;

    memset( &info, 0, sizeof( MEMMANINFO ) );
    info.dwSize = sizeof( MEMMANINFO );
    if( MemManInfo( &info ) != 0 ) {
        RCfprintf( fptr, STR_MEM_MAN_INFO );
        RCfprintf( fptr, STR_MEM_MAN_UNDERLINE );
        RCfprintf( fptr, STR_SIZE_LIN_ADDR_SPACE, info.dwTotalLinearSpace );
        RCfprintf( fptr, STR_FREE_PAGES, info.dwFreeLinearSpace );
        RCfprintf( fptr, STR_SYS_PAGE_SIZE, info.wPageSize );
        RCfprintf( fptr, STR_PAGES_IN_SWAP_FILE, info.dwSwapFilePages );
        RCfprintf( fptr, STR_TOT_PAGES_IN_SYSTEM, info.dwTotalPages );
        RCfprintf( fptr, STR_NUM_UNLOCKED_PAGES, info.dwTotalUnlockedPages );
        RCfprintf( fptr, STR_NUM_FREE_PAGES, info.dwFreePages );
        RCfprintf( fptr, STR_LARGEST_FREE_MEM_BLOCK, info.dwLargestFreeBlock );
        RCfprintf( fptr, STR_MAX_ALLOCATABLE_PAGES, info.dwMaxPagesAvailable );
        RCfprintf( fptr, STR_MAX_LOCKABLE_PAGES, info.dwMaxPagesLockable );
    }
}

/*
 * PutOutGlobalHeader
 */

void PutOutGlobalHeader( FILE *fptr ) {

    time_t      tm;
    char        *sort;
    char        *disp;

    tm = time( NULL );
    RCfprintf( fptr, STR_SNAP_GBL_CREATED, asctime( localtime( &tm ) ) );
    if( HeapType != HEAPMENU_DISPLAY_DPMI ) {
        sort = SrchMsg( GSortType, Sort_types, "" );
        disp = SrchMsg( HeapType, Display_types, "" );
        RCfprintf( fptr, STR_GBL_SORTED_BY, disp, sort );
        PutOutGlobalInfo( fptr );
        PutOutMemManInfo( fptr );
        fprintf( fptr, "%s\n\n", HeapTitles );
    } else {
        sort = SrchMsg( GSortType, DPMI_Sort_types, "" );
        disp = SrchMsg( HeapType, Display_types, "" );
        RCfprintf( fptr, STR_GBL_SORTED_BY, disp, sort );
        fprintf( fptr, "%s\n\n", HeapDPMITitles );
    }
} /* PutOutGlobalHeader */
