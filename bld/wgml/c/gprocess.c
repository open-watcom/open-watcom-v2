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
* Description:  WGML input line processing
*   some logic / ideas adopted from Watcom Script 3.2 IBM S/360 Assembler
*   see comment in wgml.c
****************************************************************************/

#include "wgml.h"
#include "gvars.h"

#include "clibext.h"

/*  split_input
 *  The (physical) line is split
 *  The second part will be processed by the next getline()
 *   pushing any already split part down
 *
 */

void        split_input( char * buf, char * split_pos, bool sol )
{
    inp_line    *   wk;
    size_t          len;

    buf = buf;
    len = strlen( split_pos );          // length of second part
    if( len > 0 ) {
        wk = mem_alloc( len + sizeof( inp_line ) );
        wk->next = input_cbs->hidden_head;
        wk->sol  = sol;
        strcpy(wk->value, split_pos );  // save second part

        input_cbs->hidden_head = wk;
        if( input_cbs->hidden_tail == NULL ) {
            input_cbs->hidden_tail = wk;
        }

        *split_pos = '\0';              // terminate first part
        input_cbs->fmflags &= ~II_eol;  // not last part of line
    }
    return;
}


/*  split_input_var
 *  The second part is constructed from 2 parts
 *  used if a substituted variable starts with CW_sep_char
 */

static  void    split_input_var( char * buf, char * split_pos, char * part2, bool sol )
{
    inp_line    *   wk;
    size_t          len;

    buf = buf;
    len = strlen( split_pos ) + strlen( part2 );// length of second part
    if( len > 0 ) {
        wk = mem_alloc( len + sizeof( inp_line ) );
        wk->next = input_cbs->hidden_head;
        wk->sol  = sol;

        strcpy(wk->value, part2 );      // second part
        strcat(wk->value, split_pos );  // second part

        input_cbs->hidden_head = wk;
        if( input_cbs->hidden_tail == NULL ) {
            input_cbs->hidden_tail = wk;
        }
        input_cbs->fmflags &= ~II_eol;  // not last part of line
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
    char    *   p;
    char    *   p2;
    char    *   pchar;
    char        c;
    bool        layoutsw;
    size_t      toklen;

    if( *buff2 == GML_char ) {
        if( !strnicmp( (buff2 + 1), "CMT", 3 ) &&
            ((*(buff2 + 4) == '.') || (*(buff2 + 4) == ' ')) ) {
            return;                     // no split for :cmt. line
        }
    }

    /***********************************************************************/
    /*  Look for GML tag start char(s) until a known tag is found          */
    /*  then split the line                                                */
    /***********************************************************************/
    pchar = strchr( buff2 + 1, GML_char );
    while( pchar != NULL ) {
        while( *(pchar + 1) == GML_char ) {
            pchar++;                    // handle repeated GML_chars
        }
        for( p2 = pchar + 1;
             is_id_char( *p2 ) && (p2 < (buff2 + buf_size));
             p2++ ) /* empty loop */ ;

        if( (p2 > pchar + 1)
            && ((*p2 == '.') ||
                is_space_tab_char( *p2 ) ||
                (*p2 == '\0') ||
                (*p2 == GML_char) ) ) { // 'good' tag end

            c = *p2;
            if( ProcFlags.layout && (c == '\t') ) {
                c = ' ';                // replace tab with space in layout
            }
            *p2 = '\0';                 // null terminate string
            toklen = p2 - pchar - 1;

            /***************************************************************/
            /* Verify valid user or system tag                             */
            /***************************************************************/
            if( (find_tag( &tag_dict, pchar + 1 ) != NULL) ||
                (find_sys_tag( pchar + 1, toklen ) != NULL) ||
                (find_lay_tag( pchar + 1, toklen ) != NULL) ) {

                *p2 = c;

                if( input_cbs->fmflags & II_sol ) {
                // remove spaces before tags at sol in restricted sections
                // in or just before LAYOUT tag
                    layoutsw = ProcFlags.layout;
                    if( !layoutsw && (strncmp( "LAYOUT", pchar + 1, 6 ) == 0 ) ) {
                        layoutsw = true;
                    }
                    if( (rs_loc > 0) || layoutsw ) {
                        p = buff2;
                        while( is_space_tab_char( *p ) ) {
                            p++;
                        }
                        if( p == pchar ) {  // only leading blanks
                            memmove( buff2, pchar, buf_size - (p - buff2) );
                            buff2_lg = strlen( buff2 );// new length
                            pchar = strchr( buff2 + 1, GML_char );  // try next GMLchar
                            continue;       // dummy split done try again
                        }
                    }
                }
                split_input( buff2, pchar, false );// split line
                if( ProcFlags.literal ) {   // if literal active
                    if( li_cnt < LONG_MAX ) {// we decrement, adjust for split line
                        li_cnt++;
                    }
                }
                break;                  // we did a split stop now
            } else {
                *p2 = c;
            }
        }
        pchar = strchr( pchar + 1, GML_char );  // try next GMLchar
    }
}


/***************************************************************************/
/*  look for control word separator character and split line if found      */
/*  and other conditions don't prevent it                                  */
/*  if splitpos is not NULL, the CWsep is already known                    */
/***************************************************************************/

static void split_at_CW_sep_char( char * splitpos ) {
    /***********************************************************/
    /*  if 2 CW_sep_chars follow, don't split line             */
    /*  ... this is NOWHERE documented, but wgml 4.0 tested    */
    /*  also don't split if last char of record                */
    /***********************************************************/

    if( buff2[1] != '\'' ) {
        if( splitpos == NULL ) {        // splitpos not yet known
            splitpos = strchr( buff2 + 2, CW_sep_char );
        }
        if( (splitpos != NULL) && (splitpos[1] != '\0') ) {

            if( (splitpos[-1] == '\'') && (splitpos[1] == '\'') ) {
                ;
            // hack for testing macro repchars docs\doc\hlp\fmtmacro.gml(174)
                                        // don't split &'index("&x.",';') TBD
            } else {

                if( splitpos[1] != CW_sep_char ) {
                    split_input( buff2, splitpos + 1, false );  // split after CW_sep_char
                    splitpos[0] = '\0'; // terminate 1. part
#if 0
                } else {                // ignore 1 CW_sep_char
                    memmove( splitpos, splitpos + 1, splitpos - buff2 + buff2_lg );
                    buff2_lg = strlen( buff2 );
#endif
                }
            }
        }
    }
}


/***************************************************************************/
/*   Split input line at GML tag or script Control word separator          */
/*   returns false if :cmt .cm  .dm found                                  */
/***************************************************************************/

static  bool    split_input_buffer( void )
{
    char            *   p2;
    char            *   pchar;

    /***********************************************************************/
    /*  look for GML tag start character and split line at GML tag         */
    /*  special for script control line: possibly split at CW_sep_char     */
    /***********************************************************************/
    if( !ProcFlags.literal && (*buff2 == SCR_char) ) {
        pchar = strchr( buff2 + 1, CW_sep_char );
        p2 = strchr( buff2 + 1, GML_char );
        if( pchar && (p2 > pchar) ) {// GML_char follows CW_sepchar in buffer

            for( p2 = buff2 + 1; *p2 == SCR_char; p2++ ) {
                ; /* empty */
            }
            if( !(  (*p2 == '\'') ||    // suppress CW_separator
                    (*p2 == '*')  ||    // comment
                    (CW_sep_char == '\0') ||// no CW sep char
                    !strnicmp( p2, "cm ", 3) // comment
                 ) ) {

                split_at_CW_sep_char( pchar );  // now split record
            }
        }
    }
    split_at_GML_tag();

    if( !ProcFlags.literal ) {

        /*******************************************************************/
        /* for :cmt. minimal processing                                    */
        /*******************************************************************/

        p2 = buff2;
        if( (*p2 == GML_char) && !strnicmp( p2 + 1, "cmt.", 4 ) ) {
            return( false );
        }

        /*******************************************************************/
        /*  .xx SCRIPT control line                                        */
        /*******************************************************************/

        if( (*p2 == SCR_char) ) {
            ++p2;
            if( (*p2 == '*') || !strnicmp( p2, "cm ", 3 ) ) {
                return( false );  // for .* or .cm comment minimal processing
            }

            /***************************************************************/
            /* if macro define (.dm xxx ... ) supress variable substitution*/
            /* for the sake of single line macro definition                */
            /* .dm xxx / &*1 / &*2 / &*0 / &* /                            */
            /*  and                                                        */
            /* ..dm xxx / &*1 / &*2 / &*0 / &* /                           */
            /***************************************************************/
            if( *p2 == SCR_char ) {
                ++p2;
            }
            if( !strnicmp( p2, "dm ", 3 ) ) {
                return( false );
            }

            /***************************************************************/
            /*  for records starting  .' ignore control word separator     */
            /*  or if control word separator is 0x00                       */
            /***************************************************************/
            if( (*p2 == '\'') || (CW_sep_char == '\0') ) {
                ProcFlags.CW_sep_ignore = true;
            } else {
                ProcFlags.CW_sep_ignore = false;

                split_at_CW_sep_char( NULL );
            }
        }
    }
    return( true );                     // further processing needed
}


/***************************************************************************/
/*  symbolic variables, single and multi letter functions are resolved     */
/*  here                                                                   */
/*  this function may be called recursively via scr_multi_funcs()          */
/*  and scr_multi_funcs() may be called recursively from here              */
/*  until the buffer is processed                                          */
/***************************************************************************/

bool    resolve_symvar_functions( char * buf )
{
    static const char   ampchar = '&';
    inp_line        *   in_wk;
    char            *   workb;
    char            *   pw;
    char            *   pwend;
    char            *   p2;
    char            *   pchar;
    char            *   varstart;
    sub_index           var_ind;
    symvar              symvar_entry;
    symsub          *   symsubval;
    size_t              buf_lg;
    int                 rc;
    bool                functions_found;
    bool                anything_substituted;

    char            *   var_unresolved; // ptr for resume search
    char            *   var_unresolved2;// ptr for resume search

    ProcFlags.substituted = false;
    ProcFlags.unresolved  = false;

    if( NULL == strchr( buf, ampchar ) ) {  // look for & in buffer
        return( false );              // no ampchar found, nothing to resolve
    }

    anything_substituted = false;
    var_unresolved = NULL;
    var_unresolved2 = NULL;
    functions_found = false;

    in_wk = mem_alloc( sizeof( inp_line ) + buf_size );// allocate workbuffer
    in_wk->next = NULL;
    workb = in_wk->value;               // allocate workbuffer

    do {                                // until no more substitutions
        strcpy( workb, buf );   // copy input buffer
        buf_lg = strlen( buf );
        pwend = workb + buf_lg - 1;
        if( var_unresolved == NULL ) {
            pw = workb;
            p2 = buf;
        } else {
            pw = workb + (var_unresolved2 - buf);
            p2 = var_unresolved2;
        }
        varstart = NULL;

        anything_substituted |= ProcFlags.substituted;
        ProcFlags.substituted = false;

        pchar = strchr( workb, ampchar ); // look for & in buffer
        while( pchar != NULL ) {        // & found
            if( *(pchar + 1) == ' ' ) { // not a symbol substition or function
                pchar = strchr( pchar + 1, ampchar );  // look for next & in buffer
                continue;
            }
            while( pw < pchar ) {       // copy all data preceding &
                *p2++ = *pw++;
            }
            buf_lg = strlen( buf );

            if( isalpha( *(pchar + 1) ) && *(pchar + 2) == '\''
                && *(pchar + 3) > ' ' ) {
                                        // not for .if '&*' eq '' .th ...
                                        // only    .if '&x'foo' eq '' .th
                char * * ppval = &p2;

                /***********************************************************/
                /*  Some single letter functions are resolved here:        */
                /*                                                         */
                /*  functions used within the OW doc build system:         */
                /*   &e'  existance of variable 0 or 1                     */
                /*   &l'  length of variable content                       */
                /*        or if undefined variable length of name          */
                /*   &u'  upper                                            */
                /*                                                         */
                /*   &s'  subscript                                        */
                /*   &S'  superscript                                      */
                /*                                                         */
                /*   other single letter functions are not used AFAIK      */
                /*                                                         */
                /***********************************************************/

                if( GlobalFlags.firstpass && input_cbs->fmflags & II_research ) {
                    add_single_func_research( pchar + 1 );
                }

                pw = scr_single_funcs( pchar, pwend, ppval );

                pchar = strchr( pw, ampchar );  // look for next & in buffer

                continue;
            }

            if( *(pchar + 1) == '\'' ) {// perhaps a multi letter function
                char   * pf;
                char * * ppval = &p2;
                int32_t  valsize = buf_size - (p2 - buf);

                pf = pchar + 2;
                while( is_function_char( *pf ) ) {
                    pf++;
                }
                if( *pf == '(' ) {// &'xyz( is start of multi char function

                /***********************************************************/
                /*  Some multi  letter functions are resolved here:        */
                /*                                                         */
                /*  functions used within the OW doc build system:         */
                /*                                                         */
                /*   &'delstr(          &'d2c(              &'index(       */
                /*   &'insert(          &'left(             &'length(      */
                /*   &'lower(           &'min(              &'pos(         */
                /*   &'right(           &'strip(            &'substr(      */
                /*   &'subword(         &'translate(        &'upper(       */
                /*   &'veclastpos(      &'vecpos(           &'word(        */
                /*   &'wordpos(         &'words(                           */
                /*                                                         */
                /*   Others are recognized but not processed               */
                /*   &'c2x(    is used for test output                     */
                /***********************************************************/

                    pw = scr_multi_funcs( pchar, pwend, ppval, valsize );

                    pchar = strchr( pw, ampchar );// look for next & in buffer
                    continue;
                }

                *p2++ = *pw++;          // copy &
                pchar = strchr( pw, ampchar );  // look for next & in buffer
                continue;               // and ignore this &... for now
            }

            /***************************************************************/
            /* & is neither start of single char function                  */
            /*   nor start of multi char function                          */
            /*                                                             */
            /* &  is probably start of a variable                          */
            /***************************************************************/

            varstart = pw;              // remember start of var
            pw++;                       // over &
            ProcFlags.suppress_msg = true;
            scan_err = false;

            pchar = scan_sym( pw, &symvar_entry, &var_ind );
            if( scan_err && *pchar == '(' ) {   // problem with subscript

                if( var_unresolved == NULL ) {
                    ProcFlags.unresolved  = true;
                    var_unresolved = varstart;
                    var_unresolved2 = p2;
                } else {
                    if( var_unresolved != varstart ) {
                        ProcFlags.unresolved  = true;
                    }
                }
                p2 += pchar - varstart;
                pw = pchar;
                pchar = strchr( pw, ampchar );  // look for next & in buffer

                continue;
            }

            ProcFlags.suppress_msg = false;

            if( symvar_entry.flags & local_var ) {  // lookup var in dict
                rc = find_symvar_l( &input_cbs->local_dict, symvar_entry.name,
                                    var_ind, &symsubval );
            } else {
                rc = find_symvar( &global_dict, symvar_entry.name, var_ind,
                                  &symsubval );
            }
            if( rc == 2 ) {             // variable found + resolved
                ProcFlags.substituted = true;
                if( !ProcFlags.CW_sep_ignore &&
                    symsubval->value[0] == CW_sep_char &&
                    symsubval->value[1] != CW_sep_char ) {

                                // split record at control word separator
                                // if variable starts with SINGLE cw separator
                                // and ignore cw separator

                    if( buf != buff2 ) {

                        // splitting input if not outermost buffer ++++ TBD
                        // needed ???
                        g_suicide();
                    }

                    if( *pchar == '.' ) {
                        pchar++;        // skip optional terminating dot
                    }
                    *p2 = '\0';
                    split_input_var( buf, pchar, &symsubval->value[1], true );
                    pw = pwend + 1;     // stop substitution for this record
                    varstart = NULL;

                    break;

                } else {
                    pw = symsubval->value;
                    if( symsubval->value[0] == CW_sep_char &&
                        symsubval->value[1] == CW_sep_char ) {
                        pw++;           // skip 1 CW_sep_char
                    }
                    strcpy( p2, pw );   // copy value
                    p2 += strlen(pw);
                    if( *pchar == '.' ) {
                        pchar++;        // skip optional terminating dot
                    }
                    pw = pchar;
                }
            } else {                    // variable not found
                if( (symvar_entry.flags & local_var )  // local var not found
                    ) {
                    if( (symvar_entry.name[0] == '\0') &&
                        (*pchar == ampchar) ) { // only &* as var name
                                                // followed by another var

                        if( var_unresolved == NULL ) {
                            ProcFlags.unresolved  = true;
                            var_unresolved = varstart;
                            var_unresolved2 = p2;
                        } else {
                            if( var_unresolved != varstart ) {
                                ProcFlags.unresolved  = true;
                            }
                        }
                        pw = varstart;
                        while( pw < pchar ) {   // treat var name as text
                            *p2++ = *pw++;  // and copy
                        }

                        continue;       // pchar points already to next &

                    } else {     // replace not found local var by nullstring
                        ProcFlags.substituted = true;
                        if( *pchar == '.' ) {
                            pchar++;    // skip optional terminating dot
                        }
                        pw = pchar;
                    }
                } else {                // global var not found
                                        // .. or local var outside of macro
                    /*******************************************************/
                    /*  keep trying for constructs such as                 */
                    /*                                                     */
                    /* .se prodgml = "Open Watcom GML"                     */
                    /* .se name = "GML"                                    */
                    /*                                                     */
                    /* My name is &prod&name..!                            */
                    /*                                                     */
                    /*  to become                                          */
                    /*                                                     */
                    /* My name is Open Watcom GML!                         */
                    /*                                                     */
                    /* This does not work for local variables, as these are*/
                    /* replaced by nullstring if not found                 */
                    /* My name is &*prod&*name..!                          */
                    /*  will become                                        */
                    /* My name is !                                        */
                    /*******************************************************/

                    if( var_unresolved == NULL ) {
                        ProcFlags.unresolved  = true;
                        var_unresolved = varstart;
                        var_unresolved2 = p2;
                    } else {
                        if( var_unresolved != varstart ) {
                            ProcFlags.unresolved  = true;
                        }
                    }

                    pw = varstart;
                    if( *pchar == '.' ) {
                        pchar++;        // copy terminating dot, too
                    }
                    while( pw < pchar ) {   // treat var name as text
                        *p2++ = *pw++;  // and copy
                    }
                }
            }
            pchar = strchr( pw, ampchar );  // look for next & in buffer
        }                               // while & found

        while( pw <= pwend) {           // copy remaining input
             *p2++ = *pw++;
        }
        *p2 = 0;                        // terminate string

    } while( ProcFlags.unresolved && ProcFlags.substituted );

    anything_substituted |= ProcFlags.substituted;

    mem_free( in_wk );                  // free workbuffer
    return( anything_substituted );
}


/***************************************************************************/
/*  set  some switches if char is script controlword start or              */
/*  gml tag start                                                          */
/***************************************************************************/

void    classify_record( char c )
{
    if( c == GML_char ) {               // classify input
        ProcFlags.gml_tag = true;
        ProcFlags.scr_cw = false;
    } else {
        ProcFlags.gml_tag = false;
        if( c == SCR_char ) {
           ProcFlags.scr_cw = true;
        } else {
           ProcFlags.scr_cw = false;
        }
    }
}


/***************************************************************************/
/*  remove leading space                                                   */
/*  to catch lines such as: "    :TAG xyz."  -> ":TAG xyz."                */
/*  returns true if spaces removed                                         */
/***************************************************************************/

static  bool    remove_leading_space( void )
{
    char    * p;
    char    * p2;
    bool    removed = false;

    if( ProcFlags.literal ) {           // .li active
        return( false );                // don't change input
    }
    p = buff2;
    while( is_space_tab_char( *p ) ) {
        p++;
    }
    if( (p != buff2) && (*p == GML_char) ) {
        p2 = buff2;
        do {
            *p2++ = *p++;
        } while( *p );
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

void        process_line( void )
{
    bool    anything_substituted;

    anything_substituted = remove_leading_space();// for "   :TAG   " constructs

    classify_record( *buff2 );      // classify script CW, GML tag or nothing

    if( !split_input_buffer() ) {
        // .* .cm .dm :cmt found
    } else {
        anything_substituted |= resolve_symvar_functions( buff2 );
    
        if( input_cbs->fmflags & II_research && GlobalFlags.firstpass &&
            anything_substituted ) {
            g_info_lm( inf_subst_line, buff2 ); // show line with substitution(s)
        }
    }

    buff2_lg = strlen( buff2 );
    scan_start = buff2;
    scan_stop  = buff2 + buff2_lg;
    return;
}

