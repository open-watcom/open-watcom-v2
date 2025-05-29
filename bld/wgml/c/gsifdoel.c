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
* Description:  process script .if  .th  .do  .el  control words
*
*               comments taken from script-tso.txt
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*  for evaluation term in .if control word                                */
/***************************************************************************/

typedef struct termcb {
    int         term_number;            // value only if numeric
    char        *term_string;           // value as string
    unsigned    term_length;            // length of string
    bool        numeric;                // term  is numeric
} termcb;


/***************************************************************************/
/*  relational operators                                                   */
/***************************************************************************/

typedef enum relop {
    EQ, NE, LT, GT, LE, GE
} relop;


/***************************************************************************/
/*  logical operators                                                      */
/***************************************************************************/

typedef enum logop {
    AND, OR
} logop;


/***************************************************************************/
/*  show current .if control block for debugging                           */
/***************************************************************************/

void    show_ifcb( char * txt, ifcb * cb ) {
    if( cb->if_level ) {
        out_msg( "%-8s %dL%d %c%c %s %s %s %s %s %s %s %s kp %d\n",
            txt,
            inc_level,
            cb->if_level,
            cb->if_flags[cb->if_level].iftrue?'t':' ',
            cb->if_flags[cb->if_level].iffalse?'f':' ',

            cb->if_flags[cb->if_level].ifthen?"th":"  ",
            cb->if_flags[cb->if_level].ifelse?"el":"  ",
            cb->if_flags[cb->if_level].ifdo?"do":"  ",
            cb->if_flags[cb->if_level].iflast?"last":"    ",
            cb->if_flags[cb->if_level].ifcwif?"cwif":"    ",
            cb->if_flags[cb->if_level].ifcwte?"cwte":"    ",
            cb->if_flags[cb->if_level].ifcwdo?"cwdo":"    ",
            cb->if_flags[cb->if_level].ifindo?"indo":"    ",
            ProcFlags.keep_ifstate
          );
    }
}

/***************************************************************************/
/* gargrelop   scan for relation operator in .if statement                 */
/*                                                                         */
/*                                                                         */
/* relation:    one of the following  "relational operators", preceded and */
/*              followed by at least one blank:                            */
/*                 =  or EQ  -  equal                                      */
/*                 ^= or <> or NE  -  not equal                            */
/*                 <  or LT  -  less than                                  */
/*                 >  or GT  -  greater than                               */
/*                 <= or LE  -  less than or equal                         */
/*                 >= or GE  -  greater than or equal                      */
/*                                                                         */
/***************************************************************************/

static condcode gargrelop( relop * r )
{
    condcode        cc;

    cc = getarg();                      // try unquoted string
    if( cc != CC_pos ) {
        return( cc );                   // scan error
    }
    if( !((*(g_tok_start + 1) == ' ')
      || (*(g_tok_start + 2) == ' ')) ) { // operator is max 2 chars long
        return( CC_no );                   // scan error
    }

    if( *(g_tok_start + 1) == ' ' ) {     // relop is single char
        switch( *g_tok_start ) {
        case '='  :
            *r = EQ;
            break;
        case '<' :
            *r = LT;
            break;
        case '>' :
            *r = GT;
            break;
        default:
            return( CC_no );               // scan error
            break;
        }
    } else {
        char    c2;

        c2 = my_tolower( *(g_tok_start + 1) );    // second char of relation operator
        switch( my_tolower( *g_tok_start ) ) {    // relop is 2 chars
        case '^'  :
            if( c2 == '=' ) {
                *r = NE;
            } else {
                return( CC_no );
            }
            break;
        case '<' :
            if( c2 == '=' ) {
                *r = LE;
            } else if( c2 == '>' ) {
                *r = NE;
            } else {
                return( CC_no );
            }
            break;
        case '>' :
            if( c2 == '=' ) {
                *r = GE;
            } else {
                return( CC_no );
            }
            break;
        case 'e' :
            if( c2 == 'q' ) {
                *r = EQ;
            } else {
                return( CC_no );
            }
            break;
        case 'n' :
            if( c2 == 'e' ) {
                *r = NE;
            } else {
                return( CC_no );
            }
            break;
        case 'l' :
            if( c2 == 'e' ) {
                *r = LE;
            } else {
                if( c2 == 't' ) {
                    *r = LT;
                } else {
                    return( CC_no );
                }
            }
            break;
        case 'g' :
            if( c2 == 'e' ) {
                *r = GE;
            } else {
                if( c2 == 't' ) {
                    *r = GT;
                } else {
                    return( CC_no );
                }
            }
            break;

        default:
            return( CC_no );
            break;
        }
    }

    return( cc );
}


/* gargterm  try to evaluate value
 *           first try numeric integers
 *           then  quoted strings
 *           last  unquoted strings
 */
static condcode gargterm( termcb * t )
{
    getnum_block    gn;
    condcode        cc;

    gn.arg = scandata;
    gn.ignore_blanks = false;
    cc = getnum( &gn );                 // try to get numeric value
    if( cc == CC_notnum ) {
        t->numeric = false;
        t->term_number = 0;

        cc = getqst();                  // try quoted string
        if( cc == CC_no ) {                // not quoted
            scandata.s = g_tok_start;   // reset start for next try

            cc = getarg();              // try unquoted string
            if( cc == CC_notnum ) {
                return( cc );           // scan error
            }
        }
        // prepare string   quoted or unquoted
        t->term_string = mem_tokdup( g_tok_start, arg_flen );
        t->term_length = arg_flen;
    } else {
        if( gn.arg.s >= gn.arg.e ) {
            scandata.s = gn.arg.e;        // enforce end of logical record
        } else {
            scandata.s = gn.arg.s;
        }
        t->numeric = true;
        t->term_number = gn.result;
        t->term_string = mem_tokdup( gn.resultstr, gn.length );
        t->term_length = gn.length;
    }
    return( cc );
}


/* ifcompare compare if condition as numeric integer or string
 *           depending whether both terms are integers
 *
 */
static bool ifcompare( termcb * t1, relop r, termcb * t2 )
{
    bool        result;
    int         term1;
    int         term2;

    term1 = t1->term_number;            // assume integers
    term2 = t2->term_number;

    if( !t1->numeric
      || !t2->numeric ) { // string compare
        char        *p1 = t1->term_string;
        char        *p2 = t2->term_string;
        unsigned    length;

        length = t1->term_length;
        if( length > t2->term_length )
            length = t2->term_length;
        while( length > 0 ) {           // try to find a difference
            if( *p1 != *p2 ) {
                break;                  // found
            } else {
                p1++;
                p2++;
            }
            --length;
        }

        term1 = *p1;                    // unequal chars or
        term2 = *p2;                    // corresponding chars of shorter string

        if( length == 0 ) {             // compared parts are equal
            if( t1->term_length == t2->term_length ) {
                term1 = term2;              // make compare equal true
            } else {
                if( t1->term_length < t2->term_length ) {
                    term1 = INT_MIN;       // shorter is smaller
                } else {
                    term2 = INT_MIN;
                }
            }
        }
    }

    switch( r ) {                       // now set compare result
    case EQ :
        result = (term1 == term2);
        break;
    case NE :
        result = (term1 != term2);
        break;
    case LT :
        result = (term1 <  term2);
        break;
    case GT :
        result = (term1 >  term2);
        break;
    case LE :
        result = (term1 <= term2);
        break;
    case GE :
        result = (term1 >= term2);
        break;

    default:
        result = false;
        if( input_cbs->fmflags & II_tag_mac ) {
            out_msg( "ERR_ifcompare internal logic error\n"
                     "\t\t\tLine %d of macro '%s'\n",
                     input_cbs->s.m->lineno, input_cbs->s.m->mac->name );
        } else {
            out_msg( "ERR_ifcompare internal logic error\n"
                     "\t\t\tLine %d of file '%s'\n",
                     input_cbs->s.f->lineno, input_cbs->s.f->filename );
        }
        show_include_stack();
        err_count++;
        break;
    }
    return( result );
}


/***************************************************************************/
/* IF  causes conditional processing of an object "line" depending on the  */
/* relationship of two numeric or character-string quantities.             */
/*                                                                         */
/*                             s1  relation  s2  <line>                    */
/*                       .IF                                               */
/*                             n1  relation  n2  <line>                    */
/*                                                                         */
/* <line>:  a control-word or text input line, starting with the first     */
/* non-blank character following the second operand.  If omitted, the next */
/* logical input line will be used as the <line> operand.  If the          */
/* condition is true, the <line> operand is processed; otherwise, the      */
/* <line> operand will be excluded from processing.  The THEN (.TH) and    */
/* ELSE (.EL) control words may be used instead of <line> to provide one   */
/* <line> to be processed if the condition is true and a different <line>  */
/* to be processed if the condition is false.  A series of .IFs may be     */
/* nested up to ten levels by making an .IF the object of a .TH or an .EL. */
/* See the .TH and .EL descriptions for details.                           */
/*                                                                         */
/* This control word does not cause a break; the <line> operand may.  If   */
/* both operands are valid numerics or numeric expressions then the        */
/* comparison will be between two signed integers.  If one of the oper-    */
/* ands is not a valid numeric expression then the comparison will be      */
/* between two character strings using the standard ASCII collating        */
/* sequence.  If the two character strings to be compared have unequal     */
/* lengths, then the comparison will be done from the start of both        */
/* strings with the minimum length of the two.  If those characters are    */
/* the same then the shorter string is considered to be the smaller.       */
/*                                                                         */
/* s1 and  s2:  undelimited  character strings,   or delimited  character  */
/*              strings that may contain any characters (including blank). */
/* n1 and n2:   integers or integer expressions containing no blanks.      */
/* relation:    one of the following  "relational operators", preceded and */
/*              followed by at least one blank:                            */
/*                 =  or EQ  -  equal                                      */
/*                 ^= or <> or NE  -  not equal                            */
/*                 <  or LT  -  less than                                  */
/*                 >  or GT  -  greater than                               */
/*                 <= or LE  -  less than or equal                         */
/*                 >= or GE  -  greater than or equal                      */
/*                                                                         */
/*  The two character or numeric operands and the "relational operator"    */
/*  form a  "condition" that  is either  "true" or  "false".   Multiple    */
/*  conditions  may occur  in one  .IF by  using one  of the               */
/*  following "logical operators" between conditions:                      */
/*              AND or &                                                   */
/*              OR  or |                                                   */
/*              such as:                                                   */
/*  .if condition1 and condition2 <line>                                   */
/*  Evaluation of multiple conditions is done left to right.               */
/***************************************************************************/

void    scr_if( void )
{
    bool            ifcond;             // current condition
    bool            totalcondition;     // resultant condition
    bool            firstcondition;     // first comparison .if
    char        *   p;
    condcode        cct1;
    condcode        cct2;
    condcode        ccrelop;
    ifcb        *   cb;                 // if stack ptr
    logop           logical;            // if more than 1 condition
    relop           relation;           // the relation between t1 and t2
    termcb          t1;                 // first argument
    termcb          t2;                 // second argument

    g_scan_err = false;

    firstcondition = true;              // first 2 terms to compare
    p = scandata.s;
    g_tok_start = NULL;

    process_late_subst(scandata.s);

    if( *(p + strlen(p) - 1) == CONT_char ) {  // remove trailing continue character
        *(p + strlen(p) - 1) = '\0';
    }

    cb = input_cbs->if_cb;              // get .if control block
    cb->if_flags[cb->if_level].ifcwif = false;  // reset cwif switch

    for( ;; ) {                         // evaluate if conditions

        cct1    = gargterm( &t1 );      // get term 1
        ccrelop = gargrelop( &relation );   // get relation operator
        cct2    = gargterm( &t2 );      // get term 2

        if( (cct1 == CC_no)
          || (cct2 == CC_no) ) {
            xx_source_err_exit( ERR_IF_TERM );
            /* never return */
        }
        if( ccrelop != CC_pos ) {
            xx_source_err_exit( ERR_IF_RELOP );
            /* never return */
        }

        // terms and operator ok now compare
        ifcond = ifcompare( &t1, relation, &t2 );
        mem_free( t1.term_string );     // don't need the strings anymore
        mem_free( t2.term_string );
        if( firstcondition ) {
            firstcondition = false;
            if( cb->if_level < MAX_IF_LEVEL ) {
                cb->if_level++;
                memset( &cb->if_flags[cb->if_level], '\0', sizeof( cb->if_flags[cb->if_level] ) );
                cb->if_flags[cb->if_level].iflast = true;
                cb->if_flags[cb->if_level].ifcwte = false;  // no .th .el yet
                cb->if_flags[cb->if_level].iftrue = false;  // cond not yet true
                cb->if_flags[cb->if_level].iffalse = false; // cond not yet false
            } else {
                xx_source_err_exit( ERR_IF_NESTING );
                /* never return */
            }
            totalcondition = ifcond;
        } else {
            // resultant condition
            if( logical == AND ) {
                totalcondition &= ifcond;
            } else {
                totalcondition |= ifcond;
            }
        }

        if( totalcondition ) {          // set if true / false flags
            cb->if_flags[cb->if_level].iftrue = true;
            cb->if_flags[cb->if_level].iffalse = false;
        } else {
            cb->if_flags[cb->if_level].iffalse = true;
            cb->if_flags[cb->if_level].iftrue = false;
        }

        SkipSpaces( scandata.s );

/*
 * test logical condition if not line end
 *         .if a = b or c GT d
 *                   ^^
 */
        if( *scandata.s != '\0' ) {
            if( *scandata.s == SCR_char ) {
                break;                  // .xx can't be logical operator
            }
            if( *(scandata.s + 1) == ' ' ) {// single char + blank
                if( *scandata.s  == '&' ) {
                    logical = AND;
                    scandata.s += 2;
                    continue;           // do next conditions
                } else if( *scandata.s == '|' ) {
                    logical = OR;
                    scandata.s += 2;
                    continue;           // do next conditions
                }
            } else {
                if( !strnicmp( "and ", scandata.s, 4 ) ) {
                    logical = AND;
                    scandata.s += 4;
                    continue;           // do next conditions
                } else if( !strnicmp( "or ", scandata.s, 3 ) ) {
                        logical = OR;
                        scandata.s += 3;
                        continue;       // do next conditions
                }
            }

        }
        break;                          // no more operators / conditions
    }


    if( cb->if_level > 1 ) {            // nested if
        if( cb->if_flags[cb->if_level - 1].ifthen ) { // object of .th
            if( cb->if_flags[cb->if_level - 1].iffalse ) {// last .if false

                cb->if_flags[cb->if_level].iftrue = true;// process nothing
                cb->if_flags[cb->if_level].iffalse = true;
            }
        } else {
            if( cb->if_flags[cb->if_level - 1].ifelse // object of .el
                && cb->if_flags[cb->if_level - 1].iftrue ) {// last .if true

                cb->if_flags[cb->if_level].iftrue = true;// process nothing
                cb->if_flags[cb->if_level].iffalse = true;
            }
        }
    }
    if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass ) {
          show_ifcb( "if", cb );
#if 0
          out_msg( "\t.if is %s Level %d\n"
                 "\t.ifcb iftrue %d, iffalse %d\n",
                 totalcondition ? "true " : "false",
                 cb->if_level,
                 cb->if_flags[cb->if_level].iftrue,
                 cb->if_flags[cb->if_level].iffalse );
#endif
    }

    if( *scandata.s != '\0' ) {             // rest of line is not empty
        split_input( buff2, scandata.s, input_cbs->fmflags );   // split and process next
        if( cb->if_flags[cb->if_level].iffalse ) {  // condition failed
            ProcFlags.pre_fsp = false;      // cancel fsp
        }
    }
    scan_restart = scandata.e;
    return;
}


/****************************************************************************/
/* THEN causes an input line to be conditionally included depending on the  */
/* truth value of a previous IF control word.                               */
/*                                                                          */
/*                                                                          */
/*             .TH      line                                                */
/*                                                                          */
/*                                                                          */
/* This control word does not cause a break, although the "line" may.  The  */
/* "line" that begins with the first non-blank character after the .TH      */
/* control word is processed only if the preceding .IF was "true".  The     */
/* line may include any control word except another Then (.TH) or an Else   */
/* (.EL).  It may be another .IF, and these may be nested up to ten levels. */
/* It may also be an Imbed (.IM) or Macro Call, in which case the current   */
/* .IF status and its nesting level will be saved and later restored when   */
/* the current file nest level is resumed.  If "line" is omitted then the   */
/* object of the Then has no effect.                                        */
/*                                                                          */
/* EXAMPLES                                                                 */
/* (1) This example starts  a new page or skips two  lines,  depending on   */
/*     whether &sptype has the value "page":                                */
/*        .if &sptype = page                                                */
/*        .   .th .pa                                                       */
/*        .   .el .sk 2                                                     */
/* (2) This example will imbed file AFILE if the value of &i is less than   */
/*     or equal to  one. Note that a .TH control word is assumed            */
/*     following an .IF if the immediately following record is not .TH or   */
/*     .EL:                                                                 */
/*        .if &i le 1                                                       */
/*        .im afile                                                         */
/****************************************************************************/

void    scr_th( void )
{
    ifcb    *   cb = input_cbs->if_cb;

    g_scan_err = false;
    cb->if_flags[cb->if_level].ifcwte = false;
    if( !cb->if_flags[cb->if_level].iflast
      || !(cb->if_flags[cb->if_level].iftrue
      || cb->if_flags[cb->if_level].iffalse)
      || cb->if_flags[cb->if_level].ifthen
      || cb->if_flags[cb->if_level].ifelse
      || cb->if_flags[cb->if_level].ifdo ) {
        xx_source_err_exit( ERR_IF_THEN );
        /* never return */
    }
    cb->if_flags[cb->if_level].iflast = false;
    cb->if_flags[cb->if_level].ifthen = true;
    ProcFlags.keep_ifstate = true;
    if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass ) {
        show_ifcb( "then", cb );
    }

    SkipSpaces( scandata.s );

    if( *scandata.s != '\0' ) {                 // rest of line is not empty
        split_input( buff2, scandata.s, input_cbs->fmflags );   // split and process next
    }
    scan_restart = scandata.e;
    return;
}


/***************************************************************************/
/* ELSE causes conditional processing of an input line, depending on the   */
/* truth value of a previous IF control word.                              */
/*                                                                         */
/*                                                                         */
/*         .EL     <line>                                                  */
/*                                                                         */
/* This control word does not cause a break.  However, a control-word      */
/* "line" operand may.  This control word may be used only immediately     */
/* following a .IF, a .TH or a .TH .DO group.  The "line" that begins with */
/* the first non-blank character after the .EL control word is processed   */
/* only if the preceding .IF was "false".  The line may include any        */
/* control word except another THEN (.TH) or ELSE (.EL).  The line may be  */
/* another .IF and these may be nested up to ten levels.  The object may   */
/* be an Imbed (.IM) or Macro Call, in which case the current status and   */
/* its nesting level will be saved and later restored when the current     */
/* file nest level is resumed.  When using a nested IF, the Else is always */
/* matched to the innermost unpaired Then.  It may thus be necessary to    */
/* enter an Else with no object line to define the required branching      */
/* structure.  If "line" is omitted then the object of the .EL has no      */
/* effect.                                                                 */
/*                                                                         */
/***************************************************************************/

void    scr_el( void )
{
    ifcb    *   cb = input_cbs->if_cb;

    g_scan_err = false;
    cb->if_flags[cb->if_level].iflast = false;
    cb->if_flags[cb->if_level].ifcwte = false;

    if( !(cb->if_flags[cb->if_level].iftrue
      || cb->if_flags[cb->if_level].iffalse)
      || cb->if_flags[cb->if_level].ifthen
      || cb->if_flags[cb->if_level].ifelse
      || cb->if_flags[cb->if_level].ifdo ) {
        xx_source_err_exit( ERR_IF_ELSE );
        /* never return */
    }
    cb->if_flags[cb->if_level].ifelse = true;
    ProcFlags.keep_ifstate = true;
    if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass ) {
        show_ifcb( "else", cb );
    }

    SkipSpaces( scandata.s );

    if( *scandata.s != '\0' ) {                 // rest of line is not empty
        split_input( buff2, scandata.s, input_cbs->fmflags );   // split and process next
    }
    scan_restart = scandata.e;
    return;
}


/***************************************************************************/
/* DO may be used following a Then or Else control word to allow multiple  */
/* input lines to be conditionally processed.                              */
/*                                                                         */
/*               .DO    <BEGIN|END>                                        */
/*                                                                         */
/* This control word may only be used as the object of a Then (.TH) or     */
/* Else (.EL) control word.  The input control word and text lines from    */
/* the .DO to the next corresponding ".DO END" control word are all        */
/* treated as the object of the Then or Else and are processed or ignored  */
/* depending on the truth value of the preceding .IF control word.         */
/* Further Ifs within a Do group are valid, as are Imbeds (.IM) and        */
/* Execute Macros (.EM).  This control word does not cause a break,        */
/* although control words within the Do group may create a break.  If no   */
/* operand is specified then BEGIN is assumed.                             */
/*                                                                         */
/***************************************************************************/

void    scr_do( void )
{
    ifcb    *   cb = input_cbs->if_cb;
    condcode    cc;

    g_scan_err = false;
    cc = getarg();

    cb->if_flags[cb->if_level].ifcwdo = false;
    if( cc == CC_omit
      || strnicmp( "begin", g_tok_start, 5 ) == 0 ) {
        if( !(cb->if_flags[cb->if_level].ifthen
          || cb->if_flags[cb->if_level].ifelse)
          || cb->if_flags[cb->if_level].ifdo ) {
            xx_source_err_exit( ERR_IF_DO );
            /* never return */
        }
        cb->if_flags[cb->if_level].ifdo = true;
        cb->if_flags[cb->if_level].ifindo = true;
        if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass ) {
            show_ifcb( "dobegin", cb );
        }
        scan_restart = scandata.e;
        return;
    } else {
        if( strnicmp( "end", g_tok_start, 3 ) == 0 ) {
            if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass ) {
                show_ifcb( "doend", cb );
            }
            do {                            // loop for last active .do begin

                if( cb->if_flags[cb->if_level].ifdo ) {

                    cb->if_flags[cb->if_level].ifdo = false;
                    if( (input_cbs->fmflags & II_research) &&
                        GlobalFlags.firstpass ) {
                        show_ifcb( "doend", cb );
                    }
                    scan_restart = scandata.e;
                    return;
                }

                if( (!cb->if_flags[cb->if_level].ifindo
                  && (cb->if_flags[cb->if_level].ifthen
                  || cb->if_flags[cb->if_level].ifelse))
                  || !(cb->if_flags[cb->if_level].iftrue
                  || cb->if_flags[cb->if_level].iffalse) ) {
                    xx_source_err_exit( ERR_IF_DO_END );
                    /* never return */
                }

                cb->if_flags[cb->if_level].ifindo = false;
            } while( cb->if_level-- > 0 );
#if 0
            if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass ) {
                out_msg( "\t.do end Level %d\n"
                         "\t.ifcb iftrue %d, iffalse %d\n",
                         cb->if_level,
                         cb->if_flags[cb->if_level].iftrue,
                         cb->if_flags[cb->if_level].iffalse );
            }
#endif
        } else {
            xx_source_err_exit( ERR_IF_DO_FUN );
            /* never return */
        }
    }
    if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass ) {
        show_ifcb( "do xx", cb );
    }
    scan_restart = scandata.e;
    return;
}

