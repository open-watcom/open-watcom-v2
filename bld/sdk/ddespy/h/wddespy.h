/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DDE Spy master include.
*
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "commonui.h"
#include <ddeml.h>
#include <dde.h>
#include "bool.h"
#include "cguimem.h"
#include "font.h"
#include "srchmsg.h"
#include "log.h"
#include "pushwin.h"
#include "alias.h"
#include "savelbox.h"
#include "toolbr.h"
#include "mark.h"
#ifndef NOUSE3D
    #include "ctl3dcvr.h"
#endif
#include "rcstr.grh"
#include "uistr.grh"
#include "ldstr.h"
#include "hint.h"
#include "ddemenu.rh"

#define RCSTR_MAX_LEN   500
#define DDE_HELP_FILE   "ddespy.hlp"
#define DDE_CHM_FILE    "ddespy.chm"
#define HELP_HELP_FILE  "winhelp.hlp"
#define TRACKING_CLASS  "WDDE_TRACKING_CLASS"

/* filter type constants */

#define FILTER_MESSAGE                  10
#define FILTER_CB                       11

/* tracking push window constants */

#define PUSH_STR_HDL                    201
#define PUSH_STR_CNT                    202
#define PUSH_STR_TEXT                   203

#define PUSH_CLIENT                     204
#define PUSH_SERVER                     205
#define PUSH_SERVICE                    206
#define PUSH_TOPIC                      207
#define PUSH_TYPE                       208
#define PUSH_ITEM                       209
#define PUSH_FORMAT                     210
#define PUSH_INST                       211

/*
 * Indices for accessing the Monitoring array
 * These macros are for coding convenience ONLY.  Not all accesses to
 * the Monitoring array use them.
 */
#define MON_POST_IND            (DDEMENU_MON_POST - DDE_MON_FIRST)
#define MON_SENT_IND            (DDEMENU_MON_SENT - DDE_MON_FIRST)
#define MON_STR_IND             (DDEMENU_MON_STR - DDE_MON_FIRST)
#define MON_CB_IND              (DDEMENU_MON_CB - DDE_MON_FIRST)
#define MON_ERR_IND             (DDEMENU_MON_ERR - DDE_MON_FIRST)
#define MON_LNK_IND             (DDEMENU_MON_LNK - DDE_MON_FIRST)
#define MON_CONV_IND            (DDEMENU_MON_CONV - DDE_MON_FIRST)


#ifdef __WINDOWS__
    #define HWND_FMT_LEN        4
    #define CONV_FMT_LEN        8
    #define TASK_FMT_LEN        4
    #define ID_FMT_LEN          4
    #define HSZ_FMT_LEN         4
    #define WPARAM_FMT_LEN      4
#else
    #define ID_FMT_LEN          8
  #ifdef _WIN64
    #define HWND_FMT_LEN        16
    #define CONV_FMT_LEN        16
    #define TASK_FMT_LEN        16
    #define HSZ_FMT_LEN         16
    #define WPARAM_FMT_LEN      16
  #else
    #define HWND_FMT_LEN        8
    #define CONV_FMT_LEN        8
    #define TASK_FMT_LEN        8
    #define HSZ_FMT_LEN         8
    #define WPARAM_FMT_LEN      8
  #endif
#endif

typedef struct lstinfo {
    HWND        box;
    LONG        hinthite;
    RECT        old_area;
    LONG        ypos;
} ListBoxInfo;

typedef struct ddewndinfo {
    ListBoxInfo         list;
    statwnd             *hintbar;
    BOOL                hint_tmp_hide;
    LONG                horz_extent;
    LONG                char_extent;
} DDEWndInfo;

typedef struct serverinfo {
    char        *server;
    char        *instname;
} ServerInfo;

typedef struct stringinfo {
    WORD        cnt;
    char        *str;
    HSZ         hsz;
} StringInfo;

/*
 * The LinkInfo structure is used to keep track of information
 * for both the link tracking window and the conversations tracking window
 * since the conversations tracking window uses a subset of the link
 * information and this allows us to keep common sorting routines.
 */

typedef struct linkinfo {
    HCONV       client;
    HCONV       server;
    char        *service;
    char        *type;
    char        *topic;
    char        *item;
    char        *format;
} LinkInfo;

typedef struct wndconfiginfo {
    HWND                hwnd;
    WORD                state;
    WORD                xsize;
    WORD                ysize;
    int                 xpos;
    int                 ypos;
    int                 last_xpos;
    int                 last_ypos;
    bool                visible;        /* used for tracking windows only */
} WndConfigInfo;

typedef struct {
    DWORD       textid;
    int         id;
    WORD        size;
} TrackHeaderInfo;

typedef struct ddetrackinfo {
    WORD                type;
    WORD                cnt;
    WORD                sorttype;
    void                *data;
    ListBoxInfo         list;
    WndConfigInfo       *config;
    WORD                hdrcnt;
    TrackHeaderInfo     *hdrinfo;
    HWND                *hdr;
} DDETrackInfo;

typedef struct ddeconfiginfo {
    bool                scroll;
    bool                alias;
    bool                screen_out;
    bool                show_tb;
    bool                show_hints;
    bool                on_top;
} DDEConfigInfo;

extern HANDLE           Instance;
extern HWND             DDEMainWnd;
extern DWORD            DDEInstId;
extern bool             Monitoring[MAX_DDE_MON];
extern WndConfigInfo    Tracking[MAX_DDE_TRK];
extern WndConfigInfo    MainWndConfig;
extern DDEConfigInfo    ConfigInfo;
extern AliasHdl         HwndAlias;
extern AliasHdl         ConvAlias;
extern AliasHdl         TaskAlias;
extern char             *AppName;

/* window and dialog procedures */
WINEXPORT extern FNCALLBACK DDEProc;
WINEXPORT extern LRESULT    CALLBACK DDEMainWndProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
WINEXPORT extern LRESULT    CALLBACK DDETrackingWndProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
WINEXPORT extern INT_PTR    CALLBACK FilterDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

/* ddeproc.c */
void    SetMainWndDefault( void );

/* ddemsg.c */
char    *HSZToString( HSZ hsz );
char    *GetFmtStr( UINT fmt, char *buf );
void    RecordMsg( const char *buf );
void    InitAliases( void );
void    RefreshAliases( void );

/* ddebox.c */
void    CreateListBox( HWND parent, ListBoxInfo *info );
void    ResizeListBox( WORD width, WORD height, ListBoxInfo *info );

/* ddetrack.c */
void    InitTrackWnd( HWND hwnd );
bool    CreateTrackWnd( void );
void    DisplayTracking( unsigned i );
void    TrackStringMsg( MONHSZSTRUCT *info );
void    TrackLinkMsg( MONLINKSTRUCT *info );
void    TrackConvMsg( MONCONVSTRUCT *info );
void    TrackServerMsg( MONCBSTRUCT *info );
void    FiniTrackWnd( void );
void    SetTrackWndDefault( void );
void    SetTrackFont( void );

/* ddefltr.c */
bool    DoFilter( UINT msg, WORD filter_type );
void    SetFilter( char *msgfilter, char *cbfilter );
void    GetFilter( char *msgfilter, char *cbfilter );

/* ddecfg.c */
void    ReadConfig( void );
void    SaveConfigFile( void );

/* ddemisc.c */
void    LogHeader( FILE *f );
void    DumpHeader( FILE *fptr );
char    *DumpLine( bool listview, HWND list, int line );
bool    InitGblStrings( void );
void    FiniRCStrings( void );
void    GetHexStr( LPSTR res, ULONG_PTR num, size_t padlen );

/* ddetool.c */
void    GetFixedTBRect( HWND hwnd, RECT *rect );
void    MakeDDEToolBar( HWND hwnd );
void    DDEToolBarFini( void );
void    ResizeTB( HWND owner );
bool    ToggleTB( HWND parent );
void    DDESetStickyState( ctl_id itemid, bool isdown );
