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


#ifndef WDECCTL_INCLUDED
#define WDECCTL_INCLUDED

#ifdef __NT__
#include <commctrl.h>
#endif

#ifndef STATUSCLASSNAME
    #ifdef __NT__
        #define STATUSCLASSNAME "msctls_statusbar32"
    #else
        #define STATUSCLASSNAME "msctls_statusbar"
    #endif
#endif

#ifndef WC_LISTVIEW
    #ifdef __NT__
        #define WC_LISTVIEW     "SysListView32"
    #else
        #define WC_LISTVIEW     "SysListView"
    #endif
#endif

#ifndef WC_TREEVIEW
    #ifdef __NT__
        #define WC_TREEVIEW     "SysTreeView32"
    #else
        #define WC_TREEVIEW     "SysTreeView"
    #endif
#endif

// **** temporary kludge
// **** the commctrl.h in lang does not define a non-uncode
// **** version of the class string
#ifdef UPDOWN_CLASS
#undef UPDOWN_CLASS
#endif

#ifndef UPDOWN_CLASS
    #ifdef __NT__
        #define UPDOWN_CLASS    "msctls_updown32"
    #else
        #define UPDOWN_CLASS    "msctls_updown"
    #endif
#endif

#ifndef TRACKBAR_CLASS
    #ifdef __NT__
        #define TRACKBAR_CLASS  "msctls_trackbar32"
    #else
        #define TRACKBAR_CLASS  "msctls_trackbar"
    #endif
#endif

#ifndef PROGRESS_CLASS
    #ifdef __NT__
        #define PROGRESS_CLASS  "msctls_progress32"
    #else
        #define PROGRESS_CLASS  "msctls_progress"
    #endif
#endif

#ifndef HOTKEY_CLASS
    #ifdef __NT__
        #define HOTKEY_CLASS    "msctls_hotkey32"
    #else
        #define HOTKEY_CLASS    "msctls_hotkey"
    #endif
#endif

#ifndef WC_HEADER
    #ifdef __NT__
        #define WC_HEADER       "SysHeader32"
    #else
        #define WC_HEADER       "SysHeader"
    #endif
#endif

#ifndef WC_TABCONTROL
    #ifdef __NT__
        #define WC_TABCONTROL   "SysTabControl32"
    #else
        #define WC_TABCONTROL   "SysTabControl"
    #endif
#endif

#ifndef ANIMATE_CLASS
    #ifdef __NT__
        #define ANIMATE_CLASS   "SysAnimate32"
    #else
        #define ANIMATE_CLASS   "SysAnimate"
    #endif
#endif

#if(WINVER < 0x0400)

/*****************************/
/* these come from WINUSER.H */
/*****************************/

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

#define ES_NUMBER           0x2000L

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

#define SS_OWNERDRAW        0x0000000DL
#define SS_ENHMETAFILE      0x0000000FL
#define SS_ETCHEDHORZ       0x00000010L
#define SS_ETCHEDVERT       0x00000011L
#define SS_ETCHEDFRAME      0x00000012L
#define SS_TYPEMASK         0x0000001FL
#define SS_NOTIFY           0x00000100L
#define SS_RIGHTJUST        0x00000400L
#define SS_REALSIZEIMAGE    0x00000800L
#define SS_SUNKEN           0x00001000L
#define SS_BITMAP           0x0000000EL
#define SS_CENTERIMAGE      0x00000200L

#ifndef DS_SETFOREGROUND
#define DS_SETFOREGROUND    0x200L  /* not in win3.1 */
#endif

#define DS_3DLOOK           0x0004L
#define DS_FIXEDSYS         0x0008L
#define DS_NOFAILCREATE     0x0010L
#define DS_CONTROL          0x0400L
#define DS_CENTER           0x0800L
#define DS_CENTERMOUSE      0x1000L
#define DS_CONTEXTHELP      0x2000L

#define LBS_NOSEL           0x4000L

#define CBS_UPPERCASE       0x2000L
#define CBS_LOWERCASE       0x4000L

#define SBS_SIZEGRIP        0x0010L

/******************************/
/* these come from COMMCTRL.H */
/******************************/

#define HDS_HORZ                0x00000000
#define HDS_BUTTONS             0x00000002
#define HDS_HIDDEN              0x00000008

#define SBT_OWNERDRAW           0x1000
#define SBT_NOBORDERS           0x0100
#define SBT_POPOUT              0x0200
#define SBT_RTLREADING          0x0400

#ifndef TBS_AUTOTICKS
#define TBS_AUTOTICKS           0x0001
#endif
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

#define UDS_WRAP                0x0001
#define UDS_SETBUDDYINT         0x0002
#define UDS_ALIGNRIGHT          0x0004
#define UDS_ALIGNLEFT           0x0008
#define UDS_AUTOBUDDY           0x0010
#define UDS_ARROWKEYS           0x0020
#define UDS_HORZ                0x0040
#define UDS_NOTHOUSANDS         0x0080

#define LVS_ICON                0x0000
#define LVS_REPORT              0x0001
#define LVS_SMALLICON           0x0002
#define LVS_LIST                0x0003
#define LVS_TYPEMASK            0x0003
#define LVS_SINGLESEL           0x0004
#define LVS_SHOWSELALWAYS       0x0008
#define LVS_SORTASCENDING       0x0010
#define LVS_SORTDESCENDING      0x0020
#define LVS_SHAREIMAGELISTS     0x0040
#define LVS_NOLABELWRAP         0x0080
#define LVS_AUTOARRANGE         0x0100
#define LVS_EDITLABELS          0x0200
#define LVS_NOSCROLL            0x2000
#define LVS_TYPESTYLEMASK       0xfc00
#define LVS_ALIGNTOP            0x0000
#define LVS_ALIGNLEFT           0x0800
#define LVS_ALIGNMASK           0x0c00
#define LVS_OWNERDRAWFIXED      0x0400
#define LVS_NOCOLUMNHEADER      0x4000
#define LVS_NOSORTHEADER        0x8000

#define TVS_HASBUTTONS          0x0001
#define TVS_HASLINES            0x0002
#define TVS_LINESATROOT         0x0004
#define TVS_EDITLABELS          0x0008
#define TVS_DISABLEDRAGDROP     0x0010
#define TVS_SHOWSELALWAYS       0x0020

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

#define ACS_CENTER              0x0001
#define ACS_TRANSPARENT         0x0002
#define ACS_AUTOPLAY            0x0004

#endif

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern Bool WdeUsingCommonControls( void );
extern void WdeInitCommonControls( void );

#endif
