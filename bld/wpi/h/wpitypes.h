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


#ifndef WPITYPE_H
#define WPITYPE_H
/*
 Description:
 ============
    Some stuff for windows development.
*/

#include <setjmp.h>
/*
 * For developers concerned about NT issues, win1632.h is included here
 * to macro from windows to nt.
 */
#if defined( __OS2_PM__ ) || defined( __OS2__ )
#include "pm1632.h"
#else
#include "win1632.h"
#endif

#ifdef __WINDOWS_386__
    #define _W386FAR                    __far
#else
    #define _W386FAR
#endif

#define WPI_SIZEWINBMPFILEHDR           14
#define WPI_SIZEWINBMPINFOHDR           40
#define WPI_SIZEOS2BMPFILEHDR           78
#define WPI_SIZEOS2BMPINFOHDR           64

#pragma pack( 1 )
/*************************/
/* new types - both ways */
/*************************/

#if defined( __OS2_PM__ ) || defined( __OS2__ )
    // PM Side
    #define WPI_ERROR_ON_CREATE         1
    #define WPI_ERRORID                 ERRORID
    #define WPI_MRESULT                 MRESULT
    #define WPI_MINMAXINFO              TRACKINFO
    #define WPI_CLASSPROC               PFNWP
    #define WPI_HICON                   LONG
    #define WPI_HCURSOR                 LONG
    #define WPI_TASK                    TID
    #define WPI_CATCHBUF                jmp_buf
    #define WPI_HLIB                    HLIB
    #define WPI_BITMAPINFO              PM1632_BITMAPINFO2
    #define WPI_BITMAP                  PM1632_BITMAPINFOHEADER2
    #define WPI_BITMAPINFOHEADER        PM1632_BITMAPINFOHEADER2
    #define WPI_BITMAPFILEHEADER        BITMAPFILEHEADER
    #define WPI_BMPBITS                 PBYTE
    #define WPI_TEXTMETRIC              FONTMETRICS
    #define WPI_LPTEXTMETRIC            FONTMETRICS*
    #define WPI_LOGFONT                 FONTMETRICS
    #define WPI_LPLOGFONT               FONTMETRICS*
    #define WPI_FONT                    FATTRS*
    #define WPI_F_FONT                  wpi_f_font
    #define WPI_PARAM1                  MPARAM
    #define WPI_PARAM2                  MPARAM
    #define WPI_HACCEL                  HACCEL
    #define WPI_MSG                     PM1632_WINDOW_MSG
    #define WPI_QMSG                    QMSG
    #define WPI_WNDPROC                 PFNWP 
    #define WPI_PROC                    PFNWP
    #define WPI_PRES                    HPS
    #define WPI_POINT                   POINTL
    #define WPI_PPOINT                  PPOINTL
    #define WPI_LPPOINT                 PPOINTL
    #define WPI_PRECT                   PRECTL
    #define WPI_RECT                    RECTL
    #define WPI_RECTDIM                 LONG
    #define WPI_COLOUR                  COLOR
    #define WPI_RGBQUAD                 PM1632_RGB2
    #define WPI_INT2ULONG               ULONG
    #define WPI_DLGRESULT               MRESULT
    #define WPI_DLGRESULT2              MRESULT
    typedef void (__export APIENTRY *WPI_LINEDDAPROC) ( int, int, WPI_PARAM2 );
    typedef BOOL (__export APIENTRY *WPI_ENUMPROC) ( HWND, LONG );
    typedef int (__export APIENTRY *WPI_ENUMFONTPROC) ( WPI_LPLOGFONT,
                                                        WPI_LPTEXTMETRIC,
                                                        unsigned int, PSZ );
    typedef struct WPI_INST {
        HAB                     hab;
        HMODULE                 mod_handle;
    } WPI_INST;
    #define WPI_MENUITEM                MENUITEM
    #define WPI_MENUSTATE               MENUITEM

    #define WC_GROUPBOX                 WC_STATIC
#else
    #define WPI_ERROR_ON_CREATE         -1
    #define WPI_ERRORID                 int
    #define WPI_MRESULT                 LONG
    #define WPI_MINMAXINFO              MINMAXINFO
    #define WPI_CLASSPROC               WNDPROC
    #define WPI_HICON                   HICON
    #define WPI_HCURSOR                 HCURSOR
    #define WPI_TASK                    HANDLE
    #define WPI_CATCHBUF                CATCHBUF
    #define WPI_BITMAP                  BITMAP
    #define WPI_BITMAPINFO              BITMAPINFO
    #define WPI_BITMAPINFOHEADER        BITMAPINFOHEADER
    #define WPI_BITMAPFILEHEADER        BITMAPFILEHEADER
    #define WPI_BMPBITS                 LPSTR
    #define WPI_HLIB                    HANDLE
    #define WPI_TEXTMETRIC              TEXTMETRIC
    #define WPI_LPTEXTMETRIC            LPTEXTMETRIC
    #define WPI_LOGFONT                 LOGFONT
    #define WPI_LPLOGFONT               LPLOGFONT
    #define WPI_FONT                    HFONT
    #define WPI_F_FONT                  LOGFONT
    #define WPI_PARAM1                  WPARAM
    #define WPI_PARAM2                  LPARAM
    #define WPI_INST                    HINSTANCE
    #define WPI_HACCEL                  HANDLE
    #define WPI_MSG                     UINT
    #define WPI_QMSG                    MSG
    #define WPI_PRES                    HDC
    #define WPI_POINT                   POINT
    #define WPI_PPOINT                  PPOINT
    #define WPI_LPPOINT                 LPPOINT
    #define WPI_RECT                    RECT
    #define WPI_PRECT                   LPRECT
    #define WPI_RECTDIM                 int
    #define WPI_COLOUR                  COLORREF
    #define WPI_WNDPROC                 WNDPROC
    #define WPI_PROC                    FARPROC
    #define WPI_ENUMFONTPROC            int CALLBACK
    #define WPI_LINEDDAPROC             LINEDDAPROC
    #define WPI_CHOOSEFONT              CHOOSEFONT
    #define WPI_LPCHOOSEFONT            LPCHOOSEFONT
    #define WPI_RGBQUAD                 RGBQUAD
    #define WPI_INT2ULONG               int
    #define WPI_DLGRESULT               int
    #define WPI_DLGRESULT2              BOOL
    #define WPI_ENUMPROC                WNDENUMPROC
    #define WPI_IDYES                   IDYES
    #define WPI_IDNO                    IDNO
    #define WPI_IDOK                    IDOK
    #define WPI_IDABORT                 IDABORT
    #define WPI_IDRETRY                 IDRETRY
    #define WPI_IDIGNORE                IDIGNORE
    #define WPI_IDCANCEL                IDCANCEL
    #define WPI_IDERROR                 0
    #define WPI_MENUSTATE               UINT
    #define WPI_HANDLE                  HANDLE
    #define WPI_WNDCLASS                WNDCLASS
    typedef struct WPI_MENUITEM {
        LONG            iPosition;
        unsigned long   afStyle;
        unsigned long   afAttribute;
        unsigned long   id;
        HWND            hwndSubMenu;
        unsigned long   hItem;
    } WPI_MENUITEM;
    #ifndef WC_BUTTON
        #define WC_BUTTON               "button"
    #endif
    #ifndef WC_COMBOBOX
        #define WC_COMBOBOX             "combobox"
    #endif
    #define WC_MLE                      "edit"
    #define WC_ENTRYFIELD               "edit"
    #ifndef WC_LISTBOX
        #define WC_LISTBOX              "listbox"
    #endif
    #ifndef WC_SCROLLBAR
        #define WC_SCROLLBAR            "scrollbar"
    #endif
    #ifndef WC_STATIC
        #define WC_STATIC               "static"
    #endif
    #define WC_GROUPBOX                 "button"
#endif

/*******************************/
/* types from one to the other */
/*******************************/

#if defined( __OS2_PM__ ) || defined( __OS2__ )
    #ifndef NULLHANDLE
    #define NULLHANDLE                  (LHANDLE) 0
    #endif
    #define HINSTANCE                   HAB
    #define HPALETTE                    HPAL
    #define GMEM_SHARE                  0
    #define WA_ACTIVE                   TRUE
    #define WA_INACTIVE                 FALSE
    #define STARTDOC                    DEVESC_STARTDOC
    #define ENDDOC                      DEVESC_ENDDOC
    #define NEWFRAME                    DEVESC_NEWFRAME
    #define DT_SINGLELINE               0
    #define DT_CALCRECT                 DT_QUERYEXTENT
    #define KEY_ASYNC_DOWN              0x8000
    #define KEY_ASYNC_PRESSED           0x0002
    #define SB_HORZ                     SBS_HORZ
    #define SB_VERT                     SBS_VERT
    #define SB_BOTH                     SBS_HORZ|SBS_VERT
    #define SB_CTL                      SBS_HORZ|SBS_VERT
    #define SB_THUMBPOSITION            SB_SLIDERPOSITION
    #define SB_THUMBTRACK               SB_SLIDERTRACK
    #define SB_TOP                      SB_SLIDERPOSITION
    #define SB_LEFT                     SB_SLIDERPOSITION
    #define SB_BOTTOM                   SB_SLIDERPOSITION
    #define SB_RIGHT                    SB_SLIDERPOSITION
    #define LPINT                       int*
    #define HCURSOR                     HPOINTER
    #define R2_NOT                      FM_INVERT
    #define R2_NOP                      FM_LEAVEALONE
    #define R2_BLACK                    FM_ZERO
    #define R2_WHITE                    FM_ONE
    #define R2_COPYPEN                  FM_OVERPAINT
    #define R2_NOTCOPYPEN               FM_NOTCOPYSRC
    #define R2_MERGEPENNOT              FM_MERGSRCNOT
    #define R2_XORPEN                   FM_XOR
    #define R2_MASKPEN                  FM_AND
    #define R2_MERGEPEN                 FM_OR
    #define HELP_CONTEXT                0x0001
    #define HELP_QUIT                   0x0002
    #define HELP_INDEX                  0x0003
    #define HELP_CONTENTS               0x0006
    #define HELP_HELPONHELP             0x0004
    #define HELP_SETINDEX               0x0005
    #define HELP_SETCONTENTS            0x0007
    #define HELP_CONTEXTPOPUP           0x0008
    #define HELP_FORCEFILE              0x0009
    #define HELP_KEY                    0x0101
    #define HELP_COMMAND                0x0102
    #define HELP_PARTIALKEY             0x0105
    #define HELP_MULTIKEY               0x0201
    #define HELP_SETWINPOS              0x0203

    /* Many Windows CS flags simply have equivalent in OS/2, or are done
       at Window creation time. Note the comments below. */
    #define CS_BYTEALIGNWINDOW          0       // see FCF_NOBYTEALIGN
    #define CS_CLASSDC                  0       // no OS/2 equiv.
    #define CS_DBLCLKS                  0       // no OS/2 equiv.
    #define CS_GLOBALCLASS              CS_PUBLIC
    #define CS_HREDRAW                  CS_SIZEREDRAW
    #define CS_NOCLOSE                  0       // no OS/2 equiv.
    #define CS_OWNDC                    0       // no OS/2 equiv.
    #define CS_PARENTDC                 0       // no OS/2 equiv.
    #define CS_VREDRAW                  CS_SIZEREDRAW

    #define WPI_VERT_MULT               (-1)
    #define HFONT                       LONG
    #define PAINTSTRUCT                 RECTL
    #define UINT                        unsigned int
    #define WPARAM                      MPARAM
    #define LPARAM                      MPARAM
    #define DWORD                       ULONG
    #define GLOBALHANDLE                ULONG *
    #ifndef HANDLE      // VX-REXX defines this (grrr...)
        #define HANDLE                  ULONG *
    #endif
    #define WORD                        USHORT
    #define OPENFILENAME                FILEDLG
    #define LPSTR                       PSZ
    #define LPVOID                      PVOID
    #define COMPLEXREGION               RGN_COMPLEX
    #define SIMPLEREGION                RGN_RECT
    #define MB_ICONSTOP                 MB_ICONHAND
    #define MB_ICONINFORMATION          MB_INFORMATION
    #define WPI_IDNO                    MBID_NO
    #define WPI_IDYES                   MBID_YES
    #define WPI_IDOK                    MBID_OK
    #define WPI_IDABORT                 MBID_ABORT
    #define WPI_IDRETRY                 MBID_RETRY
    #define WPI_IDIGNORE                MBID_IGNORE
    #define WPI_IDCANCEL                MBID_CANCEL
    #define WPI_IDERROR                 MBID_ERROR
    #define LOGFONT                     FONTMETRICS
    #define LPLOGFONT                   FONTMETRICS*
    #define HFILE_FORMAT                ULONG
    #define LMEM_MOVEABLE               0
    #define ANSI_CHARSET                0
    #define DEFAULT_CHARSET             1
    #define SYMBOL_CHARSET              2
    #define SHIFTJIS_CHARSET            128
    #define HANGEUL_CHARSET             129
    #define CHINESEBIG5_CHARSET         136
    #define OEM_CHARSET                 255

    #define DS_SETFONT                  0
    #define DS_MODALFRAME               FCF_DLGBORDER
    #define DS_SYSMODAL                 FCF_SYSMODAL

    /* The WS_* Windows styles map to mainly FCF_* styles */
    #define WS_BORDER                   FCF_BORDER
    #define WS_CAPTION                  FCF_TITLEBAR
    #define WS_CHILD                    0       // unnesc. in OS/2
    #define WS_CHILDWINDOW              WS_CHILD
    //#define WS_CLIPCHILDREN                   // defined in OS/2
    //#define WS_CLIPSIBLINGS                   // defined in OS/2
    //#define WS_DISABLED                       // defined in OS/2
    #define WS_DLGFRAME                 FCF_DLGBORDER
    //#define WS_GROUP                          // defined in OS/2
    #define WS_HSCROLL                  FCF_HORZSCROLL
    #define WS_MAXIMIZE                 WS_MAXIMIZED
    #define WS_MAXIMIZEBOX              FCF_MAXBUTTON
    #define WS_MINIMIZE                 WS_MINIMIZED
    #define WS_MINIMIZEBOX              FCF_MINBUTTON
    #define WS_OVERLAPPED               (FCF_BORDER | FCF_TITLEBAR)
    #define WS_OVERLAPPEDWINDOW         (FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX | FCF_MENU)
    #define WS_POPUP                    0       // unnesc. in OS/2
    #define WS_POPUPWINDOW              (FCF_BORDER | FCF_SYSMENU)
    #define WS_SYSMENU                  FCF_SYSMENU
    //#define WS_TABSTOP                        // defined in OS/2
    #define WS_THICKFRAME               FCF_SIZEBORDER
    //#define WS_VISIBLE                        // defined in OS/2
    #define WS_VSCROLL                  FCF_VERTSCROLL

    #define SS_CENTER                   SS_TEXT | DT_CENTER | DT_MNEMONIC
    #define SS_RIGHT                    SS_TEXT | DT_RIGHT | DT_MNEMONIC
    #define SS_LEFT                     SS_TEXT | DT_LEFT | DT_WORDBREAK | DT_MNEMONIC
    #define SS_LEFTNOWORDWRAP           SS_TEXT | DT_LEFT | DT_MNEMONIC
    #define SS_NOPREFIX                 0
    #define SS_WANTRETURN               0
    #define BS_DEFPUSHBUTTON            BS_PUSHBUTTON | BS_DEFAULT
    #define BS_GROUPBOX                 SS_GROUPBOX
    #define ES_AUTOHSCROLL              ES_AUTOSCROLL
    #define ES_MULTILINE                0
    #define ES_WANTRETURN               0
    #define LBS_NOINTEGRALHEIGHT        LS_NOADJUSTPOS
    #define LBS_NOTIFY                  0
    #define LBS_SORT                    0
    #define CBS_AUTOHSCROLL             0
    #define CBS_NOINTEGRALHEIGHT        0
    #define CBS_SORT                    0

    #define CW_USEDEFAULT               0
    #define HS_BDIAGONAL                PATSYM_DIAG1
    #define HS_HORIZONTAL               PATSYM_HORIZ
    #define HS_VERTICAL                 PATSYM_VERT
    #define HS_FDIAGONAL                PATSYM_DIAG3
    #define HS_CROSS                    PATSYM_DENSE7
    #define HS_DIAGCROSS                PATSYM_DENSE8
    #define TA_BASELINE                 TA_BASE
    #define TA_NOUPDATECP               0
    #define BI_RGB                      BCE_RGB
    #define PS_SOLID                    LINETYPE_SOLID
    #define PS_DASH                     LINETYPE_SHORTDASH
    #define PS_DOT                      LINETYPE_DOT
    #define PS_DASHDOT                  LINETYPE_DASHDOT
    #define PS_DASHDOTDOT               LINETYPE_DASHDOUBLEDOT
    #define PS_NULL                     LINETYPE_INVISIBLE
    #define PS_INSIDEFRAME              LINETYPE_SOLID
    #define BS_SOLID                    FM_OVERPAINT
    #define BS_NULL                     FM_LEAVEALONE
    #define BS_HOLLOW                   FM_LEAVEALONE
    #define BS_HATCHED                  FM_OVERPAINT
    #define BS_PATTERN                  FM_OVERPAINT
    #define FW_BOLD                     FATTR_SEL_BOLD
    #define FW_ITALIC                   FATTR_SEL_ITALIC
    #define FW_NORMAL                   0
    #define FW_STRIKEOUT                FATTR_SEL_STRIKEOUT
    #define FW_OUTLINE                  FATTR_SEL_OUTLINE
    #define GWL_STYLE                   QWL_STYLE
    #define NOT_RASTER_FONTTYPE         FM_DEFN_OUTLINE
    #define OF_READ                     OPEN_ACCESS_READONLY
    #define OF_WRITE                    OPEN_ACCESS_WRITEONLY
    #define OF_READWRITE                OPEN_ACCESS_READWRITE
    #define OF_SHARE_DENY_READ          OPEN_SHARE_DENYREAD
    #define OF_SHARE_DENY_WRITE         OPEN_SHARE_DENYWRITE
    #define OF_SHARE_DENY_NONE          OPEN_SHARE_DENYNONE
    #define OF_SHARE_EXCLUSIVE          OPEN_SHARE_DENYREADWRITE
    #define OF_EXIST                    FILE_OPEN
    #define OF_CREATE                   FILE_CREATE
    #define LIT_SELECT                  TRUE
    #define LIT_UNSELECT                FALSE
    #define CB_RESETCONTENT             LM_DELETEALL
    #define CB_ADDSTRING                LM_INSERTITEM
    #define CB_INSERTSTRING             LM_INSERTITEM
    #define CB_GETCOUNT                 LM_QUERYITEMCOUNT
    #define CB_ERR                      LIT_NONE
    #define CB_GETCURSEL                LM_QUERYSELECTION
    #define CB_SETCURSEL                LM_SELECTITEM
    #define CB_GETLBTEXT                LM_QUERYITEMTEXT
    #define CB_GETLBTEXTLEN             LM_QUERYITEMTEXTLENGTH
    #define CB_SHOWDROPDOWN             CBM_SHOWLIST
    #define CB_DELETESTRING             LM_DELETEITEM
    #define CB_SETITEMDATA              LM_SETITEMHANDLE
    #define CB_GETITEMDATA              LM_QUERYITEMHANDLE
    #define CBN_SELCHANGE               CBN_LBSELECT
    #define CBN_EDITCHANGE              CBN_EFCHANGE
    #define CBN_KILLFOCUS               CBN_ENTER
    #define LB_ERR                      LIT_NONE
    #define LB_DELETESTRING             LM_DELETEITEM
    #define LB_ADDSTRING                LM_INSERTITEM
    #define LB_INSERTSTRING             LM_INSERTITEM
    #define LB_GETTEXT                  LM_QUERYITEMTEXT
    #define LB_GETTEXTLEN               LM_QUERYITEMTEXTLENGTH
    #define LB_GETCURSEL                LM_QUERYSELECTION
    #define LB_RESETCONTENT             LM_DELETEALL
    #define LB_SETCURSEL                LM_SELECTITEM
    #define LB_SETSEL                   LM_SELECTITEM
    #define LB_GETSEL                   LM_QUERYSELECTION
    #define LB_SETTOPINDEX              LM_SETTOPINDEX
    #define LB_GETTOPINDEX              LM_QUERYTOPINDEX
    #define LB_GETCOUNT                 LM_QUERYITEMCOUNT
    #define LB_SETITEMDATA              LM_SETITEMHANDLE
    #define LB_GETITEMDATA              LM_QUERYITEMHANDLE
    #define LBN_SELCHANGE               LN_SELECT
    #define BN_DOUBLECLICKED            BN_DBLCLICKED
    #define VK_LBUTTON                  VK_BUTTON1
    #define VK_RBUTTON                  VK_BUTTON2
    #define VK_MBUTTON                  VK_BUTTON3
    #define VK_CONTROL                  VK_CTRL
    #define VK_BACK                     VK_BACKSPACE
    #define VK_ESCAPE                   VK_ESC
    #define VK_NUMPAD0                  VK_INSERT
    #define VK_NUMPAD1                  VK_END
    #define VK_NUMPAD2                  VK_DOWN
    #define VK_NUMPAD3                  VK_PAGEDOWN
    #define VK_NUMPAD4                  VK_LEFT
//there does not seem to be a virtual key code for numpad 5
    #define VK_NUMPAD6                  VK_RIGHT
    #define VK_NUMPAD7                  VK_HOME
    #define VK_NUMPAD8                  VK_UP
    #define VK_NUMPAD9                  VK_PAGEUP
    #define VK_CAPITAL                  VK_CAPSLOCK
    #define VK_SNAPSHOT                 VK_PRINTSCRN
    #define VK_ESCAPE                   VK_ESC
    #define VK_RETURN                   VK_NEWLINE
    #define VK_PRIOR                    VK_PAGEUP
    #define VK_NEXT                     VK_PAGEDOWN
    #define VK_DECIMAL                  VK_DELETE
    #define WM_KEYFIRST                 WM_CHAR
    #define WM_KEYLAST                  WM_CHAR
    #define WM_DLGCOMMAND               WM_CONTROL
    #define WM_WININICHANGE             PL_ALTERED
    #define WM_FONTCHANGE               WM_PRESPARAMCHANGED
    #define WM_LBUTTONUP                WM_BUTTON1UP
    #define WM_RBUTTONUP                WM_BUTTON2UP
    #define WM_MBUTTONUP                WM_BUTTON3UP
    #define WM_LBUTTONDOWN              WM_BUTTON1DOWN
    #define WM_RBUTTONDOWN              WM_BUTTON2DOWN
    #define WM_MBUTTONDOWN              WM_BUTTON3DOWN
    #define WM_ERASEBKGND               WM_ERASEBACKGROUND
    #define WM_GETMINMAXINFO            WM_QUERYTRACKINFO
    #define WM_INITMENUPOPUP            WM_INITMENU
    #define BM_GETCHECK                 BM_QUERYCHECK
    #define MK_LBUTTON                  KC_KEYUP
    #define DLGID_OK                    DID_OK
    #define DLGID_CANCEL                DID_CANCEL
    #define WM_NCDESTROY                WM_DESTROY
    #define WM_INITDIALOG               WM_INITDLG
    #define LOGPIXELSX                  CAPS_HORIZONTAL_RESOLUTION
    #define LOGPIXELSY                  CAPS_VERTICAL_RESOLUTION
    #define WPI_LOGPIXELSX_FONT         CAPS_HORIZONTAL_FONT_RES
    #define WPI_LOGPIXELSY_FONT         CAPS_VERTICAL_FONT_RES
    #define HORZRES                     CAPS_WIDTH
    #define VERTRES                     CAPS_HEIGHT
    #define NUMCOLORS                   CAPS_COLORS
    #define COLORREF                    COLOR
    #define OPAQUE_FORE                 FM_OVERPAINT
    #define OPAQUE                      BM_OVERPAINT
    #define TRANSPARENT_FORE            FM_LEAVEALONE
    #define TRANSPARENT                 BM_LEAVEALONE
    #define SIZENORMAL                  SWP_RESTORE
    #define SIZEFULLSCREEN              SWP_MAXIMIZE
    #define IDYES                       MBID_YES
    #define IDNO                        MBID_NO
    #define IDOK                        DLGID_OK        // DLGID_OK=MBID_OK
    #define IDCANCEL                    DLGID_CANCEL
    #define IDC_WAIT                    SPTR_WAIT
    #define IDC_ARROW                   SPTR_ARROW
    #define IDC_CROSS                   SPTR_ARROW
    #define IDC_IBEAM                   SPTR_TEXT
    #define IDC_SIZEWE                  SPTR_SIZEWE
    #define IDC_SIZENS                  SPTR_SIZENS
    #define IDC_SIZENWSE                SPTR_SIZENWSE
    #define IDC_SIZENESW                SPTR_SIZENESW
    #define IDC_SIZE                    SPTR_SIZE
    #define SRCCOPY                     ROP_SRCCOPY
    #define SRCPAINT                    ROP_SRCPAINT
    #define SRCAND                      ROP_SRCAND
    #define SRCINVERT                   ROP_SRCINVERT
    #define SRCERASE                    ROP_SRCERASE
    #define MERGECOPY                   ROP_MERGECOPY
    #define MERGEPAINT                  ROP_MERGEPAINT
    #define NOTSRCCOPY                  ROP_NOTSRCCOPY
    #define NOTSRCERASE                 ROP_NOTSRCERASE
    #define BLACKNESS                   ROP_ZERO
    #define WHITENESS                   ROP_ONE
    #define DSTINVERT                   ROP_DSTINVERT
    #define PATCOPY                     ROP_PATCOPY
    #define FF_DONTCARE                 0x00
    #define DEFAULT_PITCH               0x00
    #define FIXED_PITCH                 0x01
    #define VARIABLE_PITCH              0x02
    #define FF_ROMAN                    0x10
    #define FF_SWISS                    0x20
    #define FF_MODERN                   0x30
    #define FF_SCRIPT                   0x40
    #define FF_DECORATIVE               0x50
    #define CF_SCREENFONTS              0x00000001
    #define CF_PRINTERFONTS             0x00000002
    #define CF_BOTH                     (CF_SCREENFONTS | CF_PRINTERFONTS)
    #define CF_SHOWHELP                 0x00000004L
    #define CF_ENABLEHOOK               0x00000008L
    #define CF_ENABLETEMPLATE           0x00000010L
    #define CF_ENABLETEMPLATEHANDLE     0x00000020L
    #define CF_INITTOLOGFONTSTRUCT      0x00000040L
    #define CF_USESTYLE                 0x00000080L
    #define CF_EFFECTS                  0x00000100L
    #define CF_APPLY                    0x00000200L
    #define CF_ANSIONLY                 0x00000400L
    #define CF_NOVECTORFONTS            0x00000800L
    #define CF_NOOEMFONTS               CF_NOVECTORFONTS
    #define CF_NOSIMULATIONS            0x00001000L
    #define CF_LIMITSIZE                0x00002000L
    #define CF_FIXEDPITCHONLY           0x00004000L
    #define CF_WYSIWYG                  0x00008000L
    #define CF_FORCEFONTEXIST           0x00010000L
    #define CF_SCALABLEONLY             0x00020000L
    #define CF_TTONLY                   0x00040000L
    #define CF_NOFACESEL                0x00080000L
    #define CF_NOSTYLESEL               0x00100000L
    #define CF_NOSIZESEL                0x00200000L
    #define LF_FACESIZE                 32
    #define COLOR_ENTRYFIELD            SYSCLR_ENTRYFIELD
    #define COLOR_DIALOGBACKGROUND      SYSCLR_DIALOGBACKGROUND
    #define COLOR_WINDOW                SYSCLR_WINDOW
    #define COLOR_BTNSHADOW             SYSCLR_BUTTONDARK
    #define COLOR_BTNFACE               SYSCLR_BUTTONMIDDLE
    #define COLOR_BTNTEXT               RGB_BLACK
    #define COLOR_BTNHIGHLIGHT          SYSCLR_BUTTONLIGHT
    #define COLOR_WINDOWFRAME           SYSCLR_WINDOWFRAME
    #define LPDRAWITEMSTRUCT            POWNERITEM
    #define GetBValue(rgb)              ((BYTE)(rgb))
    #define GetGValue(rgb)              ((BYTE)(((WORD)(rgb)) >> 8))
    #define GetRValue(rgb)              ((BYTE)((rgb)>>16))
    #define RGB(r,g,b)                  ((WPI_COLOUR)(((BYTE)(b)|((WORD)(g)<<8))|(((DWORD)(BYTE)(r))<<16)))
    #define LPCSTR                      const char FAR *
    #define MAKEINTRESOURCE(i)          i
    #define HMENU                       HWND
    #define SW_HIDE                     SWP_HIDE
    #define SW_MINIMIZE                 SWP_MINIMIZE | SWP_FOCUSACTIVATE
    #define SW_RESTORE                  SWP_RESTORE
    #define SW_SHOW                     SWP_SHOW
    #define SW_SHOWMAXIMIZED            SWP_MAXIMIZE | SWP_ACTIVATE | SWP_SHOW
    #define SW_SHOWMINIMIZED            SWP_MINIMIZE | SWP_ACTIVATE | SWP_SHOW
    #define SW_SHOWMINNOACTIVE          SWP_MINIMIZE | SWP_SHOW
    #define SW_SHOWNA                   SWP_SHOW
    #define SW_SHOWNOACTIVATE           SWP_SHOW
    #define SW_SHOWNORMAL               SWP_RESTORE | SWP_SHOW
    #define SW_NORMAL                   SWP_RESTORE | SWP_SHOW
    #define SC_TASKLIST                 SC_TASKMANAGER
    #define WM_LBUTTONDBLCLK            WM_BUTTON1DBLCLK
    #define WM_RBUTTONDBLCLK            WM_BUTTON2DBLCLK
    #define WM_MBUTTONDBLCLK            WM_BUTTON3DBLCLK
    #define SM_CXSCREEN                 SV_CXSCREEN
    #define SM_CYSCREEN                 SV_CYSCREEN
    #define SM_CXFULLSCREEN             SV_CXFULLSCREEN
    #define SM_CYFULLSCREEN             SV_CYFULLSCREEN
    #define SM_CXDLGFRAME               SV_CXDLGFRAME
    #define SM_CYDLGFRAME               SV_CYDLGFRAME
    #define SM_CXFRAME                  SV_CXSIZEBORDER
    #define SM_CYFRAME                  SV_CYSIZEBORDER
    #define SM_CXBORDER                 SV_CXBORDER
    #define SM_CYBORDER                 SV_CYBORDER
    #define SM_CYCAPTION                SV_CYTITLEBAR
    #define SM_CYMENU                   SV_CYMENU
    #define SM_CXHTHUMB                 SV_CXHSLIDER
    #define SM_CYVTHUMB                 SV_CYVSLIDER
    #define SM_MOUSEPRESENT             SV_MOUSEPRESENT
    #define SWP_SHOWWINDOW              SWP_SHOW
    #define SWP_NOMOVE                  0
    #define SWP_NOSIZE                  0
    #define SWP_HIDEWINDOW              SWP_HIDE
    #define SWP_NOZORDER                0
    #define SWP_NOACTIVATE              0 //SWP_DEACTIVATE
    #define FLOODFILLSURFACE            FF_SURFACE
    #define FLOODFILLBORDER             FF_BOUNDARY
    #define DT_SINGLELINE               0

    /* menu flags */
    #define MF_SEPARATOR                MIS_SEPARATOR
    #define MF_BITMAP                   MIS_BITMAP
    #define MF_STRING                   MIS_TEXT
    #define MF_OWNERDRAW                MIS_OWNERDRAW
    #define MF_POPUP                    MIS_SUBMENU
    #define MF_MENUBARBREAK             MIS_BREAKSEPARATOR
    #define MF_MENUBREAK                MIS_BREAK
    #define MF_SYSMENU                  MIS_SYSCOMMAND

    /* menu attribute flags */
    #define MF_ENABLED                  0
    #define MF_DISABLED                 MIA_DISABLED
    #define MF_GRAYED                   MIA_DISABLED
    #define MF_UNCHECKED                0
    #define MF_CHECKED                  MIA_CHECKED
    #define MF_UNHILITE                 0
    #define MF_HILITE                   MIA_HILITED
    #define DIB_RGB_COLORS              0

    #define MLM_GETSEL                  MLM_QUERYSEL
    #define EM_LIMITTEXT                EM_SETTEXTLIMIT
    #define EM_GETSEL                   EM_QUERYSEL
    #define CB_SETEDITSEL               EM_SETSEL
    #define CB_GETEDITSEL               EM_QUERYSEL

    /* map modes */
    #define MM_TEXT                     0       // not used in OS/2

    /* text out modes */
    #define WPI_CLIPPED                 CHS_CLIP

typedef struct {
    DWORD       lStructSize;        /* */
    HWND        hwndOwner;          /* caller's window handle   */
    HDC         hDC;                /* printer DC/IC or NULL    */
    WPI_LOGFONT *lpLogFont;         /* ptr. to a LOGFONT struct */
    int         iPointSize;         /* 10 * size in points of selected font */
    DWORD       Flags;              /* enum. type flags         */
    WPI_COLOUR  rgbColors;          /* returned text color      */
    LPARAM      lCustData;          /* data passed to hook fn.  */
    UINT (APIENTRY *lpfnHook)(HWND, UINT, WPARAM, LPARAM);
                                    /* ptr. to hook function    */
    LPCSTR      lpTemplateName;     /* custom template name     */
    HINSTANCE   hInstance;          /* instance handle of.EXE that
                                     * contains cust. dlg. template
                                     */
    LPSTR       lpszStyle;          /* return the style field here
                                     * must be LF_FACESIZE or bigger */
    UINT        nFontType;          /* same value reported to the EnumFonts
                                     * call back with the extra FONTTYPE_
                                     * bits added */
    int         nSizeMin;           /* minimum pt size allowed & */
    int         nSizeMax;           /* max pt size allowed if    */
                                    /* CF_LIMITSIZE is used      */
} WPI_CHOOSEFONT;

typedef struct {
    int         left;
    int         top;
    int         right;
    int         bottom;
} RECT;

        #define WPI_LPCHOOSEFONT        WPI_CHOOSEFONT far *
        #define CHOOSEFONT              WPI_CHOOSEFONT
        #define LPCHOOSEFONT            WPI_CHOOSEFONT far *


typedef struct {
    ULONG       style;
    PFNWP       lpfnWndProc;
    int         cbClsExtra;             // not used in OS/2
    int         cbWndExtra;
    WPI_INST    hInstance;
    LONG        hIcon;                  // not used in OS/2
    LONG        hCursor;                // not used in OS/2
    LONG        hbrBackground;          // not used in OS/2
    PSZ         lpszMenuName;           // not used in OS/2
    PSZ         lpszClassName;
} WPI_WNDCLASS;
typedef WPI_WNDCLASS* WPI_PWNDCLASS;
#define WNDCLASS WPI_WNDCLASS

/*
 * NOTE:  The following is used internally by WPI on the PM side only.  For
 * someone using WPI, this should all be invisible.
 */
typedef enum {
    WPI_PEN_OBJ         = 1,
    WPI_NULLPEN_OBJ     = 2,
    WPI_BRUSH_OBJ       = 3,
    WPI_NULLBRUSH_OBJ   = 4,
    WPI_BITMAP_OBJ      = 5,
    WPI_PATBRUSH_OBJ    = 6,
    WPI_HLWBRUSH_OBJ    = 7,
} WPI_OBJECTTYPE;

typedef struct {
    AREABUNDLE          info;
    HBITMAP             bitmap;
} WPI_BRUSHTYPE;

typedef struct {
    WPI_OBJECTTYPE      type;
    union {
        LINEBUNDLE      pen;
        WPI_BRUSHTYPE   brush;
        HBITMAP         bitmap;
    };
} WPI_OBJECT;
/*
 * NOTE:  The preceding is used ONLY internally by WPI on the PM side.  For
 * someone using WPI, this should all be invisible.
 */

typedef LHANDLE         WPI_HANDLE;
typedef AREABUNDLE      LOGBRUSH;
typedef WPI_HANDLE      HBRUSH;
typedef WPI_HANDLE      HPEN;
typedef WPI_HANDLE      WPI_HBRUSH;
typedef WPI_HANDLE      WPI_HPEN;
typedef WPI_HANDLE      WPI_HBITMAP;

/* This is used in OS/2 much like a Windows LOGFONT. Use it only
   with the '_wpi_f_*' functions */
typedef struct {
    BOOL                retrieved;      // TRUE: struct set by get_f_attrs
    FATTRS              attr;
    CHARBUNDLE          bundle;
    LONG                pt_size;        // only set if retrieved = TRUE
} wpi_f_font;

#else
    #define LIT_END                     0
    #define LIT_SORTASCENDING           0
    #define LIT_SORTDESCENDING          0
    #define LIT_NONE                    -1
    #define LIT_FIRST                   0
    #define LIT_SELECT                  0
    #define LIT_UNSELECT                0
    #define HINI                        short
    #define MRESULT                     LONG
    #define WM_DLGCOMMAND               WM_COMMAND
    #define KEY_ASYNC_DOWN              0x8000
    #define KEY_ASYNC_PRESSED           0x0001
    #define WPI_VERT_MULT               1
    #define ULONG                       unsigned long
    #define NULLHANDLE                  ((HANDLE)0)
    #define HFILE                       int
    #define HFILE_FORMAT                int
    #define HMQ                         HANDLE
    #define DLGID_OK                    IDOK
    #define DLGID_CANCEL                IDCANCEL
    #define COLOR_ENTRYFIELD            COLOR_WINDOW
    #define COLOR_DIALOGBACKGROUND      COLOR_WINDOW
    #define SWP_MOVE                    0
    #define SWP_SIZE                    0
    #define SWP_ZORDER                  0
    #define DT_TEXTATTRS                0
    #define CMDSRC_MENU                 0
    #define CS_MOVENOTIFY               0       // no Windows Equivalent
    #define MLM_SETSEL                  EM_SETSEL
    #define MLM_GETSEL                  EM_GETSEL

    /* text out modes */
    #define WPI_CLIPPED                 ETO_CLIPPED

    #define MPFROMSHORT( s1 )           s1
    #define WPI_LOGPIXELSX_FONT         LOGPIXELSX
    #define WPI_LOGPIXELSY_FONT         LOGPIXELSY
#endif

#pragma pack()

// The following are not defined in the os2 2.0 header files so
// lets do it here
#ifndef CF_MMPMLAST
#define CF_MMPMLAST               19
#endif
#ifndef CF_MMPMFIRST
#define CF_MMPMFIRST              10
#endif

#endif
