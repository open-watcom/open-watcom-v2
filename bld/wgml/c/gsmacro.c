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
* Description:  macro utility functions
*
*             add_macro_cb_entry  -- add macro as input source
*             add_macro_parms     -- add macro call/file include parms to local dictionary
*             free_lines          -- free macro source lines
*             verify_sym          -- identify name=value pairs
*
*             scr_dm              -- .dm control word define macro
*             scr_me              -- .me control word macro end
*             scr_em              -- .em control word execute macro
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/*****************************************************************************/
/* verify that p points to a <symbol>=<value> pair                           */
/* spaces are allowed before or after the "="                                */
/* Note: called from add_macro_parms() for tokens that are not delimited     */
/*****************************************************************************/

static char *verify_sym( const char *p, const char *e )
{
    bool        local;
    bool        star_found;

    local = false;
    g_scan_err = true;
    star_found = false;

    if( *p == '*' ) {    // starts with "*"
        star_found = true;
        if( p + 1 < e
          && is_symbol_char(*(p + 1)) ) {            // local var
            local = true;
        }
    }
    while( (*p != '=') && p < e ) {
        p++;
    }

    if( *p == '=' ) {                   // "=" found
        if( !star_found
          || local ) {    // exclude constructs like "*=.()", which are text
            p++;
            if( p < e ) {               // something follows "="
                g_scan_err = false;
                return( (char *)p );
            }
        }
    }
    return( NULL );
}


/***************************************************************************/
/*  add info about macro   to LIFO input list                              */
/*  if second parm is not null, macro is called via GML tag processing     */
/***************************************************************************/

void    add_macro_cb_entry( mac_entry *me, gtentry *ge )
{
    macrocb *   new;
    inputcb *   nip;

    new = mem_alloc( sizeof( macrocb ) );

    nip = mem_alloc( sizeof( inputcb ) );
    nip->hidden_head = NULL;
    nip->hidden_tail = NULL;
    nip->if_cb       = mem_alloc( sizeof( ifcb ) );
    memset( nip->if_cb, '\0', sizeof( ifcb ) );
    nip->pe_cb.count = -1;
    nip->pe_cb.line  = NULL;

    init_dict( &nip->local_dict );

    nip->s.m        = new;
    nip->fm_hh      = input_cbs->fm_hh;
    nip->fm_symbol  = input_cbs->fm_symbol;
    nip->hh_tag     = input_cbs->hh_tag;
    nip->sym_space  = input_cbs->sym_space;

    new->lineno     = 0;
    new->macline    = me->macline;
    new->mac        = me;
    new->tag        = ge;
    new->ix_seen    = false;

    if( ge == NULL ) {
        new->flags      = FF_macro;
        nip->fmflags    = II_macro;
    } else {
        new->flags      = FF_tag;
        nip->fmflags    = II_tag;
    }
    nip->fmflags |= input_cbs->fmflags & II_research;   // copy research mode

    nip->prev = input_cbs;
    input_cbs = nip;
    return;
}


static char *is_quoted_string( const char *p )
{
    char        quote;

    /* Delimiter must be at the beginning and end of the string. */
    if( is_quote_char( *p ) ) {
        quote = *p++;           // skip over delimiter
        while( *p != '\0' ) {
            if( (p[0] == quote)
              && ((p[1] == ' ')
              || (p[1] == '\0')) ) {
                break;          // matching delimiter found
            }
            p++;
        }
        if( *p == quote ) {    // matching delimiter found
            return( (char *)p );
        }
    }
    return( NULL );
}


/***************************************************************************/
/* add macro parameters from input line as local automatic symbols         */
/* also used with IN/AP to do the same for tokens following the file name  */
/* for non delimited parameters try to assign local set symbols            */
/* i.e. .mac     a b c *xyz="1.8" d "1 + 2"                                */
/*          01234 -- only 1 space after mac is ignored                     */
/*    will give  &*=    a b c *xyz="1.8" d "1 + 2"                         */
/*              &*0=5                                                      */
/*              &*1=a                                                      */
/*              &*2=b                                                      */
/*              &*3=c                                                      */
/*              &*4=d                                                      */
/*              &*5=1 + 2                                                  */
/*        and &*xyz=1.8                                                    */
/*  the variable for &* is named _  This can be changed if this leads to   */
/*  conflicts  -> change define MAC_STAR_NAME in gtype.h                   */
/*  note: .mac    "a b c" produces                                         */
/*               &*=   "a b c"                                             */
/*              &*1="a b c"                                                */
/*  that is, it shares characteristics of both spaces and delimiters       */
/*  some notes on delimiters are needed, at least as applied to local      */
/*  automatic symbols; local set symbols are documented to use the same    */
/*  rules as symbols defined with control word SE (TBD)                    */
/*  .mac "a b c" produces:                                                 */
/*               &*="a b c"                                                */
/*              &*0=1                                                      */
/*              &*1=a b c                                                  */
/*  as might be expected but what may not be as expected is that           */
/*  .mac ""a b c"" produces:                                               */
/*               &*=""a b c""                                              */
/*              &*0=1                                                      */
/*              &*1="a b c"                                                */
/*  and, indeed, the final delimiter is only recognized if it is followed  */
/*  by a space or the terminating '\0'; otherwise, it is included:         */
/*  .mac ""a b"c"" produces:                                               */
/*               &*=""a b"c""                                              */
/*              &*0=1                                                      */
/*              &*1="a b"c"                                                */
/*  while, if a space precedes "c", then "c" becomes the value of *2       */
/*  Trailing spaces are removed, unless                                    */
/*    the entire operand consists of space characters                      */
/*    the line ends in a symbol that evaluates to an empty string          */
/*      Note: * will have no value and *1 will not exist                   */
/*    the space occurred before a separator character that split the line  */
/*  Note: the parsing rules are a bit different from those used in         */
/*        getarg(), so that function is not used                           */
/***************************************************************************/

void    add_macro_parms( char * p )
{
    char        *pa;
    char        starbuf[NUM2STR_LENGTH + 1];
    condcode    cc;
    int         star0;
    unsigned    len;
    unsigned    o_len;

    pa = p;                             // save start position
    SkipSpaces( p );                    // find first nonspace character, if any
    if( *p == '\0' ) {                  // only spaces follow the macro

        /* local variable * must be added even though it has no value */

        add_symvar( input_cbs->local_dict, MAC_STAR_NAME, "", 0, SI_no_subscript, SF_local_var );
    } else {                            // process text following the macro

        /* remove trailing spaces if appropriate */

        *starbuf = '\0';
        p = pa;                         // restore start position
        o_len = strlen( p );
        len = o_len;
        if ( ProcFlags.pre_fsp ) {      // space characters from symbol/attribute/function evaluation
            if( ProcFlags.concat
              && (len > 2)
              && (*(p + len - 1) == ' ') ) {
                while( *(p + len - 2) == ' ' ) {        // remove trailing spaces
                    len--;
                    if( len == 0 ) {                    // empty operand
                        break;
                    }
                }
                *(p + len) = '\0';                      // end after last non-space character
            }
        } else {                        // explicit space characters
            if( !ProcFlags.null_value
              && (input_cbs->prev->hidden_head == NULL)
              && (len != 0) ) {
                while( *(p + len - 1) == ' ' ) {        // remove trailing spaces
                    len--;
                    if( len == 0 ) {                    // empty operand
                        break;
                    }
                }
                *(p + len) = '\0';                      // end after last non-space character
            }
        }

        /* the name used for * is a macro because it may have to be changed -- TBD */
        add_symvar( input_cbs->local_dict, MAC_STAR_NAME, p, strlen( p ), SI_no_subscript, SF_local_var );
        star0 = 0;
        g_tok_start = p;                // save start of parameter
        SkipSpaces( p );                // find first nonspace character
        while( *p != '\0' ) {           // as long as there are parms
            pa = is_quoted_string( p );
            if( pa != NULL ) {          // argument is quoted
                p++;                    // skip start quote character
                star0++;
                sprintf( starbuf, "%d", star0 );
                add_symvar( input_cbs->local_dict, starbuf, p, pa - p, SI_no_subscript, SF_local_var );
                p = pa + 1;             // skip after end quote character
            } else {                    // look if it is a symbolic variable definition
                char    *ps;

                pa = p;
                pa++;
                SkipNonSpaces( pa );    // get end of string
                g_scandata.s = p;         // rescan for variable
                ProcFlags.suppress_msg = true;  // no errmsg please
                ProcFlags.blanks_allowed = 0;   // no blanks please
                ps = verify_sym( g_scandata.s, pa );
                if( ps != NULL ) {              // have name=value pair?
                    char *p1 = g_scandata.e;
                    ps = is_quoted_string( ps );// have name='string'?
                    if( ps != NULL ) {
                        g_scandata.e = ps + 1;    // point past delimiter
                    } else {
                        g_scandata.e = pa;// terminate as before
                    }
                    scr_se();           // try to set variable and value
                    g_scandata.e = p1;
                }

                ProcFlags.suppress_msg = false; // reenable err msg
                ProcFlags.blanks_allowed = 1;   // blanks again
                if( g_scan_err ) {      // not variable=value format
                    g_scandata.s = p;     // restore scan addresses
                    cc = CC_omit;
                    star0++;
                    sprintf( starbuf, "%d", star0 );
                    add_symvar( input_cbs->local_dict, starbuf, p, pa - p, SI_no_subscript, SF_local_var );
                }
                p = pa;
            }
            SkipSpaces( p );            // over spaces
        }
                                        // the positional parameter count
        add_symvar( input_cbs->local_dict, "0", starbuf, strlen( starbuf ), SI_no_subscript, SF_local_var );
    }

    if( (input_cbs->fmflags & II_research)
      && GlobalFlags.firstpass ) {
        print_sym_dict( input_cbs->local_dict );
    }
}


/*
 * free storage for macro lines
 *              or split input lines
 */

void    free_lines( inp_line * line )
{
    inp_line    *wk;
    inp_line    *wk1;

    wk = line;
    while( wk != NULL ) {
         wk1 = wk->next;
         mem_free( wk );
         wk = wk1;
    }
    return;
}

/***************************************************************************/
/* DEFINE  MACRO defines  a  sequence of  input lines  to  be invoked  by  */
/* ".tok.s" as  a user-defined control word  or as an Execute  Macro (.EM)  */
/* operand.                                                                */
/*                                                                         */
/*      +-------+--------------------------------------------------+       */
/*      |       |                                                  |       */
/*      |       |    name /line1/.../linen</>                      |       */
/*      |  .DM  |    name <BEGIN|END>                              |       */
/*      |       |    name DELETE                                   |       */
/*      |       |                                                  |       */
/*      +-------+--------------------------------------------------+       */
/*                                                                         */
/* Such user macros may be used for common sequences of control words and  */
/* text.   Keyword and positional parameters (&*, &*0, &*1, etc.)  may be  */
/* checked and substituted when the macro is invoked.                      */
/*                                                                         */
/* name:  The user  macro is known by  "name",  a one to  eight character  */
/*    identifier.                                                          */
/* name /line 1/line 2/.../line n/:  The  "macro body" of "name" consists  */
/*    of the input lines that are  separated by a self-defining character  */
/*    shown in the command prototype as "/".                               */
/* name <BEGIN|END>:  Longer user macros  are defined with a "name BEGIN"  */
/*    at the start and "name END" to terminate.   The ".DM name END" must  */
/*    start in column one of the input line.                               */
/* name DELETE:  A user macro may  be deleted by specifying "name DELETE"  */
/*    as an operand; "name OFF" is an alternate way to delete a macro.     */
/*                                                                         */
/* This control word does not cause a break.                               */
/*                                                                         */
/* NOTES                                                                   */
/* (1) The invoking of  defined user macros by ".tok.s"  can be suppressed  */
/*     with the ".MS"  (Macro Substitution)  control word.    Invoking by  */
/*     ".EM .tok.s" cannot be suppressed.                                   */
/* (2) The ".DM name END" operands  are verified for a  macro "name" that  */
/*     matches the ".DM name BEGIN".   ".DM" starting  in column one with  */
/*     no operands will also successfully terminate a macro definition.    */
/* (3) The user-defined  macro may  be invoked  with a  variable list  of  */
/*     keyword and positional operands                                     */
/*       .tok.s operand1 operand2                                           */
/*     that will assign to the local Set Symbols &*1, &*2, ..., the value  */
/*     of corresponding operands in the macro call.   Each operand may be  */
/*     a character string,  a delimited  character string,  or a numeric.  */
/*     Numeric operands  that do not consist  entirely of digits  will be  */
/*     treated as a character string.                                      */
/*       .tok.s key1=value key2=value                                       */
/*     Operands  that consist  of  a valid  Set  Symbol name  immediately  */
/*     followed by an  equal sign will assign  the value on the  right of  */
/*     the equal  sign to  the specified Set  Symbol before  invoking the  */
/*     macro.   If the Set Symbol begins with an asterisk the symbol will  */
/*     be local to the invoked macro.                                      */
/***************************************************************************/

void    scr_dm( void )
{
    char            cw[9];
    char        *   nmstart;
    char        *   p;
    char        *   pa;
    int             len;
    int             macro_line_count;
    bool            compbegin;
    bool            compend;
    inp_line    *   head;
    inp_line    *   last;
    inp_line    *   work;
    line_number     lineno_start;
    condcode        cc;
    inputcb     *   cb;
    char            linestr[NUM2STR_LENGTH + 1];
    char            macname1[MAC_NAME_LENGTH + 1];
    char            macname2[MAC_NAME_LENGTH + 1];

    cb = input_cbs;

    cc = getarg();

    if( cc == CC_omit ) {
        xx_source_err_exit( ERR_MISSING_NAME );
        /* never return */
    }

    /*  truncate name if too long WITHOUT error msg
     *  this is wgml 4.0 behaviour
     *
     */
    get_macro_name( g_tok_start, macname1 );

    cc = getarg();
    if( cc == CC_omit ) {                  // nothing found
        // SC--048 A control word parameter is missing
        xx_source_err_exit_c( ERR_MAC_DEF_FUN, macname1 );
        /* never return */
    }

    get_macro_name( g_tok_start, macname2 );

    head = NULL;
    last = NULL;
    macro_line_count = 0;
    lineno_start = 0;

    compend   = ( strcmp( "end", macname2 ) == 0 );
    compbegin = ( strcmp( "begin", macname2 ) == 0 );
    if( !compbegin
      && !compend ) { // only .dm macname /line1/line2/ possible
        char    sepchar;

        if( cc == CC_quotes ) {
            g_tok_start--;    // for single line .dm /yy/xxy/.. back to sepchar
        }
        if( ProcFlags.in_macro_define ) {
            xx_source_err_exit_c( ERR_MAC_DEF_NEST, g_tok_start );
            /* never return */
        }
        ProcFlags.in_macro_define = 1;

        lineno_start = cb->s.f->lineno;

        p = g_tok_start;
        sepchar = *p++;
        nmstart = p;
        while( *p != '\0' ) {
            while( *p != '\0' && *p != sepchar ) {  // look for separator
                ++p;
            }
            len = p - nmstart;
            *p = '\0';
            work = mem_alloc( sizeof( inp_line ) + len );
            work->next = NULL;
            strcpy( work->value, nmstart );
            if( last != NULL ) {
                last->next = work;
            }
            last = work;
            if( head == NULL ) {
                head = work;
            }
            nmstart = ++p;
            macro_line_count++;
        }
        compend = true;                    // so the end processing will happen
    }                                   // BEGIN or END not found

    if( compend
      && !(ProcFlags.in_macro_define) ) {
        // SC--003: A macro is not being defined
        xx_source_err_exit_c( ERR_MAC_DEF_END, macname1 );
        /* never return */
    }
    if( compbegin
      && (ProcFlags.in_macro_define) ) {
        // SC--002 The control word parameter '%s' is invalid
        xx_source_err_exit_c( ERR_MAC_DEF_NEST, macname1 );
        /* never return */
    }

    if( compbegin ) {                   // start new macro define
        ProcFlags.in_macro_define = 1;
        lineno_start = cb->s.f->lineno;

        while( (cb->s.f->flags & FF_eof) == 0 ) {  // process all macro lines

            get_line( true );

            if( cb->s.f->flags & (FF_eof | FF_err) ) {
                break;                  // out of read loop
            }

            len = 0;
            p = buff2;
            pa = cw;
            if( *p == SCR_char ) {              // only test script control words
                p++;

                /****************************************************************/
                /* although ".." is used with DM in the OW Docs, '.'" is not    */
                /* constructs like "..'" ".'." have yet to be explored          */
                /****************************************************************/

                if( (*p == SCR_char)
                  || (*p == '\'') ) {
                    p++;                        // over ".." or ".'"
                }
                p = get_macro_name( p, macname2 );
                if( macname2[0] == 'd'
                  && macname2[1] == 'm' ) {
                    if( (macname2[2] == '\0')
                      || (find_macro( macro_dict, macname2 ) == NULL) ) { // .dm control word
                        cc = getarg();
                        if( cc == CC_omit ) {  // only .dm  means macro end
                            compend = true;
                            break;          // out of read loop
                        }
                        get_macro_name( g_tok_start, macname2 );
                        if( strcmp( macname1, macname2 ) != 0 ) {
                            // macroname from begin different from end
                            // SC--005 Macro '%s' is not being defined
                            xx_source_err_exit_c( ERR_MAC_DEF_NOT, macname2 );
                            /* never return */
                        }
                        cc = getarg();
                        if( cc == CC_omit ) {
                            // SC--048 A control word parameter is missing
                            xx_source_err_exit( ERR_MAC_DEF_MISS );
                            /* never return */
                        }
                        get_macro_name( g_tok_start, macname2 );
                        if( strcmp( "end", macname2 ) != 0 ) {
                            // SC--002 The control word parameter '%s' is invalid
                            xx_source_err_exit_c( ERR_MAC_DEF_INV, macname2 );
                            /* never return */
                        }
                        compend = true;
                        break;              // out of read loop
                    }
                }
            }
            work = mem_alloc( sizeof( inp_line ) + cb->s.f->usedlen );
            work->next = NULL;
            strcpy( work->value, buff2 );
            if( last != NULL ) {
                last->next = work;
            }
            last = work;
            if( head == NULL ) {
                head = work;
            }
            macro_line_count++;
        }                                   // end read loop
        if( cb->s.f->flags & (FF_eof | FF_err) ) {
            // error SC--004 End of file reached
            // macro '%s' is still being defined
            xx_source_err_exit_c( ERR_MAC_DEF_EOF, macname1 );
            /* never return */
        }
    }                                   // end compbegin

    if( compend ) {                     // macro END definition processing
        mac_entry   *   me;

        me = find_macro( macro_dict, macname1 );
        if( me != NULL ) {              // delete macro with same name
            free_macro_entry( macro_dict, me );
        }

        ProcFlags.in_macro_define = 0;

        me  = mem_alloc( sizeof( mac_entry ) );
        me->next = NULL;
        me->label_cb = NULL;
        strcpy( me->name, macname1 );
        me->macline = head;
        me->lineno = lineno_start;
        me->mac_file_name = cb->s.f->filename;

        add_macro_entry( macro_dict, me );

        if( (cb->fmflags & II_research)
          && GlobalFlags.firstpass ) {
            sprintf( linestr, "%d", macro_line_count );
            g_info( INF_MAC_DEFINED, macname1, linestr );
        }
    } else {
        xx_source_err_exit_c( ERR_MAC_DEF_LOGIC, macname1 );
        /* never return */
    }
    scan_restart = g_scandata.e;
    return;
}


/***************************************************************************/
/* MACRO EXIT  causes immediate  termination of the  macro or  input file  */
/* currently being processed  and resumption of the  higher-level file or  */
/* macro (if any) or termination of processing (if none).                  */
/*                                                                         */
/*     +-------+--------------------------------------------------+        */
/*     |       |                                                  |        */
/*     |  .ME  |    <line>                                        |        */
/*     |       |                                                  |        */
/*     +-------+--------------------------------------------------+        */
/*                                                                         */
/* This control word does  not cause a break.   If an  operand "line" has  */
/* been specified,  it will be processed  as an input line immediately on  */
/* return to the higher-level file or macro.   If the .ME control word is  */
/* used in the highest-level file, SCRIPT advances to the top of the next  */
/* page  and  prints  any  stacked   output  before  termination  of  all  */
/* processing.                                                             */
/*                                                                         */
/* EXAMPLES                                                                */
/*  (1) .me .im nextfile                                                   */
/*      This will  terminate the file  or macro currently  being processed */
/*      and  cause the  higher-level  file or  macro  to imbed  "nextfile" */
/*      before it does anything else.                                      */
/*  (2) .me .me                                                            */
/*      This will  terminate the current  file or  macro and will  in turn */
/*      terminate the higher-level file or macro.                          */
/*                                                                         */
/*                                                                         */
/* ! the line operand is ignored for .me in the master document file       */
/*                                                                         */
/***************************************************************************/

void    scr_me( void )
{
    condcode        cc;

    if( input_cbs->prev != NULL ) {     // if not master document file
        cc = getarg();
        if( cc != CC_omit ) {              // line operand present

            free_lines( input_cbs->hidden_head );       // clear stacked input
            split_input( buff2, g_tok_start, input_cbs->fmflags );    // stack line operand

            // now move stacked line to previous input stack

            input_cbs->hidden_head->next = input_cbs->prev->hidden_head;
            input_cbs->prev->hidden_head = input_cbs->hidden_head;

            input_cbs->hidden_head = NULL;  // and delete from current input
            input_cbs->hidden_tail = NULL;
        }
    }

    input_cbs->fmflags |= II_eof;       // set eof

    input_cbs->if_cb->if_level = 0;     // terminate
    ProcFlags.keep_ifstate = false;     // ... all .if controls
    scan_restart = g_scandata.e;
    return;
}


/***************************************************************************/
/* ! EMPTY PAGE  is not implemented ( not used in OW documentation )       */
/*                                                                         */
/* EMPTY PAGE,  EXECUTE MACRO:   EMPTY PAGE controls suppression of empty  */
/* pages (pages  that contain nothing in  the text area);   EXECUTE MACRO  */
/* treats the operand line as a  macro,  even if Macro Substitution (.MS)  */
/* is OFF.                                                                 */
/*                                                                         */
/*      +-------+--------------------------------------------------+       */
/*      |       |                                                  |       */
/*      |  .EM  |    <YES|NO|OFFNO>  !not implemented              |       */
/*      |       |                                                  |       */
/*      +-------+--------------------------------------------------+       */
/*      |       |                                                  |       */
/*      |  .EM  |    .macro <args>                                 |       */
/*      |       |                                                  |       */
/*      +-------+--------------------------------------------------+       */
/*                                                                         */
/* Neither form of this control word causes a break.                       */
/*                                                                         */
/* .EM <YES|NO|OFFNO>:  The  situation can often arise in  which an empty  */
/*    page  is created  (a  page that  contains  only  top and/or  bottom  */
/*    running titles).    By default,   SCRIPT does  output these  pages.  */
/*    Specifying .EM YES allows  SCRIPT to print them,   .EM NO specifies  */
/*    that they are not to be printed,  and .EM OFFNO specifies that they  */
/*    are not to be printed and that the  page number is not to be incre-  */
/*    mented.   ".EM YES"  is the  initial value.    Empty pages  will be  */
/*    printed unless ".em no" is encountered.   If the operand is omitted  */
/*    then "YES" is assumed.                                               */
/* .EM .macro <parameters>:  If the operands are missing it is treated as  */
/*    EMPTY PAGE (See above).   If the first operand of the ".EM" control  */
/*    word begins with a control word indicator,  then that first operand  */
/*    is  treated as  a  Macro or  Remote  name.    Optional keyword  and  */
/*    positional parameters  may follow the  macro name.   The  local set  */
/*    symbol &*0  is set to the  count of positional parameters  and &*1,  */
/*    &*2, etc. contain their values.   The symbol &* contains the entire  */
/*    parameter list.   Keyword parameters are accessed as set symbols by  */
/*    the keyword name.                                                    */
/*                                                                         */
/* EXAMPLES                                                                */
/* (1) .EM .TEST parm1 KW1=parm2                                           */
/*     A macro named  TEST is invoked;  &*0 is  set to 1,  &*1  is set to  */
/*     "parm1",   &KW1   is  set   to  "parm2",    and  &*   is  set   to  */
/*     "parm1 KW1=parm2".                                                  */
/* (2) .EM .SK 1                                                           */
/*     A macro named "SK" is invoked.    A native control word will never  */
/*     be assumed.   If the specified macro has not already been defined,  */
/*     an error will result.                                               */
/***************************************************************************/

void    scr_em( void )
{
    char            macname[MAC_NAME_LENGTH + 1];
    condcode        cc;
    inputcb     *   cb;
    mac_entry   *   me;

    cb = input_cbs;

    cc = getarg();

    if( cc == CC_omit ) {
        xx_source_err_exit( ERR_MAC_NAME_INV );
        /* never return */
    }

    if( *g_tok_start == SCR_char ) {      // possible macro name
        /*  truncate name if too long WITHOUT error msg
         *  this is wgml 4.0 behaviour
         *
         */
        get_macro_name( g_tok_start + 1, macname ); // over .

        me = find_macro( macro_dict, macname );
    } else {
        me = NULL;                      // no macro name
    }

    if( me == NULL ) {                  // macro not specified or not defined
        xx_source_err_exit( ERR_MAC_NAME_INV );
        /* never return */
    }
    split_input( buff2, g_tok_start, input_cbs->fmflags );    // stack line operand
    scan_restart = g_scandata.e;
    return;
}

