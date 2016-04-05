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


#include "wdeglbl.h"
#include "wderesin.h"
#include "wdehash.h"
#include "wdeldres.h"
#include "wdesvres.h"
#include "wdesvdlg.h"
#include "wdesdup.h"
#include "wresall.h"
#include "wdecctl.h"
#include "wde_rc.h"
#include "wde_wres.h"
#include "wrdll.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define str( a )            #a
#define xstr( a )           str( a )
#define wpick( a, b, s )    { a##b, a##b, a##b, xstr( a )##xstr( b ) , s }
#define opick( a, b )       { a##b, 0xf, 0x0, xstr( a )##xstr( b ), ControlStyle }
#define lvs_pick( a, b )    { a##b, 0x3, 0x3, xstr( a )##xstr( b ), ControlStyle }
#define jpick( a, b )       { a##b, 0x0f00, 0x0, xstr( a )##xstr( b ), ControlStyle }
#define fpick( a, b )       { a##b, a##b, a##b, xstr( a )##xstr( b ), ControlStyle }
#define sentinel            { 0, 0, 0, NULL, 0 }

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef enum {
    WindowStyle         = 0x01,
    ControlStyle        = 0x02,
    BothStyle           = 0x03
} flag_style;

typedef struct flag_map {
    unsigned long       flag;
    unsigned long       check_mask;
    unsigned long       erase_mask;
    char                *text;
    flag_style          style;
} flag_map;

// Window Style mappings
flag_map WindowMap[] = {
    wpick( WS_, OVERLAPPEDWINDOW, BothStyle ),
    wpick( WS_, POPUPWINDOW, BothStyle ),
    wpick( WS_, CAPTION, BothStyle ),
//  wpick( WS_, OVERLAPPED, BothStyle ),
    wpick( WS_, POPUP, BothStyle ),
    wpick( WS_, CHILD, BothStyle ),
    wpick( WS_, CLIPSIBLINGS, BothStyle ),
    wpick( WS_, CLIPCHILDREN, BothStyle ),
    wpick( WS_, VISIBLE, BothStyle ),
    wpick( WS_, DISABLED, BothStyle ),
    wpick( WS_, MINIMIZE, BothStyle ),
    wpick( WS_, MAXIMIZE, BothStyle ),
    wpick( WS_, BORDER, BothStyle ),
    wpick( WS_, DLGFRAME, BothStyle ),
    wpick( WS_, VSCROLL, BothStyle ),
    wpick( WS_, HSCROLL, BothStyle ),
    wpick( WS_, SYSMENU, BothStyle ),
    wpick( WS_, THICKFRAME, BothStyle ),
    wpick( WS_, MINIMIZEBOX, WindowStyle ),
    wpick( WS_, MAXIMIZEBOX, WindowStyle ),
    wpick( WS_, GROUP, ControlStyle ),
    wpick( WS_, TABSTOP, ControlStyle ),
    sentinel
};

// Window Extended Style mappings
flag_map WindowEXMap[] = {
    wpick( WS_EX_, DLGMODALFRAME,       WindowStyle),
    wpick( WS_EX_, NOPARENTNOTIFY,      WindowStyle),
//  wpick( WS_EX_, TOPMOST,             WindowStyle),   // in code
    wpick( WS_EX_, ACCEPTFILES,         WindowStyle),
//  wpick( WS_EX_, TRANSPARENT,         WindowStyle),   // TRANSPARENT == 1 somewhere
    wpick( WS_EX_, MDICHILD,            WindowStyle),
//  wpick( WS_EX_, TOOLWINDOW,          WindowStyle),   // in code
//  wpick( WS_EX_, WINDOWEDGE,          WindowStyle),   // in code
//  wpick( WS_EX_, CLIENTEDGE,          WindowStyle),   // in code
    wpick( WS_EX_, CONTEXTHELP,         WindowStyle),
    wpick( WS_EX_, RIGHT,               WindowStyle),
//  wpick( WS_EX_, LEFT,                WindowStyle),   // default
    wpick( WS_EX_, RTLREADING,          WindowStyle),
//  wpick( WS_EX_, LTRREADING,          WindowStyle),   // default
    wpick( WS_EX_, LEFTSCROLLBAR,       WindowStyle),
//  wpick( WS_EX_, RIGHTSCROLLBAR,      WindowStyle),   // default
    wpick( WS_EX_, CONTROLPARENT,       WindowStyle),
    wpick( WS_EX_, STATICEDGE,          WindowStyle),
    wpick( WS_EX_, APPWINDOW,           WindowStyle),
//  wpick( WS_EX_, OVERLAPPEDWINDOW,    WindowStyle),  // in code
//  wpick( WS_EX_, PALETTEWINDOW,       WindowStyle),  // in code
    sentinel
};

// Dialog Window mappings
flag_map DialogMap[] = {
    fpick( DS_, ABSALIGN ),
    fpick( DS_, SYSMODAL ),
    fpick( DS_, LOCALEDIT ),
//  fpick( DS_, SETFONT ),
    fpick( DS_, MODALFRAME ),
    fpick( DS_, NOIDLEMSG ),
    fpick( DS_, SETFOREGROUND ),
#ifdef __NT__
    // fpick macro doesn't like the '3' and expands wrong
    { DS_3DLOOK, DS_3DLOOK, DS_3DLOOK, "DS_3DLOOK", ControlStyle },
    fpick( DS_, FIXEDSYS ),
    fpick( DS_, NOFAILCREATE ),
    fpick( DS_, CONTROL ),
    fpick( DS_, CENTER ),
    fpick( DS_, CENTERMOUSE ),
    fpick( DS_, CONTEXTHELP ),
#endif // __NT__
    sentinel
};

// Static Control mappings
flag_map StaticMap[] = {
    opick( SS_, LEFT ),
    opick( SS_, CENTER ),
    opick( SS_, RIGHT ),
    opick( SS_, ICON ),
    opick( SS_, BLACKRECT ),
    opick( SS_, GRAYRECT ),
    opick( SS_, WHITERECT ),
    opick( SS_, BLACKFRAME ),
    opick( SS_, GRAYFRAME ),
    opick( SS_, WHITEFRAME ),
    opick( SS_, SIMPLE ),
    opick( SS_, LEFTNOWORDWRAP ),
    fpick( SS_, NOPREFIX ),
#ifdef __NT__
    opick( SS_, BITMAP ),
    opick( SS_, ENHMETAFILE ),
    opick( SS_, ETCHEDFRAME ),
    opick( SS_, ETCHEDHORZ ),
    opick( SS_, ETCHEDVERT ),

    fpick( SS_, OWNERDRAW ),
    fpick( SS_, CENTERIMAGE ),
    fpick( SS_, NOTIFY ),
    fpick( SS_, REALSIZEIMAGE ),
    fpick( SS_, RIGHTJUST ),
    fpick( SS_, SUNKEN ),
#endif // __NT__

    sentinel
};

// Button Control mappings
flag_map ButtonMap[] = {
    opick( BS_, PUSHBUTTON ),
    opick( BS_, DEFPUSHBUTTON ),
    opick( BS_, CHECKBOX ),
    opick( BS_, AUTOCHECKBOX ),
    opick( BS_, RADIOBUTTON ),
    // opick macro doesn't like the '3' and expands wrong
    { BS_3STATE, 0xf, 0x0, "BS_3STATE", ControlStyle },
    opick( BS_, AUTO3STATE ),
    opick( BS_, GROUPBOX ),
    opick( BS_, USERBUTTON ),
    opick( BS_, AUTORADIOBUTTON ),
    opick( BS_, OWNERDRAW ),
    fpick( BS_, LEFTTEXT ),
 // fpick( BS_, TEXT ),         // default if not BITMAP or ICON
#ifdef __NT__
    fpick( BS_, BITMAP ),
    fpick( BS_, ICON ),
    jpick( BS_, LEFT ),
    jpick( BS_, RIGHT ),
    jpick( BS_, CENTER ),
    jpick( BS_, TOP ),
    jpick( BS_, BOTTOM ),
    jpick( BS_, VCENTER ),
    fpick( BS_, PUSHLIKE ),
    fpick( BS_, MULTILINE ),
    fpick( BS_, NOTIFY ),
//  fpick( BS_, RIGHTBUTTON ),  // same as LEFTTEXT
#endif // __NT__
    sentinel
};

// Edit control mappings
flag_map EditMap[] = {
//  fpick( ES_, LEFT ), // this has been special cased
    fpick( ES_, CENTER ),
    fpick( ES_, RIGHT ),
    fpick( ES_, MULTILINE ),
    fpick( ES_, UPPERCASE ),
    fpick( ES_, LOWERCASE ),
    fpick( ES_, PASSWORD ),
    fpick( ES_, AUTOVSCROLL ),
    fpick( ES_, AUTOHSCROLL ),
    fpick( ES_, NOHIDESEL ),
    fpick( ES_, OEMCONVERT ),
    fpick( ES_, READONLY ),
    fpick( ES_, WANTRETURN ),
#ifdef __NT__
    fpick( ES_, NUMBER ),
#endif // __NT__
    sentinel
};

// Horizontal Scroll bar mappings
flag_map HScrollMap[] = {
    fpick( SBS_, HORZ ),
    fpick( SBS_, TOPALIGN ),
    fpick( SBS_, BOTTOMALIGN ),
    sentinel
};

// Vertical Scroll bar mappings
flag_map VScrollMap[] = {
    fpick( SBS_, VERT ),
    fpick( SBS_, LEFTALIGN ),
    fpick( SBS_, RIGHTALIGN ),
    sentinel
};

// Size Box mappings
flag_map SizeBoxMap[] = {
    fpick( SBS_, SIZEBOX ),
    fpick( SBS_, SIZEBOXTOPLEFTALIGN ),
    fpick( SBS_, SIZEBOXBOTTOMRIGHTALIGN ),
#ifdef __NT__
    fpick( SBS_, SIZEGRIP ),
#endif // __NT__
    sentinel
};

// Listbox mappings
flag_map ListBoxMap[] = {
    fpick( LBS_, STANDARD ),
    fpick( LBS_, NOTIFY ),
    fpick( LBS_, SORT ),
    fpick( LBS_, NOREDRAW ),
    fpick( LBS_, MULTIPLESEL ),
    fpick( LBS_, OWNERDRAWFIXED ),
    fpick( LBS_, OWNERDRAWVARIABLE ),
    fpick( LBS_, HASSTRINGS ),
    fpick( LBS_, USETABSTOPS ),
    fpick( LBS_, NOINTEGRALHEIGHT ),
    fpick( LBS_, MULTICOLUMN ),
    fpick( LBS_, WANTKEYBOARDINPUT ),
    fpick( LBS_, EXTENDEDSEL ),
    fpick( LBS_, DISABLENOSCROLL ),
#ifdef __NT__
    fpick( LBS_, NOSEL ),
#endif // __NT__
    sentinel
};

// Combo box mappings
flag_map ComboBoxMap[] = {
    opick( CBS_, SIMPLE ),
    opick( CBS_, DROPDOWN ),
    opick( CBS_, DROPDOWNLIST ),
    fpick( CBS_, OWNERDRAWFIXED ),
    fpick( CBS_, OWNERDRAWVARIABLE ),
    fpick( CBS_, AUTOHSCROLL ),
    fpick( CBS_, OEMCONVERT ),
    fpick( CBS_, SORT ),
    fpick( CBS_, HASSTRINGS ),
    fpick( CBS_, NOINTEGRALHEIGHT ),
    fpick( CBS_, DISABLENOSCROLL ),
#ifdef __NT__
    fpick( CBS_, UPPERCASE ),
    fpick( CBS_, LOWERCASE ),
#endif // __NT__
    sentinel
};

// Status Bar mappings
flag_map StatusBarMap[] = {
    fpick( SBT_, OWNERDRAW ),
    fpick( SBT_, NOBORDERS ),
    fpick( SBT_, POPOUT ),
//  fpick( SBT_, RTLREADING ),   // isn't there for NT
    sentinel
};

// Progress bar mappings
flag_map ProgressMap[] = {
    sentinel   // no special styles
};

// Track Bar mappings
flag_map TrackBarMap[] = {
    fpick( TBS_, AUTOTICKS ),
    fpick( TBS_, VERT ),
//  fpick( TBS_, HORZ ),
//  fpick( TBS_, TOP ),
//  fpick( TBS_, BOTTOM ),
//  fpick( TBS_, LEFT ),
//  fpick( TBS_, RIGHT ),
    fpick( TBS_, BOTH ),
    fpick( TBS_, NOTICKS ),
    fpick( TBS_, ENABLESELRANGE ),
    fpick( TBS_, FIXEDLENGTH ),
    fpick( TBS_, NOTHUMB ),
    sentinel
};

// Up Down Control mappings
flag_map UpDownMap[] = {
    fpick( UDS_, WRAP ),
    fpick( UDS_, SETBUDDYINT ),
    fpick( UDS_, ALIGNRIGHT ),
    fpick( UDS_, ALIGNLEFT ),
    fpick( UDS_, AUTOBUDDY ),
    fpick( UDS_, ARROWKEYS ),
    fpick( UDS_, HORZ ),
    fpick( UDS_, NOTHOUSANDS ),
    sentinel
};

// Header Control mappings
flag_map HeaderMap[] = {
//  fpick( HDS_, HORZ ),
    fpick( HDS_, BUTTONS ),
//  fpick( HDS_, HIDDEN ),
    sentinel
};

// List View mappings
flag_map ListViewMap[] = {
    lvs_pick( LVS_, ICON ),
    lvs_pick( LVS_, REPORT ),
    lvs_pick( LVS_, SMALLICON ),
    lvs_pick( LVS_, LIST ),
    fpick( LVS_, SINGLESEL ),
    fpick( LVS_, SHOWSELALWAYS ),
    fpick( LVS_, SORTASCENDING ),
    fpick( LVS_, SORTDESCENDING ),
    fpick( LVS_, SHAREIMAGELISTS ),
    fpick( LVS_, NOLABELWRAP ),
    fpick( LVS_, AUTOARRANGE ),
    fpick( LVS_, EDITLABELS ),
    fpick( LVS_, NOSCROLL ),
//  fpick( LVS_, ALIGNTOP ),   // 0x0000 value
    fpick( LVS_, ALIGNLEFT ),
    fpick( LVS_, OWNERDRAWFIXED ),
    fpick( LVS_, NOCOLUMNHEADER ),
    fpick( LVS_, NOSORTHEADER ),
    sentinel
};

// Tree View mappings
flag_map TreeViewMap[] = {
    fpick( TVS_, HASBUTTONS ),
    fpick( TVS_, HASLINES ),
    fpick( TVS_, LINESATROOT ),
    fpick( TVS_, EDITLABELS ),
    fpick( TVS_, DISABLEDRAGDROP ),
    fpick( TVS_, SHOWSELALWAYS ),
    sentinel
};

// Tab Control mappings
flag_map TabControlMap[] = {
    fpick( TCS_, FORCEICONLEFT ),       // 0x0010
    fpick( TCS_, FORCELABELLEFT ),      // 0x0020
//  fpick( TCS_, TABS ),                // 0x0000
    fpick( TCS_, BUTTONS ),             // 0x0100
//  fpick( TCS_, SINGLELINE ),          // 0x0000
    fpick( TCS_, MULTILINE ),           // 0x0200
//  fpick( TCS_, RIGHTJUSTIFY ),        // 0x0000
    fpick( TCS_, FIXEDWIDTH ),          // 0x0400
//  fpick( TCS_, RAGGEDRIGHT ),         // 0x0800
    fpick( TCS_, FOCUSONBUTTONDOWN ),   // 0x1000
    fpick( TCS_, OWNERDRAWFIXED ),      // 0x2000
    fpick( TCS_, TOOLTIPS ),            // 0x4000
    fpick( TCS_, FOCUSNEVER ),          // 0x8000
    sentinel
};

// Animate mappings
flag_map AnimateMap[] = {
    fpick( ACS_, CENTER ),
//  fpick( ACS_, TRANSPARENT ),  // #define TRANSPARENT 1 somewhere
    fpick( ACS_, AUTOPLAY ),
    sentinel
};

// Hotkey mappings
flag_map HotkeyMap[] = {
    sentinel   // no special styles
};


/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool WdeWriteDlgControl( WdeResInfo *rinfo, WdeDialogBoxControl *control, bool is32bitEx, FILE *fp, uint_16 );
static bool WdeWriteDlgHeader( WdeResInfo *rinfo, WdeResDlgItem *ditem, FILE *fp );
static bool WdeSaveDlgItemToRC( WdeResInfo *rinfo, WdeResDlgItem *ditem, FILE *fp );
static bool WdeCreateItemDBI( WdeResInfo *rinfo, WdeResDlgItem *ditem );

bool WdeSetMemFlagsText( uint_16 flags, char **text )
{
    int         tlen;

    if( text == NULL ) {
        return( FALSE );
    }

    tlen = 0;
    *text = NULL;

    if( flags & MEMFLAG_PRELOAD ) {
        tlen += 8; // size of the string PRELOAD and a space
    }

    if( !(flags & MEMFLAG_MOVEABLE) ) {
        tlen += 6; // size of the string FIXED and a space
    }

    if( flags & MEMFLAG_DISCARDABLE ) {
        tlen += 12; // size of the string DISCARDABLE and a space
    }

    if( !(flags & MEMFLAG_PURE) ) {
        tlen += 7; // size of the string IMPURE and a space
    }

    if( tlen == 0 ) {
        return( TRUE );
    }

    *text = (char *)WRMemAlloc( tlen + 1 );
    if( *text == NULL ) {
        return( FALSE );
    }
    (*text)[0] = '\0';

    if( flags & MEMFLAG_PRELOAD ) {
        strcat( *text, "PRELOAD " );
    }

    if( !(flags & MEMFLAG_MOVEABLE) ) {
        strcat( *text, "FIXED " );
    }

    if( flags & MEMFLAG_DISCARDABLE ) {
        strcat( *text, "DISCARDABLE " );
    }

    if( !(flags & MEMFLAG_PURE) ) {
        strcat( *text, "IMPURE " );
    }

    return( TRUE );
}

static bool WdeSetFlagText( flag_map *map, flag_style fs, unsigned long flags, char **text )
{
    int         tlen;
    int         new_tlen;
    int         slen;
    int         not_first;

    if( map == NULL || text == NULL ) {
        return( FALSE );
    }

    tlen = 0;
    not_first = 0;
    if( *text != NULL ) {
        tlen = strlen( *text );
        not_first = 1;
    }

    while( map->text ) {
        if( (flags & map->check_mask) == map->flag && (fs & map->style) ) {
            slen = strlen( map->text );
            new_tlen = tlen + 3 * not_first + slen + 1;
            *text = (char *)WRMemRealloc( *text, new_tlen );
            if( not_first == 1 ) {
                strcat( *text, " | " );
                strcat( *text, map->text );
            } else {
                strcpy( *text, map->text );
                not_first = 1;
            }
            tlen = new_tlen;
            flags &= ~map->erase_mask;
        }
        map++;
    }

    return( TRUE );
}

#if 0
bool WdeSetWindowFlagText( unsigned long flags, char **text )
{
    if( text == NULL ) {
        return( FALSE );
    }

    *text = NULL;
    if( WdeSetFlagText( WindowMap, WindowStyle, flags, text ) ) {
        return( TRUE );
    }

    if( *text != NULL ) {
        WRMemFree( *text );
        *text = NULL;
    }

    return( FALSE );
}
#endif

static bool WdeAddStyleString( char **text, char *str )
{
    int slen;
    int tlen;

    if( text == NULL || str == NULL ) {
        return( FALSE );
    }

    slen = strlen( str );

    if( *text == NULL ) {
        *text = WRMemAlloc( slen + 1 );
        if( *text != NULL ) {
            strcpy( *text, str );
        }
    } else {
        tlen = strlen( *text );
        tlen += slen + 3 + 1;
        *text = (char *)WRMemRealloc( *text, tlen );
        if( *text != NULL ) {
            strcat( *text, " | " );
            strcat( *text, str );
        }
    }

    return( *text != NULL );
}

bool WdeSetDialogFlagText( unsigned long flags, char **text )
{
    if( text == NULL ) {
        return( FALSE );
    }

    *text = NULL;
    if( WdeSetFlagText( DialogMap, ControlStyle, flags, text ) ) {
        if( (flags & (WS_CHILD | WS_POPUP)) == 0 ) {
            WdeAddStyleString( text, "WS_OVERLAPPED" );
        }
        if( WdeSetFlagText( WindowMap, WindowStyle, flags, text ) ) {
            return( TRUE );
        }
    }

    if( *text != NULL ) {
        WRMemFree( *text );
        *text = NULL;
    }

    return( FALSE );
}

#if __NT__XX
static bool WdeSetEXFlagText( uint_32 flags, char **text )
{
    uint_32 mask;

    if( text == NULL ) {
        return( FALSE );
    }

    *text = NULL;
    mask = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;
    if( (flags & mask) == mask ) {
        WdeAddStyleString( text, "WS_EX_OVERLAPPEDWINDOW" );
    } else {
        if( flags & WS_EX_CLIENTEDGE ) {
            WdeAddStyleString( text, "WS_EX_CLIENTEDGE" );
        }
        // WINDOWEDGE will be caught below
    }

    mask = WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
    if( (flags & mask) == mask ) {
        WdeAddStyleString( text, "WS_EX_PALETTEWINDOW" );
    } else {
        if( flags & WS_EX_WINDOWEDGE ) {
            WdeAddStyleString( text, "WS_EX_WINDOWEDGE" );
        }
        if( flags & WS_EX_TOOLWINDOW ) {
            WdeAddStyleString( text, "WS_EX_TOOLWINDOW" );
        }
        if( flags & WS_EX_TOPMOST ) {
            WdeAddStyleString( text, "WS_EX_TOPMOST" );
        }
    }

    if( flags & WS_EX_TRANSPARENT ) {
        WdeAddStyleString( text, "WS_EX_TRANSPARENT" );
    }

    if( WdeSetFlagText( WindowEXMap, WindowStyle, flags, text ) ) {
        return( TRUE );
    }

    if( *text != NULL ) {
        WRMemFree( *text );
        *text = NULL;
    }

    return( FALSE );
}
#endif

bool WdeSetControlFlagText( uint_8 class, unsigned long flags, char **text )
{
    bool        ok;
    char        msg[80];

    if( text == NULL ) {
        return( FALSE );
    }

    *text = NULL;

    ok = true;

    switch( class ) {
    case CLASS_BUTTON:
        ok = WdeSetFlagText( ButtonMap, ControlStyle, flags, text );
        break;
    case CLASS_EDIT:
        if( (flags & 0x3) == 0 ) {
            ok = WdeAddStyleString( text, "ES_LEFT" );
        }
        ok = ok && WdeSetFlagText( EditMap, ControlStyle, flags, text );
        break;
    case CLASS_STATIC:
        ok = WdeSetFlagText( StaticMap, ControlStyle, flags, text );
        break;
    case CLASS_LISTBOX:
        ok = WdeSetFlagText( ListBoxMap, ControlStyle, flags, text );
        break;
    case CLASS_COMBOBOX:
        ok = WdeSetFlagText( ComboBoxMap, ControlStyle, flags, text );
        break;
    case CLASS_SCROLLBAR:
        if( flags & SBS_VERT ) {
            ok = WdeSetFlagText( VScrollMap, ControlStyle, flags, text );
        } else if( flags & SBS_SIZEBOX ) {
            ok = WdeSetFlagText( SizeBoxMap, ControlStyle, flags, text );
        } else {
            ok = WdeSetFlagText( HScrollMap, ControlStyle, flags, text );
        }
        break;
    default:
        sprintf( msg, "Class = 0x%2.2x  ('m' = 0x%2.2x)", class, (int)'m' );
        MessageBox( (HWND)NULL, msg, "FYI", MB_OK );
        break;
    }

    if( ok ) {
        if( (flags & (WS_CHILD | WS_POPUP)) == 0 ) {
            ok = WdeAddStyleString( text, "WS_OVERLAPPED" );
        }
    }

    if( ok ) {
        if( (flags & WS_VISIBLE) == 0 ) {
            ok = WdeAddStyleString( text, "NOT WS_VISIBLE" );
        }
    }

    if( ok ) {
        ok = WdeSetFlagText( WindowMap, ControlStyle, flags, text );
    }

    if( !ok ) {
        if( *text != NULL  ) {
            WRMemFree( *text );
            *text = NULL;
        }
    }

    return( ok );
}

static bool WdeSetCommControlFlagText( char *control_class, unsigned long flags, char **text )
{
    bool        ok;

    if( text == NULL ) {
        return( FALSE );
    }
    *text = NULL;
    ok = true;

    if( stricmp( control_class, STATUSCLASSNAME ) == 0 ) {
        ok = WdeSetFlagText( StatusBarMap, ControlStyle, flags, text );
    } else if( stricmp( control_class, WC_LISTVIEW ) == 0 ) {
        ok = WdeSetFlagText( ListViewMap, ControlStyle, flags, text );
    } else if( stricmp( control_class, WC_TREEVIEW ) == 0 ) {
        ok = WdeSetFlagText( TreeViewMap, ControlStyle, flags, text );
    } else if( stricmp( control_class, WC_TABCONTROL ) == 0 ) {
        ok = WdeSetFlagText( TabControlMap, ControlStyle, flags, text );
        if( ok && !(flags & TCS_RAGGEDRIGHT) ) {  // default
            ok = WdeAddStyleString( text, "TCS_RIGHTJUSTIFY" );
        }
    } else if( stricmp( control_class, ANIMATE_CLASS ) == 0 ) {
        ok = WdeSetFlagText( AnimateMap, ControlStyle, flags, text );
        if( ok && (flags & ACS_TRANSPARENT) ) {
            ok = WdeAddStyleString( text, "ACS_TRANSPARENT" );
        }
    } else if( stricmp( control_class, UPDOWN_CLASS ) == 0 ) {
        ok = WdeSetFlagText( UpDownMap, ControlStyle, flags, text );
    } else if( stricmp( control_class, TRACKBAR_CLASS ) == 0 ) {
        ok = WdeSetFlagText( TrackBarMap, ControlStyle, flags, text );
        if( ok && (flags & TBS_LEFT) ) {   // same as TBS_TOP
            if( flags & TBS_VERT ) {
                ok = WdeAddStyleString( text, "TBS_LEFT" );
            } else {
                ok = WdeAddStyleString( text, "TBS_TOP" );
            }
        }
    } else if( stricmp( control_class, PROGRESS_CLASS ) == 0 ) {
        ok = WdeSetFlagText( ProgressMap, ControlStyle, flags, text );
    } else if( stricmp( control_class, HOTKEY_CLASS ) == 0 ) {
        ok = WdeSetFlagText( HotkeyMap, ControlStyle, flags, text );
    } else if( stricmp( control_class, WC_HEADER ) == 0 ) {
        ok = WdeSetFlagText( HeaderMap, ControlStyle, flags, text );
    }

    if( ok ) {
        if( (flags & (WS_CHILD | WS_POPUP)) == 0 ) {
            ok = WdeAddStyleString( text, "WS_OVERLAPPED" );
        }
    }

    if( ok ) {
        if( (flags & WS_VISIBLE) == 0 ) {
            ok = WdeAddStyleString( text, "NOT WS_VISIBLE" );
        }
    }

    if( ok ) {
        ok = WdeSetFlagText( WindowMap, ControlStyle, flags, text );
    }

    if( !ok ) {
        if( *text != NULL ) {
            WRMemFree( *text );
            *text = NULL;
        }
    }

    return( ok );
}

bool WdeWriteDlgControl( WdeResInfo *rinfo, WdeDialogBoxControl *control,
                         bool is32bitEx, FILE *fp, uint_16 nlength )
{
    char                *ctext;
    char                *n;
    bool                ctext_alloc;
    char                *cid;
    bool                cid_alloc;
    char                *cclass;
    char                *cstyle;
    char                *ExStyle;
    ControlClass        *control_class;
    uint_8              class_type;
    uint_16             nlen;
    char                *ControlStr;
    int                 len;
    bool                ok;

    cid = NULL;
    n = NULL;
    ctext = NULL;
    cclass = NULL;
    cstyle = NULL;
    ExStyle = NULL;
    ctext_alloc = FALSE;
    cid_alloc = FALSE;
    ok = (rinfo != NULL && control != NULL &&
          GETCTL_CLASSID( control ) != NULL && fp != NULL);

    if( ok ) {
        if( GETCTL_TEXT( control ) != NULL ) {
            ctext = WdeResNameOrOrdinalToStr( GETCTL_TEXT( control ), 10 );
            ctext_alloc = TRUE;
        }
        if( ctext == NULL ) {
            ctext = "";
            ctext_alloc = FALSE;
        }
        ok = (ctext != NULL);
    }

    if( ok ) {
        n = WRConvertStringFrom( ctext, "\t\n\"", "tn\"" );
        ok = (n != NULL);
    }

    if( ok ) {
        control_class = GETCTL_CLASSID( control );
        cclass = WdeControlClassToStr( control_class );
        ok = (cclass != NULL);
    }

    if( ok ) {
        class_type = 0;
        if( control_class != NULL ) {
            if( control_class->Class & 0x80 ) {
                class_type = control_class->Class;
            }
        }
        if( class_type != 0 ) {
            ok = WdeSetControlFlagText( class_type, GETCTL_STYLE( control ), &cstyle );
        } else {
            ok = WdeSetCommControlFlagText( control_class->ClassName,
                                            GETCTL_STYLE( control ), &cstyle );
        }
        ok = (ok && cstyle != NULL);
    }

    if( ok ) {
        if( control->symbol != NULL ) {
            cid = control->symbol;
        } else if( rinfo->hash_table != NULL ) {
            cid = WdeResolveValue( rinfo->hash_table, GETCTL_ID( control ) );
            if( cid != NULL ) {
                cid_alloc = TRUE;
            }
        }
    }

    if( ok ) {
        nlen = nlength + 12;   // 12 is length of 'CONTROL..." string
        ControlStr = WRMemAlloc( nlen + 1 );
        sprintf( ControlStr, "\tCONTROL \"%s\",", n );
        for( len = strlen( ControlStr ); len < nlen; len++ ) {
            strcat( ControlStr, " " );
        }

        if( is32bitEx ) {
            /* extended control ==> need extended style and help id */
            char *helpsymbol;

            /* build a help symbol for the DialogEx line */
            if( control->helpsymbol != NULL ) {
                helpsymbol = WdeStrDup( control->helpsymbol );
            } else {
                helpsymbol = WRMemAlloc( 32 );
                if( control->HelpId > 0 ) {
                    sprintf( helpsymbol, "%ld", control->HelpId );
                } else {
                    *helpsymbol = '\0';   /* empty help id if 0 */
                }
            }

            if( cid != NULL ) {
                fprintf( fp, "%s %s, \"%s\", %s, %d, %d, %d, %d",
                         ControlStr, cid, cclass, cstyle,
                         GETCTL_SIZEX( control ), GETCTL_SIZEY( control ),
                         GETCTL_SIZEW( control ), GETCTL_SIZEH( control ) );
            } else {
                fprintf( fp, "%s %d, \"%s\", %s, %d, %d, %d, %d", ControlStr,
                         (GETCTL_ID( control ) == 0xffff) ? -1 : GETCTL_ID( control ),
                         cclass, cstyle,
                         GETCTL_SIZEX( control ), GETCTL_SIZEY( control ),
                         GETCTL_SIZEW( control ), GETCTL_SIZEH( control ) );
            }

#if __NT__XX
            WdeSetEXFlagText( GETCTL_EXSTYLE( control ), &ExStyle );
#endif
            /* put out the extended styles */
            if( ExStyle != NULL ) {
                fprintf( fp, ", %s", ExStyle );
            } else if( *helpsymbol != '\0' ) {
                fprintf( fp, ", 0" );    // or a 0 if help id follows
            }


            /* put out the help id, if its there */
            if( *helpsymbol != '\0' ) {
                fprintf( fp, ", %s", helpsymbol );
            }
            fprintf( fp, "\n" );

            /* free the help symbol */
            WRMemFree( helpsymbol );

        } else {
            /* standard control */
            if( cid != NULL ) {
                fprintf( fp, "%s %s, \"%s\", %s, %d, %d, %d, %d\n",
                         ControlStr, cid, cclass, cstyle,
                         GETCTL_SIZEX( control ), GETCTL_SIZEY( control ),
                         GETCTL_SIZEW( control ), GETCTL_SIZEH( control ) );
            } else {
                fprintf( fp, "%s %d, \"%s\", %s, %d, %d, %d, %d\n", ControlStr,
                         (GETCTL_ID( control ) == 0xffff) ? -1 : GETCTL_ID( control ),
                         cclass, cstyle,
                         GETCTL_SIZEX( control ), GETCTL_SIZEY( control ),
                         GETCTL_SIZEW( control ), GETCTL_SIZEH( control ) );
            }
        }
        if( ControlStr != NULL ) {
            WRMemFree( ControlStr );
        }
    }

    if( n != NULL ) {
        WRMemFree( n );
    }

    if( ctext_alloc && ctext != NULL ) {
        WRMemFree( ctext );
    }

    if( cclass != NULL ) {
        WRMemFree( cclass );
    }

    if( cstyle != NULL ) {
        WRMemFree( cstyle );
    }

#if __NT__XX
    if( ExStyle != NULL ) {
        WRMemFree( ExStyle );
    }
#endif

    if( cid_alloc && cid != NULL ) {
        WRMemFree( cid );
    }

    return( ok );
}

bool WdeWriteDlgHeader( WdeResInfo *rinfo, WdeResDlgItem *ditem, FILE *fp )
{
    DialogSizeInfo      size;
    DialogStyle         style;
    ResNameOrOrdinal    *rname;
    char                *name;
    char                *str;
    bool                ok;
    WdeDialogBoxHeader  *dhptr = ditem->dialog_info->dialog_header;
    uint_32             ExStyle;

    name = NULL;
    ok = (rinfo != NULL && ditem != NULL && fp != NULL);

    if( ok ) {
        if( ditem->dialog_info->dialog_header->symbol != NULL ) {
            name = WdeStrDup( ditem->dialog_info->dialog_header->symbol );
        } else {
            if( rinfo->hash_table != NULL && !ditem->dialog_name->IsName ) {
                name = WdeResolveValue( rinfo->hash_table, ditem->dialog_name->ID.Num );
            }
        }
        if( name == NULL ) {
            name = WResIDToStr( ditem->dialog_name );
        }
        ok = (name != NULL);
    }

    if( ok ) {
        str = NULL;
        ok = WdeSetMemFlagsText( ditem->dialog_info->MemoryFlags, &str );
    }

    if( ok ) {
        size = GETHDR_SIZE( dhptr );
        /* check if this is a 32 bit extended dialog */
        if( dhptr->is32bitEx ) {
            char *helpsymbol;

            /* build a help symbol for the DialogEx line */
            if( dhptr->helpsymbol != NULL ) {
                helpsymbol = WdeStrDup( dhptr->helpsymbol );
            } else {
                helpsymbol = WRMemAlloc( 32 );
                if( dhptr->HelpId > 0 ) {
                    sprintf( helpsymbol, "%ld", dhptr->HelpId );
                } else {
                    *helpsymbol = '\0';
                }
            }

            if( str != NULL ) {
                fprintf( fp, "%s DIALOGEX %s %d, %d, %d, %d",
                         name, str, size.x, size.y, size.width, size.height );
                WRMemFree( str );
                str = NULL;
            } else {
                fprintf( fp, "%s DIALOGEX %d, %d, %d, %d",
                         name, size.x, size.y, size.width, size.height );
            }
            if( *helpsymbol != '\0' ) {
                fprintf( fp, ", %s", helpsymbol );
            }
            fprintf( fp, "\n" );

            /* free the help symbol */
            WRMemFree( helpsymbol );

        } else {
            /* standard dialog */
            if( str != NULL ) {
                fprintf( fp, "%s DIALOG %s %d, %d, %d, %d\n",
                         name, str, size.x, size.y, size.width, size.height );
                WRMemFree( str );
                str = NULL;
            } else {
                fprintf( fp, "%s DIALOG %d, %d, %d, %d\n",
                         name, size.x, size.y, size.width, size.height );
            }
        }
        style = GETHDR_STYLE( ditem->dialog_info->dialog_header );
        ok = (WdeSetDialogFlagText( style, &str ) && str != NULL);
        WRMemFree( name );
        name = NULL;
    }

    if( ok ) {
        if( str != NULL ) {
            fprintf( fp, "STYLE %s\n", str );
            WRMemFree( str );
            str = NULL;
        }
        rname = GETHDR_CLASSNAME( ditem->dialog_info->dialog_header );
        if( rname != NULL ) {
            str = WdeResNameOrOrdinalToStr( rname, 10 );
            if( str != NULL ) {
                if( *str != '\0' ) {
                    fprintf( fp, "CLASS \"%s\"\n", str );
                }
                WRMemFree( str );
                str = NULL;
            }
        }
        if( style & WS_CAPTION ) {
            if( GETHDR_CAPTION( ditem->dialog_info->dialog_header ) ) {
                str = WRConvertStringFrom(
                    GETHDR_CAPTION( ditem->dialog_info->dialog_header ), "\t\n\"", "tn\"" );
                if( str != NULL ) {
                    fprintf( fp, "CAPTION \"%s\"\n", str );
                    WRMemFree( str );
                    str = NULL;
                }
            }
        }

        ExStyle = 0;
#if __NT__XX
        ExStyle = GETHDR_EXSTYLE( ditem->dialog_info->dialog_header );
        WdeSetEXFlagText( ExStyle, &str );
        if( str != NULL ) {
            fprintf( fp, "EXSTYLE %s\n", str );
            WRMemFree( str );
            str = NULL;
        }
#endif

        rname = GETHDR_MENUNAME( ditem->dialog_info->dialog_header );
        if( rname != NULL ) {
            str = WdeResNameOrOrdinalToStr( rname, 10 );
            if( str != NULL ) {
                if( *str != '\0' ) {
                    fprintf( fp, "MENU %s\n", str );
                }
                WRMemFree( str );
                str = NULL;
            }
        }
        if( (style & DS_SETFONT) &&
            GETHDR_FONTNAME( ditem->dialog_info->dialog_header ) != NULL ) {
            fprintf( fp, "FONT %d, \"%s\"\n",
                     GETHDR_POINTSIZE( ditem->dialog_info->dialog_header ),
                     GETHDR_FONTNAME( ditem->dialog_info->dialog_header ) );
        }
    }

    return( ok );
}

char *WdeConstructDLGInclude( WdeResInfo *rinfo )
{
    char        *include;
    int         len;

    if( rinfo == NULL || rinfo->sym_name == NULL ) {
        return( NULL );
    }

    len = strlen( rinfo->sym_name );
    len += 1 + 1 + 2 + 2; // for a nullchar, tab, 2 double quotes, and "\0"
    include = WRMemAlloc( len );
    if( include == NULL ) {
        return( NULL );
    }
    strcpy( include, "\t\"" );
    strcat( include, rinfo->sym_name );
    strcat( include, "\\0\"" );

    return( include );
}

#if 0
static void WdeWriteDLGInclude( WdeResInfo *rinfo, FILE *fp )
{
    char        *include;

    if( rinfo == NULL || rinfo->sym_name == NULL || fp == NULL ) {
        return;
    }

    include = WdeConstructDLGInclude( rinfo );
    if( include == NULL ) {
        return;
    }

    fwrite( "DLGINCLUDE RCDATA DISCARDABLE\n", sizeof( char ), 30, fp );
    fwrite( "BEGIN\n", sizeof( char ), 6, fp );
    fwrite( include, sizeof( char ), strlen( include ), fp );
    fwrite( "\nEND\n\n", sizeof( char ), 6, fp );

    WRMemFree( include );
}
#endif

bool WdeSaveDlgItemToRC( WdeResInfo *rinfo, WdeResDlgItem *ditem, FILE *fp )
{
    bool                ok;
    bool                wrote_begin;
    LIST                *clist;
    WdeDialogBoxControl *control;
    uint_16             nlen;
    char                *ctext;

    wrote_begin = FALSE;
    ok = (rinfo != NULL && ditem != NULL && ditem->dialog_info != NULL && fp != NULL);

    if( ok ) {
        ok = WdeWriteDlgHeader( rinfo, ditem, fp );
    }

    if( ok ) {
        // find the longest control text
        nlen = 0;
        ctext = NULL;
        clist = ditem->dialog_info->control_list;
        while( clist != NULL ) {
            control = (WdeDialogBoxControl *)ListElement( clist );
            if( GETCTL_TEXT( control ) != NULL ) {
                ctext = WdeResNameOrOrdinalToStr( GETCTL_TEXT( control ), 10 );
                if( ctext != NULL ) {
                    if( strlen( ctext ) > nlen ) {
                        nlen = strlen( ctext );
                    }
                    WRMemFree( ctext );
                    ctext = NULL;
                }
            }
            clist = ListNext( clist );
        }

        fwrite( "BEGIN\n", sizeof( char ), 6, fp );
        wrote_begin = TRUE;
        clist = ditem->dialog_info->control_list;
        while( ok && clist != NULL ) {
            control = (WdeDialogBoxControl *)ListElement( clist );
            ok = WdeWriteDlgControl( rinfo, control,
                                     ditem->dialog_info->dialog_header->is32bitEx,
                                     fp, nlen );
            clist = ListNext( clist );
        }
    }

    if( wrote_begin ) {
        fwrite( "END\n\n", sizeof( char ), 5, fp );
    }

    return( ok );
}

bool WdeCreateItemDBI( WdeResInfo *rinfo, WdeResDlgItem *ditem )
{
    if( rinfo == NULL || ditem == NULL ) {
        return( FALSE );
    }

    if( ditem->object != NULL ) {
        if( WdeGetItemDBI( ditem ) == NULL ) {
            return( FALSE );
        }
    } else if( ditem->dialog_info == NULL ) {
        ditem->dialog_info = WdeLoadDialogFromRes( rinfo, ditem->lnode, ditem->is32bit );
        if( ditem->dialog_info == NULL ) {
            return( FALSE );
        }
        if( ditem->dialog_name == NULL ) {
            ditem->dialog_name = WdeCopyWResID( &ditem->rnode->Info.ResName );
            if( ditem->dialog_name == NULL ) {
                return( FALSE );
            }
        }
    }

    return( TRUE );
}

bool WdeSaveResInfoToRC( char *filename, WdeResInfo *rinfo, bool append )
{
    FILE                *fp;
    bool                ok;
    LIST                *dlist;
    WdeResDlgItem       *ditem;

    fp = NULL;
    ok = (rinfo != NULL && rinfo->info != NULL && filename != NULL);

    if( ok ) {
        if( append ) {
            fp = fopen( filename, "at" );
        } else {
            fp = fopen( filename, "wt" );
        }
        ok = (fp != NULL);
        dlist = rinfo->dlg_item_list;
    }

#if 0
    if( ok ) {
      WdeWriteDLGInclude( rinfo, fp );
    }
#endif

    while( ok && dlist != NULL ) {
        ditem = (WdeResDlgItem *)ListElement( dlist );
        ok = WdeCreateItemDBI( rinfo, ditem );
        ok = ok && WdeSaveDlgItemToRC( rinfo, ditem, fp );
        dlist = ListNext( dlist );
    }

    if( fp != NULL ) {
        fclose( fp );
    }

    return( ok );
}

bool WdeSaveObjectToRC( char *filename, WdeResInfo *rinfo,
                        WdeResDlgItem *ditem, bool append )
{
    FILE                *fp;
    bool                ok;

    fp = NULL;
    ok = (rinfo != NULL && ditem != NULL && filename != NULL);

    if( ok ) {
        if( append ) {
            fp = fopen( filename, "at" );
        } else {
            fp = fopen( filename, "wt" );
        }
        ok = (fp != NULL);
    }

    if( ok ) {
        ok = WdeSaveDlgItemToRC( rinfo, ditem, fp );
    }

    if( fp ) {
        fclose( fp );
    }

    return( ok );
}

bool WdeSaveRC( char *filename, WdeResInfo *res_info )
{
    bool        ok;

    ok = (filename != NULL && res_info != NULL);

    if( ok && res_info->info != NULL && res_info->info->file_name != NULL ) {
        ok = WRUpdateTmp( res_info->info );
    }

    if( ok ) {
        ok = WdeSaveResInfoToRC( filename, res_info, FALSE );
    }

    return( ok );
}
