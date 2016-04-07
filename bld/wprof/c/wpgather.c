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
#include "dip.h"
#include "sampinfo.h"
#include "wpgather.h"

extern int      OptGatherCut;

bool GetCurrentGather( sio_data * curr_sio )
/******************************************/
{
    bool    gather_active;

    if( curr_sio->level_open == LEVEL_SAMPLE ) {
        gather_active = curr_sio->gather_active;
    } else if( curr_sio->level_open == LEVEL_IMAGE ) {
        gather_active = curr_sio->curr_image->gather_active;
    } else if( curr_sio->level_open == LEVEL_MODULE ) {
        gather_active = curr_sio->curr_mod->gather_active;
    } else if( curr_sio->level_open == LEVEL_FILE ) {
        gather_active = curr_sio->curr_file->gather_active;
    } else {
        gather_active = false;
    }
    return( gather_active );
}



extern void FlipCurrentGather( sio_data * curr_sio )
/**************************************************/
{
    if( curr_sio->level_open == LEVEL_SAMPLE ) {
        curr_sio->gather_active
            = !curr_sio->gather_active;
    } else if( curr_sio->level_open == LEVEL_IMAGE ) {
        curr_sio->curr_image->gather_active
            = !curr_sio->curr_image->gather_active;
    } else if( curr_sio->level_open == LEVEL_MODULE ) {
        curr_sio->curr_mod->gather_active
            = !curr_sio->curr_mod->gather_active;
    } else if( curr_sio->level_open == LEVEL_FILE ) {
        curr_sio->curr_file->gather_active
            = !curr_sio->curr_file->gather_active;
    }
}



extern void GatherCurrent( sio_data * curr_sio )
/**********************************************/
{
    bool    gather_active;

    gather_active = GetCurrentGather( curr_sio );
    if( curr_sio->level_open == LEVEL_SAMPLE ) {
        GatherImage( curr_sio, gather_active );
    } else if( curr_sio->level_open == LEVEL_IMAGE ) {
        GatherMod( curr_sio, gather_active );
    } else if( curr_sio->level_open == LEVEL_MODULE ) {
        GatherFile( curr_sio, gather_active );
    } else if( curr_sio->level_open == LEVEL_FILE ) {
        GatherRtn( curr_sio, gather_active );
    }
}



void GatherImage( sio_data * curr_sio, bool gather_active )
/*********************************************************/
{
    image_info *    curr_image;
    image_info *    gather_image;
    unsigned_32     temp;
    int             count;
    int             image_count;
    clicks_t        agg_count;
    clicks_t        cutoff;

    curr_sio->gather_active = gather_active;
    image_count = 0;
    gather_image = NULL;
    count = curr_sio->image_count;
    curr_sio->number_gathered = 0;
    if( !gather_active ) {
        while( count-- > 0 ) {
            curr_image = curr_sio->images[image_count++];
            if( curr_image->gather_image ) {
                curr_image->ignore_gather = true;
            } else {
                curr_image->ignore_gather = false;
            }
        }
    } else {
        temp = OptGatherCut;
        cutoff = (temp * curr_sio->total_samples) / 1000;
        agg_count = 0;
        while( count-- > 0 ) {
            curr_image = curr_sio->images[image_count++];
            if( curr_image->gather_image ) {
                gather_image = curr_image;
            } else if( curr_image->agg_count <= cutoff
                   && !curr_image->ignore_unknown_image ) {
                curr_sio->number_gathered++;
                agg_count += curr_image->agg_count;
                curr_image->ignore_gather = true;
            }
        }
        if( curr_sio->number_gathered > 0 ) {
            gather_image->agg_count = agg_count;
            gather_image->ignore_gather = false;
        } else {
            curr_sio->gather_active = false;
            gather_image->ignore_gather = true;
        }
    }
}



void GatherMod( sio_data * curr_sio, bool gather_active )
/*******************************************************/
{
    image_info *    curr_image;
    mod_info *      curr_mod;
    mod_info *      gather_mod;
    unsigned_32     temp;
    int             count;
    int             mod_count;
    clicks_t        agg_count;
    clicks_t        cutoff;

    gather_mod = NULL;
    curr_image = curr_sio->curr_image;
    curr_image->gather_active = gather_active;
    curr_image->number_gathered = 0;
    mod_count = 0;
    count = curr_image->mod_count;
    if( !gather_active ) {
        while( count-- > 0 ) {
            curr_mod = curr_image->module[mod_count++];
            if( curr_mod->gather_module ) {
                curr_mod->ignore_gather = true;
            } else {
                curr_mod->ignore_gather = false;
            }
        }
    } else {
        temp = OptGatherCut;
        cutoff = (temp * curr_sio->total_samples) / 1000;
        agg_count = 0;
        while( count-- > 0 ) {
            curr_mod = curr_image->module[mod_count++];
            if( curr_mod->gather_module ) {
                gather_mod = curr_mod;
            } else if( curr_mod->agg_count <= cutoff
                   && !curr_mod->ignore_unknown_mod ) {
                curr_image->number_gathered++;
                agg_count += curr_mod->agg_count;
                curr_mod->ignore_gather = true;
            }
        }
        if( curr_image->number_gathered > 0 ) {
            gather_mod->agg_count = agg_count;
            gather_mod->ignore_gather = false;
        } else {
            curr_image->gather_active = false;
            gather_mod->ignore_gather = true;
        }
    }
}



void GatherFile( sio_data * curr_sio, bool gather_active )
/********************************************************/
{
    mod_info *      curr_mod;
    file_info *     curr_file;
    file_info *     gather_file;
    unsigned_32     temp;
    int             count;
    int             file_count;
    clicks_t        agg_count;
    clicks_t        cutoff;

    gather_file = NULL;
    curr_mod = curr_sio->curr_mod;
    curr_mod->gather_active = gather_active;
    curr_mod->number_gathered = 0;
    file_count = 0;
    count = curr_mod->file_count;
    if( !gather_active ) {
        while( count-- > 0 ) {
            curr_file = curr_mod->mod_file[file_count++];
            if( curr_file->gather_file ) {
                curr_file->ignore_gather = true;
            } else {
                curr_file->ignore_gather = false;
            }
        }
    } else {
        temp = OptGatherCut;
        cutoff = (temp * curr_sio->total_samples) / 1000;
        agg_count = 0;
        while( count-- > 0 ) {
            curr_file = curr_mod->mod_file[file_count++];
            if( curr_file->gather_file ) {
                gather_file = curr_file;
            } else if( curr_file->agg_count <= cutoff
                   && !curr_file->ignore_unknown_file ) {
                curr_mod->number_gathered++;
                agg_count += curr_file->agg_count;
                curr_file->ignore_gather = true;
            }
        }
        if( curr_mod->number_gathered > 0 ) {
            gather_file->agg_count = agg_count;
            gather_file->ignore_gather = false;
        } else {
            curr_mod->gather_active = false;
            gather_file->ignore_gather = true;
        }
    }
}



void GatherRtn( sio_data * curr_sio, bool gather_active )
/*******************************************************/
{
    file_info *     curr_file;
    rtn_info *      curr_rtn;
    rtn_info *      gather_rtn;
    unsigned_32     temp;
    int             count;
    int             rtn_count;
    clicks_t        agg_count;
    clicks_t        cutoff;

    gather_rtn = NULL;
    curr_file = curr_sio->curr_file;
    curr_file->gather_active = gather_active;
    curr_file->number_gathered = 0;
    rtn_count = 0;
    count = curr_file->rtn_count;
    if( !gather_active ) {
        while( count-- > 0 ) {
            curr_rtn = curr_file->routine[rtn_count++];
            if( curr_rtn->gather_routine ) {
                curr_rtn->ignore_gather = true;
            } else {
                curr_rtn->ignore_gather = false;
            }
        }
    } else {
        temp = OptGatherCut;
        cutoff = (temp * curr_sio->total_samples) / 1000;
        agg_count = 0;
        while( count-- > 0 ) {
            curr_rtn = curr_file->routine[rtn_count++];
            if( curr_rtn->gather_routine ) {
                gather_rtn = curr_rtn;
            } else if( curr_rtn->tick_count <= cutoff
                   && !curr_rtn->ignore_unknown_rtn ) {
                curr_file->number_gathered++;
                agg_count += curr_rtn->tick_count;
                curr_rtn->ignore_gather = true;
            }
        }
        if( curr_file->number_gathered > 0 ) {
            gather_rtn->tick_count = agg_count;
            gather_rtn->ignore_gather = false;
        } else {
            curr_file->gather_active = false;
            gather_rtn->ignore_gather = true;
        }
    }
}
