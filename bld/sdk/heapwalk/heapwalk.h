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


#define _WINDOWS_16_
#include <windows.h>
#include <toolhelp.h>
#include <stdio.h>
#include "heapwlk.h"
#include "hwbiglb.h"
#include "..\misc\segmem.h"
#include "..\misc\mythelp.h"
#include "..\misc\descript.h"
#include "..\misc\mem.h"
#include "..\misc\font.h"
#include "..\misc\seginfo.h"
#include "..\misc\savelbox.h"
#include "..\misc\memwnd.h"
#include "..\misc\srchmsg.h"
#include "..\misc\pushwin.h"
#include "..\misc\ctl3d.h"
#include "memman.h"
#include "heapinfo.h"
#include "lclinfo.h"
#include "rcstr.h"
#include "ldstr.h"
#include "config.h"
#include "menu.h"
#include "add.h"
#include "alloc.h"
#include "freen.h"
#include "code.h"

#define RCSTR_MAX_LEN                   128

#define HEAPMENU_DISPLAY_ENTIRE         300
#define HEAPMENU_DISPLAY_LRU            301
#define HEAPMENU_DISPLAY_FREE           302
#define HEAPMENU_DISPLAY_DPMI           303
#define HEAPMENU_DISPLAY_INIT           304

#define HEAPMENU_SORT_ADDR              310
#define HEAPMENU_SORT_HANDLE            311
#define HEAPMENU_SORT_MODULE            312
#define HEAPMENU_SORT_SIZE              313
#define HEAPMENU_SORT_TYPE              314
#define HEAPMENU_SORT_GRAN              315
#define HEAPMENU_SORT_DPL               316
#define HEAPMENU_SORT_EXTEND            317
#define HEAPMENU_SORT_FLAG              318
#define HEAPMENU_SORT_LRU               319

#define HEAPMENU_ABOUT                  320
#define HEAPMENU_HELP_CONTENTS          321
#define HEAPMENU_HELP_SRCH              322
#define HEAPMENU_HELP_ON_HELP           323

#define HEAPMENU_OBJECT_SHOW            330
#define HEAPMENU_OBJECT_DISCARD         331
#define HEAPMENU_OBJECT_OLDEST          332
#define HEAPMENU_OBJECT_NEWEST          333
#define HEAPMENU_ADD                    334
#define HEAPMENU_OBJECT_GET_SELECTOR    335

#define HEAPMENU_LOCAL_LOCALWALK        340
#define HEAPMENU_COMPACT_AND_LOCALWALK  341
#define HEAPMENU_GDI_LOCALWALK          342
#define HEAPMENU_USER_LOCALWALK         343
#define HEAPMENU_LOCAL_MONITOR          344

#define HEAPMENU_GLOBAL_COMPACT         350
#define HEAPMENU_GLOBAL_COMP_DISC       351
#define HEAPMENU_GLOBAL_MEMORYINFO      352
#define HEAPMENU_GLOBAL_HEAPINFO        353
#define HEAPMENU_GLOBAL_CODE_SIZE       354
#define HEAPMENU_GLOBAL_REFRESH         355

#define HEAPMENU_EXIT                   360
#define HEAPMENU_FILE_SAVE              361
#define HEAPMENU_FILE_SAVE_TO           362
#define HEAPMENU_CONFIGURE              363
#define HEAPMENU_SAVE_CONFIG            364
#define HEAPMENU_FONT                   365

#define HW_INFO_REFRESH                 5000
/*
 * Notice the grouping of the alloc menu constants within
 * HEAPMENU_*_FIRST and HEAPMENU_*_LAST.  Please respect it !!!
 */
#define HEAPMENU_ALLOCMENU_FIRST        HEAPMENU_ALLOC_ALL
#define HEAPMENU_ALLOC_ALL              370
#define HEAPMENU_FREE_ALL               371

#define HEAPMENU_FREE_FIRST             HEAPMENU_FREE_1K
#define HEAPMENU_FREE_1K                381
#define HEAPMENU_FREE_2K                382
#define HEAPMENU_FREE_5K                383
#define HEAPMENU_FREE_10K               384
#define HEAPMENU_FREE_25K               385
#define HEAPMENU_FREE_50K               386
#define HEAPMENU_FREE_NK                387
#define HEAPMENU_FREE_LAST              HEAPMENU_FREE_NK

#define HEAPMENU_ALLOC_FIRST            HEAPMENU_ALLOC_1K
#define HEAPMENU_ALLOC_1K               391
#define HEAPMENU_ALLOC_2K               392
#define HEAPMENU_ALLOC_5K               393
#define HEAPMENU_ALLOC_10K              394
#define HEAPMENU_ALLOC_25K              395
#define HEAPMENU_ALLOC_50K              396
#define HEAPMENU_ALLOC_NK               397
#define HEAPMENU_ALLOC_LAST             HEAPMENU_ALLOC_NK

#define HEAPMENU_ALLOC_BUT_FIRST        HEAPMENU_ALLOC_BUT_1K
#define HEAPMENU_ALLOC_BUT_1K           401
#define HEAPMENU_ALLOC_BUT_2K           402
#define HEAPMENU_ALLOC_BUT_5K           403
#define HEAPMENU_ALLOC_BUT_10K          404
#define HEAPMENU_ALLOC_BUT_25K          405
#define HEAPMENU_ALLOC_BUT_50K          406
#define HEAPMENU_ALLOC_BUT_NK           407
#define HEAPMENU_ALLOC_BUT_LAST         HEAPMENU_ALLOC_BUT_NK
#define HEAPMENU_ALLOCMENU_LAST         HEAPMENU_ALLOC_BUT_LAST


#define         SAVE_NAME_SIZE          _MAX_PATH
#define         MAX_RES                 100

#define ITEM_DISPLAY_CLASS              "watitemdisplayclass"
#define LOCAL_DISPLAY_CLASS             "watlocaldisplayclass"
#define LOCAL_MONITOR_CLASS             "watlocalmonitorclass"


/* Constants for Extended Sort Dialog */
/* SORT_BT_* and SORT_FLD_* constants should remain consecutive */

#define SORT_DONT_CARE          101
#define SORT_BT_1               110
#define SORT_BT_2               111
#define SORT_BT_3               112
#define SORT_BT_4               113
#define SORT_BT_5               114
#define SORT_BT_6               115
#define SORT_BT_LAST            SORT_BT_6
#define SORT_OK                 120
#define SORT_CANCEL             IDCANCEL
#define SORT_FLD_1              130
#define SORT_FLD_2              131
#define SORT_FLD_3              132
#define SORT_FLD_4              133
#define SORT_FLD_5              134
#define SORT_FLD_6              135
#define SORT_FLD_7              136
#define SORT_FLD_LAST           SORT_FLD_7

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
    FARPROC     alloc_proc;
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
    char far    *res;
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
extern char             *HeapWalkName;
extern HWND             HeapWalkMainWindow;
extern HANDLE           Instance;
extern BOOL             ListingDPMI;
extern WORD             HeapType;
extern HWND             ListBox,TitleLine;
extern int              HeapListSize;
extern heap_list        **HeapList;
extern BOOL             WDebug386;
extern BOOL             RefreshMainWindow;
extern char             *HeapTitles;
extern char             *HeapDPMITitles;
extern char             *HeapLocalTitles;
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
void PaintAllWindows();
void InitPaintProc();
void FiniPaintProc();
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
BOOL __export FAR PASCAL HeapWalkProc( HWND hwnd, WORD msg, WORD wparam, DWORD lparam );
void EndAdd( void );

/* hwsort.c */
void SortHeapList( void );

/* hwdisp.c */
BOOL __export FAR PASCAL ItemDisplayProc( HWND hwnd, WORD msg, WORD wparam, DWORD lparam );
void ShowHeapObject( WORD lbhdl );

/* hwsave.c */
void PutOutGlobalHeader( FILE *fptr );

/* hwobjec.c */
void ShowSelector( HWND list );
BOOL GlobDiscardObj( HWND list );
BOOL GlobSetObjPos( HWND list, BOOL oldest );
void RefreshAdd( HWND dialog, HWND lbhwnd );
BOOL __export FAR PASCAL AllocDlgProc( HWND hwnd, WORD msg, WORD wparam,
                                    DWORD lparam );
BOOL __export FAR PASCAL FreeNDlgProc( HWND hwnd, WORD msg, WORD wparam,
                                    DWORD lparam );
BOOL __export FAR PASCAL SetCodeDlgProc( HWND hwnd, WORD msg, WORD wparam,
                                    DWORD lparam );
SetMenusForAdd( HWND hwnd, BOOL start );
HWND StartAdd( HWND parent, ListBoxInfo *info );

/* hwalloc.c */
void UpdateAllocInfo( HWND hwnd );
void AllocAllBut( WORD type );
void AllocMore( WORD type );
void FreeSomeMem( WORD type );
void MyFreeAllMem( void );
void DoNBytes( HWND parent, WORD type );

/* hwlocal.c */
BOOL __export FAR PASCAL LocalHeapProc( HWND hwnd, WORD msg, WORD wparam, DWORD lparam );
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
int ErrorBox( HWND hwnd, DWORD msgid, UINT type );
HWND *MakePushWin( HWND hwnd, char *str, WORD cnt, ... );
void KillPushWin( HWND *push );

/* hwmonit */
BOOL __export FAR PASCAL LocalMonitorProc( HWND hwnd, WORD msg, WORD wparam,
                                    DWORD lparam );
void BeginMonitor( heap_list *item );

/* hwtable */
void InitializeStringTables( void );

/* hwldstr */
BOOL InitStringTable( void );
