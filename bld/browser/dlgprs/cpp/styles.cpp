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


#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <windows.h>

#include "styles.h"

struct WinStyleStruct {
    const char *    name;
    uint_32         flag;
} WindowsStyles [] = {
    { "BS_3STATE",              BS_3STATE },
    { "BS_AUTO3STATE",          BS_AUTO3STATE },
    { "BS_AUTOCHECKBOX",        BS_AUTOCHECKBOX },
    { "BS_AUTORADIOBUTTON",     BS_AUTORADIOBUTTON },
    { "BS_CHECKBOX",            BS_CHECKBOX },
    { "BS_DEFPUSHBUTTON",       BS_DEFPUSHBUTTON },
    { "BS_GROUPBOX",            BS_GROUPBOX },
    { "BS_LEFTTEXT",            BS_LEFTTEXT },
    { "BS_OWNERDRAW",           BS_OWNERDRAW },
    { "BS_PUSHBUTTON",          BS_PUSHBUTTON },
    { "BS_RADIOBUTTON",         BS_RADIOBUTTON },
    { "BS_USERBUTTON",          BS_USERBUTTON },
    { "CBS_AUTOHSCROLL",        CBS_AUTOHSCROLL },
    { "CBS_DISABLENOSCROLL",    CBS_DISABLENOSCROLL },
    { "CBS_DROPDOWN",           CBS_DROPDOWN },
    { "CBS_DROPDOWNLIST",       CBS_DROPDOWNLIST },
    { "CBS_HASSTRINGS",         CBS_HASSTRINGS },
    { "CBS_NOINTEGRALHEIGHT",   CBS_NOINTEGRALHEIGHT },
    { "CBS_OEMCONVERT",         CBS_OEMCONVERT },
    { "CBS_OWNERDRAWFIXED",     CBS_OWNERDRAWFIXED },
    { "CBS_OWNERDRAWVARIABLE",  CBS_OWNERDRAWVARIABLE },
    { "CBS_SIMPLE",             CBS_SIMPLE },
    { "CBS_SORT",               CBS_SORT },
    { "DS_ABSALIGN",            DS_ABSALIGN },
    { "DS_LOCALEDIT",           DS_LOCALEDIT },
    { "DS_MODALFRAME",          DS_MODALFRAME },
    { "DS_NOIDLEMSG",           DS_NOIDLEMSG },
    { "DS_SETFONT",             DS_SETFONT },
    { "DS_SYSMODAL",            DS_SYSMODAL },
    { "ES_AUTOHSCROLL",         ES_AUTOHSCROLL },
    { "ES_AUTOVSCROLL",         ES_AUTOVSCROLL },
    { "ES_CENTER",              ES_CENTER },
    { "ES_LEFT",                ES_LEFT },
    { "ES_LOWERCASE",           ES_LOWERCASE },
    { "ES_MULTILINE",           ES_MULTILINE },
    { "ES_NOHIDESEL",           ES_NOHIDESEL },
    { "ES_OEMCONVERT",          ES_OEMCONVERT },
    { "ES_PASSWORD",            ES_PASSWORD },
    { "ES_READONLY",            ES_READONLY },
    { "ES_RIGHT",               ES_RIGHT },
    { "ES_UPPERCASE",           ES_UPPERCASE },
    { "ES_WANTRETURN",          ES_WANTRETURN },
    { "LBS_DISABLENOSCROLL",    LBS_DISABLENOSCROLL },
    { "LBS_EXTENDEDSEL",        LBS_EXTENDEDSEL },
    { "LBS_HASSTRINGS",         LBS_HASSTRINGS },
    { "LBS_MULTICOLUMN",        LBS_MULTICOLUMN },
    { "LBS_MULTIPLESEL",        LBS_MULTIPLESEL },
    { "LBS_NOINTEGRALHEIGHT",   LBS_NOINTEGRALHEIGHT },
    { "LBS_NOREDRAW",           LBS_NOREDRAW },
    { "LBS_NOTIFY",             LBS_NOTIFY },
    { "LBS_OWNERDRAWFIXED",     LBS_OWNERDRAWFIXED },
    { "LBS_OWNERDRAWVARIABLE",  LBS_OWNERDRAWVARIABLE },
    { "LBS_SORT",               LBS_SORT },
    { "LBS_STANDARD",           LBS_STANDARD },
    { "LBS_USETABSTOPS",        LBS_USETABSTOPS },
    { "LBS_WANTKEYBOARDINPUT",  LBS_WANTKEYBOARDINPUT },
    { "SBS_BOTTOMALIGN",        SBS_BOTTOMALIGN },
    { "SBS_HORZ",               SBS_HORZ },
    { "SBS_LEFTALIGN",          SBS_LEFTALIGN },
    { "SBS_RIGHTALIGN",         SBS_RIGHTALIGN },
    { "SBS_SIZEBOX",            SBS_SIZEBOX },
    { "SBS_SIZEBOXBOTTOMRIGHTALIGN",SBS_SIZEBOXBOTTOMRIGHTALIGN },
    { "SBS_SIZEBOXTOPLEFTALIGN",SBS_SIZEBOXTOPLEFTALIGN },
    { "SBS_TOPALIGN",           SBS_TOPALIGN },
    { "SBS_VERT",               SBS_VERT },
    { "SS_BLACKFRAME",          SS_BLACKFRAME },
    { "SS_BLACKRECT",           SS_BLACKRECT },
    { "SS_CENTER",              SS_CENTER },
    { "SS_GRAYFRAME",           SS_GRAYFRAME },
    { "SS_GRAYRECT",            SS_GRAYRECT },
    { "SS_ICON",                SS_ICON },
    { "SS_LEFT",                SS_LEFT },
    { "SS_LEFTNOWORDWRAP",      SS_LEFTNOWORDWRAP },
    { "SS_NOPREFIX",            SS_NOPREFIX },
    { "SS_RIGHT",               SS_RIGHT },
    { "SS_SIMPLE",              SS_SIMPLE },
    { "SS_WHITEFRAME",          SS_WHITEFRAME },
    { "SS_WHITERECT",           SS_WHITERECT },
    { "WS_BORDER",              WS_BORDER },
    { "WS_CAPTION",             WS_CAPTION },
    { "WS_CHILD",               WS_CHILD },
    { "WS_CHILDWINDOW",         WS_CHILDWINDOW },
    { "WS_CLIPCHILDREN",        WS_CLIPCHILDREN },
    { "WS_CLIPSIBLINGS",        WS_CLIPSIBLINGS },
    { "WS_DISABLED",            WS_DISABLED },
    { "WS_DLGFRAME",            WS_DLGFRAME },
    { "WS_EX_ACCEPTFILES",      WS_EX_ACCEPTFILES },
    { "WS_EX_DLGMODALFRAME",    WS_EX_DLGMODALFRAME },
    { "WS_EX_NOPARENTNOTIFY",   WS_EX_NOPARENTNOTIFY },
    { "WS_EX_TOPMOST",          WS_EX_TOPMOST },
    { "WS_EX_TRANSPARENT",      WS_EX_TRANSPARENT },
    { "WS_GROUP",               WS_GROUP },
    { "WS_HSCROLL",             WS_HSCROLL },
    { "WS_ICONIC",              WS_ICONIC },
    { "WS_MAXIMIZE",            WS_MAXIMIZE },
    { "WS_MAXIMIZEBOX",         WS_MAXIMIZEBOX },
    { "WS_MINIMIZE",            WS_MINIMIZE },
    { "WS_MINIMIZEBOX",         WS_MINIMIZEBOX },
    { "WS_OVERLAPPED",          WS_OVERLAPPED },
    { "WS_OVERLAPPEDWINDOW",    WS_OVERLAPPEDWINDOW },
    { "WS_POPUP",               WS_POPUP },
    { "WS_POPUPWINDOW",         WS_POPUPWINDOW },
    { "WS_SIZEBOX",             WS_SIZEBOX },
    { "WS_SYSMENU",             WS_SYSMENU },
    { "WS_TABSTOP",             WS_TABSTOP },
    { "WS_THICKFRAME",          WS_THICKFRAME },
    { "WS_TILED",               WS_TILED },
    { "WS_TILEDWINDOW",         WS_TILEDWINDOW },
    { "WS_VISIBLE",             WS_VISIBLE },
    { "WS_VSCROLL",             WS_VSCROLL },
};


static int CompareWinStyles( const void * lhs, const void * rhs )
//------------------------------------------------------------
{
    return strcmp( (const char *) lhs, ((const WinStyleStruct *)rhs)->name );
}

extern uint_32 getWindowsStyle( const char * stl )
//------------------------------------------------
{
    WinStyleStruct * res;

    res = (WinStyleStruct *) bsearch( stl, WindowsStyles, sizeof( WindowsStyles ) / sizeof( WinStyleStruct ),
                                        sizeof( WinStyleStruct ), &CompareWinStyles );

    assert( res != NULL );

    return res->flag;
}
