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
* Description:  WGML input line processing
*   some logic / ideas adopted from Watcom Script 3.2 IBM S/360 Assembler
*   see comment in wgml.c
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


static  bool        sym_space;          // compiler workaround

static char *get_func_name( const char *p, char *funcname )
{
    int     i;
    char        buf[FUNC_NAME_LENGTH + 1];

    if( funcname == NULL )
        funcname = buf;
    i = 0;
    while( is_function_char( *p ) ) {
        if( i < FUNC_NAME_LENGTH ) {
            funcname[i++] = my_tolower( *p );
        }
        p++;
    }
    funcname[i] = '\0';
    return( (char *)p );
}

/***************************************************************************/
/*  perform symbol substitution on certain special symbols:                */
/*    AMP (identified by the is_AMP tag)                                   */
/*  all other items starting with '&' are treated as text                  */
/***************************************************************************/
void process_late_subst( char * buf )
{
    char            *p;
    char            *symstart;
    char            tail[BUF_SIZE + 1];
    char            *tokenstart;     // save position of current '&'
    int             rc;
    sub_index       var_ind;        // subscript value (if symbol is subscripted)
    symsub          *symsubval;      // value of symbol
    symvar          symvar_entry;

    p = buf; // look for & in buffer
    while( (p = strchr( p, ampchar )) != NULL ) {        // & found
        if( p[1] == ' ' ) { // not a symbol substition, attribute, or function
            p++;
        } else if( my_isalpha( p[1] )
          && p[2] == '\''
          && p[3] > ' ' ) {     // attribute
            p += 3;
        } else if( p[1] == '\'' ) {         // function or text
            p = get_func_name( p + 2, NULL );   // find end of function name
        } else {                                // symbol
            tokenstart = p;
            p++;                                // over '&'
            symstart = p;                       // remember start of symbol name
            g_scan_err = false;
            ProcFlags.suppress_msg = true;
            p = scan_sym( symstart, &symvar_entry, &var_ind, NULL, false );
            ProcFlags.suppress_msg = false;
            if( !g_scan_err ) {                   // potentially qualifying symbol
                rc = find_symvar_sym( &symvar_entry, var_ind, &symsubval );
                if( rc == 2 ) {             // variable found + resolved
                    if( symsubval->base->flags & SF_is_AMP ) {
                        /* replace symbol with value */
                        strcpy( tail, p );       // copy tail
                        p = tokenstart;
                        strcpy( p, symsubval->value );  // copy value
                        if( tail[0] == '.' ) {
                            strcat( buf, tail + 1);     // append tail to buf, skipping initial "."
                        } else {
                            strcat( buf, tail );        // append tail to buf
                        }
                    }
                }
            }
        }
    }
    return;
}


/*  split_input
 *  The (physical) line is split
 *  The second part will be processed by the next getline()
 *  pushing any already split part down
 *
 */

void split_input( char *buf, char *split_pos, i_flags fmflags )
{
    inp_line        *wk;
    unsigned        len;

    (void)buf;

    len = strlen( split_pos );          // length of second part
    if( len > 0 ) {
        wk = mem_alloc( len + sizeof( inp_line ) );
        wk->next = input_cbs->hidden_head;
        wk->fmflags  = fmflags;
        wk->fm_symbol = false;
        wk->hh_tag = input_cbs->hh_tag;
        wk->ip_start = false;
        wk->sym_space = false;
        input_cbs->hh_tag = false;
        strcpy( wk->value, split_pos ); // save second part

        input_cbs->hidden_head = wk;
        if( input_cbs->hidden_tail == NULL ) {
            input_cbs->hidden_tail = wk;
        }

        *split_pos = '\0';              // terminate first part
        ProcFlags.indented_text = false;// split done
    }
    return;
}


/*  split_input_var
 *  The second part is constructed from 2 parts
 *  used if a substituted variable starts with cw_sep_char
 */

static void split_input_var( char *buf, char *split_pos, char *part2, i_flags fmflags )
{
    inp_line        *wk;
    unsigned        len;

    (void)buf;

    len = strlen( split_pos ) + strlen( part2 );// length of second part
    if( len > 0 ) {
        wk = mem_alloc( len + sizeof( inp_line ) );
        wk->next = input_cbs->hidden_head;
        wk->fmflags = fmflags;
        wk->fm_symbol = false;
        wk->sym_space = sym_space;
        wk->hh_tag = input_cbs->hh_tag;

        strcpy(wk->value, part2 );      // second part
        strcat(wk->value, split_pos );  // second part

        input_cbs->hidden_head = wk;
        if( input_cbs->hidden_tail == NULL ) {
            input_cbs->hidden_tail = wk;
        }
    }
    return;
}


/***************************************************************************/
/*  look for GML tag start character and split line if valid GML tag       */
/*  don't split if blank follows gml_char                                  */
/*  special for  xxx::::TAG construct                                      */
/*  don't split if line starts with :CMT.                                  */
/***************************************************************************/
static void split_at_GML_tag( void )
{
    char            *p;
    char            *p2;
    char            *pchar;
    const gmltag    *gle = NULL;             // GML layout tag entry
    const gmltag    *gse = NULL;             // GML system tag entry
    const gtentry   *gue = NULL;             // GML user tag entry
    char            tagname[TAG_NAME_LENGTH + 1];

    /***********************************************************************/
    /*  Look for GML tag start char(s) until a known tag is found          */
    /*  then split the line                                                */
    /***********************************************************************/

    pchar = buff2;
    while( (pchar = strchr( pchar + 1, GML_char )) != NULL ) {
        while( pchar[1] == GML_char ) {
            pchar++;                    // handle repeated GML_chars
        }
        p2 = check_tagname( pchar, tagname );
        if( p2 != NULL ) {
            if( strcmp( "CMT", tagname ) == 0 ) {
                /* is  comment */
                *pchar = '\0';
                return;
            }
            if( ProcFlags.layout
              && (*p2 == '\t') ) {
                *p2 = ' ';              // replace tab with space in layout
            }
            input_cbs->hh_tag = false;  // clear before testing
            /***************************************************************/
            /* Verify valid user or system tag                             */
            /***************************************************************/
            if( ( (gue = find_user_tag( &tags_dict, tagname )) != NULL )
              || ( (gse = find_sys_tag( tagname )) != NULL )
              || ( (gle = find_lay_tag( tagname )) != NULL ) ) {

                if( !input_cbs->fm_hh ) {
                    // remove spaces before tags at sol in restricted sections
                    // in or just before LAYOUT tag
                    if( (rs_loc > 0)
                      || ProcFlags.layout
                      || strcmp( "LAYOUT", tagname ) == 0 ) {
                        p = buff2;
                        SkipSpacesTabs( p );
                        if( p == pchar ) {  // only leading blanks
                            memmove( buff2, pchar, BUF_SIZE - (p - buff2) + 1 );
                            buff2_lg = strlen( buff2 ); // new length
                            pchar = buff2;
                            continue;       // dummy split done try again
                        }
                    }
                }
                split_input( buff2, pchar, input_cbs->fmflags );    // split at GML tag
                input_cbs->hidden_head->hh_tag = true;  // not just text starting with GML_char
                p = input_cbs->hidden_head->value + 1;
                p2 = token_buf;
                while( is_macro_char( *p ) ) {
                    *p2++ = *p++;
                }
                *p2 = '\0';
                /* inserting CONT_chars for layout tags causes errors */
                if( (gse != NULL)
                  || ((gue != NULL)
                  && (gue->tagflags & GTFLG_cont)) ) {
                    pchar[0] = CONT_char;
                    pchar[1] = '\0';
                }
                if( ((gse != NULL)
                  && (gse->tagclass & TCLS_ip_start)) ) {
                    input_cbs->hidden_head->ip_start = true;
                }
                if( ProcFlags.literal ) {   // if literal active
                    if( li_cnt < INT_MAX ) {// we decrement, adjust for split line
                        li_cnt++;
                    }
                }
                break;                  // we did a split stop now
            }
        }
    }
}


/***************************************************************************/
/*   Split input line at GML tag                                           */
/*   returns false if :cmt .* .dm found and line should be skipped         */
/***************************************************************************/

static bool split_input_buffer( void )
{
    char            *p;
    char            tagname[TAG_NAME_LENGTH + 1];
    bool            comment;

    /***********************************************************************/
    /*  look for GML tag start character and split line at GML tag         */
    /*  special processing for some script control lines                   */
    /***********************************************************************/

    comment = false;
    if( check_tagname( buff2, tagname ) != NULL ) {
        if( strcmp( "CMT", tagname ) == 0 ) {
            comment = true;
        }
    }
    if( !comment ) {
        split_at_GML_tag();
    }

    if( !ProcFlags.literal ) {

        /*******************************************************************/
        /* for :cmt. minimal processing                                    */
        /*******************************************************************/

        if( comment ) {
            return( false );
        }

        /*******************************************************************/
        /*  .xx SCRIPT control line                                        */
        /*******************************************************************/
        p = buff2;
        if( (*p++ == SCR_char) ) {
            if( p[0] == '*'
              || p[0] == '\'' && p[1] == '*' ) {
                return( false );  // for .* comment minimal processing
            }
            /***************************************************************/
            /* if macro define (.dm xxx ... ) supress variable substitution*/
            /* for the sake of single line macro definition                */
            /* .dm xxx / &*1 / &*2 / &*0 / &* /                            */
            /*  and                                                        */
            /* ..dm xxx / &*1 / &*2 / &*0 / &* /                           */
            /***************************************************************/
            if( *p == SCR_char )
                p++;
            if( strnicmp( "dm ", p, 3 ) == 0 ) {
                return( false );
            }

            /***************************************************************/
            /*  for records starting  .' which were not indented or which  */
            /*  were indented but all indents were .'                      */
            /*  or if control word separator is '\0'                       */
            /*  ignore control word separator                              */
            /***************************************************************/

            if( (!ProcFlags.cw_force_sep
              && (*p == '\''))
              || (cw_sep_char == '\0') ) {
                ProcFlags.cw_sep_ignore = true;
            } else {
                ProcFlags.cw_sep_ignore = false;
            }
        } else if( ProcFlags.indented_text ) {  // text, but preceded by indent
            ProcFlags.cw_sep_ignore = false;
        }
    }
    return( true );                             // further processing needed
}


/****************************************************************************/
/*  set either or both of two ProcFlags if c_entry->value is all spaces     */
/*    ProcFlags.co_on_indent is set if the item is at the start of the      */
/*    physical input record                                                 */
/*    ProcFlags.pre_fsp is set if the item is at the end of the             */
/*    physical input record                                                 */
/*                                                                          */
/*    Note: this function is used by parse_r2l() only                       */
/*          it was removed because it is needed in three separate places    */
/*          in parse_r2l() and needed some effort to develop                */
/****************************************************************************/

static void set_space_flags( sym_list_entry * c_entry, char * buf )
{
    char            *p;

    if( *c_entry->value != '\0' ) {     // result must have a value
        p = c_entry->value;
        SkipSpaces( p );
        if( *p == '\0' ) {
            if( (!input_cbs->fm_hh)
              && (c_entry->orig.s == buf) ) {
                ProcFlags.co_on_indent = true;
            }
            p = c_entry->orig.e;           // this the byte after the symbol value
            SkipDot( p );
            if( (*p == '\0')
              && (input_cbs->hidden_head == NULL) ) {
                ProcFlags.pre_fsp = true;
            }
        }
    }
}

/***************************************************************************/
/*  parse the current input buffer from right to left, using the stack of  */
/*  sym_list_entry instances created previously                            */
/*  this is the algorithm for unwinding the stack:                         */
/*    if curr->type is not SL_text, then                                   */
/*      if curr->type is SL_split, then                                    */
/*        if subscript, exit immediately                                   */
/*        if not subscript, split the line                                 */
/*      otherwise                                                          */
/*        copy text from curr->orig.e on from buf to tail                  */
/*        copy the value from curr->value to curr->orig.s                  */
/*        if the first character in the tail is ".", then                  */
/*          append tail+1 to buf thus skipping the "."                     */
/*        otherwise append tail to buf                                     */
/*    otherwise do nothing                                                 */
/***************************************************************************/

static bool parse_r2l( sym_list_entry *stack, char *buf, bool subscript )
{
    char            *p;
    char            tail[BUF_SIZE + 1];
    unsigned        cw_lg;
    sym_list_entry  *curr;

    ProcFlags.co_on_indent = false;
    ProcFlags.substituted = false;
    tail[0] = '\0';

    for( curr = stack; curr != NULL; curr = curr->prev ) {
        switch( curr->type ) {
        case SL_text:
            if( subscript ) {
                ProcFlags.unresolved = true;
            }
            break;
        case SL_attrib:
        case SL_funct:
            set_space_flags( curr, buf );
            ProcFlags.substituted = true;
            strcpy( tail, curr->orig.e );      // copy tail
            p = curr->orig.s;
            strcpy( p, curr->value );       // copy value
            if( tail[0] == '.' ) {
                strcat( buf, tail + 1);     // append tail to buf, skipping initial "."
            } else {
                strcat( buf, tail );        // append tail to buf
            }
            break;
        case SL_symbol:
            set_space_flags( curr, buf );
            ProcFlags.substituted = true;
            if( !ProcFlags.if_cond
              && !ProcFlags.dd_macro
              && !curr->value[0] ) {
                ProcFlags.null_value = true;
            }
            strcpy( tail, curr->orig.e );      // copy tail
            p = curr->orig.s;
            /* If we're replacing &* with &*, we have a real problem. */
            if( ((p[0] == ampchar)
              && (p[1] == '*'))
              && ((curr->value[0] == ampchar)
              && (curr->value[1] == '*')) ) {
                internal_err_exit( __FILE__, __LINE__ );
                /* never return */
            }
            strcpy( p, curr->value );       // copy value
            if( tail[0] == '.' ) {
                strcat( buf, tail + 1);     // append tail to buf, skipping initial "."
            } else {
                strcat( buf, tail );        // append tail to buf
            }
            if( input_cbs->fm_symbol ) {
                /* keep value if from prior symbol which created its own logical input record */
                sym_space = input_cbs->sym_space;
            } else {
                sym_space = false;
                if( curr->orig.s == buf ) {              // symbol at start of input line
                    sym_space = true;
                } else {                                // symbol not at start of input line
                    if( *curr->value == !SCR_char ) {   // not an scw or macro
                        sym_space = (*(curr->orig.s - 1) == ' ');
                    }
                }
            }
            input_cbs->sym_space = sym_space;
            break;
        case SL_split:
            if( subscript ) {
                ProcFlags.substituted = false;
                ProcFlags.unresolved = true;
            } else {
                ProcFlags.substituted = true;
                if( !ProcFlags.if_cond
                  && !ProcFlags.dd_macro
                  && !curr->value[0] ) {
                    ProcFlags.null_value = true;
                }
                /* NB: In some cases there may be no hidden_head after splitting. */
                split_input_var( buf, curr->orig.e, &curr->value[1], input_cbs->fmflags );
                if( input_cbs->hidden_head != NULL ) {
                    input_cbs->hidden_head->fm_symbol = true;   // new logical input record
                }
                cw_lg = 0;
                for( p = buf; *p != ' '; p++ )              // length of . plus CW
                    cw_lg++;
                cw_lg++;                                    // plus space after CW
                if( input_cbs->hidden_head ) {
                    if( ProcFlags.scr_cw && (buf + cw_lg == curr->orig.s) ) {
                        input_cbs->hidden_head->sym_space = false;  // space is space after cw
                    } else {
                        if( curr->orig.s == buf ) {          // symbol at start of input line
                            input_cbs->hidden_head->sym_space = true;
                        } else {
                            input_cbs->hidden_head->sym_space = (*(curr->orig.s - 1) == ' ');
                        }
                    }
                }
                *curr->orig.s = '\0';
            }
            break;
        default:
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
        if( subscript && (curr->type == SL_split) ) {
            break;
        }
    }
    add_sym_list_entry_to_pool( stack );
    return( ProcFlags.substituted );
}


static void expand_subscripts( char *buf, const symvar *var, sub_index lo_bound, sub_index hi_bound )
{
    const symsub    *   ws;
    bool                put_comma = false;

    *buf = '\0';                                // clear output buffer

    for( ws = var->subscripts; ws != NULL; ws = ws->next ) {
        if( ws->subscript > hi_bound )          // past the upper range?
            break;
        if( (ws->subscript >= lo_bound)
          && (ws->subscript <= hi_bound) ) {
            if( put_comma )
                strcat( buf, ", " );
            strcat( buf, ws->value );
            put_comma = true;
        }
    }
}


/***************************************************************************/
/*  scan for symbol start character (&) or, if processing a SCRIPT control */
/*  word record, also for a control word separator.                        */
/***************************************************************************/

static char *scan_sym_or_sep( char *buf, bool splittable )
{
    char        *p;
    char        *pa;

    p = NULL;
    for( pa = buf; *pa != '\0'; pa++ ) {
        if( *pa == ampchar ) {
            p = pa;
            break;
        } else if( *pa == cw_sep_char ) {
            if( splittable
              && (ProcFlags.scr_cw
              || ProcFlags.cw_force_sep)
              && !ProcFlags.cw_sep_ignore ) {
                split_input( buff2, pa + 1, II_none );      // split after cw_sep_char

                buff2_lg = pa - buff2;      // update length after splitting
                *(buff2 + buff2_lg) = '\0'; // terminate first record
                break;
            }
        }
    }
    return( p );
}

/***************************************************************************/
/*  parse the current input buffer from left to right, building a stack of */
/*  sym_list_entry instances                                               */
/*  fully resolve all items                                                */
/*  if splittable is true, then:                                           */
/*    return when a symbol (as opposed to an attribute or a function) has  */
/*    a value which starts with the control word separator (";" by         */
/*    default) or the end of buf is reached                                */
/*  if splittable is false, then:                                          */
/*    return only when the end of buf is reached                           */
/***************************************************************************/

static sym_list_entry *parse_l2r( char *buf, bool splittable )
{
    char            *p;
    char            *pa;
    char            *symstart;              // start of symbol name
    char            valbuf[BUF_SIZE + 1];   // buffer for attribute function and function values
    int             rc;
    sub_index       var_ind;                // subscript value (if symbol is subscripted)
    symsub          *symsubval;             // value of symbol
    symvar          symvar_entry;
    sym_list_entry  *curr    = NULL;        // current top-of-stack
    sym_list_entry  *temp    = NULL;        // used to create new top-of-stack
    char            funcname[FUNC_NAME_LENGTH + 1];

    p = buf;
    while( (p = scan_sym_or_sep( p, splittable )) != NULL ) {         // & found
        temp = alloc_sym_list_entry();
        if( curr == NULL ) {
            curr = temp;            // initialize stack
        } else {
            temp->prev = curr;      // push stack down
            curr = temp;
        }
        curr->orig.s = p;
        if( p[1] == ' ' ) {  // not a symbol substition, attribute, or function
            curr->orig.e = p + 2;
            curr->type = SL_text;               // text
        } else if( my_isalpha( p[1] ) && (p[2] == '\'') ) {   // attribute or text
            if( (p[3] > ' ') ) {
                funcname[0] = p[1];
                funcname[1] = '\0';
                curr->orig.e = curr->orig.s;
                while( !is_space_tab_char( *curr->orig.e ) && (*curr->orig.e != '\0') && (*curr->orig.e != '.') )
                    curr->orig.e++;
                if( GlobalFlags.firstpass && (input_cbs->fmflags & II_research) ) {
                    add_single_func_research( funcname );
                }
                pa = valbuf;
                curr->orig.e = scr_single_funcs( funcname, curr->orig.s + 3, curr->orig.e, &pa );
                if( ProcFlags.unresolved ) {
                    curr->type = SL_text;
                } else {
                    curr->type = SL_attrib;
                    strcpy( curr->value, valbuf );      // save value in current stack entry
                }
            } else {
                curr->type = SL_text;
                curr->orig.e = curr->orig.s + 3;
            }
        } else if( p[1] == '\'' ) {         // function or text
            p = get_func_name( p + 2, funcname );// over "&'" and get function name
            if( *p == '(' ) {                   // &'xyz( is start of multi char function
                curr->orig.e = curr->orig.s;
                while( !is_space_tab_char( *curr->orig.e ) && (*curr->orig.e != '\0') && (*curr->orig.e != '.') )
                    curr->orig.e++;
                pa = valbuf;
                curr->orig.e = scr_multi_funcs( funcname, p, &pa, BUF_SIZE );
                if( ProcFlags.unresolved ) {
                    curr->type = SL_text;
                } else {
                    strcpy( curr->value, valbuf );  // save value in current stack entry
                    curr->type = SL_funct;
                }
            } else {
                curr->orig.e = p;
                curr->type = SL_text;           // text
            }
        } else {                                // symbol
            if( (p[1] == '*') && (p[2] == ampchar) ) {  // special for &*&<var>
                curr->orig.e = p + 2;
                curr->type = SL_text;
                p = p + 2;
            } else {
                p++;                                // over '&'
                symstart = p;                       // remember start of symbol name
                g_scan_err = false;
                ProcFlags.suppress_msg = true;
                pa = valbuf;
                p = scan_sym( symstart, &symvar_entry, &var_ind, &pa, splittable );
                curr->orig.e = p;
                if( g_scan_err ) {                        // problem with subscript
                    if( ProcFlags.unresolved ) {
                        curr->type = SL_text;
                        if( *curr->orig.e == '\0' ) {
                            break;                      // end of text terminates processing
                        }
                        p = curr->orig.e;                // skip argument
                        p = strchr( p, ampchar );       // look for next & in buffer
                    } else {
                        if( !ProcFlags.cw_sep_ignore
                          && splittable
                          && (cw_sep_char != '\0')
                          && (valbuf[0] == cw_sep_char)
                          && (valbuf[1] != cw_sep_char) ) {
                            strcpy( curr->value, valbuf );  // repurpose curr
                            curr->orig.s = p + 1;            // & of symbol causing split
                            curr->type = SL_split;
                            break;              // line split terminates processing
                        } else {
                            var_ind = atol( valbuf );       // save value for use
                            *curr->value = '\0';            // overwrite with nothing
                            curr->type = SL_text;
                        }
                    }
                } else {
                    rc = find_symvar_sym( &symvar_entry, var_ind, &symsubval );
                    // subscript expansion needs special handling
                    if( (var_ind == SI_all_subscript) ||(var_ind == SI_neg_subscript) || (var_ind == SI_pos_subscript) ) {
                        sub_index   lo_bound = SI_min_subscript;
                        sub_index   hi_bound = SI_max_subscript;

                        // adjust default bounds which go from min to max
                        switch( var_ind ) {
                        case SI_neg_subscript:
                            hi_bound = 0;
                            break;
                        case SI_pos_subscript:
                            lo_bound = 0;
                            break;
                        default:
                            break;
                        }
                        curr->type = SL_symbol;
                        expand_subscripts( curr->value, symsubval->base, lo_bound, hi_bound );
                    } else if( rc == 2 ) {          // variable found + resolved
                        if( !ProcFlags.cw_sep_ignore && splittable && cw_sep_char != '\0' &&
                                symsubval->value[0] == cw_sep_char &&
                                symsubval->value[1] != cw_sep_char ) {
                            curr->type = SL_split;
                            strcpy( curr->value, symsubval->value );  // save value in current stack entry
                            SkipDot( curr->orig.e );
                            break;              // line split terminates processing
                        } else if( symsubval->base->flags & SF_is_AMP ) {
                            curr->type = SL_text;   // save for late substitution
                        } else {
                            curr->type = SL_symbol;
                            strcpy( curr->value, symsubval->value );  // save value in current stack entry
                        }
                    } else if( symvar_entry.flags & SF_local_var ) {    // undefined locals are set to ''
                        curr->type = SL_symbol;
                        curr->value[0] = '\0';
                    } else {                        // undefined global
                        curr->type = SL_text;
                        curr->orig.e = symstart;    // rescan for CW separator past the &
                    }
                }
            }
        }
        if( *curr->orig.e == '\0' ) {
            break;                      // end of text terminates processing
        }
        p = curr->orig.e;                // skip argument
    }
    return( curr );
}


/***************************************************************************/
/*  symbolic variables, single-letter attribute functions and multi-letter */
/*  functions are resolved here                                            */
/*  currently, this is done using two helper functions                     */
/***************************************************************************/

bool resolve_symvar_functions( char *buf, bool splittable )
{
    bool                anything_substituted    = false;
    sym_list_entry  *   stack;

    if( buf == NULL ) {
        return( false );                    // no text to process
    }
    stack = parse_l2r( buf, splittable );
    if( stack == NULL ) {
        return( false);                     // no stack to process
    }
    parse_r2l( stack, buf, false );
    anything_substituted |= ProcFlags.substituted;
    while( ProcFlags.substituted ) {
        stack = parse_l2r( buf, splittable );
        if( stack == NULL ) {
            break;                      // no stack to process
        }
        parse_r2l( stack, buf, false );
        anything_substituted |= ProcFlags.substituted;
    }
    return( anything_substituted );
}


/***************************************************************************/
/*  finalize the subscript value                                           */
/*  simplified version of resolve_symvar_functions()                       */
/*  no need to return an indicator if something has been substituted       */
/***************************************************************************/

void finalize_subscript( char **result, bool splittable )
{
    sym_list_entry  *stack;

    stack = parse_l2r( *result, splittable );
    if( stack == NULL ) {
        return;                             // no stack to process
    }
    parse_r2l( stack, *result, true );
    while( ProcFlags.substituted ) {
        stack = parse_l2r( *result, splittable );
        if( stack == NULL ) {
            break;                      // no stack to process
        }
        parse_r2l( stack, *result, true );
    }

    return;
}


/***************************************************************************/
/*  set  some switches if char is script controlword start or              */
/*  gml tag start                                                          */
/***************************************************************************/

void classify_record( const char *p )
{
    if( check_tagname( p, NULL ) != NULL ) {   // classify input
        ProcFlags.gml_tag = true;
        ProcFlags.scr_cw = false;
        ProcFlags.cw_force_sep = false;
    } else {
        ProcFlags.gml_tag = false;
        if( *p == SCR_char ) {
            ProcFlags.scr_cw = true;
        } else {
            ProcFlags.scr_cw = false;
        }
    }
}


/***************************************************************************/
/*  Possibly reclassify record after substitution. There may be a SCRIPT   */
/*  control word, but not a new GML tag.                                   */
/***************************************************************************/

static void reclassify_record( const char *p )
{
    // if record was a GML tag, it will stay a GML tag, unless it's now
    // a SCRIPT control word (if that's even possible). But a record that
    // wasn't already a GML tag will *not* become one after substitution.
    if( *p == SCR_char ) {
        ProcFlags.scr_cw  = true;
        ProcFlags.gml_tag = false;
    } else {
        ProcFlags.scr_cw  = false;
    }
}


/***************************************************************************/
/*  remove leading space                                                   */
/*  to catch lines such as: "    :TAG xyz."  -> ":TAG xyz."                */
/*  returns true if spaces removed                                         */
/***************************************************************************/

static bool remove_leading_space( void )
{
    char    * p;
    char    * p2;
    bool    removed = false;

    if( ProcFlags.literal
      || !ProcFlags.concat ) {  // .li active or .co OFF
        return( false );        // don't change input
    }
    p = buff2;
    SkipSpacesTabs( p );
    if( (p != buff2)
      && (check_tagname( p, NULL ) != NULL )
      && (cur_group_type != GRT_xmp) ) {
        p2 = buff2;
        do {
            *p2++ = *p++;
        } while( *p != '\0' );
        *p2++ = '\0';
        *p2   = '\0';
        removed = true;
    }
    return( removed );
}


/***************************************************************************/
/*  take the contents of the input line in buff2 and try to make the best  */
/*  of it                                                                  */
/*  Processing as specified in wgmlref.pdf chapter 8.1 processing rules    */
/*  and results from testing with wgml 4                                   */
/*                                        imcomplete               TBD     */
/*                                                                         */
/***************************************************************************/

void process_line( void )
{
    bool    anything_substituted;

    anything_substituted = remove_leading_space();// for "   :TAG   " constructs

    classify_record( buff2 );      // classify script CW, GML tag or nothing

    if( !split_input_buffer() ) {
        return;                         // .* .dm :cmt found
    }

    /***************************************************************************/
    /*  pre-identify IF statements                                             */
    /*  this is needed to avoid an empty symbol from having it's usual effect  */
    /*  when a line like                                                       */
    /*    .if &fred eq ''                                                      */
    /*  is used and "fred" has a null value (matching '')                      */
    /*  EL is also checked to handle lines like                                */
    /*    .el .if &fred eq ''                                                  */
    /*  note that symbol substition is done before IF is actually processed    */
    /*  this can probably be improved                                          */
    /***************************************************************************/
    if( ProcFlags.scr_cw
      && (((my_toupper(*(buff2 + 1)) == 'I')
      && (my_toupper(*(buff2 + 2)) == 'F'))
      || ((my_toupper(*(buff2 + 1)) == 'E')
      && (my_toupper(*(buff2 + 2)) == 'L'))) ) {
        ProcFlags.if_cond = true;
    } else {
        ProcFlags.if_cond = false;
    }
    if( ProcFlags.indented_text ) {
        anything_substituted |= resolve_symvar_functions( buff2, false );
        ProcFlags.indented_text = false;
    } else {
        anything_substituted |= resolve_symvar_functions( buff2, true );
    }

    reclassify_record( buff2 );    // reclassify SCRIPT/text after symbol substitution etc

    buff2_lg = strlen( buff2 );

    if( (input_cbs->fmflags & II_research)
      && GlobalFlags.firstpass
      && anything_substituted ) {
        g_info_lm( INF_SUBST_LINE, buff2 ); // show line with substitution(s)
    }

    g_scandata.s = buff2;
    g_scandata.e = buff2 + buff2_lg;
    return;
}

