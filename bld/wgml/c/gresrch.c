/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 200--2025 The Open Watcom Contributors. All Rights Reserved.
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

#include "clibext.h"


typedef struct taglist {
    struct  taglist *   nxt;
    int                 count;
    char                tagname[16];
} taglist;

static  taglist *   lay_tags = NULL;    // list of found gml layout tags
static  taglist *   sys_tags = NULL;    // list of found gml tags
static  taglist *   scrkws = NULL;      // list of found scr keywords
static  taglist *   single_funcs = NULL;// list of found scr single letter fun
static  taglist *   multi_funcs = NULL; // list of found scr multi letter fun


void printf_research( char * msg, ... )
{
    va_list args;

    va_start( args, msg );
    vfprintf( stdout, msg, args );
    va_end( args );
}


static  void    print_total( int cnt, int icnt )
{
    printf_research( "%6d  --- Total calls for %d items\n", cnt, icnt );
}


/***************************************************************************/
/*  Add and/or count GMLtag                                                */
/***************************************************************************/

void add_GML_tag_research( char * tag )
{
    taglist *   wk;
    taglist *   new;

    if( ProcFlags.layout ) {
        wk = lay_tags;
    } else {
        wk = sys_tags;
    }

    while( wk != NULL ) {
        if( stricmp( tag, wk->tagname ) == 0 ) {
            wk->count++;
            return;
        }
        if( wk->nxt == NULL ) {
            break;
        }
        wk = wk->nxt;
    }
    new = mem_alloc( sizeof( taglist ) );
    if( wk == NULL ) {
        wk = new;
        if( ProcFlags.layout ) {
            lay_tags = new;
        } else {
            sys_tags = new;
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
    taglist     *wk;
    int         cnt;
    int         cnt1;

    cnt = 0;
    cnt1 = 0;
    wk = lay_tags;
    if( wk != NULL ) {
        printf_research( "\nGML layout tag list sorted by first occurrence\n\n" );
        while( wk != NULL ) {
            printf_research( "%6d  :%s\n", wk->count, wk->tagname );
            cnt += wk->count;
            cnt1++;
            wk = wk->nxt;
        }
    }
    cnt = 0;
    cnt1 = 0;
    printf_research( "\nGML tag / macro list sorted by first occurrence\n\n" );
    for( wk = sys_tags; wk != NULL; wk = wk->nxt ) {
        printf_research( "%6d  :%s\n", wk->count, wk->tagname );
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
    taglist     *wk1;

    wk = sys_tags;
    while( wk != NULL ) {
        wk1 = wk;
        wk = wk->nxt;
        mem_free( wk1) ;
    }
    sys_tags = NULL;

    wk = lay_tags;
    while( wk != NULL ) {
        wk1 = wk;
        wk = wk->nxt;
        mem_free( wk1) ;
    }
    lay_tags = NULL;
}

/***************************************************************************/
/*  add and/or count SCR keyword                                           */
/***************************************************************************/

void    add_SCR_tag_research( char * tag )
{
    taglist     *wk;
    taglist     *new;

    for( wk = scrkws; wk != NULL; wk = wk->nxt ) {
        if( stricmp( tag, wk->tagname ) == 0 ) {
            wk->count++;
            return;
        }
        if( wk->nxt == NULL) {
            break;
        }
    }
    new = mem_alloc( sizeof( taglist ) );
    if( wk == NULL ) {
        scrkws = new;
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
    int         cnt;
    int         cnt1;

    printf_research(
        "\nScript controlword / macro list sorted by first occurrence\n\n" );
    cnt = 0;
    cnt1 = 0;
    for( wk = scrkws; wk != NULL; wk = wk->nxt ) {
        printf_research("%6d  .%s\n", wk->count, wk->tagname );
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
    taglist     *wk;
    taglist     *wk1;

    wk = scrkws;
    while( wk != NULL ) {
        wk1 = wk;
        wk = wk->nxt;
        mem_free( wk1) ;
    }
    scrkws = NULL;
}


/***************************************************************************/
/*  add and/or count multi letter functions                                */
/***************************************************************************/

void    add_multi_func_research( const char *fun )
{
    taglist     *wk;
    taglist     *new;

    for( wk = multi_funcs; wk != NULL; wk = wk->nxt ) {
        if( stricmp( fun, wk->tagname ) == 0 ) {
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
    taglist     *wk;
    int         cnt;
    int         cnt1;

    printf_research(
        "\nScript multi letter functions list sorted by first occurrence\n\n" );
    cnt = 0;
    cnt1 = 0;
    for( wk = multi_funcs; wk != NULL; wk = wk->nxt ) {
        printf_research("%6d  %s\n", wk->count, wk->tagname );
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
    taglist     *wk;
    taglist     *wk1;

    wk = multi_funcs;
    while( wk != NULL ) {
        wk1 = wk;
        wk = wk->nxt;
        mem_free( wk1) ;
    }
    multi_funcs = NULL;
}


/***************************************************************************/
/*  add and/or count SCR function                                          */
/***************************************************************************/

void    add_single_func_research( const char *fun )
{
    taglist     *wk;
    taglist     *new;

    for( wk = single_funcs; wk != NULL; wk = wk->nxt ) {
        if( *fun == *(wk->tagname) ) {
            wk->count++;
            return;
        }
        if( wk->nxt == NULL ) {
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
    taglist     *wk;
    int         cnt;
    int         cnt1;

    printf_research(
        "\nScript single letter functions list sorted by first occurrence\n\n" );
    cnt = 0;
    cnt1 = 0;
    for( wk = single_funcs; wk != NULL; wk = wk->nxt ) {
        printf_research("%6d  %s\n", wk->count, wk->tagname );
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
    taglist     *wk;
    taglist     *wk1;

    wk = single_funcs;
    while( wk != NULL ) {
        wk1 = wk;
        wk = wk->nxt;
        mem_free( wk1 );
    }
    single_funcs = NULL;
}


/***************************************************************************/
/*  testoutput of words belonging to an output line with additional info   */
/***************************************************************************/

void    test_out_t_line( text_line  * a_line )
{
    text_chars  *   tw;
    char            buf[BUF_SIZE + 1];

    if( a_line == NULL
      || a_line->first == NULL ) {
        return;
    }

    out_msg( "\n   y_address:%d     line_height:%d\n", a_line->y_address,
             a_line->line_height );

    for( tw = a_line->first; tw != NULL; tw = tw->next ) {
        snprintf( buf, sizeof( buf ),
                  "fnt:%d x:%d-%d w:%d cnt:%d type:%x txt:'%.*s'\n",
                  tw->font, tw->x_address, tw->x_address + tw->width,
                  tw->width, tw->count, tw->type, tw->count, tw->text );
        out_msg( buf );
    }
}
