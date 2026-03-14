/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description: WGML init banner layout default values
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  init banners for default layout                                         */
/***************************************************************************/

static banner_lay_tag *add_banner_layout( banner_lay_tag *ban )
{
    banner_lay_tag  *new;

    new = MemAllocSafe( sizeof( *new ) );
    if( ban == NULL ) {
        memset( new, 0, sizeof( *new ) );
    } else {
        memcpy( new, ban, sizeof( *new ) );
        ban->next = new;
    }
    return( new );
}

static region_lay_tag *add_region_layout( region_lay_tag *reg )
{
    region_lay_tag  *new;

    new = MemAllocSafe( sizeof( *new ) );
    if( reg == NULL ) {
        memset( new, 0, sizeof( *new ) );
    } else {
        memcpy( new, reg, sizeof( *new ) );
        new->next = NULL;
    }
    return( new );
}

void    banner_defaults( void )
{
    banner_lay_tag  *   ban;
    region_lay_tag  *   reg;

    static  char        z0[2] = "0";
    static  char        n1[2] = "1";
    static  char        n2[2] = "2";
    static  char        n3[2] = "3";
    static  char        n4[2] = "4";
    static  char        leftc[] = "left";
    static  char        extendc[] = "extend";
    static  char        nr[] = "pgnumr";

    ban = add_banner_layout( NULL );

    layout_work.banner = ban;

    lay_init_su( z0, &(ban->left_adjust) );
    lay_init_su( z0, &(ban->right_adjust) );
    lay_init_su( n3, &(ban->depth) );
    ban->place = bottom_place;
    ban->docsect = head0_ban;
    ban->next_refnum = 2;
    ban->style = no_content;

    reg = add_region_layout( NULL );

    ban->region = reg;
    lay_init_su( z0, &(reg->indent) );
    lay_init_su( leftc, &(reg->hoffset) );
    lay_init_su( extendc, &(reg->width) );
    lay_init_su( n2, &(reg->voffset) );
    lay_init_su( n1, &(reg->depth) );
    reg->font = FONT0;
    reg->refnum = 1;
    reg->region_position = PPOS_left;
    reg->pouring = last_pour;
    reg->script_format = true;
    reg->contents.content_type = string_content;
    strcpy( reg->contents.string, "/&$htext0.// &$pgnuma./" );
    reg->script_region[0].len = 0;
    reg->script_region[1].len = 0;
    reg->script_region[2].len = 0;
    reg->script_region[0].string = NULL;
    reg->script_region[1].string = NULL;
    reg->script_region[2].string = NULL;
    reg->final_content[0].size = 0;
    reg->final_content[1].size = 0;
    reg->final_content[2].size = 0;
    reg->final_content[0].hoffset = 0;
    reg->final_content[1].hoffset = 0;
    reg->final_content[2].hoffset = 0;
    reg->final_content[0].string = NULL;
    reg->final_content[1].string = NULL;
    reg->final_content[2].string = NULL;

    ban = add_banner_layout( ban );

    ban->docsect = body_ban;

    reg = add_region_layout( reg );

    ban->region = reg;
    strcpy( reg->contents.string, "/&$htext1.// &$pgnuma./" );
    reg->script_region[0].len = 0;
    reg->script_region[1].len = 0;
    reg->script_region[2].len = 0;
    reg->script_region[0].string = NULL;
    reg->script_region[1].string = NULL;
    reg->script_region[2].string = NULL;


    ban = add_banner_layout( ban );

    lay_init_su( n4, &(ban->depth) );
    ban->docsect = abstract_ban;
    ban->style = pgnumr_content;

    reg = add_region_layout( reg );

    ban->region = reg;
    lay_init_su( n3, &(reg->voffset) );
    reg->region_position = PPOS_center;
    reg->script_format = false;
    reg->contents.content_type = pgnumr_content;
    strcpy( reg->contents.string, nr );
    reg->script_region[0].len = 0;
    reg->script_region[1].len = 0;
    reg->script_region[2].len = 0;
    reg->script_region[0].string = NULL;
    reg->script_region[1].string = NULL;
    reg->script_region[2].string = NULL;


    ban = add_banner_layout( ban );

    ban->docsect = preface_ban;

    reg = add_region_layout( reg );

    ban->region = reg;

    ban = add_banner_layout( ban );

    lay_init_su( n3, &(ban->depth) );
    ban->place = top_place;
    ban->docsect = toc_ban;
    ban->style = no_content;

    reg = add_region_layout( reg );

    ban->region = reg;
    lay_init_su( n1, &(reg->voffset) );
    reg->font = FONT3;
    reg->contents.content_type = string_content;
    strcpy( reg->contents.string, "Table of Contents" );
    reg->script_region[0].len = 0;
    reg->script_region[1].len = 0;
    reg->script_region[2].len = 0;
    reg->script_region[0].string = NULL;
    reg->script_region[1].string = NULL;
    reg->script_region[2].string = NULL;


    ban = add_banner_layout( ban );

    ban->place = top_place;
    ban->docsect = figlist_ban;

    reg = add_region_layout( reg );

    ban->region = reg;
    strcpy( reg->contents.string, "List of Figures" );


    ban = add_banner_layout( ban );

    ban->place = top_place;
    ban->docsect = index_ban;

    reg = add_region_layout( reg );

    ban->region = reg;
    strcpy( reg->contents.string, "Index" );


    ban = add_banner_layout( ban );

    ban->place = top_place;
    ban->docsect = letter_ban;

    reg = add_region_layout( reg );

    ban->region = reg;
    lay_init_su( z0, &(reg->voffset) );
    reg->region_position = PPOS_left;
    reg->font = FONT0;
    reg->pouring = no_pour;
    reg->script_format = true;
    strcpy( reg->contents.string, "/&date.// Page &$pgnuma./" );


    ban = add_banner_layout( ban );

    ban->place = topodd_place;
    ban->docsect = letlast_ban;

    reg = add_region_layout( reg );

    ban->region = reg;
}


