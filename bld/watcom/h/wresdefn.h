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
* Description:  Windows resources definitions used by resource compiler
*
****************************************************************************/


#ifndef WRESDEFN_INCLUDED
#define WRESDEFN_INCLUDED

#if defined( WIN_GUI )
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if !defined( WIN_GUI )

/*** predefined type numbers ***/
#define RT_CURSOR           1
#define RT_BITMAP           2
#define RT_ICON             3
#define RT_MENU             4
#define RT_DIALOG           5
#define RT_STRING           6
#define RT_FONTDIR          7
#define RT_FONT             8
#define RT_ACCELERATOR      9
#define RT_RCDATA           10
#define RT_GROUP_CURSOR     12      /* note: no type 13 resource */
#define RT_GROUP_ICON       14

#endif

// these symbols are not defined in windows.h
#ifndef RT_VERSIONINFO
#define RT_VERSIONINFO      16
#endif

#ifndef RT_MESSAGETABLE
#define RT_MESSAGETABLE     11      /* for NT res files only */
#endif
#ifndef RT_ERRTABLE
#define RT_ERRTABLE         11
#endif
#ifndef RT_NAMETABLE
#define RT_NAMETABLE        15
#endif
#ifndef RT_VERSION
#define RT_VERSION          16
#endif
#ifndef RT_DLGINCLUDE
#define RT_DLGINCLUDE       17
#endif

#define RT_TOOLBAR          0xF1

#if !defined( WIN_GUI )

/* Window Styles */

#define WS_OVERLAPPED       0x00000000L
#define WS_POPUP            0x80000000L
#define WS_CHILD            0x40000000L
#define WS_MINIMIZE         0x20000000L
#define WS_VISIBLE          0x10000000L
#define WS_DISABLED         0x08000000L
#define WS_CLIPSIBLINGS     0x04000000L
#define WS_CLIPCHILDREN     0x02000000L
#define WS_MAXIMIZE         0x01000000L
#define WS_CAPTION          0x00C00000L
#define WS_BORDER           0x00800000L
#define WS_DLGFRAME         0x00400000L
#define WS_VSCROLL          0x00200000L
#define WS_HSCROLL          0x00100000L
#define WS_SYSMENU          0x00080000L
#define WS_THICKFRAME       0x00040000L
#define WS_GROUP            0x00020000L
#define WS_TABSTOP          0x00010000L
#define WS_MINIMIZEBOX      0x00020000L
#define WS_MAXIMIZEBOX      0x00010000L
#define WS_TILED            WS_OVERLAPPED
#define WS_ICONIC           WS_MINIMIZE
#define WS_SIZEBOX          WS_THICKFRAME
#define WS_TILEDWINDOW      WS_OVERLAPPEDWINDOW
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED     | \
                             WS_CAPTION        | \
                             WS_SYSMENU        | \
                             WS_THICKFRAME     | \
                             WS_MINIMIZEBOX    | \
                             WS_MAXIMIZEBOX)
#define WS_POPUPWINDOW      (WS_POPUP          | \
                             WS_BORDER         | \
                             WS_SYSMENU)
#define WS_CHILDWINDOW      (WS_CHILD)


/* Extended Window Styles */

#define WS_EX_DLGMODALFRAME     0x00000001L
#define WS_EX_NOPARENTNOTIFY    0x00000004L
#define WS_EX_TOPMOST           0x00000008L
#define WS_EX_ACCEPTFILES       0x00000010L
#define WS_EX_TRANSPARENT       0x00000020L
#define WS_EX_MDICHILD          0x00000040L
#define WS_EX_TOOLWINDOW        0x00000080L
#define WS_EX_WINDOWEDGE        0x00000100L
#define WS_EX_CLIENTEDGE        0x00000200L
#define WS_EX_CONTEXTHELP       0x00000400L
#define WS_EX_RIGHT             0x00001000L
#define WS_EX_LEFT              0x00000000L
#define WS_EX_RTLREADING        0x00002000L
#define WS_EX_LTRREADING        0x00000000L
#define WS_EX_LEFTSCROLLBAR     0x00004000L
#define WS_EX_RIGHTSCROLLBAR    0x00000000L
#define WS_EX_CONTROLPARENT     0x00010000L
#define WS_EX_STATICEDGE        0x00020000L
#define WS_EX_APPWINDOW         0x00040000L
#define WS_EX_OVERLAPPEDWINDOW  (WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE)
#define WS_EX_PALETTEWINDOW     (WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST)


/* Dialog styles */

#define DS_ABSALIGN         0x00000001L
#define DS_SYSMODAL         0x00000002L
#define DS_LOCALEDIT        0x00000020L
#define DS_SETFONT          0x00000040L
#define DS_MODALFRAME       0x00000080L
#define DS_NOIDLEMSG        0x00000100L
#define DS_SETFOREGROUND    0x00000200L
#define DS_3DLOOK           0x00000004L
#define DS_FIXEDSYS         0x00000008L
#define DS_NOFAILCREATE     0x00000010L
#define DS_CONTROL          0x00000400L
#define DS_CENTER           0x00000800L
#define DS_CENTERMOUSE      0x00001000L
#define DS_CONTEXTHELP      0x00002000L

/* Button Control Styles */

#define BS_PUSHBUTTON       0x00000000L
#define BS_DEFPUSHBUTTON    0x00000001L
#define BS_CHECKBOX         0x00000002L
#define BS_AUTOCHECKBOX     0x00000003L
#define BS_RADIOBUTTON      0x00000004L
#define BS_3STATE           0x00000005L
#define BS_AUTO3STATE       0x00000006L
#define BS_GROUPBOX         0x00000007L
#define BS_USERBUTTON       0x00000008L
#define BS_AUTORADIOBUTTON  0x00000009L
#define BS_OWNERDRAW        0x0000000BL
#define BS_LEFTTEXT         0x00000020L
#define BS_TEXT             0x00000000L
#define BS_ICON             0x00000040L
#define BS_BITMAP           0x00000080L
#define BS_LEFT             0x00000100L
#define BS_RIGHT            0x00000200L
#define BS_CENTER           0x00000300L
#define BS_TOP              0x00000400L
#define BS_BOTTOM           0x00000800L
#define BS_VCENTER          0x00000C00L
#define BS_PUSHLIKE         0x00001000L
#define BS_MULTILINE        0x00002000L
#define BS_NOTIFY           0x00004000L
#define BS_FLAT             0x00008000L
#define BS_RIGHTBUTTON      BS_LEFTTEXT


/* Combo Box styles */

#define CBS_SIMPLE            0x0001L
#define CBS_DROPDOWN          0x0002L
#define CBS_DROPDOWNLIST      0x0003L
#define CBS_OWNERDRAWFIXED    0x0010L
#define CBS_OWNERDRAWVARIABLE 0x0020L
#define CBS_AUTOHSCROLL       0x0040L
#define CBS_OEMCONVERT        0x0080L
#define CBS_SORT              0x0100L
#define CBS_HASSTRINGS        0x0200L
#define CBS_NOINTEGRALHEIGHT  0x0400L
#define CBS_DISABLENOSCROLL   0x0800L
#define CBS_UPPERCASE         0x2000L
#define CBS_LOWERCASE         0x4000L


/* Edit Control Styles */

#define ES_LEFT             0x0000L
#define ES_CENTER           0x0001L
#define ES_RIGHT            0x0002L
#define ES_MULTILINE        0x0004L
#define ES_UPPERCASE        0x0008L
#define ES_LOWERCASE        0x0010L
#define ES_PASSWORD         0x0020L
#define ES_AUTOVSCROLL      0x0040L
#define ES_AUTOHSCROLL      0x0080L
#define ES_NOHIDESEL        0x0100L
#define ES_OEMCONVERT       0x0400L
#define ES_READONLY         0x0800L
#define ES_WANTRETURN       0x1000L
#define ES_NUMBER           0x2000L
#define ES_DISABLENOSCROLL  0x2000L
#define ES_SUNKEN           0x4000L
#define ES_SAVESEL          0x8000L
#define ES_SELECTIONBAR     0x1000000L


/* Static Control Constants */

#define SS_LEFT             0x00000000L
#define SS_CENTER           0x00000001L
#define SS_RIGHT            0x00000002L
#define SS_ICON             0x00000003L
#define SS_BLACKRECT        0x00000004L
#define SS_GRAYRECT         0x00000005L
#define SS_WHITERECT        0x00000006L
#define SS_BLACKFRAME       0x00000007L
#define SS_GRAYFRAME        0x00000008L
#define SS_WHITEFRAME       0x00000009L
#define SS_USERITEM         0x0000000AL
#define SS_SIMPLE           0x0000000BL
#define SS_LEFTNOWORDWRAP   0x0000000CL
#define SS_OWNERDRAW        0x0000000DL
#define SS_BITMAP           0x0000000EL
#define SS_ENHMETAFILE      0x0000000FL
#define SS_ETCHEDHORZ       0x00000010L
#define SS_ETCHEDVERT       0x00000011L
#define SS_ETCHEDFRAME      0x00000012L
#define SS_TYPEMASK         0x0000001FL
#define SS_NOPREFIX         0x00000080L
#define SS_NOTIFY           0x00000100L
#define SS_CENTERIMAGE      0x00000200L
#define SS_RIGHTJUST        0x00000400L
#define SS_REALSIZEIMAGE    0x00000800L
#define SS_SUNKEN           0x00001000L


/* Scroll Bar Styles */

#define SBS_HORZ                    0x0000L
#define SBS_VERT                    0x0001L
#define SBS_TOPALIGN                0x0002L
#define SBS_LEFTALIGN               0x0002L
#define SBS_BOTTOMALIGN             0x0004L
#define SBS_RIGHTALIGN              0x0004L
#define SBS_SIZEBOXTOPLEFTALIGN     0x0002L
#define SBS_SIZEBOXBOTTOMRIGHTALIGN 0x0004L
#define SBS_SIZEBOX                 0x0008L
#define SBS_SIZEGRIP                0x0010L


/* Listbox Styles */

#define LBS_NOTIFY            0x0001L
#define LBS_SORT              0x0002L
#define LBS_NOREDRAW          0x0004L
#define LBS_MULTIPLESEL       0x0008L
#define LBS_OWNERDRAWFIXED    0x0010L
#define LBS_OWNERDRAWVARIABLE 0x0020L
#define LBS_HASSTRINGS        0x0040L
#define LBS_USETABSTOPS       0x0080L
#define LBS_NOINTEGRALHEIGHT  0x0100L
#define LBS_MULTICOLUMN       0x0200L
#define LBS_WANTKEYBOARDINPUT 0x0400L
#define LBS_EXTENDEDSEL       0x0800L
#define LBS_DISABLENOSCROLL   0x1000L
#define LBS_NODATA            0x2000L
#define LBS_NOSEL             0x4000L
#define LBS_STANDARD          (LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER)


/* Common control styles */

#define CCS_TOP               0x00000001L
#define CCS_NOMOVEY           0x00000002L
#define CCS_BOTTOM            0x00000003L
#define CCS_NORESIZE          0x00000004L
#define CCS_NOPARENTALIGN     0x00000008L
#define CCS_ADJUSTABLE        0x00000020L
#define CCS_NODIVIDER         0x00000040L

/* Header control styles */

#define HDS_HORZ            0x00000000
#define HDS_BUTTONS         0x00000002
#define HDS_HIDDEN          0x00000008


/* ListView styles */

#define LVS_ICON            0x0000
#define LVS_REPORT          0x0001
#define LVS_SMALLICON       0x0002
#define LVS_LIST            0x0003
#define LVS_TYPEMASK        0x0003
#define LVS_SINGLESEL       0x0004
#define LVS_SHOWSELALWAYS   0x0008
#define LVS_SORTASCENDING   0x0010
#define LVS_SORTDESCENDING  0x0020
#define LVS_SHAREIMAGELISTS 0x0040
#define LVS_NOLABELWRAP     0x0080
#define LVS_AUTOARRANGE     0x0100
#define LVS_EDITLABELS      0x0200
#define LVS_NOSCROLL        0x2000
#define LVS_TYPESTYLEMASK   0xfc00
#define LVS_ALIGNTOP        0x0000
#define LVS_ALIGNLEFT       0x0800
#define LVS_ALIGNMASK       0x0c00
#define LVS_OWNERDRAWFIXED  0x0400
#define LVS_NOCOLUMNHEADER  0x4000
#define LVS_NOSORTHEADER    0x8000


/* TreeView styles */

#define TVS_HASBUTTONS      0x0001
#define TVS_HASLINES        0x0002
#define TVS_LINESATROOT     0x0004
#define TVS_EDITLABELS      0x0008
#define TVS_DISABLEDRAGDROP 0x0010
#define TVS_SHOWSELALWAYS   0x0020


/* Progress Bar styles */

#define PBS_SHOWPERCENT         0x01
#define PBS_SHOWPOS             0x02


/* Trackbar styles */

#define TBS_AUTOTICKS           0x0001
#define TBS_VERT                0x0002
#define TBS_HORZ                0x0000
#define TBS_TOP                 0x0004
#define TBS_BOTTOM              0x0000
#define TBS_LEFT                0x0004
#define TBS_RIGHT               0x0000
#define TBS_BOTH                0x0008
#define TBS_NOTICKS             0x0010
#define TBS_ENABLESELRANGE      0x0020
#define TBS_FIXEDLENGTH         0x0040
#define TBS_NOTHUMB             0x0080


/* Up-Down styles */

#define UDS_WRAP                0x0001
#define UDS_SETBUDDYINT         0x0002
#define UDS_ALIGNRIGHT          0x0004
#define UDS_ALIGNLEFT           0x0008
#define UDS_AUTOBUDDY           0x0010
#define UDS_ARROWKEYS           0x0020
#define UDS_HORZ                0x0040
#define UDS_NOTHOUSANDS         0x0080


/* Split-Bar styles: these are WClass generated styles */

#define WSPLIT_DRAWENDS 0x00000001L
#define WSPLIT_BORDER   0x00000002L

/* Media Control Styles */

#define MCIWNDF_NOAUTOSIZEWINDOW    0x0001
#define MCIWNDF_NOPLAYBAR           0x0002
#define MCIWNDF_NOAUTOSIZEMOVIE     0x0004
#define MCIWNDF_NOMENU              0x0008
#define MCIWNDF_SHOWNAME            0x0010
#define MCIWNDF_SHOWPOS             0x0020
#define MCIWNDF_SHOWMODE            0x0040
#define MCIWNDF_SHOWALL             0x0070
#define MCIWNDF_NOTIFYANSI          0x0080
#define MCIWNDF_NOTIFYMODE          0x0100
#define MCIWNDF_NOTIFYPOS           0x0200
#define MCIWNDF_NOTIFYSIZE          0x0400
#define MCIWNDF_NOTIFYERROR         0x1000
#define MCIWNDF_NOTIFYALL           0x1F00
#define MCIWNDF_NOTIFYMEDIAA        0x0880
#define MCIWNDF_NOTIFYMEDIAW        0x0800
#define MCIWNDF_RECORD              0x2000
#define MCIWNDF_NOERRORDLG          0x4000
#define MCIWNDF_NOOPEN              0x8000

/* Animate Class Styles */

#define ACS_CENTER              0x0001
#define ACS_TRANSPARENT         0x0002
#define ACS_AUTOPLAY            0x0004

/* Class header names */

#define ANIMATE_CLASS           "SysAnimate32"
#define HOTKEY_CLASS            "msctls_hotkey32"
#define PROGRESS_CLASS          "msctls_progress32"
#define TOOLTIPS_CLASS          "tooltips_class32"
#define TRACKBAR_CLASS          "msctls_trackbar32"
#define UPDOWN_CLASS            "msctls_updown32"
#define WC_HEADER               "SysHeader32"
#define WC_LISTVIEW             "SysListView32"
#define WC_TREEVIEW             "SysTreeView32"
#define WC_TABCONTROL           "SysTabControl32"
#define MCIWND_WINDOW_CLASS     "MCIWndClass"


/* VersionInfo resource definitions */

#define VS_VERSION_INFO         1
#define VS_FFI_FILEFLAGSMASK    0x0000003FL
#define VS_FF_DEBUG             0x00000001L
#define VS_FF_PRERELEASE        0x00000002L
#define VOS_NT_WINDOWS32        0x00040004L
#define VFT_APP                 0x00000001L


/* tab control style flags */

#define TCS_FORCEICONLEFT       0x0010
#define TCS_FORCELABELLEFT      0x0020
#define TCS_TABS                0x0000
#define TCS_BUTTONS             0x0100
#define TCS_SINGLELINE          0x0000
#define TCS_MULTILINE           0x0200
#define TCS_RIGHTJUSTIFY        0x0000
#define TCS_FIXEDWIDTH          0x0400
#define TCS_RAGGEDRIGHT         0x0800
#define TCS_FOCUSONBUTTONDOWN   0x1000
#define TCS_OWNERDRAWFIXED      0x2000
#define TCS_TOOLTIPS            0x4000
#define TCS_FOCUSNEVER          0x8000

#endif

#endif
