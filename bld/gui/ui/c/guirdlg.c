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
#include <string.h>
#include <stdlib.h>
#include "watcom.h"
#include "guiutil.h"
#include "guiscale.h"
#include "wressetr.h"
#include "filefmt.h"
#include "resdiag.h"
#include "resmenu.h"
#include "wresdefn.h"
#include "guildstr.h"
#include "guirdlg.h"

#include "clibext.h"


#define DLG_X_MULT      4
#define DLG_Y_MULT      14

void GUIFreeDialogBoxControlPtrs( DialogBoxControl *dbc )
{
    if( dbc ) {
        if( dbc->ClassID != NULL ) {
            GUIMemFree( dbc->ClassID );
            dbc->ClassID = NULL;
        }
        if( dbc->Text != NULL ) {
            GUIMemFree( dbc->Text );
            dbc->Text = NULL;
        }
    }
}

void GUIFreeDialogBoxHeader( DialogBoxHeader *hdr )
{
    if( hdr != NULL ) {
        if( hdr->MenuName != NULL ) {
            GUIMemFree( hdr->MenuName );
            hdr->MenuName = NULL;
        }
        if( hdr->ClassName != NULL ) {
            GUIMemFree( hdr->ClassName );
            hdr->ClassName = NULL;
        }
        if( hdr->Caption != NULL ) {
            GUIMemFree( hdr->Caption );
            hdr->Caption = NULL;
        }
        if( hdr->FontName != NULL ) {
            GUIMemFree( hdr->FontName );
            hdr->FontName = NULL;
        }
        GUIMemFree( hdr );
    }
}

static char *ResNameOrOrdinalToStr( ResNameOrOrdinal *name, int base )
{
    char        temp[15];
    char        *cp;
    bool        ok;

    cp = NULL;

    if( name != NULL ) {
        if( name->ord.fFlag == 0xff) {
            sprintf( temp, ( base == 10 ) ? "%d" : "%x", name->ord.wOrdinalID );
            cp = GUIStrDup( temp, &ok );
        } else {
            cp = GUIStrDup( name->name, &ok );
        }
        if( !ok || ( cp == NULL ) ) {
            return( NULL );
        }
    }

    return( cp );
}

static bool Template2Dlg( DialogBoxHeader **hdr, DialogBoxControl **cntls )
{
    bool        ok;
    int         i;

    ok = ( hdr != NULL && cntls != NULL );

    if( ok ) {
        *cntls = NULL;
        *hdr = (DialogBoxHeader *)GUIMemAlloc( sizeof(DialogBoxHeader) );
        ok = ( *hdr != NULL );
    }

    if( ok ) {
        ok = !GUIResReadDialogBoxHeader( *hdr );
    }

    if( ok ) {
        *cntls = (DialogBoxControl *)GUIMemAlloc( (*hdr)->NumOfItems * sizeof( DialogBoxControl ) );
        ok = ( *cntls != NULL );
    }

    if( ok ) {
        for( i = 0; i < (*hdr)->NumOfItems; i++ ) {
            ok = !GUIResReadDialogBoxControl( *cntls + i );
            if( !ok ) {
                break;
            }
        }
    }

    if( !ok ) {
        if( *hdr != NULL ) {
            GUIFreeDialogBoxHeader( *hdr );
            *hdr = NULL;
        }
        if( *cntls != NULL ) {
            for( i = 0; i < (*hdr)->NumOfItems; i++ ) {
                GUIFreeDialogBoxControlPtrs( *cntls + i );
            }
            GUIMemFree( *cntls );
            *cntls = NULL;
        }
    }

    return( ok );
}

static gui_control_styles GetControlStyles( DialogBoxControl *ctl, gui_control_class control_class )
{
    gui_control_styles  styles;

    styles = GUI_STYLE_CONTROL_NOSTYLE;

    if( ctl->Style & WS_TABSTOP ) {
        styles |= GUI_STYLE_CONTROL_TAB_GROUP;
    }

    switch( control_class ) {
    case GUI_CHECK_BOX:
        styles |= GUI_STYLE_CONTROL_GROUP;
        if( ( ctl->Style & 0xf ) == BS_3STATE ) {
            styles |= GUI_STYLE_CONTROL_3STATE;
        }
        if( ( ctl->Style & 0xf ) == BS_AUTO3STATE ) {
            styles |= GUI_STYLE_CONTROL_3STATE | GUI_STYLE_CONTROL_AUTOMATIC;
        }
        break;
    case GUI_RADIO_BUTTON:
        if( ( ctl->Style & 0xf ) == BS_AUTORADIOBUTTON ) {
            styles |= GUI_STYLE_CONTROL_AUTOMATIC;
        }
        break;
    case GUI_LISTBOX:
        if( ctl->Style & LBS_NOINTEGRALHEIGHT ) {
            styles |= GUI_STYLE_CONTROL_NOINTEGRALHEIGHT;
        }
        if( ctl->Style & LBS_SORT ) {
            styles |= GUI_STYLE_CONTROL_SORTED;
        }
        break;
    case GUI_STATIC:
        if( ctl->Style & SS_NOPREFIX ) {
            styles |= GUI_STYLE_CONTROL_NOPREFIX;
        }
        if( ( ctl->Style & 0xf ) == SS_CENTER ) {
            styles |= GUI_STYLE_CONTROL_CENTRE;
        }
        if( ( ctl->Style & 0xf ) == SS_LEFTNOWORDWRAP ) {
            styles |= GUI_STYLE_CONTROL_LEFTNOWORDWRAP;
        }
        break;
    case GUI_EDIT_COMBOBOX:
    case GUI_COMBOBOX:
        if( ctl->Style & CBS_NOINTEGRALHEIGHT ) {
            styles |= GUI_STYLE_CONTROL_NOINTEGRALHEIGHT;
        }
        if( ctl->Style & CBS_SORT ) {
            styles |= GUI_STYLE_CONTROL_SORTED;
        }
        break;
    case GUI_EDIT:
    case GUI_EDIT_MLE:
        if( ctl->Style & ES_MULTILINE ) {
            styles |= GUI_STYLE_CONTROL_MULTILINE;
        }
        if( ctl->Style & ES_WANTRETURN ) {
            styles |= GUI_STYLE_CONTROL_WANTRETURN;
        }
        break;
    default:
        break;
    }

    return( styles );
}

#define CHK_BSTYLE(s,m) ( ( s & m ) == m )

static gui_control_class GetControlClass( DialogBoxControl *ctl )
{
    gui_control_class   control_class;

    control_class = GUI_BAD_CLASS;

    if( ctl != NULL && ctl->ClassID && (ctl->ClassID->Class & 0x80) ) {
        switch( ctl->ClassID->Class ) {
        case CLASS_BUTTON:
            control_class = GUI_PUSH_BUTTON;
            if( CHK_BSTYLE( ctl->Style, BS_GROUPBOX ) ) {
                control_class = GUI_GROUPBOX;
            } else if( CHK_BSTYLE( ctl->Style, BS_AUTORADIOBUTTON ) ||
                       CHK_BSTYLE( ctl->Style, BS_RADIOBUTTON ) ) {
                control_class = GUI_RADIO_BUTTON;
            } else if( CHK_BSTYLE( ctl->Style, BS_AUTOCHECKBOX ) ||
                       CHK_BSTYLE( ctl->Style, BS_CHECKBOX ) ||
                       CHK_BSTYLE( ctl->Style, BS_3STATE ) ||
                       CHK_BSTYLE( ctl->Style, BS_AUTO3STATE ) ) {
                control_class = GUI_CHECK_BOX;
            } else if( CHK_BSTYLE( ctl->Style, BS_DEFPUSHBUTTON ) ) {
                control_class = GUI_DEFPUSH_BUTTON;
            }
            break;
        case CLASS_EDIT:
            control_class = GUI_EDIT;
            if( ctl->Style & ES_MULTILINE ) {
                control_class = GUI_EDIT_MLE;
            }
            break;
        case CLASS_STATIC:
            control_class = GUI_STATIC;
            break;
        case CLASS_LISTBOX:
            control_class = GUI_LISTBOX;
            break;
        case CLASS_SCROLLBAR:
            control_class = GUI_SCROLLBAR;
            break;
        case CLASS_COMBOBOX:
            control_class = GUI_EDIT_COMBOBOX;
            if( ctl->Style & CBS_DROPDOWNLIST ) {
                control_class = GUI_COMBOBOX;
            }
            break;
        }
    }

    return( control_class );
}

static bool DialogBoxControl2GUI( DialogBoxControl *ctl, gui_control_info *ctl_info )
{
    SAREA               area;
    bool                ok;

    ok = ( ctl && ctl_info != NULL );

    if( ok ) {
        // initialize the create struct
        memset( ctl_info, 0, sizeof( gui_control_info ) );

        // set the control class
        ctl_info->control_class = GetControlClass( ctl );
        ok = ( ctl_info->control_class != GUI_BAD_CLASS );
    }

    if( ok ) {
        // set the control style
        ctl_info->style = GetControlStyles( ctl, ctl_info->control_class );
    }

    if( ok ) {
        // set the initial text. NULL is ok
        ctl_info->text = ResNameOrOrdinalToStr( ctl->Text, 10 );

        // set the control id
        ctl_info->id = ctl->ID;

        // set the scroll styles
        ctl_info->scroll_style = GUI_NOSCROLL;
        if( ctl->Style & WS_HSCROLL ) {
            ctl_info->scroll_style |= GUI_HSCROLL;
        }
        if( ctl->Style & WS_VSCROLL ) {
            ctl_info->scroll_style |= GUI_VSCROLL;
        }

        // set the control postion
        area.row = ctl->SizeInfo.y / DLG_Y_MULT;
        area.col = ctl->SizeInfo.x / DLG_X_MULT;
        area.width = ( ( ctl->SizeInfo.width + DLG_X_MULT/2 ) / DLG_X_MULT );
        if( area.width < 1 )
            area.width = 1;
        area.height = ( ( ctl->SizeInfo.height + DLG_Y_MULT/2 ) / DLG_Y_MULT );
        if( area.height < 1 )
            area.height = 1;
        ok = GUIScreenToScaleRectR( &area, &ctl_info->rect );
    }

    if( !ok ) {
        if( ctl_info != NULL ) {
            if( ctl_info->text != NULL ) {
                GUIMemFree( (void *)ctl_info->text );
            }
        }
    }

    return( ok );
}

static gui_create_info *DialogBoxHeader2GUI( DialogBoxHeader *hdr )
{
    gui_create_info     *dlg_info;
    SAREA               area;
    SAREA               bounding;
    bool                ok;

    if( hdr == NULL )
        return( NULL ) ;
    dlg_info = (gui_create_info *)GUIMemAlloc( sizeof( gui_create_info ) );
    if( dlg_info == NULL )
        return( NULL ) ;

    // initialize the create struct
    memset( dlg_info, 0, sizeof( gui_create_info ) );

    // set the initial text
    dlg_info->title = hdr->Caption; // NULL text is ok

    // set the dialog postion remembering to add the size of the frame
    GUIGetScreenArea( &bounding );
    area.width = hdr->SizeInfo.width / DLG_X_MULT + 2;
    area.height = hdr->SizeInfo.height / DLG_Y_MULT + 2;
    area.row = 0;
    if( bounding.height > area.height )
        area.row = ( bounding.height - area.height ) / 2;
    area.col = 0;
    if( bounding.width > area.width )
        area.col = ( bounding.width - area.width ) / 2;
    ok = GUIScreenToScaleRect( &area, &dlg_info->rect );

    if( ok ) {
        // set the scroll styles
        dlg_info->scroll_style = GUI_NOSCROLL;
        if( hdr->Style & WS_HSCROLL ) {
            dlg_info->scroll_style |= GUI_HSCROLL;
        }
        if( hdr->Style & WS_VSCROLL ) {
            dlg_info->scroll_style |= GUI_VSCROLL;
        }
        // set the window styles
        dlg_info->style = GUI_NONE;
        if( hdr->Style & WS_THICKFRAME ) {
            dlg_info->style = GUI_RESIZEABLE;
        }
        if( hdr->Style & WS_THICKFRAME ) {
            dlg_info->style = GUI_RESIZEABLE;
        }
        if( hdr->Style & WS_MINIMIZEBOX ) {
            dlg_info->style = GUI_MINIMIZE;
        }
        if( hdr->Style & WS_MAXIMIZEBOX ) {
            dlg_info->style = GUI_MAXIMIZE;
        }
        if( hdr->Style & WS_SYSMENU ) {
            dlg_info->style = GUI_SYSTEM_MENU;
        }
        if( hdr->Style & WS_POPUP ) {
            dlg_info->style = GUI_POPUP;
        }
    }

    if( !ok ) {
        if( dlg_info != NULL ) {
            GUIMemFree( dlg_info );
            dlg_info = NULL;
        }
    }

    return( dlg_info );
}

bool GUICreateDialogFromRes( res_name_or_id dlg_id, gui_window *parent_wnd, GUICALLBACK *gui_call_back, void *extra )
{
    DialogBoxHeader     *hdr;
    DialogBoxControl    *cntls;
    gui_create_info     *dlg_info;
    gui_control_info    *controls_info;
    int                 i;
    int                 last_was_radio;
    bool                ok;

    hdr = NULL;
    cntls = NULL;
    dlg_info = NULL;
    controls_info = NULL;

    ok = ( gui_call_back != NULL );

    if( ok ) {
        ok = GUISeekDialogTemplate( dlg_id );
    }

    if( ok ) {
        ok = Template2Dlg( &hdr, &cntls );
    }

    if( ok ) {
        dlg_info = DialogBoxHeader2GUI( hdr );
        ok = ( dlg_info != NULL );
    }

    if( ok ) {
        controls_info = (gui_control_info *)GUIMemAlloc( sizeof( gui_control_info ) * hdr->NumOfItems );
        ok = ( controls_info != NULL );
    }

    last_was_radio = -1;
    if( ok ) {
        memset( controls_info, 0, sizeof( gui_control_info ) * hdr->NumOfItems );
        for( i = 0; i < hdr->NumOfItems; i++ ) {
            ok = DialogBoxControl2GUI( &cntls[i], &controls_info[i] );
            if( !ok )
                break;
            if( controls_info[i].control_class == GUI_RADIO_BUTTON ) {
                if( last_was_radio != 1 ) {
                    controls_info[i].style |= GUI_STYLE_CONTROL_GROUP;
                }
                last_was_radio = 1;
            } else {
                if( last_was_radio == 1 ) {
                    controls_info[i - 1].style |= GUI_STYLE_CONTROL_GROUP;
                }
                last_was_radio = 0;
            }
        }
    }

    if( ok ) {
        dlg_info->parent = parent_wnd;
        dlg_info->gui_call_back = gui_call_back;
        dlg_info->extra = extra;
        ok = GUICreateDialog( dlg_info, hdr->NumOfItems, controls_info );
    }

    if( controls_info != NULL ) {
        for( i = 0; i < hdr->NumOfItems; i++ ) {
            GUIMemFree( (void *)controls_info[i].text );
        }
        GUIMemFree( controls_info );
    }

    if( dlg_info != NULL ) {
        GUIMemFree( dlg_info );
    }

    if( cntls != NULL ) {
        for( i = 0; i < hdr->NumOfItems; i++ ) {
            GUIFreeDialogBoxControlPtrs( &cntls[i] );
        }
        GUIMemFree( cntls );
    }

    if( hdr != NULL ) {
        GUIFreeDialogBoxHeader( hdr );
    }
    return( ok );
}
