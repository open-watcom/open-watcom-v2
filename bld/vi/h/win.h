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
* Description:  Screen output and windowing interface.
*
****************************************************************************/


#include "winaux.h"

#ifndef __WIN__

#include <stdio.h>

#define WRITE_SCREEN( a, b )        (*(char_info _FAR *)&(a)) = (*(char_info *)(&b))
#define WRITE_SCREEN2( a, b )       (*(char_info _FAR *)&(a)) = (*(char_info _FAR *)(&b))
#define WRITE_SCREEN_DATA( a, b )   (*(char_info *)&(a)) = (*(char_info *)(&b))

/*
 * character info
 */
#if defined( __NT__ )
typedef CHAR_INFO char_info;
#define cinfo_char  Char.AsciiChar
#define cinfo_wchar Char.UnicodeChar
#define cinfo_attr  Attributes
#else
typedef struct char_info {
    char            _char;
    unsigned char   _attr;
} char_info;
#define cinfo_char  _char
#define cinfo_attr  _attr
#endif

/*
 * window structure
 */
typedef struct wind {
    vi_color    border_color1, border_color2;
    vi_color    text_color, background_color;
    short       x1, y1, x2, y2, width, height;
    char_info   *text;
    window_id   *overlap;
    window_id   *whooverlapping;
    short       text_lines, text_cols;
    char        *title;
    char        *borderdata;
    short       bordercol;
    window_id   id;
    unsigned    isswapped           : 1;
    unsigned    accessed            : 1;
    unsigned    has_border          : 1;
    unsigned    has_gadgets         : 1;
    unsigned    min_slot            : 1;
    unsigned    has_scroll_gadgets  : 1;
    unsigned    spare7              : 1;
    unsigned    spare8              : 1;
    int         vert_scroll_pos;
    signed char overcnt[1];
} wind;

extern wind         *Windows[MAX_WINDS];

#define THUMB_START         2
#define NORMAL_ATTR         7
#define MAX_MIN_SLOTS       40
#define WIND_TOP_BORDER     0
#define WIND_BOTTOM_BORDER  1

#ifndef __VIO__
    #define MAKE_ATTR( w, a, b )        (viattr_t)( (a) + (b) * 16 )
#else
    #ifdef __CURSES__
        #define MAKE_ATTR( w, a, b )    WindowAttr( w, a, b )
    #else
        #define MAKE_ATTR( w, a, b )    (viattr_t)( (a) + ((b) & 7) * 16 )
    #endif
#endif

#ifdef __NT__
extern HANDLE       InputHandle, OutputHandle;
#endif

extern char_info    WindowNormalAttribute;
extern char         WindowBordersNG[];
extern char         WindowBordersG[];
extern char_info    _FAR *Scrn;
extern char_info    _FAR *ClockStart;
extern char_info    _FAR *SpinLoc;
extern window_id    *ScreenImage;
extern char         MinSlots[MAX_MIN_SLOTS];

#endif

#ifdef __WIN__

/* win/window.c */
extern void     DefaultWindows( RECT *world, RECT *workspace );
extern void     InitWindows( void );
extern void     FiniWindows( void );
extern int      WindowAuxInfo( window_id id, int type );
extern vi_rc    NewWindow2( window_id *id, window_info *info );
extern void     CloseAWindow( window_id id );
extern void     CloseAChildWindow( window_id id );
extern bool     InsideWindow( window_id id, int x, int y );
extern void     InactiveWindow( window_id id );
extern void     ActiveWindow( window_id id );
extern void     MoveWindowToFront( window_id id );
extern void     MoveWindowToFrontDammit( window_id id, bool );
extern vi_rc    MaximizeCurrentWindow( void );
extern vi_rc    MinimizeCurrentWindow( void );
extern void     FinishWindows( void );

/* win/display.c */
extern void     ScreenPage( int page );
extern void     WindowTitleAOI( window_id id, const char *title, bool active );
extern void     WindowTitle( window_id id, const char *title );
extern void     ClearWindow( window_id id );
extern vi_rc    DisplayLineInWindow( window_id id, int line, char *text );
extern void     ShiftWindowUpDown( window_id id, int lines );
extern bool     SetDrawingObjects( HDC hdc, type_style *ts );
extern void     MyTabbedTextOut( HDC, char **, int, bool, POINT *, type_style *, RECT *, window_id, char *, int );
extern int      DisplayLineInWindowWithSyntaxStyle( window_id, int, line *, linenum, char *, int, HDC );

/* win/stubs.c */
extern void     RedrawMouse( int p1, int p2 );
extern bool     DisplayMouse( bool p1 );
extern void     TurnOffCapsLock( void );
extern vi_rc    HandleMouseEvent( void );
extern void     SwapAllWindows( void );
extern void     SetMouseSpeed( int i );
extern void     GetClockStart( void );
extern void     GetSpinStart( void );
extern void     WindowAuxUpdate( window_id id, int x, int y );
extern void     DrawBorder( window_id id );
extern void     PushMouseEventHandler( mouse_callback cb );
extern void     PopMouseEventHandler( void );
extern void     WindowBorderData( window_id id, const char *c, int x );
extern vi_rc    ResizeWindowRelative( window_id id, int p1, int p2, int p3, int p4, bool flags );
extern vi_rc    ResizeWindow( window_id id, int p1, int p2, int p3, int p4, bool flags );
extern void     RestoreInterrupts( void );
extern void     WindowSwapFileClose( void );
extern void     FiniMouse( void );
extern void     ScreenFini( void );
extern vi_rc    ResizeCurrentWindowWithKeys( void );
extern vi_rc    MoveCurrentWindowWithKeys( void );
extern drive_type DoGetDriveType( int i );
extern void     ClearScreen( void );
extern vi_rc    ResetWindow( window_id *id );
extern bool     WindowIsVisible( window_id id );
extern void     ScreenInit( void );
extern void     SetInterrupts( void );
extern void     ChkExtendedKbd( void );
extern void     InitMouse( void );
extern void     SetBorderGadgets( window_id id, bool how );
extern void     NewWindow( void );
extern vi_rc    GetNewValueDialog( char * );
extern void     DisplayCrossLineInWindow( window_id id, int line );
extern int      SetCharInWindowWithColor( window_id wn, int line, int col, char text, type_style *style );
extern void     DisplayLineWithHilite( window_id id, int line, char *text, int start, int end, int ignore );
extern void     SetPosToMessageLine( void );
extern void     HideCursor( void );

/* win/editwnd.c */
extern void     PositionVerticalScrollThumb( window_id id, linenum top, linenum last );
extern void     PositionHorizontalScrollThumb( window_id id, int left );

/* win/wintica.c */
extern vi_rc    WindowTile( int, int );
extern vi_rc    WindowCascade( void );

/* win/cursor.c */
extern void     SetCursorOnLine( window_id id, int col, char *str, type_style *style );
extern void     SetGenericWindowCursor( window_id id, int row, int col );
extern void     ResetEditWindowCursor( window_id id );
extern void     MyShowCaret( window_id id );
extern void     MyHideCaret( window_id id );
extern void     MyKillCaret( window_id id );
extern void     MyRaiseCaret( window_id id );

/* win/main.c */
extern void     StartWindows( void );

/* win/utils.c */
extern void     SetGadgetString( char *str );

/* win/repcnt.c */
extern void     UpdateRepeatString( char *str );

/* win/clipbrd.c */
extern int      AddLineToClipboard( char *data, int scol, int ecol );
extern int      AddFcbsToClipboard( fcb_list *fcbs );
extern int      GetClipboardSavebuf( savebuf *clip );
extern bool     IsClipboardEmpty( void );

#else

/* adjwin.c */
extern vi_rc    ResizeCurrentWindowWithKeys( void );
extern vi_rc    MoveCurrentWindowWithKeys( void );
extern vi_rc    ResizeCurrentWindowWithMouse( void );
extern vi_rc    MoveCurrentWindowWithMouse( void );

/* winaux.c */
extern int      WindowAuxInfo( window_id, int );
extern void     WindowAuxUpdate( window_id, int, int );

/* winbrdr.c */
extern void     DrawBorder( window_id );
extern void     SetBorderGadgets( window_id, bool );
extern void     WindowBorderData( window_id, const char *, int );
extern void     SetGadgetString( char *str );

/* windisp.c */
extern vi_rc    DisplayLineInWindowWithColor( window_id, int, char *, type_style *, int );
extern vi_rc    DisplayLineInWindowWithSyntaxStyle( window_id, int, line *, linenum, char *, int, unsigned int );
extern void     DisplayCrossLineInWindow( window_id, int );
extern void     HiliteAColumnRange( linenum, int, int );
extern void     ColorAColumnRange( int, int, int, type_style * );
extern vi_rc    SetCharInWindowWithColor( window_id, int, int, char, type_style * );
extern vi_rc    DisplayLineInWindow( window_id, int, char * );

/* ui/wininit.c */
extern void     StartWindows( void );
extern void     FinishWindows( void );

/* winnew.c */
extern vi_rc        ResetWindow( window_id * );
extern bool         ValidDimension( int, int, int, int, bool );
extern window_id    GimmeWindow( void );
extern wind         *AllocWindow( window_id, int, int, int, int, bool, bool, bool, vi_color, vi_color, vi_color, vi_color );
extern void         FreeWindow( wind * );
extern vi_rc        NewWindow( window_id *, int, int, int, int, bool, vi_color, vi_color, type_style * );
extern void         CloseAWindow( window_id );

/* window.c */
extern void     CloseAChildWindow( window_id );

/* winover.c */
extern void         ResetOverlap( wind * );
extern bool         TestVisible( wind * );
extern void         MarkOverlap( window_id );
extern void         RestoreOverlap( window_id, bool );
extern bool         TestOverlap( window_id );
extern bool         WindowIsVisible( window_id );
extern window_id    WhoIsUnder( int *, int * );

/* winscrl.c */
extern void     ShiftWindowUpDown( window_id, int );

/* winsize.c */
extern vi_rc    ResizeWindow( window_id, int, int, int, int, bool );
extern vi_rc    ResizeWindowRelative( window_id, int, int, int, int, bool );
extern vi_rc    MinimizeCurrentWindow( void );
extern vi_rc    MaximizeCurrentWindow( void );

/* winswap.c */
extern void     SwapAllWindows( void );
extern wind     *AccessWindow( window_id );
extern void     ReleaseWindow( wind *);
extern void     WindowSwapFileClose( void );

/* winthumb.c */
extern void     PositionVerticalScrollThumb( window_id wn, linenum curr, linenum last );
extern void     PositionHorizontalScrollThumb( window_id, int );
extern void     DrawVerticalThumb( wind *w, char ch );
extern vi_rc    PositionToNewThumbPosition( wind *w, int thumb );

/* ui/wintica.c */
extern vi_rc    WindowTile( int, int );
extern vi_rc    WindowCascade( void );

/* filesel.c (Windows only functions) */
extern vi_rc    SelectFileSave( char * );

/* ui/winshow.c */
extern void     MoveWindowToFront( window_id );
extern void     MoveWindowToFrontDammit( window_id, bool );
extern void     InactiveWindow( window_id );
extern void     ActiveWindow( window_id );
extern void     WindowTitleAOI( window_id wn, const char *title, bool active );
extern void     WindowTitleInactive( window_id, const char * );
extern void     WindowTitle( window_id, const char * );
extern void     ClearWindow( window_id );
extern void     SetGenericWindowCursor( window_id, int, int );
extern void     SetCursorOnLine( window_id, int, char *, type_style * );
extern void     ResetEditWindowCursor( window_id );
extern bool     InsideWindow( window_id, int, int );
extern void     MyShowCaret( window_id );
extern void     MyHideCaret( window_id );
extern void     MyKillCaret( window_id );
extern void     MyRaiseCaret( window_id );

extern viattr_t WindowAttr( wind *, vi_color, vi_color );

#endif
