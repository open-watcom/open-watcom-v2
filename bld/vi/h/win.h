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


#include <stdio.h>

#ifdef __SNOW__
    extern void DoTheWrite( void *, int );
    #pragma aux DoTheWrite = \
     0xBA 0xDA 0x03 /* mov  dx,03dah */ \
     0xEC           /* retest: in   al,dx */ \
     0xA8 0x08      /* test al,8 */ \
     0x74 0xFB      /* jz   short retest */ \
     0x26 0x89 0x1D /* mov  word ptr es:[di],bx */ \
        parm [es di] [bx] modify [ax dx];

    #define WRITE_SCREEN( a,b ) DoTheWrite( &(a), b )
#else
    #ifdef __CURSES__
        #define __VIO__
    #endif
    #define WRITE_SCREEN( a,b ) (*(cinfo_type _FAR *)&(a)) = (*(cinfo_type*)(&b))
    #define WRITE_SCREEN2( a,b ) (*(cinfo_type _FAR *)&(a)) = (*(cinfo_type _FAR *)(&b))
#endif
#define WRITE_SCREEN_DATA( a,b ) (*(cinfo_type*)&(a)) = (*(cinfo_type*)(&b))

/*
 * window structure
 */
typedef struct wind {
    short       border_color1,border_color2;
    short       text_color,background_color;
    short       x1,y1,x2,y2,width,height;
    char        *text;
    char        *overlap;
    char        *whooverlapping;
    short       text_lines,text_cols;
    char        *title;
    char        *borderdata;
    short       bordercol;
    window_id   id;
    unsigned    isswapped:1,accessed:1,has_border:1,has_gadgets:1,min_slot:1;
    unsigned    has_scroll_gadgets:1;
    unsigned    spare7:1,spare8:1;
    int         vert_scroll_pos;
    char        overcnt[1];
} wind;
#define WIND_SIZE sizeof( wind )

#ifndef __NT__
#pragma pack(1);
typedef struct {
    char ch;
    char attr;
} char_info;
#pragma pack();
typedef unsigned short cinfo_type;
#else
typedef struct {
    unsigned short ch;
    unsigned short attr;
} char_info;
typedef unsigned long cinfo_type;
#endif

#define THUMB_START     2
#define NORMAL_ATTR     7
#define MAX_WINDS       250
#define NO_CHAR         (char) MAX_WINDS+1
#define MAX_MIN_SLOTS   40
#define WIND_TOP_BORDER 0
#define WIND_BOTTOM_BORDER 1

#define WB_TOPLEFT      0
#define WB_TOPRIGHT     1
#define WB_BOTTOMLEFT   2
#define WB_BOTTOMRIGHT  3
#define WB_LEFTSIDE     4
#define WB_TOPBOTTOM    5
#define WB_RIGHTT       6
#define WB_LEFTT        7
#define WB_UPTRIANGLE   8
#define WB_DOWNTRIANGLE 9
#define WB_RIGHTSIDE    10
#define WB_THUMB        11

#ifndef __VIO__
    #define MAKE_ATTR( w,a,b ) (unsigned short) ((a)+(b)*16)
#else
    #ifdef __CURSES__
        extern unsigned short WindowAttr( wind *, short, short );
        #define MAKE_ATTR( w,a,b ) ((unsigned short)WindowAttr( w,a,b ))
    #else
        #define MAKE_ATTR( w,a,b ) (unsigned short) ((a)+((b)&7)*16)
    #endif
#endif

extern wind *Windows[ MAX_WINDS ];
extern char_info WindowNormalAttribute;
extern char *GadgetString;
extern char WindowBordersNG[];
extern char WindowBordersG[];
extern char _FAR *Scrn, _FAR *ClockStart, _FAR *SpinLoc;
extern char *ScreenImage;
extern char MinSlots[MAX_MIN_SLOTS];

#include "winaux.h"

/* adjwin.c */
extern int ResizeCurrentWindowWithKeys(void );
extern int MoveCurrentWindowWithKeys(void );
extern int ResizeCurrentWindowWithMouse(void );
extern int MoveCurrentWindowWithMouse(void );

/* winaux.c */
extern int WindowAuxInfo(window_id ,int );
extern void WindowAuxUpdate(window_id ,int ,int );

/* winbrdr.c */
extern void DrawBorder(window_id );
extern void SetBorderGadgets(window_id ,bool );
extern void WindowBorderData(window_id ,char *,int );
extern void SetGadgetString( char *str );

/* windisp.c */
extern int DisplayLineInWindowWithColor(window_id ,int, char *, type_style *,int );
#ifndef __WIN__
    int DisplayLineInWindowWithSyntaxStyle( window_id, int, line *, linenum, char *, int, unsigned int );
#endif
extern void DisplayCrossLineInWindow(window_id ,int );
extern void HiliteAColumnRange(linenum ,int ,int );
extern void ColorAColumnRange(int ,int ,int , type_style *);
extern int SetCharInWindowWithColor(window_id , int, int, char , type_style *);
extern int DisplayLineInWindow(window_id ,int ,char *);

/* wininit.c */
extern void StartWindows(void );
extern void FinishWindows(void );

/* winnew.c */
extern int ResetWindow(window_id *);
extern bool ValidDimension(int ,int ,int ,int ,bool );
extern window_id GimmeWindow(void );
extern wind *AllocWindow(int ,int ,int ,int ,bool ,int ,int ,int ,int );
extern int NewWindow(window_id *,int ,int ,int ,int ,bool ,int ,int , type_style *);
extern void FreeWindow(wind *);
extern void CloseAWindow(window_id );

/* window.c */
extern void CloseAChildWindow(window_id );

/* winover.c */
extern void ResetOverlap(wind *);
extern void MarkOverlap(window_id );
extern void RestoreOverlap(window_id ,bool );
extern bool TestOverlap(window_id );
extern bool TestVisible(wind *);
extern bool WindowIsVisible( window_id );
extern window_id WhoIsUnder(int *,int *);

/* winscrl.c */
extern void ShiftWindowUpDown(window_id ,int );

/* winshow.c */
extern void MoveWindowToFront(window_id );
extern void MoveWindowToFrontDammit(window_id ,bool );
extern void InactiveWindow(window_id );
extern void ActiveWindow(window_id );
extern void WindowTitleAOI( window_id wn, char *title, bool active );
extern void WindowTitleInactive(window_id ,char *);
extern void WindowTitle(window_id ,char *);
extern void ClearWindow(window_id );
extern void SetGenericWindowCursor(window_id ,int ,int );
extern void SetCursorOnLine(window_id ,int ,char *, type_style *);
extern void ResetEditWindowCursor(window_id );
extern bool InsideWindow(window_id ,int ,int );
extern void MyShowCaret(window_id );
extern void MyHideCaret(window_id );
extern void MyKillCaret(window_id );
extern void MyRaiseCaret(window_id );

/* winsize.c */
extern int ResizeWindow(window_id ,int ,int ,int ,int ,int );
extern int ResizeWindowRelative(window_id ,int ,int ,int ,int ,int );
extern int MinimizeCurrentWindow(void );
extern int MaximizeCurrentWindow(void );

/* winswap.c */
extern void SwapAllWindows(void );
extern wind *AccessWindow(window_id );
extern void ReleaseWindow(wind *);
extern void WindowSwapFileClose(void );

/* winthumb.c */
void PositionVerticalScrollThumb( window_id wn, linenum curr, linenum last );
void PositionHorizontalScrollThumb( window_id, int );
void DrawVerticalThumb( wind *w, char ch );
int PositionToNewThumbPosition( wind *w, int thumb );

/* wintica.c */
extern int WindowTile(int ,int );
extern int WindowCascade(void );

/* filesel.c (Windows only functions) */
int SelectFileSave( char * );

