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
* Description: implement .ix (index)  script control word
*                        only used options are implemented
*                        i.e.   index structure 1 only
*                               s1 s2 s3
*                               . dump   (experimental)
*         extension found during testing:
*                               s1 s2 s3 xxx
*           xxx is treated like gml :I3 pg="XXX"  attribute
*
*         not implemented are   s1 s2 . ref
*                               . purge
*
*  comments are from script-tso.txt
****************************************************************************/

#include "wgml.h"
#include "gvars.h"

/**************************************************************************/
/*                                                                        */
/*  !not all options are supported / implemented                          */
/*                                                                        */
/*                                                                        */
/* INDEX builds  an index structure  with up  to three levels  of headers */
/* with references, or prints the index structure.                        */
/*                                                                        */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커      */
/*      |       |                                                  |      */
/*      |       |    <1|n> 's1' <'s2' <'s3'>> <<.> <ref>>          |      */
/*      |  .IX  |                                                  |      */
/*      |       |    <1|n> . <DUMP|PURGE>                          |      */
/*      |       |                                                  |      */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸      */
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

void    scr_ix( void )
{
    condcode        cc;                 // resultcode from getarg()
    static char     cwcurr[4] = {" ix"};// control word string for errmsg
    int             lvl;              // max index level in control word data
    int             k;
    int             comp_len;// compare length for searching existing entries
    int             comp_res;           // compare result
    char        *   ix[3];              // index string(s) to add
    uint32_t        ixlen[3];           // corresponding lengths
    ix_h_blk    * * ixhwork;            // anchor point for insert
    ix_h_blk    *   ixhwk;              // index block
    ix_h_blk    *   ixhcurr[3];         // active index heading block per lvl
    ix_e_blk    *   ixewk;              // index entry block
    bool            do_nothing;         // true if index string duplicate
    uint32_t        wkpage;


    scan_restart = scan_stop;

    if( !(GlobalFlags.index && GlobalFlags.lastpass) ) {
        return;                         // no need to process .ix
    }                                   // no index wanted or not lastpass
    cwcurr[0] = SCR_char;
    lvl = 0;                            // index level

//  if( ProcFlags.page_started ) {
//      wkpage = page;
//  } else {
        wkpage = page + 1;              // not quite clear TBD
//  }

    garginit();                         // over control word

    while( lvl < 3 ) {                  // try to get 3 lvls of index

        cc = getarg();

        if( cc == omit || cc == quotes0 ) { // no (more) arguments
            if( lvl == 0 ) {
                parm_miss_err( cwcurr );
                return;
            } else {
                break;
            }
        } else {
            if( *tok_start == '.' && arg_flen == 1  ) {
                if( lvl > 0 ) {
                    xx_opt_err( cwcurr, tok_start );
                    break;             // .ix s1 s2 . ref format not supprted
                }
                cc = getarg();
                if( cc == pos || cc == quotes ) {   // .ix . dump ???
                    if( arg_flen == 4 ) {
                        if( !strnicmp( tok_start, "DUMP", 4 ) ) {

                            ixdump( index_dict );

                            break;
                        }
                    }
                    xx_opt_err( cwcurr, tok_start );// unknown option
                } else {
                    parm_miss_err( cwcurr );
                    return;
                }
                break;                  // no index entry text
            }
            ix[lvl] = tok_start;
            *(tok_start + arg_flen) = 0;
            ixlen[lvl] = arg_flen;
            lvl++;
        }
    }
    cc = getarg();
/***************************************************************************/
/*  The docu says .ix "I1" "I2" "I3" "extra" is invalid, but WGML4 accepts */
/*  it without error and processes it like the :I3 pg="extra" attribute    */
/***************************************************************************/
//  if( cc != omit ) {
//      parm_extra_err( cwcurr, tok_start - (cc == quotes) );
//      return;
//  }

    if( lvl > 0 ) {                     // we have at least one index string

        ixhwork = &index_dict;
        for( k = 0; k < lvl; ++k ) {
            do_nothing = false;
            while( *ixhwork != NULL ) { // find alfabetic point to insert
                comp_len = ixlen[k];
                if( comp_len > (*ixhwork)->ix_term_len )
                    comp_len = (*ixhwork)->ix_term_len;
                ++comp_len;
                comp_res = strnicmp( ix[k], (*ixhwork)->ix_term, comp_len );
                if( comp_res > 0 ) {    // new is later in alfabet
                    ixhwork = &((*ixhwork)->next);
                    continue;
                }
                if( comp_res == 0 ) {   // equal
                    if( ixlen[k] == (*ixhwork)->ix_term_len ) {
                        do_nothing = true;
                        break;          // entry already there
                    }
                    if( ixlen[k] > (*ixhwork)->ix_term_len ) {
                        ixhwork = &((*ixhwork)->next);
                        continue;       // new is longer
                    }
                }
                break;                  // insert point reached
            }
            if( !do_nothing ) {
                // insert point reached
                ixhwk = mem_alloc( sizeof( ix_h_blk ) );
                ixhwk->next  = *ixhwork;
                ixhwk->ix_lvl= k + 1;
                ixhwk->lower = NULL;
                ixhwk->entry = NULL;
                ixhwk->prt_term = NULL;
                ixhwk->prt_term_len = 0;
                ixhwk->ix_term_len   = ixlen[k];
                ixhwk->ix_term = mem_alloc( ixlen[k] + 1 );
                strcpy( ixhwk->ix_term, ix[k] );
                *ixhwork = ixhwk;
            } else {            // string already in dictionary at this level
                ixhwk = *ixhwork;
            }
            ixhcurr[lvl] = ixhwk;
            if( k < lvl ) {
                ixhwork = &(ixhwk->lower); // next lower level
            }
        }

        // now add the pageno to index entry
        if( ixhwk->entry == NULL ) {    // first pageno for entry

/***************************************************************************/
/*  The docu says .ix "I1" "I2" "I3" "extra" is invalid, but WGML4 accepts */
/*  it without error and processes it like the :I3 pg="extra" attribute    */
/*  try to process the extra parm                                          */
/***************************************************************************/
            if( cc != omit ) {
                *(tok_start + arg_flen) = 0;
                fill_ix_e_blk( &(ixhwk->entry), ixhwk, pgstring, tok_start, arg_flen );
            } else {
                fill_ix_e_blk( &(ixhwk->entry), ixhwk, pgpageno, NULL, 0 );
            }
        } else {
            ixewk = ixhwk->entry;
            while( ixewk->next != NULL ) {  // find last entry
                ixewk = ixewk->next;
            }
            if( (ixewk->entry_typ >= pgstring) || (ixewk->u.page_no != wkpage) ) {
                // if last entry doesn't point to current page create entry
                if( cc != omit ) {
                    *(tok_start + arg_flen) = 0;
                    fill_ix_e_blk( &(ixewk->next), ixhwk, pgstring, tok_start, arg_flen );
                } else {
                    fill_ix_e_blk( &(ixewk->next), ixhwk, pgpageno, NULL, 0 );
                }
            }
        }
    }
    return;
}

