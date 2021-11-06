/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Heap walker internal prototypes.
*
****************************************************************************/


#include "commonui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "watcom.h"
#include "hwbiglb.h"
#include "segmem.h"
#include "mythelp.h"
#include "descript.h"
#include "cguimem.h"
#include "font.h"
#include "savelbox.h"
#include "memwnd.h"
#include "srchmsg.h"
#include "pushwin.h"
#include "ctl3dcvr.h"
#include "ldstr.h"

#include "heapwalk.rh"


#define INT_PTR                 int

#define RCSTR_MAX_LEN           128

#define SAVE_NAME_SIZE          _MAX_PATH
#define MAX_RES                 100

#define ITEM_DISPLAY_CLASS      "watitemdisplayclass"
#define LOCAL_DISPLAY_CLASS     "watlocaldisplayclass"
#define LOCAL_MONITOR_CLASS     "watlocalmonitorclass"

#define LOCAL_LB                1001
#define GLOBAL_LB               1002

typedef struct hl {
    union {
        GLOBALENTRY             ge;
        struct {
            WORD                sel;
            descriptor          desc;
        } mem;
    } info;
    WORD                        lru_pos;
    char                        szModule[MAX_MODULE_NAME + 1];
    char                        flag;
    char                        is_dpmi;
    char                        is_added;       /* used for the add operation */
} heap_list;

typedef struct lcinfo {
    WORD        free_count;
    WORD        fixed_count;
    WORD        movable_count;
    WORD        free_size;
    WORD        fixed_size;
    WORD        movable_size;
    WORD        tot_size;
    WORD        tot_count;
} LclInfo;

typedef struct lstinfo {
    HWND        box;
    HWND        *title;
    WORD        ypos;
} ListBoxInfo;

typedef struct gblwndinfo {
    HWND        add_dialog;
    HWND        alloc_dialog;
    DLGPROC     alloc_dlgproc;
    ListBoxInfo list;
    BOOL        doing_add:1;
    BOOL        need_refresh:1;
    BOOL        minimized:1;
} GblWndInfo;

typedef struct lclwndinfo {
    HWND        dialog;
    ListBoxInfo list;
} LclWndInfo;

typedef struct resinfo {
    WORD        type;
    HANDLE      hdl;
    char __far  *res;
    HWND        menu_const;     /* used for menu items only */
} ResInfo;

typedef struct heapconfiginfo {
    char        gfname[_MAX_PATH];
    char        lfname[_MAX_PATH];
    int         glob_xpos;
    int         glob_ypos;
    int         last_glob_xpos;
    int         last_glob_ypos;
    WORD        glob_xsize;
    WORD        glob_ysize;
    BOOL        disp_res;
    BOOL        save_glob_pos;
    BOOL        save_mem_pos;
} HeapConfigInfo;

/*
 * globals
 */
extern const char       *HeapWalkName;
extern HWND             HeapWalkMainWindow;
extern HINSTANCE        Instance;
extern BOOL             ListingDPMI;
extern WORD             HeapType;
extern HWND             ListBox,TitleLine;
extern int              HeapListSize;
extern heap_list        **HeapList;
extern bool             WDebug386;
extern BOOL             RefreshMainWindow;
extern const char       *HeapTitles;
extern const char       *HeapDPMITitles;
extern const char       *HeapLocalTitles;
extern int              GSortType;
extern int              LSortType;
extern HWND             ResHwnd[MAX_RES];
extern HeapConfigInfo   Config;


/*
 * function prototypes
 */
/* hwbox.c */
void CreateListBox( HWND parent, ListBoxInfo *info, WORD type  );
void ResizeListBox( WORD width, WORD height, ListBoxInfo *info );
void MoveListBox( WORD xpos, WORD ypos, ListBoxInfo *info );
void PaintAllWindows( void );
void InitPaintProc( void );
void FiniPaintProc( void );
void PositionListBox( ListBoxInfo *info, HWND parent );

/* hwlist.c */
void InitListStrTables( void );
void InitHeapList( HWND hwnd, BOOL keeppos );
void ReDisplayHeapList( HWND hwnd, struct glb_state * );
void FreeHeapList( void );
BOOL FormatHeapListItem( char *line, unsigned index );
DWORD GetDGroupIndex( char *owner );
BOOL GetDGroupItem( char *owner, heap_list *hl );
int SortByGlobType( heap_list **p1, heap_list **p2 );
char *GetGlobalTextItem( unsigned i );
char *GetLocalTextItem( unsigned i );

/* hwproc.c */
LRESULT __export FAR PASCAL HeapWalkProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
void EndAdd( void );

/* hwsort.c */
void SortHeapList( void );

/* hwdisp.c */
LRESULT __export FAR PASCAL ItemDisplayProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
void ShowHeapObject( HWND lbhdl );

/* hwsave.c */
void PutOutGlobalHeader( FILE *fptr );
char *DumpGlobalLine( bool listview, HWND list, int line );

/* hwobjec.c */
void ShowSelector( HWND list );
BOOL GlobDiscardObj( HWND list );
BOOL GlobSetObjPos( HWND list, BOOL oldest );
void RefreshAdd( HWND dialog, HWND lbhwnd );
WINEXPORT INT_PTR CALLBACK AllocDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
WINEXPORT INT_PTR CALLBACK SetCodeDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
void SetMenusForAdd( HWND hwnd, BOOL start );
HWND StartAdd( HWND parent, ListBoxInfo *info );

/* hwalloc.c */
void UpdateAllocInfo( HWND hwnd );
void AllocAllBut( WORD type );
void AllocMore( WORD type );
void FreeSomeMem( WORD type );
void MyFreeAllMem( void );
void DoNBytes( HWND parent, WORD type );

/* hwlocal.c */
LRESULT __export FAR PASCAL LocalHeapProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
BOOL FormatLocalHeapListItem( char *line, unsigned index );
int  SortByLocalType( LOCALENTRY **, LOCALENTRY ** );
void LocalWalk( heap_list *item );
void LclHeapInfo( LclInfo *info );
void ResetLocalFont( void );

/* hwlsort.c */
void SortLocalHeapList( HWND, WORD );

/* hwconfig.c */
void ReadConfig( void );
void SaveConfigFile( BOOL save_all_values );
void HWConfigure( void );

/* hwinfo.c */
void DisplayGlobHeapInfo( HWND parent );
void DisplayMemManInfo( HWND parent );
HWND DisplayLocalHeapInfo( HWND parent );

/* hwutil.c */
void SetStaticText( HWND hwnd, int id, char *str );
void CenterDlg( HWND hwnd );
int ErrorBox( HWND hwnd, msg_id msgid, UINT type );
HWND *MakePushWin( HWND hwnd, const char *str, WORD cnt, ... );
void KillPushWin( HWND *push );

/* hwmonit */
LRESULT __export FAR PASCAL LocalMonitorProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
void BeginMonitor( heap_list *item );

/* hwtable */
void InitializeStringTables( void );

/* hwldstr */
BOOL InitStringTable( void );
char *HWAllocRCString( msg_id id );
void HWFreeRCString( char *str );
const char *HWGetRCString( msg_id msgid );
