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


#include <stdio.h>

#ifdef __SNOW__
    extern void DoTheWrite( void *, int );
    #pragma aux DoTheWrite = \
            0xBA 0xDA 0x03  /*          mov  dx, 03dah */ \
            0xEC            /* retest:  in   al, dx */ \
            0xA8 0x08       /*          test al, 8 */ \
            0x74 0xFB       /*          jz   short retest */ \
            0x26 0x89 0x1D  /*          mov  word ptr es:[di], bx */ \
        parm [es di] [bx] modify [ax dx];

    #define WRITE_SCREEN( a, b ) DoTheWrite( &(a), b )
#else
    #ifdef __CURSES__
        #define __VIO__
    #endif
    #define WRITE_SCREEN( a, b )    (*(cinfo_type _FAR *)&(a)) = (*(cinfo_type*)(&b))
    #define WRITE_SCREEN2( a, b )   (*(cinfo_type _FAR *)&(a)) = (*(cinfo_type _FAR *)(&b))
#endif
#define WRITE_SCREEN_DATA( a, b )   (*(cinfo_type*)&(a)) = (*(cinfo_type*)(&b))

/*
 * window structure
 */
typedef struct wind {
    vi_color    border_color1, border_color2;
    vi_color    text_color, background_color;
    short       x1, y1, x2, y2, width, height;
    char        *text;
    char        *overlap;
    char        *whooverlapping;
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
    char        overcnt[1];
} wind;
#define WIND_SIZE sizeof( wind )

#ifndef __NT__
typedef struct {
    char ch;
    char attr;
} char_info;
typedef unsigned short cinfo_type;
#else
typedef struct {
    unsigned short ch;
    unsigned short attr;
} char_info;
typedef unsigned long cinfo_type;
#endif

#define THUMB_START         2
#define NORMAL_ATTR         7
#define MAX_WINDS           250
#define NO_CHAR             (char) MAX_WINDS + 1
#define MAX_MIN_SLOTS       40
#define WIND_TOP_BORDER     0
#define WIND_BOTTOM_BORDER  1

enum border_char {
#undef vi_pick
#define vi_pick( enum, UnixNG, UnixG, DosNG, DosG ) enum,
#include "borders.h"
#undef vi_pick
};

#ifndef __VIO__
    #define MAKE_ATTR( w, a, b )        (unsigned short) ((a) + (b) * 16)
#else
    #ifdef __CURSES__
        extern unsigned short WindowAttr( wind *, vi_color, vi_color );
        #define MAKE_ATTR( w, a, b )    ((unsigned short) WindowAttr( w, a, b ))
    #else
        #define MAKE_ATTR( w, a, b )    (unsigned short) ((a) + ((b) & 7) * 16)
    #endif
#endif

extern wind         *Windows[MAX_WINDS];
extern char_info    WindowNormalAttribute;
extern char         *GadgetString;
extern char         WindowBordersNG[];
extern char         WindowBordersG[];
extern char         _FAR *Scrn;
extern char         _FAR *ClockStart;
extern char         _FAR *SpinLoc;
extern char         *ScreenImage;
extern char         MinSlots[MAX_MIN_SLOTS];

#include "winaux.h"

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
extern void     WindowBorderData( window_id, char *, int );
extern void     SetGadgetString( char *str );

/* windisp.c */
extern vi_rc    DisplayLineInWindowWithColor( window_id, int, char *, type_style *, int );
#ifndef __WIN__
extern vi_rc    DisplayLineInWindowWithSyntaxStyle( window_id, int, line *, linenum, char *, int, unsigned int );
#endif
extern void     DisplayCrossLineInWindow( window_id, int );
extern void     HiliteAColumnRange( linenum, int, int );
extern void     ColorAColumnRange( int, int, int, type_style * );
extern vi_rc    SetCharInWindowWithColor( window_id, int, int, char, type_style * );
extern vi_rc    DisplayLineInWindow( window_id, int, char * );

/* wininit.c */
extern void     StartWindows( void );
extern void     FinishWindows( void );

/* winnew.c */
extern vi_rc        ResetWindow( window_id * );
extern bool         ValidDimension( int, int, int, int, bool );
extern window_id    GimmeWindow( void );
extern wind         *AllocWindow( int, int, int, int, bool, vi_color, vi_color, vi_color, vi_color );
extern vi_rc        NewWindow( window_id *, int, int, int, int, bool, vi_color, vi_color, type_style * );
extern void         FreeWindow( wind * );
extern void         CloseAWindow( window_id );

/* window.c */
extern void CloseAChildWindow( window_id );

/* winover.c */
extern void         ResetOverlap( wind * );
extern void         MarkOverlap( window_id );
extern void         RestoreOverlap( window_id, bool );
extern bool         TestOverlap( window_id );
extern bool         TestVisible( wind * );
extern bool         WindowIsVisible( window_id );
extern window_id    WhoIsUnder( int *, int * );

/* winscrl.c */
extern void ShiftWindowUpDown( window_id, int );

/* winshow.c */
extern void MoveWindowToFront( window_id );
extern void MoveWindowToFrontDammit( window_id, bool );
extern void InactiveWindow( window_id );
extern void ActiveWindow( window_id );
extern void WindowTitleAOI( window_id wn, char *title, bool active );
extern void WindowTitleInactive( window_id, char * );
extern void WindowTitle( window_id, char * );
extern void ClearWindow( window_id );
extern void SetGenericWindowCursor( window_id, int, int );
extern void SetCursorOnLine( window_id, int, char *, type_style * );
extern void ResetEditWindowCursor( window_id );
extern bool InsideWindow( window_id, int, int );
extern void MyShowCaret( window_id );
extern void MyHideCaret( window_id );
extern void MyKillCaret( window_id );
extern void MyRaiseCaret( window_id );

/* winsize.c */
extern vi_rc    ResizeWindow( window_id, int, int, int, int, int );
extern vi_rc    ResizeWindowRelative( window_id, int, int, int, int, int );
extern vi_rc    MinimizeCurrentWindow( void );
extern vi_rc    MaximizeCurrentWindow( void );

/* winswap.c */
extern void SwapAllWindows( void );
extern wind *AccessWindow( window_id );
extern void ReleaseWindow( wind *);
extern void WindowSwapFileClose( void );

/* winthumb.c */
extern void     PositionVerticalScrollThumb( window_id wn, linenum curr, linenum last );
extern void     PositionHorizontalScrollThumb( window_id, int );
extern void     DrawVerticalThumb( wind *w, char ch );
extern vi_rc    PositionToNewThumbPosition( wind *w, int thumb );

/* wintica.c */
extern vi_rc    WindowTile( int, int );
extern vi_rc    WindowCascade( void );

/* filesel.c (Windows only functions) */
extern vi_rc     SelectFileSave( char * );
