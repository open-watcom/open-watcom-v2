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


#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dos.h>
#include "heapwalk.h"

typedef struct glb_state{
    heap_list           sel;
    heap_list           top;
    BOOL                sel_valid;
}GlobStateStruct;

static char             *gbl_types[ 11 ];
static char             *res_types[ 15 ];
static char             Buffer[100];

void InitListStrTables( void ) {
    gbl_types[ 0 ] = HWGetRCString( STR_MC_UNKNOWN );
    gbl_types[ 1 ] = HWGetRCString( STR_DGROUP );
    gbl_types[ 2 ] = HWGetRCString( STR_DATA );
    gbl_types[ 3 ] = HWGetRCString( STR_CODE );
    gbl_types[ 4 ] = HWGetRCString( STR_TASK );
    gbl_types[ 5 ] = HWGetRCString( STR_RESOURCE );
    gbl_types[ 6 ] = HWGetRCString( STR_MC_MODULE );
    gbl_types[ 7 ] = HWGetRCString( STR_MC_FREE );
    gbl_types[ 8 ] = HWGetRCString( STR_INTERNAL );
    gbl_types[ 9 ] = HWGetRCString( STR_SENTINEL );
    gbl_types[ 10 ] = HWGetRCString( STR_BURGERMASTER );
    res_types[ 0 ] = HWGetRCString( STR_USERDEFINED );
    res_types[ 1 ] = HWGetRCString( STR_CURSOR_COMP );
    res_types[ 2 ] = HWGetRCString( STR_MC_BITMAP );
    res_types[ 3 ] = HWGetRCString( STR_ICON_COMP );
    res_types[ 4 ] = HWGetRCString( STR_MC_MENU );
    res_types[ 5 ] = HWGetRCString( STR_DIALOG );
    res_types[ 6 ] = HWGetRCString( STR_STRING );
    res_types[ 7 ] = HWGetRCString( STR_FONTDIR );
    res_types[ 8 ] = HWGetRCString( STR_MC_FONT );
    res_types[ 9 ] = HWGetRCString( STR_ACCELERATORS );
    res_types[ 10 ] = HWGetRCString( STR_RCDATA );
    res_types[ 11 ] = HWGetRCString( STR_ERRTABLE );
    res_types[ 12 ] = HWGetRCString( STR_MC_CURSOR );
    res_types[ 13 ] = HWGetRCString( STR_MC_ICON );
    res_types[ 14 ] = HWGetRCString( STR_NAMETABLE );
}

/*
 * CALL BACK functions that returned formated text lines for the list box
 */

char *GetGlobalTextItem( unsigned i ) {

    FormatHeapListItem( Buffer, i );
    return( Buffer );
}

char *GetLocalTextItem( unsigned i ) {

    FormatLocalHeapListItem( Buffer, i );
    return( Buffer );
}

/*
 * SortByGlobType
 */

int SortByGlobType( heap_list **p1, heap_list **p2 )
{
    WORD        t1;
    WORD        t2;
    WORD        tmp;
    int         ret;

    if( !ListingDPMI ) {
        t1 = (*p1)->info.ge.wType;
        t2 = (*p2)->info.ge.wType;
        ret = strcmp( gbl_types[t1], gbl_types[t2] );
        if( ret == 0 ) {
            tmp = t1;
            t1 = (*p1)->info.ge.wData;
            t2 = (*p2)->info.ge.wData;
            if( tmp == GT_CODE ) {
                /* compare the module names */
                ret = strcmp( (*p1)->szModule, (*p2)->szModule );
                if( ret == 0 ) {
                    ret = t1 - t2;
                }
            } else if( tmp == GT_RESOURCE ) {
                if( t1 >= (sizeof( res_types ) /sizeof( char *) ) ) {
                    ret = strcmp( HWGetRCString( STR_LOWER_UNKNOWN ),
                                  res_types[t2] );
                } else if( t2 >= (sizeof( res_types ) /sizeof( char *) ) ) {
                    ret = strcmp(  res_types[t1],
                                   HWGetRCString( STR_LOWER_UNKNOWN ) );
                } else ret = strcmp( res_types[t1], res_types[t2] );
            }
        }
        return( ret );
    } else {
        t1 = (*p1)->info.mem.desc.type;
        t2 = (*p2)->info.mem.desc.type;
        if( t1 == t2 ) return( 0 );
        if( t1 == 2 ) return( 1 );
        return( -1 );
    }
} /* SortByType */

/*
 * GetDGroupItem - fills hl with information about the DGROUP owned by
 *                 owner
 */

BOOL GetDGroupItem( char *owner, heap_list *hl ) {

    MODULEENTRY me;

    MyGlobalFirst( &( hl->info.ge ), GLOBAL_ALL );
    do {
        if( hl->info.ge.wType == GT_DGROUP ) {
            if( hl->info.ge.hOwner != NULL ) {
                if( MyModuleFindHandle( &me, hl->info.ge.hOwner ) ) {
                    if( !strcmp( me.szModule, owner ) ){
                        strcpy( hl->szModule , owner );
                        hl->szModule[MAX_MODULE_NAME]=0;
                        return( TRUE );
                    }
                }
            }
        }
    } while( MyGlobalNext( &hl->info.ge, GLOBAL_ALL ) );
    return( FALSE );
}

/*
 * AddToBuff - add a string to the current formatted selector
 */
static char *_buff;
void AddToBuff( char *str, ... )
{
    va_list     al;
    char        tmp[256];

    va_start( al, str );
    vsprintf( tmp, str, al );
    va_end( al );
    strcat( _buff, tmp );

} /* AddToBuff */

/*
 * FormatSel - format selector info
 */
static void FormatSel( char *which, WORD sel, char *buff )
{
    descriptor  desc;
    DWORD       base;
    DWORD       limit;

    buff[ 0 ] = 0;
    _buff = buff;
    GetADescriptor( sel, &desc );
    if( which != NULL ) {
        AddToBuff( "      %s = %04x  ", which, sel );
        if( sel == 0L ) {
            AddToBuff( "********  ********  ****  ****  *   ***    *\n" );
            return;
        }
    } else {
        AddToBuff( " %04x  ", sel );
    }

    base = GET_DESC_BASE( desc );
    limit = GET_DESC_LIMIT( desc );
    AddToBuff( "%08lx  %08lx  ", base, limit );
    if( desc.granularity ) {
        AddToBuff( "page  " );
    } else {
        AddToBuff( "byte  " );
    }
    if( desc.type == 2 ) {
        AddToBuff( "data  " );
    } else {
        AddToBuff( "code  " );
    }
    AddToBuff( "%1d   ", (WORD) desc.dpl );
    if( desc.type == 2 )  {
        AddToBuff( "R" );
        if( desc.writeable_or_readable ) AddToBuff( "/W" );
        else AddToBuff( "  " );
        AddToBuff( "    " );
    } else {
        AddToBuff( "Ex" );
        if( desc.writeable_or_readable ) AddToBuff( "/R" );
        else AddToBuff( "  " );
        AddToBuff( "   " );
    }
    if( desc.big_or_default ) {
        AddToBuff( "Y" );
    } else {
        AddToBuff( " " );
    }

} /* FormatSel */

/*
 * FormatHeapListItem - format a single line of the heaplist
 */

BOOL FormatHeapListItem( char *line, unsigned index )
{
    char        type[30];
    char        handle[5];
    char        lock[15];
    heap_list   *hl;

    if( index >= HeapListSize ) return( FALSE );
    hl = HeapList[index];
    if( hl->is_dpmi ) {
        FormatSel( NULL, hl->info.mem.sel, line );
    } else {
        if( hl->info.ge.hBlock != 0 ) {
            sprintf( handle,"%04x", hl->info.ge.hBlock );
        } else {
            handle[0] = 0;
        }
        lock[0] = 0;
        if( hl->info.ge.wcLock ) {
            strcat( lock, "L1" );
            if( hl->info.ge.wcPageLock ) strcat( lock,"," );
        }
        if( hl->info.ge.wcPageLock ) {
            strcat( lock, "P1" );
        }
        if( hl->info.ge.wType == GT_CODE ) {
            sprintf( type,"%s(%d)", gbl_types[GT_CODE], hl->info.ge.wData );
        } else if( hl->info.ge.wType == GT_RESOURCE ) {
            sprintf( type,"%s(%s)", gbl_types[GT_RESOURCE],
                   (hl->info.ge.wData < (sizeof( res_types ) /sizeof( char *))) ?
                    res_types[ hl->info.ge.wData ]: HWGetRCString( STR_LOWER_UNKNOWN ) );
        } else {
            strcpy( type,gbl_types[hl->info.ge.wType] );
        }
        sprintf( line,"%s%4s  %08lx %8ld  %-8s   %c   %-5s  %c   %-21s",
                hl->is_added ? "*" : " ",
                handle,
                hl->info.ge.dwAddress,
                hl->info.ge.dwBlockSize,
                hl->szModule,
                hl->flag,
                lock,
                (hl->info.ge.wHeapPresent ? 'Y':' '),
                type
                );
    }
    return( TRUE );
} /* FormatHeapListItem */

/*
 * SaveGlobalListState - save the top item and selected item in the
 *              list box so we can restore them later
 */

static BOOL SaveGlobalListState( HWND boxhwnd, GlobStateStruct *state ) {

    int     top, sel;

    if( HeapList == NULL ) return( FALSE );
    top = (int)SendMessage( boxhwnd, LB_GETTOPINDEX, 0, 0L );
    sel = (int)SendMessage( boxhwnd, LB_GETCURSEL, 0, 0L );
    if( sel != LB_ERR && sel < HeapListSize ) {
        state->sel = *HeapList[sel];
        state->sel_valid = TRUE;
    } else {
        state->sel_valid = FALSE;
    }
    if( top >= HeapListSize ) top = 0;
    state->top = *HeapList[top];
    return( TRUE );
}

/*
 * ReDisplayHeapList - dump heap list into list box
 */
void ReDisplayHeapList( HWND boxhwnd, GlobStateStruct *state ) {

    WORD        topselector;
    WORD        curselector;
    unsigned    i;

    if( state == NULL ) {
        SendMessage( boxhwnd, LB_SETTOPINDEX, 0, 0 );
        SendMessage( boxhwnd, LB_SETCURSEL, -1, 0 );
    }
    SetBoxCnt( boxhwnd, HeapListSize );
    if( state != NULL ) {
        if( state->top.is_dpmi ) {
            curselector = state->sel.info.mem.sel;
            topselector = state->top.info.mem.sel;
            for( i=0; i < HeapListSize; i++ ) {
                if( HeapList[i]->info.mem.sel == topselector  ) {
                    SendMessage( boxhwnd, LB_SETTOPINDEX, i, 0 );
                }
                if( state->sel_valid
                && HeapList[i]->info.mem.sel == curselector ) {
                    SendMessage( boxhwnd, LB_SETCURSEL, i, 0 );
                }
            }
        } else {
            curselector = (WORD)state->sel.info.ge.hBlock;
            topselector = (WORD)state->top.info.ge.hBlock;
            for( i=0; i < HeapListSize; i++ ) {
                if( (WORD)HeapList[i]->info.ge.hBlock == topselector
                && HeapList[i]->info.ge.dwAddress
                == state->top.info.ge.dwAddress ) {
                    SendMessage( boxhwnd, LB_SETTOPINDEX, i, 0 );
                }

                if( state->sel_valid
                && (WORD)HeapList[i]->info.ge.hBlock == curselector
                && HeapList[i]->info.ge.dwAddress
                == state->sel.info.ge.dwAddress ) {
                    SendMessage( boxhwnd, LB_SETCURSEL, i, 0 );
                }
            }
        }
    }
}

/*
 * FreeHeapList - release old heap list
 */
void FreeHeapList( void )
{
    int i;

    if( HeapListSize != 0 ) {
        for( i=0;i<HeapListSize;i++ ) {
            MemFree( HeapList[i] );
        }
        MemFree( HeapList );
        HeapList = NULL;
        HeapListSize = 0;
    }

} /* FreeHeapList */

/*
 * AddToHeapList - add a new item to the heap list
 */
static BOOL AddToHeapList( heap_list *hl )
{
    void        *ptr;
    ptr = MemReAlloc( HeapList,
                    sizeof( heap_list * ) * (HeapListSize + 1) );
    if( ptr == NULL ) {
        return( FALSE );
    } else {
        HeapList = ptr;
    }
    HeapList[ HeapListSize ] = MemAlloc( sizeof( heap_list ) );
    if( HeapList[ HeapListSize ] == NULL ) return( FALSE );
    *HeapList[ HeapListSize ] = *hl;
    HeapListSize++;
    return( TRUE );
} /* AddToHeapList */


/*
 * AddAllSelectors - add all DPMI selectors starting at given one
 */
static BOOL AddAllSelectors( WORD sel )
{
    heap_list   hl;
    BOOL        ret;

    hl.is_dpmi = TRUE;
    hl.is_added = FALSE;
    while( 1 ) {
        if( IsValidSelector( sel ) ) {
            hl.info.mem.sel = sel;
            GetADescriptor( sel, &hl.info.mem.desc );
            ret = AddToHeapList( &hl );
            if( !ret ) return( FALSE );
        }
        if( sel + 0x08 < sel ) break;
        sel += 0x08;
    }
    return( TRUE );
} /* AddAllSelectors */

static char GetMemFlag( heap_list *hl ) {

    if( (WORD)hl->info.ge.hBlock % 2 ) return( 'F' );
    if( hl->info.ge.wType == GT_CODE || hl->info.ge.wType == GT_RESOURCE ) {
        if( hl->info.ge.wcLock == 0 && hl->info.ge.wcPageLock == 0 ) {
            return( 'D' );
        }
    }
    return( ' ' );
} /* GetMemFlag */

/*
 * FindModuleName - find the name of the owner of a piece of memory.
 *                  modhdl may be a task id or a module id so we try both
 */
void FindModuleName( char *buf, HANDLE modhdl ) {

    MODULEENTRY         me;
    TASKENTRY           te;

    if( MyModuleFindHandle( &me, modhdl ) ) {
        strcpy( buf, me.szModule );
        buf[MAX_MODULE_NAME]=0;
    } else if( MyTaskFindHandle( &te, modhdl ) ) {
        strcpy( buf, te.szModule );
        buf[MAX_MODULE_NAME]=0;
    } else {
        buf[0] = '\0';
    }
}


/*
 * InitHeapList - build a heap list
 */
void InitHeapList( HWND boxhwnd, BOOL keeppos )
{
    GlobStateStruct     state;
    int                 htype;
    HCURSOR             hourglass;
    HCURSOR             oldcursor;
    WORD                pos;
    BOOL                ret;

    hourglass = LoadCursor( NULL, IDC_WAIT );
    SetCapture( boxhwnd );
    oldcursor= SetCursor( hourglass );
    if( keeppos ) {
        keeppos = SaveGlobalListState( boxhwnd, &state );
    }
    FreeHeapList();
    switch( HeapType ) {
    case HEAPMENU_DISPLAY_ENTIRE:
        htype = GLOBAL_ALL;
        break;
    case HEAPMENU_DISPLAY_LRU:
        htype = GLOBAL_LRU;
        break;
    case HEAPMENU_DISPLAY_FREE:
        htype = GLOBAL_FREE;
        break;
    }

    if( HeapType != HEAPMENU_DISPLAY_DPMI ) {
        heap_list       hl;

        ListingDPMI = FALSE;
        hl.is_dpmi = FALSE;
        hl.is_added = FALSE;
        pos = 0;
        MyGlobalFirst( &hl.info.ge, htype );
        do {
            hl.szModule[0] = 0;
            hl.lru_pos = pos;
            if( hl.info.ge.hOwner != NULL ) {
                FindModuleName( hl.szModule, hl.info.ge.hOwner );
            }
            hl.flag = GetMemFlag( &hl );
            ret = AddToHeapList( &hl );
            if( !ret ) break;
            pos ++;
        } while( MyGlobalNext( &hl.info.ge, htype ) );
    } else {
        ListingDPMI = TRUE;
        ret = AddAllSelectors( 7 );
        if( ret ) {
            ret = AddAllSelectors( 0 );
        }
    }
    if( !ret ) {
        ErrorBox( HeapWalkMainWindow, STR_CANT_CONSTRUCT_GBL_LIST,
                    MB_OK | MB_ICONINFORMATION );
        SendMessage( boxhwnd, LB_RESETCONTENT, 0, 0L );
    } else {
        SortHeapList();
        ReDisplayHeapList( boxhwnd, keeppos ? &state:NULL );
    }
    SetCursor( oldcursor );
    ReleaseCapture();
} /* InitHeapList */
