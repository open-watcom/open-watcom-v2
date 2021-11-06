/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "commonui.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "watcom.h"
#include "spy.rh"

#include "ldstr.h"
#include "hint.h"

#include "jdlg.h"
#include "font.h"
#include "cguimem.h"
#include "savelbox.h"
#include "ctl3dcvr.h"
#ifdef __NT__
    #include "desknt.h"
#endif


#ifdef _M_I86
    #define _NEAR   __near
#else
    #define _NEAR
#endif

#ifdef __WINDOWS__
    #define SPY_CLASS_NAME  "watspy"
#else
    #define SPY_CLASS_NAME  "watspy_NT"
#endif

#ifndef WM_APP
    #define WM_APP          0x8000
#endif

typedef enum {
    DLG_FILE_OPEN,
    DLG_FILE_SAVE
} file_dlg_type;

#define BITMAP_X                (23 + 4)
#define BITMAP_Y                (19 + 4)
#define BORDER_X( x )           ((x) / 4)
#define BORDER_Y( y )           ((y) / 16)
#define GET_TOOLBAR_HEIGHT( y ) ((y) + 2 * BORDER_Y( y ) + 3)
#define TOOLBAR_HEIGHT          GET_TOOLBAR_HEIGHT( BITMAP_Y )

#define STYLE_HEX_LEN           8
#ifdef __WINDOWS__
    #define HWND_HEX_LEN        4
    #define ID_HEX_LEN          4
    #define WPARAM_HEX_LEN      4
    #define LPARAM_HEX_LEN      8
#else
    #define ID_HEX_LEN          8
  #ifdef _WIN64
    #define HWND_HEX_LEN        16
    #define WPARAM_HEX_LEN      16
    #define LPARAM_HEX_LEN      16
  #else
    #define HWND_HEX_LEN        8
    #define WPARAM_HEX_LEN      8
    #define LPARAM_HEX_LEN      8
  #endif
#endif

/*
 * field length in spy messages
 */
#define SPYOUT_NAME_LEN         29
#define SPYOUT_HWND_LEN         HWND_HEX_LEN
#define SPYOUT_MSG_LEN          ID_HEX_LEN
#define SPYOUT_WPARAM_LEN       WPARAM_HEX_LEN
#define SPYOUT_LPARAM_LEN       LPARAM_HEX_LEN

/*
 * offsets in spy messages
 */
#define SPYOUT_HWND             (0 + SPYOUT_NAME_LEN + 1)  /* 26 */
#define SPYOUT_MSG              (SPYOUT_HWND + SPYOUT_HWND_LEN + 1)
#ifdef __WINDOWS__
    #define SPYOUT_WPARAM       (SPYOUT_MSG + SPYOUT_MSG_LEN + 3)
    #define SPYOUT_LPARAM       (SPYOUT_WPARAM + SPYOUT_WPARAM_LEN + 2)
#else
    #define SPYOUT_WPARAM       (SPYOUT_MSG + SPYOUT_MSG_LEN + 1)
    #define SPYOUT_LPARAM       (SPYOUT_WPARAM + SPYOUT_WPARAM_LEN + 1)
#endif
#define SPYOUT_LENGTH           (SPYOUT_LPARAM + SPYOUT_LPARAM_LEN)

typedef enum {
    ON,
    OFF,
    NEITHER
} spystate;


typedef enum {
    #define pick(a,b,c) a,
    #include "spymsgcl.h"
    #undef pick
    FILTER_ENTRIES
} MsgClass;

typedef struct {
    const char      *str;
    UINT            id;
    boolbit         watch   : 1;
    boolbit         stopon  : 1;
} filter;

typedef struct {
    boolbit         watch   : 1;
    boolbit         stopon  : 1;
    UINT            id;
    char            *str;
    MsgClass        type;
    DWORD           count;
} message;

typedef struct {
    const char      *class_name;
    message         *message_array;
    WORD            message_array_size;
} class_messages;

typedef struct {
    WORD            xsize;
    WORD            ysize;
    int             xpos;
    int             ypos;
    int             last_xpos;
    int             last_ypos;
    boolbit         minimized       : 1;
    boolbit         on_top          : 1;
    boolbit         show_hints      : 1;
    boolbit         show_toolbar    : 1;
} WndConfigInfo;

typedef struct {
    char            *name;
    DWORD           flags;
    DWORD           mask;
} style_info;

typedef struct {
    const char      *class_name;
    style_info      *style_array;
    WORD            style_array_size;
} class_styles;

typedef void        CALLBACK message_func( LPMSG pmsg );

/*
 * globals
 */
extern char             *SpyName;
extern char             *TitleBar;
extern char             *TitleBarULine;
extern size_t           TitleBarLen;
extern spystate         SpyState;
extern char             _NEAR SpyPickClass[];
extern HWND             SpyListBox;
extern HWND             SpyListBoxTitle;
extern bool             SpyMessagesPaused;
extern HWND             SpyMainWindow;
extern HANDLE           MyTask;
extern HANDLE           Instance;
extern HANDLE           ResInstance;
extern WORD             ClassMessagesSize;
extern class_messages   _NEAR ClassMessages[];
extern bool             SpyMessagesAutoScroll;
extern bool             AutoSaveConfig;
extern WORD             WindowCount;
extern HWND             *WindowList;
extern class_styles     _NEAR ClassStyles[];
extern WORD             ClassStylesSize;
extern style_info       _NEAR StyleArray[];
extern WORD             StyleArraySize;
extern style_info       _NEAR ExStyleArray[];
extern WORD             ExStyleArraySize;
extern style_info       _NEAR ClassStyleArray[];
extern WORD             ClassStyleArraySize;


extern WndConfigInfo    SpyMainWndInfo;
extern HMENU            SpyMenu;
extern statwnd          *StatusHdl;
extern filter           Filters[FILTER_ENTRIES];
extern WORD             TotalMessageArraySize;


/*
 * function prototypes
 */

/* spybox.c */
extern void             SpyOut( const char *msg, LPMSG pmsg, const char *class_name );
extern void             CreateSpyBox( HWND );
extern void             ClearSpyBox( void );
extern void             SpyMessagePauseToggle( void );
extern void             ResizeSpyBox( WORD width, WORD height );
extern void             SaveSpyBox( void );
extern void             ResetSpyListBox( void );
extern bool             GetSpyBoxSelection( char *str );
extern char             *LogSpyBoxLine( bool listview, HWND list, int line );

/* spycfg.c */
extern void             LoadSpyConfig( char *fname );
extern void             SaveSpyConfig( char *fname );
extern void             DoSaveSpyConfig( void );
extern void             DoLoadSpyConfig( void );

/* spyfilt.c */
WINEXPORT extern message_func HandleMessage;

/* spymdlgs.c */
extern void             DoMessageDialog( HWND hwnd, ctl_id id );
extern void             DoMessageSelDialog( HWND hwnd );

/* spymisc.c */
extern void             GetHexStr( LPSTR res, ULONG_PTR num, size_t padlen );
extern bool             IsMyWindow( HWND hwnd );
extern void             GetWindowName( HWND hwnd, char *str );
extern void             GetClassStyleString( HWND hwnd, char *str, char *sstr );
extern void             GetWindowStyleString( HWND hwnd, char *str, char *sstr );
extern void             DumpToComboBox( char *str, HWND cb );
extern void             FormatSpyMessage( const char *msg, LPMSG pmsg, char *res );
extern void             SetSpyState( spystate ss );
extern bool             GetFileName( char *ext, file_dlg_type type, char *fname );
extern bool             InitGblStrings( void );

/* spymsgs.c */
extern message          *GetMessageDataFromID( UINT msgid, const char *class_name );
extern void             ProcessIncomingMessage( UINT msgid, const char *class_name, char *res );
extern void             InitMessages( void );
extern void             SetFilterMsgs( MsgClass type, bool val, bool is_watch );
extern bool             *SaveBitState( bool is_watch );
extern void             RestoreBitState( bool *data, bool is_watch );
extern void             ClearMessageCount( void );
extern bool             *CloneBitState( bool *old );
extern void             FreeBitState( bool *data );
extern void             CopyBitState( bool *dst, bool *src );
extern void             SetFilterSaveBitsMsgs( MsgClass type, bool val, bool *bits );

/* spypick.c */
extern void             FrameAWindow( HWND hwnd );
extern void             UpdateFramedInfo( HWND dlg, HWND framedhwnd, bool ispick  );
extern HWND             DoPickDialog( ctl_id );

/* spyproc.c */
WINEXPORT extern LRESULT CALLBACK SpyWindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
extern void             SetSpyState( spystate ss );

/* spysel.c */
extern void             ClearSelectedWindows( void );
extern void             AddSelectedWindow( HWND hwnd );
extern void             DoShowSelectedDialog( HWND hwnd, bool *spyall );
extern void             ShowFramedInfo( HWND hwnd, HWND framed );

/* spytool.c */
extern void             CreateSpyTool( HWND parent );
extern void             DestroySpyTool( void );
extern void             SetOnOffTool( spystate ss );
extern void             ResizeSpyTool( WORD width, WORD height );
extern void             ShowSpyTool( bool show );
extern void             GetSpyToolRect( RECT *prect );

/* spylog.c */
extern void             LogSpyBoxHeader( FILE *f );

/* spy.c */
extern void             SpyFini( void );

/* spyzord.c */
extern HWND             GetHwndFromPt( POINT *pt );
extern void             IdentifyWindows( HWND toplevel, HWND topmost );
extern void             RemoveWindow( HWND hwnd );
