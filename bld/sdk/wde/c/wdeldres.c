/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


WdeDialogBoxInfo *WdeLoadDialogFromRes( WdeResInfo *res_info, WResLangNode *langnode, bool is32bit )
{
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

    ok = ( res_info != NULL && langnode != NULL );

    if( ok ) {
        file_name = res_info->info->tmp_file;
        dlg_info = MemAlloc( sizeof( WdeDialogBoxInfo ) );
        ok = ( dlg_info != NULL );
    }

    if( ok ) {
        dlg_info->header = WdeAllocDialogBoxHeader();
        ok = ( dlg_info->header != NULL );
    }

    if( ok ) {
        dlg_info->header->is32bit = is32bit;
        dlg_info->control_list = NULL;
        dlg_info->res_flags = DEFAULT_FLAGS_NONE;
        ok = ( (fp = ResOpenFileRO( file_name )) != NULL );
    }

    if( ok ) {
        dlg_info->res_flags = langnode->Info.res_flags;
        ok = !ResSeek( fp, langnode->Info.Offset, SEEK_SET );
    }

    if( ok ) {
        if( is32bit ) {
            /* JPK - check if its an extended dialog */
            dlg_info->header->is32bitEx = ResIsDialogBoxEx( fp );
            ResSeek( fp, langnode->Info.Offset, SEEK_SET );

            if( dlg_info->header->is32bitEx ) {
                ok = !ResReadDialogBoxExHeader( &dlg_info->header->head, fp );
            } else {
                ok = !ResReadDialogBoxHeader32( &dlg_info->header->head, fp );
            }
        } else {
            ok = !ResReadDialogBoxHeader( &dlg_info->header->head, fp );
        }
    }

    if( ok ) {
        prev_control = NULL;
        for( index = 0; index < GETHDR_NUMITEMS( dlg_info->header ); index++ ) {
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
                if( dlg_info->header->is32bitEx ) {
                    if( ResReadDialogBoxExControl( &control->ctrl, fp ) ) {
                        ok = false;
                        break;
                    }
                } else {
                    if( ResReadDialogBoxControl32( &control->ctrl, fp ) ) {
                        ok = false;
                        break;
                    }
                }
            } else {
                if( ResReadDialogBoxControl( &control->ctrl, fp ) ) {
                    ok = false;
                    break;
                }
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
