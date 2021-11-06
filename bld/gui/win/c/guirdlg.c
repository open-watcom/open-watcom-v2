/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "guiwind.h"
#include "guicontr.h"
#include "guixdlg.h"
#include "guirdlg.h"
#include "guixwind.h"
#include "wclbproc.h"
#ifdef __NT__
    #undef _WIN32_IE
    #define _WIN32_IE   0x0400
    #include <commctrl.h>
#endif
#include "oswincls.h"


/* Local Window callback functions prototypes */
WINEXPORT BOOL CALLBACK InsertResDlgCntlFunc( HWND hwnd, LPARAM lparam );

WPI_INST                        GUIResHInst;

typedef struct GetClassMap {
    gui_control_class   control_class;
    const char          *osclassname;
    DWORD               style;
    DWORD               mask;
} GetClassMap;

#ifdef __OS2_PM__
// note: the order of entries this table is important
static GetClassMap Map[] =
{
    { GUI_RADIO_BUTTON,     WC_SYS_BUTTON,           BS_RADIOBUTTON,     0xf             }
,   { GUI_CHECK_BOX,        WC_SYS_BUTTON,           BS_CHECKBOX,        0xf             }
,   { GUI_DEFPUSH_BUTTON,   WC_SYS_BUTTON,           BS_DEFAULT,         BS_DEFAULT      }
,   { GUI_PUSH_BUTTON,      WC_SYS_BUTTON,           0xffff,             0xffff          }
,   { GUI_GROUPBOX,         WC_SYS_STATIC,           SS_GROUPBOX,        SS_GROUPBOX     }
,   { GUI_STATIC,           WC_SYS_STATIC,           0xffff,             0xffff          }
,   { GUI_EDIT_COMBOBOX,    WC_SYS_COMBOBOX,         CBS_DROPDOWN,       CBS_DROPDOWN    }
,   { GUI_EDIT_COMBOBOX,    WC_SYS_COMBOBOX,         CBS_SIMPLE,         CBS_SIMPLE      }
,   { GUI_COMBOBOX,         WC_SYS_COMBOBOX,         0xffff,             0xffff          }
,   { GUI_EDIT,             WC_SYS_ENTRYFIELD,       0xffff,             0xffff          }
,   { GUI_EDIT_MLE,         WC_SYS_MLE,              0xffff,             0xffff          }
,   { GUI_LISTBOX,          WC_SYS_LISTBOX,          0xffff,             0xffff          }
,   { GUI_SCROLLBAR,        WC_SYS_SCROLLBAR,        0xffff,             0xffff          }
};
#else
// note: the order of entries this table is important
static GetClassMap Map[] =
{
    { GUI_GROUPBOX,         WC_BUTTON,      BS_GROUPBOX,        BS_GROUPBOX         }
,   { GUI_RADIO_BUTTON,     WC_BUTTON,      BS_RADIOBUTTON,     BS_RADIOBUTTON      }
,   { GUI_CHECK_BOX,        WC_BUTTON,      BS_CHECKBOX,        BS_CHECKBOX         }
,   { GUI_DEFPUSH_BUTTON,   WC_BUTTON,      BS_DEFPUSHBUTTON,   BS_DEFPUSHBUTTON    }
,   { GUI_PUSH_BUTTON,      WC_BUTTON,      0xffff,             0xffff              }
,   { GUI_COMBOBOX,         WC_COMBOBOX,    CBS_DROPDOWNLIST,   CBS_DROPDOWNLIST    }
,   { GUI_EDIT_COMBOBOX,    WC_COMBOBOX,    CBS_DROPDOWN,       CBS_DROPDOWN        }
,   { GUI_EDIT_COMBOBOX,    WC_COMBOBOX,    0xffff,             0xffff              }
,   { GUI_EDIT_MLE,         WC_EDIT,        ES_MULTILINE,       ES_MULTILINE        }
,   { GUI_EDIT,             WC_EDIT,        0xffff,             0xffff              }
,   { GUI_LISTBOX,          WC_LISTBOX,     0xffff,             0xffff              }
,   { GUI_SCROLLBAR,        WC_SCROLLBAR,   0xffff,             0xffff              }
,   { GUI_STATIC,           WC_STATIC,      0xffff,             0xffff              }
};
#endif

gui_control_class GUIGetControlClassFromHWND( HWND cntl )
{
    gui_control_class   control_class;
    char                osclassname[GUI_CLASSNAME_MAX + 1];
    DWORD               style;
    int                 index;

    control_class = GUI_BAD_CLASS;
    if( _wpi_getclassname( cntl, osclassname, sizeof( osclassname ) ) ) {
        style = _wpi_getwindowlong( cntl, GWL_STYLE );
        for( index = 0; ( index < GUI_ARRAY_SIZE( Map ) ) && ( control_class == GUI_BAD_CLASS ); index++ ) {
            if( stricmp( Map[index].osclassname, osclassname ) == 0 ) {
                if( Map[index].mask == 0xffff ) {
                    control_class = Map[index].control_class;
                } else if( (style & Map[index].mask) == Map[index].style ) {
                    control_class = Map[index].control_class;
                }
            }
        }
    }
    return( control_class );
}

gui_control_styles GUIGetControlStylesFromHWND( HWND cntl, gui_control_class control_class )
{
    gui_control_styles  styles;
    DWORD               style;

    styles = GUI_STYLE_CONTROL_NOSTYLE;
    style = _wpi_getwindowlong( cntl, GWL_STYLE );

    if( style & WS_TABSTOP ) {
        styles |= GUI_STYLE_CONTROL_TAB_GROUP;
    }

    switch( control_class ) {
    case GUI_CHECK_BOX:
        if( ( style & BS_3STATE ) == BS_3STATE ) {
            styles |= GUI_STYLE_CONTROL_3STATE;
        }
        break;
    case GUI_LISTBOX:
        if( style & LBS_NOINTEGRALHEIGHT ) {
            styles |= GUI_STYLE_CONTROL_NOINTEGRALHEIGHT;
        }
        if( style & LBS_SORT ) {
            styles |= GUI_STYLE_CONTROL_SORTED;
        }
        break;
    case GUI_STATIC:
        if( style & SS_NOPREFIX ) {
            styles |= GUI_STYLE_CONTROL_NOPREFIX;
        }
        if( ( style & SS_CENTER ) == SS_CENTER ) {
            styles |= GUI_STYLE_CONTROL_CENTRE;
        }
        if( ( style & SS_LEFTNOWORDWRAP ) == SS_LEFTNOWORDWRAP ) {
            styles |= GUI_STYLE_CONTROL_LEFTNOWORDWRAP;
        }
        break;
    case GUI_EDIT_COMBOBOX:
    case GUI_COMBOBOX:
        if( style & CBS_NOINTEGRALHEIGHT ) {
            styles |= GUI_STYLE_CONTROL_NOINTEGRALHEIGHT;
        }
        if( style & CBS_SORT ) {
            styles |= GUI_STYLE_CONTROL_SORTED;
        }
        break;
    case GUI_EDIT:
    case GUI_EDIT_MLE:
        if( style & ES_MULTILINE ) {
            styles |= GUI_STYLE_CONTROL_MULTILINE;
        }
        if( style & ES_WANTRETURN ) {
            styles |= GUI_STYLE_CONTROL_WANTRETURN;
        }
        if( style & ES_READONLY ) {
            styles |= GUI_STYLE_CONTROL_READONLY;
        }
#ifdef __OS2_PM__
        if( style & MLS_READONLY ) {
            styles |= GUI_STYLE_CONTROL_READONLY;
        }
#endif
        break;
    }

    return( styles );
}

BOOL CALLBACK InsertResDlgCntlFunc( HWND hwnd, LPARAM lparam )
{
    GUIControlInsertByHWND( hwnd, (gui_window *)lparam );
    return( TRUE );
}

bool GUIInsertResDialogControls( gui_window *wnd )
{
#ifdef __OS2_PM__
    WPI_ENUMPROC        wndenumproc;

    wndenumproc = _wpi_makeenumprocinstance( InsertResDlgCntlFunc, GUIMainHInst );
    _wpi_enumchildwindows( wnd->hwnd, wndenumproc, (LPARAM)wnd );
    _wpi_freeenumprocinstance( wndenumproc );
#else
    WNDENUMPROC         wndenumproc;

    wndenumproc = MakeProcInstance_WNDENUM( InsertResDlgCntlFunc, GUIMainHInst );
    EnumChildWindows( wnd->hwnd, wndenumproc, (LPARAM)wnd );
    FreeProcInstance_WNDENUM( wndenumproc );
#endif
    return( true );
}

bool GUICreateDialogFromRes( res_name_or_id dlg_id, gui_window *parent_wnd, GUICALLBACK *gui_call_back, void *extra )
{
#ifdef __OS2_PM__
    WPI_DLGPROC     dlgproc;
#else
    DLGPROC         dlgproc;
#endif
    HWND            parent_hwnd;
    bool            ok;

    /* unused parameters */ (void)gui_call_back;

    parent_hwnd = parent_wnd->hwnd;
    if( parent_hwnd == NULLHANDLE )
        parent_hwnd = HWND_DESKTOP;
#ifdef __OS2_PM__
    dlgproc = _wpi_makedlgprocinstance( GUIDialogDlgProc, GUIMainHInst );
    ok = ( dlgproc != NULL );
    if( ok ) {
        ok = ( _wpi_dialogbox( parent_hwnd, dlgproc, GUIResHInst, dlg_id, extra ) != -1 );
        _wpi_freedlgprocinstance( dlgproc );
    }
#else
    dlgproc = MakeProcInstance_DLG( GUIDialogDlgProc, GUIMainHInst );
    ok = ( dlgproc != NULL );
    if( ok ) {
        ok = ( DialogBoxParam( GUIResHInst, dlg_id, parent_hwnd, dlgproc, (LPARAM)extra ) != -1 );
        FreeProcInstance_DLG( dlgproc );
    }
#endif
    return( ok );
}

