/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "drwatcom.h"
#include "memwnd.h"
#include "mem.h"

typedef struct {
    HANDLE                      prochdl;
    DWORD                       procid;
    ProcStats                   *stats;
    HWND                        labels;
    DWORD                       label_hite;
    LBoxHdl                     *lbox;
    MemListData                 listdata;
} MemWalkerInfo;

static HWND                     curWalkHwnd;

#define MEM_ALLOC_INCR          200
#define MEM_LIMIT               0x80000000
#define MEM_WALKER_CLASS        "Dr_WATCOM_NT_mem_Walker"


void FormatMemListEntry( char *buf, MemListItem *item ) {

    char                        *prot;
    char                        *state;
    MEMORY_BASIC_INFORMATION    *mbi;

    mbi = &item->mbi;
    switch( mbi->State ) {
    case MEM_COMMIT:
        state = AllocRCString( STR_COMMITTED );
        break;
    case MEM_FREE:
        state = AllocRCString( STR_FREE );
        break;
    case MEM_RESERVE:
        state = AllocRCString( STR_RESERVED );
        break;
    default:
        state = AllocRCString( STR_UNKNOWN );
    }
    if( mbi->Protect & PAGE_READONLY ) {
        prot = "RO";
    } else if( mbi->Protect & PAGE_READWRITE ) {
        prot = "RW";
    } else if( mbi->Protect & PAGE_WRITECOPY ) {
        prot = "WC";
    } else if( mbi->Protect & PAGE_EXECUTE ) {
        prot = "Ex";
    } else if( mbi->Protect & PAGE_EXECUTE_READ ) {
        prot = "ExRO";
    } else if( mbi->Protect & PAGE_EXECUTE_READWRITE ) {
        prot = "ExRW";
    } else if( mbi->Protect & PAGE_EXECUTE_WRITECOPY ) {
        prot = "ExWC";
    } else if( mbi->Protect & PAGE_NOACCESS ) {
        prot = "NA";
    } else {
        if( mbi->State == MEM_RESERVE ) {
            prot = "NA";
        } else {
            prot = "??";
        }
    }
    sprintf( buf, "%08lX %08lX  %8lX %-4s %-10s %-9s %s", mbi->BaseAddress,
                mbi->AllocationBase, mbi->RegionSize, prot, state,
                item->objname, item->modname );
    FreeRCString( state );
}

/*
 * redrawMemList
 */
static void redrawMemList( LBoxHdl *hdl, MemListData *info ) {

    DWORD       i;
    char        buf[150];

    ClearListBox( hdl );
    for( i=0; i < info->used; i++ ) {
        FormatMemListEntry( buf, info->data[i] );
        LBStrPrintf( hdl, buf );
    }
    SendMessage( GetListBoxHwnd( hdl ), LB_SETTOPINDEX, 0, 0 );
}

/*
 * createMemListItem
 */
static MemListItem *createMemListItem( char *objname, char *modname,
                                        MEMORY_BASIC_INFORMATION *mbi )
{
    MemListItem         *new;
    DWORD               namelen;

    namelen = strlen( modname ) + 1;
    new = MemAlloc( sizeof( MemListItem ) + namelen + strlen( objname ) + 1 );
    strcpy( new->data, modname );
    strcpy( new->data + namelen, objname );
    new->mbi = *mbi;
    new->modname = new->data;
    new->objname = new->data + namelen;
    return( new );
}

/*
 * identifySingleStack - identify the memory pages that are the stack for
 *                       a give thread
 */
static void identifySingleStack( ProcNode *pnode, ThreadNode *tnode,
                                 MemListData *list ) {

    BOOL                        found_guard;
    MEMORY_BASIC_INFORMATION    mbi;
    DWORD                       i;
    DWORD                       last;
    MemListItem                 *old;
    char                        modname[20];
    char                        *objname;

    VirtualQueryEx( pnode->prochdl, (void *) tnode->stack, &mbi,
                    sizeof( mbi ) );
    if( mbi.AllocationBase != 0 ) {
        for( i=0; i < list->used; i++ ) {
            if( list->data[i]->mbi.AllocationBase == mbi.AllocationBase ) break;
        }
        if( i == list->used ) return;
        last = i + 1;
        /*
         * stack space is contiguous and all pages in a stack share the
         * same allocation base
         */
        while( list->data[last]->mbi.AllocationBase == mbi.AllocationBase ) {
            last++;
        }
        found_guard = FALSE;
        for( ; i < last; i++ ) {
            /*
             * identify the guard page
             * The guard page is a single page of committed memory at
             * the lower end of committed memory and immediately adjacent
             * to the stack's reserved memory. If there is no more reserved
             * stack space left, the guard page will be the last page of
             * committed memory.  A stack cannot exist without a guard page.
             * There will be at most three regions of memory in the stack:
             * the actual stack space is committed, the guard page is
             * committed and the remaining address space is marked reserved.
             */
            if( !found_guard && list->data[i]->mbi.State == MEM_COMMIT ) {
                objname = AllocRCString( STR_STACK_GUARD );
                found_guard = TRUE;
            } else {
                objname = AllocRCString( STR_STACK );
            }
            sprintf( modname, "tid: %08lX", tnode->threadid );
            old = list->data[i];
            list->data[i] = createMemListItem( objname, modname, &old->mbi );
            FreeRCString( objname );
            MemFree( old );
        }
    }
}

/*
 * identifyAllStacks
 */
static void identifyAllStacks( ProcNode *pnode, MemListData *list ) {

    ThreadNode          *thread;

    if( pnode == NULL ) {
        thread = NULL;
    } else {
        thread = pnode->thread;
        while( thread != NULL ) {
            identifySingleStack( pnode, thread, list );
            thread = thread->next;
        }
    }
}

/*
 * addMemListItem
 */
static void addMemListItem( ProcNode *pnode, MemListData *info,
                            MEMORY_BASIC_INFORMATION *mbi ) {

    ModuleNode                  *mnode;
    MemListItem                 *new;
    ObjectInfo                  *obj;
    DWORD                       i;
    DWORD                       rva;
    char                        *name;
    char                        *objname;
    MEMORY_BASIC_INFORMATION    topass;

    if( info->used >= info->allocated ) {
        info->allocated += MEM_ALLOC_INCR;
        info->data = MemReAlloc( info->data,
                                 info->allocated * sizeof( void * ) );
    }
    mnode = ModuleFromAddr( pnode, mbi->BaseAddress );
    if( mnode == NULL ) {
        name = "";
        objname = "";
    } else {
        if( mnode->name == NULL ) {
            name = "";
        } else {
            name = mnode->name;
        }
        obj = mnode->objects;
        rva = (DWORD)mbi->BaseAddress - mnode->base;
        for( i=0; i < mnode->num_objects; i++ ) {
            if( obj[i].rva > rva ) break;
        }
        i--;
        if( i == -1 ) {
            objname = "";
        } else {
            objname = obj[i].name;
        }
    }
    info->data[info->used] = createMemListItem( objname, name, mbi );
    info->used++;
    if( mnode != NULL && i < mnode->num_objects - 1 ) {
        i++;
        /*
         * if this region contains more than one object the split it
         */
        if( obj[i].rva < rva + mbi->RegionSize ) {
            new = info->data[info->used - 1];
            topass = *mbi;
            new->mbi.RegionSize = obj[i].rva - rva;
            topass.BaseAddress = (char *)new->mbi.BaseAddress
                                 + new->mbi.RegionSize;
            topass.RegionSize = mbi->RegionSize - new->mbi.RegionSize;
            addMemListItem( pnode, info, &topass );
        }
    }
}

/*
 * FreeMemListItems
 */
static void freeMemListItems( MemListData *info ) {

    DWORD       i;

    for( i=0; i < info->used; i++ ) {
        MemFree( info->data[i] );
    }
    info->used = 0;
}


void FreeMemList( MemListData *info ) {
    freeMemListItems( info );
    MemFree( info->data );
    info->allocated = 0;
    info->data = NULL;
}

void RefreshMemList( DWORD procid, HANDLE prochdl, MemListData *proclist ) {

    DWORD                       offset;
    MEMORY_BASIC_INFORMATION    mbi;
    ProcNode                    *pnode;

    offset = 0;
    pnode = FindProcess( procid );
    for( ;; ) {
        VirtualQueryEx( prochdl, (LPVOID)offset, &mbi,
                        sizeof( MEMORY_BASIC_INFORMATION ) );
        if( offset != (DWORD)mbi.BaseAddress ) break;
        addMemListItem( pnode, proclist, &mbi );
        offset = (DWORD)mbi.BaseAddress + mbi.RegionSize;
    }
    if( pnode != NULL ) {
        identifyAllStacks( pnode, proclist );
    }
}

/*
 * viewMem
 */
static void viewMem( MemWalkerInfo *info ) {

    int                         index;
    MEMORY_BASIC_INFORMATION    *mbi;
    char                        buf[100];
    BOOL                        ret;

    index = (int)SendMessage( GetListBoxHwnd( info->lbox ), LB_GETCURSEL, 0, 0 );
    mbi = &info->listdata.data[index]->mbi;
    if( mbi->State == MEM_FREE || mbi->State == MEM_RESERVE ) {
        RCMessageBox( GetListBoxHwnd( info->lbox ), STR_MEM_NOT_COMMITTED,
                      AppName, MB_OK | MB_ICONEXCLAMATION );
    } else {
        ret = ReadProcessMemory( info->prochdl, mbi->BaseAddress,
                                 buf, 1, NULL );
        if( ret ) {
            if( info->stats != NULL ) {
                RCsprintf( buf, STR_MEM_RANGE_NAME_PROC,
                    (DWORD)mbi->BaseAddress,
                    (DWORD)mbi->BaseAddress + mbi->RegionSize - 1,
                    info->procid, info->stats->name );
            } else {
                RCsprintf( buf, STR_MEM_RANGE_UNNAMED_PROC,
                    (DWORD)mbi->BaseAddress,
                    (DWORD)mbi->BaseAddress + mbi->RegionSize - 1,
                    info->procid );
            }
            DispNTMem( GetListBoxHwnd( info->lbox ), Instance,
                info->prochdl, (DWORD)mbi->BaseAddress,
                (DWORD)mbi->BaseAddress + mbi->RegionSize, buf );
        } else {
            RCsprintf( buf, STR_CANT_READ_MEM_AT_X, (DWORD)mbi->BaseAddress );
            MessageBox( GetListBoxHwnd( info->lbox ), buf, AppName,
                        MB_OK | MB_ICONEXCLAMATION );
        }
    }
}

LONG CALLBACK MemWalkerProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    MemWalkerInfo       *info;
    WORD                cmd;
    SIZE                txtsize;
    HDC                 dc;
    char                buf[150];

    info = (MemWalkerInfo *)GetWindowLong( hwnd, 0 );
    switch ( msg ) {
    case WM_CREATE:
        info = (MemWalkerInfo *)( (CREATESTRUCT *)lparam )->lpCreateParams;
        SetWindowLong( hwnd, 0, (DWORD)info );
        info->listdata.data = NULL;
        info->listdata.allocated = 0;
        info->listdata.used = 0;
        info->labels = CreateWindow(
            "static",    /* Window class name */
            "",    /* Window caption */
            WS_CHILD,/* Window style */
            CW_USEDEFAULT,          /* Initial X position */
            0,                      /* Initial Y position */
            0,                      /* Initial X size */
            0,                      /* Initial Y size */
            hwnd,                   /* Parent window handle */
            NULL,                   /* Window menu handle */
            Instance,               /* Program instance handle */
            NULL );                 /* Create parameters */
        ShowWindow( info->labels, SW_SHOWNORMAL );
        SetMonoFont( info->labels );
        dc = GetDC( info->labels );
        GetTextExtentPoint( dc, MEM_WALKER_HEADER, 1, &txtsize );
        ReleaseDC( info->labels, dc );
        info->label_hite = txtsize.cy;
        SetWindowText( info->labels, MEM_WALKER_HEADER );
        info->lbox = CreateListBox( hwnd );
        RefreshMemList( info->procid, info->prochdl, &info->listdata );
        redrawMemList( info->lbox, &info->listdata );
        SendMessage( GetListBoxHwnd( info->lbox ), LB_SETCURSEL, 0, 0 );
        SetFocus( GetListBoxHwnd( info->lbox ) );
        info->stats = MemAlloc( sizeof( ProcStats ) );
        if( GetProcessInfo( info->procid, info->stats ) ) {
            RCsprintf( buf, STR_MEM_WLK_NAMED_PROC, info->procid,
                        info->stats->name );
        } else {
            RCsprintf( buf, STR_MEM_WLK_UNNAMED_PROC, info->procid,
                        info->stats->name );
            MemFree( info->stats );
            info->stats = NULL;
        }
        SetWindowText( hwnd, buf );
        break;
    case WM_SIZE:
        MoveListBox( info->lbox, 0, info->label_hite, LOWORD( lparam ),
                     HIWORD( lparam ) - info->label_hite );
        MoveWindow( info->labels, 0, 0, LOWORD( lparam ), info->label_hite,
                    TRUE );
        break;
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
        case LISTBOX_1:
            if( HIWORD( wparam ) == LBN_DBLCLK ) {
                viewMem( info );
            }
            break;
        }
        break;
    case WM_DESTROY:
        CloseHandle( info->prochdl );
        FreeMemList( &info->listdata );
        if( info->stats != NULL ) {
            MemFree( info->stats );
        }
        MemFree( info );
        curWalkHwnd = NULL;
        PostMessage( GetWindow( hwnd, GW_OWNER ), STAT_FOREGROUND, 0, 0 );
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0L );
}

BOOL RegisterMemWalker( void ) {

    WNDCLASS            wc;

    wc.style = 0L;
    wc.lpfnWndProc = MemWalkerProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 4;
    wc.hInstance = Instance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = MEM_WALKER_CLASS;
    return( RegisterClass( &wc ) );
}

void WalkMemory( HWND parent, HANDLE hdl, DWORD procid ) {

    MemWalkerInfo       *info;
    int                 resp;
    char                *title;

    if( curWalkHwnd != NULL ) {
        resp = RCMessageBox( parent, STR_MEM_WLK_IN_PROGRESS,
                             AppName, MB_YESNO | MB_ICONQUESTION );
        if( resp == IDYES ) {
            DestroyWindow( curWalkHwnd );
        } else {
            return;
        }
    }
    info = MemAlloc( sizeof( MemWalkerInfo ) );
    info->prochdl = hdl;
    info->procid = procid;
    title = AllocRCString( STR_MEMORY_WALKER_TITLE );
    curWalkHwnd = CreateWindow(
        MEM_WALKER_CLASS,               /* Window class name */
        title,                          /* Window caption */
        WS_OVERLAPPED | WS_CAPTION
        | WS_SYSMENU | WS_THICKFRAME
        | WS_MAXIMIZEBOX,               /* Window style */
        CW_USEDEFAULT,                  /* Initial X position */
        0,                              /* Initial Y position */
        CW_USEDEFAULT,                  /* Initial X size */
        0,                              /* Initial Y size */
        parent,                         /* Parent window handle */
        NULL,                           /* Window menu handle */
        Instance,                       /* Program instance handle */
        info );                         /* Create parameters */
    FreeRCString( title );
    ShowWindow( curWalkHwnd, SW_SHOWNORMAL );
    UpdateWindow( curWalkHwnd );
}

