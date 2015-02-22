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
* Description:  utility functions for wgml research related
*                                          i.e. non production formatting
****************************************************************************/

#include <stdarg.h>
#include "wgml.h"
#include "gvars.h"


typedef struct taglist {
    struct  taglist *   nxt;
    long                count;
    char                tagname[16];
} taglist;

static  taglist *   r_lay_tags = NULL;      // list of found gml layout tags
static  taglist *   r_gml_tags = NULL;      // list of found gml tags
static  taglist *   r_scrkws = NULL;        // list of found scr keywords
static  taglist *   single_funcs = NULL;    // list of found scr single letter fun
static  taglist *   multi_funcs = NULL;     // list of found scr multi letter fun



void printf_research( char * msg, ... )
{
    va_list args;

    va_start( args, msg );
    vfprintf( stdout, msg, args );
    va_end( args );
}


static  void    print_total( int32_t cnt, int32_t tags )
{
    printf_research( "%6ld  --- Total calls for %ld items\n", cnt, tags );
}


/***************************************************************************/
/*  Add and/or count GMLtag                                                */
/***************************************************************************/

void add_GML_tag_research( char * tag )
{
    taglist *   wk;
    taglist *   new;

    if( ProcFlags.layout ) {
        wk = r_lay_tags;
    } else {
        wk = r_gml_tags;
    }
    for( ; wk != NULL; wk = wk->nxt ) {
        if( !stricmp( tag, wk->tagname ) ) {
            wk->count++;
            return;
        }
        if( wk->nxt == NULL ) {
            break;
        }
    }
    new = mem_alloc( sizeof( taglist ) );
    if( wk == NULL ) {
        wk = new;
        if( ProcFlags.layout ) {
            r_lay_tags = new;
        } else {
            r_gml_tags = new;
        }
    } else {
        wk->nxt = new;
    }
    new->nxt = NULL;
    strcpy( new->tagname, tag );
    strupr( new->tagname );
    new->count = 1;
}

/***************************************************************************/
/*  print found GMLtags with usagecount                                    */
/***************************************************************************/

void    print_GML_tags_research( void )
{
    taglist *   wk;
    int32_t     cnt = 0;
    int32_t     cnt1 = 0;

    if( r_lay_tags != NULL ) {
        printf_research( "\nGML layout tag list sorted by first occurrence\n\n" );
        for( wk = r_lay_tags; wk != NULL; wk = wk->nxt ) {
            printf_research("%6ld  :%s\n", wk->count, wk->tagname );
            cnt += wk->count;
            cnt1++;
        }
    }
    cnt = 0;
    cnt1 = 0;
    printf_research( "\nGML tag / macro list sorted by first occurrence\n\n" );
    for( wk = r_gml_tags; wk != NULL; wk = wk->nxt ) {
        printf_research("%6ld  :%s\n", wk->count, wk->tagname );
        cnt += wk->count;
        cnt1++;
    }
    print_total( cnt, cnt1 );
}

/***************************************************************************/
/*  free storage of GMLtaglist                                             */
/***************************************************************************/

void    free_GML_tags_research( void )
{
    taglist     *wk;

    while( (wk = r_gml_tags) != NULL ) {
        r_gml_tags = wk->nxt;
        mem_free( wk) ;
    }

    while( (wk = r_lay_tags) != NULL ) {
        r_lay_tags = wk->nxt;
        mem_free( wk );
    }
}

/***************************************************************************/
/*  add and/or count SCR keyword                                           */
/***************************************************************************/

void    add_SCR_tag_research( char * tag )
{
    taglist     *wk;
    taglist     *new;

    for( wk = r_scrkws; wk != NULL; wk = wk->nxt ) {
        if( !stricmp( tag, wk->tagname ) ) {
            wk->count++;
            return;
        }
        if( wk->nxt == NULL) {
            break;
        }
    }
    new = mem_alloc( sizeof( taglist ) );
    if( wk == NULL ) {
        r_scrkws = new;
    } else {
        wk->nxt = new;
    }
    new->nxt = NULL;
    strcpy( new->tagname, tag );
    strlwr( new->tagname );
    new->count = 1;
}

/***************************************************************************/
/*  print found SCR keywords and usage count                               */
/***************************************************************************/

void    print_SCR_tags_research( void )
{
    taglist     *wk;
    int32_t     cnt = 0;
    int32_t     cnt1 = 0;

    printf_research(
        "\nScript controlword / macro list sorted by first occurrence\n\n" );
    for( wk = r_scrkws; wk != NULL; wk = wk->nxt ) {
        printf_research("%6ld  .%s\n", wk->count, wk->tagname );
        cnt += wk->count;
        cnt1++;
    }
    print_total( cnt, cnt1 );
}

/***************************************************************************/
/*  free SCR keyword list                                                  */
/***************************************************************************/

void    free_SCR_tags_research( void )
{
    taglist *   wk;

    while( (wk = r_scrkws) != NULL ) {
        r_scrkws = wk->nxt;
        mem_free( wk );
    }
}


/***************************************************************************/
/*  add and/or count multi letter functions                                */
/***************************************************************************/

void    add_multi_func_research( char * fun )
{
    taglist *   wk;
    taglist *   new;

    for( wk = multi_funcs; wk != NULL; wk = wk->nxt ) {
        if( !stricmp( fun, wk->tagname ) ) {
            wk->count++;
            return;
        }
        if( wk->nxt == NULL) {
            break;
        }
    }
    new = mem_alloc( sizeof( taglist ) );
    if( wk == NULL ) {
        multi_funcs = new;
    } else {
        wk->nxt = new;
    }
    new->nxt = NULL;
    strcpy( new->tagname, fun );
    strlwr( new->tagname );
    new->count = 1;
}

/***************************************************************************/
/*  print found multi letter functions and count                           */
/***************************************************************************/

void    print_multi_funcs_research( void )
{
    taglist *   wk;
    int32_t     cnt = 0;
    int32_t     cnt1 = 0;

    printf_research(
        "\nScript multi letter functions list sorted by first occurrence\n\n" );
    for( wk = multi_funcs; wk != NULL; wk = wk->nxt ) {
        printf_research("%6ld  %s\n", wk->count, wk->tagname );
        cnt += wk->count;
        cnt1++;
    }
    print_total( cnt, cnt1 );
}

/***************************************************************************/
/*  free multi function list                                               */
/***************************************************************************/

void    free_multi_funcs_research( void )
{
    taglist *   wk;

    while( (wk = multi_funcs) != NULL ) {
        multi_funcs = wk->nxt;
        mem_free( wk) ;
    }
}



/***************************************************************************/
/*  add and/or count SCR function                                          */
/***************************************************************************/

void    add_single_func_research( char * fun )
{
    taglist *   wk;
    taglist *   new;

    for( wk = single_funcs; wk != NULL; wk = wk->nxt ) {
        if( *fun == *(wk->tagname) ) {
            wk->count++;
            return;
        }
        if( wk->nxt == NULL) {
            break;
        }
    }
    new = mem_alloc( sizeof( taglist ) );
    if( wk == NULL ) {
        single_funcs = new;
    } else {
        wk->nxt = new;
    }
    new->nxt = NULL;
    *(new->tagname) = *fun;
    *(new->tagname + 1) = '\0';
    new->count = 1;
}

/***************************************************************************/
/*  print found SCR functions and usage count                              */
/***************************************************************************/

void    print_single_funcs_research( void )
{
    taglist *   wk = single_funcs;
    int32_t     cnt = 0;
    int32_t     cnt1 = 0;

    printf_research(
        "\nScript single letter functions list sorted by first occurrence\n\n" );
    for( wk = single_funcs; wk != NULL; wk = wk->nxt ) {
        printf_research("%6ld  %s\n", wk->count, wk->tagname );
        cnt += wk->count;
        cnt1++;
    }
    print_total( cnt, cnt1 );
}

/***************************************************************************/
/*  free SCR function list                                                 */
/***************************************************************************/

void    free_single_funcs_research( void )
{
    taglist *   wk;

    while( (wk = single_funcs) != NULL ) {
        single_funcs = wk->nxt;
        mem_free( wk ) ;
    }
}


/***************************************************************************/
/*  testoutput of words belonging to an output line with additional info   */
/***************************************************************************/

void    test_out_t_line( text_line  * a_line )
{
    text_chars  *   tw;
    char            buf[BUF_SIZE];

    if( a_line == NULL || a_line->first == NULL) {
        return;
    }

    out_msg( "\n   y_address:%d     line_height:%d\n", a_line->y_address,
             a_line->line_height );

    for( tw = a_line->first; tw != NULL; tw = tw->next ) {

        snprintf( buf, buf_size,
                  "fnt:%d x:%d-%d w:%d cnt:%d type:%x txt:'%.*s'\n",
                  tw->font, tw->x_address, tw->x_address + tw->width,
                  tw->width, tw->count, tw->type, tw->count, tw->text );
        out_msg( buf );
    }
}
