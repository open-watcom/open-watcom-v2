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
* Description:  Default windowing - main include file.
*
****************************************************************************/

#include "variety.h"
#ifdef __OS2__
#define INCL_WIN
#include <wos2.h>
typedef int     HANDLE;

#pragma aux __WinSetWindowPos = parm [eax] modify [ebx]
extern  unsigned        __WinSetWindowPos(unsigned);
#define WinSetWindowPos(a1,a2,a3,a4,a5,a6,a7)           \
        __WinSetWindowPos(WinSetWindowPos(a1,a2,a3,a4,a5,a6,a7))
#else
#include <windows.h>
#include <win1632.h>
#endif
#include <stdarg.h>
#include "_defwin.h"


/*
 * Define the _MBCS macro to compile defwin stuff with multibyte support.
 */
#define _MBCS
#include "mbdefwin.h"


#if defined(__NT__) && defined(__SW_BM)
extern void _AccessWinLines( void );
extern void _ReleaseWinLines( void );
#else
#define _AccessWinLines()
#define _ReleaseWinLines()
#endif

#if defined(__386__) || defined(__AXP__) || defined(__PPC__)
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
    #define MK_FP32( a ) a
    #define AllocAlias16( a ) a
    #define FreeAlias16( a )
  #endif
#else
  #define _EXPORT __export
  #define FARmemcpy _fmemcpy
  #define FARmemset _fmemset
  #define FARstrlen _fstrlen
  #define FARstrcpy _fstrcpy
  #define FARstrcat _fstrcat
  #define FARmalloc _fmalloc
  #define FARrealloc _frealloc
  #define FARfree _ffree
#endif

#if defined( __OS2__ )
  #include <pmdlg.h>
#else
  #include <windlg.h>
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
#define MSG_EXIT        1
#define MSG_FLUSH       2
#define MSG_WRITE       3
#define MSG_COPY        4
#define MSG_SETCLEARINT 5
#define MSG_ABOUT       6
#define DLG1_EDIT       10
#define MSG_WINDOWS     200

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
    int         *handles;
    int         handle_cnt;
    int         text_color;
    int         background_color;
    int         x1, y1, x2, y2;
    int         width, height;
    int         xchar, ychar;
#ifdef _MBCS
    mb_char _WCI86FAR *image;
#else
    LPSTR       image;
#endif
#if !defined( __OS2__ )
    HBRUSH      brush;
#endif
    HWND        hwnd;
    LPLDATA     LineHead, LineTail;
    DWORD       LastLineNumber;
    DWORD       CurrentLineNumber;
    DWORD       TopLineNumber;
    LPLDATA     tmpbuff;
    int         buffoff;
    int         curr_pos;
#if !defined( __OS2__ )
    HANDLE      inst;
#endif
    int         maxwidth;
#if defined( __OS2__ )
    int         base_offset;
#endif
    int         InputMode;
#if defined( __OS2__ )
    HWND        frame;
#endif
    short       menuid;
    BOOL        lineinprogress;
    cursors     CaretType;
    char        resizing:1;
    char        active:1;
    char        hascursor:1;
    char        gphwin:1;
    char        no_advance:1;
    char        destroy:1;
    char        hold7:1;
    char        hold8:1;
} window_data;

// LPWDATA is defined in _defwin.h as follows:
// typedef struct window_data _WCI86FAR *LPWDATA;
typedef LPWDATA _WCI86FAR *LPLPWDATA;

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
extern HAB _AnchorBlock;
extern HWND _MainFrameWindow;
#else
extern HFONT _FixedFont;
extern HMENU _MainMenu,_SubMenuEdit,_SubMenuWindows;
extern char _MenuFlush[];
extern char _MenuSetClearInt[];
extern char _MenuWrite[];
extern char _MenuFile[];
extern char _MenuExit[];
extern char _MenuEdit[];
extern char _MenuCopy[];
extern char _MenuHelp[];
extern char _MenuAbout[];
extern char _MenuWindows[];
#endif
extern HWND _MainWindow;
extern int _MainWindowDestroyed;
extern LPMWDATA _MainWindowData;
extern char *_ClassName;
extern DWORD _AutoClearLines;
extern BOOL _GotEOF;
extern DWORD _ColorMap[16];


/*
 * function prototypes
 */

#if defined( __OS2__ )
/* pmdrvr.c */
MRESULT EXPENTRY _MainDriver( HWND, USHORT, MPARAM, MPARAM );
int _SetAboutDlg( char *, char * );

/* pmmain.c */
void _SelectFont( HPS );
#endif

/* windisp.c */
void _DisplayAllLines( LPWDATA, int );
void _ResizeWin( LPWDATA, int, int, int, int );
void _ShiftWindow( LPWDATA, int );
void _DisplayLineInWindowWithColor( LPWDATA, int, LPSTR, int, int, int, int );
void _DisplayLineInWindow( LPWDATA, int, LPSTR );
void _ClearWindow( LPWDATA );

/* windlg.c */
void _GetClearInterval( void );
void _DoAbout( void );

#if !defined( __OS2__ )
/* windrvr.c */
long CALLBACK _MainDriver( HWND hwnd, UINT message, UINT wparam, LONG lparam );
int _SetAboutDlg( char *, char * );
#endif

/* wingen.c */
LPWDATA _GetWindowData( HWND );
LPWDATA _GetActiveWindowData( void );
void _MakeWindowActive( LPWDATA w );
LPWDATA _IsWindowedHandle( int handle );
void _InitFunctionPointers( void );
void _InitMainWindowData( HANDLE );
LPWDATA _AnotherWindowData( HWND hwnd, va_list al );
void _DestroyAWindow( LPWDATA );
void _GetWindowNameAndCoords( char *name, char *dest, int *x1, int *x2, int *y1, int *y2 );
void _WindowsExit( void );
int _DestroyOnClose( LPWDATA );
int _YieldControl( void );

/* winio.c */
unsigned _DoStdin( LPWDATA, void *, unsigned );
unsigned _DoStdout( LPWDATA, const void *, unsigned );
unsigned _DoKbhit( LPWDATA );
unsigned _DoGetch( LPWDATA );
unsigned _DoGetche( LPWDATA );
void _DoPutch( LPWDATA, unsigned );

/* winkey.c */
void _WindowsKeyUp( WORD, WORD );
void _WindowsKeyPush( WORD, WORD );
void _WindowsVirtualKeyPush( WORD, WORD );
int _KeyboardHit( BOOL );
int _GetKeyboard( int * );
int _GetString( LPWDATA, char *, int );

/* winlines.c */
void _AddLine( LPWDATA, const void *, unsigned );
DWORD _GetLineFromThumbPosition( LPWDATA, WORD );
void _PositionScrollThumb( LPWDATA );
LPLDATA _GetLineDataPointer( LPWDATA, DWORD );
void _FreeAllLines( LPWDATA );
void _SaveAllLines( LPWDATA );
void _CopyAllLines( LPWDATA );
int _UpdateInputLine( LPWDATA, char *, unsigned, BOOL );
DWORD _GetLastLineNumber( LPWDATA w );

/* winmisc.c/pmmisc.c */
#if !defined( __OS2__ )
HFONT _SetMyDC( HDC, DWORD, DWORD ) ;
#else
extern void _Error( HWND hwndDlg, char *caption, char *msg );
#endif
int _MessageLoop( BOOL );
int _BlockingMessageLoop( BOOL );
void _WCI86FAR *_MemAlloc( unsigned );
void _WCI86FAR *_MemReAlloc( void _WCI86FAR *ptr, unsigned size );
void _MemFree( void _WCI86FAR * );
void _NewCursor( LPWDATA, cursors );
void _DisplayCursor( LPWDATA w );
void _SetInputMode( LPWDATA, BOOL );
void _ShowWindowActive( LPWDATA w, LPWDATA last );
void _OutOfMemory( void );
void _ExecutionComplete( void );
int _SetAppTitle( char * );
int _SetConTitle( LPWDATA, char *);
int _ShutDown( void );

/* winmove.c */
void _MovePageUp( LPWDATA );
void _MovePageDown( LPWDATA );
void _MoveLineUp( LPWDATA );
void _MoveLineDown( LPWDATA );
void _MoveToLine( LPWDATA, DWORD, BOOL );

/* winnew.c/pmmnew.c */
unsigned _NewWindow( char *name, ... );
int _CloseWindow( LPWDATA );
void _ReleaseWindowResources( LPWDATA w );

/* winpaint.c */
#if defined( __OS2__ )
void _RepaintWindow( LPWDATA, PRECT, HPS );
#else
void _RepaintWindow( LPWDATA, PRECT, HDC );
#endif
