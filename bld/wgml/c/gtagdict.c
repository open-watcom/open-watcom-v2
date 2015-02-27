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
* Description:  Implements user GML tags (tables and access routines)
*
****************************************************************************/

#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*  init_tag_dict   initialize dictionary pointer                          */
/***************************************************************************/

void    init_tag_dict( gtentry * * dict )
{
    *dict = NULL;
    return;
}


/***************************************************************************/
/*  add_tag     add tag entry to dictionary                                */
/*              if tag already defined error                               */
/***************************************************************************/

gtentry *   add_tag( gtentry * * dict, const char * name, const char * mac, const int flags )
{
    gtentry     *   ge;
    gtentry     *   wk;

    wk = find_tag( dict, name );
    if( wk != NULL ) {
        err_count++;
        g_err( err_tag_exist, name );
        g_info_inp_pos();
        show_include_stack();
        return( NULL );
    }

    ge = mem_alloc( sizeof( gtentry ) );

    ge->next = *dict;
    *dict = ge;

    memcpy( ge->name, name, sizeof( ge->name ) );
    ge->namelen = strlen( ge->name );
    strcpy( ge->macname, mac );
    ge->tagflags = flags;
    ge->attribs = NULL;
    ge->usecount = 0;

    return( ge );
}


/***************************************************************************/
/*  change_tag     change macro to execute in tag entry                    */
/***************************************************************************/

gtentry *   change_tag( gtentry * * dict, const char * name, const char * mac )
{
    gtentry     *   ge = NULL;

    if( *dict != NULL ) {
        ge = find_tag( dict, name );
        if( ge != NULL ) {
           strcpy( ge->macname, mac );
        }
    }
    return( ge );
}


/***************************************************************************/
/*  free_att  delete single attribute                                      */
/***************************************************************************/
static  void    free_att( gaentry * ga )
{
    gavalentry  *   vw;
    gavalentry  *   vwn;

    vw = ga->vals;
    while( vw != NULL ) {
        if( vw->valflags & val_valptr ) {
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

gtentry     *   free_tag( gtentry * * dict, gtentry * ge )
{
    gtentry     *   wk;
    gaentry     *   gaw;
    gaentry     *   gawn;


    if( ge == NULL ) {                  // nothing to delete
        return( NULL );
    }
    if( *dict == ge ) {                 // delete first entry
        *dict = ge->next;
        wk = NULL;                      // previous is null
    } else {
        for( wk = *dict; wk != NULL; wk = wk->next ) {
            if( wk->next == ge ) {
                wk->next = ge->next;    // chain update
                break;
            }
        }
    }
    gaw = ge->attribs;
    while( gaw != NULL ) {              // delete all attributes
        gawn = gaw->next;
        free_att( gaw );
        gaw = gawn;
    }
    mem_free( ge );                     // now the entry itself
    return( wk );                       // return previous entry or NULL
}


/***************************************************************************/
/*  free_tag_dict   free all user tag dictionary entries                   */
/***************************************************************************/

void    free_tag_dict( gtentry * * dict )
{
    gtentry     *   gtw;
    gtentry     *   gtwn;

    gtw = *dict;
    while( gtw != NULL ) {
        gtwn = gtw->next;;
        free_tag( dict, gtw );
        gtw = gtwn;
    }
    return;
}


/***************************************************************************/
/*  search tag entry in specified dictionary                               */
/*  returns ptr to tag or NULL if not found                                */
/***************************************************************************/

gtentry     *   find_tag( gtentry * * dict, const char * name )
{
    gtentry     *   wk;

    for( wk = *dict; wk != NULL; wk = wk->next ) {
        if( !stricmp( wk->name, name ) ) {
            break;
        }
    }
    return( wk );
}


/***************************************************************************/
/*  print_val_entry   print single GML tag  attribute  value               */
/***************************************************************************/

static  void    print_val_entry( gavalentry *wk )
{
    unsigned            flags;
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

    if( flags & val_range ) {
        if( flags & val_def ) {
            sprintf( opt, "default=%ld %ld min=%ld max=%ld",
                     wk->a.range[2], wk->a.range[3],
                     wk->a.range[0], wk->a.range[1] );
        } else {
            sprintf( opt, "min=%ld max=%ld", wk->a.range[0],
                     wk->a.range[1] );
        }
        out_msg( "val:        %-10.10s %s\n", " ", opt );

    } else if( flags & val_length ) {

        sprintf( opt, "length=%ld\n", wk->a.range[0] );
        out_msg( "val:        %-10.10s %s\n", " ", opt );

    } else if( flags & val_any ) {

        if( flags & val_value ) {

            out_msg( "val:        %-10.10s any use='%s'\n", "", wk->a.value );

        } else if( flags & val_valptr ) {

            out_msg( "val:        %-10.10s any use='%s'\n", " ", wk->a.valptr );
        }
    } else if( flags & val_auto ) {

        if( flags & val_value ) {

            out_msg( "val:        %-10.10s automatic use='%s'\n", "", wk->a.value );

        } else if( flags & val_valptr ) {

            out_msg( "val:        %-10.10s automatic use='%s'\n", " ", wk->a.valptr );
        }
    } else if( flags & val_value ) {

        if( flags & val_def ) {
            strcpy( opt, "default" );
        } else {
            opt[0] = '\0';
        }
        out_msg( "val:        %-10.10s %s\n", wk->a.value, opt );

    } else if( flags & val_valptr ) {

        if( flags & val_def ) {
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
    flags = wk->attflags & ~att_proc_all;   // clear processing flags
    find = 0;
    while( flags > 0 ) {
        if( flags & 1 ) {
            strcat( opt, a_txt[find] );
        }
        find++;
        flags >>= 1;
    }
    out_msg( "att:     %-13.13s %s\n", wk->name, opt );
    for( gaval = wk->vals; gaval != NULL; gaval = gaval->next ) {
        print_val_entry( gaval );
    }
    return;
}

/***************************************************************************/
/*  print_tag_entry   print single GML tag                                 */
/***************************************************************************/

void    print_tag_entry( gtentry * wk )
{
    gaentry         *   gawk;
    unsigned            flags;
    int                 find;
    char                opt[256];
    static const char   t_txt[10] [12] =
                    {
                        { "attributes " },
                        { "continue "   },
                        { "nocontinue " },
                        { "csoff "      },
                        { "tagnext "    },
                        { "textdef "    },
                        { "texterror "  },
                        { "textline "   },
                        { "textreqd "   },
                        { "off "        }
                    };

    if( wk == NULL ) {
        return;                         // nothing to print
    }
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
    out_msg( "tag:  %-16.16s tagcount=%d macro=%s %s\n", wk->name,
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

void    print_tag_dict( gtentry * dict )
{
    gtentry         *   wk;
    int                 cnt;

    cnt = 0;
    out_msg( "\nList of defined User GML tags:\n" );
    for( wk = dict; wk != NULL; wk = wk->next ) {
        print_tag_entry( wk );
        cnt++;
    }
    out_msg( "\nTotal GML tags defined: %d\n", cnt );
    return;
}

