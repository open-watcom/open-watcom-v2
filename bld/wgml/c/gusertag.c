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


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*  clear and set the relevant attribute parametercheck flags              */
/***************************************************************************/

static gaflags set_att_proc_flags( gaflags attflags )
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
static void add_defaults_to_dict( gtentry * ge, symdict_hdl dict )
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

static bool check_att_value( gaentry * ga, gtentry * ge, symdict_hdl loc_dict )
{
    gavalentry  *   gaval;
    char        *   valp;
    bool            msg_done;
    int             rc;

    (void)ge;

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
            if( strcmp( token_buf, valp ) == 0 ) {
                scan_err = false;       // value is allowed
                break;
            }
        } else {
            if( gaval->valflags & val_range ) {
                long    attval;

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
        rc = add_symvar( loc_dict, ga->name, token_buf,
                         no_subscript, local_var );
    } else {
        if( !msg_done ) {
            xx_err_cc( err_att_val, token_buf, ga->name );
        }
    }
    return( scan_err );
}


/***************************************************************************/
/*  process user defined GML tag                                           */
/*  scan the line, prepare arguments and call processing macro             */
/***************************************************************************/

bool process_tag( gtentry * ge, mac_entry * me )
{
    bool            processed;
    char            longwork[NUM2STR_LENGTH];
    char        *   p;
    char        *   p2;
    char        *   pa;
    char            quote;
    gaentry     *   ga;
    gavalentry  *   gaval;
    inp_line    *   pline;
    int             rc;
    size_t          len;
    symdict_hdl     loc_dict;   // for preparing local vars

    processed = true;           // return value, always true
    init_dict( &loc_dict );

    add_defaults_to_dict( ge, loc_dict );

    /***********************************************************************/
    /*  scan input for attributes and / or tag-text                        */
    /***********************************************************************/

    p = tok_start + ge->namelen + 1;    // over tagname

    if( ge->tagflags & tag_textline ) {

        /***********************************************************************/
        /*  TEXTLine treats everything after the tag as tag-text               */
        /***********************************************************************/

        if( input_cbs->hidden_head != NULL ) {
            pline = input_cbs->hidden_head;
            input_cbs->hidden_head = input_cbs->hidden_head->next;
            len = strlen( pline->value );
            p2 = p;
            while( *p != '\0' ) {
                p++;
            }
            if( *(p - 1) == CONT_char ) {    // remove continue character if present
                p--;
                *p = '\0';
            }
            strcpy( p, pline->value );
            mem_free( pline );
            p = p2;
        }
    } else if( ge->attribs != NULL && (ge->tagflags & tag_attr) ) {

        /***********************************************************************/
        /*  only process attributes if ATTribute was used and at least one     */
        /*  attribute was actually defined                                     */
        /***********************************************************************/

        while( *p == ' ' ) {        // not yet end of tag, process attributes

            SkipSpaces( p );        // over WS to attribute
            if( *p == '.' ) {
                break;
            }
            p2 = token_buf;
            pa = p;
            while( is_macro_char( *p ) ) {
                *p2++ = *p++;
            }
            *p2 = '\0';
            if( p2 != token_buf ) {     // ignore nullstring
                for( ga = ge->attribs; ga != NULL; ga = ga->next ) {// all attrs
                    if( !stricmp( ga->name, token_buf ) ) {
                        ga->attflags |= att_proc_seen; // attribute specified
                        if( ga->attflags & att_auto ) {
                            xx_line_err_cc( err_auto_att, token_buf, pa );
                        }

                        if( is_space_tab_char( *p ) ) { // no whitespace allowed before '='
                            xx_line_err_cc( err_no_att_val, token_buf, p );
                        }

                        /* no line end allowed before '=' except with TEXTLine */
                        if( (*p == '\0') && !(ge->tagflags & tag_textline) ) {
                            xx_line_err_cc( err_no_att_val, token_buf, p );
                        }

                        if( *p == '=' ) {   // value follows

                            p++;            // over =

                            if( is_space_tab_char( *p ) ) { // no whitespace allowed after '='
                                xx_line_err_cc( err_no_att_val, token_buf, p );
                            }

                            ga->attflags |= att_proc_val;
                            p2 = token_buf;
                            if( is_quote_char( *p ) ) {
                                quote = *p++;
                                while( *p != '\0' && *p != quote ) {// quoted value
                                    *p2++ = *p++;
                                }
                                if( *p == quote ) {
                                    p++;// over ending quote
                                }
                            } else {
                                quote = '\0';
                                while( *p != '\0' && (*p != ' ') && (*p != '.') ) {
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

                            scan_err = check_att_value( ga, ge, loc_dict );

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
                                     rc = add_symvar( loc_dict, ga->name,
                                                      token_buf, no_subscript,
                                                      local_var );
                                }
                            }
                        }
                        break;
                    }
                }
                if( ga == NULL ) {      // supposed attribute not found
                    p = pa;
                    xx_line_warn_cc( wng_att_name, token_buf, pa );
                }
            } else {
                if( *p != '\0' ) {
                    xx_line_warn_cc( wng_att_name, p, pa );
                }
            }

            /***************************************************************/
            /*  check for tag end .                                        */
            /***************************************************************/
            if( *p == ' ' ) {
                continue;               // not yet at buffer / tag end
            }

            /***************************************************************/
            /*  unlike predefined tags, user-defined tags never try to     */
            /*  find additional parameters on the next line                */
            /***************************************************************/

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
            att_req_err( ge->name, token_buf );
        }
    } else if( ge->tagflags & tag_attr ) {
        p2 = p;                                 // save value
        SkipSpaces( p );
        if( (*p != '.') && (*p != '\0') ) {
            xx_line_warn_cc( wng_att_name, p, p );
        }
        p = p2;                                 // restore value
    }

    /********************************************************************/
    /*  at this point, the end of the tag has been reached              */
    /*    if it ends in '\0', then there is no tag-text                 */
    /*    if it ends in '.', then the dot must be skipped               */
    /*    if it ends in ' ', then the tag-text follows                  */
    /*  the ultimate goal here is to set local variable * correctly     */
    /********************************************************************/

    p2 = p;                         // p2 saves the start value for p
    SkipSpaces( p );                // skip spaces before the '.', if one is present
    if( (*p2 != '.') && (p2 != p) && (*p == '.') ) {
        p2 = p;                     // reset p2 to '.' ending tag
    }
    SkipDot( p );                   // skip the '.', if present
    SkipSpaces( p );                // if '.' was present, skip to first non-space character
    if( *p == '\0' ) {              // no tag-text found

        /* If TEXTReqrd was used in defining the user-tag, this is an error */

        if( ge->tagflags & tag_textreq ) {  // text must be present
            xx_line_err_cc( err_att_text_req, ge->name, p2 );
        }

        /* Otherwise, the value of * will be an empty string */

    } else {                        // tag-text found

        /* If TEXTError was used in defining the user-tag, this is an error */

        if( ge->tagflags & tag_texterr ) {  // no text allowed
            xx_line_err_cc( err_att_text, ge->name, p );
        }

        /* Otherwise, things get a bit complicated */

        p = p2;                                     // reset
        if( *p2 == '.' ) {
            SkipDot( p );
        } else {
            if( (ge->tagflags & tag_attr) && !(ge->tagflags & tag_textline) ) {
                SkipSpaces( p );
            }
        }

        // remove trailing spaces
        len = strlen( p );
        if( !ProcFlags.null_value && (len != 0) && (input_cbs->hidden_head == NULL) ) {
            while( *(p + len - 1) == ' ' ) {        // remove trailing spaces
                len--;
                if( len == 0 ) {                    // empty operand
                    break;
                }
            }
            *(p + len) = '\0';                      // end after last non-space character
        }

        // remove trailing continue character if tag has NOCONTinue option
        if( (ge->tagflags & tag_nocont) ) {
            len = strlen( p );
            if( *(p + len - 1) == CONT_char ) {
                len--;
            }
            *(p + len) = '\0';
        }
    }
    strcpy( token_buf, p );
    rc = add_symvar( loc_dict, "_", token_buf, no_subscript, local_var );
    p += strlen( token_buf );

    scan_start = p + 1;             // all processed
    if( *scan_start == ' ' ){
        SkipSpaces( scan_start );   // do not treat spaces removed from end as text
    }

    /*******************************************************************/
    /*  add standard symbols to dict                                   */
    /*******************************************************************/

    rc = add_symvar( loc_dict, "_tag", ge->name, no_subscript, local_var );
    ge->usecount++;
    sprintf( longwork, "%u", ge->usecount );
    rc = add_symvar( loc_dict, "_n", longwork, no_subscript, local_var );

    add_macro_cb_entry( me, ge );   // prepare GML macro as input
    free_dict( &input_cbs->local_dict );    // not super efficient
    input_cbs->local_dict = loc_dict;
    input_cbs->fm_hh = input_cbs->prev->fm_hh;
    input_cbs->hh_tag = input_cbs->prev->hh_tag;
    inc_inc_level();                // start new include level

    if( ge->tagflags & tag_cont ) {
        ProcFlags.utc = true;
    }

    if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass ) {
        print_sym_dict( input_cbs->local_dict );
    }

    return( processed );
}

