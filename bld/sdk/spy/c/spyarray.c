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
* Description:  Message and Control styles table for the spy.
*
****************************************************************************/


/* Include new common control styles that require recent versions of Windows. */
#ifdef __NT__
    #undef  _WIN32_IE
    #define _WIN32_IE 0x0A00
    #undef  _WIN32_WINNT
    #define _WIN32_WINNT 0x0A00
    #undef  WINVER
    #define WINVER 0x0A00
    #undef  NTDDI_VERSION
    #define NTDDI_VERSION 0x0A000000
#endif
#include "spy.h"
#include <dde.h>
#ifdef __NT__
    #include <commctrl.h>
#endif
#include "oswincls.h"


/**********************************************************
 * Messages undocumented for NT but documented for WINDOWS
 */
#ifndef WM_CTLCOLOR
    #define WM_CTLCOLOR                     0x0019
#endif
// Control styles
#ifndef CS_KEYCVTWINDOW
    #define CS_KEYCVTWINDOW                 0x0004
#endif
#ifndef CS_NOKEYCVT
    #define CS_NOKEYCVT                     0x0100
#endif

/**********************************************************
 * Messages undocumented for WINDOWS but documented for NT
 */
#ifndef WM_PAINTICON
    #define WM_PAINTICON                    0x0026
#endif
#ifndef WM_SETHOTKEY
    #define WM_SETHOTKEY                    0x0032
#endif
#ifndef WM_GETHOTKEY
    #define WM_GETHOTKEY                    0x0033
#endif
#ifndef WM_SYNCPAINT
    #define WM_SYNCPAINT                    0x0088
#endif
#ifndef WM_ENTERMENULOOP
    #define WM_ENTERMENULOOP                0x0211
#endif
#ifndef WM_EXITMENULOOP
    #define WM_EXITMENULOOP                 0x0212
#endif
#ifndef WM_NEXTMENU
    #define WM_NEXTMENU                     0x0213
#endif
#ifndef WM_ENTERSIZEMOVE
    #define WM_ENTERSIZEMOVE                0x0231
#endif
#ifndef WM_EXITSIZEMOVE
    #define WM_EXITSIZEMOVE                 0x0232
#endif
// Control styles
#ifndef BS_TYPEMASK
    #define BS_TYPEMASK                     0x000FL
#endif
#ifndef SS_TYPEMASK
    #define SS_TYPEMASK                     0x001FL
#endif

/**********************************************************
 * Messages undocumented for any version
 */
#ifndef WM_SIZEWAIT
    #define WM_SIZEWAIT                     0x0004
#endif
#ifndef WM_SETVISIBLE
    #define WM_SETVISIBLE                   0x0009
#endif
#ifndef WM_SYSTEMERROR
    #define WM_SYSTEMERROR                  0x0017
#endif
#ifndef WM_ATTABACTIVE
    #define WM_ATTABACTIVE                  0x0029
#endif
#ifndef WM_FILESYSCHANGE
    #define WM_FILESYSCHANGE                0x0034
#endif
#ifndef WM_ISACTIVEICON
    #define WM_ISACTIVEICON                 0x0035
#endif
#ifndef WM_QUERYPARKICON
    #define WM_QUERYPARKICON                0x0036
#endif
#ifndef WM_QUERYSAVESTATE
    #define WM_QUERYSAVESTATE               0x0038
#endif
#ifndef WM_ACTIVATESHELLWINDOW
    #define WM_ACTIVATESHELLWINDOW          0x003E
#endif
#ifndef WM_OTHERWINDOWCREATED
    #define WM_OTHERWINDOWCREATED           0x0042
#endif
#ifndef WM_OTHERWINDOWDESTROYED
    #define WM_OTHERWINDOWDESTROYED         0x0043
#endif
#ifndef WM_SYNCTASK
    #define WM_SYNCTASK                     0x0089
#endif
#ifndef WM_KLUDGEMINRECT
    #define WM_KLUDGEMINRECT                0x008B
#endif
#ifndef WM_UAHDESTROYWINDOW
    #define WM_UAHDESTROYWINDOW             0x0090
#endif
#ifndef WM_UAHDRAWMENU
    #define WM_UAHDRAWMENU                  0x0091
#endif
#ifndef WM_UAHDRAWMENUITEM
    #define WM_UAHDRAWMENUITEM              0x0092
#endif
#ifndef WM_UAHINITMENU
    #define WM_UAHINITMENU                  0x0093
#endif
#ifndef WM_UAHMEASUREMENUITEM
    #define WM_UAHMEASUREMENUITEM           0x0094
#endif
#ifndef WM_UAHNCPAINTMENUPOPUP
    #define WM_UAHNCPAINTMENUPOPUP          0x0095
#endif
#ifndef WM_NCUAHDRAWCAPTION
    #define WM_NCUAHDRAWCAPTION             0x00AE
#endif
#ifndef WM_NCUAHDRAWFRAME
    #define WM_NCUAHDRAWFRAME               0x00AF
#endif
#ifndef WM_YOMICHAR
    #define WM_YOMICHAR                     0x0108
#endif
#ifndef WM_CONVERTREQUEST
    #define WM_CONVERTREQUEST               0x010A
#endif
#ifndef WM_CONVERTRESULT
    #define WM_CONVERTRESULT                0x010B
#endif
#ifndef WM_INTERIM
    #define WM_INTERIM                      0x010C
#endif
#ifndef WM_SYSTIMER
    #define WM_SYSTIMER                     0x0118
#endif
#ifndef LBCB_STARTTRACK
    #define LBCB_STARTTRACK                 0x01AE
#endif
#ifndef LBCB_ENDTRACK
    #define LBCB_ENDTRACK                   0x01AF
#endif
#ifndef WM_LBTRACKPOINT
    #define WM_LBTRACKPOINT                 0x0131
#endif
#ifndef WM_DROPOBJECT
    #define WM_DROPOBJECT                   0x022A
#endif
#ifndef WM_QUERYDROPOBJECT
    #define WM_QUERYDROPOBJECT              0x022B
#endif
#ifndef WM_BEGINDRAG
    #define WM_BEGINDRAG                    0x022C
#endif
#ifndef WM_DRAGLOOP
    #define WM_DRAGLOOP                     0x022D
#endif
#ifndef WM_DRAGSELECT
    #define WM_DRAGSELECT                   0x022E
#endif
#ifndef WM_DRAGMOVE
    #define WM_DRAGMOVE                     0x022F
#endif
#ifndef WM_IME_SYSTEM
    #define WM_IME_SYSTEM                   0x0287
#endif
#ifndef WM_COALESCE_FIRST
    #define WM_COALESCE_FIRST               0x0390
#endif
#ifndef WM_COALESCE_LAST
    #define WM_COALESCE_LAST                0x039F
#endif


// RB_GETBANDINFO is defined differently in IE3 than in later versions.  This definition
// facilitates recognizing both versions of the message.  RB_GETBANDINFO_IE3 is not an
// official message name.
#define RB_GETBANDINFO_IE3                  (WM_USER + 5)

/****************************************************************************/

#define msgpick1(a,b)       { true, false, a, #a, b, 0L },
#define msgpick1x(a,b,c)    { true, false, a, b, c, 0L },
#ifdef __WINDOWS__
#define msgpick2(a,b)
#else
#define msgpick2(a,b)       { true, false, a, #a, b, 0L },
#endif
#define msgpick3(a)         { true, false, a, #a, MC_CONTROL, 0L },
#define msgpick3a(a)        { true, false, a ## A, #a "(A)", MC_CONTROL, 0L },
#define msgpick3w(a)        { true, false, a ## W, #a "(W)", MC_CONTROL, 0L },
#ifdef __WINDOWS__
#define msgpick4(a)
#else
#define msgpick4(a)         { true, false, a, #a, MC_CONTROL, 0L },
#endif

static message _NEAR MessageArray[] =  {
    #include "spyarray.h"
};

#define MESSAGEARRAYSIZE            (sizeof( MessageArray )/sizeof( message ))

static message _NEAR EditMessageArray[] = {
    msgpick3(  EM_GETSEL )
    msgpick3(  EM_SETSEL )
    msgpick3(  EM_GETRECT )
    msgpick3(  EM_SETRECT )
    msgpick3(  EM_SETRECTNP )
    msgpick4(  EM_SCROLL )
    msgpick3(  EM_LINESCROLL )
    msgpick4(  EM_SCROLLCARET )
    msgpick3(  EM_GETMODIFY )
    msgpick3(  EM_SETMODIFY )
    msgpick3(  EM_GETLINECOUNT )
    msgpick3(  EM_LINEINDEX )
    msgpick3(  EM_SETHANDLE )
    msgpick3(  EM_GETHANDLE )
    msgpick4(  EM_GETTHUMB )
    msgpick3(  EM_LINELENGTH )
    msgpick3(  EM_REPLACESEL )
    msgpick3(  EM_GETLINE )
    msgpick3(  EM_LIMITTEXT )
    msgpick3(  EM_CANUNDO )
    msgpick3(  EM_UNDO )
    msgpick3(  EM_FMTLINES )
    msgpick3(  EM_LINEFROMCHAR )
    msgpick3(  EM_SETTABSTOPS )
    msgpick3(  EM_SETPASSWORDCHAR )
    msgpick3(  EM_EMPTYUNDOBUFFER )
    msgpick3(  EM_GETFIRSTVISIBLELINE )
    msgpick3(  EM_SETREADONLY )
    msgpick3(  EM_SETWORDBREAKPROC )
    msgpick3(  EM_GETWORDBREAKPROC )
    msgpick3(  EM_GETPASSWORDCHAR )
    msgpick4(  EM_SETMARGINS )
    msgpick4(  EM_GETMARGINS )
    msgpick4(  EM_GETLIMITTEXT )
    msgpick4(  EM_POSFROMCHAR )
    msgpick4(  EM_CHARFROMPOS )
    msgpick4(  EM_SETIMESTATUS )
    msgpick4(  EM_GETIMESTATUS )
};

#define EDITMESSAGEARRAYSIZE        (sizeof( EditMessageArray )/sizeof( message ))

static message _NEAR ButtonMessageArray[] = {
    msgpick3(  BM_GETCHECK )
    msgpick3(  BM_SETCHECK )
    msgpick3(  BM_GETSTATE )
    msgpick3(  BM_SETSTATE )
    msgpick3(  BM_SETSTYLE )
    msgpick4(  BM_CLICK )
    msgpick4(  BM_GETIMAGE )
    msgpick4(  BM_SETIMAGE )
    msgpick4(  BM_SETDONTCLICK )
};

#define BUTTONMESSAGEARRAYSIZE      (sizeof( ButtonMessageArray )/sizeof( message ))

static message _NEAR StaticMessageArray[] = {
    msgpick3(  STM_SETICON )
    msgpick3(  STM_GETICON )
    msgpick4(  STM_SETIMAGE )
    msgpick4(  STM_GETIMAGE )
};

#define STATICMESSAGEARRAYSIZE      (sizeof( StaticMessageArray )/sizeof( message ))

static message _NEAR ListBoxMessageArray[] = {
    msgpick3(  LB_ADDSTRING )
    msgpick3(  LB_INSERTSTRING )
    msgpick3(  LB_DELETESTRING )
    msgpick4(  LB_SELITEMRANGEEX )
    msgpick3(  LB_RESETCONTENT )
    msgpick3(  LB_SETSEL )
    msgpick3(  LB_SETCURSEL )
    msgpick3(  LB_GETSEL )
    msgpick3(  LB_GETCURSEL )
    msgpick3(  LB_GETTEXT )
    msgpick3(  LB_GETTEXTLEN )
    msgpick3(  LB_GETCOUNT )
    msgpick3(  LB_SELECTSTRING )
    msgpick3(  LB_DIR )
    msgpick3(  LB_GETTOPINDEX )
    msgpick3(  LB_FINDSTRING )
    msgpick3(  LB_GETSELCOUNT )
    msgpick3(  LB_GETSELITEMS )
    msgpick3(  LB_SETTABSTOPS )
    msgpick3(  LB_GETHORIZONTALEXTENT )
    msgpick3(  LB_SETHORIZONTALEXTENT )
    msgpick3(  LB_SETCOLUMNWIDTH )
    msgpick4(  LB_ADDFILE )
    msgpick3(  LB_SETTOPINDEX )
    msgpick3(  LB_GETITEMRECT )
    msgpick3(  LB_GETITEMDATA )
    msgpick3(  LB_SETITEMDATA )
    msgpick3(  LB_SELITEMRANGE )
    msgpick4(  LB_SETANCHORINDEX )
    msgpick4(  LB_GETANCHORINDEX )
    msgpick3(  LB_SETCARETINDEX )
    msgpick3(  LB_GETCARETINDEX )
    msgpick3(  LB_SETITEMHEIGHT )
    msgpick3(  LB_GETITEMHEIGHT )
    msgpick3(  LB_FINDSTRINGEXACT )
    msgpick4(  LB_SETLOCALE )
    msgpick4(  LB_GETLOCALE )
    msgpick4(  LB_SETCOUNT )
    msgpick4(  LB_INITSTORAGE )
    msgpick4(  LB_ITEMFROMPOINT )
    msgpick4(  LB_GETLISTBOXINFO )
};

#define LISTBOXMESSAGEARRAYSIZE     (sizeof( ListBoxMessageArray )/sizeof( message ))

#define combobox_messages_old() \
    msgpick3(  CB_GETEDITSEL ) \
    msgpick3(  CB_LIMITTEXT ) \
    msgpick3(  CB_SETEDITSEL ) \
    msgpick3(  CB_ADDSTRING ) \
    msgpick3(  CB_DELETESTRING ) \
    msgpick3(  CB_DIR ) \
    msgpick3(  CB_GETCOUNT ) \
    msgpick3(  CB_GETCURSEL ) \
    msgpick3(  CB_GETLBTEXT ) \
    msgpick3(  CB_GETLBTEXTLEN ) \
    msgpick3(  CB_INSERTSTRING ) \
    msgpick3(  CB_RESETCONTENT ) \
    msgpick3(  CB_FINDSTRING ) \
    msgpick3(  CB_SELECTSTRING ) \
    msgpick3(  CB_SETCURSEL ) \
    msgpick3(  CB_SHOWDROPDOWN ) \
    msgpick3(  CB_GETITEMDATA ) \
    msgpick3(  CB_SETITEMDATA ) \
    msgpick3(  CB_GETDROPPEDCONTROLRECT ) \
    msgpick3(  CB_SETITEMHEIGHT ) \
    msgpick3(  CB_GETITEMHEIGHT ) \
    msgpick3(  CB_SETEXTENDEDUI ) \
    msgpick3(  CB_GETEXTENDEDUI ) \
    msgpick3(  CB_GETDROPPEDSTATE ) \
    msgpick3(  CB_FINDSTRINGEXACT )

#define combobox_messages_new() \
    msgpick3(  CB_SETLOCALE ) \
    msgpick3(  CB_GETLOCALE ) \
    msgpick3(  CB_GETTOPINDEX ) \
    msgpick3(  CB_SETTOPINDEX ) \
    msgpick3(  CB_GETHORIZONTALEXTENT ) \
    msgpick3(  CB_SETHORIZONTALEXTENT ) \
    msgpick3(  CB_GETDROPPEDWIDTH ) \
    msgpick3(  CB_SETDROPPEDWIDTH ) \
    msgpick3(  CB_INITSTORAGE ) \
    msgpick3(  CB_GETCOMBOBOXINFO )

static message _NEAR ComboBoxMessageArray[] = {
    combobox_messages_old()
#ifdef __NT__
    combobox_messages_new()
#endif
};

#define COMBOBOXMESSAGEARRAYSIZE    (sizeof( ComboBoxMessageArray )/sizeof( message ))

#ifdef __NT__
static message _NEAR ScrollBarMessageArray[] = {
    msgpick3(  SBM_SETPOS )
    msgpick3(  SBM_GETPOS )
    msgpick3(  SBM_SETRANGE )
    msgpick3(  SBM_SETRANGEREDRAW )
    msgpick3(  SBM_GETRANGE )
    msgpick3(  SBM_ENABLE_ARROWS )
    msgpick3(  SBM_SETSCROLLINFO )
    msgpick3(  SBM_GETSCROLLINFO )
    msgpick3(  SBM_GETSCROLLBARINFO )
};

#define SCROLLBARMESSAGEARRAYSIZE   (sizeof( ScrollBarMessageArray )/sizeof( message ))

static message _NEAR HeaderMessageArray[] = {
    msgpick3(  HDM_GETITEMCOUNT )
    msgpick3a( HDM_INSERTITEM )
    msgpick3(  HDM_DELETEITEM )
    msgpick3a( HDM_GETITEM )
    msgpick3a( HDM_SETITEM )
    msgpick3(  HDM_LAYOUT )
    msgpick3(  HDM_HITTEST )
    msgpick3(  HDM_GETITEMRECT )
    msgpick3(  HDM_SETIMAGELIST )
    msgpick3(  HDM_GETIMAGELIST )
    msgpick3w( HDM_INSERTITEM )
    msgpick3w( HDM_GETITEM )
    msgpick3w( HDM_SETITEM )
    msgpick3(  HDM_ORDERTOINDEX )
    msgpick3(  HDM_CREATEDRAGIMAGE )
    msgpick3(  HDM_GETORDERARRAY )
    msgpick3(  HDM_SETORDERARRAY )
    msgpick3(  HDM_SETHOTDIVIDER )
    msgpick3(  HDM_SETBITMAPMARGIN )
    msgpick3(  HDM_GETBITMAPMARGIN )
    msgpick3(  HDM_SETFILTERCHANGETIMEOUT )
    msgpick3(  HDM_EDITFILTER )
    msgpick3(  HDM_CLEARFILTER )
    msgpick3(  HDM_GETITEMDROPDOWNRECT )
    msgpick3(  HDM_GETOVERFLOWRECT )
    msgpick3(  HDM_GETFOCUSEDITEM )
    msgpick3(  HDM_SETFOCUSEDITEM )
};

#define HEADERMESSAGEARRAYSIZE      (sizeof( HeaderMessageArray )/sizeof( message ))

static message _NEAR ToolbarMessageArray[] = {
    msgpick3(  TB_ENABLEBUTTON )
    msgpick3(  TB_CHECKBUTTON )
    msgpick3(  TB_PRESSBUTTON )
    msgpick3(  TB_HIDEBUTTON )
    msgpick3(  TB_INDETERMINATE )
    msgpick3(  TB_MARKBUTTON )
    msgpick3(  TB_ISBUTTONENABLED )
    msgpick3(  TB_ISBUTTONCHECKED )
    msgpick3(  TB_ISBUTTONPRESSED )
    msgpick3(  TB_ISBUTTONHIDDEN )
    msgpick3(  TB_ISBUTTONINDETERMINATE )
    msgpick3(  TB_ISBUTTONHIGHLIGHTED )
    msgpick3(  TB_SETSTATE )
    msgpick3(  TB_GETSTATE )
    msgpick3(  TB_ADDBITMAP )
    msgpick3a( TB_ADDBUTTONS )
    msgpick3a( TB_INSERTBUTTON )
    msgpick3(  TB_DELETEBUTTON )
    msgpick3(  TB_GETBUTTON )
    msgpick3(  TB_BUTTONCOUNT )
    msgpick3(  TB_COMMANDTOINDEX )
    msgpick3a( TB_SAVERESTORE )
    msgpick3(  TB_CUSTOMIZE )
    msgpick3a( TB_ADDSTRING )
    msgpick3(  TB_GETITEMRECT )
    msgpick3(  TB_BUTTONSTRUCTSIZE )
    msgpick3(  TB_SETBUTTONSIZE )
    msgpick3(  TB_SETBITMAPSIZE )
    msgpick3(  TB_AUTOSIZE )
    msgpick3(  TB_GETTOOLTIPS )
    msgpick3(  TB_SETTOOLTIPS )
    msgpick3(  TB_SETPARENT )
    msgpick3(  TB_SETROWS )
    msgpick3(  TB_GETROWS )
    msgpick3(  TB_GETBITMAPFLAGS )
    msgpick3(  TB_SETCMDID )
    msgpick3(  TB_CHANGEBITMAP )
    msgpick3(  TB_GETBITMAP )
    msgpick3a( TB_GETBUTTONTEXT )
    msgpick3(  TB_REPLACEBITMAP )
    msgpick3(  TB_SETINDENT )
    msgpick3(  TB_SETIMAGELIST )
    msgpick3(  TB_GETIMAGELIST )
    msgpick3(  TB_LOADIMAGES )
    msgpick3(  TB_GETRECT )
    msgpick3(  TB_SETHOTIMAGELIST )
    msgpick3(  TB_GETHOTIMAGELIST )
    msgpick3(  TB_SETDISABLEDIMAGELIST )
    msgpick3(  TB_GETDISABLEDIMAGELIST )
    msgpick3(  TB_SETSTYLE )
    msgpick3(  TB_GETSTYLE )
    msgpick3(  TB_GETBUTTONSIZE )
    msgpick3(  TB_SETBUTTONWIDTH )
    msgpick3(  TB_SETMAXTEXTROWS )
    msgpick3(  TB_GETTEXTROWS )
    msgpick3(  TB_GETOBJECT )
    msgpick3w( TB_GETBUTTONINFO )
    msgpick3w( TB_SETBUTTONINFO )
    msgpick3a( TB_GETBUTTONINFO )
    msgpick3a( TB_SETBUTTONINFO )
    msgpick3w( TB_INSERTBUTTON )
    msgpick3w( TB_ADDBUTTONS )
    msgpick3(  TB_HITTEST )
    msgpick3(  TB_SETDRAWTEXTFLAGS )
    msgpick3(  TB_SETHOTITEM )
    msgpick3(  TB_GETHOTITEM )
    msgpick3(  TB_SETANCHORHIGHLIGHT )
    msgpick3(  TB_GETANCHORHIGHLIGHT )
    msgpick3w( TB_GETBUTTONTEXT )
    msgpick3w( TB_SAVERESTORE )
    msgpick3w( TB_ADDSTRING )
    msgpick3a( TB_MAPACCELERATOR )
    msgpick3(  TB_GETINSERTMARK )
    msgpick3(  TB_SETINSERTMARK )
    msgpick3(  TB_INSERTMARKHITTEST )
    msgpick3(  TB_MOVEBUTTON )
    msgpick3(  TB_GETMAXSIZE )
    msgpick3(  TB_SETEXTENDEDSTYLE )
    msgpick3(  TB_GETEXTENDEDSTYLE )
    msgpick3(  TB_GETPADDING )
    msgpick3(  TB_SETPADDING )
    msgpick3(  TB_SETINSERTMARKCOLOR )
    msgpick3(  TB_GETINSERTMARKCOLOR )
    msgpick3w( TB_MAPACCELERATOR )
    msgpick3w( TB_GETSTRING )
    msgpick3a( TB_GETSTRING )
    msgpick3(  TB_GETMETRICS )
    msgpick3(  TB_SETMETRICS )
    msgpick3(  TB_SETPRESSEDIMAGELIST )
    msgpick3(  TB_GETPRESSEDIMAGELIST )
};

#define TOOLBARMESSAGEARRAYSIZE     (sizeof( ToolbarMessageArray )/sizeof( message ))

static message _NEAR RebarMessageArray[] = {
    msgpick3a( RB_INSERTBAND )
    msgpick3(  RB_DELETEBAND )
    msgpick3(  RB_GETBARINFO )
    msgpick3(  RB_SETBARINFO )
    msgpick3(  RB_GETBANDINFO_IE3 )
    msgpick3a( RB_SETBANDINFO )
    msgpick3(  RB_SETPARENT )
    msgpick3(  RB_HITTEST )
    msgpick3(  RB_GETRECT )
    msgpick3w( RB_INSERTBAND )
    msgpick3w( RB_SETBANDINFO )
    msgpick3(  RB_GETBANDCOUNT )
    msgpick3(  RB_GETROWCOUNT )
    msgpick3(  RB_GETROWHEIGHT )
    msgpick3(  RB_IDTOINDEX )
    msgpick3(  RB_GETTOOLTIPS )
    msgpick3(  RB_SETTOOLTIPS )
    msgpick3(  RB_SETBKCOLOR )
    msgpick3(  RB_GETBKCOLOR )
    msgpick3(  RB_SETTEXTCOLOR )
    msgpick3(  RB_GETTEXTCOLOR )
    msgpick3(  RB_SIZETORECT )
    msgpick3(  RB_BEGINDRAG )
    msgpick3(  RB_ENDDRAG )
    msgpick3(  RB_DRAGMOVE )
    msgpick3(  RB_GETBARHEIGHT )
    msgpick3w( RB_GETBANDINFO )
    msgpick3a( RB_GETBANDINFO )
    msgpick3(  RB_MINIMIZEBAND )
    msgpick3(  RB_MAXIMIZEBAND )
    msgpick3(  RB_GETBANDBORDERS )
    msgpick3(  RB_SHOWBAND )
    msgpick3(  RB_SETPALETTE )
    msgpick3(  RB_GETPALETTE )
    msgpick3(  RB_MOVEBAND )
    msgpick3(  RB_GETBANDMARGINS )
    msgpick3(  RB_SETEXTENDEDSTYLE )
    msgpick3(  RB_GETEXTENDEDSTYLE )
    msgpick3(  RB_PUSHCHEVRON )
    msgpick3(  RB_SETBANDWIDTH )
};

#define REBARMESSAGEARRAYSIZE       (sizeof( RebarMessageArray )/sizeof( message ))

static message _NEAR ToolTipsMessageArray[] = {
    msgpick3(  TTM_ACTIVATE )
    msgpick3(  TTM_SETDELAYTIME )
    msgpick3a( TTM_ADDTOOL )
    msgpick3a( TTM_DELTOOL )
    msgpick3a( TTM_NEWTOOLRECT )
    msgpick3(  TTM_RELAYEVENT )
    msgpick3a( TTM_GETTOOLINFO )
    msgpick3a( TTM_SETTOOLINFO )
    msgpick3a( TTM_HITTEST )
    msgpick3a( TTM_GETTEXT )
    msgpick3a( TTM_UPDATETIPTEXT )
    msgpick3(  TTM_GETTOOLCOUNT )
    msgpick3a( TTM_ENUMTOOLS )
    msgpick3a( TTM_GETCURRENTTOOL )
    msgpick3(  TTM_WINDOWFROMPOINT )
    msgpick3(  TTM_TRACKACTIVATE )
    msgpick3(  TTM_TRACKPOSITION )
    msgpick3(  TTM_SETTIPBKCOLOR )
    msgpick3(  TTM_SETTIPTEXTCOLOR )
    msgpick3(  TTM_GETDELAYTIME )
    msgpick3(  TTM_GETTIPBKCOLOR )
    msgpick3(  TTM_GETTIPTEXTCOLOR )
    msgpick3(  TTM_SETMAXTIPWIDTH )
    msgpick3(  TTM_GETMAXTIPWIDTH )
    msgpick3(  TTM_SETMARGIN )
    msgpick3(  TTM_GETMARGIN )
    msgpick3(  TTM_POP )
    msgpick3(  TTM_UPDATE )
    msgpick3(  TTM_GETBUBBLESIZE )
    msgpick3(  TTM_ADJUSTRECT )
    msgpick3a( TTM_SETTITLE )
    msgpick3w( TTM_SETTITLE )
    msgpick3(  TTM_POPUP )
    msgpick3(  TTM_GETTITLE )
    msgpick3w( TTM_ADDTOOL )
    msgpick3w( TTM_DELTOOL )
    msgpick3w( TTM_NEWTOOLRECT )
    msgpick3w( TTM_GETTOOLINFO )
    msgpick3w( TTM_SETTOOLINFO )
    msgpick3w( TTM_HITTEST )
    msgpick3w( TTM_GETTEXT )
    msgpick3w( TTM_UPDATETIPTEXT )
    msgpick3w( TTM_ENUMTOOLS )
    msgpick3w( TTM_GETCURRENTTOOL )
};

#define TOOLTIPSMESSAGEARRAYSIZE    (sizeof( ToolTipsMessageArray )/sizeof( message ))

static message _NEAR StatusBarMessageArray[] = {
    msgpick3a( SB_SETTEXT )
    msgpick3a( SB_GETTEXT )
    msgpick3a( SB_GETTEXTLENGTH )
    msgpick3(  SB_SETPARTS )
    msgpick3(  SB_GETPARTS )
    msgpick3(  SB_GETBORDERS )
    msgpick3(  SB_SETMINHEIGHT )
    msgpick3(  SB_SIMPLE )
    msgpick3(  SB_GETRECT )
    msgpick3w( SB_SETTEXT )
    msgpick3w( SB_GETTEXTLENGTH )
    msgpick3w( SB_GETTEXT )
    msgpick3(  SB_ISSIMPLE )
    msgpick3(  SB_SETICON )
    msgpick3a( SB_SETTIPTEXT )
    msgpick3w( SB_SETTIPTEXT )
    msgpick3a( SB_GETTIPTEXT )
    msgpick3w( SB_GETTIPTEXT )
    msgpick3(  SB_GETICON )
};

#define STATUSBARMESSAGEARRAYSIZE   (sizeof( StatusBarMessageArray )/sizeof( message ))

static message _NEAR TrackBarMessageArray[] = {
    msgpick3(  TBM_GETPOS )
    msgpick3(  TBM_GETRANGEMIN )
    msgpick3(  TBM_GETRANGEMAX )
    msgpick3(  TBM_GETTIC )
    msgpick3(  TBM_SETTIC )
    msgpick3(  TBM_SETPOS )
    msgpick3(  TBM_SETRANGE )
    msgpick3(  TBM_SETRANGEMIN )
    msgpick3(  TBM_SETRANGEMAX )
    msgpick3(  TBM_CLEARTICS )
    msgpick3(  TBM_SETSEL )
    msgpick3(  TBM_SETSELSTART )
    msgpick3(  TBM_SETSELEND )
    msgpick3(  TBM_GETPTICS )
    msgpick3(  TBM_GETTICPOS )
    msgpick3(  TBM_GETNUMTICS )
    msgpick3(  TBM_GETSELSTART )
    msgpick3(  TBM_GETSELEND )
    msgpick3(  TBM_CLEARSEL )
    msgpick3(  TBM_SETTICFREQ )
    msgpick3(  TBM_SETPAGESIZE )
    msgpick3(  TBM_GETPAGESIZE )
    msgpick3(  TBM_SETLINESIZE )
    msgpick3(  TBM_GETLINESIZE )
    msgpick3(  TBM_GETTHUMBRECT )
    msgpick3(  TBM_GETCHANNELRECT )
    msgpick3(  TBM_SETTHUMBLENGTH )
    msgpick3(  TBM_GETTHUMBLENGTH )
    msgpick3(  TBM_SETTOOLTIPS )
    msgpick3(  TBM_GETTOOLTIPS )
    msgpick3(  TBM_SETTIPSIDE )
    msgpick3(  TBM_SETBUDDY )
    msgpick3(  TBM_GETBUDDY )
};

#define TRACKBARMESSAGEARRAYSIZE    (sizeof( TrackBarMessageArray )/sizeof( message ))

static message _NEAR UpDownMessageArray[] = {
    msgpick3(  UDM_SETRANGE )
    msgpick3(  UDM_GETRANGE )
    msgpick3(  UDM_SETPOS )
    msgpick3(  UDM_GETPOS )
    msgpick3(  UDM_SETBUDDY )
    msgpick3(  UDM_GETBUDDY )
    msgpick3(  UDM_SETACCEL )
    msgpick3(  UDM_GETACCEL )
    msgpick3(  UDM_SETBASE )
    msgpick3(  UDM_GETBASE )
    msgpick3(  UDM_SETRANGE32 )
    msgpick3(  UDM_GETRANGE32 )
    msgpick3(  UDM_SETPOS32 )
    msgpick3(  UDM_GETPOS32 )
};

#define UPDOWNMESSAGEARRAYSIZE      (sizeof( UpDownMessageArray )/sizeof( message ))

static message _NEAR ProgressBarMessageArray[] = {
    msgpick3(  PBM_SETRANGE )
    msgpick3(  PBM_SETPOS )
    msgpick3(  PBM_DELTAPOS )
    msgpick3(  PBM_SETSTEP )
    msgpick3(  PBM_STEPIT )
    msgpick3(  PBM_SETRANGE32 )
    msgpick3(  PBM_GETRANGE )
    msgpick3(  PBM_GETPOS )
    msgpick3(  PBM_SETBARCOLOR )
    msgpick3(  PBM_SETMARQUEE )
    msgpick3(  PBM_GETSTEP )
    msgpick3(  PBM_GETBKCOLOR )
    msgpick3(  PBM_GETBARCOLOR )
    msgpick3(  PBM_SETSTATE )
    msgpick3(  PBM_GETSTATE )
};

#define PROGRESSBARMESSAGEARRAYSIZE (sizeof( ProgressBarMessageArray )/sizeof( message ))

static message _NEAR HotKeyMessageArray[] = {
    msgpick3(  HKM_SETHOTKEY )
    msgpick3(  HKM_GETHOTKEY )
    msgpick3(  HKM_SETRULES )
};

#define HOTKEYMESSAGEARRAYSIZE      (sizeof( HotKeyMessageArray )/sizeof( message ))

static message _NEAR ListViewMessageArray[] = {
    msgpick3(  LVM_GETBKCOLOR )
    msgpick3(  LVM_SETBKCOLOR )
    msgpick3(  LVM_GETIMAGELIST )
    msgpick3(  LVM_SETIMAGELIST )
    msgpick3(  LVM_GETITEMCOUNT )
    msgpick3a( LVM_GETITEM )
    msgpick3a( LVM_SETITEM )
    msgpick3a( LVM_INSERTITEM )
    msgpick3(  LVM_DELETEITEM )
    msgpick3(  LVM_DELETEALLITEMS )
    msgpick3(  LVM_GETCALLBACKMASK )
    msgpick3(  LVM_SETCALLBACKMASK )
    msgpick3(  LVM_GETNEXTITEM )
    msgpick3a( LVM_FINDITEM )
    msgpick3(  LVM_GETITEMRECT )
    msgpick3(  LVM_SETITEMPOSITION )
    msgpick3(  LVM_GETITEMPOSITION )
    msgpick3a( LVM_GETSTRINGWIDTH )
    msgpick3(  LVM_HITTEST )
    msgpick3(  LVM_ENSUREVISIBLE )
    msgpick3(  LVM_SCROLL )
    msgpick3(  LVM_REDRAWITEMS )
    msgpick3(  LVM_ARRANGE )
    msgpick3a( LVM_EDITLABEL )
    msgpick3(  LVM_GETEDITCONTROL )
    msgpick3a( LVM_GETCOLUMN )
    msgpick3a( LVM_SETCOLUMN )
    msgpick3a( LVM_INSERTCOLUMN )
    msgpick3(  LVM_DELETECOLUMN )
    msgpick3(  LVM_GETCOLUMNWIDTH )
    msgpick3(  LVM_SETCOLUMNWIDTH )
    msgpick3(  LVM_GETHEADER )
    msgpick3(  LVM_CREATEDRAGIMAGE )
    msgpick3(  LVM_GETVIEWRECT )
    msgpick3(  LVM_GETTEXTCOLOR )
    msgpick3(  LVM_SETTEXTCOLOR )
    msgpick3(  LVM_GETTEXTBKCOLOR )
    msgpick3(  LVM_SETTEXTBKCOLOR )
    msgpick3(  LVM_GETTOPINDEX )
    msgpick3(  LVM_GETCOUNTPERPAGE )
    msgpick3(  LVM_GETORIGIN )
    msgpick3(  LVM_UPDATE )
    msgpick3(  LVM_SETITEMSTATE )
    msgpick3(  LVM_GETITEMSTATE )
    msgpick3a( LVM_GETITEMTEXT )
    msgpick3a( LVM_SETITEMTEXT )
    msgpick3(  LVM_SETITEMCOUNT )
    msgpick3(  LVM_SORTITEMS )
    msgpick3(  LVM_SETITEMPOSITION32 )
    msgpick3(  LVM_GETSELECTEDCOUNT )
    msgpick3(  LVM_GETITEMSPACING )
    msgpick3a( LVM_GETISEARCHSTRING )
    msgpick3(  LVM_SETICONSPACING )
    msgpick3(  LVM_SETEXTENDEDLISTVIEWSTYLE )
    msgpick3(  LVM_GETEXTENDEDLISTVIEWSTYLE )
    msgpick3(  LVM_GETSUBITEMRECT )
    msgpick3(  LVM_SUBITEMHITTEST )
    msgpick3(  LVM_SETCOLUMNORDERARRAY )
    msgpick3(  LVM_GETCOLUMNORDERARRAY )
    msgpick3(  LVM_SETHOTITEM )
    msgpick3(  LVM_GETHOTITEM )
    msgpick3(  LVM_SETHOTCURSOR )
    msgpick3(  LVM_GETHOTCURSOR )
    msgpick3(  LVM_APPROXIMATEVIEWRECT )
    msgpick3(  LVM_SETWORKAREAS )
    msgpick3(  LVM_GETSELECTIONMARK )
    msgpick3(  LVM_SETSELECTIONMARK )
    msgpick3a( LVM_SETBKIMAGE )
    msgpick3a( LVM_GETBKIMAGE )
    msgpick3(  LVM_GETWORKAREAS )
    msgpick3(  LVM_SETHOVERTIME )
    msgpick3(  LVM_GETHOVERTIME )
    msgpick3(  LVM_GETNUMBEROFWORKAREAS )
    msgpick3(  LVM_SETTOOLTIPS )
    msgpick3w( LVM_GETITEM )
    msgpick3w( LVM_SETITEM )
    msgpick3w( LVM_INSERTITEM )
    msgpick3(  LVM_GETTOOLTIPS )
    msgpick3(  LVM_SORTITEMSEX )
    msgpick3w( LVM_FINDITEM )
    msgpick3w( LVM_GETSTRINGWIDTH )
    msgpick3(  LVM_GETGROUPSTATE )
    msgpick3(  LVM_GETFOCUSEDGROUP )
    msgpick3w( LVM_GETCOLUMN )
    msgpick3w( LVM_SETCOLUMN )
    msgpick3w( LVM_INSERTCOLUMN )
    msgpick3(  LVM_GETGROUPRECT )
    msgpick3w( LVM_GETITEMTEXT )
    msgpick3w( LVM_SETITEMTEXT )
    msgpick3w( LVM_GETISEARCHSTRING )
    msgpick3w( LVM_EDITLABEL )
    msgpick3w( LVM_SETBKIMAGE )
    msgpick3w( LVM_GETBKIMAGE )
    msgpick3(  LVM_SETSELECTEDCOLUMN )
    msgpick3(  LVM_SETVIEW )
    msgpick3(  LVM_GETVIEW )
    msgpick3(  LVM_INSERTGROUP )
    msgpick3(  LVM_SETGROUPINFO )
    msgpick3(  LVM_GETGROUPINFO )
    msgpick3(  LVM_REMOVEGROUP )
    msgpick3(  LVM_MOVEGROUP )
    msgpick3(  LVM_GETGROUPCOUNT )
    msgpick3(  LVM_GETGROUPINFOBYINDEX )
    msgpick3(  LVM_MOVEITEMTOGROUP )
    msgpick3(  LVM_SETGROUPMETRICS )
    msgpick3(  LVM_GETGROUPMETRICS )
    msgpick3(  LVM_ENABLEGROUPVIEW )
    msgpick3(  LVM_SORTGROUPS )
    msgpick3(  LVM_INSERTGROUPSORTED )
    msgpick3(  LVM_REMOVEALLGROUPS )
    msgpick3(  LVM_HASGROUP )
    msgpick3(  LVM_SETTILEVIEWINFO )
    msgpick3(  LVM_GETTILEVIEWINFO )
    msgpick3(  LVM_SETTILEINFO )
    msgpick3(  LVM_GETTILEINFO )
    msgpick3(  LVM_SETINSERTMARK )
    msgpick3(  LVM_GETINSERTMARK )
    msgpick3(  LVM_INSERTMARKHITTEST )
    msgpick3(  LVM_GETINSERTMARKRECT )
    msgpick3(  LVM_SETINSERTMARKCOLOR )
    msgpick3(  LVM_GETINSERTMARKCOLOR )
    msgpick3(  LVM_SETINFOTIP )
    msgpick3(  LVM_GETSELECTEDCOLUMN )
    msgpick3(  LVM_ISGROUPVIEWENABLED )
    msgpick3(  LVM_GETOUTLINECOLOR )
    msgpick3(  LVM_SETOUTLINECOLOR )
    msgpick3(  LVM_CANCELEDITLABEL )
    msgpick3(  LVM_MAPINDEXTOID )
    msgpick3(  LVM_MAPIDTOINDEX )
    msgpick3(  LVM_ISITEMVISIBLE )
    msgpick3(  LVM_GETEMPTYTEXT )
    msgpick3(  LVM_GETFOOTERRECT )
    msgpick3(  LVM_GETFOOTERINFO )
    msgpick3(  LVM_GETFOOTERITEMRECT )
    msgpick3(  LVM_GETFOOTERITEM )
    msgpick3(  LVM_GETITEMINDEXRECT )
    msgpick3(  LVM_SETITEMINDEXSTATE )
    msgpick3(  LVM_GETNEXTITEMINDEX )
};

#define LISTVIEWMESSAGEARRAYSIZE    (sizeof( ListViewMessageArray )/sizeof( message ))

static message _NEAR TreeViewMessageArray[] = {
    msgpick3a( TVM_INSERTITEM )
    msgpick3(  TVM_DELETEITEM )
    msgpick3(  TVM_EXPAND )
    msgpick3(  TVM_GETITEMRECT )
    msgpick3(  TVM_GETCOUNT )
    msgpick3(  TVM_GETINDENT )
    msgpick3(  TVM_SETINDENT )
    msgpick3(  TVM_GETIMAGELIST )
    msgpick3(  TVM_SETIMAGELIST )
    msgpick3(  TVM_GETNEXTITEM )
    msgpick3(  TVM_SELECTITEM )
    msgpick3a( TVM_GETITEM )
    msgpick3a( TVM_SETITEM )
    msgpick3a( TVM_EDITLABEL )
    msgpick3(  TVM_GETEDITCONTROL )
    msgpick3(  TVM_GETVISIBLECOUNT )
    msgpick3(  TVM_HITTEST )
    msgpick3(  TVM_CREATEDRAGIMAGE )
    msgpick3(  TVM_SORTCHILDREN )
    msgpick3(  TVM_ENSUREVISIBLE )
    msgpick3(  TVM_SORTCHILDRENCB )
    msgpick3(  TVM_ENDEDITLABELNOW )
    msgpick3a( TVM_GETISEARCHSTRING )
    msgpick3(  TVM_SETTOOLTIPS )
    msgpick3(  TVM_GETTOOLTIPS )
    msgpick3(  TVM_SETINSERTMARK )
    msgpick3(  TVM_SETITEMHEIGHT )
    msgpick3(  TVM_GETITEMHEIGHT )
    msgpick3(  TVM_SETBKCOLOR )
    msgpick3(  TVM_SETTEXTCOLOR )
    msgpick3(  TVM_GETBKCOLOR )
    msgpick3(  TVM_GETTEXTCOLOR )
    msgpick3(  TVM_SETSCROLLTIME )
    msgpick3(  TVM_GETSCROLLTIME )
    msgpick3(  TVM_SETINSERTMARKCOLOR )
    msgpick3(  TVM_GETINSERTMARKCOLOR )
    msgpick3(  TVM_GETITEMSTATE )
    msgpick3(  TVM_SETLINECOLOR )
    msgpick3(  TVM_GETLINECOLOR )
    msgpick3(  TVM_MAPACCIDTOHTREEITEM )
    msgpick3(  TVM_MAPHTREEITEMTOACCID )
    msgpick3(  TVM_SETEXTENDEDSTYLE )
    msgpick3(  TVM_GETEXTENDEDSTYLE )
    msgpick3w( TVM_INSERTITEM )
    msgpick3(  TVM_SETAUTOSCROLLINFO )
    msgpick3w( TVM_GETITEM )
    msgpick3w( TVM_SETITEM )
    msgpick3w( TVM_GETISEARCHSTRING )
    msgpick3w( TVM_EDITLABEL )
    msgpick3(  TVM_GETSELECTEDCOUNT )
    msgpick3(  TVM_SHOWINFOTIP )
    msgpick3(  TVM_GETITEMPARTRECT )
};

#define TREEVIEWMESSAGEARRAYSIZE    (sizeof( TreeViewMessageArray )/sizeof( message ))

static message _NEAR ComboBoxExMessageArray[] = {
    combobox_messages_old()
    combobox_messages_new()
    msgpick3a( CBEM_INSERTITEM )
    msgpick3(  CBEM_SETIMAGELIST )
    msgpick3(  CBEM_GETIMAGELIST )
    msgpick3a( CBEM_GETITEM )
    msgpick3a( CBEM_SETITEM )
    msgpick3(  CBEM_GETCOMBOCONTROL )
    msgpick3(  CBEM_GETEDITCONTROL )
    msgpick3(  CBEM_SETEXSTYLE )
    msgpick3(  CBEM_GETEXSTYLE )
    msgpick3(  CBEM_HASEDITCHANGED )
    msgpick3w( CBEM_INSERTITEM )
    msgpick3w( CBEM_SETITEM )
    msgpick3w( CBEM_GETITEM )
    msgpick3(  CBEM_SETEXTENDEDSTYLE )
};

#define COMBOBOXEXMESSAGEARRAYSIZE  (sizeof( ComboBoxExMessageArray )/sizeof( message ))

static message _NEAR TabControlMessageArray[] = {
    msgpick3(  TCM_GETIMAGELIST )
    msgpick3(  TCM_SETIMAGELIST )
    msgpick3(  TCM_GETITEMCOUNT )
    msgpick3a( TCM_GETITEM )
    msgpick3a( TCM_SETITEM )
    msgpick3a( TCM_INSERTITEM )
    msgpick3(  TCM_DELETEITEM )
    msgpick3(  TCM_DELETEALLITEMS )
    msgpick3(  TCM_GETITEMRECT )
    msgpick3(  TCM_GETCURSEL )
    msgpick3(  TCM_SETCURSEL )
    msgpick3(  TCM_HITTEST )
    msgpick3(  TCM_SETITEMEXTRA )
    msgpick3(  TCM_ADJUSTRECT )
    msgpick3(  TCM_SETITEMSIZE )
    msgpick3(  TCM_REMOVEIMAGE )
    msgpick3(  TCM_SETPADDING )
    msgpick3(  TCM_GETROWCOUNT )
    msgpick3(  TCM_GETTOOLTIPS )
    msgpick3(  TCM_SETTOOLTIPS )
    msgpick3(  TCM_GETCURFOCUS )
    msgpick3(  TCM_SETCURFOCUS )
    msgpick3(  TCM_SETMINTABWIDTH )
    msgpick3(  TCM_DESELECTALL )
    msgpick3(  TCM_HIGHLIGHTITEM )
    msgpick3(  TCM_SETEXTENDEDSTYLE )
    msgpick3(  TCM_GETEXTENDEDSTYLE )
    msgpick3w( TCM_GETITEM )
    msgpick3w( TCM_SETITEM )
    msgpick3w( TCM_INSERTITEM )
};

#define TABCONTROLMESSAGEARRAYSIZE  (sizeof( TabControlMessageArray )/sizeof( message ))

static message _NEAR AnimateMessageArray[] = {
    msgpick3a( ACM_OPEN )
    msgpick3(  ACM_PLAY )
    msgpick3(  ACM_STOP )
    msgpick3w( ACM_OPEN )
    msgpick3(  ACM_ISPLAYING )
};

#define ANIMATEMESSAGEARRAYSIZE     (sizeof( AnimateMessageArray )/sizeof( message ))

static message _NEAR MonthCalMessageArray[] = {
    msgpick3(  MCM_GETCURSEL )
    msgpick3(  MCM_SETCURSEL )
    msgpick3(  MCM_GETMAXSELCOUNT )
    msgpick3(  MCM_SETMAXSELCOUNT )
    msgpick3(  MCM_GETSELRANGE )
    msgpick3(  MCM_SETSELRANGE )
    msgpick3(  MCM_GETMONTHRANGE )
    msgpick3(  MCM_SETDAYSTATE )
    msgpick3(  MCM_GETMINREQRECT )
    msgpick3(  MCM_SETCOLOR )
    msgpick3(  MCM_GETCOLOR )
    msgpick3(  MCM_SETTODAY )
    msgpick3(  MCM_GETTODAY )
    msgpick3(  MCM_HITTEST )
    msgpick3(  MCM_SETFIRSTDAYOFWEEK )
    msgpick3(  MCM_GETFIRSTDAYOFWEEK )
    msgpick3(  MCM_GETRANGE )
    msgpick3(  MCM_SETRANGE )
    msgpick3(  MCM_GETMONTHDELTA )
    msgpick3(  MCM_SETMONTHDELTA )
    msgpick3(  MCM_GETMAXTODAYWIDTH )
    msgpick3(  MCM_GETCURRENTVIEW )
    msgpick3(  MCM_GETCALENDARCOUNT )
    msgpick3(  MCM_GETCALENDARGRIDINFO )
    msgpick3(  MCM_GETCALID )
    msgpick3(  MCM_SETCALID )
    msgpick3(  MCM_SIZERECTTOMIN )
    msgpick3(  MCM_SETCALENDARBORDER )
    msgpick3(  MCM_GETCALENDARBORDER )
    msgpick3(  MCM_SETCURRENTVIEW )
};

#define MONTHCALMESSAGEARRAYSIZE    (sizeof( MonthCalMessageArray )/sizeof( message ))

static message _NEAR DateTimeMessageArray[] = {
    msgpick3(  DTM_GETSYSTEMTIME )
    msgpick3(  DTM_SETSYSTEMTIME )
    msgpick3(  DTM_GETRANGE )
    msgpick3(  DTM_SETRANGE )
    msgpick3a( DTM_SETFORMAT )
    msgpick3(  DTM_SETMCCOLOR )
    msgpick3(  DTM_GETMCCOLOR )
    msgpick3(  DTM_GETMONTHCAL )
    msgpick3(  DTM_SETMCFONT )
    msgpick3(  DTM_GETMCFONT )
    msgpick3(  DTM_SETMCSTYLE )
    msgpick3(  DTM_GETMCSTYLE )
    msgpick3(  DTM_CLOSEMONTHCAL )
    msgpick3(  DTM_GETDATETIMEPICKERINFO )
    msgpick3(  DTM_GETIDEALSIZE )
    msgpick3w( DTM_SETFORMAT )
};

#define DATETIMEMESSAGEARRAYSIZE    (sizeof( DateTimeMessageArray )/sizeof( message ))

static message _NEAR ComboListBoxMessageArray[] = {
    msgpick3(  LBCB_STARTTRACK )
    msgpick3(  LBCB_ENDTRACK )
};

#define COMBOLISTBOXMESSAGEARRAYSIZE    (sizeof( ComboListBoxMessageArray )/sizeof( message ))

#endif

#undef msgpick1
#undef msgpick1x
#undef msgpick2
#undef msgpick3
#undef msgpick3a
#undef msgpick3w
#undef msgpick4

class_messages _NEAR ClassMessages[] = {
    { "",                   MessageArray,               MESSAGEARRAYSIZE               },
    { WC_EDIT,              EditMessageArray,           EDITMESSAGEARRAYSIZE           },
    { WC_BUTTON,            ButtonMessageArray,         BUTTONMESSAGEARRAYSIZE         },
    { WC_STATIC,            StaticMessageArray,         STATICMESSAGEARRAYSIZE         },
    { WC_LISTBOX,           ListBoxMessageArray,        LISTBOXMESSAGEARRAYSIZE        },
    { WC_COMBOBOX,          ComboBoxMessageArray,       COMBOBOXMESSAGEARRAYSIZE       },
#ifdef __NT__
    { "scrollbar",          ScrollBarMessageArray,      SCROLLBARMESSAGEARRAYSIZE      },
    { WC_HEADER,            HeaderMessageArray,         HEADERMESSAGEARRAYSIZE         },
    { TOOLBARCLASSNAME,     ToolbarMessageArray,        TOOLBARMESSAGEARRAYSIZE        },
    { REBARCLASSNAME,       RebarMessageArray,          REBARMESSAGEARRAYSIZE          },
    { TOOLTIPS_CLASS,       ToolTipsMessageArray,       TOOLTIPSMESSAGEARRAYSIZE       },
    { STATUSCLASSNAME,      StatusBarMessageArray,      STATUSBARMESSAGEARRAYSIZE      },
    { TRACKBAR_CLASS,       TrackBarMessageArray,       TRACKBARMESSAGEARRAYSIZE       },
    { UPDOWN_CLASS,         UpDownMessageArray,         UPDOWNMESSAGEARRAYSIZE         },
    { PROGRESS_CLASS,       ProgressBarMessageArray,    PROGRESSBARMESSAGEARRAYSIZE    },
    { HOTKEY_CLASS,         HotKeyMessageArray,         HOTKEYMESSAGEARRAYSIZE         },
    { WC_LISTVIEW,          ListViewMessageArray,       LISTVIEWMESSAGEARRAYSIZE       },
    { WC_TREEVIEW,          TreeViewMessageArray,       TREEVIEWMESSAGEARRAYSIZE       },
    { WC_COMBOBOXEX,        ComboBoxExMessageArray,     COMBOBOXEXMESSAGEARRAYSIZE     },
    { WC_TABCONTROL,        TabControlMessageArray,     TABCONTROLMESSAGEARRAYSIZE     },
    { ANIMATE_CLASS,        AnimateMessageArray,        ANIMATEMESSAGEARRAYSIZE        },
    { MONTHCAL_CLASS,       MonthCalMessageArray,       MONTHCALMESSAGEARRAYSIZE       },
    { DATETIMEPICK_CLASS,   DateTimeMessageArray,       DATETIMEMESSAGEARRAYSIZE       },
    { "combolbox",          ComboListBoxMessageArray,   COMBOLISTBOXMESSAGEARRAYSIZE   },
#endif
};

WORD ClassMessagesSize = sizeof( ClassMessages ) / sizeof( class_messages );

#ifdef __WINDOWS__
WORD TotalMessageArraySize = \
    + MESSAGEARRAYSIZE \
    + EDITMESSAGEARRAYSIZE \
    + BUTTONMESSAGEARRAYSIZE \
    + STATICMESSAGEARRAYSIZE \
    + LISTBOXMESSAGEARRAYSIZE \
    + COMBOBOXMESSAGEARRAYSIZE;
#else
WORD TotalMessageArraySize = \
    + MESSAGEARRAYSIZE \
    + EDITMESSAGEARRAYSIZE \
    + BUTTONMESSAGEARRAYSIZE \
    + STATICMESSAGEARRAYSIZE \
    + LISTBOXMESSAGEARRAYSIZE \
    + COMBOBOXMESSAGEARRAYSIZE \
    + SCROLLBARMESSAGEARRAYSIZE \
    + HEADERMESSAGEARRAYSIZE \
    + TOOLBARMESSAGEARRAYSIZE \
    + REBARMESSAGEARRAYSIZE \
    + TOOLTIPSMESSAGEARRAYSIZE \
    + STATUSBARMESSAGEARRAYSIZE \
    + TRACKBARMESSAGEARRAYSIZE \
    + UPDOWNMESSAGEARRAYSIZE \
    + PROGRESSBARMESSAGEARRAYSIZE \
    + HOTKEYMESSAGEARRAYSIZE \
    + LISTVIEWMESSAGEARRAYSIZE \
    + TREEVIEWMESSAGEARRAYSIZE \
    + COMBOBOXEXMESSAGEARRAYSIZE \
    + TABCONTROLMESSAGEARRAYSIZE \
    + ANIMATEMESSAGEARRAYSIZE \
    + MONTHCALMESSAGEARRAYSIZE \
    + DATETIMEMESSAGEARRAYSIZE;
#endif


static style_info _NEAR ButtonStyleArray[] = {
    { "BS_PUSHBUTTON",      BS_PUSHBUTTON,      BS_TYPEMASK         },
    { "BS_DEFPUSHBUTTON",   BS_DEFPUSHBUTTON,   BS_TYPEMASK         },
    { "BS_CHECKBOX",        BS_CHECKBOX,        BS_TYPEMASK         },
    { "BS_AUTOCHECKBOX",    BS_AUTOCHECKBOX,    BS_TYPEMASK         },
    { "BS_RADIOBUTTON",     BS_RADIOBUTTON,     BS_TYPEMASK         },
    { "BS_3STATE",          BS_3STATE,          BS_TYPEMASK         },
    { "BS_AUTO3STATE",      BS_AUTO3STATE,      BS_TYPEMASK         },
    { "BS_GROUPBOX",        BS_GROUPBOX,        BS_TYPEMASK         },
    { "BS_USERBUTTON",      BS_USERBUTTON,      BS_TYPEMASK         },
    { "BS_AUTORADIOBUTTON", BS_AUTORADIOBUTTON, BS_TYPEMASK         },
    { "BS_OWNERDRAW",       BS_OWNERDRAW,       BS_TYPEMASK         },
    { "BS_LEFTTEXT",        BS_LEFTTEXT,        BS_LEFTTEXT         },
#ifdef __NT__
    { "BS_TEXT",            BS_TEXT,            BS_ICON | BS_BITMAP },
    { "BS_ICON",            BS_ICON,            BS_ICON | BS_BITMAP },
    { "BS_BITMAP",          BS_BITMAP,          BS_ICON | BS_BITMAP },
    { "BS_LEFT",            BS_LEFT,            BS_LEFT | BS_RIGHT  },
    { "BS_RIGHT",           BS_RIGHT,           BS_LEFT | BS_RIGHT  },
    { "BS_CENTER",          BS_CENTER,          BS_LEFT | BS_RIGHT  },
    { "BS_TOP",             BS_TOP,             BS_TOP | BS_BOTTOM  },
    { "BS_BOTTOM",          BS_BOTTOM,          BS_TOP | BS_BOTTOM  },
    { "BS_VCENTER",         BS_VCENTER,         BS_TOP | BS_BOTTOM  },
    { "BS_PUSHLIKE",        BS_PUSHLIKE,        BS_PUSHLIKE         },
    { "BS_MULTILINE",       BS_MULTILINE,       BS_MULTILINE        },
    { "BS_NOTIFY",          BS_NOTIFY,          BS_NOTIFY           },
    { "BS_FLAT",            BS_FLAT,            BS_FLAT             }
#endif
};

#define BUTTONSTYLEARRAYSIZE    (sizeof( ButtonStyleArray ) / sizeof( style_info ))

static style_info _NEAR EditStyleArray[] = {
    { "ES_LEFT",        ES_LEFT,        ES_CENTER | ES_RIGHT },
    { "ES_CENTER",      ES_CENTER,      ES_CENTER | ES_RIGHT },
    { "ES_RIGHT",       ES_RIGHT,       ES_CENTER | ES_RIGHT },
    { "ES_MULTILINE",   ES_MULTILINE,   ES_MULTILINE         },
    { "ES_UPPERCASE",   ES_UPPERCASE,   ES_UPPERCASE         },
    { "ES_LOWERCASE",   ES_LOWERCASE,   ES_LOWERCASE         },
    { "ES_PASSWORD",    ES_PASSWORD,    ES_PASSWORD          },
    { "ES_AUTOVSCROLL", ES_AUTOVSCROLL, ES_AUTOVSCROLL       },
    { "ES_AUTOHSCROLL", ES_AUTOHSCROLL, ES_AUTOHSCROLL       },
    { "ES_NOHIDESEL",   ES_NOHIDESEL,   ES_NOHIDESEL         },
    { "ES_OEMCONVERT",  ES_OEMCONVERT,  ES_OEMCONVERT        },
    { "ES_READONLY",    ES_READONLY,    ES_READONLY          },
    { "ES_WANTRETURN",  ES_WANTRETURN,  ES_WANTRETURN        },
#ifdef __NT__
    { "ES_NUMBER",      ES_NUMBER,      ES_NUMBER            }
#endif
};

#define EDITSTYLEARRAYSIZE      (sizeof( EditStyleArray ) / sizeof( style_info ))

static style_info _NEAR StaticStyleArray[] = {
    { "SS_LEFT",            SS_LEFT,            SS_TYPEMASK        },
    { "SS_CENTER",          SS_CENTER,          SS_TYPEMASK        },
    { "SS_RIGHT",           SS_RIGHT,           SS_TYPEMASK        },
    { "SS_ICON",            SS_ICON,            SS_TYPEMASK        },
    { "SS_BLACKRECT",       SS_BLACKRECT,       SS_TYPEMASK        },
    { "SS_GRAYRECT",        SS_GRAYRECT,        SS_TYPEMASK        },
    { "SS_WHITERECT",       SS_WHITERECT,       SS_TYPEMASK        },
    { "SS_BLACKFRAME",      SS_BLACKFRAME,      SS_TYPEMASK        },
    { "SS_GRAYFRAME",       SS_GRAYFRAME,       SS_TYPEMASK        },
    { "SS_WHITEFRAME",      SS_WHITEFRAME,      SS_TYPEMASK        },
#ifdef __NT__
    { "SS_USERITEM",        SS_USERITEM,        SS_TYPEMASK        },
#endif
    { "SS_SIMPLE",          SS_SIMPLE,          SS_TYPEMASK        },
    { "SS_LEFTNOWORDWRAP",  SS_LEFTNOWORDWRAP,  SS_TYPEMASK        },
#ifdef __NT__
    { "SS_OWNERDRAW",       SS_OWNERDRAW,       SS_TYPEMASK        },
    { "SS_BITMAP",          SS_BITMAP,          SS_TYPEMASK        },
    { "SS_ENHMETAFILE",     SS_ENHMETAFILE,     SS_TYPEMASK        },
    { "SS_ETCHEDHORZ",      SS_ETCHEDHORZ,      SS_TYPEMASK        },
    { "SS_ETCHEDVERT",      SS_ETCHEDVERT,      SS_TYPEMASK        },
    { "SS_ETCHEDFRAME",     SS_ETCHEDFRAME,     SS_TYPEMASK        },
    { "SS_REALSIZECONTROL", SS_REALSIZECONTROL, SS_REALSIZECONTROL },
#endif
    { "SS_NOPREFIX",        SS_NOPREFIX,        SS_NOPREFIX        },
#ifdef __NT__
    { "SS_NOTIFY",          SS_NOTIFY,          SS_NOTIFY          },
    { "SS_CENTERIMAGE",     SS_CENTERIMAGE,     SS_CENTERIMAGE     },
    { "SS_RIGHTJUST",       SS_RIGHTJUST,       SS_RIGHTJUST       },
    { "SS_REALSIZEIMAGE",   SS_REALSIZEIMAGE,   SS_REALSIZEIMAGE   },
    { "SS_SUNKEN",          SS_SUNKEN,          SS_SUNKEN          },
    { "SS_EDITCONTROL",     SS_EDITCONTROL,     SS_EDITCONTROL     },
    { "SS_ENDELLIPSIS",     SS_ENDELLIPSIS,     SS_ELLIPSISMASK    },
    { "SS_PATHELLIPSIS",    SS_PATHELLIPSIS,    SS_ELLIPSISMASK    },
    { "SS_WORDELLIPSIS",    SS_WORDELLIPSIS,    SS_ELLIPSISMASK    }
#endif
};

#define STATICSTYLEARRAYSIZE    (sizeof( StaticStyleArray ) / sizeof( style_info ))

static style_info _NEAR ListBoxStyleArray[] = {
    { "LBS_NOTIFY",             LBS_NOTIFY,             LBS_NOTIFY            },
    { "LBS_SORT",               LBS_SORT,               LBS_SORT              },
    { "LBS_NOREDRAW",           LBS_NOREDRAW,           LBS_NOREDRAW          },
    { "LBS_MULTIPLESEL",        LBS_MULTIPLESEL,        LBS_MULTIPLESEL       },
    { "LBS_OWNERDRAWFIXED",     LBS_OWNERDRAWFIXED,     LBS_OWNERDRAWFIXED    },
    { "LBS_OWNERDRAWVARIABLE",  LBS_OWNERDRAWVARIABLE,  LBS_OWNERDRAWVARIABLE },
    { "LBS_HASSTRINGS",         LBS_HASSTRINGS,         LBS_HASSTRINGS        },
    { "LBS_USETABSTOPS",        LBS_USETABSTOPS,        LBS_USETABSTOPS       },
    { "LBS_NOINTEGRALHEIGHT",   LBS_NOINTEGRALHEIGHT,   LBS_NOINTEGRALHEIGHT  },
    { "LBS_MULTICOLUMN",        LBS_MULTICOLUMN,        LBS_MULTICOLUMN       },
    { "LBS_WANTKEYBOARDINPUT",  LBS_WANTKEYBOARDINPUT,  LBS_WANTKEYBOARDINPUT },
    { "LBS_EXTENDEDSEL",        LBS_EXTENDEDSEL,        LBS_EXTENDEDSEL       },
    { "LBS_DISABLENOSCROLL",    LBS_DISABLENOSCROLL,    LBS_DISABLENOSCROLL   },
#ifdef __NT__
    { "LBS_NODATA",             LBS_NODATA,             LBS_NODATA            },
    { "LBS_NOSEL",              LBS_NOSEL,              LBS_NOSEL             },
    { "LBS_COMBOBOX",           LBS_COMBOBOX,           LBS_COMBOBOX          }
#endif
};

#define LISTBOXSTYLEARRAYSIZE   (sizeof( ListBoxStyleArray ) / sizeof( style_info ))

static style_info _NEAR ComboBoxStyleArray[] = {
    { "CBS_SIMPLE",             CBS_SIMPLE,             CBS_SIMPLE | CBS_DROPDOWN },
    { "CBS_DROPDOWN",           CBS_DROPDOWN,           CBS_SIMPLE | CBS_DROPDOWN },
    { "CBS_DROPDOWNLIST",       CBS_DROPDOWNLIST,       CBS_SIMPLE | CBS_DROPDOWN },
    { "CBS_OWNERDRAWFIXED",     CBS_OWNERDRAWFIXED,     CBS_OWNERDRAWFIXED        },
    { "CBS_OWNERDRAWVARIABLE",  CBS_OWNERDRAWVARIABLE,  CBS_OWNERDRAWVARIABLE     },
    { "CBS_AUTOHSCROLL",        CBS_AUTOHSCROLL,        CBS_AUTOHSCROLL           },
    { "CBS_OEMCONVERT",         CBS_OEMCONVERT,         CBS_OEMCONVERT            },
    { "CBS_SORT",               CBS_SORT,               CBS_SORT                  },
    { "CBS_HASSTRINGS",         CBS_HASSTRINGS,         CBS_HASSTRINGS            },
    { "CBS_NOINTEGRALHEIGHT",   CBS_NOINTEGRALHEIGHT,   CBS_NOINTEGRALHEIGHT      },
    { "CBS_DISABLENOSCROLL",    CBS_DISABLENOSCROLL,    CBS_DISABLENOSCROLL       },
#ifdef __NT__
    { "CBS_UPPERCASE",          CBS_UPPERCASE,          CBS_UPPERCASE             },
    { "CBS_LOWERCASE",          CBS_LOWERCASE,          CBS_LOWERCASE             }
#endif
};

#define COMBOBOXSTYLEARRAYSIZE  (sizeof( ComboBoxStyleArray ) / sizeof( style_info ))

static style_info _NEAR DialogStyleArray[] = {
    { "DS_ABSALIGN",        DS_ABSALIGN,        DS_ABSALIGN              },
    { "DS_SYSMODAL",        DS_SYSMODAL,        DS_SYSMODAL              },
    { "DS_LOCALEDIT",       DS_LOCALEDIT,       DS_LOCALEDIT             },
#ifdef __WINDOWS__
    { "DS_SETFONT",         DS_SETFONT,         DS_SETFONT               },
#else
    { "DS_SETFONT",         DS_SETFONT,         DS_SETFONT | DS_FIXEDSYS },
    { "DS_SETFOREGROUND",   DS_SETFOREGROUND,   DS_SETFOREGROUND         },
    { "DS_3DLOOK",          DS_3DLOOK,          DS_3DLOOK                },
    { "DS_FIXEDSYS",        DS_FIXEDSYS,        DS_SETFONT | DS_FIXEDSYS },
    { "DS_NOFAILCREATE",    DS_NOFAILCREATE,    DS_NOFAILCREATE          },
    { "DS_CONTROL",         DS_CONTROL,         DS_CONTROL               },
    { "DS_CENTER",          DS_CENTER,          DS_CENTER                },
    { "DS_CENTERMOUSE",     DS_CENTERMOUSE,     DS_CENTERMOUSE           },
    { "DS_CONTEXTHELP",     DS_CONTEXTHELP,     DS_CONTEXTHELP           },
    { "DS_SHELLFONT",       DS_SHELLFONT,       DS_SETFONT | DS_FIXEDSYS }
#endif
};

#define DIALOGSTYLEARRAYSIZE    (sizeof( DialogStyleArray ) / sizeof( style_info ))

#ifdef __NT__
static style_info _NEAR AnimateStyleArray[] = {
    { "ACS_CENTER",         ACS_CENTER,         ACS_CENTER      },
    { "ACS_TRANSPARENT",    ACS_TRANSPARENT,    ACS_TRANSPARENT },
    { "ACS_AUTOPLAY",       ACS_AUTOPLAY,       ACS_AUTOPLAY    },
    { "ACS_TIMER",          ACS_TIMER,          ACS_TIMER       }
};

#define ANIMATESTYLEARRAYSIZE   (sizeof( AnimateStyleArray ) / sizeof( style_info ))

static style_info _NEAR DateTimeStyleArray[] = {
    { "DTS_UPDOWN",                 DTS_UPDOWN,                 DTS_UPDOWN         },
    { "DTS_SHOWNONE",               DTS_SHOWNONE,               DTS_SHOWNONE       },
    { "DTS_SHORTDATECENTURYFORMAT", DTS_SHORTDATECENTURYFORMAT, DTS_SHORTDATECENTURYFORMAT },
    { "DTS_LONGDATEFORMAT",         DTS_LONGDATEFORMAT,         DTS_LONGDATEFORMAT },
    { "DTS_TIMEFORMAT",             DTS_TIMEFORMAT,             DTS_TIMEFORMAT     },
    { "DTS_SHORTDATEFORMAT",        DTS_SHORTDATEFORMAT,        DTS_SHORTDATECENTURYFORMAT | DTS_LONGDATEFORMAT | DTS_TIMEFORMAT },
    { "DTS_APPCANPARSE",            DTS_APPCANPARSE,            DTS_APPCANPARSE    },
    { "DTS_RIGHTALIGN",             DTS_RIGHTALIGN,             DTS_RIGHTALIGN     }
};

#define DATETIMESTYLEARRAYSIZE  (sizeof( DateTimeStyleArray ) / sizeof( style_info ))

static style_info _NEAR MonthCalStyleArray[] = {
    { "MCS_DAYSTATE",           MCS_DAYSTATE,           MCS_DAYSTATE         },
    { "MCS_MULTISELECT",        MCS_MULTISELECT,        MCS_MULTISELECT      },
    { "MCS_WEEKNUMBERS",        MCS_WEEKNUMBERS,        MCS_WEEKNUMBERS      },
    { "MCS_NOTODAYCIRCLE",      MCS_NOTODAYCIRCLE,      MCS_NOTODAYCIRCLE    },
    { "MCS_NOTODAY",            MCS_NOTODAY,            MCS_NOTODAY          },
    { "MCS_NOTRAILINGDATES",    MCS_NOTRAILINGDATES,    MCS_NOTRAILINGDATES  },
    { "MCS_SHORTDAYSOFWEEK",    MCS_SHORTDAYSOFWEEK,    MCS_SHORTDAYSOFWEEK  },
    { "MCS_NOSELCHANGEONNAV",   MCS_NOSELCHANGEONNAV,   MCS_NOSELCHANGEONNAV }
};

#define MONTHCALSTYLEARRAYSIZE  (sizeof( MonthCalStyleArray ) / sizeof( style_info ))

static style_info _NEAR ProgressBarStyleArray[] = {
    { "PBS_SMOOTH",         PBS_SMOOTH,         PBS_SMOOTH        },
    { "PBS_VERTICAL",       PBS_VERTICAL,       PBS_VERTICAL      },
    { "PBS_MARQUEE",        PBS_MARQUEE,        PBS_MARQUEE       },
    { "PBS_SMOOTHREVERSE",  PBS_SMOOTHREVERSE,  PBS_SMOOTHREVERSE }
};

#define PROGRESSBARSTYLEARRAYSIZE   (sizeof( ProgressBarStyleArray ) / sizeof( style_info ))

static style_info _NEAR RebarStyleArray[] = {
    { "RBS_TOOLTIPS",           RBS_TOOLTIPS,           RBS_TOOLTIPS        },
    { "RBS_VARHEIGHT",          RBS_VARHEIGHT,          RBS_VARHEIGHT       },
    { "RBS_BANDBORDERS",        RBS_BANDBORDERS,        RBS_BANDBORDERS     },
    { "RBS_FIXEDORDER",         RBS_FIXEDORDER,         RBS_FIXEDORDER      },
    { "RBS_REGISTERDROP",       RBS_REGISTERDROP,       RBS_REGISTERDROP    },
    { "RBS_AUTOSIZE",           RBS_AUTOSIZE,           RBS_AUTOSIZE        },
    { "RBS_VERTICALGRIPPER",    RBS_VERTICALGRIPPER,    RBS_VERTICALGRIPPER },
    { "RBS_DBLCLKTOGGLE",       RBS_DBLCLKTOGGLE,       RBS_DBLCLKTOGGLE    }
};

#define REBARSTYLEARRAYSIZE     (sizeof( RebarStyleArray ) / sizeof( style_info ))

static style_info _NEAR StatusBarStyleArray[] = {
    { "SBARS_SIZEGRIP", SBARS_SIZEGRIP, SBARS_SIZEGRIP },
    { "SBARS_TOOLTIPS", SBARS_TOOLTIPS, SBARS_TOOLTIPS }
};

#define STATUSBARSTYLEARRAYSIZE (sizeof( StatusBarStyleArray ) / sizeof( style_info ))

static style_info _NEAR ToolbarStyleArray[] = {
    { "TBSTYLE_TOOLTIPS",       TBSTYLE_TOOLTIPS,       TBSTYLE_TOOLTIPS     },
    { "TBSTYLE_WRAPABLE",       TBSTYLE_WRAPABLE,       TBSTYLE_WRAPABLE     },
    { "TBSTYLE_ALTDRAG",        TBSTYLE_ALTDRAG,        TBSTYLE_ALTDRAG      },
    { "TBSTYLE_FLAT",           TBSTYLE_FLAT,           TBSTYLE_FLAT         },
    { "TBSTYLE_LIST",           TBSTYLE_LIST,           TBSTYLE_LIST         },
    { "TBSTYLE_CUSTOMERASE",    TBSTYLE_CUSTOMERASE,    TBSTYLE_CUSTOMERASE  },
    { "TBSTYLE_REGISTERDROP",   TBSTYLE_REGISTERDROP,   TBSTYLE_REGISTERDROP },
    { "TBSTYLE_TRANSPARENT",    TBSTYLE_TRANSPARENT,    TBSTYLE_TRANSPARENT  }
};

#define TOOLBARSTYLEARRAYSIZE   (sizeof( ToolbarStyleArray ) / sizeof( style_info ))

static style_info _NEAR ToolTipsStyleArray[] = {
    { "TTS_ALWAYSTIP",      TTS_ALWAYSTIP,      TTS_ALWAYSTIP      },
    { "TTS_NOPREFIX",       TTS_NOPREFIX,       TTS_NOPREFIX       },
    { "TTS_NOANIMATE",      TTS_NOANIMATE,      TTS_NOANIMATE      },
    { "TTS_NOFADE",         TTS_NOFADE,         TTS_NOFADE         },
    { "TTS_BALLOON",        TTS_BALLOON,        TTS_BALLOON        },
    { "TTS_CLOSE",          TTS_CLOSE,          TTS_CLOSE          },
    { "TTS_USEVISUALSTYLE", TTS_USEVISUALSTYLE, TTS_USEVISUALSTYLE }
};

#define TOOLTIPSSTYLEARRAYSIZE  (sizeof( ToolTipsStyleArray ) / sizeof( style_info ))

static style_info _NEAR TrackBarStyleArray[] = {
    { "TBS_AUTOTICKS",          TBS_AUTOTICKS,          TBS_AUTOTICKS        },
    { "TBS_VERT",               TBS_VERT,               TBS_VERT             },
    { "TBS_LEFT",               TBS_VERT | TBS_LEFT,    TBS_VERT | TBS_LEFT  },
    { "TBS_RIGHT",              TBS_VERT,               TBS_VERT | TBS_LEFT | TBS_BOTH | TBS_NOTICKS },
    { "TBS_HORZ",               TBS_HORZ,               TBS_VERT             },
    { "TBS_TOP",                TBS_TOP,                TBS_VERT | TBS_TOP   },
    { "TBS_BOTTOM",             TBS_HORZ,               TBS_VERT | TBS_TOP | TBS_BOTH | TBS_NOTICKS },
    { "TBS_BOTH",               TBS_BOTH,               TBS_BOTH             },
    { "TBS_NOTICKS",            TBS_NOTICKS,            TBS_NOTICKS          },
    { "TBS_ENABLESELRANGE",     TBS_ENABLESELRANGE,     TBS_ENABLESELRANGE   },
    { "TBS_FIXEDLENGTH",        TBS_FIXEDLENGTH,        TBS_FIXEDLENGTH      },
    { "TBS_NOTHUMB",            TBS_NOTHUMB,            TBS_NOTHUMB          },
    { "TBS_TOOLTIPS",           TBS_TOOLTIPS,           TBS_TOOLTIPS         },
    { "TBS_REVERSED",           TBS_REVERSED,           TBS_REVERSED         },
    { "TBS_DOWNISLEFT",         TBS_DOWNISLEFT,         TBS_DOWNISLEFT       },
    { "TBS_NOTIFYBEFOREMOVE",   TBS_NOTIFYBEFOREMOVE,   TBS_NOTIFYBEFOREMOVE },
    { "TBS_TRANSPARENTBKGND",   TBS_TRANSPARENTBKGND,   TBS_TRANSPARENTBKGND }
};

#define TRACKBARSTYLEARRAYSIZE  (sizeof( TrackBarStyleArray ) / sizeof( style_info ))

static style_info _NEAR UpDownStyleArray[] = {
    { "UDS_WRAP",           UDS_WRAP,           UDS_WRAP        },
    { "UDS_SETBUDDYINT",    UDS_SETBUDDYINT,    UDS_SETBUDDYINT },
    { "UDS_ALIGNRIGHT",     UDS_ALIGNRIGHT,     UDS_ALIGNRIGHT  },
    { "UDS_ALIGNLEFT",      UDS_ALIGNLEFT,      UDS_ALIGNLEFT   },
    { "UDS_AUTOBUDDY",      UDS_AUTOBUDDY,      UDS_AUTOBUDDY   },
    { "UDS_ARROWKEYS",      UDS_ARROWKEYS,      UDS_ARROWKEYS   },
    { "UDS_HORZ",           UDS_HORZ,           UDS_HORZ        },
    { "UDS_NOTHOUSANDS",    UDS_NOTHOUSANDS,    UDS_NOTHOUSANDS },
    { "UDS_HOTTRACK",       UDS_HOTTRACK,       UDS_HOTTRACK    }
};

#define UPDOWNSTYLEARRAYSIZE    (sizeof( UpDownStyleArray ) / sizeof( style_info ))

static style_info _NEAR HeaderStyleArray[] = {
    { "HDS_BUTTONS",    HDS_BUTTONS,    HDS_BUTTONS    },
    { "HDS_HOTTRACK",   HDS_HOTTRACK,   HDS_HOTTRACK   },
    { "HDS_HIDDEN",     HDS_HIDDEN,     HDS_HIDDEN     },
    { "HDS_DRAGDROP",   HDS_DRAGDROP,   HDS_DRAGDROP   },
    { "HDS_FULLDRAG",   HDS_FULLDRAG,   HDS_FULLDRAG   },
    { "HDS_FILTERBAR",  HDS_FILTERBAR,  HDS_FILTERBAR  },
    { "HDS_FLAT",       HDS_FLAT,       HDS_FLAT       },
    { "HDS_CHECKBOXES", HDS_CHECKBOXES, HDS_CHECKBOXES },
    { "HDS_NOSIZING",   HDS_NOSIZING,   HDS_NOSIZING   },
    { "HDS_OVERFLOW",   HDS_OVERFLOW,   HDS_OVERFLOW   }
};

#define HEADERSTYLEARRAYSIZE    (sizeof( HeaderStyleArray ) / sizeof( style_info ))

static style_info _NEAR ListViewStyleArray[] = {
    { "LVS_ICON",               LVS_ICON,               LVS_TYPEMASK        },
    { "LVS_REPORT",             LVS_REPORT,             LVS_TYPEMASK        },
    { "LVS_SMALLICON",          LVS_SMALLICON,          LVS_TYPEMASK        },
    { "LVS_LIST",               LVS_LIST,               LVS_TYPEMASK        },
    { "LVS_SINGLESEL",          LVS_SINGLESEL,          LVS_SINGLESEL       },
    { "LVS_SHOWSELALWAYS",      LVS_SHOWSELALWAYS,      LVS_SHOWSELALWAYS   },
    { "LVS_SORTASCENDING",      LVS_SORTASCENDING,      LVS_SORTASCENDING   },
    { "LVS_SORTDESCENDING",     LVS_SORTDESCENDING,     LVS_SORTDESCENDING  },
    { "LVS_SHAREIMAGELISTS",    LVS_SHAREIMAGELISTS,    LVS_SHAREIMAGELISTS },
    { "LVS_NOLABELWRAP",        LVS_NOLABELWRAP,        LVS_NOLABELWRAP     },
    { "LVS_AUTOARRANGE",        LVS_AUTOARRANGE,        LVS_AUTOARRANGE     },
    { "LVS_EDITLABELS",         LVS_EDITLABELS,         LVS_EDITLABELS      },
    { "LVS_OWNERDRAWFIXED",     LVS_OWNERDRAWFIXED,     LVS_OWNERDRAWFIXED  },
    { "LVS_ALIGNTOP",           LVS_ALIGNTOP,           LVS_ALIGNMASK       },
    { "LVS_ALIGNLEFT",          LVS_ALIGNLEFT,          LVS_ALIGNMASK       },
    { "LVS_OWNERDATA",          LVS_OWNERDATA,          LVS_OWNERDATA       },
    { "LVS_NOSCROLL",           LVS_NOSCROLL,           LVS_NOSCROLL        },
    { "LVS_NOCOLUMNHEADER",     LVS_NOCOLUMNHEADER,     LVS_NOCOLUMNHEADER  },
    { "LVS_NOSORTHEADER",       LVS_NOSORTHEADER,       LVS_NOSORTHEADER    }
};

#define LISTVIEWSTYLEARRAYSIZE  (sizeof( ListViewStyleArray ) / sizeof( style_info ))

static style_info _NEAR TabControlStyleArray[] = {
    { "TCS_SCROLLOPPOSITE",     TCS_SCROLLOPPOSITE,         TCS_SCROLLOPPOSITE        },
    { "TCS_BOTTOM",             TCS_BOTTOM | TCS_VERTICAL,  TCS_BOTTOM | TCS_VERTICAL },
    { "TCS_RIGHT",              TCS_RIGHT,                  TCS_BOTTOM | TCS_VERTICAL },
    { "TCS_MULTISELECT",        TCS_MULTISELECT,            TCS_MULTISELECT           },
    { "TCS_FLATBUTTONS",        TCS_FLATBUTTONS,            TCS_FLATBUTTONS           },
    { "TCS_FORCEICONLEFT",      TCS_FORCEICONLEFT,          TCS_FORCEICONLEFT         },
    { "TCS_FORCELABELLEFT",     TCS_FORCELABELLEFT,         TCS_FORCELABELLEFT        },
    { "TCS_HOTTRACK",           TCS_HOTTRACK,               TCS_HOTTRACK              },
    { "TCS_VERTICAL",           TCS_VERTICAL,               TCS_VERTICAL              },
    { "TCS_BUTTONS",            TCS_BUTTONS,                TCS_BUTTONS               },
    { "TCS_TABS",               TCS_TABS,                   TCS_BUTTONS               },
    { "TCS_MULTILINE",          TCS_MULTILINE,              TCS_MULTILINE             },
    { "TCS_SINGLELINE",         TCS_SINGLELINE,             TCS_MULTILINE             },
    { "TCS_RIGHTJUSTIFY",       TCS_MULTILINE,              TCS_MULTILINE | TCS_FIXEDWIDTH | TCS_RAGGEDRIGHT },
    { "TCS_FIXEDWIDTH",         TCS_FIXEDWIDTH,             TCS_FIXEDWIDTH            },
    { "TCS_RAGGEDRIGHT",        TCS_RAGGEDRIGHT,            TCS_RAGGEDRIGHT           },
    { "TCS_FOCUSONBUTTONDOWN",  TCS_FOCUSONBUTTONDOWN,      TCS_FOCUSONBUTTONDOWN     },
    { "TCS_OWNERDRAWFIXED",     TCS_OWNERDRAWFIXED,         TCS_OWNERDRAWFIXED        },
    { "TCS_TOOLTIPS",           TCS_TOOLTIPS,               TCS_TOOLTIPS              },
    { "TCS_FOCUSNEVER",         TCS_FOCUSNEVER,             TCS_FOCUSNEVER            }
};

#define TABCONTROLSTYLEARRAYSIZE    (sizeof( TabControlStyleArray ) / sizeof( style_info ))

static style_info _NEAR TreeViewStyleArray[] = {
    { "TVS_HASBUTTONS",         TVS_HASBUTTONS,         TVS_HASBUTTONS      },
    { "TVS_HASLINES",           TVS_HASLINES,           TVS_HASLINES        },
    { "TVS_LINESATROOT",        TVS_LINESATROOT,        TVS_LINESATROOT     },
    { "TVS_EDITLABELS",         TVS_EDITLABELS,         TVS_EDITLABELS      },
    { "TVS_DISABLEDRAGDROP",    TVS_DISABLEDRAGDROP,    TVS_DISABLEDRAGDROP },
    { "TVS_SHOWSELALWAYS",      TVS_SHOWSELALWAYS,      TVS_SHOWSELALWAYS   },
    { "TVS_RTLREADING",         TVS_RTLREADING,         TVS_RTLREADING      },
    { "TVS_NOTOOLTIPS",         TVS_NOTOOLTIPS,         TVS_NOTOOLTIPS      },
    { "TVS_CHECKBOXES",         TVS_CHECKBOXES,         TVS_CHECKBOXES      },
    { "TVS_TRACKSELECT",        TVS_TRACKSELECT,        TVS_TRACKSELECT     },
    { "TVS_SINGLEEXPAND",       TVS_SINGLEEXPAND,       TVS_SINGLEEXPAND    },
    { "TVS_INFOTIP",            TVS_INFOTIP,            TVS_INFOTIP         },
    { "TVS_FULLROWSELECT",      TVS_FULLROWSELECT,      TVS_FULLROWSELECT   },
    { "TVS_NOSCROLL",           TVS_NOSCROLL,           TVS_NOSCROLL        },
    { "TVS_NONEVENHEIGHT",      TVS_NONEVENHEIGHT,      TVS_NONEVENHEIGHT   },
    { "TVS_NOHSCROLL",          TVS_NOHSCROLL,          TVS_NOHSCROLL       }
};

#define TREEVIEWSTYLEARRAYSIZE  (sizeof( TreeViewStyleArray ) / sizeof( style_info ))
#endif

class_styles _NEAR ClassStyles[] = {
    { WC_BUTTON,            ButtonStyleArray,       BUTTONSTYLEARRAYSIZE      },
    { WC_EDIT,              EditStyleArray,         EDITSTYLEARRAYSIZE        },
    { WC_STATIC,            StaticStyleArray,       STATICSTYLEARRAYSIZE      },
    { WC_LISTBOX,           ListBoxStyleArray,      LISTBOXSTYLEARRAYSIZE     },
    { WC_COMBOBOX,          ComboBoxStyleArray,     COMBOBOXSTYLEARRAYSIZE    },
    { WC_SYS_DIALOGBOX,     DialogStyleArray,       DIALOGSTYLEARRAYSIZE      },
#ifdef __NT__
    { ANIMATE_CLASS,        AnimateStyleArray,      ANIMATESTYLEARRAYSIZE     },
    { DATETIMEPICK_CLASS,   DateTimeStyleArray,     DATETIMESTYLEARRAYSIZE    },
    { MONTHCAL_CLASS,       MonthCalStyleArray,     MONTHCALSTYLEARRAYSIZE    },
    { PROGRESS_CLASS,       ProgressBarStyleArray,  PROGRESSBARSTYLEARRAYSIZE },
    { REBARCLASSNAME,       RebarStyleArray,        REBARSTYLEARRAYSIZE       },
    { STATUSCLASSNAME,      StatusBarStyleArray,    STATUSBARSTYLEARRAYSIZE   },
    { TOOLBARCLASSNAME,     ToolbarStyleArray,      TOOLBARSTYLEARRAYSIZE     },
    { TOOLTIPS_CLASS,       ToolTipsStyleArray,     TOOLTIPSSTYLEARRAYSIZE    },
    { TRACKBAR_CLASS,       TrackBarStyleArray,     TRACKBARSTYLEARRAYSIZE    },
    { UPDOWN_CLASS,         UpDownStyleArray,       UPDOWNSTYLEARRAYSIZE      },
    { WC_HEADER,            HeaderStyleArray,       HEADERSTYLEARRAYSIZE      },
    { WC_LISTVIEW,          ListViewStyleArray,     LISTVIEWSTYLEARRAYSIZE    },
    { WC_TABCONTROL,        TabControlStyleArray,   TABCONTROLSTYLEARRAYSIZE  },
    { WC_TREEVIEW,          TreeViewStyleArray,     TREEVIEWSTYLEARRAYSIZE    }
#endif
};

WORD ClassStylesSize = sizeof( ClassStyles ) / sizeof( class_styles );

style_info _NEAR StyleArray[] = {
    { "WS_POPUP",           WS_POPUP,           WS_POPUP            },
    { "WS_CHILD",           WS_CHILD,           WS_CHILD            },
    { "WS_OVERLAPPED",      WS_OVERLAPPED,      WS_POPUP | WS_CHILD },
    { "WS_BORDER",          WS_BORDER,          WS_BORDER           },
    { "WS_CAPTION",         WS_CAPTION,         WS_CAPTION          },
    { "WS_SYSMENU",         WS_SYSMENU,         WS_SYSMENU          },
    { "WS_THICKFRAME",      WS_THICKFRAME,      WS_THICKFRAME       },
    { "WS_MINIMIZEBOX",     WS_MINIMIZEBOX,     WS_MINIMIZEBOX      },
    { "WS_MAXIMIZEBOX",     WS_MAXIMIZEBOX,     WS_MAXIMIZEBOX      },
    { "WS_MINIMIZE",        WS_MINIMIZE,        WS_MINIMIZE         },
    { "WS_VISIBLE",         WS_VISIBLE,         WS_VISIBLE          },
    { "WS_DISABLED",        WS_DISABLED,        WS_DISABLED         },
    { "WS_CLIPSIBLINGS",    WS_CLIPSIBLINGS,    WS_CLIPSIBLINGS     },
    { "WS_CLIPCHILDREN",    WS_CLIPCHILDREN,    WS_CLIPCHILDREN     },
    { "WS_MAXIMIZE",        WS_MAXIMIZE,        WS_MAXIMIZE         },
    { "WS_DLGFRAME",        WS_DLGFRAME,        WS_DLGFRAME         },
    { "WS_VSCROLL",         WS_VSCROLL,         WS_VSCROLL          },
    { "WS_HSCROLL",         WS_HSCROLL,         WS_HSCROLL          },
    { "WS_GROUP",           WS_GROUP,           WS_GROUP            },
    { "WS_TABSTOP",         WS_TABSTOP,         WS_TABSTOP          }
};

WORD StyleArraySize = sizeof( StyleArray ) / sizeof( style_info );

style_info _NEAR ExStyleArray[] = {
    { "WS_EX_DLGMODALFRAME",    WS_EX_DLGMODALFRAME,    WS_EX_DLGMODALFRAME   },
    { "WS_EX_NOPARENTNOTIFY",   WS_EX_NOPARENTNOTIFY,   WS_EX_NOPARENTNOTIFY  },
    { "WS_EX_TOPMOST",          WS_EX_TOPMOST,          WS_EX_TOPMOST         },
    { "WS_EX_ACCEPTFILES",      WS_EX_ACCEPTFILES,      WS_EX_ACCEPTFILES     },
    { "WS_EX_TRANSPARENT",      WS_EX_TRANSPARENT,      WS_EX_TRANSPARENT     },
#ifdef __NT__
    { "WS_EX_MDICHILD",         WS_EX_MDICHILD,         WS_EX_MDICHILD        },
    { "WS_EX_TOOLWINDOW",       WS_EX_TOOLWINDOW,       WS_EX_TOOLWINDOW      },
    { "WS_EX_WINDOWEDGE",       WS_EX_WINDOWEDGE,       WS_EX_WINDOWEDGE      },
    { "WS_EX_CLIENTEDGE",       WS_EX_CLIENTEDGE,       WS_EX_CLIENTEDGE      },
    { "WS_EX_CONTEXTHELP",      WS_EX_CONTEXTHELP,      WS_EX_CONTEXTHELP     },
    { "WS_EX_RIGHT",            WS_EX_RIGHT,            WS_EX_RIGHT           },
    { "WS_EX_RTLREADING",       WS_EX_RTLREADING,       WS_EX_RTLREADING      },
    { "WS_EX_LEFTSCROLLBAR",    WS_EX_LEFTSCROLLBAR,    WS_EX_LEFTSCROLLBAR   },
    { "WS_EX_CONTROLPARENT",    WS_EX_CONTROLPARENT,    WS_EX_CONTROLPARENT   },
    { "WS_EX_STATICEDGE",       WS_EX_STATICEDGE,       WS_EX_STATICEDGE      },
    { "WS_EX_APPWINDOW",        WS_EX_APPWINDOW,        WS_EX_APPWINDOW       },
    { "WS_EX_LAYERED",          WS_EX_LAYERED,          WS_EX_LAYERED         },
    { "WS_EX_NOINHERITLAYOUT",  WS_EX_NOINHERITLAYOUT,  WS_EX_NOINHERITLAYOUT },
    { "WS_EX_LAYOUTRTL",        WS_EX_LAYOUTRTL,        WS_EX_LAYOUTRTL       },
    { "WS_EX_COMPOSITED",       WS_EX_COMPOSITED,       WS_EX_COMPOSITED      },
    { "WS_EX_NOACTIVATE",       WS_EX_NOACTIVATE,       WS_EX_NOACTIVATE      }
#endif
};

WORD ExStyleArraySize = sizeof( ExStyleArray ) / sizeof( style_info );

style_info _NEAR ClassStyleArray[] = {
    { "CS_VREDRAW",         CS_VREDRAW,         CS_VREDRAW         },
    { "CS_HREDRAW",         CS_HREDRAW,         CS_HREDRAW         },
    { "CS_KEYCVTWINDOW",    CS_KEYCVTWINDOW,    CS_KEYCVTWINDOW    },
    { "CS_DBLCLKS",         CS_DBLCLKS,         CS_DBLCLKS         },
    { "CS_OWNDC",           CS_OWNDC,           CS_OWNDC           },
    { "CS_CLASSDC",         CS_CLASSDC,         CS_CLASSDC         },
    { "CS_PARENTDC",        CS_PARENTDC,        CS_PARENTDC        },
    { "CS_NOKEYCVT",        CS_NOKEYCVT,        CS_NOKEYCVT        },
    { "CS_NOCLOSE",         CS_NOCLOSE,         CS_NOCLOSE         },
    { "CS_SAVEBITS",        CS_SAVEBITS,        CS_SAVEBITS        },
    { "CS_BYTEALIGNCLIENT", CS_BYTEALIGNCLIENT, CS_BYTEALIGNCLIENT },
    { "CS_BYTEALIGNWINDOW", CS_BYTEALIGNWINDOW, CS_BYTEALIGNWINDOW },
    { "CS_GLOBALCLASS",     CS_GLOBALCLASS,     CS_GLOBALCLASS     }
};

WORD ClassStyleArraySize = sizeof( ClassStyleArray ) / sizeof( style_info );
