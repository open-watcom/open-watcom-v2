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
* Description:  macro utility functions
*
*             add_macro_cb_entry  -- add macro as input source
*             add_macro_parms     -- add macro call parms to local dictionary
*             free_lines          -- free macro source lines
*
*             scr_dm              -- .dm control word define macro
*             scr_me              -- .me control word macro end
*             scr_em              -- .em control word execute macro
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"



/***************************************************************************/
/*  add info about macro   to LIFO input list                              */
/*  if second parm is not null, macro is called via GML tag processing     */
/***************************************************************************/

void    add_macro_cb_entry( mac_entry * me, gtentry * ge )
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
    nip->pe_cb.line = NULL;

    init_dict( &nip->local_dict );

    nip->s.m        = new;

    new->lineno     = 0;
    new->macline    = me->macline;
    new->mac        = me;
    new->tag        = ge;

    if( ge == NULL ) {
        new->flags      = FF_macro;
        nip->fmflags    = II_macro;
    } else {
        new->flags      = FF_tag;
        nip->fmflags    = II_tag;
    }
    nip->fmflags |= input_cbs->fmflags & II_research;   // copy research mode
    nip->fmflags |= input_cbs->fmflags & II_sol;        // copy start-of-line
    nip->fmflags |= input_cbs->fmflags & II_eol;        // copy end-of-line

    nip->prev = input_cbs;
    input_cbs = nip;
    return;
}


/*
 * add macro parms from input line as local symbolic variables
 * for non quoted parms try to assign symbolic variables
 * i.e. .mac     a b c *xyz="1.8" d "1 + 2"
 *          01234
 *        only 1 space after mac is ignored
 *    will give  &*=    a b c *xyz="1.8" d "1 + 2"
 *              &*0=5
 *              &*1=a
 *              &*2=b
 *              &*3=c
 *              &*4=d
 *              &*5=1 + 2
 *        and &*xyz=1.8
 *
 *  the variable for &* is named _  This can be changed if this leads to
 *  conflicts  -> change define MAC_STAR_NAME in gtype.h
 *
 */

void    add_macro_parms( char * p )
{
    int             len;
    condcode        cc;

    /************************************************/
    /*.macro   parameters                           */
    /*      012                                     */
    /*      p points here 2 spaces are kept         */
    /************************************************/
    p++;
    len = strlen( p );
    if( len > 0 ) {
        char    starbuf[12];
        int     star0;
                                        // the macro parameter line
                                        // the name _ has to change (perhaps)
        add_symvar( &input_cbs->local_dict, MAC_STAR_NAME, p, no_subscript,
                    local_var );
        star0 = 0;
        garginit();
        cc = getarg();
        while( cc > omit ) {            // as long as there are parms
            char        c;
            char    *   scan_save;

            if( cc == pos ) {           // argument not quoted
                           /* look if it is a symbolic variable definition */
                scan_save  = scan_start;
                c          = *scan_save; // prepare value end
                *scan_save = '\0';      // terminate string
                scan_start = tok_start; // rescan for variable
                ProcFlags.suppress_msg = true;  // no errmsg please
                ProcFlags.blanks_allowed = 0;   // no blanks please

                scr_se();               // try to set variable and value

                ProcFlags.suppress_msg = false; // reenable err msg
                ProcFlags.blanks_allowed = 1;   // blanks again
                *scan_save = c;        // restore original char at string end
                scan_start = scan_save; // restore scan address
                if( scan_err ) {        // not variable=value format
                    cc = omit;
                    star0++;
                    sprintf( starbuf, "%d", star0 );
                    p = tok_start + arg_flen;
                    c = *p;                 // prepare value end
                    *p = '\0';              // terminate string
                    add_symvar( &input_cbs->local_dict, starbuf, tok_start,
                                no_subscript, local_var );
                    *p = c;                // restore original char at string end
                }

            }
            if( cc == quotes ) {        // add argument as local symbolic var
                star0++;
                sprintf( starbuf, "%d", star0 );
                p = tok_start + arg_flen;
                c = *p;                 // prepare value end
                *p = '\0';              // terminate string
                add_symvar( &input_cbs->local_dict, starbuf, tok_start,
                            no_subscript, local_var );
                *p = c;                // restore original char at string end
            }
            cc = getarg();              // look for next parm
        }
                                        // the positional parameter count
        add_symvar( &input_cbs->local_dict, "0", starbuf,
                    no_subscript, local_var );
    }

    if( input_cbs->fmflags & II_research && GlobalFlags.firstpass ) {
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
/* ".name" as  a user-defined control word  or as an Execute  Macro (.EM)  */
/* operand.                                                                */
/*                                                                         */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커       */
/*      |       |                                                  |       */
/*      |       |    name /line1/.../linen</>                      |       */
/*      |  .DM  |    name <BEGIN|END>                              |       */
/*      |       |    name DELETE                                   |       */
/*      |       |                                                  |       */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸       */
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
/* (1) The invoking of  defined user macros by ".name"  can be suppressed  */
/*     with the ".MS"  (Macro Substitution)  control word.    Invoking by  */
/*     ".EM .name" cannot be suppressed.                                   */
/* (2) The ".DM name END" operands  are verified for a  macro "name" that  */
/*     matches the ".DM name BEGIN".   ".DM" starting  in column one with  */
/*     no operands will also successfully terminate a macro definition.    */
/* (3) The user-defined  macro may  be invoked  with a  variable list  of  */
/*     keyword and positional operands                                     */
/*       .name operand1 operand2                                           */
/*     that will assign to the local Set Symbols &*1, &*2, ..., the value  */
/*     of corresponding operands in the macro call.   Each operand may be  */
/*     a character string,  a delimited  character string,  or a numeric.  */
/*     Numeric operands  that do not consist  entirely of digits  will be  */
/*     treated as a character string.                                      */
/*       .name key1=value key2=value                                       */
/*     Operands  that consist  of  a valid  Set  Symbol name  immediately  */
/*     followed by an  equal sign will assign  the value on the  right of  */
/*     the equal  sign to  the specified Set  Symbol before  invoking the  */
/*     macro.   If the Set Symbol begins with an asterisk the symbol will  */
/*     be local to the invoked macro.                                      */
/***************************************************************************/

void    scr_dm( void )
{
    char        *   nmstart;
    char        *   p;
    char        *   pn;
    char            save;
    int             len;
    int             macro_line_count;
    int             compbegin;
    int             compend;
    char            macname[MAC_NAME_LENGTH + 1];
    inp_line    *   head;
    inp_line    *   last;
    inp_line    *   work;
    line_number     lineno_start;
    condcode        cc;
    inputcb     *   cb;
    char            linestr[MAX_L_AS_STR];

    cb = input_cbs;

    garginit();

    cc = getarg();

    if( cc == omit ) {
        err_count++;
        g_err( err_missing_name );
        ultoa( cb->s.f->lineno, linestr, 10 );
        g_info( inf_file_line, linestr, cb->s.f->filename );
        show_include_stack();
        return;
    }

    p   = tok_start;
    pn  = macname;
    len = 0;

    /*  truncate name if too long WITHOUT error msg
     *  this is wgml 4.0 behaviour
     *
     */
    while( *p && is_macro_char( *p ) ) {
        if( len < MAC_NAME_LENGTH ) {
            *pn++ = tolower( *p++ );    // copy lowercase macroname
            *pn   = '\0';
        } else {
            break;
        }
        len++;
    }
    macname[MAC_NAME_LENGTH] = '\0';

    cc = getarg();
    if( cc == omit ) {                  // nothing found
        err_count++;
        // SC--048 A control word parameter is missing
        g_err( err_mac_def_fun, macname );
        ultoa( cb->s.f->lineno, linestr, 10 );
        g_info( inf_file_line, linestr, cb->s.f->filename );
        return;
    }

    p    = scan_start;
    head = NULL;
    last = NULL;
    save = *p;             // save char so we can make null terminated string
    *p   = '\0';
    macro_line_count = 0;

    compend   = !stricmp( tok_start, "end" );
    compbegin = !stricmp( tok_start, "begin" );
    if( !(compbegin | compend) ) { // only .dm macname /line1/line2/ possible
        char    sepchar;

        if( cc == quotes ) {
            tok_start--;    // for single line .dm /yy/xxy/.. back to sepchar
        }
        if( ProcFlags.in_macro_define ) {
            err_count++;
            g_err( err_mac_def_nest, tok_start );
            ultoa( cb->s.f->lineno, linestr, 10 );
            g_info( inf_file_line, linestr, cb->s.f->filename );
            return;
        }
        ProcFlags.in_macro_define = 1;

        *p   = save;
        lineno_start = cb->s.f->lineno;

        p = tok_start;
        sepchar = *p++;
        nmstart = p;
        while( *p ) {
            while( *p && *p != sepchar ) {  // look for seperator
                ++p;
            }
            len = p - nmstart;
            *p = '\0';
            work = mem_alloc( sizeof( inp_line ) + len );
            work->next = NULL;
            strcpy_s( work->value, len + 1, nmstart );
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
        compend = 1;                    // so the end processing will happen
    }                                   // BEGIN or END not found

    if( compend && !(ProcFlags.in_macro_define) ) {
        err_count++;
        // SC--003: A macro is not being defined
        g_err( err_mac_def_end, macname );
        ultoa( cb->s.f->lineno, linestr, 10 );
        g_info( inf_file_line, linestr, cb->s.f->filename );
        return;
    }
    if( compbegin && (ProcFlags.in_macro_define) ) {
        err_count++;
        // SC--002 The control word parameter '%s' is invalid
        g_err( err_mac_def_nest, macname );
        ultoa( cb->s.f->lineno, linestr, 10 );
        g_info( inf_file_line, linestr, cb->s.f->filename );
    }
    *p  = save;
    if( compbegin ) {                   // start new macro define

        ProcFlags.in_macro_define = 1;
        lineno_start = cb->s.f->lineno;

        while( !(cb->s.f->flags & FF_eof) ) {  // process all macro lines

            get_line( true );

            if( cb->s.f->flags & (FF_eof | FF_err) ) {
                break;                  // out of read loop
            }
            p = buff2;
            if( *p == SCR_char ) {      // possible macro end
                int second = (*(p + 1) == SCR_char);// for ..dm case

                if( tolower( *(p + 1 + second) ) == 'd' &&
                    tolower( *(p + 2 + second) ) == 'm' &&
                    (*(p + 3 + second) == ' ' || *(p + 3 + second) == '\0') ) {

                    garginit();

                    cc = getarg();
                    if( cc == omit ) {  // only .dm  means macro end
                        compend = 1;
                        break;          // out of read loop
                    }
                    p = scan_start;
                    save = *p;
                    *p = '\0';
                    if( strnicmp( macname, tok_start, MAC_NAME_LENGTH ) ) {
                        // macroname from begin different from end
                        err_count++;
                        // SC--005 Macro '%s' is not being defined
                        g_err( err_mac_def_not, tok_start );
                        ultoa( cb->s.f->lineno, linestr, 10 );
                        g_info( inf_file_line, linestr, cb->s.f->filename );
                        *p = save;
                        free_lines( head );
                        return;
                    }
                    *p = save;
                    cc = getarg();
                    if( cc == omit ) {
                        err_count++;
                        // SC--048 A control word parameter is missing
                        g_err( err_mac_def_miss );
                        ultoa( cb->s.f->lineno, linestr, 10 );
                        g_info( inf_file_line, linestr, cb->s.f->filename );
                        free_lines( head );
                        return;
                    }
                    p = scan_start;
                    save = *p;
                    *p = '\0';
                    if( stricmp( tok_start, "end") ) {
                        err_count++;
                        // SC--002 The control word parameter '%s' is invalid
                        g_err( err_mac_def_inv, tok_start );
                        ultoa( cb->s.f->lineno, linestr, 10 );
                        g_info( inf_file_line, linestr, cb->s.f->filename );
                        free_lines( head );
                        return;
                    }
                    compend = 1;
                    break;              // out of read loop
                }
            }
            work = mem_alloc( sizeof( inp_line ) + cb->s.f->usedlen );
            work->next = NULL;
            strcpy_s( work->value, cb->s.f->usedlen + 1, buff2 );
            if( last != NULL ) {
                last->next = work;
            }
            last = work;
            if( head == NULL ) {
                head = work;
            }
            macro_line_count++;
        }                               // end read loop
        if( cb->s.f->flags & (FF_eof | FF_err) ) {
            err_count++;
            // error SC--004 End of file reached
            // macro '%s' is still being defined
            g_err( err_mac_def_eof, macname );
            ultoa( cb->s.f->lineno, linestr, 10 );
            g_info( inf_file_line, linestr, cb->s.f->filename );
            free_lines( head );
            return;
        }
    }                                   // end compbegin

    if( compend ) {                     // macro END definition processing
        mac_entry   *   me;

        me = find_macro( macro_dict, macname );
        if( me != NULL ) {              // delete macro with same name
            free_macro_entry( &macro_dict, me );
        }

        ProcFlags.in_macro_define = 0;

        me  = mem_alloc( sizeof( mac_entry ) );
        me->next = NULL;
        me->label_cb = NULL;
        strcpy( me->name, macname );
        me->macline = head;
        me->lineno = lineno_start;
        me->mac_file_name = cb->s.f->filename;

        add_macro_entry( &macro_dict, me );

        if( cb->fmflags & II_research && GlobalFlags.firstpass ) {
            utoa( macro_line_count, linestr, 10 );
            g_info( inf_mac_defined, macname, linestr );
        }
    } else {
        ProcFlags.in_macro_define = 0;
        err_count++;
        g_err( err_mac_def_logic, macname );
        free_lines( head );
        show_include_stack();
        return;
    }
    scan_restart = scan_stop;
    return;
}


/***************************************************************************/
/* MACRO EXIT  causes immediate  termination of the  macro or  input file  */
/* currently being processed  and resumption of the  higher-level file or  */
/* macro (if any) or termination of processing (if none).                  */
/*                                                                         */
/*     +----------------------------------------------------------+        */
/*     |       |                                                  |        */
/*     |  .ME  |    <line>                                        |        */
/*     |       |                                                  |        */
/*     +----------------------------------------------------------+        */
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

        garginit();

        cc = getarg();
        if( cc != omit ) {              // line operand present

            free_lines( input_cbs->hidden_head );   // clear stacked input
            split_input( buff2, tok_start, true );  // stack line operand

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
    scan_restart = scan_stop;
    return;
}


static void macro_missing( void )
{
    char        linestr[MAX_L_AS_STR];

    g_err( err_mac_name_inv );
    if( input_cbs->fmflags & II_tag_mac ) {
        ultoa( input_cbs->s.m->lineno, linestr, 10 );
        g_info( inf_mac_line, linestr, input_cbs->s.m->mac->name );
    } else {
        ultoa( input_cbs->s.f->lineno, linestr, 10 );
        g_info( inf_file_line, linestr, input_cbs->s.f->filename );
    }
}


/***************************************************************************/
/* ! EMPTY PAGE  is not implemented ( not used in OW documentation )       */
/*                                                                         */
/* EMPTY PAGE,  EXECUTE MACRO:   EMPTY PAGE controls suppression of empty  */
/* pages (pages  that contain nothing in  the text area);   EXECUTE MACRO  */
/* treats the operand line as a  macro,  even if Macro Substitution (.MS)  */
/* is OFF.                                                                 */
/*                                                                         */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커       */
/*      |       |                                                  |       */
/*      |  .EM  |    <YES|NO|OFFNO>  !not implemented              |       */
/*      |       |                                                  |       */
/*      |컴컴컴�|컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴|       */
/*      |       |                                                  |       */
/*      |  .EM  |    .macro <args>                                 |       */
/*      |       |                                                  |       */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸       */
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
    char        *   p;
    char        *   pn;
    char            macname[MAC_NAME_LENGTH + 1];
    condcode        cc;
    inputcb     *   cb;
    mac_entry   *   me;
    int             len;

    cb = input_cbs;

    garginit();

    cc = getarg();

    if( cc == omit ) {
        err_count++;
        macro_missing();
        show_include_stack();
        return;
    }

    if( *tok_start == SCR_char ) {      // possible macro name
        p   = tok_start + 1;            // over .
        pn  = macname;
        len = 0;

        /*  truncate name if too long WITHOUT error msg
         *  this is wgml 4.0 behaviour
         *
         */
        while( *p && is_macro_char( *p ) ) {
            if( len < MAC_NAME_LENGTH ) {
                *pn++ = *p++;           // copy macroname
                *pn   = '\0';
            } else {
                break;
            }
            len++;
        }
        macname[MAC_NAME_LENGTH] = '\0';

        me = find_macro( macro_dict, macname );
    } else {
        me = NULL;                      // no macro name
    }

    if( me == NULL ) {                  // macro not specified or not defined
        err_count++;
        macro_missing();
        show_include_stack();
        return;
    } else {
        split_input( buff2, tok_start, true ); // stack line operand in input
    }
    scan_restart = scan_stop;
    return;
}

