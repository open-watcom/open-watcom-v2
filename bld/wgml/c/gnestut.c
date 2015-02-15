/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WGML utility functions for nested :sl, :ol, ... and friends
*
*               init_nest_cb         alloc + init a tag_cb
*               copy_to_nest_stack   copy input stack to nest stack
*
****************************************************************************/
#include    "wgml.h"
#include    "gvars.h"

/***************************************************************************/
/*  alloc and (partially) initialize a nest_cb                             */
/***************************************************************************/

void init_nest_cb( void )
{
    tag_cb  *   wk;

    wk = alloc_tag_cb();
    memset( wk, 0, sizeof( *wk ) );

    wk->prev = nest_cb;
    wk->c_tag = t_NONE;
    wk->p_stack = NULL;
    wk->lay_tag = NULL;

    if( nest_cb == NULL ) {             // if first one set defaults
        wk->left_indent  = 0;
        wk->right_indent = 0;
        wk->post_skip    = 0;
        wk->tsize        = 0;
        wk->termhi       = 0;
        wk->headhi       = 0;
        wk->dl_break     = false;
        wk->compact      = false;
        wk->font         = g_curr_font;
    }

    nest_cb = wk;                       // new top of stack
    return;
}


/***************************************************************************/
/*  copy input stack to nest stack (called at :tag start)                  */
/***************************************************************************/

nest_stack * copy_to_nest_stack( void )
{
    nest_stack  *   head;
    nest_stack  *   nest_p;
    nest_stack  *   nest_o;
    inputcb     *   inwk;

    head = NULL;
    for( inwk = input_cbs; inwk != NULL; inwk = inwk->prev ) {
        nest_p = mem_alloc( sizeof( nest_stack ) );
        nest_p->prev = NULL;
        nest_p->nest_flag = inwk->fmflags & II_input;
        if( head == NULL ) {
            head = nest_p;
        } else {
            nest_o->prev = nest_p;
        }
        if( inwk->fmflags & II_tag_mac ) {
            nest_p->s.mt.m = inwk->s.m->mac;
            nest_p->lineno = inwk->s.m->lineno;
            if( inwk->fmflags & II_tag ) {
                nest_p->s.mt.tag_m = inwk->s.m->tag;
            } else {
                nest_p->s.mt.tag_m = NULL;
            }
        } else {
            nest_p->s.filename = inwk->s.f->filename;
            nest_p->lineno = inwk->s.f->lineno;
        }
        nest_o = nest_p;
    }
    return( head );
}

