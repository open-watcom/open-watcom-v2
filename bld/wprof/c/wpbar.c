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

//#include "wpbar.def"



bool GetCurrentAbsBar( sio_data * curr_sio )
/******************************************/
{
    bool        bar = false;

    if( curr_sio->level_open == LEVEL_SAMPLE ) {
        bar = curr_sio->abs_bar;
    } else if( curr_sio->level_open == LEVEL_IMAGE ) {
        bar = curr_sio->curr_image->abs_bar;
    } else if( curr_sio->level_open == LEVEL_MODULE ) {
        bar = curr_sio->curr_mod->abs_bar;
    } else if( curr_sio->level_open == LEVEL_FILE ) {
        bar = curr_sio->curr_file->abs_bar;
    } else if( curr_sio->level_open >= LEVEL_ROUTINE ) {
        bar = curr_sio->asm_src_info.abs_bar;
    }
    return( bar );
}



bool GetCurrentRelBar( sio_data * curr_sio )
/******************************************/
{
    bool        bar = false;

    if( curr_sio->level_open == LEVEL_SAMPLE ) {
        bar = curr_sio->rel_bar;
    } else if( curr_sio->level_open == LEVEL_IMAGE ) {
        bar = curr_sio->curr_image->rel_bar;
    } else if( curr_sio->level_open == LEVEL_MODULE ) {
        bar = curr_sio->curr_mod->rel_bar;
    } else if( curr_sio->level_open == LEVEL_FILE ) {
        bar = curr_sio->curr_file->rel_bar;
    } else if( curr_sio->level_open >= LEVEL_ROUTINE ) {
        bar = curr_sio->asm_src_info.rel_bar;
    }
    return( bar );
}



bool GetCurrentMaxBar( sio_data * curr_sio )
/******************************************/
{
    bool        bar = false;

    if( curr_sio->level_open == LEVEL_SAMPLE ) {
        bar = curr_sio->bar_max;
    } else if( curr_sio->level_open == LEVEL_IMAGE ) {
        bar = curr_sio->curr_image->bar_max;
    } else if( curr_sio->level_open == LEVEL_MODULE ) {
        bar = curr_sio->curr_mod->bar_max;
    } else if( curr_sio->level_open == LEVEL_FILE ) {
        bar = curr_sio->curr_file->bar_max;
    } else if( curr_sio->level_open >= LEVEL_ROUTINE ) {
        bar = curr_sio->asm_src_info.bar_max;
    }
    return( bar );
}



extern void FlipCurrentAbsBar( sio_data * curr_sio )
/**************************************************/
{
    if( curr_sio->level_open == LEVEL_SAMPLE ) {
        curr_sio->abs_bar = !curr_sio->abs_bar;
    } else if( curr_sio->level_open == LEVEL_IMAGE ) {
        curr_sio->curr_image->abs_bar = !curr_sio->curr_image->abs_bar;
    } else if( curr_sio->level_open == LEVEL_MODULE ) {
        curr_sio->curr_mod->abs_bar = !curr_sio->curr_mod->abs_bar;
    } else if( curr_sio->level_open == LEVEL_FILE ) {
        curr_sio->curr_file->abs_bar = !curr_sio->curr_file->abs_bar;
    } else if( curr_sio->level_open >= LEVEL_ROUTINE ) {
        curr_sio->asm_src_info.abs_bar = !curr_sio->asm_src_info.abs_bar;
    }
}



extern void FlipCurrentRelBar( sio_data * curr_sio )
/**************************************************/
{
    if( curr_sio->level_open == LEVEL_SAMPLE ) {
        curr_sio->rel_bar = !curr_sio->rel_bar;
    } else if( curr_sio->level_open == LEVEL_IMAGE ) {
        curr_sio->curr_image->rel_bar = !curr_sio->curr_image->rel_bar;
    } else if( curr_sio->level_open == LEVEL_MODULE ) {
        curr_sio->curr_mod->rel_bar = !curr_sio->curr_mod->rel_bar;
    } else if( curr_sio->level_open == LEVEL_FILE ) {
        curr_sio->curr_file->rel_bar = !curr_sio->curr_file->rel_bar;
    } else if( curr_sio->level_open >= LEVEL_ROUTINE ) {
        curr_sio->asm_src_info.rel_bar = !curr_sio->asm_src_info.rel_bar;
    }
}



extern void FlipCurrentMaxBar( sio_data * curr_sio )
/**************************************************/
{
    if( curr_sio->level_open == LEVEL_SAMPLE ) {
        curr_sio->bar_max = !curr_sio->bar_max;
    } else if( curr_sio->level_open == LEVEL_IMAGE ) {
        curr_sio->curr_image->bar_max = !curr_sio->curr_image->bar_max;
    } else if( curr_sio->level_open == LEVEL_MODULE ) {
        curr_sio->curr_mod->bar_max = !curr_sio->curr_mod->bar_max;
    } else if( curr_sio->level_open == LEVEL_FILE ) {
        curr_sio->curr_file->bar_max = !curr_sio->curr_file->bar_max;
    } else if( curr_sio->level_open >= LEVEL_ROUTINE ) {
        curr_sio->asm_src_info.bar_max = !curr_sio->asm_src_info.bar_max;
    }
}
