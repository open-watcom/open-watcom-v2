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
#include <io.h>
#include "wderesin.h"
#include "wde_wres.h"
#include "wdedebug.h"
#include "wdesdup.h"
#include "wdetfile.h"
#include "wdewait.h"
#include "wdeldres.h"
#include "rcrtns.h"
#include "wresdefn.h"

#include "clibext.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WResTypeNode *WdeFindTypeNode( WResDir, uint_16, char * );

WdeResInfo *WdeLoadResource( const char *file_name )
{
    WdeResInfo  *res_info;
    WRFileType  file_type;
    bool        ok;

    file_type = 0;
    WdeSetWaitCursor( true );

    ok = ( (res_info = WdeAllocResInfo()) != NULL );

    if( ok ) {
        file_type = WRIdentifyFile( file_name );
        ok = ( file_type != WR_INVALID_FILE );
    }

    if( ok ) {
        res_info->info = WRLoadResource( file_name, file_type );
        ok = ( res_info->info != NULL );
    }

    if( ok ) {
        res_info->dlg_entry = WdeFindTypeNode( res_info->info->dir, RESOURCE2INT( RT_DIALOG ), "DIALOG" );
    }

    if( !ok ) {
        if( res_info != NULL ) {
            WdeFreeResInfo( res_info );
            res_info = NULL;
        }
    }

    WdeSetWaitCursor( false );

    return( res_info );
}


WdeDialogBoxInfo *WdeLoadDialogFromRes( WdeResInfo *res_info, WResLangNode *lnode, bool is32bit )
{
    DialogBoxExHeader32short    h32ex;
    DialogBoxHeader32           h32;
    DialogBoxHeader             h16;

    DialogBoxControl            c16;
    DialogBoxControl32          c32;
    DialogBoxExControl32        c32ex;

    WdeDialogBoxInfo            *dlg_info;
    FILE                        *fp;
    WdeDialogBoxControl         *control;
    LIST                        *prev_control;
#if 0
    WdeDialogBoxControl         *nc;
    LIST                        *clist;
#endif
    int                         index;
    char                        *file_name;
    bool                        ok;

    dlg_info = NULL;
    fp = NULL;
    file_name = NULL;
    memset( &h32ex, 0, sizeof( h32ex ) );
    memset( &h32, 0, sizeof( h32 ) );
    memset( &h16, 0, sizeof( h16 ) );

    ok = ( res_info != NULL && lnode != NULL );

    if( ok ) {
        file_name = res_info->info->tmp_file;
        dlg_info = (WdeDialogBoxInfo *)WRMemAlloc( sizeof( WdeDialogBoxInfo ) );
        ok = ( dlg_info != NULL );
    }

    if( ok ) {
        dlg_info->dialog_header = WdeAllocDialogBoxHeader();
        ok = ( dlg_info->dialog_header != NULL );
    }

    if( ok ) {
        dlg_info->dialog_header->is32bit = is32bit;
        dlg_info->control_list = NULL;
        dlg_info->MemoryFlags = 0;
        ok = ( (fp = ResOpenFileRO( file_name )) != NULL );
    }

    if( ok ) {
        dlg_info->MemoryFlags = lnode->Info.MemoryFlags;
        ok = !ResSeek( fp, lnode->Info.Offset, SEEK_SET );
    }

    if( ok ) {
        if( is32bit ) {
            /* JPK - check if its an extended dialog */
            dlg_info->dialog_header->is32bitEx = ResIsDialogBoxEx( fp );
            ResSeek( fp, lnode->Info.Offset, SEEK_SET );

            if( dlg_info->dialog_header->is32bitEx ) {
                ok = !ResReadDialogBoxExHeader32( &h32, &h32ex, fp );
            } else {
                ok = !ResReadDialogBoxHeader32( &h32, fp );
            }
        } else {
            ok = !ResReadDialogBoxHeader( &h16, fp );
        }
    }

    if( ok ) {
        if( is32bit ) {
            if( dlg_info->dialog_header->is32bitEx ) {
                dlg_info->dialog_header->FontWeight = h32ex.FontWeight;
                dlg_info->dialog_header->FontItalic = h32ex.FontItalic;
                dlg_info->dialog_header->FontCharset = h32ex.FontCharset;
                dlg_info->dialog_header->HelpId = h32ex.HelpId;
                dlg_info->dialog_header->FontWeightDefined = ( h32ex.FontWeightDefined != 0 );
                dlg_info->dialog_header->FontItalicDefined = ( h32ex.FontItalicDefined != 0 );
                dlg_info->dialog_header->FontCharsetDefined = ( h32ex.FontCharsetDefined != 0 );
            }
            dlg_info->dialog_header->Style = h32.Style;
            dlg_info->dialog_header->ExtendedStyle = h32.ExtendedStyle;
            dlg_info->dialog_header->NumOfItems = h32.NumOfItems;
            dlg_info->dialog_header->SizeInfo.x = h32.SizeInfo.x;
            dlg_info->dialog_header->SizeInfo.y = h32.SizeInfo.y;
            dlg_info->dialog_header->SizeInfo.width = h32.SizeInfo.width;
            dlg_info->dialog_header->SizeInfo.height = h32.SizeInfo.height;
            dlg_info->dialog_header->MenuName = h32.MenuName;
            dlg_info->dialog_header->ClassName = h32.ClassName;
            dlg_info->dialog_header->Caption = h32.Caption;
            dlg_info->dialog_header->PointSize = h32.PointSize;
            dlg_info->dialog_header->FontName = h32.FontName;
        } else {
            dlg_info->dialog_header->Style = h16.Style;
            dlg_info->dialog_header->NumOfItems = h16.NumOfItems;
            dlg_info->dialog_header->SizeInfo.x = h16.SizeInfo.x;
            dlg_info->dialog_header->SizeInfo.y = h16.SizeInfo.y;
            dlg_info->dialog_header->SizeInfo.width = h16.SizeInfo.width;
            dlg_info->dialog_header->SizeInfo.height = h16.SizeInfo.height;
            dlg_info->dialog_header->MenuName = h16.MenuName;
            dlg_info->dialog_header->ClassName = h16.ClassName;
            dlg_info->dialog_header->Caption = h16.Caption;
            dlg_info->dialog_header->PointSize = h16.PointSize;
            dlg_info->dialog_header->FontName = h16.FontName;
        }

        prev_control = NULL;
        for( index = 0; index < GETHDR_NUMITEMS( dlg_info->dialog_header ); index++ ) {
            control = WdeAllocDialogBoxControl();
            if( control == NULL ) {
                ok = false;
                break;
            }
            if( is32bit ) {
                /*
                 * JPK - check which control structure to expect based on
                 *       whether this an extended dialog or not
                */
                if( dlg_info->dialog_header->is32bitEx ) {
                    if( ResReadDialogBoxExControl32( &c32ex, fp ) ) {
                        ok = false;
                        break;
                    }
                    control->HelpId = c32ex.HelpId;
                    control->ExtendedStyle = c32ex.ExtendedStyle;
                    control->Style = c32ex.Style;
                    control->SizeInfo.x = c32ex.SizeInfo.x;
                    control->SizeInfo.y = c32ex.SizeInfo.y;
                    control->SizeInfo.width = c32ex.SizeInfo.width;
                    control->SizeInfo.height = c32ex.SizeInfo.height;
                    control->ID = (uint_16)c32ex.ID;
                    control->ClassID = c32ex.ClassID;
                    control->Text = c32ex.Text;
                    control->ExtraBytes = c32ex.ExtraBytes;
                } else {
                    if( ResReadDialogBoxControl32( &c32, fp ) ) {
                        ok = false;
                        break;
                    }
                    control->Style = c32.Style;
                    control->ExtendedStyle = c32.ExtendedStyle;
                    control->SizeInfo.x = c32.SizeInfo.x;
                    control->SizeInfo.y = c32.SizeInfo.y;
                    control->SizeInfo.width = c32.SizeInfo.width;
                    control->SizeInfo.height = c32.SizeInfo.height;
                    control->ID = c32.ID;
                    control->ClassID = c32.ClassID;
                    control->Text = c32.Text;
                    control->ExtraBytes = c32.ExtraBytes;
                }
            } else {
                if( ResReadDialogBoxControl( &c16, fp ) ) {
                    ok = false;
                    break;
                }
                control->SizeInfo.x = c16.SizeInfo.x;
                control->SizeInfo.y = c16.SizeInfo.y;
                control->SizeInfo.width = c16.SizeInfo.width;
                control->SizeInfo.height = c16.SizeInfo.height;
                control->ID = c16.ID;
                control->Style = c16.Style;
                control->ClassID = c16.ClassID;
                control->Text = c16.Text;
                control->ExtraBytes = c16.ExtraBytes;
            }
            if ( prev_control == NULL ) {
                ListAddElt( &dlg_info->control_list, (OBJPTR)control );
                prev_control = dlg_info->control_list;
            } else {
                ListInsertElt( prev_control, (OBJPTR)control );
                prev_control = ListNext( prev_control );
            }
        }
    }

    if( !ok ) {
        if( dlg_info != NULL ) {
            WdeFreeDialogBoxInfo( dlg_info );
            dlg_info = NULL;
        }
    }

    if( fp != NULL ) {
        ResCloseFile( fp );
    }

    return( dlg_info );
}

WResTypeNode *WdeFindTypeNode( WResDir dir, uint_16 type, char *type_name )
{
    return( WRFindTypeNode( dir, type, type_name ) );
}
