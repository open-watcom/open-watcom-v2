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
* Description:  Message table for the spy.
*
****************************************************************************/


#include "spy.h"
#include <stdio.h>
#include <string.h>
#include <dde.h>

#ifdef __NT__
    #define NT_MSGS
#endif

#include "spyexmsg.h"

#define msgpick( a, b, c, d, e, f )     { a, b, c, d, e, f }

message _NEAR MessageArray[] =  {
    #include "spyarray.h"
};

#define MESSAGEARRAYSIZE            (sizeof( MessageArray )/sizeof( message ))

static message _NEAR EditMessageArray[] = {
    msgpick( 1, 0, EM_GETSEL,               "EM_GETSEL",                MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_SETSEL,               "EM_SETSEL",                MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_GETRECT,              "EM_GETRECT",               MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_SETRECT,              "EM_SETRECT",               MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_SETRECTNP,            "EM_SETRECTNP",             MC_CONTROL, 0L ),
#ifdef NT_MSGS
    msgpick( 1, 0, EM_SCROLL,               "EM_SCROLL",                MC_CONTROL, 0L ),
#endif
    msgpick( 1, 0, EM_LINESCROLL,           "EM_LINESCROLL",            MC_CONTROL, 0L ),
#ifdef NT_MSGS
    msgpick( 1, 0, EM_SCROLLCARET,          "EM_SCROLLCARET",           MC_CONTROL, 0L ),
#endif
    msgpick( 1, 0, EM_GETMODIFY,            "EM_GETMODIFY",             MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_SETMODIFY,            "EM_SETMODIFY",             MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_GETLINECOUNT,         "EM_GETLINECOUNT",          MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_LINEINDEX,            "EM_LINEINDEX",             MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_SETHANDLE,            "EM_SETHANDLE",             MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_GETHANDLE,            "EM_GETHANDLE",             MC_CONTROL, 0L ),
#ifdef NT_MSGS
    msgpick( 1, 0, EM_GETTHUMB,             "EM_GETTHUMB",              MC_CONTROL, 0L ),
#endif
    msgpick( 1, 0, EM_LINELENGTH,           "EM_LINELENGTH",            MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_REPLACESEL,           "EM_REPLACESEL",            MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_GETLINE,              "EM_GETLINE",               MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_LIMITTEXT,            "EM_LIMITTEXT",             MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_CANUNDO,              "EM_CANUNDO",               MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_UNDO,                 "EM_UNDO",                  MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_FMTLINES,             "EM_FMTLINES",              MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_LINEFROMCHAR,         "EM_LINEFROMCHAR",          MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_SETTABSTOPS,          "EM_SETTABSTOPS",           MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_SETPASSWORDCHAR,      "EM_SETPASSWORDCHAR",       MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_EMPTYUNDOBUFFER,      "EM_EMPTYUNDOBUFFER",       MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_GETFIRSTVISIBLELINE,  "EM_GETFIRSTVISIBLELINE",   MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_SETREADONLY,          "EM_SETREADONLY",           MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_SETWORDBREAKPROC,     "EM_SETWORDBREAKPROC",      MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_GETWORDBREAKPROC,     "EM_GETWORDBREAKPROC",      MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_GETPASSWORDCHAR,      "EM_GETPASSWORDCHAR",       MC_CONTROL, 0L ),
#ifdef NT_MSGS
    msgpick( 1, 0, EM_SETMARGINS,           "EM_SETMARGINS",            MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_GETMARGINS,           "EM_GETMARGINS",            MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_GETLIMITTEXT,         "EM_GETLIMITTEXT",          MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_POSFROMCHAR,          "EM_POSFROMCHAR",           MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_CHARFROMPOS,          "EM_CHARFROMPOS",           MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_SETIMESTATUS,         "EM_SETIMESTATUS",          MC_CONTROL, 0L ),
    msgpick( 1, 0, EM_GETIMESTATUS,         "EM_GETIMESTATUS",          MC_CONTROL, 0L )
#endif
};

#define EDITMESSAGEARRAYSIZE        (sizeof( EditMessageArray )/sizeof( message ))

static message _NEAR ButtonMessageArray[] = {
    msgpick( 1, 0, BM_GETCHECK,     "BM_GETCHECK",      MC_CONTROL, 0L ),
    msgpick( 1, 0, BM_SETCHECK,     "BM_SETCHECK",      MC_CONTROL, 0L ),
    msgpick( 1, 0, BM_GETSTATE,     "BM_GETSTATE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, BM_SETSTATE,     "BM_SETSTATE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, BM_SETSTYLE,     "BM_SETSTYLE",      MC_CONTROL, 0L ),
#ifdef NT_MSGS
    msgpick( 1, 0, BM_CLICK,        "BM_CLICK",         MC_CONTROL, 0L ),
    msgpick( 1, 0, BM_GETIMAGE,     "BM_GETIMAGE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, BM_SETIMAGE,     "BM_SETIMAGE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, BM_SETDONTCLICK, "BM_SETDONTCLICK",  MC_CONTROL, 0L ),
#endif
};

#define BUTTONMESSAGEARRAYSIZE      (sizeof( ButtonMessageArray )/sizeof( message ))

static message _NEAR StaticMessageArray[] = {
    msgpick( 1, 0, STM_SETICON,     "STM_SETICON",  MC_CONTROL, 0L ),
    msgpick( 1, 0, STM_GETICON,     "STM_GETICON",  MC_CONTROL, 0L ),
#ifdef NT_MSGS
    msgpick( 1, 0, STM_GETICON,     "STM_GETICON",  MC_CONTROL, 0L ),
    msgpick( 1, 0, STM_SETIMAGE,    "STM_SETIMAGE", MC_CONTROL, 0L ),
    msgpick( 1, 0, STM_GETIMAGE,    "STM_GETIMAGE", MC_CONTROL, 0L )
#endif
};

#define STATICMESSAGEARRAYSIZE      (sizeof( StaticMessageArray )/sizeof( message ))

static message _NEAR ListBoxMessageArray[] = {
    msgpick( 1, 0, LB_ADDSTRING,            "LB_ADDSTRING",             MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_INSERTSTRING,         "LB_INSERTSTRING",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_DELETESTRING,         "LB_DELETESTRING",          MC_CONTROL, 0L ),
#ifdef NT_MSGS
    msgpick( 1, 0, LB_SELITEMRANGEEX,       "LB_SELITEMRANGEEX",        MC_CONTROL, 0L ),
#endif
    msgpick( 1, 0, LB_RESETCONTENT,         "LB_RESETCONTENT",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_SETSEL,               "LB_SETSEL",                MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_SETCURSEL,            "LB_SETCURSEL",             MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_GETSEL,               "LB_GETSEL",                MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_GETCURSEL,            "LB_GETCURSEL",             MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_GETTEXT,              "LB_GETTEXT",               MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_GETTEXTLEN,           "LB_GETTEXTLEN",            MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_GETCOUNT,             "LB_GETCOUNT",              MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_SELECTSTRING,         "LB_SELECTSTRING",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_DIR,                  "LB_DIR",                   MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_GETTOPINDEX,          "LB_GETTOPINDEX",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_FINDSTRING,           "LB_FINDSTRING",            MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_GETSELCOUNT,          "LB_GETSELCOUNT",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_GETSELITEMS,          "LB_GETSELITEMS",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_SETTABSTOPS,          "LB_SETTABSTOPS",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_GETHORIZONTALEXTENT,  "LB_GETHORIZONTALEXTENT",   MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_SETHORIZONTALEXTENT,  "LB_SETHORIZONTALEXTENT",   MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_SETCOLUMNWIDTH,       "LB_SETCOLUMNWIDTH",        MC_CONTROL, 0L ),
#ifdef NT_MSGS
    msgpick( 1, 0, LB_ADDFILE,              "LB_ADDFILE",               MC_CONTROL, 0L ),
#endif
    msgpick( 1, 0, LB_SETTOPINDEX,          "LB_SETTOPINDEX",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_GETITEMRECT,          "LB_GETITEMRECT",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_GETITEMDATA,          "LB_GETITEMDATA",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_SETITEMDATA,          "LB_SETITEMDATA",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_SELITEMRANGE,         "LB_SELITEMRANGE",          MC_CONTROL, 0L ),
#ifdef NT_MSGS
    msgpick( 1, 0, LB_SETANCHORINDEX,       "LB_SETANCHORINDEX",        MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_GETANCHORINDEX,       "LB_GETANCHORINDEX",        MC_CONTROL, 0L ),
#endif
    msgpick( 1, 0, LB_SETCARETINDEX,        "LB_SETCARETINDEX",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_GETCARETINDEX,        "LB_GETCARETINDEX",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_SETITEMHEIGHT,        "LB_SETITEMHEIGHT",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_GETITEMHEIGHT,        "LB_GETITEMHEIGHT",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_FINDSTRINGEXACT,      "LB_FINDSTRINGEXACT",       MC_CONTROL, 0L ),
#ifdef NT_MSGS
    msgpick( 1, 0, LB_SETLOCALE,            "LB_SETLOCALE",             MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_GETLOCALE,            "LB_GETLOCALE",             MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_SETCOUNT,             "LB_SETCOUNT",              MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_INITSTORAGE,          "LB_INITSTORAGE",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_ITEMFROMPOINT,        "LB_ITEMFROMPOINT",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LB_GETLISTBOXINFO,       "LB_GETLISTBOXINFO",        MC_CONTROL, 0L )
#endif
};

#define LISTBOXMESSAGEARRAYSIZE     (sizeof( ListBoxMessageArray )/sizeof( message ))

#define combobox_messages_old() \
    msgpick( 1, 0, CB_GETEDITSEL,           "CB_GETEDITSEL",            MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_LIMITTEXT,            "CB_LIMITTEXT",             MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_SETEDITSEL,           "CB_SETEDITSEL",            MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_ADDSTRING,            "CB_ADDSTRING",             MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_DELETESTRING,         "CB_DELETESTRING",          MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_DIR,                  "CB_DIR",                   MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_GETCOUNT,             "CB_GETCOUNT",              MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_GETCURSEL,            "CB_GETCURSEL",             MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_GETLBTEXT,            "CB_GETLBTEXT",             MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_GETLBTEXTLEN,         "CB_GETLBTEXTLEN",          MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_INSERTSTRING,         "CB_INSERTSTRING",          MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_RESETCONTENT,         "CB_RESETCONTENT",          MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_FINDSTRING,           "CB_FINDSTRING",            MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_SELECTSTRING,         "CB_SELECTSTRING",          MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_SETCURSEL,            "CB_SETCURSEL",             MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_SHOWDROPDOWN,         "CB_SHOWDROPDOWN",          MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_GETITEMDATA,          "CB_GETITEMDATA",           MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_SETITEMDATA,          "CB_SETITEMDATA",           MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_GETDROPPEDCONTROLRECT,"CB_GETDROPPEDCONTROLRECT", MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_SETITEMHEIGHT,        "CB_SETITEMHEIGHT",         MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_GETITEMHEIGHT,        "CB_GETITEMHEIGHT",         MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_SETEXTENDEDUI,        "CB_SETEXTENDEDUI",         MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_GETEXTENDEDUI,        "CB_GETEXTENDEDUI",         MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_GETDROPPEDSTATE,      "CB_GETDROPPEDSTATE",       MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_FINDSTRINGEXACT,      "CB_FINDSTRINGEXACT",       MC_CONTROL, 0L ),
    
#define combobox_messages_new() \
    msgpick( 1, 0, CB_SETLOCALE,            "CB_SETLOCALE",             MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_GETLOCALE,            "CB_GETLOCALE",             MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_GETTOPINDEX,          "CB_GETTOPINDEX",           MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_SETTOPINDEX,          "CB_SETTOPINDEX",           MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_GETHORIZONTALEXTENT,  "CB_GETHORIZONTALEXTENT",   MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_SETHORIZONTALEXTENT,  "CB_SETHORIZONTALEXTENT",   MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_GETDROPPEDWIDTH,      "CB_GETDROPPEDWIDTH",       MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_SETDROPPEDWIDTH,      "CB_SETDROPPEDWIDTH",       MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_INITSTORAGE,          "CB_INITSTORAGE",           MC_CONTROL, 0L ), \
    msgpick( 1, 0, CB_GETCOMBOBOXINFO,      "CB_GETCOMBOBOXINFO",       MC_CONTROL, 0L ),

static message _NEAR ComboBoxMessageArray[] = {
    combobox_messages_old()
#ifdef NT_MSGS
    combobox_messages_new()
#endif
};

#define COMBOBOXMESSAGEARRAYSIZE    (sizeof( ComboBoxMessageArray )/sizeof( message ))

#ifdef NT_MSGS
static message _NEAR ScrollBarMessageArray[] = {
    msgpick( 1, 0, SBM_SETPOS,              "SBM_SETPOS",           MC_CONTROL, 0L ),
    msgpick( 1, 0, SBM_GETPOS,              "SBM_GETPOS",           MC_CONTROL, 0L ),
    msgpick( 1, 0, SBM_SETRANGE,            "SBM_SETRANGE",         MC_CONTROL, 0L ),
    msgpick( 1, 0, SBM_SETRANGEREDRAW,      "SBM_SETRANGEREDRAW",   MC_CONTROL, 0L ),
    msgpick( 1, 0, SBM_GETRANGE,            "SBM_GETRANGE",         MC_CONTROL, 0L ),
    msgpick( 1, 0, SBM_ENABLE_ARROWS,       "SBM_ENABLE_ARROWS",    MC_CONTROL, 0L ),
    msgpick( 1, 0, SBM_SETSCROLLINFO,       "SBM_SETSCROLLINFO",    MC_CONTROL, 0L ),
    msgpick( 1, 0, SBM_GETSCROLLINFO,       "SBM_GETSCROLLINFO",    MC_CONTROL, 0L ),
    msgpick( 1, 0, SBM_GETSCROLLBARINFO,    "SBM_GETSCROLLBARINFO", MC_CONTROL, 0L )
};

#define SCROLLBARMESSAGEARRAYSIZE   (sizeof( ScrollBarMessageArray )/sizeof( message ))

static message _NEAR HeaderMessageArray[] = {
    msgpick( 1, 0, HDM_GETITEMCOUNT,        "HDM_GETITEMCOUNT",         MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_INSERTITEMA,         "HDM_INSERTITEM",           MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_DELETEITEM,          "HDM_DELETEITEM",           MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_GETITEMA,            "HDM_GETITEM",              MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_SETITEMA,            "HDM_SETITEM",              MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_LAYOUT,              "HDM_LAYOUT",               MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_HITTEST,             "HDM_HITTEST",              MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_GETITEMRECT,         "HDM_GETITEMRECT",          MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_SETIMAGELIST,        "HDM_SETIMAGELIST",         MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_GETIMAGELIST,        "HDM_GETIMAGELIST",         MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_INSERTITEMW,         "HDM_INSERTITEM",           MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_GETITEMW,            "HDM_GETITEM",              MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_SETITEMW,            "HDM_SETITEM",              MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_ORDERTOINDEX,        "HDM_ORDERTOINDEX",         MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_CREATEDRAGIMAGE,     "HDM_CREATEDRAGIMAGE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_GETORDERARRAY,       "HDM_GETORDERARRAY",        MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_SETORDERARRAY,       "HDM_SETORDERARRAY",        MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_SETHOTDIVIDER,       "HDM_SETHOTDIVIDER",        MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_SETBITMAPMARGIN,     "HDM_SETBITMAPMARGIN",      MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_GETBITMAPMARGIN,     "HDM_GETBITMAPMARGIN",      MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_SETFILTERCHANGETIMEOUT,"HDM_SETFILTERCHANGETIMEOUT",MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_EDITFILTER,          "HDM_EDITFILTER",           MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_CLEARFILTER,         "HDM_CLEARFILTER",          MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_GETITEMDROPDOWNRECT, "HDM_GETITEMDROPDOWNRECT",  MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_GETOVERFLOWRECT,     "HDM_GETOVERFLOWRECT",      MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_GETFOCUSEDITEM,      "HDM_GETFOCUSEDITEM",       MC_CONTROL, 0L ),
    msgpick( 1, 0, HDM_SETFOCUSEDITEM,      "HDM_SETFOCUSEDITEM",       MC_CONTROL, 0L ),
};

#define HEADERMESSAGEARRAYSIZE      (sizeof( HeaderMessageArray )/sizeof( message ))

static message _NEAR ToolbarMessageArray[] = {
    msgpick( 1, 0, TB_ENABLEBUTTON,         "TB_ENABLEBUTTON",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_CHECKBUTTON,          "TB_CHECKBUTTON",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_PRESSBUTTON,          "TB_PRESSBUTTON",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_HIDEBUTTON,           "TB_HIDEBUTTON",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_INDETERMINATE,        "TB_INDETERMINATE",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_MARKBUTTON,           "TB_MARKBUTTON",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_ISBUTTONENABLED,      "TB_ISBUTTONENABLED",       MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_ISBUTTONCHECKED,      "TB_ISBUTTONCHECKED",       MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_ISBUTTONPRESSED,      "TB_ISBUTTONPRESSED",       MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_ISBUTTONHIDDEN,       "TB_ISBUTTONHIDEEN",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_ISBUTTONINDETERMINATE,"TB_ISBUTTONINDETERMINATE", MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_ISBUTTONHIGHLIGHTED,  "TB_ISBUTTONHIGHLIGHTED",   MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETSTATE,             "TB_SETSTATE",              MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETSTATE,             "TB_GETSTATE",              MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_ADDBITMAP,            "TB_ADDBITMAP",             MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_ADDBUTTONSA,          "TB_ADDBUTTONS",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_INSERTBUTTONA,        "TB_INSERTBUTTON",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_DELETEBUTTON,         "TB_DELETEBUTTON",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETBUTTON,            "TB_GETBUTTON",             MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_BUTTONCOUNT,          "TB_BUTTONCOUNT",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_COMMANDTOINDEX,       "TB_COMMANDTOINDEX",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SAVERESTOREA,         "TB_SAVERESTORE",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_CUSTOMIZE,            "TB_CUSTOMIZE",             MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_ADDSTRINGA,           "TB_ADDSTRING",             MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETITEMRECT,          "TB_GETITEMRECT",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_BUTTONSTRUCTSIZE,     "TB_BUTTONSTRUCTSIZE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETBUTTONSIZE,        "TB_SETBUTTONSIZE",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETBITMAPSIZE,        "TB_SETBITMAPSIZE",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_AUTOSIZE,             "TB_AUTOSIZE",              MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETTOOLTIPS,          "TB_GETTOOLTIPS",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETTOOLTIPS,          "TB_SETTOOLTIPS",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETPARENT,            "TB_SETPARENT",             MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETROWS,              "TB_SETROWS",               MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETROWS,              "TB_GETROWS",               MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETBITMAPFLAGS,       "TB_GETBITMAPFLAGS",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETCMDID,             "TB_SETCMDID",              MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_CHANGEBITMAP,         "TB_CHANGEBITMAP",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETBITMAP,            "TB_GETBITMAP",             MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETBUTTONTEXTA,       "TB_GETBUTTONTEXT",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_REPLACEBITMAP,        "TB_REPLACEBITMAP",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETINDENT,            "TB_SETINDENT",             MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETIMAGELIST,         "TB_SETIMAGELIST",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETIMAGELIST,         "TB_GETIMAGELIST",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_LOADIMAGES,           "TB_LOADIMAGES",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETRECT,              "TB_GETRECT",               MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETHOTIMAGELIST,      "TB_SETHOTIMAGELIST",       MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETHOTIMAGELIST,      "TB_GETHOTIMAGELIST",       MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETDISABLEDIMAGELIST, "TB_SETDISABLEDIMAGELIST",  MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETDISABLEDIMAGELIST, "TB_GETDISABLEDIMAGELIST",  MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETSTYLE,             "TB_SETSTYLE",              MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETSTYLE,             "TB_GETSTYLE",              MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETBUTTONSIZE,        "TB_GETBUTTONSIZE",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETBUTTONWIDTH,       "TB_SETBUTTONWIDTH",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETMAXTEXTROWS,       "TB_SETMAXTEXTROWS",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETTEXTROWS,          "TB_GETTEXTROWS",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETOBJECT,            "TB_GETOBJECT",             MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETBUTTONINFOW,       "TB_GETBUTTONINFO",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETBUTTONINFOW,       "TB_SETBUTTONINFO",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETBUTTONINFOA,       "TB_GETBUTTONINFO",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETBUTTONINFOA,       "TB_SETBUTTONINFO",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_INSERTBUTTONW,        "TB_INSERTBUTTON",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_ADDBUTTONSW,          "TB_ADDBUTTONS",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_HITTEST,              "TB_HITTEST",               MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETDRAWTEXTFLAGS,     "TB_SETDRAWTEXTFLAGS",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETHOTITEM,           "TB_SETHOTITEM",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETHOTITEM,           "TB_GETHOTITEM",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETANCHORHIGHLIGHT,   "TB_SETANCHORHIGHLIGHT",    MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETANCHORHIGHLIGHT,   "TB_GETANCHORHIGHLIGHT",    MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETBUTTONTEXTW,       "TB_GETBUTTONTEXT",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SAVERESTOREW,         "TB_SAVERESTORE",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_ADDSTRINGW,           "TB_ADDSTRING",             MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_MAPACCELERATORA,      "TB_MAPACCELERATOR",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETINSERTMARK,        "TB_GETINSERTMARK",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETINSERTMARK,        "TB_SETINSERTMARK",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_INSERTMARKHITTEST,    "TB_INSERTMARKHITTEST",     MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_MOVEBUTTON,           "TB_MOVEBUTTON",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETMAXSIZE,           "TB_GETMAXSIZE",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETEXTENDEDSTYLE,     "TB_SETEXTENDEDSTYLE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETEXTENDEDSTYLE,     "TB_GETEXTENDEDSTYLE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETPADDING,           "TB_GETPADDING",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETPADDING,           "TB_SETPADDING",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETINSERTMARKCOLOR,   "TB_SETINSERTMARKCOLOR",    MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETINSERTMARKCOLOR,   "TB_GETINSERTMARKCOLOR",    MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_MAPACCELERATORW,      "TB_MAPACCELERATOR",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETSTRINGW,           "TB_GETSTRING",             MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETSTRINGA,           "TB_GETSTRING",             MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETMETRICS,           "TB_GETMETRICS",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETMETRICS,           "TB_SETMETRICS",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_SETPRESSEDIMAGELIST,  "TB_SETPRESSEDIMAGELIST",   MC_CONTROL, 0L ),
    msgpick( 1, 0, TB_GETPRESSEDIMAGELIST,  "TB_GETPRESSEDIMAGELIST",   MC_CONTROL, 0L ),
};

#define TOOLBARMESSAGEARRAYSIZE     (sizeof( ToolbarMessageArray )/sizeof( message ))

static message _NEAR RebarMessageArray[] = {
    msgpick( 1, 0, RB_INSERTBANDA,      "RB_INSERTBAND",        MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_DELETEBAND,       "RB_DELETEBAND",        MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_GETBARINFO,       "RB_GETBARINFO",        MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_SETBARINFO,       "RB_SETBARINFO",        MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_GETBANDINFO_IE3,  "RB_GETBANDINFO",       MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_SETBANDINFOA,     "RB_SETBANDINFO",       MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_SETPARENT,        "RB_SETPARENT",         MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_HITTEST,          "RB_HITTEST",           MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_GETRECT,          "RB_GETRECT",           MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_INSERTBANDW,      "RB_INSERTBAND",        MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_SETBANDINFOW,     "RB_SETBANDINFO",       MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_GETBANDCOUNT,     "RB_GETBANDCOUNT",      MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_GETROWCOUNT,      "RB_GETROWCOUNT",       MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_GETROWHEIGHT,     "RB_GETROWHEIGHT",      MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_IDTOINDEX,        "RB_IDTOINDEX",         MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_GETTOOLTIPS,      "RB_GETTOOLTIPS",       MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_SETTOOLTIPS,      "RB_SETTOOLTIPS",       MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_SETBKCOLOR,       "RB_SETBKCOLOR",        MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_GETBKCOLOR,       "RB_GETBKCOLOR",        MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_SETTEXTCOLOR,     "RB_SETTEXTCOLOR",      MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_GETTEXTCOLOR,     "RB_GETTEXTCOLOR",      MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_SIZETORECT,       "RB_SIZETORECT",        MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_BEGINDRAG,        "RB_BEGINDRAG",         MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_ENDDRAG,          "RB_ENDDRAG",           MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_DRAGMOVE,         "RB_DRAGMOVE",          MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_GETBARHEIGHT,     "RB_GETBARHEIGHT",      MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_GETBANDINFOW,     "RB_GETBANDINFO",       MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_GETBANDINFOA,     "RB_GETBANDINFO",       MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_MINIMIZEBAND,     "RB_MINIMIZEBAND",      MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_MAXIMIZEBAND,     "RB_MAXIMIZEBAND",      MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_GETBANDBORDERS,   "RB_GETBANDBORDERS",    MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_SHOWBAND,         "RB_SHOWBAND",          MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_SETPALETTE,       "RB_SETPALETTE",        MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_GETPALETTE,       "RB_GETPALETTE",        MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_MOVEBAND,         "RB_MOVEBAND",          MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_GETBANDMARGINS,   "RB_GETBANDMARGINS",    MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_SETEXTENDEDSTYLE, "RB_SETEXTENDEDSTYLE",  MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_GETEXTENDEDSTYLE, "RB_GETEXTENDEDSTYLE",  MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_PUSHCHEVRON,      "RB_PUSHCHEVRON",       MC_CONTROL, 0L ),
    msgpick( 1, 0, RB_SETBANDWIDTH,     "RB_SETBANDWIDTH",      MC_CONTROL, 0L ),
};

#define REBARMESSAGEARRAYSIZE       (sizeof( RebarMessageArray )/sizeof( message ))

static message _NEAR ToolTipsMessageArray[] = {
    msgpick( 1, 0, TTM_ACTIVATE,        "TTM_ACTIVATE",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_SETDELAYTIME,    "TTM_SETDELAYTIME",     MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_ADDTOOLA,        "TTM_ADDTOOL",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_DELTOOLA,        "TTM_DELTOOL",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_NEWTOOLRECTA,    "TTM_NEWTOOLRECT",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_RELAYEVENT,      "TTM_RELAYEVENT",       MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_GETTOOLINFOA,    "TTM_GETTOOLINFO",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_SETTOOLINFOA,    "TTM_SETTOOLINFO",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_HITTESTA,        "TTM_HITTEST",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_GETTEXTA,        "TTM_GETTEXT",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_UPDATETIPTEXTA,  "TTM_UPDATETIPTEXT",    MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_GETTOOLCOUNT,    "TTM_GETTOOLCOUNT",     MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_ENUMTOOLSA,      "TTM_ENUMTOOLS",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_GETCURRENTTOOLA, "TTM_GETCURRENTTOOL",   MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_WINDOWFROMPOINT, "TTM_WINDOWFROMPOINT",  MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_TRACKACTIVATE,   "TTM_TRACKACTIVATE",    MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_TRACKPOSITION,   "TTM_TRACKPOSITION",    MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_SETTIPBKCOLOR,   "TTM_SETTIPBKCOLOR",    MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_SETTIPTEXTCOLOR, "TTM_SETTIPTEXTCOLOR",  MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_GETDELAYTIME,    "TTM_GETDELAYTIME",     MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_GETTIPBKCOLOR,   "TTM_GETTIPBKCOLOR",    MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_GETTIPTEXTCOLOR, "TTM_GETTIPTEXTCOLOR",  MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_SETMAXTIPWIDTH,  "TTM_SETMAXTIPWIDTH",   MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_GETMAXTIPWIDTH,  "TTM_GETMAXTIPWIDTH",   MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_SETMARGIN,       "TTM_SETMARGIN",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_GETMARGIN,       "TTM_GETMARGIN",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_POP,             "TTM_POP",              MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_UPDATE,          "TTM_UPDATE",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_GETBUBBLESIZE,   "TTM_GETBUBBLESIZE",    MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_ADJUSTRECT,      "TTM_ADJUSTRECT",       MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_SETTITLEA,       "TTM_SETTITLE",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_SETTITLEW,       "TTM_SETTITLE",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_POPUP,           "TTM_POPUP",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_GETTITLE,        "TTM_GETTITLE",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_ADDTOOLW,        "TTM_ADDTOOL",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_DELTOOLW,        "TTM_DELTOOL",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_NEWTOOLRECTW,    "TTM_NEWTOOLRECT",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_GETTOOLINFOW,    "TTM_GETTOOLINFO",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_SETTOOLINFOW,    "TTM_SETTOOLINFO",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_HITTESTW,        "TTM_HITTEST",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_GETTEXTW,        "TTM_GETTEXT",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_UPDATETIPTEXTW,  "TTM_UPDATETIPTEXT",    MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_ENUMTOOLSW,      "TTM_ENUMTOOLS",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TTM_GETCURRENTTOOLW, "TTM_GETCURRENTTOOL",   MC_CONTROL, 0L ),
};

#define TOOLTIPSMESSAGEARRAYSIZE    (sizeof( ToolTipsMessageArray )/sizeof( message ))

static message _NEAR StatusBarMessageArray[] = {
    msgpick( 1, 0, SB_SETTEXTA,         "SB_SETTEXT",       MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_GETTEXTA,         "SB_GETTEXT",       MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_GETTEXTLENGTHA,   "SB_GETTEXTLENGTH", MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_SETPARTS,         "SB_SETPARTS",      MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_GETPARTS,         "SB_GETPARTS",      MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_GETBORDERS,       "SB_GETBORDERS",    MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_SETMINHEIGHT,     "SB_SETMINHEIGHT",  MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_SIMPLE,           "SB_SIMPLE",        MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_GETRECT,          "SB_GETRECT",       MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_SETTEXTW,         "SB_SETTEXT",       MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_GETTEXTLENGTHW,   "SB_GETTEXTLENGTH", MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_GETTEXTW,         "SB_GETTEXT",       MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_ISSIMPLE,         "SB_ISSIMPLE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_SETICON,          "SB_SETICON",       MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_SETTIPTEXTA,      "SB_SETTIPTEXT",    MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_SETTIPTEXTW,      "SB_SETTIPTEXT",    MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_GETTIPTEXTA,      "SB_GETTIPTEXT",    MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_GETTIPTEXTW,      "SB_GETTIPTEXT",    MC_CONTROL, 0L ),
    msgpick( 1, 0, SB_GETICON,          "SB_GETICON",       MC_CONTROL, 0L ),
};

#define STATUSBARMESSAGEARRAYSIZE   (sizeof( StatusBarMessageArray )/sizeof( message ))

static message _NEAR TrackBarMessageArray[] = {
    msgpick( 1, 0, TBM_GETPOS,          "TBM_GETPOS",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_GETRANGEMIN,     "TBM_GETRANGEMIN",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_GETRANGEMAX,     "TBM_GETRANGEMAX",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_GETTIC,          "TBM_GETTIC",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_SETTIC,          "TBM_SETTIC",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_SETPOS,          "TBM_SETPOS",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_SETRANGE,        "TBM_SETRANGE",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_SETRANGEMIN,     "TBM_SETRANGEMIN",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_SETRANGEMAX,     "TBM_SETRANGEMAX",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_CLEARTICS,       "TBM_CLEARTICS",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_SETSEL,          "TBM_SETSEL",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_SETSELSTART,     "TBM_SETSELSTART",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_SETSELEND,       "TBM_SETSELEND",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_GETPTICS,        "TBM_GETPTICS",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_GETTICPOS,       "TBM_GETTICPOS",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_GETNUMTICS,      "TBM_GETNUMTICS",       MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_GETSELSTART,     "TBM_GETSELSTART",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_GETSELEND,       "TBM_GETSELEND",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_CLEARSEL,        "TBM_CLEARSEL",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_SETTICFREQ,      "TBM_SETTICFREQ",       MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_SETPAGESIZE,     "TBM_SETPAGESIZE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_GETPAGESIZE,     "TBM_GETPAGESIZE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_SETLINESIZE,     "TBM_SETLINESIZE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_GETLINESIZE,     "TBM_GETLINESIZE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_GETTHUMBRECT,    "TBM_GETTHUMBRECT",     MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_GETCHANNELRECT,  "TBM_GETCHANNELRECT",   MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_SETTHUMBLENGTH,  "TBM_SETTHUMBLENGTH",   MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_GETTHUMBLENGTH,  "TBM_GETTHUMBLENGTH",   MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_SETTOOLTIPS,     "TBM_SETTOOLTIPS",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_GETTOOLTIPS,     "TBM_GETTOOLTIPS",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_SETTIPSIDE,      "TBM_SETTIPSIDE",       MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_SETBUDDY,        "TBM_SETBUDDY",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TBM_GETBUDDY,        "TBM_GETBUDDY",         MC_CONTROL, 0L )
};

#define TRACKBARMESSAGEARRAYSIZE    (sizeof( TrackBarMessageArray )/sizeof( message ))

static message _NEAR UpDownMessageArray[] = {
    msgpick( 1, 0, UDM_SETRANGE,    "UDM_SETRANGE",     MC_CONTROL, 0L ),
    msgpick( 1, 0, UDM_GETRANGE,    "UDM_GETRANGE",     MC_CONTROL, 0L ),
    msgpick( 1, 0, UDM_SETPOS,      "UDM_SETPOS",       MC_CONTROL, 0L ),
    msgpick( 1, 0, UDM_GETPOS,      "UDM_GETPOS",       MC_CONTROL, 0L ),
    msgpick( 1, 0, UDM_SETBUDDY,    "UDM_SETBUDDY",     MC_CONTROL, 0L ),
    msgpick( 1, 0, UDM_GETBUDDY,    "UDM_GETBUDDY",     MC_CONTROL, 0L ),
    msgpick( 1, 0, UDM_SETACCEL,    "UDM_SETACCEL",     MC_CONTROL, 0L ),
    msgpick( 1, 0, UDM_GETACCEL,    "UDM_GETACCEL",     MC_CONTROL, 0L ),
    msgpick( 1, 0, UDM_SETBASE,     "UDM_SETBASE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, UDM_GETBASE,     "UDM_GETBASE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, UDM_SETRANGE32,  "UDM_SETRANGE32",   MC_CONTROL, 0L ),
    msgpick( 1, 0, UDM_GETRANGE32,  "UDM_GETRANGE32",   MC_CONTROL, 0L ),
    msgpick( 1, 0, UDM_SETPOS32,    "UDM_SETPOS32",     MC_CONTROL, 0L ),
    msgpick( 1, 0, UDM_GETPOS32,    "UDM_GETPOS32",     MC_CONTROL, 0L ),
};

#define UPDOWNMESSAGEARRAYSIZE      (sizeof( UpDownMessageArray )/sizeof( message ))

static message _NEAR ProgressBarMessageArray[] = {
    msgpick( 1, 0, PBM_SETRANGE,    "PBM_SETRANGE",     MC_CONTROL, 0L ),
    msgpick( 1, 0, PBM_SETPOS,      "PBM_SETPOS",       MC_CONTROL, 0L ),
    msgpick( 1, 0, PBM_DELTAPOS,    "PBM_DELTAPOS",     MC_CONTROL, 0L ),
    msgpick( 1, 0, PBM_SETSTEP,     "PBM_SETSTEP",      MC_CONTROL, 0L ),
    msgpick( 1, 0, PBM_STEPIT,      "PBM_STEPIT",       MC_CONTROL, 0L ),
    msgpick( 1, 0, PBM_SETRANGE32,  "PBM_SETRANGE32",   MC_CONTROL, 0L ),
    msgpick( 1, 0, PBM_GETRANGE,    "PBM_GETRANGE",     MC_CONTROL, 0L ),
    msgpick( 1, 0, PBM_GETPOS,      "PBM_GETPOS",       MC_CONTROL, 0L ),
    msgpick( 1, 0, PBM_SETBARCOLOR, "PBM_SETBARCOLOR",  MC_CONTROL, 0L ),
    msgpick( 1, 0, PBM_SETMARQUEE,  "PBM_SETMARQUEE",   MC_CONTROL, 0L ),
    msgpick( 1, 0, PBM_GETSTEP,     "PBM_GETSTEP",      MC_CONTROL, 0L ),
    msgpick( 1, 0, PBM_GETBKCOLOR,  "PBM_GETBKCOLOR",   MC_CONTROL, 0L ),
    msgpick( 1, 0, PBM_GETBARCOLOR, "PBM_GETBARCOLOR",  MC_CONTROL, 0L ),
    msgpick( 1, 0, PBM_SETSTATE,    "PBM_SETSTATE",     MC_CONTROL, 0L ),
    msgpick( 1, 0, PBM_GETSTATE,    "PBM_GETSTATE",     MC_CONTROL, 0L )
};

#define PROGRESSBARMESSAGEARRAYSIZE (sizeof( ProgressBarMessageArray )/sizeof( message ))

static message _NEAR HotKeyMessageArray[] = {
    msgpick( 1, 0, HKM_SETHOTKEY,   "HKM_SETHOTKEY",    MC_CONTROL, 0L ),
    msgpick( 1, 0, HKM_GETHOTKEY,   "HKM_GETHOTKEY",    MC_CONTROL, 0L ),
    msgpick( 1, 0, HKM_SETRULES,    "HKM_SETRULES",     MC_CONTROL, 0L )
};

#define HOTKEYMESSAGEARRAYSIZE      (sizeof( HotKeyMessageArray )/sizeof( message ))

static message _NEAR ListViewMessageArray[] = {
    msgpick( 1, 0, LVM_GETBKCOLOR,          "LVM_GETBKCOLOR",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETBKCOLOR,          "LVM_SETBKCOLOR",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETIMAGELIST,        "LVM_GETIMAGELIST",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETIMAGELIST,        "LVM_SETIMAGELIST",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETITEMCOUNT,        "LVM_GETITEMCOUNT",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETITEMA,            "LVM_GETITEMA",             MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETITEMA,            "LVM_SETITEMA",             MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_INSERTITEMA,         "LVM_INSERTITEMA",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_DELETEITEM,          "LVM_DELETEITEM",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_DELETEALLITEMS,      "LVM_DELETEALLITEMS",       MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETCALLBACKMASK,     "LVM_GETCALLBACKMASK",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETCALLBACKMASK,     "LVM_SETCALLBACKMASK",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETNEXTITEM,         "LVM_GETNEXTITEM",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_FINDITEMA,           "LVM_FINDITEMA",            MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETITEMRECT,         "LVM_GETITEMRECT",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETITEMPOSITION,     "LVM_SETITEMPOSITION",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETITEMPOSITION,     "LVM_GETITEMPOSITION",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETSTRINGWIDTHA,     "LVM_GETSTRINGWIDTHA",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_HITTEST,             "LVM_HITTEST",              MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_ENSUREVISIBLE,       "LVM_ENSUREVISIBLE",        MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SCROLL,              "LVM_SCROLL",               MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_REDRAWITEMS,         "LVM_REDRAWITEMS",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_ARRANGE,             "LVM_ARRANGE",              MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_EDITLABELA,          "LVM_EDITLABEL",            MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETEDITCONTROL,      "LVM_GETEDITCONTROL",       MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETCOLUMNA,          "LVM_GETCOLUMN",            MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETCOLUMNA,          "LVM_SETCOLUMN",            MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_INSERTCOLUMNA,       "LVM_INSERTCOLUMN",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_DELETECOLUMN,        "LVM_DELETECOLUMN",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETCOLUMNWIDTH,      "LVM_GETCOLUMNWIDTH",       MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETCOLUMNWIDTH,      "LVM_SETCOLUMNWIDTH",       MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETHEADER,           "LVM_GETHEADER",            MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_CREATEDRAGIMAGE,     "LVM_CREATEDRAGIMAGE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETVIEWRECT,         "LVM_GETVIEWRECT",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETTEXTCOLOR,        "LVM_GETTEXTCOLOR",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETTEXTCOLOR,        "LVM_SETTEXTCOLOR",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETTEXTBKCOLOR,      "LVM_GETTEXTBKCOLOR",       MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETTEXTBKCOLOR,      "LVM_SETTEXTBKCOLOR",       MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETTOPINDEX,         "LVM_GETTOPINDEX",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETCOUNTPERPAGE,     "LVM_GETCOUNTPERPAGE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETORIGIN,           "LVM_GETORIGIN",            MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_UPDATE,              "LVM_UPDATE",               MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETITEMSTATE,        "LVM_SETITEMSTATE",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETITEMSTATE,        "LVM_GETITEMSTATE",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETITEMTEXTA,        "LVM_GETITEMTEXT",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETITEMTEXTA,        "LVM_SETITEMTEXT",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETITEMCOUNT,        "LVM_SETITEMCOUNT",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SORTITEMS,           "LVM_SORTITEMS",            MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETITEMPOSITION32,   "LVM_SETITEMPOSITION32",    MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETSELECTEDCOUNT,    "LVM_GETSELECTEDCOUNT",     MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETITEMSPACING,      "LVM_GETITEMSPACING",       MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETISEARCHSTRINGA,   "LVM_GETISEARCHSTRING",     MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETICONSPACING,      "LVM_SETICONSPACING",       MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETEXTENDEDLISTVIEWSTYLE,"LVM_SETEXTENDEDLISTVIEWSTYLE",MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETEXTENDEDLISTVIEWSTYLE,"LVM_GETEXTENDEDLISTVIEWSTYLE",MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETSUBITEMRECT,      "LVM_GETSUBITEMRECT",       MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SUBITEMHITTEST,      "LVM_SUBITEMHITTEST",       MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETCOLUMNORDERARRAY, "LVM_SETCOLUMNORDERARRAY",  MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETCOLUMNORDERARRAY, "LVM_GETCOLUMNORDERARRAY",  MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETHOTITEM,          "LVM_SETHOTITEM",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETHOTITEM,          "LVM_GETHOTITEM",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETHOTCURSOR,        "LVM_SETHOTCURSOR",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETHOTCURSOR,        "LVM_GETHOTCURSOR",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_APPROXIMATEVIEWRECT, "LVM_APPROXIMATEVIEWRECT",  MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETWORKAREAS,        "LVM_SETWORKAREAS",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETSELECTIONMARK,    "LVM_GETSELECTIONMARK",     MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETSELECTIONMARK,    "LVM_SETSELECTIONMARK",     MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETBKIMAGEA,         "LVM_SETBKIMAGE",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETBKIMAGEA,         "LVM_GETBKIMAGE",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETWORKAREAS,        "LVM_GETWORKAREAS",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETHOVERTIME,        "LVM_SETHOVERTIME",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETHOVERTIME,        "LVM_GETHOVERTIME",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETNUMBEROFWORKAREAS,"LVM_GETNUMBEROFWORKAREAS", MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETTOOLTIPS,         "LVM_SETTOOLTIPS",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETITEMW,            "LVM_GETITEM",              MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETITEMW,            "LVM_SETITEM",              MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_INSERTITEMW,         "LVM_INSERTITEM",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETTOOLTIPS,         "LVM_GETTOOLTIPS",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SORTITEMSEX,         "LVM_SORTITEMSEX",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_FINDITEMW,           "LVM_FINDITEM",             MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETSTRINGWIDTHW,     "LVM_GETSTRINGWIDTH",       MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETGROUPSTATE,       "LVM_GETGROUPSTATE",        MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETFOCUSEDGROUP,     "LVM_GETFOCUSEDGROUP",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETCOLUMNW,          "LVM_GETCOLUMN",            MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETCOLUMNW,          "LVM_SETCOLUMN",            MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_INSERTCOLUMNW,       "LVM_INSERTCOLUMN",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETGROUPRECT,        "LVM_GETGROUPRECT",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETITEMTEXTW,        "LVM_GETITEMTEXT",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETITEMTEXTW,        "LVM_SETITEMTEXT",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETISEARCHSTRINGW,   "LVM_GETISEARCHSTRING",     MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_EDITLABELW,          "LVM_EDITLABEL",            MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETBKIMAGEW,         "LVM_SETBKIMAGE",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETBKIMAGEW,         "LVM_GETBKIMAGE",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETSELECTEDCOLUMN,   "LVM_SETSELECTEDCOLUMN",    MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETVIEW,             "LVM_SETVIEW",              MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETVIEW,             "LVM_GETVIEW",              MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_INSERTGROUP,         "LVM_INSERTGROUP",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETGROUPINFO,        "LVM_SETGROUPINFO",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETGROUPINFO,        "LVM_GETGROUPINFO",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_REMOVEGROUP,         "LVM_REMOVEGROUP",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_MOVEGROUP,           "LVM_MOVEGROUP",            MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETGROUPCOUNT,       "LVM_GETGROUPCOUNT",        MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETGROUPINFOBYINDEX, "LVM_GETGROUPINFOBYINDEX",  MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_MOVEITEMTOGROUP,     "LVM_MOVEITEMTOGROUP",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETGROUPMETRICS,     "LVM_SETGROUPMETRICS",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETGROUPMETRICS,     "LVM_GETGROUPMETRICS",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_ENABLEGROUPVIEW,     "LVM_ENABLEGROUPVIEW",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SORTGROUPS,          "LVM_SORTGROUPS",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_INSERTGROUPSORTED,   "LVM_INSERTGROUPSORTED",    MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_REMOVEALLGROUPS,     "LVM_REMOVEALLGROUPS",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_HASGROUP,            "LVM_HASGROUP",             MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETTILEVIEWINFO,     "LVM_SETTILEVIEWINFO",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETTILEVIEWINFO,     "LVM_GETTILEVIEWINFO",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETTILEINFO,         "LVM_SETTILEINFO",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETTILEINFO,         "LVM_GETTILEINFO",          MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETINSERTMARK,       "LVM_SETINSERTMARK",        MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETINSERTMARK,       "LVM_GETINSERTMARK",        MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_INSERTMARKHITTEST,   "LVM_INSERTMARKHITTEST",    MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETINSERTMARKRECT,   "LVM_GETINSERTMARKRECT",    MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETINSERTMARKCOLOR,  "LVM_SETINSERTMARKCOLOR",   MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETINSERTMARKCOLOR,  "LVM_GETINSERTMARKCOLOR",   MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETINFOTIP,          "LVM_SETINFOTIP",           MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETSELECTEDCOLUMN,   "LVM_GETSELECTEDCOLUMN",    MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_ISGROUPVIEWENABLED,  "LVM_ISGROUPVIEWENABLED",   MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETOUTLINECOLOR,     "LVM_GETOUTLINECOLOR",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETOUTLINECOLOR,     "LVM_SETOUTLINECOLOR",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_CANCELEDITLABEL,     "LVM_CANCELEDITLABEL",      MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_MAPINDEXTOID,        "LVM_MAPINDEXTOID",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_MAPIDTOINDEX,        "LVM_MAPIDTOINDEX",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_ISITEMVISIBLE,       "LVM_ISITEMVISIBLE",        MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETEMPTYTEXT,        "LVM_GETEMPTYTEXT",         MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETFOOTERRECT,       "LVM_GETFOOTERRECT",        MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETFOOTERINFO,       "LVM_GETFOOTERINFO",        MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETFOOTERITEMRECT,   "LVM_GETFOOTERITEMRECT",    MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETFOOTERITEM,       "LVM_GETFOOTERITEM",        MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETITEMINDEXRECT,    "LVM_GETITEMINDEXRECT",     MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_SETITEMINDEXSTATE,   "LVM_SETITEMINDEXSTATE",    MC_CONTROL, 0L ),
    msgpick( 1, 0, LVM_GETNEXTITEMINDEX,    "LVM_GETNEXTITEMINDEX",     MC_CONTROL, 0L ),
};

#define LISTVIEWMESSAGEARRAYSIZE    (sizeof( ListViewMessageArray )/sizeof( message ))

static message _NEAR TreeViewMessageArray[] = {
    msgpick( 1, 0, TVM_INSERTITEMA,         "TVM_INSERTITEM",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_DELETEITEM,          "TVM_DELETEITEM",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_EXPAND,              "TVM_EXPAND",               MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETITEMRECT,         "TVM_GETITEMRECT",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETCOUNT,            "TVM_GETCOUNT",             MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETINDENT,           "TVM_GETINDENT",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SETINDENT,           "TVM_SETINDENT",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETIMAGELIST,        "TVM_GETIMAGELIST",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SETIMAGELIST,        "TVM_SETIMAGELIST",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETNEXTITEM,         "TVM_GETNEXTITEM",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SELECTITEM,          "TVM_SELECTITEM",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETITEMA,            "TVM_GETITEM",              MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SETITEMA,            "TVM_SETITEM",              MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_EDITLABELA,          "TVM_EDITLABEL",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETEDITCONTROL,      "TVM_GETEDITCONTROL",       MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETVISIBLECOUNT,     "TVM_GETVISIBLECOUNT",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_HITTEST,             "TVM_HITTEST",              MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_CREATEDRAGIMAGE,     "TVM_CREATEDRAGIMAGE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SORTCHILDREN,        "TVM_SORTCHILDREN",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_ENSUREVISIBLE,       "TVM_ENSUREVISIBLE",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SORTCHILDRENCB,      "TVM_SORTCHILDRENCB",       MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_ENDEDITLABELNOW,     "TVM_ENDEDITLABELNOW",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETISEARCHSTRINGA,   "TVM_GETISEARCHSTRING",     MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SETTOOLTIPS,         "TVM_SETTOOLTIPS",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETTOOLTIPS,         "TVM_GETTOOLTIPS",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SETINSERTMARK,       "TVM_SETINSERTMARK",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SETITEMHEIGHT,       "TVM_SETITEMHEIGHT",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETITEMHEIGHT,       "TVM_GETITEMHEIGHT",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SETBKCOLOR,          "TVM_SETBKCOLOR",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SETTEXTCOLOR,        "TVM_SETTEXTCOLOR",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETBKCOLOR,          "TVM_GETBKCOLOR",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETTEXTCOLOR,        "TVM_GETTEXTCOLOR",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SETSCROLLTIME,       "TVM_SETSCROLLTIME",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETSCROLLTIME,       "TVM_GETSCROLLTIME",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SETINSERTMARKCOLOR,  "TVM_SETINSERTMARKCOLOR",   MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETINSERTMARKCOLOR,  "TVM_GETINSERTMARKCOLOR",   MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETITEMSTATE,        "TVM_GETITEMSTATE",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SETLINECOLOR,        "TVM_SETLINECOLOR",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETLINECOLOR,        "TVM_GETLINECOLOR",         MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_MAPACCIDTOHTREEITEM, "TVM_MAPACCIDTOHTREEITEM",  MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_MAPHTREEITEMTOACCID, "TVM_MAPHTREEITEMTOACCID",  MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SETEXTENDEDSTYLE,    "TVM_SETEXTENDEDSTYLE",     MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETEXTENDEDSTYLE,    "TVM_GETEXTENDEDSTYLE",     MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_INSERTITEMW,         "TVM_INSERTITEM",           MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SETAUTOSCROLLINFO,   "TVM_SETAUTOSCROLLINFO",    MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETITEMW,            "TVM_GETITEM",              MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SETITEMW,            "TVM_SETITEM",              MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETISEARCHSTRINGW,   "TVM_GETISEARCHSTRING",     MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_EDITLABELW,          "TVM_EDITLABEL",            MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETSELECTEDCOUNT,    "TVM_GETSELECTEDCOUNT",     MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_SHOWINFOTIP,         "TVM_SHOWINFOTIP",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TVM_GETITEMPARTRECT,     "TVM_GETITEMPARTRECT",      MC_CONTROL, 0L )
};

#define TREEVIEWMESSAGEARRAYSIZE    (sizeof( TreeViewMessageArray )/sizeof( message ))

static message _NEAR ComboBoxExMessageArray[] = {
    combobox_messages_old()
    combobox_messages_new()
    msgpick( 1, 0, CBEM_INSERTITEMA,        "CBEM_INSERTITEM",          MC_CONTROL, 0L ),
    msgpick( 1, 0, CBEM_SETIMAGELIST,       "CBEM_SETIMAGELIST",        MC_CONTROL, 0L ),
    msgpick( 1, 0, CBEM_GETIMAGELIST,       "CBEM_GETIMAGELIST",        MC_CONTROL, 0L ),
    msgpick( 1, 0, CBEM_GETITEMA,           "CBEM_GETITEM",             MC_CONTROL, 0L ),
    msgpick( 1, 0, CBEM_SETITEMA,           "CBEM_SETITEM",             MC_CONTROL, 0L ),
    msgpick( 1, 0, CBEM_GETCOMBOCONTROL,    "CBEM_GETCOMBOCONTROL",     MC_CONTROL, 0L ),
    msgpick( 1, 0, CBEM_GETEDITCONTROL,     "CBEM_GETEDITCONTROL",      MC_CONTROL, 0L ),
    msgpick( 1, 0, CBEM_SETEXSTYLE,         "CBEM_SETEXSTYLE",          MC_CONTROL, 0L ),
    msgpick( 1, 0, CBEM_GETEXSTYLE,         "CBEM_GETEXSTYLE",          MC_CONTROL, 0L ),
    msgpick( 1, 0, CBEM_HASEDITCHANGED,     "CBEM_HASEDITCHANGED",      MC_CONTROL, 0L ),
    msgpick( 1, 0, CBEM_INSERTITEMW,        "CBEM_INSERTITEM",          MC_CONTROL, 0L ),
    msgpick( 1, 0, CBEM_SETITEMW,           "CBEM_SETITEM",             MC_CONTROL, 0L ),
    msgpick( 1, 0, CBEM_GETITEMW,           "CBEM_GETITEM",             MC_CONTROL, 0L ),
    msgpick( 1, 0, CBEM_SETEXTENDEDSTYLE,   "CBEM_SETEXTENDEDSTYLE",    MC_CONTROL, 0L )
};

#define COMBOBOXEXMESSAGEARRAYSIZE  (sizeof( ComboBoxExMessageArray )/sizeof( message ))

static message _NEAR TabControlMessageArray[] = {
    msgpick( 1, 0, TCM_GETIMAGELIST,        "TCM_GETIMAGELIST",     MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_SETIMAGELIST,        "TCM_SETIMAGELIST",     MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_GETITEMCOUNT,        "TCM_GETITEMCOUNT",     MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_GETITEMA,            "TCM_GETITEM",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_SETITEMA,            "TCM_SETITEM",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_INSERTITEMA,         "TCM_INSERTITEMA",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_DELETEITEM,          "TCM_DELETEITEM",       MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_DELETEALLITEMS,      "TCM_DELETEALLITEMS",   MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_GETITEMRECT,         "TCM_GETITEMRECT",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_GETCURSEL,           "TCM_GETCURSEL",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_SETCURSEL,           "TCM_SETCURSEL",        MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_HITTEST,             "TCM_HITTEST",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_SETITEMEXTRA,        "TCM_SETITEMEXTRA",     MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_ADJUSTRECT,          "TCM_ADJUSTRECT",       MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_SETITEMSIZE,         "TCM_SETITEMSIZE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_REMOVEIMAGE,         "TCM_REMOVEIMAGE",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_SETPADDING,          "TCM_SETPADDING",       MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_GETROWCOUNT,         "TCM_GETROWCOUNT",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_GETTOOLTIPS,         "TCM_GETTOOLTIPS",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_SETTOOLTIPS,         "TCM_SETTOOLTIPS",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_GETCURFOCUS,         "TCM_GETCURFOCUS",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_SETCURFOCUS,         "TCM_SETCURFOCUS",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_SETMINTABWIDTH,      "TCM_SETMINTABWIDTH",   MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_DESELECTALL,         "TCM_DESELECTALL",      MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_HIGHLIGHTITEM,       "TCM_HIGHLIGHTITEM",    MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_SETEXTENDEDSTYLE,    "TCM_SETEXTENDEDSTYLE", MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_GETEXTENDEDSTYLE,    "TCM_GETEXTENDEDSTYLE", MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_GETITEMW,            "TCM_GETITEM",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_SETITEMW,            "TCM_SETITEM",          MC_CONTROL, 0L ),
    msgpick( 1, 0, TCM_INSERTITEMW,         "TCM_INSERTITEM",       MC_CONTROL, 0L )
};

#define TABCONTROLMESSAGEARRAYSIZE  (sizeof( TabControlMessageArray )/sizeof( message ))

static message _NEAR AnimateMessageArray[] = {
    msgpick( 1, 0, ACM_OPENA,       "ACM_OPEN",         MC_CONTROL, 0L ),
    msgpick( 1, 0, ACM_PLAY,        "ACM_PLAY",         MC_CONTROL, 0L ),
    msgpick( 1, 0, ACM_STOP,        "ACM_STOP",         MC_CONTROL, 0L ),
    msgpick( 1, 0, ACM_OPENW,       "ACM_OPEN",         MC_CONTROL, 0L ),
    msgpick( 1, 0, ACM_ISPLAYING,   "ACM_ISPLAYING",    MC_CONTROL, 0L )
};

#define ANIMATEMESSAGEARRAYSIZE     (sizeof( AnimateMessageArray )/sizeof( message ))

static message _NEAR MonthCalMessageArray[] = {
    msgpick( 1, 0, MCM_GETCURSEL,           "MCM_GETCURSEL",            MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_SETCURSEL,           "MCM_SETCURSEL",            MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_GETMAXSELCOUNT,      "MCM_GETMAXSELCOUNT",       MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_SETMAXSELCOUNT,      "MCM_SETMAXSELCOUNT",       MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_GETSELRANGE,         "MCM_GETSELRANGE",          MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_SETSELRANGE,         "MCM_SETSELRANGE",          MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_GETMONTHRANGE,       "MCM_GETMONTHRANGE",        MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_SETDAYSTATE,         "MCM_SETDAYSTATE",          MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_GETMINREQRECT,       "MCM_GETMINREQRECT",        MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_SETCOLOR,            "MCM_SETCOLOR",             MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_GETCOLOR,            "MCM_GETCOLOR",             MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_SETTODAY,            "MCM_SETTODAY",             MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_GETTODAY,            "MCM_GETTODAY",             MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_HITTEST,             "MCM_HITTEST",              MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_SETFIRSTDAYOFWEEK,   "MCM_SETFIRSTDAYOFWEEK",    MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_GETFIRSTDAYOFWEEK,   "MCM_GETFIRSTDAYOFWEEK",    MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_GETRANGE,            "MCM_GETRANGE",             MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_SETRANGE,            "MCM_SETRANGE",             MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_GETMONTHDELTA,       "MCM_GETMONTHDELTA",        MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_SETMONTHDELTA,       "MCM_SETMONTHDELTA",        MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_GETMAXTODAYWIDTH,    "MCM_GETMAXTODAYWIDTH",     MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_GETCURRENTVIEW,      "MCM_GETCURRENTVIEW",       MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_GETCALENDARCOUNT,    "MCM_GETCALENDARCOUNT",     MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_GETCALENDARGRIDINFO, "MCM_GETCALENDARGRIDINFO",  MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_GETCALID,            "MCM_GETCALID",             MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_SETCALID,            "MCM_SETCALID",             MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_SIZERECTTOMIN,       "MCM_SIZERECTTOMIN",        MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_SETCALENDARBORDER,   "MCM_SETCALENDARBORDER",    MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_GETCALENDARBORDER,   "MCM_GETCALENDARBORDER",    MC_CONTROL, 0L ),
    msgpick( 1, 0, MCM_SETCURRENTVIEW,      "MCM_SETCURRENTVIEW",       MC_CONTROL, 0L )
};

#define MONTHCALMESSAGEARRAYSIZE    (sizeof( MonthCalMessageArray )/sizeof( message ))

static message _NEAR DateTimeMessageArray[] = {
    msgpick( 1, 0, DTM_GETSYSTEMTIME,   "DTM_GETSYSTEMTIME",    MC_CONTROL, 0L ),
    msgpick( 1, 0, DTM_SETSYSTEMTIME,   "DTM_SETSYSTEMTIME",    MC_CONTROL, 0L ),
    msgpick( 1, 0, DTM_GETRANGE,        "DTM_GETRANGE",         MC_CONTROL, 0L ),
    msgpick( 1, 0, DTM_SETRANGE,        "DTM_SETRANGE",         MC_CONTROL, 0L ),
    msgpick( 1, 0, DTM_SETFORMATA,      "DTM_SETFORMAT",        MC_CONTROL, 0L ),
    msgpick( 1, 0, DTM_SETMCCOLOR,      "DTM_SETMCCOLOR",       MC_CONTROL, 0L ),
    msgpick( 1, 0, DTM_GETMCCOLOR,      "DTM_GETMCCOLOR",       MC_CONTROL, 0L ),
    msgpick( 1, 0, DTM_GETMONTHCAL,     "DTM_GETMONTHCAL",      MC_CONTROL, 0L ),
    msgpick( 1, 0, DTM_SETMCFONT,       "DTM_SETMCFONT",        MC_CONTROL, 0L ),
    msgpick( 1, 0, DTM_GETMCFONT,       "DTM_GETMCFONT",        MC_CONTROL, 0L ),
    msgpick( 1, 0, DTM_SETMCSTYLE,      "DTM_SETMCSTYLE",       MC_CONTROL, 0L ),
    msgpick( 1, 0, DTM_GETMCSTYLE,      "DTM_GETMCSTYLE",       MC_CONTROL, 0L ),
    msgpick( 1, 0, DTM_CLOSEMONTHCAL,   "DTM_CLOSEMONTHCAL",    MC_CONTROL, 0L ),
    msgpick( 1, 0, DTM_GETDATETIMEPICKERINFO,"DTM_GETDATETIMEPICKERINFO",MC_CONTROL, 0L ),
    msgpick( 1, 0, DTM_GETIDEALSIZE,    "DTM_GETIDEALSIZE",     MC_CONTROL, 0L ),
    msgpick( 1, 0, DTM_SETFORMATW,      "DTM_SETFORMAT",        MC_CONTROL, 0L ),
};

#define DATETIMEMESSAGEARRAYSIZE    (sizeof( DateTimeMessageArray )/sizeof( message ))

#endif

class_messages _NEAR ClassMessages[] = {
    { "",                   MessageArray,               MESSAGEARRAYSIZE               },
    { "edit",               EditMessageArray,           EDITMESSAGEARRAYSIZE           },
    { "button",             ButtonMessageArray,         BUTTONMESSAGEARRAYSIZE         },
    { "static",             StaticMessageArray,         STATICMESSAGEARRAYSIZE         },
    { "listbox",            ListBoxMessageArray,        LISTBOXMESSAGEARRAYSIZE        },
    { "combobox",           ComboBoxMessageArray,       COMBOBOXMESSAGEARRAYSIZE       },
#ifdef NT_MSGS
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
    { DATETIMEPICK_CLASS,   DateTimeMessageArray,       DATETIMEMESSAGEARRAYSIZE       }
#endif
};

WORD ClassMessagesSize = sizeof( ClassMessages ) / sizeof( class_messages );

#ifndef NT_MSGS
unsigned TotalMessageArraySize = \
    + MESSAGEARRAYSIZE \
    + EDITMESSAGEARRAYSIZE \
    + BUTTONMESSAGEARRAYSIZE \
    + STATICMESSAGEARRAYSIZE \
    + LISTBOXMESSAGEARRAYSIZE \
    + COMBOBOXMESSAGEARRAYSIZE;
#else
unsigned TotalMessageArraySize = \
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

