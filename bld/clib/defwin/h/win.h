/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Default windowing - main include file.
*
****************************************************************************/


#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#ifdef _M_I86
    #include <malloc.h>
#endif
#include "_defwin.h"
/*
 * Define the _MBCS macro to compile defwin stuff with multibyte support.
 */
#define _MBCS
#include "mbdefwin.h"

#if defined( __OS2__ )
    #include "pmdlg.h"
#else
    #include "windlg.h"
#endif


#ifdef __OS2__
typedef int         HANDLE;
extern  unsigned    __WinSetWindowPos(unsigned);
#pragma aux __WinSetWindowPos __parm [__eax] __modify [__ebx]
#define WinSetWindowPos(a1,a2,a3,a4,a5,a6,a7)           \
        __WinSetWindowPos(WinSetWindowPos(a1,a2,a3,a4,a5,a6,a7))
#endif

#if defined( __WINDOWS_386__ )
#define GetPtrGlobalLock(data) MK_FP32( GlobalLock( data ) )
#else
#define GetPtrGlobalLock(data) GlobalLock( data )
#endif

#if defined( __SW_BM ) && defined( __NT__ )
extern void _AccessWinLines( void );
extern void _ReleaseWinLines( void );
#else
#define _AccessWinLines()
#define _ReleaseWinLines()
#endif

#ifdef _M_I86
  #define _EXPORT __export
  #define FARmemcpy _fmemcpy
  #define FARmemset _fmemset
  #define FARstrlen _fstrlen
  #define FARstrcpy _fstrcpy
  #define FARstrcat _fstrcat
  #define FARmalloc _fmalloc
  #define FARrealloc _frealloc
  #define FARfree _ffree
#else
  #define _EXPORT
  #define FARmemcpy memcpy
  #define FARmemset memset
  #define FARstrlen strlen
  #define FARstrcpy strcpy
  #define FARstrcat strcat
  #define FARmalloc malloc
  #define FARrealloc realloc
  #define FARfree free
  #ifdef __NT__
    #define AllocAlias16( a ) a
    #define FreeAlias16( a )
  #endif
#endif

#if defined( __OS2__ )
  #define DWORD ULONG
  #define WORD  USHORT
  #if !defined( WPI_INTERFAC )
    typedef char        _WCI86FAR *LPSTR;
    typedef struct {
        int     left;
        int     top;
        int     right;
        int     bottom;
    } RECT;
  #endif
  typedef RECT  *PRECT;

  #define GetWindowLong WinQueryWindowULong
  #define VK_CONTROL VK_CTRL
  #define VK_RETURN  VK_NEWLINE
  #define VK_BACK    VK_BACKSPACE
#endif

#define SPACE_BETWEEN_LINES     2

/*
 * macros etc
 */
#define MAX_BUFF        128

#define CTRL_V          'V'-'A'+1
#define CTRL_U          'U'-'A'+1
#define CTRL_Z          'Z'-'A'+1

#define DEFAULT_CLEAR_INTERVAL  1000000

#define TAB( col ) ((((col-1)/8)+1)*8 - (col-1))
#define TOOWIDE( val,w ) (val >= (w->maxwidth-2))

#if defined( __OS2__ )
  #define FIXED_FONT 1
#endif

#if !defined( __OS2__ )
  typedef enum {
      BLACK=0, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, WHITE,
      DARK_GRAY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED,
      LIGHT_MAGENTA, YELLOW, BRIGHT_WHITE
  } colors;
#endif

typedef enum {
    SMALL_CURSOR, FAT_CURSOR, ORIGINAL_CURSOR, KILL_CURSOR
} cursors;

/*
 * structures
 */
typedef struct line_data {
    struct line_data _WCI86FAR *next, _WCI86FAR *prev;
    char has_cr;
    char data[1];
} line_data;
typedef line_data _WCI86FAR *LPLDATA;

typedef struct window_data {
    int             _WCI86FAR *handles;
    int             handle_cnt;
    int             text_color;
    int             background_color;
    int             x1, y1, x2, y2;
    int             width, height;
    int             xchar, ychar;
#ifdef _MBCS
    mb_char         _WCI86FAR *image;
#else
    LPSTR           image;
#endif
#if !defined( __OS2__ )
    HBRUSH          brush;
#endif
    HWND            hwnd;
    LPLDATA         LineHead;
    LPLDATA         LineTail;
    DWORD           LastLineNumber;
    DWORD           CurrentLineNumber;
    DWORD           TopLineNumber;
    LPLDATA         tmpbuff;
    int             buffoff;
    int             curr_pos;
#if !defined( __OS2__ )
    HANDLE          inst;
#endif
    int             maxwidth;
#if defined( __OS2__ )
    int             base_offset;
#endif
    int             InputMode;
#if defined( __OS2__ )
    HWND            frame;
#endif
    short           menuid;
    BOOL            lineinprogress;
    cursors         CaretType;
    unsigned char   resizing:1;
    unsigned char   active:1;
    unsigned char   hascursor:1;
    unsigned char   gphwin:1;
    unsigned char   no_advance:1;
    unsigned char   destroy:1;
    unsigned char   hold7:1;
    unsigned char   hold8:1;
} window_data;

// LPWDATA is defined in _defwin.h as follows:
// typedef struct window_data _WCI86FAR *LPWDATA;
typedef LPWDATA     _WCI86FAR *LPLPWDATA;

typedef struct {
    LPLPWDATA   windows;
    HANDLE      inst;
    int         window_count;
} main_window_data;
typedef main_window_data _WCI86FAR *LPMWDATA;

/*
 * global variables
 */
#if defined( __OS2__ )
extern HAB      _AnchorBlock;
extern HWND     _MainFrameWindow;
#else
extern HFONT    _FixedFont;
extern HMENU    _MainMenu,_SubMenuEdit,_SubMenuWindows;
extern char     _MenuFlush[];
extern char     _MenuSetClearInt[];
extern char     _MenuWrite[];
extern char     _MenuFile[];
extern char     _MenuExit[];
extern char     _MenuEdit[];
extern char     _MenuCopy[];
extern char     _MenuHelp[];
extern char     _MenuAbout[];
extern char     _MenuWindows[];
#endif
extern HWND     _MainWindow;
extern int      _MainWindowDestroyed;
extern LPMWDATA _MainWindowData;
extern char     *_ClassName;
extern DWORD    _AutoClearLines;
extern BOOL     _GotEOF;
extern DWORD    _ColorMap[16];


/*
 * function prototypes
 */

/* windrvr.c/pmdrvr.c */
extern int      _SetAboutDlg( const char *, const char * );

#if defined( __OS2__ )
/* pmdrvr.c */
extern MRESULT EXPENTRY _MainDriver( HWND, USHORT, MPARAM, MPARAM );
#else
/* windrvr.c */
extern WINEXPORT LRESULT CALLBACK _MainDriver( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam );
#endif

/* windisp.c */
extern void     _DisplayAllLines( LPWDATA, int );
extern void     _ResizeWin( LPWDATA, int, int, int, int );
extern void     _ShiftWindow( LPWDATA, int );
extern void     _DisplayLineInWindowWithColor( LPWDATA, int, LPSTR, int, int, int, int );
extern void     _DisplayLineInWindow( LPWDATA, int, LPSTR );
extern void     _ClearWindow( LPWDATA );

/* windlg.c */
extern void     _GetAutoClearInterval( void );
extern void     _DoAbout( void );

/* wingen.c */
extern LPWDATA  _GetWindowData( HWND );
extern LPWDATA  _GetActiveWindowData( void );
extern void     _MakeWindowActive( LPWDATA w );
extern LPWDATA  _IsWindowedHandle( int handle );
extern void     _InitFunctionPointers( void );
extern void     _InitMainWindowData( HANDLE );
extern void     _FiniMainWindowData( void );
extern LPWDATA  _AnotherWindowData( HWND hwnd, va_list args );
extern void     _FreeWindowData( LPWDATA );
extern void     _GetWindowNameAndCoords( const char *name, char *dest, int *x1, int *x2, int *y1, int *y2 );
extern void     _WindowsExit( void );
extern int      _DestroyOnClose( LPWDATA );
extern int      _YieldControl( void );
extern void     _RemoveWindowedHandle( int handle );

/* winio.c */
extern unsigned _DoStdin( LPWDATA, void *, unsigned );
extern unsigned _DoStdout( LPWDATA, const void *, unsigned );
extern unsigned _DoKbhit( LPWDATA );
extern unsigned _DoGetch( LPWDATA );
extern unsigned _DoGetche( LPWDATA );
extern void     _DoPutch( LPWDATA, unsigned );

/* winkey.c */
extern void     _WindowsKeyUp( WORD, WORD );
extern void     _WindowsKeyPush( WORD, WORD );
extern void     _WindowsVirtualKeyPush( WORD, WORD );
extern int      _KeyboardHit( BOOL );
extern int      _GetKeyboard( int * );
extern int      _GetString( LPWDATA, char *, int );

/* winlines.c */
extern void     _AddLine( LPWDATA, const void *, unsigned );
extern DWORD    _GetLineFromThumbPosition( LPWDATA, WORD );
extern void     _PositionScrollThumb( LPWDATA );
extern LPLDATA  _GetLineDataPointer( LPWDATA, DWORD );
extern void     _FreeAllLines( LPWDATA );
extern void     _SaveAllLines( LPWDATA );
extern void     _CopyAllLines( LPWDATA );
extern int      _UpdateInputLine( LPWDATA, char *, unsigned, BOOL );
extern DWORD    _GetLastLineNumber( LPWDATA w );

/* winmisc.c/pmmisc.c */
#if !defined( __OS2__ )
extern HFONT    _SetMyDC( HDC, DWORD, DWORD ) ;
#else
extern void     _Error( HWND hwndDlg, char *caption, char *msg );
extern void     _ResizeWindows( void );
#endif
extern int      _MessageLoop( BOOL );
extern int      _BlockingMessageLoop( BOOL );
extern void     _NewCursor( LPWDATA, cursors );
extern void     _DisplayCursor( LPWDATA w );
extern void     _SetInputMode( LPWDATA, int );
extern void     _ShowWindowActive( LPWDATA w, LPWDATA last );
extern void     _OutOfMemory( void );
extern _WCNORETURN void _OutOfMemoryExit( void );
extern void     _ExecutionComplete( void );
extern int      _SetAppTitle( const char * );
extern int      _SetConTitle( LPWDATA, const char *);
extern int      _ShutDown( void );

/* winmove.c */
extern void     _MovePageUp( LPWDATA );
extern void     _MovePageDown( LPWDATA );
extern void     _MoveLineUp( LPWDATA );
extern void     _MoveLineDown( LPWDATA );
extern void     _MoveToLine( LPWDATA, DWORD, BOOL );

/* winnew.c/pmmnew.c */
extern unsigned _NewWindow( const char *name, ... );
extern int      _CloseWindow( LPWDATA );
extern void     _ReleaseWindowResources( LPWDATA w );
#if defined( __OS2__ )
extern void     _SetWinMenuHandle( HWND hmenu );
extern HWND     _GetWinMenuHandle( void );
#endif

/* winpaint.c */
#if defined( __OS2__ )
extern void     _RepaintWindow( LPWDATA, PRECT, HPS );
#else
extern void     _RepaintWindow( LPWDATA, PRECT, HDC );
#endif

/* pmmain.c */
#if defined( __OS2__ )
extern void     _SelectFont( HPS );
extern void     _CreateFont( LPWDATA );
#endif

/* winmain.c */
#if defined( __WINDOWS__ ) || defined( __NT__ )
extern int      main( int, char ** );

extern int      PASCAL DefaultWinMain( HINSTANCE inst, HINSTANCE previnst,
                        LPSTR cmd, int show, int (*pmain)( int, char ** ) );
extern int      PASCAL WinMain( HINSTANCE inst, HINSTANCE previnst, LPSTR cmd, int show );
#endif
