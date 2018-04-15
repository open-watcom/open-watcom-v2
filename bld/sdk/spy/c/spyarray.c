/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2018 The Open Watcom Contributors. All Rights Reserved.
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

/**********************************************************/

#define msgpick1(a,b)       { true, false, a, #a, b, 0L },
#define msgpick1x(a,b,c)    { true, false, a, b, c, 0L },
#ifdef __WINDOWS__
#define msgpick2(a,b)
#else
#define msgpick2(a,b)       { true, false, a, #a, b, 0L },
#endif
#define msgpick3(a,b,c,d)   { true, false, a, b, c, d },
#ifdef __WINDOWS__
#define msgpick4(a,b,c,d)
#else
#define msgpick4(a,b,c,d)   { true, false, a, b, c, d },
#endif

static message _NEAR MessageArray[] =  {
    #include "spyarray.h"
};

#define MESSAGEARRAYSIZE            (sizeof( MessageArray )/sizeof( message ))

static message _NEAR EditMessageArray[] = {
    msgpick3( EM_GETSEL,               "EM_GETSEL",                MC_CONTROL, 0L )
    msgpick3( EM_SETSEL,               "EM_SETSEL",                MC_CONTROL, 0L )
    msgpick3( EM_GETRECT,              "EM_GETRECT",               MC_CONTROL, 0L )
    msgpick3( EM_SETRECT,              "EM_SETRECT",               MC_CONTROL, 0L )
    msgpick3( EM_SETRECTNP,            "EM_SETRECTNP",             MC_CONTROL, 0L )
    msgpick4( EM_SCROLL,               "EM_SCROLL",                MC_CONTROL, 0L )
    msgpick3( EM_LINESCROLL,           "EM_LINESCROLL",            MC_CONTROL, 0L )
    msgpick4( EM_SCROLLCARET,          "EM_SCROLLCARET",           MC_CONTROL, 0L )
    msgpick3( EM_GETMODIFY,            "EM_GETMODIFY",             MC_CONTROL, 0L )
    msgpick3( EM_SETMODIFY,            "EM_SETMODIFY",             MC_CONTROL, 0L )
    msgpick3( EM_GETLINECOUNT,         "EM_GETLINECOUNT",          MC_CONTROL, 0L )
    msgpick3( EM_LINEINDEX,            "EM_LINEINDEX",             MC_CONTROL, 0L )
    msgpick3( EM_SETHANDLE,            "EM_SETHANDLE",             MC_CONTROL, 0L )
    msgpick3( EM_GETHANDLE,            "EM_GETHANDLE",             MC_CONTROL, 0L )
    msgpick4( EM_GETTHUMB,             "EM_GETTHUMB",              MC_CONTROL, 0L )
    msgpick3( EM_LINELENGTH,           "EM_LINELENGTH",            MC_CONTROL, 0L )
    msgpick3( EM_REPLACESEL,           "EM_REPLACESEL",            MC_CONTROL, 0L )
    msgpick3( EM_GETLINE,              "EM_GETLINE",               MC_CONTROL, 0L )
    msgpick3( EM_LIMITTEXT,            "EM_LIMITTEXT",             MC_CONTROL, 0L )
    msgpick3( EM_CANUNDO,              "EM_CANUNDO",               MC_CONTROL, 0L )
    msgpick3( EM_UNDO,                 "EM_UNDO",                  MC_CONTROL, 0L )
    msgpick3( EM_FMTLINES,             "EM_FMTLINES",              MC_CONTROL, 0L )
    msgpick3( EM_LINEFROMCHAR,         "EM_LINEFROMCHAR",          MC_CONTROL, 0L )
    msgpick3( EM_SETTABSTOPS,          "EM_SETTABSTOPS",           MC_CONTROL, 0L )
    msgpick3( EM_SETPASSWORDCHAR,      "EM_SETPASSWORDCHAR",       MC_CONTROL, 0L )
    msgpick3( EM_EMPTYUNDOBUFFER,      "EM_EMPTYUNDOBUFFER",       MC_CONTROL, 0L )
    msgpick3( EM_GETFIRSTVISIBLELINE,  "EM_GETFIRSTVISIBLELINE",   MC_CONTROL, 0L )
    msgpick3( EM_SETREADONLY,          "EM_SETREADONLY",           MC_CONTROL, 0L )
    msgpick3( EM_SETWORDBREAKPROC,     "EM_SETWORDBREAKPROC",      MC_CONTROL, 0L )
    msgpick3( EM_GETWORDBREAKPROC,     "EM_GETWORDBREAKPROC",      MC_CONTROL, 0L )
    msgpick3( EM_GETPASSWORDCHAR,      "EM_GETPASSWORDCHAR",       MC_CONTROL, 0L )
    msgpick4( EM_SETMARGINS,           "EM_SETMARGINS",            MC_CONTROL, 0L )
    msgpick4( EM_GETMARGINS,           "EM_GETMARGINS",            MC_CONTROL, 0L )
    msgpick4( EM_GETLIMITTEXT,         "EM_GETLIMITTEXT",          MC_CONTROL, 0L )
    msgpick4( EM_POSFROMCHAR,          "EM_POSFROMCHAR",           MC_CONTROL, 0L )
    msgpick4( EM_CHARFROMPOS,          "EM_CHARFROMPOS",           MC_CONTROL, 0L )
    msgpick4( EM_SETIMESTATUS,         "EM_SETIMESTATUS",          MC_CONTROL, 0L )
    msgpick4( EM_GETIMESTATUS,         "EM_GETIMESTATUS",          MC_CONTROL, 0L )
};

#define EDITMESSAGEARRAYSIZE        (sizeof( EditMessageArray )/sizeof( message ))

static message _NEAR ButtonMessageArray[] = {
    msgpick3( BM_GETCHECK,     "BM_GETCHECK",      MC_CONTROL, 0L )
    msgpick3( BM_SETCHECK,     "BM_SETCHECK",      MC_CONTROL, 0L )
    msgpick3( BM_GETSTATE,     "BM_GETSTATE",      MC_CONTROL, 0L )
    msgpick3( BM_SETSTATE,     "BM_SETSTATE",      MC_CONTROL, 0L )
    msgpick3( BM_SETSTYLE,     "BM_SETSTYLE",      MC_CONTROL, 0L )
    msgpick4( BM_CLICK,        "BM_CLICK",         MC_CONTROL, 0L )
    msgpick4( BM_GETIMAGE,     "BM_GETIMAGE",      MC_CONTROL, 0L )
    msgpick4( BM_SETIMAGE,     "BM_SETIMAGE",      MC_CONTROL, 0L )
    msgpick4( BM_SETDONTCLICK, "BM_SETDONTCLICK",  MC_CONTROL, 0L )
};

#define BUTTONMESSAGEARRAYSIZE      (sizeof( ButtonMessageArray )/sizeof( message ))

static message _NEAR StaticMessageArray[] = {
    msgpick3( STM_SETICON,     "STM_SETICON",  MC_CONTROL, 0L )
    msgpick3( STM_GETICON,     "STM_GETICON",  MC_CONTROL, 0L )
    msgpick4( STM_SETIMAGE,    "STM_SETIMAGE", MC_CONTROL, 0L )
    msgpick4( STM_GETIMAGE,    "STM_GETIMAGE", MC_CONTROL, 0L )
};

#define STATICMESSAGEARRAYSIZE      (sizeof( StaticMessageArray )/sizeof( message ))

static message _NEAR ListBoxMessageArray[] = {
    msgpick3( LB_ADDSTRING,            "LB_ADDSTRING",             MC_CONTROL, 0L )
    msgpick3( LB_INSERTSTRING,         "LB_INSERTSTRING",          MC_CONTROL, 0L )
    msgpick3( LB_DELETESTRING,         "LB_DELETESTRING",          MC_CONTROL, 0L )
    msgpick4( LB_SELITEMRANGEEX,       "LB_SELITEMRANGEEX",        MC_CONTROL, 0L )
    msgpick3( LB_RESETCONTENT,         "LB_RESETCONTENT",          MC_CONTROL, 0L )
    msgpick3( LB_SETSEL,               "LB_SETSEL",                MC_CONTROL, 0L )
    msgpick3( LB_SETCURSEL,            "LB_SETCURSEL",             MC_CONTROL, 0L )
    msgpick3( LB_GETSEL,               "LB_GETSEL",                MC_CONTROL, 0L )
    msgpick3( LB_GETCURSEL,            "LB_GETCURSEL",             MC_CONTROL, 0L )
    msgpick3( LB_GETTEXT,              "LB_GETTEXT",               MC_CONTROL, 0L )
    msgpick3( LB_GETTEXTLEN,           "LB_GETTEXTLEN",            MC_CONTROL, 0L )
    msgpick3( LB_GETCOUNT,             "LB_GETCOUNT",              MC_CONTROL, 0L )
    msgpick3( LB_SELECTSTRING,         "LB_SELECTSTRING",          MC_CONTROL, 0L )
    msgpick3( LB_DIR,                  "LB_DIR",                   MC_CONTROL, 0L )
    msgpick3( LB_GETTOPINDEX,          "LB_GETTOPINDEX",           MC_CONTROL, 0L )
    msgpick3( LB_FINDSTRING,           "LB_FINDSTRING",            MC_CONTROL, 0L )
    msgpick3( LB_GETSELCOUNT,          "LB_GETSELCOUNT",           MC_CONTROL, 0L )
    msgpick3( LB_GETSELITEMS,          "LB_GETSELITEMS",           MC_CONTROL, 0L )
    msgpick3( LB_SETTABSTOPS,          "LB_SETTABSTOPS",           MC_CONTROL, 0L )
    msgpick3( LB_GETHORIZONTALEXTENT,  "LB_GETHORIZONTALEXTENT",   MC_CONTROL, 0L )
    msgpick3( LB_SETHORIZONTALEXTENT,  "LB_SETHORIZONTALEXTENT",   MC_CONTROL, 0L )
    msgpick3( LB_SETCOLUMNWIDTH,       "LB_SETCOLUMNWIDTH",        MC_CONTROL, 0L )
    msgpick4( LB_ADDFILE,              "LB_ADDFILE",               MC_CONTROL, 0L )
    msgpick3( LB_SETTOPINDEX,          "LB_SETTOPINDEX",           MC_CONTROL, 0L )
    msgpick3( LB_GETITEMRECT,          "LB_GETITEMRECT",           MC_CONTROL, 0L )
    msgpick3( LB_GETITEMDATA,          "LB_GETITEMDATA",           MC_CONTROL, 0L )
    msgpick3( LB_SETITEMDATA,          "LB_SETITEMDATA",           MC_CONTROL, 0L )
    msgpick3( LB_SELITEMRANGE,         "LB_SELITEMRANGE",          MC_CONTROL, 0L )
    msgpick4( LB_SETANCHORINDEX,       "LB_SETANCHORINDEX",        MC_CONTROL, 0L )
    msgpick4( LB_GETANCHORINDEX,       "LB_GETANCHORINDEX",        MC_CONTROL, 0L )
    msgpick3( LB_SETCARETINDEX,        "LB_SETCARETINDEX",         MC_CONTROL, 0L )
    msgpick3( LB_GETCARETINDEX,        "LB_GETCARETINDEX",         MC_CONTROL, 0L )
    msgpick3( LB_SETITEMHEIGHT,        "LB_SETITEMHEIGHT",         MC_CONTROL, 0L )
    msgpick3( LB_GETITEMHEIGHT,        "LB_GETITEMHEIGHT",         MC_CONTROL, 0L )
    msgpick3( LB_FINDSTRINGEXACT,      "LB_FINDSTRINGEXACT",       MC_CONTROL, 0L )
    msgpick4( LB_SETLOCALE,            "LB_SETLOCALE",             MC_CONTROL, 0L )
    msgpick4( LB_GETLOCALE,            "LB_GETLOCALE",             MC_CONTROL, 0L )
    msgpick4( LB_SETCOUNT,             "LB_SETCOUNT",              MC_CONTROL, 0L )
    msgpick4( LB_INITSTORAGE,          "LB_INITSTORAGE",           MC_CONTROL, 0L )
    msgpick4( LB_ITEMFROMPOINT,        "LB_ITEMFROMPOINT",         MC_CONTROL, 0L )
    msgpick4( LB_GETLISTBOXINFO,       "LB_GETLISTBOXINFO",        MC_CONTROL, 0L )
};

#define LISTBOXMESSAGEARRAYSIZE     (sizeof( ListBoxMessageArray )/sizeof( message ))

#define combobox_messages_old() \
    msgpick3( CB_GETEDITSEL,           "CB_GETEDITSEL",            MC_CONTROL, 0L ) \
    msgpick3( CB_LIMITTEXT,            "CB_LIMITTEXT",             MC_CONTROL, 0L ) \
    msgpick3( CB_SETEDITSEL,           "CB_SETEDITSEL",            MC_CONTROL, 0L ) \
    msgpick3( CB_ADDSTRING,            "CB_ADDSTRING",             MC_CONTROL, 0L ) \
    msgpick3( CB_DELETESTRING,         "CB_DELETESTRING",          MC_CONTROL, 0L ) \
    msgpick3( CB_DIR,                  "CB_DIR",                   MC_CONTROL, 0L ) \
    msgpick3( CB_GETCOUNT,             "CB_GETCOUNT",              MC_CONTROL, 0L ) \
    msgpick3( CB_GETCURSEL,            "CB_GETCURSEL",             MC_CONTROL, 0L ) \
    msgpick3( CB_GETLBTEXT,            "CB_GETLBTEXT",             MC_CONTROL, 0L ) \
    msgpick3( CB_GETLBTEXTLEN,         "CB_GETLBTEXTLEN",          MC_CONTROL, 0L ) \
    msgpick3( CB_INSERTSTRING,         "CB_INSERTSTRING",          MC_CONTROL, 0L ) \
    msgpick3( CB_RESETCONTENT,         "CB_RESETCONTENT",          MC_CONTROL, 0L ) \
    msgpick3( CB_FINDSTRING,           "CB_FINDSTRING",            MC_CONTROL, 0L ) \
    msgpick3( CB_SELECTSTRING,         "CB_SELECTSTRING",          MC_CONTROL, 0L ) \
    msgpick3( CB_SETCURSEL,            "CB_SETCURSEL",             MC_CONTROL, 0L ) \
    msgpick3( CB_SHOWDROPDOWN,         "CB_SHOWDROPDOWN",          MC_CONTROL, 0L ) \
    msgpick3( CB_GETITEMDATA,          "CB_GETITEMDATA",           MC_CONTROL, 0L ) \
    msgpick3( CB_SETITEMDATA,          "CB_SETITEMDATA",           MC_CONTROL, 0L ) \
    msgpick3( CB_GETDROPPEDCONTROLRECT,"CB_GETDROPPEDCONTROLRECT", MC_CONTROL, 0L ) \
    msgpick3( CB_SETITEMHEIGHT,        "CB_SETITEMHEIGHT",         MC_CONTROL, 0L ) \
    msgpick3( CB_GETITEMHEIGHT,        "CB_GETITEMHEIGHT",         MC_CONTROL, 0L ) \
    msgpick3( CB_SETEXTENDEDUI,        "CB_SETEXTENDEDUI",         MC_CONTROL, 0L ) \
    msgpick3( CB_GETEXTENDEDUI,        "CB_GETEXTENDEDUI",         MC_CONTROL, 0L ) \
    msgpick3( CB_GETDROPPEDSTATE,      "CB_GETDROPPEDSTATE",       MC_CONTROL, 0L ) \
    msgpick3( CB_FINDSTRINGEXACT,      "CB_FINDSTRINGEXACT",       MC_CONTROL, 0L )

#define combobox_messages_new() \
    msgpick3( CB_SETLOCALE,            "CB_SETLOCALE",             MC_CONTROL, 0L ) \
    msgpick3( CB_GETLOCALE,            "CB_GETLOCALE",             MC_CONTROL, 0L ) \
    msgpick3( CB_GETTOPINDEX,          "CB_GETTOPINDEX",           MC_CONTROL, 0L ) \
    msgpick3( CB_SETTOPINDEX,          "CB_SETTOPINDEX",           MC_CONTROL, 0L ) \
    msgpick3( CB_GETHORIZONTALEXTENT,  "CB_GETHORIZONTALEXTENT",   MC_CONTROL, 0L ) \
    msgpick3( CB_SETHORIZONTALEXTENT,  "CB_SETHORIZONTALEXTENT",   MC_CONTROL, 0L ) \
    msgpick3( CB_GETDROPPEDWIDTH,      "CB_GETDROPPEDWIDTH",       MC_CONTROL, 0L ) \
    msgpick3( CB_SETDROPPEDWIDTH,      "CB_SETDROPPEDWIDTH",       MC_CONTROL, 0L ) \
    msgpick3( CB_INITSTORAGE,          "CB_INITSTORAGE",           MC_CONTROL, 0L ) \
    msgpick3( CB_GETCOMBOBOXINFO,      "CB_GETCOMBOBOXINFO",       MC_CONTROL, 0L )

static message _NEAR ComboBoxMessageArray[] = {
    combobox_messages_old()
#ifdef __NT__
    combobox_messages_new()
#endif
};

#define COMBOBOXMESSAGEARRAYSIZE    (sizeof( ComboBoxMessageArray )/sizeof( message ))

#ifdef __NT__
static message _NEAR ScrollBarMessageArray[] = {
    msgpick3( SBM_SETPOS,              "SBM_SETPOS",           MC_CONTROL, 0L )
    msgpick3( SBM_GETPOS,              "SBM_GETPOS",           MC_CONTROL, 0L )
    msgpick3( SBM_SETRANGE,            "SBM_SETRANGE",         MC_CONTROL, 0L )
    msgpick3( SBM_SETRANGEREDRAW,      "SBM_SETRANGEREDRAW",   MC_CONTROL, 0L )
    msgpick3( SBM_GETRANGE,            "SBM_GETRANGE",         MC_CONTROL, 0L )
    msgpick3( SBM_ENABLE_ARROWS,       "SBM_ENABLE_ARROWS",    MC_CONTROL, 0L )
    msgpick3( SBM_SETSCROLLINFO,       "SBM_SETSCROLLINFO",    MC_CONTROL, 0L )
    msgpick3( SBM_GETSCROLLINFO,       "SBM_GETSCROLLINFO",    MC_CONTROL, 0L )
    msgpick3( SBM_GETSCROLLBARINFO,    "SBM_GETSCROLLBARINFO", MC_CONTROL, 0L )
};

#define SCROLLBARMESSAGEARRAYSIZE   (sizeof( ScrollBarMessageArray )/sizeof( message ))

static message _NEAR HeaderMessageArray[] = {
    msgpick3( HDM_GETITEMCOUNT,        "HDM_GETITEMCOUNT",         MC_CONTROL, 0L )
    msgpick3( HDM_INSERTITEMA,         "HDM_INSERTITEM",           MC_CONTROL, 0L )
    msgpick3( HDM_DELETEITEM,          "HDM_DELETEITEM",           MC_CONTROL, 0L )
    msgpick3( HDM_GETITEMA,            "HDM_GETITEM",              MC_CONTROL, 0L )
    msgpick3( HDM_SETITEMA,            "HDM_SETITEM",              MC_CONTROL, 0L )
    msgpick3( HDM_LAYOUT,              "HDM_LAYOUT",               MC_CONTROL, 0L )
    msgpick3( HDM_HITTEST,             "HDM_HITTEST",              MC_CONTROL, 0L )
    msgpick3( HDM_GETITEMRECT,         "HDM_GETITEMRECT",          MC_CONTROL, 0L )
    msgpick3( HDM_SETIMAGELIST,        "HDM_SETIMAGELIST",         MC_CONTROL, 0L )
    msgpick3( HDM_GETIMAGELIST,        "HDM_GETIMAGELIST",         MC_CONTROL, 0L )
    msgpick3( HDM_INSERTITEMW,         "HDM_INSERTITEM",           MC_CONTROL, 0L )
    msgpick3( HDM_GETITEMW,            "HDM_GETITEM",              MC_CONTROL, 0L )
    msgpick3( HDM_SETITEMW,            "HDM_SETITEM",              MC_CONTROL, 0L )
    msgpick3( HDM_ORDERTOINDEX,        "HDM_ORDERTOINDEX",         MC_CONTROL, 0L )
    msgpick3( HDM_CREATEDRAGIMAGE,     "HDM_CREATEDRAGIMAGE",      MC_CONTROL, 0L )
    msgpick3( HDM_GETORDERARRAY,       "HDM_GETORDERARRAY",        MC_CONTROL, 0L )
    msgpick3( HDM_SETORDERARRAY,       "HDM_SETORDERARRAY",        MC_CONTROL, 0L )
    msgpick3( HDM_SETHOTDIVIDER,       "HDM_SETHOTDIVIDER",        MC_CONTROL, 0L )
    msgpick3( HDM_SETBITMAPMARGIN,     "HDM_SETBITMAPMARGIN",      MC_CONTROL, 0L )
    msgpick3( HDM_GETBITMAPMARGIN,     "HDM_GETBITMAPMARGIN",      MC_CONTROL, 0L )
    msgpick3( HDM_SETFILTERCHANGETIMEOUT,"HDM_SETFILTERCHANGETIMEOUT",MC_CONTROL, 0L )
    msgpick3( HDM_EDITFILTER,          "HDM_EDITFILTER",           MC_CONTROL, 0L )
    msgpick3( HDM_CLEARFILTER,         "HDM_CLEARFILTER",          MC_CONTROL, 0L )
    msgpick3( HDM_GETITEMDROPDOWNRECT, "HDM_GETITEMDROPDOWNRECT",  MC_CONTROL, 0L )
    msgpick3( HDM_GETOVERFLOWRECT,     "HDM_GETOVERFLOWRECT",      MC_CONTROL, 0L )
    msgpick3( HDM_GETFOCUSEDITEM,      "HDM_GETFOCUSEDITEM",       MC_CONTROL, 0L )
    msgpick3( HDM_SETFOCUSEDITEM,      "HDM_SETFOCUSEDITEM",       MC_CONTROL, 0L )
};

#define HEADERMESSAGEARRAYSIZE      (sizeof( HeaderMessageArray )/sizeof( message ))

static message _NEAR ToolbarMessageArray[] = {
    msgpick3( TB_ENABLEBUTTON,         "TB_ENABLEBUTTON",          MC_CONTROL, 0L )
    msgpick3( TB_CHECKBUTTON,          "TB_CHECKBUTTON",           MC_CONTROL, 0L )
    msgpick3( TB_PRESSBUTTON,          "TB_PRESSBUTTON",           MC_CONTROL, 0L )
    msgpick3( TB_HIDEBUTTON,           "TB_HIDEBUTTON",            MC_CONTROL, 0L )
    msgpick3( TB_INDETERMINATE,        "TB_INDETERMINATE",         MC_CONTROL, 0L )
    msgpick3( TB_MARKBUTTON,           "TB_MARKBUTTON",            MC_CONTROL, 0L )
    msgpick3( TB_ISBUTTONENABLED,      "TB_ISBUTTONENABLED",       MC_CONTROL, 0L )
    msgpick3( TB_ISBUTTONCHECKED,      "TB_ISBUTTONCHECKED",       MC_CONTROL, 0L )
    msgpick3( TB_ISBUTTONPRESSED,      "TB_ISBUTTONPRESSED",       MC_CONTROL, 0L )
    msgpick3( TB_ISBUTTONHIDDEN,       "TB_ISBUTTONHIDEEN",        MC_CONTROL, 0L )
    msgpick3( TB_ISBUTTONINDETERMINATE,"TB_ISBUTTONINDETERMINATE", MC_CONTROL, 0L )
    msgpick3( TB_ISBUTTONHIGHLIGHTED,  "TB_ISBUTTONHIGHLIGHTED",   MC_CONTROL, 0L )
    msgpick3( TB_SETSTATE,             "TB_SETSTATE",              MC_CONTROL, 0L )
    msgpick3( TB_GETSTATE,             "TB_GETSTATE",              MC_CONTROL, 0L )
    msgpick3( TB_ADDBITMAP,            "TB_ADDBITMAP",             MC_CONTROL, 0L )
    msgpick3( TB_ADDBUTTONSA,          "TB_ADDBUTTONS",            MC_CONTROL, 0L )
    msgpick3( TB_INSERTBUTTONA,        "TB_INSERTBUTTON",          MC_CONTROL, 0L )
    msgpick3( TB_DELETEBUTTON,         "TB_DELETEBUTTON",          MC_CONTROL, 0L )
    msgpick3( TB_GETBUTTON,            "TB_GETBUTTON",             MC_CONTROL, 0L )
    msgpick3( TB_BUTTONCOUNT,          "TB_BUTTONCOUNT",           MC_CONTROL, 0L )
    msgpick3( TB_COMMANDTOINDEX,       "TB_COMMANDTOINDEX",        MC_CONTROL, 0L )
    msgpick3( TB_SAVERESTOREA,         "TB_SAVERESTORE",           MC_CONTROL, 0L )
    msgpick3( TB_CUSTOMIZE,            "TB_CUSTOMIZE",             MC_CONTROL, 0L )
    msgpick3( TB_ADDSTRINGA,           "TB_ADDSTRING",             MC_CONTROL, 0L )
    msgpick3( TB_GETITEMRECT,          "TB_GETITEMRECT",           MC_CONTROL, 0L )
    msgpick3( TB_BUTTONSTRUCTSIZE,     "TB_BUTTONSTRUCTSIZE",      MC_CONTROL, 0L )
    msgpick3( TB_SETBUTTONSIZE,        "TB_SETBUTTONSIZE",         MC_CONTROL, 0L )
    msgpick3( TB_SETBITMAPSIZE,        "TB_SETBITMAPSIZE",         MC_CONTROL, 0L )
    msgpick3( TB_AUTOSIZE,             "TB_AUTOSIZE",              MC_CONTROL, 0L )
    msgpick3( TB_GETTOOLTIPS,          "TB_GETTOOLTIPS",           MC_CONTROL, 0L )
    msgpick3( TB_SETTOOLTIPS,          "TB_SETTOOLTIPS",           MC_CONTROL, 0L )
    msgpick3( TB_SETPARENT,            "TB_SETPARENT",             MC_CONTROL, 0L )
    msgpick3( TB_SETROWS,              "TB_SETROWS",               MC_CONTROL, 0L )
    msgpick3( TB_GETROWS,              "TB_GETROWS",               MC_CONTROL, 0L )
    msgpick3( TB_GETBITMAPFLAGS,       "TB_GETBITMAPFLAGS",        MC_CONTROL, 0L )
    msgpick3( TB_SETCMDID,             "TB_SETCMDID",              MC_CONTROL, 0L )
    msgpick3( TB_CHANGEBITMAP,         "TB_CHANGEBITMAP",          MC_CONTROL, 0L )
    msgpick3( TB_GETBITMAP,            "TB_GETBITMAP",             MC_CONTROL, 0L )
    msgpick3( TB_GETBUTTONTEXTA,       "TB_GETBUTTONTEXT",         MC_CONTROL, 0L )
    msgpick3( TB_REPLACEBITMAP,        "TB_REPLACEBITMAP",         MC_CONTROL, 0L )
    msgpick3( TB_SETINDENT,            "TB_SETINDENT",             MC_CONTROL, 0L )
    msgpick3( TB_SETIMAGELIST,         "TB_SETIMAGELIST",          MC_CONTROL, 0L )
    msgpick3( TB_GETIMAGELIST,         "TB_GETIMAGELIST",          MC_CONTROL, 0L )
    msgpick3( TB_LOADIMAGES,           "TB_LOADIMAGES",            MC_CONTROL, 0L )
    msgpick3( TB_GETRECT,              "TB_GETRECT",               MC_CONTROL, 0L )
    msgpick3( TB_SETHOTIMAGELIST,      "TB_SETHOTIMAGELIST",       MC_CONTROL, 0L )
    msgpick3( TB_GETHOTIMAGELIST,      "TB_GETHOTIMAGELIST",       MC_CONTROL, 0L )
    msgpick3( TB_SETDISABLEDIMAGELIST, "TB_SETDISABLEDIMAGELIST",  MC_CONTROL, 0L )
    msgpick3( TB_GETDISABLEDIMAGELIST, "TB_GETDISABLEDIMAGELIST",  MC_CONTROL, 0L )
    msgpick3( TB_SETSTYLE,             "TB_SETSTYLE",              MC_CONTROL, 0L )
    msgpick3( TB_GETSTYLE,             "TB_GETSTYLE",              MC_CONTROL, 0L )
    msgpick3( TB_GETBUTTONSIZE,        "TB_GETBUTTONSIZE",         MC_CONTROL, 0L )
    msgpick3( TB_SETBUTTONWIDTH,       "TB_SETBUTTONWIDTH",        MC_CONTROL, 0L )
    msgpick3( TB_SETMAXTEXTROWS,       "TB_SETMAXTEXTROWS",        MC_CONTROL, 0L )
    msgpick3( TB_GETTEXTROWS,          "TB_GETTEXTROWS",           MC_CONTROL, 0L )
    msgpick3( TB_GETOBJECT,            "TB_GETOBJECT",             MC_CONTROL, 0L )
    msgpick3( TB_GETBUTTONINFOW,       "TB_GETBUTTONINFO",         MC_CONTROL, 0L )
    msgpick3( TB_SETBUTTONINFOW,       "TB_SETBUTTONINFO",         MC_CONTROL, 0L )
    msgpick3( TB_GETBUTTONINFOA,       "TB_GETBUTTONINFO",         MC_CONTROL, 0L )
    msgpick3( TB_SETBUTTONINFOA,       "TB_SETBUTTONINFO",         MC_CONTROL, 0L )
    msgpick3( TB_INSERTBUTTONW,        "TB_INSERTBUTTON",          MC_CONTROL, 0L )
    msgpick3( TB_ADDBUTTONSW,          "TB_ADDBUTTONS",            MC_CONTROL, 0L )
    msgpick3( TB_HITTEST,              "TB_HITTEST",               MC_CONTROL, 0L )
    msgpick3( TB_SETDRAWTEXTFLAGS,     "TB_SETDRAWTEXTFLAGS",      MC_CONTROL, 0L )
    msgpick3( TB_SETHOTITEM,           "TB_SETHOTITEM",            MC_CONTROL, 0L )
    msgpick3( TB_GETHOTITEM,           "TB_GETHOTITEM",            MC_CONTROL, 0L )
    msgpick3( TB_SETANCHORHIGHLIGHT,   "TB_SETANCHORHIGHLIGHT",    MC_CONTROL, 0L )
    msgpick3( TB_GETANCHORHIGHLIGHT,   "TB_GETANCHORHIGHLIGHT",    MC_CONTROL, 0L )
    msgpick3( TB_GETBUTTONTEXTW,       "TB_GETBUTTONTEXT",         MC_CONTROL, 0L )
    msgpick3( TB_SAVERESTOREW,         "TB_SAVERESTORE",           MC_CONTROL, 0L )
    msgpick3( TB_ADDSTRINGW,           "TB_ADDSTRING",             MC_CONTROL, 0L )
    msgpick3( TB_MAPACCELERATORA,      "TB_MAPACCELERATOR",        MC_CONTROL, 0L )
    msgpick3( TB_GETINSERTMARK,        "TB_GETINSERTMARK",         MC_CONTROL, 0L )
    msgpick3( TB_SETINSERTMARK,        "TB_SETINSERTMARK",         MC_CONTROL, 0L )
    msgpick3( TB_INSERTMARKHITTEST,    "TB_INSERTMARKHITTEST",     MC_CONTROL, 0L )
    msgpick3( TB_MOVEBUTTON,           "TB_MOVEBUTTON",            MC_CONTROL, 0L )
    msgpick3( TB_GETMAXSIZE,           "TB_GETMAXSIZE",            MC_CONTROL, 0L )
    msgpick3( TB_SETEXTENDEDSTYLE,     "TB_SETEXTENDEDSTYLE",      MC_CONTROL, 0L )
    msgpick3( TB_GETEXTENDEDSTYLE,     "TB_GETEXTENDEDSTYLE",      MC_CONTROL, 0L )
    msgpick3( TB_GETPADDING,           "TB_GETPADDING",            MC_CONTROL, 0L )
    msgpick3( TB_SETPADDING,           "TB_SETPADDING",            MC_CONTROL, 0L )
    msgpick3( TB_SETINSERTMARKCOLOR,   "TB_SETINSERTMARKCOLOR",    MC_CONTROL, 0L )
    msgpick3( TB_GETINSERTMARKCOLOR,   "TB_GETINSERTMARKCOLOR",    MC_CONTROL, 0L )
    msgpick3( TB_MAPACCELERATORW,      "TB_MAPACCELERATOR",        MC_CONTROL, 0L )
    msgpick3( TB_GETSTRINGW,           "TB_GETSTRING",             MC_CONTROL, 0L )
    msgpick3( TB_GETSTRINGA,           "TB_GETSTRING",             MC_CONTROL, 0L )
    msgpick3( TB_GETMETRICS,           "TB_GETMETRICS",            MC_CONTROL, 0L )
    msgpick3( TB_SETMETRICS,           "TB_SETMETRICS",            MC_CONTROL, 0L )
    msgpick3( TB_SETPRESSEDIMAGELIST,  "TB_SETPRESSEDIMAGELIST",   MC_CONTROL, 0L )
    msgpick3( TB_GETPRESSEDIMAGELIST,  "TB_GETPRESSEDIMAGELIST",   MC_CONTROL, 0L )
};

#define TOOLBARMESSAGEARRAYSIZE     (sizeof( ToolbarMessageArray )/sizeof( message ))

static message _NEAR RebarMessageArray[] = {
    msgpick3( RB_INSERTBANDA,      "RB_INSERTBAND",        MC_CONTROL, 0L )
    msgpick3( RB_DELETEBAND,       "RB_DELETEBAND",        MC_CONTROL, 0L )
    msgpick3( RB_GETBARINFO,       "RB_GETBARINFO",        MC_CONTROL, 0L )
    msgpick3( RB_SETBARINFO,       "RB_SETBARINFO",        MC_CONTROL, 0L )
    msgpick3( RB_GETBANDINFO_IE3,  "RB_GETBANDINFO",       MC_CONTROL, 0L )
    msgpick3( RB_SETBANDINFOA,     "RB_SETBANDINFO",       MC_CONTROL, 0L )
    msgpick3( RB_SETPARENT,        "RB_SETPARENT",         MC_CONTROL, 0L )
    msgpick3( RB_HITTEST,          "RB_HITTEST",           MC_CONTROL, 0L )
    msgpick3( RB_GETRECT,          "RB_GETRECT",           MC_CONTROL, 0L )
    msgpick3( RB_INSERTBANDW,      "RB_INSERTBAND",        MC_CONTROL, 0L )
    msgpick3( RB_SETBANDINFOW,     "RB_SETBANDINFO",       MC_CONTROL, 0L )
    msgpick3( RB_GETBANDCOUNT,     "RB_GETBANDCOUNT",      MC_CONTROL, 0L )
    msgpick3( RB_GETROWCOUNT,      "RB_GETROWCOUNT",       MC_CONTROL, 0L )
    msgpick3( RB_GETROWHEIGHT,     "RB_GETROWHEIGHT",      MC_CONTROL, 0L )
    msgpick3( RB_IDTOINDEX,        "RB_IDTOINDEX",         MC_CONTROL, 0L )
    msgpick3( RB_GETTOOLTIPS,      "RB_GETTOOLTIPS",       MC_CONTROL, 0L )
    msgpick3( RB_SETTOOLTIPS,      "RB_SETTOOLTIPS",       MC_CONTROL, 0L )
    msgpick3( RB_SETBKCOLOR,       "RB_SETBKCOLOR",        MC_CONTROL, 0L )
    msgpick3( RB_GETBKCOLOR,       "RB_GETBKCOLOR",        MC_CONTROL, 0L )
    msgpick3( RB_SETTEXTCOLOR,     "RB_SETTEXTCOLOR",      MC_CONTROL, 0L )
    msgpick3( RB_GETTEXTCOLOR,     "RB_GETTEXTCOLOR",      MC_CONTROL, 0L )
    msgpick3( RB_SIZETORECT,       "RB_SIZETORECT",        MC_CONTROL, 0L )
    msgpick3( RB_BEGINDRAG,        "RB_BEGINDRAG",         MC_CONTROL, 0L )
    msgpick3( RB_ENDDRAG,          "RB_ENDDRAG",           MC_CONTROL, 0L )
    msgpick3( RB_DRAGMOVE,         "RB_DRAGMOVE",          MC_CONTROL, 0L )
    msgpick3( RB_GETBARHEIGHT,     "RB_GETBARHEIGHT",      MC_CONTROL, 0L )
    msgpick3( RB_GETBANDINFOW,     "RB_GETBANDINFO",       MC_CONTROL, 0L )
    msgpick3( RB_GETBANDINFOA,     "RB_GETBANDINFO",       MC_CONTROL, 0L )
    msgpick3( RB_MINIMIZEBAND,     "RB_MINIMIZEBAND",      MC_CONTROL, 0L )
    msgpick3( RB_MAXIMIZEBAND,     "RB_MAXIMIZEBAND",      MC_CONTROL, 0L )
    msgpick3( RB_GETBANDBORDERS,   "RB_GETBANDBORDERS",    MC_CONTROL, 0L )
    msgpick3( RB_SHOWBAND,         "RB_SHOWBAND",          MC_CONTROL, 0L )
    msgpick3( RB_SETPALETTE,       "RB_SETPALETTE",        MC_CONTROL, 0L )
    msgpick3( RB_GETPALETTE,       "RB_GETPALETTE",        MC_CONTROL, 0L )
    msgpick3( RB_MOVEBAND,         "RB_MOVEBAND",          MC_CONTROL, 0L )
    msgpick3( RB_GETBANDMARGINS,   "RB_GETBANDMARGINS",    MC_CONTROL, 0L )
    msgpick3( RB_SETEXTENDEDSTYLE, "RB_SETEXTENDEDSTYLE",  MC_CONTROL, 0L )
    msgpick3( RB_GETEXTENDEDSTYLE, "RB_GETEXTENDEDSTYLE",  MC_CONTROL, 0L )
    msgpick3( RB_PUSHCHEVRON,      "RB_PUSHCHEVRON",       MC_CONTROL, 0L )
    msgpick3( RB_SETBANDWIDTH,     "RB_SETBANDWIDTH",      MC_CONTROL, 0L )
};

#define REBARMESSAGEARRAYSIZE       (sizeof( RebarMessageArray )/sizeof( message ))

static message _NEAR ToolTipsMessageArray[] = {
    msgpick3( TTM_ACTIVATE,        "TTM_ACTIVATE",         MC_CONTROL, 0L )
    msgpick3( TTM_SETDELAYTIME,    "TTM_SETDELAYTIME",     MC_CONTROL, 0L )
    msgpick3( TTM_ADDTOOLA,        "TTM_ADDTOOL",          MC_CONTROL, 0L )
    msgpick3( TTM_DELTOOLA,        "TTM_DELTOOL",          MC_CONTROL, 0L )
    msgpick3( TTM_NEWTOOLRECTA,    "TTM_NEWTOOLRECT",      MC_CONTROL, 0L )
    msgpick3( TTM_RELAYEVENT,      "TTM_RELAYEVENT",       MC_CONTROL, 0L )
    msgpick3( TTM_GETTOOLINFOA,    "TTM_GETTOOLINFO",      MC_CONTROL, 0L )
    msgpick3( TTM_SETTOOLINFOA,    "TTM_SETTOOLINFO",      MC_CONTROL, 0L )
    msgpick3( TTM_HITTESTA,        "TTM_HITTEST",          MC_CONTROL, 0L )
    msgpick3( TTM_GETTEXTA,        "TTM_GETTEXT",          MC_CONTROL, 0L )
    msgpick3( TTM_UPDATETIPTEXTA,  "TTM_UPDATETIPTEXT",    MC_CONTROL, 0L )
    msgpick3( TTM_GETTOOLCOUNT,    "TTM_GETTOOLCOUNT",     MC_CONTROL, 0L )
    msgpick3( TTM_ENUMTOOLSA,      "TTM_ENUMTOOLS",        MC_CONTROL, 0L )
    msgpick3( TTM_GETCURRENTTOOLA, "TTM_GETCURRENTTOOL",   MC_CONTROL, 0L )
    msgpick3( TTM_WINDOWFROMPOINT, "TTM_WINDOWFROMPOINT",  MC_CONTROL, 0L )
    msgpick3( TTM_TRACKACTIVATE,   "TTM_TRACKACTIVATE",    MC_CONTROL, 0L )
    msgpick3( TTM_TRACKPOSITION,   "TTM_TRACKPOSITION",    MC_CONTROL, 0L )
    msgpick3( TTM_SETTIPBKCOLOR,   "TTM_SETTIPBKCOLOR",    MC_CONTROL, 0L )
    msgpick3( TTM_SETTIPTEXTCOLOR, "TTM_SETTIPTEXTCOLOR",  MC_CONTROL, 0L )
    msgpick3( TTM_GETDELAYTIME,    "TTM_GETDELAYTIME",     MC_CONTROL, 0L )
    msgpick3( TTM_GETTIPBKCOLOR,   "TTM_GETTIPBKCOLOR",    MC_CONTROL, 0L )
    msgpick3( TTM_GETTIPTEXTCOLOR, "TTM_GETTIPTEXTCOLOR",  MC_CONTROL, 0L )
    msgpick3( TTM_SETMAXTIPWIDTH,  "TTM_SETMAXTIPWIDTH",   MC_CONTROL, 0L )
    msgpick3( TTM_GETMAXTIPWIDTH,  "TTM_GETMAXTIPWIDTH",   MC_CONTROL, 0L )
    msgpick3( TTM_SETMARGIN,       "TTM_SETMARGIN",        MC_CONTROL, 0L )
    msgpick3( TTM_GETMARGIN,       "TTM_GETMARGIN",        MC_CONTROL, 0L )
    msgpick3( TTM_POP,             "TTM_POP",              MC_CONTROL, 0L )
    msgpick3( TTM_UPDATE,          "TTM_UPDATE",           MC_CONTROL, 0L )
    msgpick3( TTM_GETBUBBLESIZE,   "TTM_GETBUBBLESIZE",    MC_CONTROL, 0L )
    msgpick3( TTM_ADJUSTRECT,      "TTM_ADJUSTRECT",       MC_CONTROL, 0L )
    msgpick3( TTM_SETTITLEA,       "TTM_SETTITLE",         MC_CONTROL, 0L )
    msgpick3( TTM_SETTITLEW,       "TTM_SETTITLE",         MC_CONTROL, 0L )
    msgpick3( TTM_POPUP,           "TTM_POPUP",            MC_CONTROL, 0L )
    msgpick3( TTM_GETTITLE,        "TTM_GETTITLE",         MC_CONTROL, 0L )
    msgpick3( TTM_ADDTOOLW,        "TTM_ADDTOOL",          MC_CONTROL, 0L )
    msgpick3( TTM_DELTOOLW,        "TTM_DELTOOL",          MC_CONTROL, 0L )
    msgpick3( TTM_NEWTOOLRECTW,    "TTM_NEWTOOLRECT",      MC_CONTROL, 0L )
    msgpick3( TTM_GETTOOLINFOW,    "TTM_GETTOOLINFO",      MC_CONTROL, 0L )
    msgpick3( TTM_SETTOOLINFOW,    "TTM_SETTOOLINFO",      MC_CONTROL, 0L )
    msgpick3( TTM_HITTESTW,        "TTM_HITTEST",          MC_CONTROL, 0L )
    msgpick3( TTM_GETTEXTW,        "TTM_GETTEXT",          MC_CONTROL, 0L )
    msgpick3( TTM_UPDATETIPTEXTW,  "TTM_UPDATETIPTEXT",    MC_CONTROL, 0L )
    msgpick3( TTM_ENUMTOOLSW,      "TTM_ENUMTOOLS",        MC_CONTROL, 0L )
    msgpick3( TTM_GETCURRENTTOOLW, "TTM_GETCURRENTTOOL",   MC_CONTROL, 0L )
};

#define TOOLTIPSMESSAGEARRAYSIZE    (sizeof( ToolTipsMessageArray )/sizeof( message ))

static message _NEAR StatusBarMessageArray[] = {
    msgpick3( SB_SETTEXTA,         "SB_SETTEXT",       MC_CONTROL, 0L )
    msgpick3( SB_GETTEXTA,         "SB_GETTEXT",       MC_CONTROL, 0L )
    msgpick3( SB_GETTEXTLENGTHA,   "SB_GETTEXTLENGTH", MC_CONTROL, 0L )
    msgpick3( SB_SETPARTS,         "SB_SETPARTS",      MC_CONTROL, 0L )
    msgpick3( SB_GETPARTS,         "SB_GETPARTS",      MC_CONTROL, 0L )
    msgpick3( SB_GETBORDERS,       "SB_GETBORDERS",    MC_CONTROL, 0L )
    msgpick3( SB_SETMINHEIGHT,     "SB_SETMINHEIGHT",  MC_CONTROL, 0L )
    msgpick3( SB_SIMPLE,           "SB_SIMPLE",        MC_CONTROL, 0L )
    msgpick3( SB_GETRECT,          "SB_GETRECT",       MC_CONTROL, 0L )
    msgpick3( SB_SETTEXTW,         "SB_SETTEXT",       MC_CONTROL, 0L )
    msgpick3( SB_GETTEXTLENGTHW,   "SB_GETTEXTLENGTH", MC_CONTROL, 0L )
    msgpick3( SB_GETTEXTW,         "SB_GETTEXT",       MC_CONTROL, 0L )
    msgpick3( SB_ISSIMPLE,         "SB_ISSIMPLE",      MC_CONTROL, 0L )
    msgpick3( SB_SETICON,          "SB_SETICON",       MC_CONTROL, 0L )
    msgpick3( SB_SETTIPTEXTA,      "SB_SETTIPTEXT",    MC_CONTROL, 0L )
    msgpick3( SB_SETTIPTEXTW,      "SB_SETTIPTEXT",    MC_CONTROL, 0L )
    msgpick3( SB_GETTIPTEXTA,      "SB_GETTIPTEXT",    MC_CONTROL, 0L )
    msgpick3( SB_GETTIPTEXTW,      "SB_GETTIPTEXT",    MC_CONTROL, 0L )
    msgpick3( SB_GETICON,          "SB_GETICON",       MC_CONTROL, 0L )
};

#define STATUSBARMESSAGEARRAYSIZE   (sizeof( StatusBarMessageArray )/sizeof( message ))

static message _NEAR TrackBarMessageArray[] = {
    msgpick3( TBM_GETPOS,          "TBM_GETPOS",           MC_CONTROL, 0L )
    msgpick3( TBM_GETRANGEMIN,     "TBM_GETRANGEMIN",      MC_CONTROL, 0L )
    msgpick3( TBM_GETRANGEMAX,     "TBM_GETRANGEMAX",      MC_CONTROL, 0L )
    msgpick3( TBM_GETTIC,          "TBM_GETTIC",           MC_CONTROL, 0L )
    msgpick3( TBM_SETTIC,          "TBM_SETTIC",           MC_CONTROL, 0L )
    msgpick3( TBM_SETPOS,          "TBM_SETPOS",           MC_CONTROL, 0L )
    msgpick3( TBM_SETRANGE,        "TBM_SETRANGE",         MC_CONTROL, 0L )
    msgpick3( TBM_SETRANGEMIN,     "TBM_SETRANGEMIN",      MC_CONTROL, 0L )
    msgpick3( TBM_SETRANGEMAX,     "TBM_SETRANGEMAX",      MC_CONTROL, 0L )
    msgpick3( TBM_CLEARTICS,       "TBM_CLEARTICS",        MC_CONTROL, 0L )
    msgpick3( TBM_SETSEL,          "TBM_SETSEL",           MC_CONTROL, 0L )
    msgpick3( TBM_SETSELSTART,     "TBM_SETSELSTART",      MC_CONTROL, 0L )
    msgpick3( TBM_SETSELEND,       "TBM_SETSELEND",        MC_CONTROL, 0L )
    msgpick3( TBM_GETPTICS,        "TBM_GETPTICS",         MC_CONTROL, 0L )
    msgpick3( TBM_GETTICPOS,       "TBM_GETTICPOS",        MC_CONTROL, 0L )
    msgpick3( TBM_GETNUMTICS,      "TBM_GETNUMTICS",       MC_CONTROL, 0L )
    msgpick3( TBM_GETSELSTART,     "TBM_GETSELSTART",      MC_CONTROL, 0L )
    msgpick3( TBM_GETSELEND,       "TBM_GETSELEND",        MC_CONTROL, 0L )
    msgpick3( TBM_CLEARSEL,        "TBM_CLEARSEL",         MC_CONTROL, 0L )
    msgpick3( TBM_SETTICFREQ,      "TBM_SETTICFREQ",       MC_CONTROL, 0L )
    msgpick3( TBM_SETPAGESIZE,     "TBM_SETPAGESIZE",      MC_CONTROL, 0L )
    msgpick3( TBM_GETPAGESIZE,     "TBM_GETPAGESIZE",      MC_CONTROL, 0L )
    msgpick3( TBM_SETLINESIZE,     "TBM_SETLINESIZE",      MC_CONTROL, 0L )
    msgpick3( TBM_GETLINESIZE,     "TBM_GETLINESIZE",      MC_CONTROL, 0L )
    msgpick3( TBM_GETTHUMBRECT,    "TBM_GETTHUMBRECT",     MC_CONTROL, 0L )
    msgpick3( TBM_GETCHANNELRECT,  "TBM_GETCHANNELRECT",   MC_CONTROL, 0L )
    msgpick3( TBM_SETTHUMBLENGTH,  "TBM_SETTHUMBLENGTH",   MC_CONTROL, 0L )
    msgpick3( TBM_GETTHUMBLENGTH,  "TBM_GETTHUMBLENGTH",   MC_CONTROL, 0L )
    msgpick3( TBM_SETTOOLTIPS,     "TBM_SETTOOLTIPS",      MC_CONTROL, 0L )
    msgpick3( TBM_GETTOOLTIPS,     "TBM_GETTOOLTIPS",      MC_CONTROL, 0L )
    msgpick3( TBM_SETTIPSIDE,      "TBM_SETTIPSIDE",       MC_CONTROL, 0L )
    msgpick3( TBM_SETBUDDY,        "TBM_SETBUDDY",         MC_CONTROL, 0L )
    msgpick3( TBM_GETBUDDY,        "TBM_GETBUDDY",         MC_CONTROL, 0L )
};

#define TRACKBARMESSAGEARRAYSIZE    (sizeof( TrackBarMessageArray )/sizeof( message ))

static message _NEAR UpDownMessageArray[] = {
    msgpick3( UDM_SETRANGE,    "UDM_SETRANGE",     MC_CONTROL, 0L )
    msgpick3( UDM_GETRANGE,    "UDM_GETRANGE",     MC_CONTROL, 0L )
    msgpick3( UDM_SETPOS,      "UDM_SETPOS",       MC_CONTROL, 0L )
    msgpick3( UDM_GETPOS,      "UDM_GETPOS",       MC_CONTROL, 0L )
    msgpick3( UDM_SETBUDDY,    "UDM_SETBUDDY",     MC_CONTROL, 0L )
    msgpick3( UDM_GETBUDDY,    "UDM_GETBUDDY",     MC_CONTROL, 0L )
    msgpick3( UDM_SETACCEL,    "UDM_SETACCEL",     MC_CONTROL, 0L )
    msgpick3( UDM_GETACCEL,    "UDM_GETACCEL",     MC_CONTROL, 0L )
    msgpick3( UDM_SETBASE,     "UDM_SETBASE",      MC_CONTROL, 0L )
    msgpick3( UDM_GETBASE,     "UDM_GETBASE",      MC_CONTROL, 0L )
    msgpick3( UDM_SETRANGE32,  "UDM_SETRANGE32",   MC_CONTROL, 0L )
    msgpick3( UDM_GETRANGE32,  "UDM_GETRANGE32",   MC_CONTROL, 0L )
    msgpick3( UDM_SETPOS32,    "UDM_SETPOS32",     MC_CONTROL, 0L )
    msgpick3( UDM_GETPOS32,    "UDM_GETPOS32",     MC_CONTROL, 0L )
};

#define UPDOWNMESSAGEARRAYSIZE      (sizeof( UpDownMessageArray )/sizeof( message ))

static message _NEAR ProgressBarMessageArray[] = {
    msgpick3( PBM_SETRANGE,    "PBM_SETRANGE",     MC_CONTROL, 0L )
    msgpick3( PBM_SETPOS,      "PBM_SETPOS",       MC_CONTROL, 0L )
    msgpick3( PBM_DELTAPOS,    "PBM_DELTAPOS",     MC_CONTROL, 0L )
    msgpick3( PBM_SETSTEP,     "PBM_SETSTEP",      MC_CONTROL, 0L )
    msgpick3( PBM_STEPIT,      "PBM_STEPIT",       MC_CONTROL, 0L )
    msgpick3( PBM_SETRANGE32,  "PBM_SETRANGE32",   MC_CONTROL, 0L )
    msgpick3( PBM_GETRANGE,    "PBM_GETRANGE",     MC_CONTROL, 0L )
    msgpick3( PBM_GETPOS,      "PBM_GETPOS",       MC_CONTROL, 0L )
    msgpick3( PBM_SETBARCOLOR, "PBM_SETBARCOLOR",  MC_CONTROL, 0L )
    msgpick3( PBM_SETMARQUEE,  "PBM_SETMARQUEE",   MC_CONTROL, 0L )
    msgpick3( PBM_GETSTEP,     "PBM_GETSTEP",      MC_CONTROL, 0L )
    msgpick3( PBM_GETBKCOLOR,  "PBM_GETBKCOLOR",   MC_CONTROL, 0L )
    msgpick3( PBM_GETBARCOLOR, "PBM_GETBARCOLOR",  MC_CONTROL, 0L )
    msgpick3( PBM_SETSTATE,    "PBM_SETSTATE",     MC_CONTROL, 0L )
    msgpick3( PBM_GETSTATE,    "PBM_GETSTATE",     MC_CONTROL, 0L )
};

#define PROGRESSBARMESSAGEARRAYSIZE (sizeof( ProgressBarMessageArray )/sizeof( message ))

static message _NEAR HotKeyMessageArray[] = {
    msgpick3( HKM_SETHOTKEY,   "HKM_SETHOTKEY",    MC_CONTROL, 0L )
    msgpick3( HKM_GETHOTKEY,   "HKM_GETHOTKEY",    MC_CONTROL, 0L )
    msgpick3( HKM_SETRULES,    "HKM_SETRULES",     MC_CONTROL, 0L )
};

#define HOTKEYMESSAGEARRAYSIZE      (sizeof( HotKeyMessageArray )/sizeof( message ))

static message _NEAR ListViewMessageArray[] = {
    msgpick3( LVM_GETBKCOLOR,          "LVM_GETBKCOLOR",           MC_CONTROL, 0L )
    msgpick3( LVM_SETBKCOLOR,          "LVM_SETBKCOLOR",           MC_CONTROL, 0L )
    msgpick3( LVM_GETIMAGELIST,        "LVM_GETIMAGELIST",         MC_CONTROL, 0L )
    msgpick3( LVM_SETIMAGELIST,        "LVM_SETIMAGELIST",         MC_CONTROL, 0L )
    msgpick3( LVM_GETITEMCOUNT,        "LVM_GETITEMCOUNT",         MC_CONTROL, 0L )
    msgpick3( LVM_GETITEMA,            "LVM_GETITEMA",             MC_CONTROL, 0L )
    msgpick3( LVM_SETITEMA,            "LVM_SETITEMA",             MC_CONTROL, 0L )
    msgpick3( LVM_INSERTITEMA,         "LVM_INSERTITEMA",          MC_CONTROL, 0L )
    msgpick3( LVM_DELETEITEM,          "LVM_DELETEITEM",           MC_CONTROL, 0L )
    msgpick3( LVM_DELETEALLITEMS,      "LVM_DELETEALLITEMS",       MC_CONTROL, 0L )
    msgpick3( LVM_GETCALLBACKMASK,     "LVM_GETCALLBACKMASK",      MC_CONTROL, 0L )
    msgpick3( LVM_SETCALLBACKMASK,     "LVM_SETCALLBACKMASK",      MC_CONTROL, 0L )
    msgpick3( LVM_GETNEXTITEM,         "LVM_GETNEXTITEM",          MC_CONTROL, 0L )
    msgpick3( LVM_FINDITEMA,           "LVM_FINDITEMA",            MC_CONTROL, 0L )
    msgpick3( LVM_GETITEMRECT,         "LVM_GETITEMRECT",          MC_CONTROL, 0L )
    msgpick3( LVM_SETITEMPOSITION,     "LVM_SETITEMPOSITION",      MC_CONTROL, 0L )
    msgpick3( LVM_GETITEMPOSITION,     "LVM_GETITEMPOSITION",      MC_CONTROL, 0L )
    msgpick3( LVM_GETSTRINGWIDTHA,     "LVM_GETSTRINGWIDTHA",      MC_CONTROL, 0L )
    msgpick3( LVM_HITTEST,             "LVM_HITTEST",              MC_CONTROL, 0L )
    msgpick3( LVM_ENSUREVISIBLE,       "LVM_ENSUREVISIBLE",        MC_CONTROL, 0L )
    msgpick3( LVM_SCROLL,              "LVM_SCROLL",               MC_CONTROL, 0L )
    msgpick3( LVM_REDRAWITEMS,         "LVM_REDRAWITEMS",          MC_CONTROL, 0L )
    msgpick3( LVM_ARRANGE,             "LVM_ARRANGE",              MC_CONTROL, 0L )
    msgpick3( LVM_EDITLABELA,          "LVM_EDITLABEL",            MC_CONTROL, 0L )
    msgpick3( LVM_GETEDITCONTROL,      "LVM_GETEDITCONTROL",       MC_CONTROL, 0L )
    msgpick3( LVM_GETCOLUMNA,          "LVM_GETCOLUMN",            MC_CONTROL, 0L )
    msgpick3( LVM_SETCOLUMNA,          "LVM_SETCOLUMN",            MC_CONTROL, 0L )
    msgpick3( LVM_INSERTCOLUMNA,       "LVM_INSERTCOLUMN",         MC_CONTROL, 0L )
    msgpick3( LVM_DELETECOLUMN,        "LVM_DELETECOLUMN",         MC_CONTROL, 0L )
    msgpick3( LVM_GETCOLUMNWIDTH,      "LVM_GETCOLUMNWIDTH",       MC_CONTROL, 0L )
    msgpick3( LVM_SETCOLUMNWIDTH,      "LVM_SETCOLUMNWIDTH",       MC_CONTROL, 0L )
    msgpick3( LVM_GETHEADER,           "LVM_GETHEADER",            MC_CONTROL, 0L )
    msgpick3( LVM_CREATEDRAGIMAGE,     "LVM_CREATEDRAGIMAGE",      MC_CONTROL, 0L )
    msgpick3( LVM_GETVIEWRECT,         "LVM_GETVIEWRECT",          MC_CONTROL, 0L )
    msgpick3( LVM_GETTEXTCOLOR,        "LVM_GETTEXTCOLOR",         MC_CONTROL, 0L )
    msgpick3( LVM_SETTEXTCOLOR,        "LVM_SETTEXTCOLOR",         MC_CONTROL, 0L )
    msgpick3( LVM_GETTEXTBKCOLOR,      "LVM_GETTEXTBKCOLOR",       MC_CONTROL, 0L )
    msgpick3( LVM_SETTEXTBKCOLOR,      "LVM_SETTEXTBKCOLOR",       MC_CONTROL, 0L )
    msgpick3( LVM_GETTOPINDEX,         "LVM_GETTOPINDEX",          MC_CONTROL, 0L )
    msgpick3( LVM_GETCOUNTPERPAGE,     "LVM_GETCOUNTPERPAGE",      MC_CONTROL, 0L )
    msgpick3( LVM_GETORIGIN,           "LVM_GETORIGIN",            MC_CONTROL, 0L )
    msgpick3( LVM_UPDATE,              "LVM_UPDATE",               MC_CONTROL, 0L )
    msgpick3( LVM_SETITEMSTATE,        "LVM_SETITEMSTATE",         MC_CONTROL, 0L )
    msgpick3( LVM_GETITEMSTATE,        "LVM_GETITEMSTATE",         MC_CONTROL, 0L )
    msgpick3( LVM_GETITEMTEXTA,        "LVM_GETITEMTEXT",          MC_CONTROL, 0L )
    msgpick3( LVM_SETITEMTEXTA,        "LVM_SETITEMTEXT",          MC_CONTROL, 0L )
    msgpick3( LVM_SETITEMCOUNT,        "LVM_SETITEMCOUNT",         MC_CONTROL, 0L )
    msgpick3( LVM_SORTITEMS,           "LVM_SORTITEMS",            MC_CONTROL, 0L )
    msgpick3( LVM_SETITEMPOSITION32,   "LVM_SETITEMPOSITION32",    MC_CONTROL, 0L )
    msgpick3( LVM_GETSELECTEDCOUNT,    "LVM_GETSELECTEDCOUNT",     MC_CONTROL, 0L )
    msgpick3( LVM_GETITEMSPACING,      "LVM_GETITEMSPACING",       MC_CONTROL, 0L )
    msgpick3( LVM_GETISEARCHSTRINGA,   "LVM_GETISEARCHSTRING",     MC_CONTROL, 0L )
    msgpick3( LVM_SETICONSPACING,      "LVM_SETICONSPACING",       MC_CONTROL, 0L )
    msgpick3( LVM_SETEXTENDEDLISTVIEWSTYLE,"LVM_SETEXTENDEDLISTVIEWSTYLE",MC_CONTROL, 0L )
    msgpick3( LVM_GETEXTENDEDLISTVIEWSTYLE,"LVM_GETEXTENDEDLISTVIEWSTYLE",MC_CONTROL, 0L )
    msgpick3( LVM_GETSUBITEMRECT,      "LVM_GETSUBITEMRECT",       MC_CONTROL, 0L )
    msgpick3( LVM_SUBITEMHITTEST,      "LVM_SUBITEMHITTEST",       MC_CONTROL, 0L )
    msgpick3( LVM_SETCOLUMNORDERARRAY, "LVM_SETCOLUMNORDERARRAY",  MC_CONTROL, 0L )
    msgpick3( LVM_GETCOLUMNORDERARRAY, "LVM_GETCOLUMNORDERARRAY",  MC_CONTROL, 0L )
    msgpick3( LVM_SETHOTITEM,          "LVM_SETHOTITEM",           MC_CONTROL, 0L )
    msgpick3( LVM_GETHOTITEM,          "LVM_GETHOTITEM",           MC_CONTROL, 0L )
    msgpick3( LVM_SETHOTCURSOR,        "LVM_SETHOTCURSOR",         MC_CONTROL, 0L )
    msgpick3( LVM_GETHOTCURSOR,        "LVM_GETHOTCURSOR",         MC_CONTROL, 0L )
    msgpick3( LVM_APPROXIMATEVIEWRECT, "LVM_APPROXIMATEVIEWRECT",  MC_CONTROL, 0L )
    msgpick3( LVM_SETWORKAREAS,        "LVM_SETWORKAREAS",         MC_CONTROL, 0L )
    msgpick3( LVM_GETSELECTIONMARK,    "LVM_GETSELECTIONMARK",     MC_CONTROL, 0L )
    msgpick3( LVM_SETSELECTIONMARK,    "LVM_SETSELECTIONMARK",     MC_CONTROL, 0L )
    msgpick3( LVM_SETBKIMAGEA,         "LVM_SETBKIMAGE",           MC_CONTROL, 0L )
    msgpick3( LVM_GETBKIMAGEA,         "LVM_GETBKIMAGE",           MC_CONTROL, 0L )
    msgpick3( LVM_GETWORKAREAS,        "LVM_GETWORKAREAS",         MC_CONTROL, 0L )
    msgpick3( LVM_SETHOVERTIME,        "LVM_SETHOVERTIME",         MC_CONTROL, 0L )
    msgpick3( LVM_GETHOVERTIME,        "LVM_GETHOVERTIME",         MC_CONTROL, 0L )
    msgpick3( LVM_GETNUMBEROFWORKAREAS,"LVM_GETNUMBEROFWORKAREAS", MC_CONTROL, 0L )
    msgpick3( LVM_SETTOOLTIPS,         "LVM_SETTOOLTIPS",          MC_CONTROL, 0L )
    msgpick3( LVM_GETITEMW,            "LVM_GETITEM",              MC_CONTROL, 0L )
    msgpick3( LVM_SETITEMW,            "LVM_SETITEM",              MC_CONTROL, 0L )
    msgpick3( LVM_INSERTITEMW,         "LVM_INSERTITEM",           MC_CONTROL, 0L )
    msgpick3( LVM_GETTOOLTIPS,         "LVM_GETTOOLTIPS",          MC_CONTROL, 0L )
    msgpick3( LVM_SORTITEMSEX,         "LVM_SORTITEMSEX",          MC_CONTROL, 0L )
    msgpick3( LVM_FINDITEMW,           "LVM_FINDITEM",             MC_CONTROL, 0L )
    msgpick3( LVM_GETSTRINGWIDTHW,     "LVM_GETSTRINGWIDTH",       MC_CONTROL, 0L )
    msgpick3( LVM_GETGROUPSTATE,       "LVM_GETGROUPSTATE",        MC_CONTROL, 0L )
    msgpick3( LVM_GETFOCUSEDGROUP,     "LVM_GETFOCUSEDGROUP",      MC_CONTROL, 0L )
    msgpick3( LVM_GETCOLUMNW,          "LVM_GETCOLUMN",            MC_CONTROL, 0L )
    msgpick3( LVM_SETCOLUMNW,          "LVM_SETCOLUMN",            MC_CONTROL, 0L )
    msgpick3( LVM_INSERTCOLUMNW,       "LVM_INSERTCOLUMN",         MC_CONTROL, 0L )
    msgpick3( LVM_GETGROUPRECT,        "LVM_GETGROUPRECT",         MC_CONTROL, 0L )
    msgpick3( LVM_GETITEMTEXTW,        "LVM_GETITEMTEXT",          MC_CONTROL, 0L )
    msgpick3( LVM_SETITEMTEXTW,        "LVM_SETITEMTEXT",          MC_CONTROL, 0L )
    msgpick3( LVM_GETISEARCHSTRINGW,   "LVM_GETISEARCHSTRING",     MC_CONTROL, 0L )
    msgpick3( LVM_EDITLABELW,          "LVM_EDITLABEL",            MC_CONTROL, 0L )
    msgpick3( LVM_SETBKIMAGEW,         "LVM_SETBKIMAGE",           MC_CONTROL, 0L )
    msgpick3( LVM_GETBKIMAGEW,         "LVM_GETBKIMAGE",           MC_CONTROL, 0L )
    msgpick3( LVM_SETSELECTEDCOLUMN,   "LVM_SETSELECTEDCOLUMN",    MC_CONTROL, 0L )
    msgpick3( LVM_SETVIEW,             "LVM_SETVIEW",              MC_CONTROL, 0L )
    msgpick3( LVM_GETVIEW,             "LVM_GETVIEW",              MC_CONTROL, 0L )
    msgpick3( LVM_INSERTGROUP,         "LVM_INSERTGROUP",          MC_CONTROL, 0L )
    msgpick3( LVM_SETGROUPINFO,        "LVM_SETGROUPINFO",         MC_CONTROL, 0L )
    msgpick3( LVM_GETGROUPINFO,        "LVM_GETGROUPINFO",         MC_CONTROL, 0L )
    msgpick3( LVM_REMOVEGROUP,         "LVM_REMOVEGROUP",          MC_CONTROL, 0L )
    msgpick3( LVM_MOVEGROUP,           "LVM_MOVEGROUP",            MC_CONTROL, 0L )
    msgpick3( LVM_GETGROUPCOUNT,       "LVM_GETGROUPCOUNT",        MC_CONTROL, 0L )
    msgpick3( LVM_GETGROUPINFOBYINDEX, "LVM_GETGROUPINFOBYINDEX",  MC_CONTROL, 0L )
    msgpick3( LVM_MOVEITEMTOGROUP,     "LVM_MOVEITEMTOGROUP",      MC_CONTROL, 0L )
    msgpick3( LVM_SETGROUPMETRICS,     "LVM_SETGROUPMETRICS",      MC_CONTROL, 0L )
    msgpick3( LVM_GETGROUPMETRICS,     "LVM_GETGROUPMETRICS",      MC_CONTROL, 0L )
    msgpick3( LVM_ENABLEGROUPVIEW,     "LVM_ENABLEGROUPVIEW",      MC_CONTROL, 0L )
    msgpick3( LVM_SORTGROUPS,          "LVM_SORTGROUPS",           MC_CONTROL, 0L )
    msgpick3( LVM_INSERTGROUPSORTED,   "LVM_INSERTGROUPSORTED",    MC_CONTROL, 0L )
    msgpick3( LVM_REMOVEALLGROUPS,     "LVM_REMOVEALLGROUPS",      MC_CONTROL, 0L )
    msgpick3( LVM_HASGROUP,            "LVM_HASGROUP",             MC_CONTROL, 0L )
    msgpick3( LVM_SETTILEVIEWINFO,     "LVM_SETTILEVIEWINFO",      MC_CONTROL, 0L )
    msgpick3( LVM_GETTILEVIEWINFO,     "LVM_GETTILEVIEWINFO",      MC_CONTROL, 0L )
    msgpick3( LVM_SETTILEINFO,         "LVM_SETTILEINFO",          MC_CONTROL, 0L )
    msgpick3( LVM_GETTILEINFO,         "LVM_GETTILEINFO",          MC_CONTROL, 0L )
    msgpick3( LVM_SETINSERTMARK,       "LVM_SETINSERTMARK",        MC_CONTROL, 0L )
    msgpick3( LVM_GETINSERTMARK,       "LVM_GETINSERTMARK",        MC_CONTROL, 0L )
    msgpick3( LVM_INSERTMARKHITTEST,   "LVM_INSERTMARKHITTEST",    MC_CONTROL, 0L )
    msgpick3( LVM_GETINSERTMARKRECT,   "LVM_GETINSERTMARKRECT",    MC_CONTROL, 0L )
    msgpick3( LVM_SETINSERTMARKCOLOR,  "LVM_SETINSERTMARKCOLOR",   MC_CONTROL, 0L )
    msgpick3( LVM_GETINSERTMARKCOLOR,  "LVM_GETINSERTMARKCOLOR",   MC_CONTROL, 0L )
    msgpick3( LVM_SETINFOTIP,          "LVM_SETINFOTIP",           MC_CONTROL, 0L )
    msgpick3( LVM_GETSELECTEDCOLUMN,   "LVM_GETSELECTEDCOLUMN",    MC_CONTROL, 0L )
    msgpick3( LVM_ISGROUPVIEWENABLED,  "LVM_ISGROUPVIEWENABLED",   MC_CONTROL, 0L )
    msgpick3( LVM_GETOUTLINECOLOR,     "LVM_GETOUTLINECOLOR",      MC_CONTROL, 0L )
    msgpick3( LVM_SETOUTLINECOLOR,     "LVM_SETOUTLINECOLOR",      MC_CONTROL, 0L )
    msgpick3( LVM_CANCELEDITLABEL,     "LVM_CANCELEDITLABEL",      MC_CONTROL, 0L )
    msgpick3( LVM_MAPINDEXTOID,        "LVM_MAPINDEXTOID",         MC_CONTROL, 0L )
    msgpick3( LVM_MAPIDTOINDEX,        "LVM_MAPIDTOINDEX",         MC_CONTROL, 0L )
    msgpick3( LVM_ISITEMVISIBLE,       "LVM_ISITEMVISIBLE",        MC_CONTROL, 0L )
    msgpick3( LVM_GETEMPTYTEXT,        "LVM_GETEMPTYTEXT",         MC_CONTROL, 0L )
    msgpick3( LVM_GETFOOTERRECT,       "LVM_GETFOOTERRECT",        MC_CONTROL, 0L )
    msgpick3( LVM_GETFOOTERINFO,       "LVM_GETFOOTERINFO",        MC_CONTROL, 0L )
    msgpick3( LVM_GETFOOTERITEMRECT,   "LVM_GETFOOTERITEMRECT",    MC_CONTROL, 0L )
    msgpick3( LVM_GETFOOTERITEM,       "LVM_GETFOOTERITEM",        MC_CONTROL, 0L )
    msgpick3( LVM_GETITEMINDEXRECT,    "LVM_GETITEMINDEXRECT",     MC_CONTROL, 0L )
    msgpick3( LVM_SETITEMINDEXSTATE,   "LVM_SETITEMINDEXSTATE",    MC_CONTROL, 0L )
    msgpick3( LVM_GETNEXTITEMINDEX,    "LVM_GETNEXTITEMINDEX",     MC_CONTROL, 0L )
};

#define LISTVIEWMESSAGEARRAYSIZE    (sizeof( ListViewMessageArray )/sizeof( message ))

static message _NEAR TreeViewMessageArray[] = {
    msgpick3( TVM_INSERTITEMA,         "TVM_INSERTITEM",           MC_CONTROL, 0L )
    msgpick3( TVM_DELETEITEM,          "TVM_DELETEITEM",           MC_CONTROL, 0L )
    msgpick3( TVM_EXPAND,              "TVM_EXPAND",               MC_CONTROL, 0L )
    msgpick3( TVM_GETITEMRECT,         "TVM_GETITEMRECT",          MC_CONTROL, 0L )
    msgpick3( TVM_GETCOUNT,            "TVM_GETCOUNT",             MC_CONTROL, 0L )
    msgpick3( TVM_GETINDENT,           "TVM_GETINDENT",            MC_CONTROL, 0L )
    msgpick3( TVM_SETINDENT,           "TVM_SETINDENT",            MC_CONTROL, 0L )
    msgpick3( TVM_GETIMAGELIST,        "TVM_GETIMAGELIST",         MC_CONTROL, 0L )
    msgpick3( TVM_SETIMAGELIST,        "TVM_SETIMAGELIST",         MC_CONTROL, 0L )
    msgpick3( TVM_GETNEXTITEM,         "TVM_GETNEXTITEM",          MC_CONTROL, 0L )
    msgpick3( TVM_SELECTITEM,          "TVM_SELECTITEM",           MC_CONTROL, 0L )
    msgpick3( TVM_GETITEMA,            "TVM_GETITEM",              MC_CONTROL, 0L )
    msgpick3( TVM_SETITEMA,            "TVM_SETITEM",              MC_CONTROL, 0L )
    msgpick3( TVM_EDITLABELA,          "TVM_EDITLABEL",            MC_CONTROL, 0L )
    msgpick3( TVM_GETEDITCONTROL,      "TVM_GETEDITCONTROL",       MC_CONTROL, 0L )
    msgpick3( TVM_GETVISIBLECOUNT,     "TVM_GETVISIBLECOUNT",      MC_CONTROL, 0L )
    msgpick3( TVM_HITTEST,             "TVM_HITTEST",              MC_CONTROL, 0L )
    msgpick3( TVM_CREATEDRAGIMAGE,     "TVM_CREATEDRAGIMAGE",      MC_CONTROL, 0L )
    msgpick3( TVM_SORTCHILDREN,        "TVM_SORTCHILDREN",         MC_CONTROL, 0L )
    msgpick3( TVM_ENSUREVISIBLE,       "TVM_ENSUREVISIBLE",        MC_CONTROL, 0L )
    msgpick3( TVM_SORTCHILDRENCB,      "TVM_SORTCHILDRENCB",       MC_CONTROL, 0L )
    msgpick3( TVM_ENDEDITLABELNOW,     "TVM_ENDEDITLABELNOW",      MC_CONTROL, 0L )
    msgpick3( TVM_GETISEARCHSTRINGA,   "TVM_GETISEARCHSTRING",     MC_CONTROL, 0L )
    msgpick3( TVM_SETTOOLTIPS,         "TVM_SETTOOLTIPS",          MC_CONTROL, 0L )
    msgpick3( TVM_GETTOOLTIPS,         "TVM_GETTOOLTIPS",          MC_CONTROL, 0L )
    msgpick3( TVM_SETINSERTMARK,       "TVM_SETINSERTMARK",        MC_CONTROL, 0L )
    msgpick3( TVM_SETITEMHEIGHT,       "TVM_SETITEMHEIGHT",        MC_CONTROL, 0L )
    msgpick3( TVM_GETITEMHEIGHT,       "TVM_GETITEMHEIGHT",        MC_CONTROL, 0L )
    msgpick3( TVM_SETBKCOLOR,          "TVM_SETBKCOLOR",           MC_CONTROL, 0L )
    msgpick3( TVM_SETTEXTCOLOR,        "TVM_SETTEXTCOLOR",         MC_CONTROL, 0L )
    msgpick3( TVM_GETBKCOLOR,          "TVM_GETBKCOLOR",           MC_CONTROL, 0L )
    msgpick3( TVM_GETTEXTCOLOR,        "TVM_GETTEXTCOLOR",         MC_CONTROL, 0L )
    msgpick3( TVM_SETSCROLLTIME,       "TVM_SETSCROLLTIME",        MC_CONTROL, 0L )
    msgpick3( TVM_GETSCROLLTIME,       "TVM_GETSCROLLTIME",        MC_CONTROL, 0L )
    msgpick3( TVM_SETINSERTMARKCOLOR,  "TVM_SETINSERTMARKCOLOR",   MC_CONTROL, 0L )
    msgpick3( TVM_GETINSERTMARKCOLOR,  "TVM_GETINSERTMARKCOLOR",   MC_CONTROL, 0L )
    msgpick3( TVM_GETITEMSTATE,        "TVM_GETITEMSTATE",         MC_CONTROL, 0L )
    msgpick3( TVM_SETLINECOLOR,        "TVM_SETLINECOLOR",         MC_CONTROL, 0L )
    msgpick3( TVM_GETLINECOLOR,        "TVM_GETLINECOLOR",         MC_CONTROL, 0L )
    msgpick3( TVM_MAPACCIDTOHTREEITEM, "TVM_MAPACCIDTOHTREEITEM",  MC_CONTROL, 0L )
    msgpick3( TVM_MAPHTREEITEMTOACCID, "TVM_MAPHTREEITEMTOACCID",  MC_CONTROL, 0L )
    msgpick3( TVM_SETEXTENDEDSTYLE,    "TVM_SETEXTENDEDSTYLE",     MC_CONTROL, 0L )
    msgpick3( TVM_GETEXTENDEDSTYLE,    "TVM_GETEXTENDEDSTYLE",     MC_CONTROL, 0L )
    msgpick3( TVM_INSERTITEMW,         "TVM_INSERTITEM",           MC_CONTROL, 0L )
    msgpick3( TVM_SETAUTOSCROLLINFO,   "TVM_SETAUTOSCROLLINFO",    MC_CONTROL, 0L )
    msgpick3( TVM_GETITEMW,            "TVM_GETITEM",              MC_CONTROL, 0L )
    msgpick3( TVM_SETITEMW,            "TVM_SETITEM",              MC_CONTROL, 0L )
    msgpick3( TVM_GETISEARCHSTRINGW,   "TVM_GETISEARCHSTRING",     MC_CONTROL, 0L )
    msgpick3( TVM_EDITLABELW,          "TVM_EDITLABEL",            MC_CONTROL, 0L )
    msgpick3( TVM_GETSELECTEDCOUNT,    "TVM_GETSELECTEDCOUNT",     MC_CONTROL, 0L )
    msgpick3( TVM_SHOWINFOTIP,         "TVM_SHOWINFOTIP",          MC_CONTROL, 0L )
    msgpick3( TVM_GETITEMPARTRECT,     "TVM_GETITEMPARTRECT",      MC_CONTROL, 0L )
};

#define TREEVIEWMESSAGEARRAYSIZE    (sizeof( TreeViewMessageArray )/sizeof( message ))

static message _NEAR ComboBoxExMessageArray[] = {
    combobox_messages_old()
    combobox_messages_new()
    msgpick3( CBEM_INSERTITEMA,        "CBEM_INSERTITEM",          MC_CONTROL, 0L )
    msgpick3( CBEM_SETIMAGELIST,       "CBEM_SETIMAGELIST",        MC_CONTROL, 0L )
    msgpick3( CBEM_GETIMAGELIST,       "CBEM_GETIMAGELIST",        MC_CONTROL, 0L )
    msgpick3( CBEM_GETITEMA,           "CBEM_GETITEM",             MC_CONTROL, 0L )
    msgpick3( CBEM_SETITEMA,           "CBEM_SETITEM",             MC_CONTROL, 0L )
    msgpick3( CBEM_GETCOMBOCONTROL,    "CBEM_GETCOMBOCONTROL",     MC_CONTROL, 0L )
    msgpick3( CBEM_GETEDITCONTROL,     "CBEM_GETEDITCONTROL",      MC_CONTROL, 0L )
    msgpick3( CBEM_SETEXSTYLE,         "CBEM_SETEXSTYLE",          MC_CONTROL, 0L )
    msgpick3( CBEM_GETEXSTYLE,         "CBEM_GETEXSTYLE",          MC_CONTROL, 0L )
    msgpick3( CBEM_HASEDITCHANGED,     "CBEM_HASEDITCHANGED",      MC_CONTROL, 0L )
    msgpick3( CBEM_INSERTITEMW,        "CBEM_INSERTITEM",          MC_CONTROL, 0L )
    msgpick3( CBEM_SETITEMW,           "CBEM_SETITEM",             MC_CONTROL, 0L )
    msgpick3( CBEM_GETITEMW,           "CBEM_GETITEM",             MC_CONTROL, 0L )
    msgpick3( CBEM_SETEXTENDEDSTYLE,   "CBEM_SETEXTENDEDSTYLE",    MC_CONTROL, 0L )
};

#define COMBOBOXEXMESSAGEARRAYSIZE  (sizeof( ComboBoxExMessageArray )/sizeof( message ))

static message _NEAR TabControlMessageArray[] = {
    msgpick3( TCM_GETIMAGELIST,        "TCM_GETIMAGELIST",     MC_CONTROL, 0L )
    msgpick3( TCM_SETIMAGELIST,        "TCM_SETIMAGELIST",     MC_CONTROL, 0L )
    msgpick3( TCM_GETITEMCOUNT,        "TCM_GETITEMCOUNT",     MC_CONTROL, 0L )
    msgpick3( TCM_GETITEMA,            "TCM_GETITEM",          MC_CONTROL, 0L )
    msgpick3( TCM_SETITEMA,            "TCM_SETITEM",          MC_CONTROL, 0L )
    msgpick3( TCM_INSERTITEMA,         "TCM_INSERTITEMA",      MC_CONTROL, 0L )
    msgpick3( TCM_DELETEITEM,          "TCM_DELETEITEM",       MC_CONTROL, 0L )
    msgpick3( TCM_DELETEALLITEMS,      "TCM_DELETEALLITEMS",   MC_CONTROL, 0L )
    msgpick3( TCM_GETITEMRECT,         "TCM_GETITEMRECT",      MC_CONTROL, 0L )
    msgpick3( TCM_GETCURSEL,           "TCM_GETCURSEL",        MC_CONTROL, 0L )
    msgpick3( TCM_SETCURSEL,           "TCM_SETCURSEL",        MC_CONTROL, 0L )
    msgpick3( TCM_HITTEST,             "TCM_HITTEST",          MC_CONTROL, 0L )
    msgpick3( TCM_SETITEMEXTRA,        "TCM_SETITEMEXTRA",     MC_CONTROL, 0L )
    msgpick3( TCM_ADJUSTRECT,          "TCM_ADJUSTRECT",       MC_CONTROL, 0L )
    msgpick3( TCM_SETITEMSIZE,         "TCM_SETITEMSIZE",      MC_CONTROL, 0L )
    msgpick3( TCM_REMOVEIMAGE,         "TCM_REMOVEIMAGE",      MC_CONTROL, 0L )
    msgpick3( TCM_SETPADDING,          "TCM_SETPADDING",       MC_CONTROL, 0L )
    msgpick3( TCM_GETROWCOUNT,         "TCM_GETROWCOUNT",      MC_CONTROL, 0L )
    msgpick3( TCM_GETTOOLTIPS,         "TCM_GETTOOLTIPS",      MC_CONTROL, 0L )
    msgpick3( TCM_SETTOOLTIPS,         "TCM_SETTOOLTIPS",      MC_CONTROL, 0L )
    msgpick3( TCM_GETCURFOCUS,         "TCM_GETCURFOCUS",      MC_CONTROL, 0L )
    msgpick3( TCM_SETCURFOCUS,         "TCM_SETCURFOCUS",      MC_CONTROL, 0L )
    msgpick3( TCM_SETMINTABWIDTH,      "TCM_SETMINTABWIDTH",   MC_CONTROL, 0L )
    msgpick3( TCM_DESELECTALL,         "TCM_DESELECTALL",      MC_CONTROL, 0L )
    msgpick3( TCM_HIGHLIGHTITEM,       "TCM_HIGHLIGHTITEM",    MC_CONTROL, 0L )
    msgpick3( TCM_SETEXTENDEDSTYLE,    "TCM_SETEXTENDEDSTYLE", MC_CONTROL, 0L )
    msgpick3( TCM_GETEXTENDEDSTYLE,    "TCM_GETEXTENDEDSTYLE", MC_CONTROL, 0L )
    msgpick3( TCM_GETITEMW,            "TCM_GETITEM",          MC_CONTROL, 0L )
    msgpick3( TCM_SETITEMW,            "TCM_SETITEM",          MC_CONTROL, 0L )
    msgpick3( TCM_INSERTITEMW,         "TCM_INSERTITEM",       MC_CONTROL, 0L )
};

#define TABCONTROLMESSAGEARRAYSIZE  (sizeof( TabControlMessageArray )/sizeof( message ))

static message _NEAR AnimateMessageArray[] = {
    msgpick3( ACM_OPENA,       "ACM_OPEN",         MC_CONTROL, 0L )
    msgpick3( ACM_PLAY,        "ACM_PLAY",         MC_CONTROL, 0L )
    msgpick3( ACM_STOP,        "ACM_STOP",         MC_CONTROL, 0L )
    msgpick3( ACM_OPENW,       "ACM_OPEN",         MC_CONTROL, 0L )
    msgpick3( ACM_ISPLAYING,   "ACM_ISPLAYING",    MC_CONTROL, 0L )
};

#define ANIMATEMESSAGEARRAYSIZE     (sizeof( AnimateMessageArray )/sizeof( message ))

static message _NEAR MonthCalMessageArray[] = {
    msgpick3( MCM_GETCURSEL,           "MCM_GETCURSEL",            MC_CONTROL, 0L )
    msgpick3( MCM_SETCURSEL,           "MCM_SETCURSEL",            MC_CONTROL, 0L )
    msgpick3( MCM_GETMAXSELCOUNT,      "MCM_GETMAXSELCOUNT",       MC_CONTROL, 0L )
    msgpick3( MCM_SETMAXSELCOUNT,      "MCM_SETMAXSELCOUNT",       MC_CONTROL, 0L )
    msgpick3( MCM_GETSELRANGE,         "MCM_GETSELRANGE",          MC_CONTROL, 0L )
    msgpick3( MCM_SETSELRANGE,         "MCM_SETSELRANGE",          MC_CONTROL, 0L )
    msgpick3( MCM_GETMONTHRANGE,       "MCM_GETMONTHRANGE",        MC_CONTROL, 0L )
    msgpick3( MCM_SETDAYSTATE,         "MCM_SETDAYSTATE",          MC_CONTROL, 0L )
    msgpick3( MCM_GETMINREQRECT,       "MCM_GETMINREQRECT",        MC_CONTROL, 0L )
    msgpick3( MCM_SETCOLOR,            "MCM_SETCOLOR",             MC_CONTROL, 0L )
    msgpick3( MCM_GETCOLOR,            "MCM_GETCOLOR",             MC_CONTROL, 0L )
    msgpick3( MCM_SETTODAY,            "MCM_SETTODAY",             MC_CONTROL, 0L )
    msgpick3( MCM_GETTODAY,            "MCM_GETTODAY",             MC_CONTROL, 0L )
    msgpick3( MCM_HITTEST,             "MCM_HITTEST",              MC_CONTROL, 0L )
    msgpick3( MCM_SETFIRSTDAYOFWEEK,   "MCM_SETFIRSTDAYOFWEEK",    MC_CONTROL, 0L )
    msgpick3( MCM_GETFIRSTDAYOFWEEK,   "MCM_GETFIRSTDAYOFWEEK",    MC_CONTROL, 0L )
    msgpick3( MCM_GETRANGE,            "MCM_GETRANGE",             MC_CONTROL, 0L )
    msgpick3( MCM_SETRANGE,            "MCM_SETRANGE",             MC_CONTROL, 0L )
    msgpick3( MCM_GETMONTHDELTA,       "MCM_GETMONTHDELTA",        MC_CONTROL, 0L )
    msgpick3( MCM_SETMONTHDELTA,       "MCM_SETMONTHDELTA",        MC_CONTROL, 0L )
    msgpick3( MCM_GETMAXTODAYWIDTH,    "MCM_GETMAXTODAYWIDTH",     MC_CONTROL, 0L )
    msgpick3( MCM_GETCURRENTVIEW,      "MCM_GETCURRENTVIEW",       MC_CONTROL, 0L )
    msgpick3( MCM_GETCALENDARCOUNT,    "MCM_GETCALENDARCOUNT",     MC_CONTROL, 0L )
    msgpick3( MCM_GETCALENDARGRIDINFO, "MCM_GETCALENDARGRIDINFO",  MC_CONTROL, 0L )
    msgpick3( MCM_GETCALID,            "MCM_GETCALID",             MC_CONTROL, 0L )
    msgpick3( MCM_SETCALID,            "MCM_SETCALID",             MC_CONTROL, 0L )
    msgpick3( MCM_SIZERECTTOMIN,       "MCM_SIZERECTTOMIN",        MC_CONTROL, 0L )
    msgpick3( MCM_SETCALENDARBORDER,   "MCM_SETCALENDARBORDER",    MC_CONTROL, 0L )
    msgpick3( MCM_GETCALENDARBORDER,   "MCM_GETCALENDARBORDER",    MC_CONTROL, 0L )
    msgpick3( MCM_SETCURRENTVIEW,      "MCM_SETCURRENTVIEW",       MC_CONTROL, 0L )
};

#define MONTHCALMESSAGEARRAYSIZE    (sizeof( MonthCalMessageArray )/sizeof( message ))

static message _NEAR DateTimeMessageArray[] = {
    msgpick3( DTM_GETSYSTEMTIME,   "DTM_GETSYSTEMTIME",    MC_CONTROL, 0L )
    msgpick3( DTM_SETSYSTEMTIME,   "DTM_SETSYSTEMTIME",    MC_CONTROL, 0L )
    msgpick3( DTM_GETRANGE,        "DTM_GETRANGE",         MC_CONTROL, 0L )
    msgpick3( DTM_SETRANGE,        "DTM_SETRANGE",         MC_CONTROL, 0L )
    msgpick3( DTM_SETFORMATA,      "DTM_SETFORMAT",        MC_CONTROL, 0L )
    msgpick3( DTM_SETMCCOLOR,      "DTM_SETMCCOLOR",       MC_CONTROL, 0L )
    msgpick3( DTM_GETMCCOLOR,      "DTM_GETMCCOLOR",       MC_CONTROL, 0L )
    msgpick3( DTM_GETMONTHCAL,     "DTM_GETMONTHCAL",      MC_CONTROL, 0L )
    msgpick3( DTM_SETMCFONT,       "DTM_SETMCFONT",        MC_CONTROL, 0L )
    msgpick3( DTM_GETMCFONT,       "DTM_GETMCFONT",        MC_CONTROL, 0L )
    msgpick3( DTM_SETMCSTYLE,      "DTM_SETMCSTYLE",       MC_CONTROL, 0L )
    msgpick3( DTM_GETMCSTYLE,      "DTM_GETMCSTYLE",       MC_CONTROL, 0L )
    msgpick3( DTM_CLOSEMONTHCAL,   "DTM_CLOSEMONTHCAL",    MC_CONTROL, 0L )
    msgpick3( DTM_GETDATETIMEPICKERINFO,"DTM_GETDATETIMEPICKERINFO",MC_CONTROL, 0L )
    msgpick3( DTM_GETIDEALSIZE,    "DTM_GETIDEALSIZE",     MC_CONTROL, 0L )
    msgpick3( DTM_SETFORMATW,      "DTM_SETFORMAT",        MC_CONTROL, 0L )
};

#define DATETIMEMESSAGEARRAYSIZE    (sizeof( DateTimeMessageArray )/sizeof( message ))

static message _NEAR ComboListBoxMessageArray[] = {
    msgpick3( LBCB_STARTTRACK,     "LBCB_STARTTRACK",      MC_CONTROL, 0L )
    msgpick3( LBCB_ENDTRACK,       "LBCB_ENDTRACK",        MC_CONTROL, 0L )
};

#define COMBOLISTBOXMESSAGEARRAYSIZE    (sizeof( ComboListBoxMessageArray )/sizeof( message ))

#endif

#undef msgpick1
#undef msgpick2
#undef msgpick3
#undef msgpick4

class_messages _NEAR ClassMessages[] = {
    { "",                   MessageArray,               MESSAGEARRAYSIZE               },
    { "edit",               EditMessageArray,           EDITMESSAGEARRAYSIZE           },
    { "button",             ButtonMessageArray,         BUTTONMESSAGEARRAYSIZE         },
    { "static",             StaticMessageArray,         STATICMESSAGEARRAYSIZE         },
    { "listbox",            ListBoxMessageArray,        LISTBOXMESSAGEARRAYSIZE        },
    { "combobox",           ComboBoxMessageArray,       COMBOBOXMESSAGEARRAYSIZE       },
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
    { "button",             ButtonStyleArray,       BUTTONSTYLEARRAYSIZE      },
    { "edit",               EditStyleArray,         EDITSTYLEARRAYSIZE        },
    { "static",             StaticStyleArray,       STATICSTYLEARRAYSIZE      },
    { "listbox",            ListBoxStyleArray,      LISTBOXSTYLEARRAYSIZE     },
    { "combobox",           ComboBoxStyleArray,     COMBOBOXSTYLEARRAYSIZE    },
    { "#32770",             DialogStyleArray,       DIALOGSTYLEARRAYSIZE      },
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
