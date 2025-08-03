/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implements user GML tags (tables and access routines)
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*  init_tag_dict   initialize dictionary pointer                          */
/***************************************************************************/

void    init_tag_dict( tag_dict *pdict )
{
    *pdict = NULL;
    return;
}


/***************************************************************************/
/*  add_tag     add tag entry to dictionary                                */
/*              if tag already defined error                               */
/***************************************************************************/

gtentry *add_tag( tag_dict *pdict, const char *tagname, const char *macname, int flags )
{
    tag_dict    dict;
    gtentry     *ge;

    ge = find_user_tag( pdict, tagname );
    if( ge != NULL ) {
        xx_source_err_exit_c( ERR_TAG_EXIST, tagname );
        /* never return */
    }
    dict = mem_alloc( sizeof( *dict ) );
    dict->next = *pdict;
    *pdict = dict;

    ge = DICT2GE( dict );
    strcpy( ge->tagname, tagname );
    ge->taglen = strlen( ge->tagname );
    strcpy( ge->macname, macname );
    ge->tagflags = flags;
    ge->attribs = NULL;
    ge->usecount = 0;
    return( ge );
}


/***************************************************************************/
/*  change_tag     change macro to execute in tag entry                    */
/***************************************************************************/

gtentry *change_tag( tag_dict *pdict, const char *tagname, const char *macname )
{
    gtentry     *ge = NULL;

    if( *pdict != NULL ) {
        ge = find_user_tag( pdict, tagname );
        if( ge != NULL ) {
           strcpy( ge->macname, macname );
        }
    }
    return( ge );
}


/***************************************************************************/
/*  free_att  delete single attribute                                      */
/***************************************************************************/
static  void    free_att( gaentry *ga )
{
    gavalentry  *vw;
    gavalentry  *vwn;

    vw = ga->vals;
    while( vw != NULL ) {
        if( vw->valflags & GAVAL_valptr ) {
            mem_free( vw->a.valptr );
        }
        vwn = vw->next;
        mem_free( vw );
        vw = vwn;
    }
    mem_free( ga );
}


/***************************************************************************/
/*  free_tag  delete single tag                                            */
/*            returns previuos entry or null if first deleted              */
/***************************************************************************/

tag_dict    free_tag( tag_dict *pdict, gtentry *ge )
{
    tag_dict    prev_dict;
    tag_dict    dict;
    gaentry     *attrib;

    /*
     * it is always call for existing dictionary entry
     * - call for delete all dictionary entries
     * - call for just find dictionary entry
     * not need check for entry existence
     */
    dict = *pdict;
    if( DICT2GE( dict ) == ge ) {              // delete first entry
        *pdict = dict->next;
        prev_dict = NULL;               // previous is null
    } else {
        dict = NULL;
        for( prev_dict = *pdict; prev_dict != NULL; prev_dict = prev_dict->next ) {
            if( DICT2GE( prev_dict->next ) == ge ) {
                dict = prev_dict->next;
                prev_dict->next = prev_dict->next->next;  // chain update
                break;
            }
        }
    }
    while( (attrib = DICT2GE( dict )->attribs) != NULL ) {      // delete all attributes
        DICT2GE( dict )->attribs = attrib->next;
        free_att( attrib );
    }
    mem_free( dict );                   // now the entry itself
    return( prev_dict );                // return previous entry or NULL
}


/***************************************************************************/
/*  free_tag_dict   free all user tag dictionary entries                   */
/***************************************************************************/

void    free_tag_dict( tag_dict *pdict )
{
    tag_dict    dict;
    tag_dict    next;

    dict = *pdict;
    while( dict != NULL ) {
        next = dict->next;
        free_tag( pdict, DICT2GE( dict ) );
        dict = next;
    }
}


/***************************************************************************/
/*  search tag entry in specified dictionary                               */
/*  returns ptr to tag or NULL if not found                                */
/***************************************************************************/

gtentry     *find_user_tag( tag_dict *pdict, const char *tagname )
{
    tag_dict    dict;
    unsigned    taglen;

    taglen = strlen( tagname );
    for( dict = *pdict; dict != NULL; dict = dict->next ) {
        if( DICT2GE( dict )->taglen == taglen ) {
            if( strcmp( DICT2GE( dict )->tagname, tagname ) == 0 ) {
                return( DICT2GE( dict ) );
            }
        }
    }
    return( NULL );
}


/***************************************************************************/
/*  print_val_entry   print single GML tag  attribute  value               */
/***************************************************************************/

static  void    print_val_entry( gavalentry *wk )
{
    gavalflags          flags;
    int                 find;
    char                opt[256];
    static const char   v_txt[8] [11] =
                    {
                        { "default "   },
                        { "any "       },
                        { "length "    },
                        { "range "     },
                        { ""           },   // value
                        { ""           },   // valptr
                        { "automatic " },
                        { "reset "     },
                    };

    if( wk == NULL ) {
        return;                         // nothing to print
    }
    opt[0] = '\0';
    flags = wk->valflags;
    find = 0;
    while( flags > 0 ) {
        if( flags & 1 ) {
            strcat( opt, v_txt[find] );
        }
        find++;
        flags >>= 1;
    }
//    out_msg( "val:        %-10.10s %s\n", "jaja", opt );
    flags = wk->valflags;

    if( flags & GAVAL_range ) {
        if( flags & GAVAL_def ) {
            sprintf( opt, "default=%d %d min=%d max=%d",
                     wk->a.range[2], wk->a.range[3],
                     wk->a.range[0], wk->a.range[1] );
        } else {
            sprintf( opt, "min=%d max=%d", wk->a.range[0],
                     wk->a.range[1] );
        }
        out_msg( "val:        %-10.10s %s\n", " ", opt );

    } else if( flags & GAVAL_length ) {

        sprintf( opt, "length=%d\n", wk->a.range[0] );
        out_msg( "val:        %-10.10s %s\n", " ", opt );

    } else if( flags & GAVAL_any ) {

        if( flags & GAVAL_value ) {

            out_msg( "val:        %-10.10s any use='%s'\n", "", wk->a.value );

        } else if( flags & GAVAL_valptr ) {

            out_msg( "val:        %-10.10s any use='%s'\n", " ", wk->a.valptr );
        }
    } else if( flags & GAVAL_auto ) {

        if( flags & GAVAL_value ) {

            out_msg( "val:        %-10.10s automatic use='%s'\n", "", wk->a.value );

        } else if( flags & GAVAL_valptr ) {

            out_msg( "val:        %-10.10s automatic use='%s'\n", " ", wk->a.valptr );
        }
    } else if( flags & GAVAL_value ) {

        if( flags & GAVAL_def ) {
            strcpy( opt, "default" );
        } else {
            opt[0] = '\0';
        }
        out_msg( "val:        %-10.10s %s\n", wk->a.value, opt );

    } else if( flags & GAVAL_valptr ) {

        if( flags & GAVAL_def ) {
            strcpy( opt, "default" );
        } else {
            opt[0] = '\0';
        }
        out_msg( "val:        %-10.10s %s '%s'\n", " ", opt, wk->a.valptr );

    }
    return;
}


/***************************************************************************/
/*  print_att_entry   print single GML tag  attribute                      */
/***************************************************************************/

static  void    print_att_entry( gaentry *wk )
{
    gavalentry      *   gaval;
    unsigned            flags;
    int                 find;
    char                opt[256];
    static const char   a_txt[7] [11] =
                    {
                        { "default "   },
                        { "range "     },
                        { "automatic " },
                        { "any "       },
                        { "required "  },
                        { "uppercase " },
                        { "off "       },
                    };

    if( wk == NULL ) {
        return;                         // nothing to print
    }
    opt[0] = '\0';
    flags = wk->attflags & ~GAFLG_proc_all;   // clear processing flags
    find = 0;
    while( flags > 0 ) {
        if( flags & 1 ) {
            strcat( opt, a_txt[find] );
        }
        find++;
        flags >>= 1;
    }
    out_msg( "att:     %-13.13s %s\n", wk->attname, opt );
    for( gaval = wk->vals; gaval != NULL; gaval = gaval->next ) {
        print_val_entry( gaval );
    }
    return;
}

/***************************************************************************/
/*  print_tag_entry   print single GML tag                                 */
/***************************************************************************/

void    print_tag_entry( const gtentry *wk )
{
    gaentry         *   gawk;
    unsigned            flags;
    int                 find;
    char                opt[256];
    static const char   * const t_txt[] = {
        "attributes ",
        "continue ",
        "nocontinue ",
        "csoff ",
        "tagnext ",
        "textdef ",
        "texterror ",
        "textline ",
        "textreqd ",
        "off "
    };

    opt[0] = '\0';
    flags = wk->tagflags;
    find = 0;
    while( flags > 0 ) {
        if( flags & 1 ) {
            strcat( opt, t_txt[find] );
        }
        find++;
        flags >>= 1;
    }
    out_msg( "tag:  %-16.16s tagcount=%u macro=%s %s\n", wk->tagname,
             wk->usecount, wk->macname, opt );
    for( gawk = wk->attribs; gawk != NULL; gawk = gawk->next ) {
        print_att_entry( gawk );
    }
    out_msg( "\n" );
    return;
}


/***************************************************************************/
/*  print_tag_dict  output all of the user tag dictionary                  */
/***************************************************************************/

void    print_tag_dict( tag_dict dict )
{
    int         cnt;

    cnt = 0;
    out_msg( "\nList of defined User GML tags:\n" );
    for( ; dict != NULL; dict = dict->next ) {
        print_tag_entry( DICT2GE( dict ) );
        cnt++;
    }
    out_msg( "\nTotal GML tags defined: %d\n", cnt );
    return;
}

