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
* Description: implement .ix (index)  script control word
*                        only used options are implemented:
*                               s1
*                               s1 . ref
*                               s1 s2
*                               s1 s2 . ref
*                               s1 s2 s3
*                               s1 s2 s3 . ref
*                               s1 s2 s3 ref
*                               *       (primary reference designator)
*
*         not implemented are   <1|n>   (structure number)
*                               . purge (no preceding items except structure number)
*                               . dump  (no preceding items except structure number)
*         these items are identified and appropriate messages issued,
*         but otherwise have no effect
*
*  Note: the "." is the control word indicator, normally "." but changeable
*        similarly, the "*" can be changed using control word DC
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/**************************************************************************/
/*                                                                        */
/*  not all options are supported / implemented                           */
/*                                                                        */
/*                                                                        */
/* INDEX builds  an index structure  with up  to three levels  of headers */
/* with references, or prints the index structure.                        */
/*                                                                        */
/*      +-------+--------------------------------------------------+      */
/*      |       |                                                  |      */
/*      |       |    <1|n> 's1' <'s2' <'s3'>> <<.> <ref>>          |      */
/*      |  .IX  |                                                  |      */
/*      |       |    <1|n> . <DUMP|PURGE>                          |      */
/*      |       |                                                  |      */
/*      +-------+--------------------------------------------------+      */
/*                                                                        */
/* This control word does not cause a break.                              */
/*                                                                        */
/* <1|n>:  specifies the index structure (from 1 to 9) on which the oper- */
/*    ation is to be performed.  If omitted, structure 1 is used.         */
/*                                                                        */
/* Building the Index                                                     */
/*                                                                        */
/* 's1' <'s2' <'s3'>>:  adds  up to three levels of index  headers to the */
/*    index structure.  The index headers may be specified as undelimited */
/*    strings,  or as delimited strings  if they contain embedded blanks. */
/*    The current output page number will be used as the reference entry. */
/* <<.>  <ref>>:  specifies  a  "reference string"  that  is  to be  used */
/*    instead of the current output page  number for the reference entry. */
/*    If s3 (or s2 and s3)  is  omitted,  then the control word indicator */
/*    (normally period) must be placed between the index level(s) and the */
/*    reference operand.   If all four operands  are present,  the use of */
/*    the control word indicator as a separator is optional.   Use of the */
/*    control word indicator means a reference must follow,  even if null */
/*    (see the Example below).                                            */
/*                                                                        */
/* It is possible  to designate one or more reference  entries as primary */
/* reference entries,   so that  they will  appear first  in the  list of */
/* reference entries  for that index  header entry,  regardless  of where */
/* they appeared in  the input.   If the reference entry  is an asterisk, */
/* then the  current page number  will be  used as the  primary reference */
/* entry.   If the reference entry is a character string starting with an */
/* asterisk,  then the characters following the  asterisk will be used as */
/* the primary reference entry.   The asterisk character used to identify */
/* a primary reference may be changed with a ".DC PIX" control word.      */
/*                                                                        */
/* Printing the Index Structure                                           */
/*                                                                        */
/* .  DUMP:  DUMP  causes it to be  output and then deleted.    The index */
/*    structure will be printed in  alphabetic order within levels.   For */
/*    purposes of ordering the level entries,  the entries are treated as */
/*    if entered in uppercase as defined by the Translate Uppercase (.TU) */
/*    control word.                                                       */
/*       A list of characters  may be specified that are to  be sorted as */
/*    if they were blanks with the ".DC IXB" control word.  Characters to */
/*    be  totally  ignored  for  sorting purposes  may  be  defined  with */
/*    ".DC IXI".                                                          */
/*       Index references on a range of consecutive pages are joined with */
/*    the "&SYSPRS" symbol,   Page Range Separator,  which  defaults to a */
/*    hyphen  character (-)   but may  be redefined  before printing  the */
/*    index.   Other references are separated  by the Page List Separator */
/*    symbol,  ",  ",  which defaults  to comma/blank (, ).   The default */
/*    range character may be redefined or disabled with ".DC IXJ".   Only */
/*    internally generated page numbers are eligible for joining, never a */
/*    user reference string.   A null reference between two references on */
/*    consecutive  pages  will  make   those  references  ineligible  for */
/*    joining.                                                            */
/* .  PURGE:   PURGE causes  the index  structure to  be deleted  without */
/*    printing it.                                                        */
/*                                                                        */
/* NOTES                                                                  */
/* (1) This control word will be ignored  if the NOINDEX option is speci- */
/*     fied.  This can reduce the processing time for draft documents.    */
/*                                                                        */
/* EXAMPLES                                                               */
/* (1) .ix 'level1' 'level2'                                              */
/*     adds  a first-  and a  second-level  header and  the current  page */
/*     number as the reference entry, to index structure 1.               */
/* (2) .ix 2 'level1' . 'see...'                                          */
/*     adds a  first-level header  and the  reference string  "see..." to */
/*     index structure 2.                                                 */
/* (3) .ix 'level1' . ''                                                  */
/*     adds a  first-level header  and a  null reference  entry to  index */
/*     structure 1.                                                       */
/* (4) .ix 'level1' 'level2' . *                                          */
/*     adds a first- and second-level index  header to index structure 1, */
/*     using the current page number as a primary reference entry.        */
/* (5) .ix 'level1' . '*text'                                             */
/*     adds a first-level  index header to index structure  1,  using the */
/*     characters "text" as the primary reference entry.                  */
/* (6) The index  structure is printed with  the DUMP operand.    See the */
/*     Index Entry control word (.IE)  for  customizing the format of the */
/*     result.                                                            */
/*       .ix . DUMP                                                       */
/**************************************************************************/


/***************************************************************************/
/*  .ix control word processing                                            */
/***************************************************************************/

void scr_ix( void )
{
    bool            do_major;           // true if reference is a major reference
    bool            do_nothing;         // true if index string/ref is duplicate
    char        *   ix[3] = { NULL, NULL, NULL };   // index string start pointers
    char        *   p;
    char            pix_char;           // major reference prefix char ($pix)
    char        *   ref = NULL;         // ref string start pointer
    condcode        cc;                 // result code
    entry_list  *   entry;
    getnum_block    gn;
    int             lvl;                // max index level in control word data
    int             i;
    unsigned        k;
    ix_h_blk    *   ixbase[3] = { NULL, NULL, NULL };   // current list base
    ix_h_blk    *   ixhwork;            // insertion point/found match item
    ix_h_blk    *   ixhwk;              // index block
    symsub      *   dictval;
    unsigned        ixlen[3] = {0, 0, 0};   // index string lengths
    unsigned        reflen = 0;         // ref string length
    unsigned        wkpage;             // predicted page number

    static char     cwcurr[4] = {" ix"};// control word string for errmsg

    if( input_cbs->fmflags & II_tag_mac ) {   // ensure next line is valid
        input_cbs->s.m->ix_seen = true;     // records use of control word, even if indexing is off
    }

    if( !GlobalFlags.index ) {
        ProcFlags.index_tag_cw_seen = true;
        scan_restart = g_scandata.e;
        return;                         // no need to process .ix
    }

    start_doc_sect();                   // if not already done
    scan_restart = g_scandata.e;

    cwcurr[0] = SCR_char;
    lvl = 0;                            // index level

    find_symvar( sys_dict, "$pix", SI_no_subscript, &dictval);
    pix_char = *(dictval->value);
    wkpage = g_page + 1;                  // predicted number of current page

    p = g_scandata.s;

    /* Check for no operands, a structure number, or a DUMP/PURGE line */

    g_tok_start = NULL;                   // clear token start address
    cc = getarg();                      // get next operand
    if( cc == CC_omit
      || cc == CC_quotes0 ) { // no operands
        /* Position adjusted to avoid buffer overflow */
        xx_line_err_exit_cc( ERR_PARM_MISSING, cwcurr, g_scandata.s - 1 );
        /* never return */
    }

    /****************************************************************/
    /* If the operand is quoted, it is an index entry even if       */
    /* numeric, DUMP or PURGE                                       */
    /* DUMP and PURGE are identified and ignored only if preceded   */
    /* by '.' (the control word indicator)                          */
    /****************************************************************/

    if( cc != CC_quotes ) {

        /* Unquoted numeric string here must be a structure number */

        gn.arg.s = g_tok_start;
        gn.arg.e = g_scandata.e;
        gn.ignore_blanks = false;
        cc = getnum( &gn );

        if( (cc == CC_pos)
          || (cc == CC_neg) ) {
            p = g_tok_start;
            for( k = 0; k < arg_flen; k++ ) {
                if( (*p == ' ')
                  || !my_isdigit( *p ) ) {
                    break;
                }
                p++;
            }
            if( *p == ' ' ) {   // if space found first, then numeric is structure number

                /* Structures are ignored, issue warning */

                xx_warn_c( WNG_UNSUPP_CW_OPT, "structure" );

                if( (gn.result < 1)
                  || (gn.result > 9) ) { // out of range
                    xx_line_err_exit_c( ERR_STRUCT_RANGE, g_tok_start );
                    /* never return */
                }
                cc = getarg();                  // get next operand
                if( cc == CC_omit
                  || cc == CC_quotes0 ) { // no operands
                    xx_line_err_exit_cc( ERR_PARM_MISSING, cwcurr, g_tok_start );
                    /* never return */
                }
            }
        } else {

            /* Check for '.' (the control word indicator) */

            if( *g_tok_start == SCR_char
              && arg_flen == 1  ) {

                cc = getarg();                  // get next operand

                /* Only DUMP/PURGE allowed in this position */

                if( cc == CC_omit
                  || cc == CC_quotes0 ) { // no operands
                    xx_line_err_exit_cc( ERR_PARM_MISSING, cwcurr, g_tok_start );
                    /* never return */
                } else if( (arg_flen == 4)
                  && stricmp( "DUMP", g_tok_start ) == 0 ) {
                    xx_warn_c( WNG_UNSUPP_CW_OPT, "DUMP" );
                } else if( (arg_flen == 5)
                  && stricmp( "PURGE", g_tok_start ) == 0 ) {
                    xx_warn_c( WNG_UNSUPP_CW_OPT, "PURGE" );
                } else {
                    xx_line_err_exit_c( ERR_BAD_DP_VALUE, g_tok_start );
                    /* never return */
                }
                cc = getarg();                  // get next operand
            }
        }
    }

    /****************************************************************/
    /* Parse the arguments to extract the index elements, of which  */
    /* four can be accomodated: three index terms plus one refernce */
    /* Note: there is something subtle going on here betwen the     */
    /*       value of lvl, as used in the code below, and the       */
    /*       trapping of a reference after less than three index    */
    /*       terms                                                  */
    /****************************************************************/

    for( lvl = 0; lvl < 3; lvl++ ) {
        if( cc == CC_omit
          || cc == CC_quotes0 ) { // no (more) arguments
            break;
        }

        /* Process a reference preceded by SCR_char */

        if( *g_tok_start == SCR_char
          && arg_flen == 1  ) {    // identify reference
            cc = getarg();                                  // get next operand
            if( (cc == CC_pos)
              || (cc == CC_quotes) ) {           // identify reference
                ref = g_tok_start;
                reflen = arg_flen;
                cc = getarg();                  // get next operand
            }
            break;
        }
        ix[lvl] = g_tok_start;
        ixlen[lvl] = arg_flen;
        cc = getarg();                  // get next operand
    }

    if( lvl == 3 ) {                                        // check for reference
        if( *g_tok_start == SCR_char
          && arg_flen == 1  ) {    // skip reference indicator
            cc = getarg();                                  // get next operand
        }

        if( (cc == CC_pos)
          || (cc == CC_quotes) ) {   // reference found
            ref = g_tok_start;
            reflen = arg_flen;
            cc = getarg();                      // get next operand
        }
    }

    if( (cc == CC_pos)
      || (cc == CC_quotes) ) {       // extra data on line
        xx_line_err_exit_c( ERR_EXTRA_DATA, g_tok_start );
        /* never return */
    }

    ProcFlags.post_ix = true;           // records use of control word only if indexing is on

    if( !GlobalFlags.lastpass ) {
        return;
    }

    /* Now fill the index structures -- but only on the last pass */

    if( lvl > 0 ) {                             // we have at least one index string

        /* Create an ix_h_blk in ixhcurr[] for each non-null entry in ix[] */

        ixhwork = index_dict;
        ixbase[0] = index_dict;             // preserve starting point
        for( i = 0; i < lvl; i++ ) {
            if( ix[i] == NULL ) {           // we are done
                break;
            }
            do_nothing = false;
            ixhwk = find_create_ix_h_entry( ixhwork, ixbase[i], NULL, 0, ix[i],
                                            ixlen[i], i );
            if( i + 1 < lvl ) {
                ixbase[i + 1] = ixhwk;              // preserve attach point for lower level
                ixhwork = ixhwk->lower;             // next lower level
            }
        }

        /* Add the ix_e_blk, with reference/page number information */

        do_major = false;
        if( (ref != NULL)
          && (ref[0] == pix_char) ) {      // identify major reference
            do_major = true;
            reflen--;
            if( reflen == 0 ) {         // major numeric reference
                ref = NULL;
            } else {                    // major string reference: drop the first char
                ref++;
            }
        }

        if( ixhwk->entry == NULL ) {    // first ref/pageno for entry
            init_entry_list( ixhwk);
            if( ref != NULL ) {
                if( do_major ) {
                    find_create_ix_e_entry( ixhwk, ref, reflen, NULL, PGREF_majorstring );
                } else {
                    find_create_ix_e_entry( ixhwk, ref, reflen, NULL, PGREF_string );
                }
            } else {
                if( do_major ) {
                    find_create_ix_e_entry( ixhwk, NULL, 0, NULL, PGREF_major );
                } else {
                    find_create_ix_e_entry( ixhwk, NULL, 0, NULL, PGREF_pageno );
                }
            }
        } else {
            entry = ixhwk->entry;
            if( ref != NULL ) {
                if( do_major ) {
                    find_create_ix_e_entry( ixhwk, ref, reflen, NULL, PGREF_majorstring );
                } else {
                    find_create_ix_e_entry( ixhwk, ref, reflen, NULL, PGREF_string );
                }
            } else {
                if( do_major ) {
                    find_create_ix_e_entry( ixhwk, NULL, 0, NULL, PGREF_major );
                } else {
                    find_create_ix_e_entry( ixhwk, NULL, 0, NULL, PGREF_pageno );
                }
            }
        }
    }
    return;
}
