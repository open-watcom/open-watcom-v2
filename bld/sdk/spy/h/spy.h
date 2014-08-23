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
* Description:  Main header file for the spy.
*
****************************************************************************/


/* Include new common control styles that require recent versions of Windows. */
#ifdef __NT__
    #undef _WIN32_IE
    #define _WIN32_IE   0x0600
#endif

#if defined( _M_I86 )
    #define _NEAR   __near
#else
    #define _NEAR
#endif

#include "precomp.h"
#include "bool.h"
#define MSG_RC_BASE     0
#include "rcstr.gh"
#include "uistr.gh"
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

#include "spydll.h"
#include "jdlg.h"
#include "font.h"
#include "mem.h"
#include "savelbox.h"
#include "log.h"
#include "spymsgs.h"
#include "msgs.h"
#include "winpick.h"
#include "msgsel.h"
#include "selwin.h"
#include "peekmsg.h"
#include "peekwin.h"
#include "spymenu.h"
#include "ctl3dcvr.h"
#include "watcom.h"
#ifdef USE_SNAP_WINDOW
    #include "desknt.h"
    #define SNAP_MODE   TRUE
#else
    #define SNAP_MODE   FALSE
#endif

#define RCSTR_MAX_LEN   128
#ifndef FILE_OPEN
    #define FILE_OPEN   1
#endif
#define FILE_SAVE       2

#define BITMAP_X                (23 + 4)
#define BITMAP_Y                (19 + 4)
#define BORDER_X( x )           ((x) / 4)
#define BORDER_Y( y )           ((y) / 16)
#define GET_TOOLBAR_HEIGHT( y ) ((y) + 2 * BORDER_Y( y ) + 3)
#define TOOLBAR_HEIGHT          GET_TOOLBAR_HEIGHT( BITMAP_Y )

/*
 * offsets in spy messages
 */
#define SPYOUT_HWND             26
#ifdef __NT__
    #define SPYOUT_HWND_LEN     8
    #define SPYOUT_MSG_LEN      8
    #define SPYOUT_WPARAM_LEN   8
    #define SPYOUT_LPARAM_LEN   8
    #define SPYOUT_WPARAM       SPYOUT_MSG + 1 + SPYOUT_MSG_LEN
    #define SPYOUT_LPARAM       SPYOUT_WPARAM + 1 + SPYOUT_WPARAM_LEN
#else
    #define SPYOUT_HWND_LEN     4
    #define SPYOUT_MSG_LEN      4
    #define SPYOUT_WPARAM_LEN   4
    #define SPYOUT_LPARAM_LEN   8
    #define SPYOUT_WPARAM       SPYOUT_MSG + 3 + SPYOUT_MSG_LEN
    #define SPYOUT_LPARAM       SPYOUT_WPARAM + 2 + SPYOUT_WPARAM_LEN
#endif

#define SPYOUT_MSG      SPYOUT_HWND + 1 + SPYOUT_HWND_LEN
#define SPYOUT_LENGTH   SPYOUT_LPARAM + SPYOUT_LPARAM_LEN


typedef enum {
    ON,
    OFF,
    NEITHER
} spystate;


typedef enum {
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
    MC_WINDOW,
    MC_CONTROL,
    FILTER_ENTRIES
} MsgClass;

typedef struct {
    bool        flag[2];
} filter;

#define M_WATCH         0
#define M_STOPON        1
typedef struct {
    bool        bits[2];
    WORD        id;
    char        *str;
    MsgClass    type;
    DWORD       count;
} message;

typedef struct {
    char        *class_name;
    message     *message_array;
    unsigned    message_array_size;
} class_messages;

typedef struct {
    WORD                xsize;
    WORD                ysize;
    int                 xpos;
    int                 ypos;
    bool                minimized;
    int                 last_xpos;
    int                 last_ypos;
    bool                on_top;
    bool                show_hints;
    bool                show_toolbar;
} WndConfigInfo;

/*
 * globals
 */
extern char             *SpyName;
extern char             *TitleBar;
extern char             *TitleBarULine;
extern spystate         SpyState;
extern char             _NEAR SpyPickClass[];
extern HWND             SpyListBox;
extern HWND             SpyListBoxTitle;
extern BOOL             SpyMessagesPaused;
extern HWND             SpyMainWindow;
extern HANDLE           MyTask;
extern HANDLE           Instance;
extern HANDLE           ResInstance;
extern WORD             MessageArraySize;
extern message          _NEAR MessageArray[];
extern WORD             ClassMessagesSize;
extern class_messages   _NEAR ClassMessages[];
extern BOOL             SpyMessagesAutoScroll;
extern BOOL             AutoSaveConfig;
extern WORD             WindowCount;
extern HWND             *WindowList;
extern LPVOID           HandleMessageInst;
extern WndConfigInfo    SpyMainWndInfo;
extern HMENU            SpyMenu;
extern statwnd          *StatusHdl;
extern filter           Filters[FILTER_ENTRIES];
extern unsigned         TotalMessageArraySize;


/*
 * function prototypes
 */

/* spybox.c */
extern void SpyOut( char *msg, LPMSG pmsg );
extern void CreateSpyBox( HWND );
extern void ClearSpyBox( void );
extern void SpyMessagePauseToggle( void );
extern void ResizeSpyBox( WORD width, WORD height );
extern void SaveSpyBox( void );
extern void ResetSpyListBox( void );
extern bool GetSpyBoxSelection( char *str );

/* spycfg.c */
extern void LoadSpyConfig( char *fname );
extern void SaveSpyConfig( char *fname );
extern void DoSaveSpyConfig( void );
extern void DoLoadSpyConfig( void );

/* spyfilt.c */
extern void FAR HandleMessage( LPMSG pmsg );

/* spymdlgs.c */
extern void DoMessageDialog( HWND hwnd, WORD wparam );
extern void DoMessageSelDialog( HWND hwnd );

/* spymisc.c */
extern void GetHexStr( LPSTR res, DWORD num, size_t padlen );
extern bool IsMyWindow( HWND hwnd );
extern void GetWindowName( HWND hwnd, char *str );
extern void GetClassStyleString( HWND hwnd, char *str, char *sstr );
extern void GetWindowStyleString( HWND hwnd, char *str, char *sstr );
extern void DumpToComboBox( char *str, HWND cb );
extern void FormatSpyMessage( char *msg, LPMSG pmsg, char *res );
extern void SetSpyState( spystate ss );
extern bool GetFileName( char *ext, int type, char *fname );
extern bool InitGblStrings( void );

/* spymsgs.c */
extern message *GetMessageDataFromID( int msgid, char *class_name );
extern void ProcessIncomingMessage( int msgid, char *class_name, char *res );
extern LPSTR GetMessageStructAddr( int msgid );
extern void InitMessages( void );
extern void SetFilterMsgs( MsgClass type, bool val, int bit );
extern bool *SaveBitState( int x );
extern void RestoreBitState( bool *data, int x );
extern void ClearMessageCount( void );
extern bool *CloneBitState( bool *old );
extern void FreeBitState( bool *data );
extern void CopyBitState( bool *dst, bool *src );
extern void SetFilterSaveBitsMsgs( MsgClass type, bool val, bool *bits );

/* spypick.c */
extern void FrameAWindow( HWND hwnd );
extern void UpdateFramedInfo( HWND dlg, HWND framedhwnd, bool ispick  );
extern HWND DoPickDialog( WORD );

/* spyproc.c */
WINEXPORT extern LRESULT CALLBACK SpyWindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
extern void SetSpyState( spystate ss );

/* spysel.c */
extern void ClearSelectedWindows( void );
extern void AddSelectedWindow( HWND hwnd );
extern void DoShowSelectedDialog( HWND hwnd, BOOL *spyall );
extern void ShowFramedInfo( HWND hwnd, HWND framed );

/* spytool.c */
extern void CreateSpyTool( HWND parent );
extern void DestroySpyTool( void );
extern void SetOnOffTool( spystate ss );
extern void ResizeSpyTool( WORD width, WORD height );
extern void ShowSpyTool( BOOL show );
extern void GetSpyToolRect( RECT *prect );

/* spylog.c */
extern void SpyLogTitle( FILE *f );

/* spy.c */
extern void SpyFini( void );

/* spyzord.c */
extern HWND GetHwndFromPt( POINT *pt );
extern void IdentifyWindows( HWND toplevel, HWND topmost );
extern void RemoveWindow( HWND hwnd );

