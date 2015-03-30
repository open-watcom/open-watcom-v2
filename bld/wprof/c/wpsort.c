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


#include <string.h>
#include <stdio.h>

#include "common.h"
#include "aui.h"
#include "dip.h"
#include "sampinfo.h"
#include "clibext.h"


extern void SortImage( sio_data * );
extern void SortMod( sio_data * );
extern void SortFile( sio_data * );
extern void SortRtn( sio_data * );


STATIC int imageCountCmp( pointer *, pointer * );
STATIC int modCountCmp( pointer *, pointer * );
STATIC int fileCountCmp( pointer *, pointer * );
STATIC int rtnCountCmp( pointer *, pointer * );
STATIC int imageNameCmp( pointer *, pointer * );
STATIC int modNameCmp( pointer *, pointer * );
STATIC int fileNameCmp( pointer *, pointer * );
STATIC int rtnNameCmp( pointer *, pointer * );

static void * imageSortCmp[] = {
    &imageCountCmp,
    &imageNameCmp
};
static void * modSortCmp[] = {
    &modCountCmp,
    &modNameCmp
};
static void * fileSortCmp[] = {
    &fileCountCmp,
    &fileNameCmp
};
static void * rtnSortCmp[] = {
    &rtnCountCmp,
    &rtnNameCmp
};



extern int GetCurrentSort( sio_data * curr_sio )
/**********************************************/
{
    int     sort_type;

    if( curr_sio->level_open == LEVEL_SAMPLE ) {
        sort_type = curr_sio->sort_type;
    } else if( curr_sio->level_open == LEVEL_IMAGE ) {
        sort_type = curr_sio->curr_image->sort_type;
    } else if( curr_sio->level_open == LEVEL_MODULE ) {
        sort_type = curr_sio->curr_mod->sort_type;
    } else if( curr_sio->level_open == LEVEL_FILE ) {
        sort_type = curr_sio->curr_file->sort_type;
    } else {
        sort_type = SORT_DISABLE;
    }
    return( sort_type );
}



extern void SetCurrentSort( sio_data * curr_sio, int sort_type )
/**************************************************************/
{
    if( curr_sio->level_open == LEVEL_SAMPLE ) {
        curr_sio->sort_type = sort_type;
        curr_sio->sort_needed = true;
    } else if( curr_sio->level_open == LEVEL_IMAGE ) {
        curr_sio->curr_image->sort_type = sort_type;
        curr_sio->curr_image->sort_needed = true;
    } else if( curr_sio->level_open == LEVEL_MODULE ) {
        curr_sio->curr_mod->sort_type = sort_type;
        curr_sio->curr_mod->sort_needed = true;
    } else if( curr_sio->level_open == LEVEL_FILE ) {
        curr_sio->curr_file->sort_type = sort_type;
        curr_sio->curr_file->sort_needed = true;
    }
}



extern void SortCurrent( sio_data * curr_sio )
/********************************************/
{
    if( curr_sio->level_open == LEVEL_SAMPLE ) {
        SortImage( curr_sio );
    } else if( curr_sio->level_open == LEVEL_IMAGE ) {
        SortMod( curr_sio );
    } else if( curr_sio->level_open == LEVEL_MODULE ) {
        SortFile( curr_sio );
    } else if( curr_sio->level_open == LEVEL_FILE ) {
        SortRtn( curr_sio );
    }
}



extern void SortImage( sio_data * curr_sio )
/******************************************/
{
    if( curr_sio->sort_needed ) {
        qsort( curr_sio->images, curr_sio->image_count,
               sizeof(pointer), imageSortCmp[curr_sio->sort_type] );
    }
}



extern void SortMod( sio_data * curr_sio )
/****************************************/
{
    if( curr_sio->curr_image->sort_needed ) {
        qsort( curr_sio->curr_image->module, curr_sio->curr_image->mod_count,
               sizeof(pointer), modSortCmp[curr_sio->curr_image->sort_type] );
    }
}



extern void SortFile( sio_data * curr_sio )
/*****************************************/
{
    if( curr_sio->curr_mod->sort_needed ) {
        qsort( curr_sio->curr_mod->mod_file, curr_sio->curr_mod->file_count,
               sizeof(pointer), fileSortCmp[curr_sio->curr_mod->sort_type] );
    }
}



extern void SortRtn( sio_data * curr_sio )
/****************************************/
{
    if( curr_sio->curr_file->sort_needed ) {
        qsort( curr_sio->curr_file->routine, curr_sio->curr_file->rtn_count,
               sizeof(pointer), rtnSortCmp[curr_sio->curr_file->sort_type] );
    }
}



STATIC int imageCountCmp( pointer * d1, pointer * d2 )
/****************************************************/
{
    image_info *    data1;
    image_info *    data2;

    data1 = *d1;
    data2 = *d2;
    if( data1->agg_count < data2->agg_count ) {
        return( 1 );
    }
    if( data1->agg_count > data2->agg_count ) {
        return( -1 );
    }
    return( stricmp( data1->name, data2->name ) );
}



STATIC int modCountCmp( pointer * d1, pointer * d2 )
/**************************************************/
{
    mod_info *      data1;
    mod_info *      data2;

    data1 = *d1;
    data2 = *d2;
    if( data1->agg_count < data2->agg_count ) {
        return( 1 );
    }
    if( data1->agg_count > data2->agg_count ) {
        return( -1 );
    }
    return( stricmp( data1->name, data2->name ) );
}



STATIC int fileCountCmp( pointer * d1, pointer * d2 )
/***************************************************/
{
    file_info *     data1;
    file_info *     data2;

    data1 = *d1;
    data2 = *d2;
    if( data1->agg_count < data2->agg_count ) {
        return( 1 );
    }
    if( data1->agg_count > data2->agg_count ) {
        return( -1 );
    }
    return( stricmp( data1->name, data2->name ) );
}



STATIC int rtnCountCmp( pointer * d1, pointer * d2 )
/**************************************************/
{
    rtn_info *      data1;
    rtn_info *      data2;

    data1 = *d1;
    data2 = *d2;
    if( data1->tick_count < data2->tick_count ) {
        return( 1 );
    }
    if( data1->tick_count > data2->tick_count ) {
        return( -1 );
    }
    return( stricmp( data1->name, data2->name ) );
}



STATIC int imageNameCmp( pointer * d1, pointer * d2 )
/***************************************************/
{
    image_info *    data1;
    image_info *    data2;

    data1 = *d1;
    data2 = *d2;
    return( stricmp( data1->name, data2->name ) );
}



STATIC int modNameCmp( pointer * d1, pointer * d2 )
/*************************************************/
{
    mod_info *      data1;
    mod_info *      data2;

    data1 = *d1;
    data2 = *d2;
    return( stricmp( data1->name, data2->name ) );
}



STATIC int fileNameCmp( pointer * d1, pointer * d2 )
/**************************************************/
{
    file_info *     data1;
    file_info *     data2;

    data1 = *d1;
    data2 = *d2;
    return( stricmp( data1->name, data2->name ) );
}



STATIC int rtnNameCmp( pointer * d1, pointer * d2 )
/*************************************************/
{
    rtn_info *      data1;
    rtn_info *      data2;

    data1 = *d1;
    data2 = *d2;
    return( stricmp( data1->name, data2->name ) );
}
