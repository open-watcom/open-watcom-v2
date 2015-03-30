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


#include "guiwind.h"
#include "guicontr.h"

extern  WPI_INST                GUIMainHInst;
WPI_INST                        GUIResHInst;
extern  WPI_DLGRESULT CALLBACK  GUIDialogFunc( HWND hwnd, WPI_MSG message,
                                               WPI_PARAM1 wparam,
                                               WPI_PARAM2 lparam );

typedef struct GetClassMap {
    gui_control_class   control_class;
    char                *classname;
    DWORD               style;
    DWORD               mask;
} GetClassMap;

#ifdef __OS2_PM__
// note: the order of entries this table is important
static GetClassMap Map[] =
{
    { GUI_RADIO_BUTTON,         "#3",   BS_RADIOBUTTON,         0xf             }
,   { GUI_CHECK_BOX,            "#3",   BS_CHECKBOX,            0xf             }
,   { GUI_DEFPUSH_BUTTON,       "#3",   BS_DEFAULT,             BS_DEFAULT      }
,   { GUI_PUSH_BUTTON,          "#3",   0xffff,                 0xffff          }
,   { GUI_GROUPBOX,             "#5",   SS_GROUPBOX,            SS_GROUPBOX     }
,   { GUI_STATIC,               "#5",   0xffff,                 0xffff          }
,   { GUI_EDIT_COMBOBOX,        "#2",   CBS_DROPDOWN,           CBS_DROPDOWN    }
,   { GUI_EDIT_COMBOBOX,        "#2",   CBS_SIMPLE,             CBS_SIMPLE      }
,   { GUI_COMBOBOX,             "#2",   0xffff,                 0xffff          }
,   { GUI_EDIT,                 "#6",   0xffff,                 0xffff          }
,   { GUI_EDIT_MLE,             "#10",  0xffff,                 0xffff          }
,   { GUI_LISTBOX,              "#7",   0xffff,                 0xffff          }
,   { GUI_SCROLLBAR,            "#8",   0xffff,                 0xffff          }
};
#else
// note: the order of entries this table is important
static GetClassMap Map[] =
{
    { GUI_GROUPBOX,             "button",       BS_GROUPBOX,            BS_GROUPBOX             }
,   { GUI_RADIO_BUTTON,         "button",       BS_RADIOBUTTON,         BS_RADIOBUTTON          }
,   { GUI_CHECK_BOX,            "button",       BS_CHECKBOX,            BS_CHECKBOX             }
,   { GUI_DEFPUSH_BUTTON,       "button",       BS_DEFPUSHBUTTON,       BS_DEFPUSHBUTTON        }
,   { GUI_PUSH_BUTTON,          "button",       0xffff,                 0xffff                  }
,   { GUI_COMBOBOX,             "combobox",     CBS_DROPDOWNLIST,       CBS_DROPDOWNLIST        }
,   { GUI_EDIT_COMBOBOX,        "combobox",     CBS_DROPDOWN,           CBS_DROPDOWN            }
,   { GUI_EDIT_COMBOBOX,        "combobox",     0xffff,                 0xffff                  }
,   { GUI_EDIT_MLE,             "edit",         ES_MULTILINE,           ES_MULTILINE            }
,   { GUI_EDIT,                 "edit",         0xffff,                 0xffff                  }
,   { GUI_LISTBOX,              "listbox",      0xffff,                 0xffff                  }
,   { GUI_SCROLLBAR,            "scrollbar",    0xffff,                 0xffff                  }
,   { GUI_STATIC,               "static",       0xffff,                 0xffff                  }
};
#endif

#define MAP_SIZE ( sizeof( Map ) / sizeof( Map[0] ) )

gui_control_class GUIGetControlClassFromHWND( HWND cntl )
{
    gui_control_class   control_class;
    char                classname[15];
    DWORD               style;
    int                 index;

    if( !_wpi_getclassname( cntl, classname, sizeof( classname ) ) ) {
        return( GUI_BAD_CLASS );
    }

    style = _wpi_getwindowlong( cntl, GWL_STYLE );
    control_class = GUI_BAD_CLASS;

    for( index=0; ( index < MAP_SIZE ) && ( control_class == GUI_BAD_CLASS ); index++ ) {
        if(( Map[index].classname != NULL ) && !stricmp( Map[index].classname, classname ) ) {
            if( Map[index].mask == 0xffff ) {
                control_class = Map[index].control_class;
            } else {
                if( ( style & Map[index].mask ) == Map[index].style ) {
                    control_class = Map[index].control_class;
                }
            }
        }
    }
    return( control_class );
}

gui_control_styles GUIGetControlStylesFromHWND( HWND cntl,
                                                gui_control_class control_class )
{
    gui_control_styles  styles;
    DWORD               style;

    styles = GUI_NOSTYLE;
    style = _wpi_getwindowlong( cntl, GWL_STYLE );

    if( style & WS_TABSTOP ) {
        styles |= GUI_TAB_GROUP;
    }

    switch( control_class ) {
        case GUI_CHECK_BOX:
            if( ( style & BS_3STATE ) == BS_3STATE ) {
                styles |= GUI_CONTROL_3STATE;
            }
            break;
        case GUI_LISTBOX:
            if( style & LBS_NOINTEGRALHEIGHT ) {
                styles |= GUI_CONTROL_NOINTEGRALHEIGHT;
            }
            if( style & LBS_SORT ) {
                styles |= GUI_CONTROL_SORTED;
            }
            break;
        case GUI_STATIC:
            if( style & SS_NOPREFIX ) {
                styles |= GUI_CONTROL_NOPREFIX;
            }
            if( ( style & SS_CENTER ) == SS_CENTER ) {
                styles |= GUI_CONTROL_CENTRE;
            }
            if( ( style & SS_LEFTNOWORDWRAP ) == SS_LEFTNOWORDWRAP ) {
                styles |= GUI_CONTROL_LEFTNOWORDWRAP;
            }
            break;
        case GUI_EDIT_COMBOBOX:
        case GUI_COMBOBOX:
            if( style & CBS_NOINTEGRALHEIGHT ) {
                styles |= GUI_CONTROL_NOINTEGRALHEIGHT;
            }
            if( style & CBS_SORT ) {
                styles |= GUI_CONTROL_SORTED;
            }
            break;
        case GUI_EDIT:
        case GUI_EDIT_MLE:
            if( style & ES_MULTILINE ) {
                styles |= GUI_CONTROL_MULTILINE;
            }
            if( style & ES_WANTRETURN ) {
                styles |= GUI_CONTROL_WANTRETURN;
            }
            if( style & ES_READONLY ) {
                styles |= GUI_CONTROL_READONLY;
            }
#ifdef __OS2_PM__
            if( style & MLS_READONLY ) {
                styles |= GUI_CONTROL_READONLY;
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
    WPI_ENUMPROC        enum_func;

    enum_func = _wpi_makeenumprocinstance( (WPI_PROC)InsertResDlgCntlFunc, GUIMainHInst );
    _wpi_enumchildwindows( wnd->hwnd, enum_func, (LPARAM)wnd );
    _wpi_freeprocinstance( (WPI_PROC)enum_func );

    return( true );
}

#ifdef __OS2_PM__
#define DLG_RESOURCE(a) (a)
#else
#define DLG_RESOURCE(a) MAKEINTRESOURCE(a)
#endif
bool GUIDoCreateResDialog( long dlg_id, HWND parent, void *data )
{
    WPI_PROC    fp;

    fp = _wpi_makeprocinstance( (WPI_PROC)GUIDialogFunc, GUIMainHInst );
    if( !fp ) {
        return( false );
    }
    if( _wpi_dialogbox( parent, (WPI_DLGPROC)fp, GUIResHInst, DLG_RESOURCE(dlg_id), data ) == -1 ) {
        _wpi_freeprocinstance( fp );
        return( false );
    }
    _wpi_freeprocinstance( fp );

    return( true );
}

bool GUICreateDialogFromRes( gui_res_id id, gui_window *parent, GUICALLBACK cb, void *extra )
{
    id=id;
    parent=parent;
    cb=cb;
    extra=extra;
    return( false );
}

