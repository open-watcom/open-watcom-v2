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


#define WIN31
#define STRICT
#include <windows.h>
#define MSG_RC_BASE     0
#include "rcstr.h"
#include "ldstr.h"
#include "hint.h"

#ifdef __NT__
#define USE_SNAP_WINDOW         1
#endif

#ifdef __NT__
 #define GCW_STYLE              (-26)
 #define UINT_STR_LEN           8
#else
 #define UINT_STR_LEN           4
#endif

#include "win1632.h"
#include "spydll.h"
#include "jdlg.h"

/* PenWindows specific messages */
#ifndef WM_PENWINFIRST
#define WM_PENWINFIRST      0x0380
#endif
#ifndef WM_PENWINLAST
#define WM_PENWINLAST       0x038F
#endif
#ifndef WM_COALESCE_FIRST
#define WM_COALESCE_FIRST   0x0390
#endif
#ifndef WM_COALESCE_LAST
#define WM_COALESCE_LAST    0x039F
#endif
#ifndef WM_CTLCOLOR
#define WM_CTLCOLOR         0x0019
#endif
#ifndef WM_GETHOTKEY
#define WM_GETHOTKEY    0x0033
#endif
#ifndef WM_SETHOTKEY
#define WM_SETHOTKEY    0x0032
#endif
#ifndef WM_PAINTICON
#define WM_PAINTICON    0x26
#endif
#ifndef WM_OTHERWINDOWCREATED
#define WM_OTHERWINDOWCREATED   0x0042
#endif
#ifndef WM_OTHERWINDOWDESTROYED
#define WM_OTHERWINDOWDESTROYED 0x0043
#endif

// Messages undocumented for WINDOWS but documented for NT
#ifndef WM_ENTERMENULOOP
#define WM_ENTERMENULOOP        0x0211
#endif
#ifndef WM_EXITMENULOOP
#define WM_EXITMENULOOP         0x0212
#endif

// Windows 95 specific messages

#ifndef WM_NOTIFY
#define WM_NOTIFY                       0x004E
#endif
#ifndef WM_INPUTLANGCHANGEREQUEST
#define WM_INPUTLANGCHANGEREQUEST       0x0050
#endif
#ifndef WM_INPUTLANGCHANGE
#define WM_INPUTLANGCHANGE              0x0051
#endif
#ifndef WM_TCARD
#define WM_TCARD                        0x0052
#endif
#ifndef WM_HELP
#define WM_HELP                         0x0053
#endif
#ifndef WM_USERCHANGED
#define WM_USERCHANGED                  0x0054
#endif
#ifndef WM_CONTEXTMENU
#define WM_CONTEXTMENU                  0x007B
#endif
#ifndef WM_STYLECHANGING
#define WM_STYLECHANGING                0x007C
#endif
#ifndef WM_STYLECHANGED
#define WM_STYLECHANGED                 0x007D
#endif
#ifndef WM_DISPLAYCHANGE
#define WM_DISPLAYCHANGE                0x007E
#endif
#ifndef WM_GETICON
#define WM_GETICON                      0x007F
#endif
#ifndef WM_SETICON
#define WM_SETICON                      0x0080
#endif

#include "..\misc\font.h"
#include "..\misc\mem.h"
#include "..\misc\savelbox.h"
#include "..\misc\log.h"
#ifdef __NT__
#include "spymsgnt.h"
#else
#include "spymsg.h"
#endif
#include "msgs.h"
#include "winpick.h"
#include "msgsel.h"
#include "selwin.h"
#include "peekmsg.h"
#include "peekwin.h"
#include "spymenu.h"
#include "ctl3d.h"
#ifdef USE_SNAP_WINDOW
 #include "desknt.h"
 #define SNAP_MODE      TRUE
#else
 #define SNAP_MODE      FALSE
#endif

#define RCSTR_MAX_LEN   128
#define FILE_OPEN       1
#define FILE_SAVE       2

#define BITMAP_X                ( 23 + 4 )
#define BITMAP_Y                ( 19 + 4 )
#define BORDER_X( x )           ( (x) / 4 )
#define BORDER_Y( y )           ( (y) / 16 )
#define GET_TOOLBAR_HEIGHT( y ) ( (y) + 2 * BORDER_Y( y ) + 3 )
#define TOOLBAR_HEIGHT          GET_TOOLBAR_HEIGHT( BITMAP_Y )

/*
 * offsets in spy messages
 */
#define SPYOUT_HWND     26
#ifdef __NT__
 #define SPYOUT_HWND_LEN        8
 #define SPYOUT_MSG_LEN         8
 #define SPYOUT_WPARAM_LEN      8
 #define SPYOUT_LPARAM_LEN      8
 #define SPYOUT_WPARAM          SPYOUT_MSG+1+SPYOUT_MSG_LEN
 #define SPYOUT_LPARAM          SPYOUT_WPARAM+1+SPYOUT_WPARAM_LEN
#else
 #define SPYOUT_HWND_LEN        4
 #define SPYOUT_MSG_LEN         4
 #define SPYOUT_WPARAM_LEN      4
 #define SPYOUT_LPARAM_LEN      8
 #define SPYOUT_WPARAM          SPYOUT_MSG+3+SPYOUT_MSG_LEN
 #define SPYOUT_LPARAM          SPYOUT_WPARAM+2+SPYOUT_WPARAM_LEN
#endif

#define SPYOUT_MSG      SPYOUT_HWND+1+SPYOUT_HWND_LEN
#define SPYOUT_LENGTH   SPYOUT_LPARAM+SPYOUT_LPARAM_LEN


typedef enum {
    ON,
    OFF,
    NEITHER
} spystate;


typedef enum {
    MC_NULL,
    MC_CLIPBRD,
    MC_DDE,
    MC_IME,
    MC_INIT,
    MC_INPUT,
    MC_MDI,
    MC_MOUSE,
    MC_NC_MISC,
    MC_NC_MOUSE,
    MC_OTHER,
    MC_SYSTEM,
    MC_UNKNOWN,
    MC_USER,
    MC_WINDOW
} MsgClass;

typedef struct {
    char        flag[2];
    MsgClass    type;
} filter;

typedef struct {
    filter clipboard;
    filter dde;
    filter ime;
    filter init;
    filter input;
    filter mdi;
    filter mouse;
    filter ncmisc;
    filter ncmouse;
    filter other;
    filter system;
    filter unknown;
    filter user;
    filter window;
} _filters;

#define FILTER_ENTRIES  13
typedef union {
    _filters filts;
    filter array[ FILTER_ENTRIES ];
} filters;
#define M_WATCH         0
#define M_STOPON        1
typedef struct {
    char        bits[2];
    WORD        id;
    char        *str;
    MsgClass    type;
    DWORD       count;
} message;

typedef struct {
    WORD                xsize;
    WORD                ysize;
    int                 xpos;
    int                 ypos;
    BOOL                minimized;
    int                 last_xpos;
    int                 last_ypos;
    BOOL                on_top;
    BOOL                show_hints;
}WndConfigInfo;

/*
 * globals
 */
extern char             *SpyName;
extern char             *TitleBar;
extern char             *TitleBarULine;
extern int              TitleBarLen;
extern spystate         SpyState;
extern char             near SpyPickClass[];
extern HWND             SpyListBox;
extern HWND             SpyListBoxTitle;
extern BOOL             SpyMessagesPaused;
extern HWND             SpyMainWindow;
extern HANDLE           MyTask;
extern HANDLE           Instance;
extern HANDLE           ResInstance;
extern filters          Filters;
extern WORD             MessageArraySize;
extern message          near MessageArray[];
extern BOOL             SpyMessagesAutoScroll;
extern BOOL             AutoSaveConfig;
extern WORD             WindowCount;
extern HWND             *WindowList;
extern LPVOID           HandleMessageInst;
extern WndConfigInfo    SpyMainWndInfo;
extern HMENU            SpyMenu;
extern statwnd          *StatusHdl;

/*
 * function prototypes
 */

/* spybox.c */
void SpyOut( LPSTR res );
void CreateSpyBox( HWND );
void ClearSpyBox( void );
void SpyMessagePauseToggle( void );
void ResizeSpyBox( WORD width, WORD height );
void SaveSpyBox( void );
void ResetSpyListBox( void );

/* spycfg.c */
void LoadSpyConfig( char *fname );
void SaveSpyConfig( char *fname );
void DoSaveSpyConfig( void );
void DoLoadSpyConfig( void );

/* spyfilt.c */
void CALLBACK HandleMessage( LPMSG pmsg );

/* spymdlgs.c */
void DoMessageDialog( HWND hwnd, WORD wparam );
void DoMessageSelDialog( HWND hwnd );

/* spymisc.c */
void GetHexStr( LPSTR res, DWORD num, int padlen );
BOOL IsMyWindow( HWND hwnd );
void GetWindowName( HWND hwnd, char *str );
void GetClassStyleString( HWND hwnd, char *str, char *sstr );
void GetWindowStyleString( HWND hwnd, char *str, char *sstr );
void DumpToComboBox( char *str, HWND cb );
void FormatSpyMessage( char *msg, LPMSG pmsg, char *res );
void SetSpyState( spystate ss );
BOOL GetFileName( char *ext, int type, char *fname );
BOOL InitGblStrings( void );

/* spymsgs.c */
message *GetMessageDataFromID( int msgid );
void ProcessIncomingMessage( int msgid, char *res );
LPSTR GetMessageStructAddr( int msgid );
void InitMessages( void );
void SetFilterMsgs( MsgClass type, BOOL val, int bit );
char *SaveBitState( int x );
void RestoreBitState( char *data, int x );
void ClearMessageCount( void );
char *CloneBitState( char *old );
void FreeBitState( char *data );
void CopyBitState( char *dst, char *src );
void SetFilterSaveBitsMsgs( MsgClass type, BOOL val, char *bits );

/* spypick.c */
void FrameAWindow( HWND hwnd, BOOL use_snap );
void UpdateFramedInfo( HWND dlg, HWND framedhwnd, BOOL ispick  );
HWND DoPickDialog( WORD );
LONG CALLBACK SpyPickProc( HWND hwnd, int msg, UINT wparam, LONG lparam );

/* spyproc.c */
LONG CALLBACK SpyWindowProc( HWND hwnd, int msg, UINT wparam, LONG lparam );
void SetSpyState( spystate ss );

/* spysel.c */
void ClearSelectedWindows( void );
void AddSelectedWindow( HWND hwnd );
void DoShowSelectedDialog( HWND hwnd, BOOL *spyall );
void ShowFramedInfo( HWND hwnd, HWND framed );

/* spytool.c */
void CreateSpyTool( HWND parent );
void DestroySpyTool( void );
void SetOnOffTool( spystate ss );
void ResizeSpyTool( WORD width, WORD height );

/* spylog.c */
void SpyLogTitle( int f );

/* spy.c */
void SpyFini( void );

/* spyzord.c */
HWND GetHwndFromPt( POINT *pt );
void IdentifyWindows( HWND toplevel, HWND topmost );
void RemoveWindow( HWND hwnd );
