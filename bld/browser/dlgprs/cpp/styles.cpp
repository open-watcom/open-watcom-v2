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

#if defined( __WINDOWS__ ) || defined( __NT__ )
#include <windows.h>
#else
/* Button control styles */
#define BS_PUSHBUTTON           0x0000L
#define BS_DEFPUSHBUTTON        0x0001L
#define BS_CHECKBOX             0x0002L
#define BS_AUTOCHECKBOX         0x0003L
#define BS_RADIOBUTTON          0x0004L
#define BS_3STATE               0x0005L
#define BS_AUTO3STATE           0x0006L
#define BS_GROUPBOX             0x0007L
#define BS_USERBUTTON           0x0008L
#define BS_AUTORADIOBUTTON      0x0009L
#define BS_OWNERDRAW            0x000BL
#define BS_LEFTTEXT             0x0020L
/* Combo box control styles */
#define CBS_SIMPLE              0x0001L
#define CBS_DROPDOWN            0x0002L
#define CBS_DROPDOWNLIST        0x0003L
#define CBS_OWNERDRAWFIXED      0x0010L
#define CBS_OWNERDRAWVARIABLE   0x0020L
#define CBS_AUTOHSCROLL         0x0040L
#define CBS_OEMCONVERT          0x0080L
#define CBS_SORT                0x0100L
#define CBS_HASSTRINGS          0x0200L
#define CBS_NOINTEGRALHEIGHT    0x0400L
#define CBS_DISABLENOSCROLL     0x0800L
/* Dialog box styles */
#define DS_ABSALIGN             0x0001L
#define DS_SYSMODAL             0x0002L
#define DS_LOCALEDIT            0x0020L
#define DS_SETFONT              0x0040L
#define DS_MODALFRAME           0x0080L
#define DS_NOIDLEMSG            0x0100L
/* Edit control styles */
#define ES_LEFT                 0x0000L
#define ES_CENTER               0x0001L
#define ES_RIGHT                0x0002L
#define ES_MULTILINE            0x0004L
#define ES_UPPERCASE            0x0008L
#define ES_LOWERCASE            0x0010L
#define ES_PASSWORD             0x0020L
#define ES_AUTOVSCROLL          0x0040L
#define ES_AUTOHSCROLL          0x0080L
#define ES_NOHIDESEL            0x0100L
#define ES_OEMCONVERT           0x0400L
#define ES_READONLY             0x0800L
#define ES_WANTRETURN           0x1000L
/* List box control styles */
#define LBS_NOTIFY              0x0001L
#define LBS_SORT                0x0002L
#define LBS_NOREDRAW            0x0004L
#define LBS_MULTIPLESEL         0x0008L
#define LBS_OWNERDRAWFIXED      0x0010L
#define LBS_OWNERDRAWVARIABLE   0x0020L
#define LBS_HASSTRINGS          0x0040L
#define LBS_USETABSTOPS         0x0080L
#define LBS_NOINTEGRALHEIGHT    0x0100L
#define LBS_MULTICOLUMN         0x0200L
#define LBS_WANTKEYBOARDINPUT   0x0400L
#define LBS_EXTENDEDSEL         0x0800L
#define LBS_DISABLENOSCROLL     0x1000L
#define LBS_STANDARD            (LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER)
/* Scroll bar control styles */
#define SBS_HORZ                    0x0000L
#define SBS_VERT                    0x0001L
#define SBS_TOPALIGN                0x0002L
#define SBS_LEFTALIGN               0x0002L
#define SBS_BOTTOMALIGN             0x0004L
#define SBS_RIGHTALIGN              0x0004L
#define SBS_SIZEBOXTOPLEFTALIGN     0x0002L
#define SBS_SIZEBOXBOTTOMRIGHTALIGN 0x0004L
#define SBS_SIZEBOX                 0x0008L
/* Static control styles */
#define SS_LEFT                 0x0000L
#define SS_CENTER               0x0001L
#define SS_RIGHT                0x0002L
#define SS_ICON                 0x0003L
#define SS_BLACKRECT            0x0004L
#define SS_GRAYRECT             0x0005L
#define SS_WHITERECT            0x0006L
#define SS_BLACKFRAME           0x0007L
#define SS_GRAYFRAME            0x0008L
#define SS_WHITEFRAME           0x0009L
#define SS_SIMPLE               0x000BL
#define SS_LEFTNOWORDWRAP       0x000CL
#define SS_NOPREFIX             0x0080L
/* Window styles */
#define WS_OVERLAPPED           0x00000000L
#define WS_POPUP                0x80000000L
#define WS_CHILD                0x40000000L
#define WS_CLIPSIBLINGS         0x04000000L
#define WS_CLIPCHILDREN         0x02000000L
#define WS_VISIBLE              0x10000000L
#define WS_DISABLED             0x08000000L
#define WS_MINIMIZE             0x20000000L
#define WS_MAXIMIZE             0x01000000L
#define WS_CAPTION              0x00C00000L
#define WS_BORDER               0x00800000L
#define WS_DLGFRAME             0x00400000L
#define WS_VSCROLL              0x00200000L
#define WS_HSCROLL              0x00100000L
#define WS_SYSMENU              0x00080000L
#define WS_THICKFRAME           0x00040000L
#define WS_MINIMIZEBOX          0x00020000L
#define WS_MAXIMIZEBOX          0x00010000L
#define WS_GROUP                0x00020000L
#define WS_TABSTOP              0x00010000L
#define WS_OVERLAPPEDWINDOW     (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#define WS_POPUPWINDOW      	(WS_POPUP | WS_BORDER | WS_SYSMENU)
#define WS_CHILDWINDOW      	(WS_CHILD)
/* Extended window styles */
#define WS_EX_DLGMODALFRAME     0x00000001L
#define WS_EX_NOPARENTNOTIFY    0x00000004L
#define WS_EX_TOPMOST           0x00000008L
#define WS_EX_ACCEPTFILES       0x00000010L
#define WS_EX_TRANSPARENT   	0x00000020L
/* Old names for window styles */
#define WS_TILED                WS_OVERLAPPED
#define WS_ICONIC               WS_MINIMIZE
#define WS_SIZEBOX              WS_THICKFRAME
#define WS_TILEDWINDOW          WS_OVERLAPPEDWINDOW
#endif

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
