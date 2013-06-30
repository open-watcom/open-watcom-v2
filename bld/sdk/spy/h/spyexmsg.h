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
* Description:  Extra message definitions for compatibility with various
*               Win32 header versions.
*
****************************************************************************/


// Control styles
#ifndef BS_TYPEMASK
    #define BS_TYPEMASK         0x000FL
#endif
#ifndef SS_TYPEMASK
    #define SS_TYPEMASK         0x001FL
#endif
#ifndef SS_REALSIZECONTROL
    #define SS_REALSIZECONTROL  0x0040L
#endif
#ifndef SS_EDITCONTROL
    #define SS_EDITCONTROL      0x2000L
#endif
#ifndef LBS_COMBOBOX
    #define LBS_COMBOBOX        0x8000L
#endif

// Common control style
#ifndef MCS_NOTRAILINGDATES
    #define MCS_NOTRAILINGDATES         0x0040L
#endif
#ifndef MCS_SHORTDAYSOFWEEK
    #define MCS_SHORTDAYSOFWEEK         0x0080L
#endif
#ifndef MCS_NOSELCHANGEONNAV
    #define MCS_NOSELCHANGEONNAV        0x0100L
#endif
#ifndef PBS_MARQUEE
    #define PBS_MARQUEE                 0x0008L
#endif
#ifndef PBS_SMOOTHREVERSE
    #define PBS_SMOOTHREVERSE           0x0010L
#endif
#ifndef SBARS_TOOLTIPS
    #define SBARS_TOOLTIPS              0x0800L
#endif
#ifndef TTS_USEVISUALSTYLE
    #define TTS_USEVISUALSTYLE          0x0100L
#endif
#ifndef TBS_NOTIFYBEFOREMOVE
    #define TBS_NOTIFYBEFOREMOVE        0x0800L
#endif
#ifndef TBS_TRANSPARENTBKGND
    #define TBS_TRANSPARENTBKGND        0x1000L
#endif
#ifndef HDS_FLAT
    #define HDS_FLAT                    0x0200L
#endif
#ifndef HDS_CHECKBOXES
    #define HDS_CHECKBOXES              0x0400L
#endif
#ifndef HDS_NOSIZING
    #define HDS_NOSIZING                0x0800L
#endif
#ifndef HDS_OVERFLOW
    #define HDS_OVERFLOW                0x1000L
#endif

// Extended styles
#ifndef WS_EX_LAYERED
    #define WS_EX_LAYERED           0x00080000L
#endif
#ifndef WS_EX_NOINHERITLAYOUT
    #define WS_EX_NOINHERITLAYOUT   0x00100000L
#endif
#ifndef WS_EX_LAYOUTRTL
    #define WS_EX_LAYOUTRTL         0x00400000L
#endif
#ifndef WS_EX_COMPOSITED
    #define WS_EX_COMPOSITED        0x02000000L
#endif
#ifndef WS_EX_NOACTIVATE
    #define WS_EX_NOACTIVATE        0x08000000L
#endif

// Common control message ranges
#ifndef MCM_FIRST
    #define MCM_FIRST                       0x1000
#endif
#ifndef DTM_FIRST
    #define DTM_FIRST                       0x1000
#endif

// PenWindows specific messages
#ifndef WM_PENWINFIRST
    #define WM_PENWINFIRST                  0x0380
#endif
#ifndef WM_PENWINLAST
    #define WM_PENWINLAST                   0x038F
#endif
#ifndef WM_COALESCE_FIRST
    #define WM_COALESCE_FIRST               0x0390
#endif
#ifndef WM_COALESCE_LAST
    #define WM_COALESCE_LAST                0x039F
#endif
#ifndef WM_CTLCOLOR
    #define WM_CTLCOLOR                     0x0019
#endif
#ifndef WM_GETHOTKEY
    #define WM_GETHOTKEY                    0x0033
#endif
#ifndef WM_SETHOTKEY
    #define WM_SETHOTKEY                    0x0032
#endif
#ifndef WM_PAINTICON
    #define WM_PAINTICON                    0x0026
#endif
#ifndef WM_OTHERWINDOWCREATED
    #define WM_OTHERWINDOWCREATED           0x0042
#endif
#ifndef WM_OTHERWINDOWDESTROYED
    #define WM_OTHERWINDOWDESTROYED         0x0043
#endif

// Messages undocumented for WINDOWS but documented for NT
#ifndef WM_ENTERMENULOOP
    #define WM_ENTERMENULOOP                0x0211
#endif
#ifndef WM_EXITMENULOOP
    #define WM_EXITMENULOOP                 0x0212
#endif

// Windows 95 specific messages
#ifndef WM_NOTIFY
    #define WM_NOTIFY                       0x004E
#endif
#ifndef WM_INPUTLANGCHANGEREQUEST
    #define WM_INPUTLANGCHANGEREQUEST       0x0050
#endif
#ifndef WM_INPUTLANGCHANGE
    #define WM_INPUTLANGCHANGE              0x0051
#endif
#ifndef WM_TCARD
    #define WM_TCARD                        0x0052
#endif
#ifndef WM_HELP
    #define WM_HELP                         0x0053
#endif
#ifndef WM_USERCHANGED
    #define WM_USERCHANGED                  0x0054
#endif
#ifndef WM_CONTEXTMENU
    #define WM_CONTEXTMENU                  0x007B
#endif
#ifndef WM_STYLECHANGING
    #define WM_STYLECHANGING                0x007C
#endif
#ifndef WM_STYLECHANGED
    #define WM_STYLECHANGED                 0x007D
#endif
#ifndef WM_DISPLAYCHANGE
    #define WM_DISPLAYCHANGE                0x007E
#endif
#ifndef WM_GETICON
    #define WM_GETICON                      0x007F
#endif
#ifndef WM_SETICON
    #define WM_SETICON                      0x0080
#endif

// Messages introduced after Windows 95
#ifndef WM_NCXBUTTONDOWN
    #define WM_NCXBUTTONDOWN                0x00AB
#endif
#ifndef WM_NCXBUTTONUP
    #define WM_NCXBUTTONUP                  0x00AC
#endif
#ifndef WM_NCXBUTTONDBLCLK
    #define WM_NCXBUTTONDBLCLK              0x00AD
#endif
#ifndef WM_INPUT_DEVICE_CHANGE
    #define WM_INPUT_DEVICE_CHANGE          0x00FE
#endif
#ifndef WM_INPUT
    #define WM_INPUT                        0x00FF
#endif
#ifndef WM_UNICHAR
    #define WM_UNICHAR                      0x0108
#endif
#ifndef WM_MENURBUTTONUP
    #define WM_MENURBUTTONUP                0x0122
#endif
#ifndef WM_MENUDRAG
    #define WM_MENUDRAG                     0x0123
#endif
#ifndef WM_MENUGETOBJECT
    #define WM_MENUGETOBJECT                0x0124
#endif
#ifndef WM_UNINITMENUPOPUP
    #define WM_UNINITMENUPOPUP              0x0125
#endif
#ifndef WM_MENUCOMMAND
    #define WM_MENUCOMMAND                  0x0126
#endif
#ifndef WM_CHANGEUISTATE
    #define WM_CHANGEUISTATE                0x0127
#endif
#ifndef WM_UPDATEUISTATE
    #define WM_UPDATEUISTATE                0x0128
#endif
#ifndef WM_QUERYUISTATE
    #define WM_QUERYUISTATE                 0x0129
#endif
#ifndef WM_MOUSEWHEEL
    #define WM_MOUSEWHEEL                   0x020A
#endif
#ifndef WM_XBUTTONDOWN
    #define WM_XBUTTONDOWN                  0x020B
#endif
#ifndef WM_XBUTTONUP
    #define WM_XBUTTONUP                    0x020C
#endif
#ifndef WM_XBUTTONDBLCLK
    #define WM_XBUTTONDBLCLK                0x020D
#endif
#ifndef WM_MOUSEHWHEEL
    #define WM_MOUSEHWHEEL                  0x020E
#endif
#ifndef WM_IME_REQUEST
    #define WM_IME_REQUEST                  0x0288
#endif
#ifndef WM_MOUSEHOVER
    #define WM_MOUSEHOVER                   0x02A1
#endif
#ifndef WM_MOUSELEAVE
    #define WM_MOUSELEAVE                   0x02A3
#endif
#ifndef WM_NCMOUSEHOVER
    #define WM_NCMOUSEHOVER                 0x02A0
#endif
#ifndef WM_NCMOUSELEAVE
    #define WM_NCMOUSELEAVE                 0x02A2
#endif
#ifndef WM_WTSSESSION_CHANGE
    #define WM_WTSSESSION_CHANGE            0x02B1
#endif
#ifndef WM_APPCOMMAND
    #define WM_APPCOMMAND                   0x0319
#endif
#ifndef WM_THEMECHANGED
    #define WM_THEMECHANGED                 0x031A
#endif
#ifndef WM_CLIPBOARDUPDATE
    #define WM_CLIPBOARDUPDATE              0x031D
#endif
#ifndef WM_DWMCOMPOSITIONCHANGED
    #define WM_DWMCOMPOSITIONCHANGED        0x031E
#endif
#ifndef WM_DWMNCRENDERINGCHANGED
    #define WM_DWMNCRENDERINGCHANGED        0x031F
#endif
#ifndef WM_DWMCOLORIZATIONCOLORCHANGED
    #define WM_DWMCOLORIZATIONCOLORCHANGED  0x0320
#endif
#ifndef WM_DWMWINDOWMAXIMIZEDCHANGE
    #define WM_DWMWINDOWMAXIMIZEDCHANGE     0x0321
#endif
#ifndef WM_GETTITLEBARINFOEX
    #define WM_GETTITLEBARINFOEX            0x033F
#endif

// Control messages
#ifndef EM_SETIMESTATUS
    #define EM_SETIMESTATUS                 0x00D8
#endif
#ifndef EM_GETIMESTATUS
    #define EM_GETIMESTATUS                 0x00D9
#endif
#ifndef BM_SETDONTCLICK
    #define BM_SETDONTCLICK                 0x00F8
#endif
#ifndef LB_GETLISTBOXINFO
    #define LB_GETLISTBOXINFO               0x01B2
#endif
#ifndef CB_GETCOMBOBOXINFO
    #define CB_GETCOMBOBOXINFO              0x0164
#endif
#ifndef SBM_GETSCROLLBARINFO
    #define SBM_GETSCROLLBARINFO            0x00EB
#endif

// Common control messages
#ifndef HDM_SETBITMAPMARGIN
    #define HDM_SETBITMAPMARGIN             (HDM_FIRST + 20)
#endif
#ifndef HDM_GETBITMAPMARGIN
    #define HDM_GETBITMAPMARGIN             (HDM_FIRST + 21)
#endif
#ifndef HDM_SETFILTERCHANGETIMEOUT
    #define HDM_SETFILTERCHANGETIMEOUT      (HDM_FIRST + 22)
#endif
#ifndef HDM_EDITFILTER
    #define HDM_EDITFILTER                  (HDM_FIRST + 23)
#endif
#ifndef HDM_CLEARFILTER
    #define HDM_CLEARFILTER                 (HDM_FIRST + 24)
#endif
#ifndef HDM_GETITEMDROPDOWNRECT
    #define HDM_GETITEMDROPDOWNRECT         (HDM_FIRST + 25)
#endif
#ifndef HDM_GETOVERFLOWRECT
    #define HDM_GETOVERFLOWRECT             (HDM_FIRST + 26)
#endif
#ifndef HDM_GETFOCUSEDITEM
    #define HDM_GETFOCUSEDITEM              (HDM_FIRST + 27)
#endif
#ifndef HDM_SETFOCUSEDITEM
    #define HDM_SETFOCUSEDITEM              (HDM_FIRST + 28)
#endif
#ifndef TB_GETMETRICS
    #define TB_GETMETRICS                   (WM_USER + 101)
#endif
#ifndef TB_SETMETRICS
    #define TB_SETMETRICS                   (WM_USER + 102)
#endif
#ifndef TB_SETPRESSEDIMAGELIST
    #define TB_SETPRESSEDIMAGELIST          (WM_USER + 104)
#endif
#ifndef TB_GETPRESSEDIMAGELIST
    #define TB_GETPRESSEDIMAGELIST          (WM_USER + 105)
#endif
#ifndef RB_GETBANDMARGINS
    #define RB_GETBANDMARGINS               (WM_USER + 40)
#endif
#ifndef RB_SETEXTENDEDSTYLE
    #define RB_SETEXTENDEDSTYLE             (WM_USER + 41)
#endif
#ifndef RB_GETEXTENDEDSTYLE
    #define RB_GETEXTENDEDSTYLE             (WM_USER + 42)
#endif
#ifndef RB_PUSHCHEVRON
    #define RB_PUSHCHEVRON                  (WM_USER + 43)
#endif
#ifndef RB_SETBANDWIDTH
    #define RB_SETBANDWIDTH                 (WM_USER + 44)
#endif
#ifndef TTM_POPUP
    #define TTM_POPUP                       (WM_USER + 34)
#endif
#ifndef TTM_GETTITLE
    #define TTM_GETTITLE                    (WM_USER + 35)
#endif
#ifndef PBM_SETMARQUEE
    #define PBM_SETMARQUEE                  (WM_USER + 10)
#endif
#ifndef PBM_GETSTEP
    #define PBM_GETSTEP                     (WM_USER + 11)
#endif
#ifndef PBM_GETBKCOLOR
    #define PBM_GETBKCOLOR                  (WM_USER + 12)
#endif
#ifndef PBM_GETBARCOLOR
    #define PBM_GETBARCOLOR                 (WM_USER + 13)
#endif
#ifndef PBM_SETSTATE
    #define PBM_SETSTATE                    (WM_USER + 14)
#endif
#ifndef PBM_GETSTATE
    #define PBM_GETSTATE                    (WM_USER + 15)
#endif
#ifndef LVM_GETGROUPSTATE
    #define LVM_GETGROUPSTATE               (LVM_FIRST + 92)
#endif
#ifndef LVM_GETFOCUSEDGROUP
    #define LVM_GETFOCUSEDGROUP             (LVM_FIRST + 93)
#endif
#ifndef LVM_GETGROUPRECT
    #define LVM_GETGROUPRECT                (LVM_FIRST + 98)
#endif
#ifndef LVM_SETSELECTEDCOLUMN
    #define LVM_SETSELECTEDCOLUMN           (LVM_FIRST + 140)
#endif
#ifndef LVM_SETVIEW
    #define LVM_SETVIEW                     (LVM_FIRST + 142)
#endif
#ifndef LVM_GETVIEW
    #define LVM_GETVIEW                     (LVM_FIRST + 143)
#endif
#ifndef LVM_INSERTGROUP
    #define LVM_INSERTGROUP                 (LVM_FIRST + 145)
#endif
#ifndef LVM_SETGROUPINFO
    #define LVM_SETGROUPINFO                (LVM_FIRST + 147)
#endif
#ifndef LVM_GETGROUPINFO
    #define LVM_GETGROUPINFO                (LVM_FIRST + 149)
#endif
#ifndef LVM_REMOVEGROUP
    #define LVM_REMOVEGROUP                 (LVM_FIRST + 150)
#endif
#ifndef LVM_MOVEGROUP
    #define LVM_MOVEGROUP                   (LVM_FIRST + 151)
#endif
#ifndef LVM_GETGROUPCOUNT
    #define LVM_GETGROUPCOUNT               (LVM_FIRST + 152)
#endif
#ifndef LVM_GETGROUPINFOBYINDEX
    #define LVM_GETGROUPINFOBYINDEX         (LVM_FIRST + 153)
#endif
#ifndef LVM_MOVEITEMTOGROUP
    #define LVM_MOVEITEMTOGROUP             (LVM_FIRST + 154)
#endif
#ifndef LVM_SETGROUPMETRICS
    #define LVM_SETGROUPMETRICS             (LVM_FIRST + 155)
#endif
#ifndef LVM_GETGROUPMETRICS
    #define LVM_GETGROUPMETRICS             (LVM_FIRST + 156)
#endif
#ifndef LVM_ENABLEGROUPVIEW
    #define LVM_ENABLEGROUPVIEW             (LVM_FIRST + 157)
#endif
#ifndef LVM_SORTGROUPS
    #define LVM_SORTGROUPS                  (LVM_FIRST + 158)
#endif
#ifndef LVM_INSERTGROUPSORTED
    #define LVM_INSERTGROUPSORTED           (LVM_FIRST + 159)
#endif
#ifndef LVM_REMOVEALLGROUPS
    #define LVM_REMOVEALLGROUPS             (LVM_FIRST + 160)
#endif
#ifndef LVM_HASGROUP
    #define LVM_HASGROUP                    (LVM_FIRST + 161)
#endif
#ifndef LVM_SETTILEVIEWINFO
    #define LVM_SETTILEVIEWINFO             (LVM_FIRST + 162)
#endif
#ifndef LVM_GETTILEVIEWINFO
    #define LVM_GETTILEVIEWINFO             (LVM_FIRST + 163)
#endif
#ifndef LVM_SETTILEINFO
    #define LVM_SETTILEINFO                 (LVM_FIRST + 164)
#endif
#ifndef LVM_GETTILEINFO
    #define LVM_GETTILEINFO                 (LVM_FIRST + 165)
#endif
#ifndef LVM_SETINSERTMARK
    #define LVM_SETINSERTMARK               (LVM_FIRST + 166)
#endif
#ifndef LVM_GETINSERTMARK
    #define LVM_GETINSERTMARK               (LVM_FIRST + 167)
#endif
#ifndef LVM_INSERTMARKHITTEST
    #define LVM_INSERTMARKHITTEST           (LVM_FIRST + 168)
#endif
#ifndef LVM_GETINSERTMARKRECT
    #define LVM_GETINSERTMARKRECT           (LVM_FIRST + 169)
#endif
#ifndef LVM_SETINSERTMARKCOLOR
    #define LVM_SETINSERTMARKCOLOR          (LVM_FIRST + 170)
#endif
#ifndef LVM_GETINSERTMARKCOLOR
    #define LVM_GETINSERTMARKCOLOR          (LVM_FIRST + 171)
#endif
#ifndef LVM_SETINFOTIP
    #define LVM_SETINFOTIP                  (LVM_FIRST + 173)
#endif
#ifndef LVM_GETSELECTEDCOLUMN
    #define LVM_GETSELECTEDCOLUMN           (LVM_FIRST + 174)
#endif
#ifndef LVM_ISGROUPVIEWENABLED
    #define LVM_ISGROUPVIEWENABLED          (LVM_FIRST + 175)
#endif
#ifndef LVM_GETOUTLINECOLOR
    #define LVM_GETOUTLINECOLOR             (LVM_FIRST + 176)
#endif
#ifndef LVM_SETOUTLINECOLOR
    #define LVM_SETOUTLINECOLOR             (LVM_FIRST + 177)
#endif
#ifndef LVM_CANCELEDITLABEL
    #define LVM_CANCELEDITLABEL             (LVM_FIRST + 179)
#endif
#ifndef LVM_MAPINDEXTOID
    #define LVM_MAPINDEXTOID                (LVM_FIRST + 180)
#endif
#ifndef LVM_MAPIDTOINDEX
    #define LVM_MAPIDTOINDEX                (LVM_FIRST + 181)
#endif
#ifndef LVM_ISITEMVISIBLE
    #define LVM_ISITEMVISIBLE               (LVM_FIRST + 182)
#endif
#ifndef LVM_GETEMPTYTEXT
    #define LVM_GETEMPTYTEXT                (LVM_FIRST + 204)
#endif
#ifndef LVM_GETFOOTERRECT
    #define LVM_GETFOOTERRECT               (LVM_FIRST + 205)
#endif
#ifndef LVM_GETFOOTERINFO
    #define LVM_GETFOOTERINFO               (LVM_FIRST + 206)
#endif
#ifndef LVM_GETFOOTERITEMRECT
    #define LVM_GETFOOTERITEMRECT           (LVM_FIRST + 207)
#endif
#ifndef LVM_GETFOOTERITEM
    #define LVM_GETFOOTERITEM               (LVM_FIRST + 208)
#endif
#ifndef LVM_GETITEMINDEXRECT
    #define LVM_GETITEMINDEXRECT            (LVM_FIRST + 209)
#endif
#ifndef LVM_SETITEMINDEXSTATE
    #define LVM_SETITEMINDEXSTATE           (LVM_FIRST + 210)
#endif
#ifndef LVM_GETNEXTITEMINDEX
    #define LVM_GETNEXTITEMINDEX            (LVM_FIRST + 211)
#endif
#ifndef TVM_MAPACCIDTOHTREEITEM
    #define TVM_MAPACCIDTOHTREEITEM         (TV_FIRST + 42)
#endif
#ifndef TVM_MAPHTREEITEMTOACCID
    #define TVM_MAPHTREEITEMTOACCID         (TV_FIRST + 43)
#endif
#ifndef TVM_SETEXTENDEDSTYLE
    #define TVM_SETEXTENDEDSTYLE            (TV_FIRST + 44)
#endif
#ifndef TVM_GETEXTENDEDSTYLE
    #define TVM_GETEXTENDEDSTYLE            (TV_FIRST + 45)
#endif
#ifndef TVM_SETAUTOSCROLLINFO
    #define TVM_SETAUTOSCROLLINFO           (TV_FIRST + 59)
#endif
#ifndef TVM_GETSELECTEDCOUNT
    #define TVM_GETSELECTEDCOUNT            (TV_FIRST + 70)
#endif
#ifndef TVM_SHOWINFOTIP
    #define TVM_SHOWINFOTIP                 (TV_FIRST + 71)
#endif
#ifndef TVM_GETITEMPARTRECT
    #define TVM_GETITEMPARTRECT             (TV_FIRST + 72)
#endif
#ifndef MCM_GETCURRENTVIEW
    #define MCM_GETCURRENTVIEW              (MCM_FIRST + 22)
#endif
#ifndef MCM_GETCALENDARCOUNT
    #define MCM_GETCALENDARCOUNT            (MCM_FIRST + 23)
#endif
#ifndef MCM_GETCALENDARGRIDINFO
    #define MCM_GETCALENDARGRIDINFO         (MCM_FIRST + 24)
#endif
#ifndef MCM_GETCALID
    #define MCM_GETCALID                    (MCM_FIRST + 25)
#endif
#ifndef MCM_SETCALID
    #define MCM_SETCALID                    (MCM_FIRST + 26)
#endif
#ifndef MCM_SIZERECTTOMIN
    #define MCM_SIZERECTTOMIN               (MCM_FIRST + 27)
#endif
#ifndef MCM_SETCALENDARBORDER
    #define MCM_SETCALENDARBORDER           (MCM_FIRST + 28)
#endif
#ifndef MCM_GETCALENDARBORDER
    #define MCM_GETCALENDARBORDER           (MCM_FIRST + 29)
#endif
#ifndef MCM_SETCURRENTVIEW
    #define MCM_SETCURRENTVIEW              (MCM_FIRST + 30)
#endif
#ifndef DTM_SETMCSTYLE
    #define DTM_SETMCSTYLE                  (DTM_FIRST + 11)
#endif
#ifndef DTM_GETMCSTYLE
    #define DTM_GETMCSTYLE                  (DTM_FIRST + 12)
#endif
#ifndef DTM_CLOSEMONTHCAL
    #define DTM_CLOSEMONTHCAL               (DTM_FIRST + 13)
#endif
#ifndef DTM_GETDATETIMEPICKERINFO
    #define DTM_GETDATETIMEPICKERINFO       (DTM_FIRST + 14)
#endif
#ifndef DTM_GETIDEALSIZE
    #define DTM_GETIDEALSIZE                (DTM_FIRST + 15)
#endif
#ifndef ACM_ISPLAYING
    #define ACM_ISPLAYING                   (WM_USER + 104)
#endif

// RB_GETBANDINFO is defined differently in IE3 than in later versions.  This definition
// facilitates recognizing both versions of the message.  RB_GETBANDINFO_IE3 is not an
// official message name.
#define RB_GETBANDINFO_IE3  (WM_USER + 5)

