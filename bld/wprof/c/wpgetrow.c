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

//#include "wpgetrow.def"


STATIC int          WPGetImageRow( sio_data * );
STATIC int          WPGetModRow( sio_data * );
STATIC int          WPGetFileRow( sio_data * );
STATIC int          WPGetRtnRow( sio_data * );



extern image_info * SImageGetImage( a_window * wnd, int row )
/***********************************************************/
{
    sio_data *      curr_sio;
    image_info *    image;
    int             count;
    int             count2;
    int             image_count;

    curr_sio = WndExtra( wnd );
    image_count = 0;
    count = 0;
    count2 = curr_sio->image_count;
    while( count2-- > 0 ) {
        image = curr_sio->images[image_count++];
        if( !image->ignore_unknown_image && !image->ignore_gather ) {
            if( count++ == row ) return( image );
        }
    }
    return( NULL );
}



extern mod_info * SModGetModule( a_window * wnd, int row )
/********************************************************/
{
    sio_data *      curr_sio;
    image_info *    curr_image;
    mod_info *      mod;
    int             count;
    int             count2;
    int             mod_count;

    curr_sio = WndExtra( wnd );
    curr_image = curr_sio->curr_image;
    count = 0;
    mod_count = 0;
    count2 = curr_image->mod_count;
    while( count2-- > 0 ) {
        mod = curr_image->module[mod_count++];
        if( !mod->ignore_unknown_mod && !mod->ignore_gather ) {
            if( count++ == row ) return( mod );
        }
    }
    return( NULL );
}



extern file_info * SFileGetFile( a_window * wnd, int row )
/********************************************************/
{
    sio_data *      curr_sio;
    mod_info *      curr_mod;
    file_info *     curr_file;
    int             count;
    int             count2;
    int             file_count;

    curr_sio = WndExtra( wnd );
    curr_mod = curr_sio->curr_mod;
    count = 0;
    file_count = 0;
    count2 = curr_mod->file_count;
    while( count2-- > 0 ) {
        curr_file = curr_mod->mod_file[file_count++];
        if( !curr_file->ignore_unknown_file && !curr_file->ignore_gather ) {
            if( count++ == row ) return( curr_file );
        }
    }
    return( NULL );
}



extern rtn_info * SRtnGetRoutine( a_window * wnd, int row )
/*********************************************************/
{
    sio_data *      curr_sio;
    file_info *     curr_file;
    rtn_info *      rtn;
    int             count;
    int             count2;
    int             rtn_count;

    curr_sio = WndExtra( wnd );
    curr_file = curr_sio->curr_file;
    count = 0;
    rtn_count = 0;
    count2 = curr_file->rtn_count;
    while( count2-- > 0 ) {
        rtn = curr_file->routine[rtn_count++];
        if( !rtn->ignore_unknown_rtn && !rtn->ignore_gather ) {
            if( count++ == row ) return( rtn );
        }
    }
    return( NULL );
}



extern int WPGetRow( sio_data * curr_sio )
/****************************************/
{
    int             curr_line;

    if( curr_sio->level_open == LEVEL_SAMPLE ) {
        curr_line = WPGetImageRow( curr_sio );
    } else if( curr_sio->level_open == LEVEL_IMAGE ) {
        curr_line = WPGetModRow( curr_sio );
    } else if( curr_sio->level_open == LEVEL_MODULE ) {
        curr_line = WPGetFileRow( curr_sio );
    } else if( curr_sio->level_open == LEVEL_FILE ) {
        curr_line = WPGetRtnRow( curr_sio );
    } else {
        curr_line = 0;
    }
    return( curr_line );
}



STATIC int WPGetImageRow( sio_data * curr_sio )
/*********************************************/
{
    image_info *    image;
    image_info *    curr_image;
    int             row;
    int             count;
    int             count2;

    row = 0;
    count = 0;
    curr_image = curr_sio->curr_image;
    count2 = curr_sio->image_count;
    while( count2-- > 0 ) {
        image = curr_sio->images[count++];
        if( !image->ignore_unknown_image && !image->ignore_gather ) {
            if( image == curr_image ) return( row );
            row++;
        }
    }
    return( 0 );
}



STATIC int WPGetModRow( sio_data * curr_sio )
/*******************************************/
{
    image_info *    curr_image;
    mod_info *      curr_mod;
    mod_info *      mod;
    int             row;
    int             count;
    int             count2;

    row = 0;
    count = 0;
    curr_image = curr_sio->curr_image;
    curr_mod = curr_sio->curr_mod;
    count2 = curr_image->mod_count;
    while( count2-- > 0 ) {
        mod = curr_image->module[count++];
        if( !mod->ignore_unknown_mod && !mod->ignore_gather ) {
            if( mod == curr_mod ) return( row );
            row++;
        }
    }
    return( 0 );
}



STATIC int WPGetFileRow( sio_data * curr_sio )
/********************************************/
{
    mod_info *      curr_mod;
    file_info *     curr_file;
    file_info *     cfile;
    int             row;
    int             count;
    int             count2;

    row = 0;
    count = 0;
    curr_mod = curr_sio->curr_mod;
    curr_file = curr_sio->curr_file;
    count2 = curr_mod->file_count;
    while( count2-- > 0 ) {
        cfile = curr_mod->mod_file[count++];
        if( !cfile->ignore_unknown_file && !cfile->ignore_gather ) {
            if( cfile == curr_file ) return( row );
            row++;
        }
    }
    return( 0 );
}



STATIC int WPGetRtnRow( sio_data * curr_sio )
/*******************************************/
{
    file_info *     curr_file;
    rtn_info *      curr_rtn;
    rtn_info *      rtn;
    int             row;
    int             count;
    int             count2;

    row = 0;
    count = 0;
    curr_file = curr_sio->curr_file;
    curr_rtn = curr_sio->curr_rtn;
    count2 = curr_file->rtn_count;
    while( count2-- > 0 ) {
        rtn = curr_file->routine[count++];
        if( !rtn->ignore_unknown_rtn && !rtn->ignore_gather ) {
            if( rtn == curr_rtn ) return( row );
            row++;
        }
    }
    return( 0 );
}
