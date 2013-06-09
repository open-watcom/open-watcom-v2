/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  interpret GML user tag i.e., construct the corresponding
*               macro call from the tag parameters
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

static  symvar  *   loc_dict;           // for preparing local vars


/***************************************************************************/
/*  clear and set the relevant attribute parametercheck flags              */
/*                                                                         */
/***************************************************************************/

static  gaflags set_att_proc_flags( gaflags attflags )
{
    gaflags fl = attflags & ~att_proc_all;

    if( fl & att_auto ) {
        fl |= att_proc_auto;
    }
    if( fl & att_req ) {
        fl |= att_proc_req;
    }
    return( fl );
}

/***************************************************************************/
/*  add attribute default values to dict                                   */
/***************************************************************************/
static  void    add_defaults_to_dict( gtentry * ge, symvar * * dict )
{
    gaentry     *   ga;
    gavalentry  *   gaval;
    char        *   valp;
    int             rc;

    for( ga = ge->attribs; ga != NULL; ga = ga->next ) {// for all attributes

        ga->attflags = set_att_proc_flags( ga->attflags );
        if( ga->attflags & att_off ) {  // attribute inactive
            continue;
        }
        if( ga->attflags & (att_def | att_auto) ) {  // with default
            for( gaval = ga->vals; gaval != NULL; gaval = gaval->next ) {
                if( gaval->valflags & (val_def | val_auto) ) {// value is default
                    valp = NULL;
                    if( gaval->valflags & val_value ) { // short string
                        valp = gaval->a.value;
                    } else {
                        if( gaval->valflags & val_valptr ) {// long string
                            valp = gaval->a.valptr;
                        }
                    }
                    if( gaval->valflags & val_range ) { // range default
                        sprintf( token_buf, "%d", gaval->a.range[2] );
                        valp = token_buf;
                    }
                    if( valp != NULL ) {
                        rc = add_symvar( dict, ga->name, valp,
                                         no_subscript, local_var );
                    }
                }
            }
        }
    }
    return;
}


/***************************************************************************/
/*  check the attribute value against restrictions                         */
/***************************************************************************/

static bool check_att_value( gaentry * ga )
{
    gavalentry  *   gaval;
    char        *   valp;
    long            attval;
    bool            msg_done;
    int             rc;

    scan_err = true;
    msg_done = false;
    for( gaval = ga->vals; gaval != NULL; gaval = gaval->next ) {
        if( gaval->valflags & val_any ) {
            scan_err = false;           // any value is allowed
            break;
        }

        valp = NULL;
        if( gaval->valflags & val_value ) {
            valp = gaval->a.value;
        } else {
            if( gaval->valflags & val_valptr ) {
                valp = gaval->a.valptr;
            }
        }
        if( valp != NULL ) {
            if( !strcmp( token_buf, valp ) ) {
                scan_err = false;       // value is allowed
                break;
            }
        } else {
            if( gaval->valflags & val_range ) {
                attval = strtol( token_buf, NULL, 10 );
                if( attval < gaval->a.range[0] ||
                    attval > gaval->a.range[1]  ) {
                    xx_err( ERR_ATT_RANGE_INV );// value outside range
                    msg_done = true;
                    break;
                }
            } else {
                if( gaval->valflags & val_length ) {
                    if( strlen( token_buf ) > gaval->a.length ) {
                        xx_err( err_att_len_inv );  // value too long
                        msg_done = true;
                    } else {
                        scan_err = false;
                    }
                    break;
                }
            }
        }
    }
    if( !scan_err ) {
        rc = add_symvar( &loc_dict, ga->name, token_buf,
                         no_subscript, local_var );
    } else {
        if( !msg_done ) {
            att_val_err( ga->name );
        }
    }
    return( scan_err );
}


/***************************************************************************/
/*  process user defined GML tag                                           */
/*  scan the line, prepare arguments and call processing macro             */
/***************************************************************************/

bool        process_tag( gtentry * ge, mac_entry * me )
{
    bool            processed;
    gaentry     *   ga;
    gavalentry  *   gaval;
    char        *   p;
    char        *   p2;
    int             rc;
    char            quote;
    char            longwork[20];
    bool            tag_end_found = false;

    processed = true;
    init_dict( &loc_dict );

    add_defaults_to_dict( ge, &loc_dict );

    /***********************************************************************/
    /*  scan input for attributes and / or tagtext                         */
    /***********************************************************************/

    p = tok_start + ge->namelen + 1;    // over tagname

    if( ge->attribs != NULL ) {     // only process attributes if they exist
        while( *p == ' ' ) {        // not yet end of tag, process attributes

            while( *p == ' ' ) {        // over WS to attribute
                p++;
            }
            if( *p == '.' ) {
                tag_end_found = true;
                break;
            }
            p2 = token_buf;
            while( is_id_char( *p ) ) {
                *p2++ = *p++;
            }
            *p2 = '\0';
            if( p2 != token_buf ) {     // ignore nullstring
                for( ga = ge->attribs; ga != NULL; ga = ga->next ) {// all attrs
                    if( !stricmp( ga->name, token_buf ) ) {
                        ga->attflags |= att_proc_seen; // attribute specified
                        if( ga->attflags & att_auto ) {
                            auto_att_err();
                            break;
                        }

                        if( *p == '=' ) {   // value follows
                            ga->attflags |= att_proc_val;

                            p++;        // over =
                            p2 = token_buf;
                            if( is_quote_char( *p ) ) {
                                quote = *p++;
                                while( *p && *p != quote ) {// quoted value
                                    *p2++ = *p++;
                                }
                                if( *p == quote ) {
                                    p++;// over ending quote
                                }
                            } else {
                                quote = '\0';
                                while( *p && (*p != ' ') && (*p != '.') ) {
                                    *p2++ = *p++;
                                }
                            }
                            *p2 = '\0';
                            if( ga->attflags & att_off ) {// attribute inactive
                                continue;
                            }
                            if( ga->attflags & att_upper ) {// uppercase option
                                strupr( token_buf );
                            }

                            scan_err = check_att_value( ga );

                        } else {// special for range set default2 if no value
                            if( ga->attflags & att_range ) {
                                for( gaval = ga->vals; gaval != NULL;
                                     gaval = gaval->next ) {
                                     if( gaval->valflags & val_range ) {
                                        break;
                                     }
                                }
                                if( gaval != NULL ) {
                                     sprintf( token_buf, "%d",
                                              gaval->a.range[3] );
                                     rc = add_symvar( &loc_dict, ga->name,
                                                      token_buf, no_subscript,
                                                      local_var );
                                }
                            }
                        }
                        break;
                    }
                }
                if( ga == NULL ) {      // attribute Not found
                    char        linestr[MAX_L_AS_STR];

                    processed = false;
                    wng_count++;
                    //***WARNING*** SC--040: 'abd' is not a valid attribute name
                    g_warn( wng_att_name, token_buf );
                    if( input_cbs->fmflags & II_macro ) {
                        ultoa( input_cbs->s.m->lineno, linestr, 10 );
                        g_info( inf_mac_line, linestr, input_cbs->s.m->mac->name );
                    } else {
                        ultoa( input_cbs->s.f->lineno, linestr, 10 );
                        g_info( inf_file_line, linestr, input_cbs->s.f->filename );
                    }
                    show_include_stack();
                }
            }
            /***************************************************************/
            /*  check for tag end .                                        */
            /***************************************************************/
            if( *p == ' ' ) {
                continue;               // not yet at buffer / tag end
            }
#if 0
            /***************************************************************/
            /*  continue scanning for attriutes on next line if not tag end*/
            /*  This does not work for constructs such as                  */
            /*                                                             */
            /*  :hdref refid=diffs                                         */
            /*  .bd to determine if you need to recompile your application.*/
            /*  from docs\doc\gs\intro.gml line 37 f                       */
            /***************************************************************/

            if( *p != '.' ) { //
                if( get_line( true ) ) {
                    p = buff2;
                } else {
                    *p = '\0';
                }
            } else {
                tag_end_found = true;
            }
#else
            if( (*p == '.') || (*p == '\0') ) {
                tag_end_found = true;
            }
#endif
        }

        /*******************************************************************/
        /*  check for missing reqrd attributes                             */
        /*******************************************************************/
        *token_buf = '\0';
        for( ga = ge->attribs; ga != NULL; ga = ga->next ) {// for all attrs
            if( ga->attflags & att_req ) {
                if( !(ga->attflags & att_proc_seen) ) {
                    if( *token_buf != '\0' ) {
                        strcat( token_buf, " '" );
                    } else {
                        strcpy( token_buf, "'" );
                    }
                    strcat( token_buf, ga->name );
                    strcat( token_buf, "' " );
                }
            }
        }
        if( *token_buf != '\0' ) {      // some req attr missing
            char        linestr[MAX_L_AS_STR];

        // the errmsg in wgml 4.0 is wrong, it shows the macroname, not tag.
// ****ERROR**** SC--047: For the tag '@willi', the required attribute(s)
//                       'muss2'
//                       'muss'
//                       have not been specified

            processed = false;
            err_count++;
            g_err( err_att_req, ge->name, token_buf );
            if( input_cbs->fmflags & II_macro ) {
                ultoa( input_cbs->s.m->lineno, linestr, 10 );
                g_info( inf_mac_line, linestr, input_cbs->s.m->mac->name );
            } else {
                ultoa( input_cbs->s.f->lineno, linestr, 10 );
                g_info( inf_file_line, linestr, input_cbs->s.f->filename );
            }
            show_include_stack();
        }

        if( *p == '.' ) {               // does text follow tag end
            if( strlen( p + 1 ) > 0 ) {
                if( ge->tagflags & tag_texterr ) { // no text allowed
                    tag_text_err( ge->name );
                    processed = false;
                }
            } else {
                if( ge->tagflags & tag_textreq ) {  // reqrd text missing
                    tag_text_req_err( ge->name );
                    processed = false;
                }
            }
            strcpy( token_buf, p + 1 );
            rc = add_symvar( &loc_dict, "_", token_buf, no_subscript, local_var );
            p += strlen( token_buf );
        }

        scan_start = p + 1;             // all processed
        /*******************************************************************/
        /*  add standard symbols to dict                                   */
        /*******************************************************************/

        rc = add_symvar( &loc_dict, "_tag", ge->name, no_subscript, local_var );
        ge->usecount++;
        sprintf( longwork, "%d", ge->usecount );
        rc = add_symvar( &loc_dict, "_n", longwork, no_subscript, local_var );


        add_macro_cb_entry( me, ge );   // prepare GML macro as input
        input_cbs->local_dict = loc_dict;
        inc_inc_level();                // start new include level
        if( ge->tagflags & tag_cont ) {   // +++++++++++++++++++ TBD trial
            post_space = 0;
            ProcFlags.ct = true;
        }

        if( input_cbs->fmflags & II_research && GlobalFlags.firstpass ) {
            print_sym_dict( input_cbs->local_dict );
        }
    } else {                            // user-defined tag has no attributes
        if( ge->tagflags & tag_texterr ) {  // no text allowed
            // '.' or CW_sep_char immediately after the tag does not count as text
            if( (*p == '.') || (*p == CW_sep_char ) ) {
                p++;
            }
            while( *p == ' ' ) {        // spaces don't count as text
                p++;
            }
            if( *p ) {                  // text found
                tag_text_err( ge->name );
                processed = false;
                return( processed );
            }
        }
        if( ge->tagflags & tag_textreq ) {  // text is required
            // per wgml 4.0 behavior
            if( *p == CW_sep_char ) {
                processed = false;
                return( processed );
            }
            // '.' immediately after the tag does not count as text
            if( *p == '.' ) {
                p++;
            }
            while( *p == ' ' ) {        // spaces don't count as text
                p++;
            }
            if( !*p ) {                 // no text found
                tag_text_req_err( ge->name );
                processed = false;
                return( processed );
            }
        }
        // per wgml 4.0 behavior
        if( *p == CW_sep_char ) {
            processed = false;
            return( processed );
        }
        // '.' immediately after the tag is not passed to the macro
        if( *p == '.' ) {
            p++;
        }
        strcpy( token_buf, p );
        rc = add_symvar( &loc_dict, "_", token_buf, no_subscript, local_var );
        p += strlen( token_buf );

        scan_start = p + 1;             // all processed
        /*******************************************************************/
        /*  add standard symbols to dict                                   */
        /*******************************************************************/

        rc = add_symvar( &loc_dict, "_tag", ge->name, no_subscript, local_var );
        ge->usecount++;
        sprintf( longwork, "%d", ge->usecount );
        rc = add_symvar( &loc_dict, "_n", longwork, no_subscript, local_var );


        add_macro_cb_entry( me, ge );   // prepare GML macro as input
        input_cbs->local_dict = loc_dict;
        inc_inc_level();                // start new include level
        if( ge->tagflags & tag_cont ) { // +++++++++++++++++++ TBD trial
            post_space = 0;
            ProcFlags.ct = true;
        }

        if( input_cbs->fmflags & II_research && GlobalFlags.firstpass ) {
            print_sym_dict( input_cbs->local_dict );
        }
    }

    return( processed );
}

