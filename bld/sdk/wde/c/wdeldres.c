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


#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <io.h>

#include "wdeglbl.h"
#include "wderesin.h"
#include "wde_wres.h"
#include "wdemem.h"
#include "wdedebug.h"
#include "wdesdup.h"
#include "wdetfile.h"
#include "wdewait.h"
#include "wdeldres.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WResTypeNode *WdeFindTypeNode          ( WResDir, uint_16, char * );

WdeResInfo *WdeLoadResource( const char *file_name )
{
    WdeResInfo  *res_info;
    WRFileType   file_type;
    Bool         ok;

    WdeSetWaitCursor ( TRUE );

    ok = ( ( res_info = WdeAllocResInfo() ) != NULL );

    if ( ok ) {
        file_type = WRIdentifyFile ( file_name );
        ok = ( file_type != WR_INVALID_FILE );
    }

    if ( ok ) {
        res_info->info = WRLoadResource ( file_name, file_type );
        ok = ( res_info->info != NULL );
    }

    if ( ok ) {
        res_info->dlg_entry =
            WdeFindTypeNode ( res_info->info->dir, (uint_16) RT_DIALOG,
                              "DIALOG" );
    }

    if ( !ok ) {
        if ( res_info ) {
            WdeFreeResInfo ( res_info );
            res_info = NULL;
        }
    }

    WdeSetWaitCursor ( FALSE );

    return ( res_info );
}


WdeDialogBoxInfo *WdeLoadDialogFromRes( WdeResInfo   *res_info,
                                        WResLangNode *lnode, Bool is32bit )
{
    DialogExHeader32    h32ex;
    DialogBoxHeader32   h32;
    DialogBoxHeader     h16;

    DialogBoxControl    c16;
    DialogBoxControl32  c32;
    DialogBoxExControl32 c32ex;

    WdeDialogBoxInfo    *dlg_info;
    WResFileID          file;
    WdeDialogBoxControl *control;
    LIST                *prev_control;
    #if 0
    WdeDialogBoxControl *nc;
    LIST                *clist;
    #endif
    int                 index;
    char                *file_name;
    Bool                ok;

    dlg_info = NULL;
    file     = -1;

    ok = ( res_info && lnode );

    if ( ok ) {
        file_name = res_info->info->tmp_file;
        dlg_info = (WdeDialogBoxInfo *)
            WdeMemAlloc ( sizeof(WdeDialogBoxInfo) );
        ok = ( dlg_info != NULL );
    }


    if ( ok ) {
        dlg_info->dialog_header = WdeAllocDialogBoxHeader();
        ok = ( dlg_info->dialog_header != NULL );
    }

    if ( ok ) {
        dlg_info->dialog_header->is32bit = is32bit;
        dlg_info->control_list  = NULL;
        dlg_info->MemoryFlags   = 0;
        file = ResOpenFileRO( file_name );
        ok = ( file != -1 );
    }

    if( ok ) {
        dlg_info->MemoryFlags = lnode->Info.MemoryFlags;
        ok = ( lseek ( file, lnode->Info.Offset, SEEK_SET ) != -1 );
    }

    if( ok ) {
        if( is32bit ) {
            /* JPK - check if its an extended dialog */
            dlg_info->dialog_header->is32bitEx = ResIsDialogEx( file );
            lseek ( file, lnode->Info.Offset, SEEK_SET );

            if ( dlg_info->dialog_header->is32bitEx ) {
                ok = ( !ResReadDialogExHeader32( &h32, &h32ex, file ) );
            } else {
                ok = ( !ResReadDialogBoxHeader32( &h32, file ) );
            }
        } else {
            ok = ( !ResReadDialogBoxHeader( &h16, file ) );
        }
    }

    if( ok ) {
        if( is32bit ) {
            if( dlg_info->dialog_header->is32bitEx ) {
                dlg_info->dialog_header->FontWeight = h32ex.FontWeight;
                dlg_info->dialog_header->FontItalic = h32ex.FontItalic;
                dlg_info->dialog_header->HelpId = h32ex.HelpId;
                dlg_info->dialog_header->FontWeightDefined =
                    ( h32ex.FontWeightDefined != 0 );
                dlg_info->dialog_header->FontItalicDefined =
                    ( h32ex.FontItalicDefined != 0 );
            }
            dlg_info->dialog_header->Style = h32.Style;
            dlg_info->dialog_header->ExtendedStyle = h32.ExtendedStyle;
            dlg_info->dialog_header->NumOfItems = h32.NumOfItems;
            dlg_info->dialog_header->Size = h32.Size;
            dlg_info->dialog_header->MenuName = h32.MenuName;
            dlg_info->dialog_header->ClassName = h32.ClassName;
            dlg_info->dialog_header->Caption = h32.Caption;
            dlg_info->dialog_header->PointSize = h32.PointSize;
            dlg_info->dialog_header->FontName = h32.FontName;
        } else {
            dlg_info->dialog_header->Style = h16.Style;
            dlg_info->dialog_header->NumOfItems = h16.NumOfItems;
            dlg_info->dialog_header->Size = h16.Size;
            dlg_info->dialog_header->MenuName = h16.MenuName;
            dlg_info->dialog_header->ClassName = h16.ClassName;
            dlg_info->dialog_header->Caption = h16.Caption;
            dlg_info->dialog_header->PointSize = h16.PointSize;
            dlg_info->dialog_header->FontName = h16.FontName;
        }

        prev_control = NULL;
        for ( index=0; index<GETHDR_NUMITEMS(dlg_info->dialog_header); index++ ) {
            control = WdeAllocDialogBoxControl();
            if( control == NULL ) {
                ok = FALSE;
                break;
            }
            if( is32bit ) {
                /*
                 * JPK - check which control structure to expect based on
                 *       whether this an extended dialog or not
                */
                if ( dlg_info->dialog_header->is32bitEx ) {
                    if( ResReadDialogExControl32( &c32ex, file ) ) {
                        ok = FALSE;
                        break;
                    }
                    control->HelpId = c32ex.HelpId;
                    control->ExtendedStyle = c32ex.ExtendedStyle;
                    control->Style = c32ex.Style;
                    control->Size = c32ex.Size;
                    control->ID = c32ex.ID;
                    control->ClassID = c32ex.ClassID;
                    control->Text = c32ex.Text;
                    control->ExtraBytes = c32ex.ExtraBytes;
                } else {
                    if( ResReadDialogBoxControl32( &c32, file ) ) {
                        ok = FALSE;
                        break;
                    }
                    control->Style = c32.Style;
                    control->ExtendedStyle = c32.ExtendedStyle;
                    control->Size = c32.Size;
                    control->ID = c32.ID;
                    control->ClassID = c32.ClassID;
                    control->Text = c32.Text;
                    control->ExtraBytes = c32.ExtraBytes;
                }
            } else {
                if( ResReadDialogBoxControl( &c16, file ) ) {
                    ok = FALSE;
                    break;
                }
                control->Size = c16.Size;
                control->ID = c16.ID;
                control->Style = c16.Style;
                control->ClassID = c16.ClassID;
                control->Text = c16.Text;
                control->ExtraBytes = c16.ExtraBytes;
            }
            if ( prev_control == NULL ) {
                ListAddElt( &(dlg_info->control_list), (void *) control);
                prev_control = dlg_info->control_list;
            } else {
                ListInsertElt( prev_control, (void *) control);
                prev_control = ListNext(prev_control);
            }
        }
    }

    #if 0
    /*
     * JPK - if the dialog is 32 bit but not EX, then convert the dialog
     *       header and the control list to EX; this will force all
     *       dialogs to EX, for now
    */
    if (is32bit && !dlg_info->dialog_header->is32bitEx) {
        /* deal with the dialog header first */
        dlg_info->dialog_header->is32bitEx = TRUE;

        dlg_info->dialog_header->FontWeight        = 0;
        dlg_info->dialog_header->FontItalic        = 0;
        dlg_info->dialog_header->HelpId            = 0;
        dlg_info->dialog_header->FontWeightDefined = FALSE;
        dlg_info->dialog_header->FontItalicDefined = FALSE;

        /* now deal with the list of controls */
        nc = (WdeDialogBoxControl *)WdeMemAlloc(sizeof(WdeDialogBoxControl));
        for (clist = dlg_info->control_list; clist; clist = ListNext(clist))
        {
            control = (WdeDialogBoxControl *) ListElement( clist );
            memcpy(nc, control, sizeof(WdeDialogBoxControl));

            nc->HelpId        = 0;
            nc->ExtendedStyle = control->ExtendedStyle;
            nc->Style         = control->Style;
            memcpy(&nc->Size, &control->Size, sizeof(DialogSizeInfo));
            nc->ID            = control->ID;
            nc->ClassID       = control->ClassID;
            nc->Text          = control->Text;
            nc->ExtraBytes    = control->ExtraBytes;

            memcpy(control, nc, sizeof(WdeDialogBoxControl));
        }
        WdeMemFree(nc);
    }
    #endif

    if ( !ok ) {
        if ( dlg_info ) {
            WdeFreeDialogBoxInfo ( dlg_info );
            dlg_info = NULL;
        }
    }

    if ( file != -1 ) {
        ResCloseFile ( file );
    }

    return ( dlg_info );
}

WResTypeNode *WdeFindTypeNode ( WResDir dir, uint_16 type, char *type_name )
{
    return(  WRFindTypeNode( dir, type, type_name ) );
}

