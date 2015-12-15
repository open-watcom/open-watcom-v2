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


#include <stdio.h>

#include "common.h"
#include "aui.h"
#include "dip.h"
#include "sampinfo.h"
#include "wpsrcfil.h"
#include "wpasmfil.h"

//#include "wpnumrow.def"


//STATIC int              sampleNumRows( a_window * );
STATIC int              simageGetRows( sio_data * );
STATIC int              smodGetRows( sio_data * );
STATIC int              sfileGetRows( sio_data * );
STATIC int              srtnGetRows( sio_data * );
STATIC int              ssrcGetRows( sio_data * );
STATIC int              sasmGetRows( sio_data * );

typedef int (SAMPLEROWRTNS)( sio_data * wnd );

static SAMPLEROWRTNS * overviewRowRtns[] = {
    &simageGetRows,
    &smodGetRows,
    &sfileGetRows,
    &srtnGetRows,
    &ssrcGetRows,
    &sasmGetRows,
};



extern int SampleNumRows( a_window * wnd )
/****************************************/
{
    sio_data *      curr_sio;

    curr_sio = WndExtra( wnd );
    if( overviewRowRtns[curr_sio->level_open] == NULL ) {
        return( 0 );
    }
    return( overviewRowRtns[curr_sio->level_open]( curr_sio ) );
}



STATIC int simageGetRows( sio_data * curr_sio )
/*********************************************/
{
    int             count;

    count = curr_sio->image_count - curr_sio->number_gathered;
    if( curr_sio->ignore_unknown_image ) {
        count--;
    }
    if( !curr_sio->gather_active ) {
        count--;
    }
    return( count );
}



STATIC int smodGetRows( sio_data * curr_sio )
/*******************************************/
{
    image_info *    curr_image;
    int             count;

    curr_image = curr_sio->curr_image;
    count = curr_image->mod_count - curr_image->number_gathered;
    if( curr_image->ignore_unknown_mod ) {
        count--;
    }
    if( !curr_image->gather_active ) {
        count--;
    }
    return( count );
}



STATIC int sfileGetRows( sio_data * curr_sio )
/********************************************/
{
    mod_info *      curr_mod;
    int             count;

    curr_mod = curr_sio->curr_mod;
    count = curr_mod->file_count - curr_mod->number_gathered;
    if( curr_mod->ignore_unknown_file ) {
        count--;
    }
    if( !curr_mod->gather_active ) {
        count--;
    }
    return( count );
}



STATIC int srtnGetRows( sio_data * curr_sio )
/*******************************************/
{
    file_info *     curr_file;
    int             count;

    curr_file = curr_sio->curr_file;
    count = curr_file->rtn_count - curr_file->number_gathered;
    if( curr_file->ignore_unknown_rtn ) {
        count--;
    }
    if( !curr_file->gather_active ) {
        count--;
    }
    return( count );
}



STATIC int ssrcGetRows( sio_data * curr_sio )
/*******************************************/
{
    wp_srcfile *    wp_src;

    wp_src = curr_sio->src_file;
    return( wp_src->src_rows );
}



STATIC int sasmGetRows( sio_data * curr_sio )
/*******************************************/
{
    wp_asmfile *    wp_asm;

    wp_asm = curr_sio->asm_file;
    return( wp_asm->asm_rows );
}
