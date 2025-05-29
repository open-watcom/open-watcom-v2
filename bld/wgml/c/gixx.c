/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2012 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WGML tags :I1  :I2  :I3  :IREF     implementation
*                         :IH1 :IH2 :IH3
*
*                          ix=x (index group) attribute not implemented
*
*                         :IREF   not completely implemented
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*  :I1 - :I3  :IH1 - :IH3 :IREF common input processing                   */
/*   hx_lvl is 1 to 3 for :Ix and :IHx                                     */
/*         and 0      for :IREF                                            */
/***************************************************************************/

static void gml_ixxx_common( const gmltag * entry, int hx_lvl )
{
    bool            idseen      = false;    // create reference entry if true
    bool            pgseen      = false;    // set pgvalue to pgpageno if true
    bool            printseen   = false;    // needed to catch empty string values
    bool            refidseen   = false;    // required by IREF & used by I2 I3
    bool            seeidseen   = false;    // used in processing IHx
    bool            seeseen     = false;    // needed to catch empty string values
    char            hxstring[TAG_NAME_LENGTH + 1 + 1];
    char            ixrefid[REFID_LEN + 1]; // holds attribute ixrefid value
    char            refrefid[REFID_LEN + 1];// holds attribute refrefid value
    char            seerefid[REFID_LEN + 1];// holds attribute seerefid value
    char            lvlc;
    char        *   p;
    char        *   pa;
    char        *   pgtext      = NULL;     // val_start for pg = <string> value
    char        *   printtxt    = NULL;     // val_start for print = <string> value
    char        *   seetext     = NULL;     // val_start for see = <string> value
    char        *   txt;                    // val_start for entry value
    condcode        cc;                     // result code
    ereftyp         pgvalue;
    getnum_block    gn;
    ix_h_blk    *   ixhbase;                // ix_h_blk->lower points to list to search
    ix_h_blk    *   ixhwk;                  // new ix_h_blk to be inserted
    ix_h_blk    *   ixhwork;                // current ix_h_blk/ix_h_blk to attach refs to
    ref_entry   *   refwork;                // new ref_entry to be inserted
    ref_entry   *   refwk;                  // ref_entry found for value of refid
    ref_entry   *   seeidwk;                // ref_entry found for value of seeid
    unsigned        pgtextlen       = 0;    // val_len for pg = <string> value
    unsigned        printtxtlen     = 0;    // val_len for print = <string> value
    unsigned        seetextlen      = 0;    // val_len for see = <string> value
    unsigned        txtlen;                 // val_len for entry value
    att_name_type   attr_name;
    att_val_type    attr_val;

    if( input_cbs->fmflags & II_tag_mac ) {   // ensure next line is valid
        input_cbs->s.m->ix_seen = true;     // records use of tag, even if indexing is off
    }

    if( !GlobalFlags.index ) {          // index option not active
        ProcFlags.index_tag_cw_seen = true;
        scandata.s = scandata.e;         // ignore tag
        return;
    }

    start_doc_sect();                   // if not already done

    lvlc = '0' + hx_lvl;
    hxstring[0] = GML_char;           // construct tagname for possible error msg
    strcpy( hxstring + 1, entry->tagname );

    pgvalue = pgnone;

    p = scandata.s;

    /***********************************************************************/
    /*  Scan attributes for Ix IHx IREF                                    */
    /*  Note: this table reflects which attributes actually apply to the   */
    /*  various tags, which is not always the same as given in the         */
    /*  Reference Manual                                                   */
    /*  id          Ix  IHx                                                */
    /*  ix          Ix  IHx IREF                                           */
    /*  pg          Ix  IREF                                               */
    /*  print       IHx                                                    */
    /*  refid       I2  I3 IREF                                            */
    /*  see         IHx IREF                                               */
    /*  seeid       IHx IREF                                               */
    /*  NOTE: when Ix/IHx tags reach end-of-tag because of an attribute    */
    /*        that they do not use, that attribute and value become part   */
    /*        of the index term; when IREF does the same, it is treated    */
    /*        as text and appear in the document text                      */
    /***********************************************************************/

    SkipSpaces( p );
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;; ) {
            p = get_tag_att_name( p, &pa, &attr_name );
            if( ProcFlags.reprocess_line )
                break;
            if( ProcFlags.tag_end_found )
                break;
            if( strcmp( "id", attr_name.attname.t ) == 0 ) {
                p = get_refid_value( p, &attr_val, ixrefid );
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                if( hx_lvl > 0 ) {      // :Ix :IHx
                    idseen = true;      // id attribute found
                } else {                // end-of-tag for :IREF
                    p = pa;             // restore spaces before text
                    break;
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "refid", attr_name.attname.t ) == 0 ) {
                p = get_refid_value( p, &attr_val, refrefid );
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                if( (hx_lvl == 0) || ((hx_lvl > 1) && (hxstring[2] == lvlc)) ) {
                    refidseen = true;   // refid attribute found
                    refwk = find_refid( ix_ref_dict, refrefid );
                    if( refwk == NULL ) {   // refid not in dict
                        if( GlobalFlags.lastpass ) {// this is an error
                            xx_line_err_exit_cc( ERR_ID_UNDEFINED, refrefid, attr_val.tok.s );
                            /* never return */
                        }
                    }
                } else {                // not allowed for :I1 and :IHx
                    xx_line_err_exit_cc( ERR_REF_NOT_ALLOWED, hxstring, attr_val.tok.s );
                    /* never return */
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "pg", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                scandata.s = p;
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                if( (hx_lvl == 0) || (hxstring[2] == lvlc) ) {
                    pgseen = true;
                    if( attr_val.quoted == ' ' ) {  // value not quoted
                        if( strcmp( "start", attr_val.specval ) == 0 ) {
                            pgvalue = pgstart;
                        } else if( strcmp( "end", attr_val.specval ) == 0 ) {
                            pgvalue = pgend;
                        } else if( strcmp( "major", attr_val.specval ) == 0 ) {
                            pgvalue = pgmajor;
                        }
                    }
                    if( pgvalue == pgnone ) {                // arbitrary string value
                        pgvalue = pgstring;
                        pgtext = mem_tokdup( attr_val.tok.s, attr_val.tok.l );  // use text instead of pageno
                        pgtextlen = attr_val.tok.l;
                    }
                } else {                        // end-of-tag for IHx
                    p = pa;                     // restore spaces before text
                    break;
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "print", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                scandata.s = p;
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                if( hxstring[3] == lvlc ) {     // IHx only
                    printseen = true;
                    printtxt = mem_tokdup( attr_val.tok.s, attr_val.tok.l );
                    printtxtlen = attr_val.tok.l;
                } else {                        // end-of-tag for Ix, IREF
                    p = pa;                     // restore spaces before text
                    break;
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "seeid", attr_name.attname.t ) == 0 ) {
                p = get_refid_value( p, &attr_val, seerefid );
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                if( (hx_lvl == 0) || (hxstring[3] == lvlc) ) {  // IREF IHx
                    seeidseen = true;
                    seeidwk = find_refid( ix_ref_dict, seerefid );
                    if( seeidwk == NULL ) {             // not in dict, this is an error
                        if( GlobalFlags.lastpass ) {    // during lastpass
                            xx_line_err_exit_cc( ERR_ID_UNDEFINED, seerefid, attr_val.tok.s );
                            /* never return */
                        }
                    }
                } else {                        // end-of-tag for Ix
                    p = pa;                     // restore spaces before text
                    break;
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "see", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                scandata.s = p;
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                if( hx_lvl == 0 || (hxstring[3] == lvlc) ) {// :IREF :IHx
                    seeseen = true;
                    seetext = mem_tokdup( attr_val.tok.s, attr_val.tok.l );
                    seetextlen = attr_val.tok.l;
                } else {                        // end-of-tag for Ix
                    p = pa;                     // restore spaces before text
                    break;
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "ix", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                gn.arg.s = attr_val.tok.s;
                gn.arg.e = attr_val.tok.s + attr_val.tok.l;
                gn.ignore_blanks = false;
                cc = getnum( &gn );

                if( (cc == CC_pos) || (cc == CC_neg) ) {

                    /* Groups are ignored, issue warning */

                    xx_warn_c( WNG_UNSUPP_ATT, "ix" );

                    if( (gn.result < 1) || (gn.result > 9) ) { // out of range
                        xx_line_err_exit_c( ERR_STRUCT_RANGE, attr_val.tok.s );
                        /* never return */
                    }
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else {
                p = pa; // restore spaces before text
                break;
            }
        }
    }

    SkipDot( p );                       // possible tag end
    SkipSpaces( p );                    // step over spaces
    if( hx_lvl > 0 ) {                  // not for IREF, take existing text, if any, as-is
        while( *p == '\0' ) {           // we need a text line for :Ix :IHx
            get_line( true );
            /*
             * buff2 must be restored if it is to be reprocessed
             * so that any symbol substitutions will reflect any
             * changes made by the tag calling it
             */
            scandata.s = buff2;
            scandata.e = buff2 + buff2_lg;
            if( check_tagname( scandata.s, NULL ) != NULL // tag found: error
              || (*scandata.s == SCR_char)          // cw found: error
              || (input_cbs->fmflags & II_eof) ) {  // EOF found: error
                xx_err_exit( ERR_TEXT_NOT_TAG_CW );
                /* never return */
            }
            process_line();
            p = scandata.s;             // new line is part of current tag
        }
        SkipSpaces( p );                // step over spaces
    }

    /***********************************************************************/
    /* process the found attributes and the text line                      */
    /***********************************************************************/

    txt = p;
    txtlen = strlen( txt );
    if( txt[txtlen - 1] == CONT_char ) {
        txt[txtlen - 1] = '\0';
        txtlen--;
    }
    if( txt[txtlen - 1] != in_esc ) {
        intrans( txt, strlen( txt ) + 1, g_curr_font );
    }
    for( ; txtlen > 0; txtlen-- ) { // back off trailing spaces
        if( txt[txtlen - 1] != ' ' ) {
            break;
        }
    }

    if( !pgseen ) {
        pgvalue = pgpageno;             // set default
    }

    if( hx_lvl == 0 ) {                 // :IREF tag
        if( !refidseen ) {              // refid= missing
            xx_err_exit( ERR_ATT_MISSING );
            /* never return */
        }
    } else if( !refidseen ) {           // not required for refid
        if( hx_lvl == 1 ) {             // first level tag
            ixhlvl[0] = true;           // record first level found
            ixhlvl[1] = false;          // second level not found
        } else if( hx_lvl == 2 ) {      // second level tag
            if( !ixhlvl[0] ) {          // first level must exist
                xx_err_exit_c( ERR_PARENT_UNDEF, hxstring );
                /* never return */
            }
            ixhlvl[1] = true;           // record first level found
        } else if( hx_lvl == 3 ) {      // third level tag
            if( !ixhlvl[1] ) {          // second level must exist
                xx_err_exit_c( ERR_PARENT_UNDEF, hxstring );
                /* never return */
            }
        }
    }


    if( GlobalFlags.lastpass ) {
        if( hx_lvl == 0 ) {                 // :IREF tag

        /***********************************************************************/
        /* processing for :IREF                                                */
        /***********************************************************************/

            if( refidseen && (refwk != NULL) ) {
                ixhwk = refwk->u.ix.hblk;
            } else {
                ixhwk = ixhtag[hx_lvl];
            }

            if( !ProcFlags.reprocess_line && *p != '\0' ) {
                SkipDot( p );                       // possible tag end
                if( *p != '\0' ) {
                    process_text( p, g_curr_font);  // if text follows
                }
            }

        } else if( hxstring[2] == lvlc ) {    // test for :Ix

        /***********************************************************************/
        /* processing for :Ix                                                  */
        /***********************************************************************/

            switch( hx_lvl ) {              // processing for :I1 :I2 :I3
            case 1 :
                ixhbase = ixhtag[hx_lvl - 1];
                ixhwork = index_dict;
                ixhwk = find_create_ix_h_entry( ixhwork, ixhbase, printtxt, printtxtlen,
                                                txt, txtlen, hx_lvl - 1 );
                if( ixhwk->prt_term == printtxt ) {         // printtxt was reassigned
                    printtxt = NULL;
                }
                ixhtag[hx_lvl] = ixhwk;
                break;
            case 2 :
            case 3 :
                if( refidseen && (refwk != NULL) ) {
                    if( hx_lvl > refwk->u.ix.hblk->ix_lvl ) {
                        ixhbase = refwk->u.ix.hblk;
                        ixhwork = refwk->u.ix.hblk->lower;
                    } else {
                        ixhbase = refwk->u.ix.base;
                        ixhwork = refwk->u.ix.hblk;
                    }
                } else {
                    ixhbase = ixhtag[hx_lvl - 1];
                    ixhwork = ixhtag[hx_lvl - 1]->lower;
                }
                ixhwk = find_create_ix_h_entry( ixhwork, ixhbase, printtxt, printtxtlen,
                                                txt, txtlen, hx_lvl - 1 );
                if( ixhwk->prt_term == printtxt ) {         // printtxt was reassigned
                    printtxt = NULL;
                }
                if( !refidseen && (hx_lvl == 2) ) {
                    ixhtag[hx_lvl] = ixhwk;
                }
                break;
            default:
                break;
            }

        } else if( hxstring[3] == lvlc ) {    // test for :IHx

        /***********************************************************************/
        /* processing for :IHx                                                 */
        /***********************************************************************/

            switch( hx_lvl ) {              // processing for :IH1 :IH2 :IH3
            case 1 :
                ixhbase = ixhtag[hx_lvl - 1];
                ixhwork = index_dict;
                ixhwk = find_create_ix_h_entry( ixhwork, ixhbase, printtxt, printtxtlen,
                                                txt, txtlen, hx_lvl - 1 );
                if( ixhwk->prt_term == printtxt ) {         // printtxt was reassigned
                    printtxt = NULL;
                }
                ixhtag[hx_lvl] = ixhwk;
                break;
            case 2 :
            case 3 :
                ixhbase = ixhtag[hx_lvl - 1];
                ixhwork = ixhtag[hx_lvl - 1]->lower;
                ixhwk = find_create_ix_h_entry( ixhwork, ixhbase, printtxt, printtxtlen,
                                                txt, txtlen, hx_lvl - 1 );
                if( ixhwk->prt_term == printtxt ) {         // printtxt was reassigned
                    printtxt = NULL;
                }
                if( hx_lvl == 2 ) {
                    ixhtag[hx_lvl] = ixhwk;
                }
                break;
            default:
                break;
            }
        }

        /***********************************************************************/
        /* at this point, ixhwk has been set to the ix_h_block to which the    */
        /* references are to be attached                                       */
        /***********************************************************************/

        if( hxstring[3] != lvlc ) {                             // not IH1/IH2/IH3
            if( ixhwk->entry == NULL ) {
                init_entry_list( ixhwk );
            }
            find_create_ix_e_entry( ixhwk, pgtext, pgtextlen, NULL, pgvalue );
        }

        if( seeidseen ) {   // get reference value from the ix_h_blk record found for seeid
            if( ixhwk->entry == NULL ) {
                init_entry_list( ixhwk );
            }
            find_create_ix_e_entry( ixhwk, seeidwk->u.ix.hblk->ix_term,
                                    seeidwk->u.ix.hblk->ix_term_len, seeidwk->u.ix.hblk, pgsee );
        }

        if( seeseen ) {
            if( ixhwk->entry == NULL ) {
                init_entry_list( ixhwk );
            }
            find_create_ix_e_entry( ixhwk, seetext, seetextlen, NULL, pgsee );
        }
    }

    if( idseen ) {                 // ID specified create reference entry
        if( GlobalFlags.firstpass || GlobalFlags.lastpass ) {
            refwork = find_refid( ix_ref_dict, ixrefid );
            if( GlobalFlags.firstpass ) {           // first pass: build dict
                if( refwork == NULL ) {             // new entry
                    refwork = add_new_refid( &ix_ref_dict, ixrefid, NULL );
                } else {                            // duplicate id
                    dup_refid_err_exit( refwork->refid, "figure" );
                    /* never return */
                }
            }
            if( GlobalFlags.lastpass ){         // last pass: add data
                if( refwork == NULL ) {         // shouldn't happen
                    xx_err_exit_c( ERR_ID_UNDEFINED, ixrefid );
                    /* never return */
                }
                refwork->u.ix.hblk = ixhwk;
                refwork->u.ix.base = ixhtag[hx_lvl - 1];
            }
        }
    }
    if( pgtext != NULL ) {
        mem_free( pgtext );
    }
    if( printtxt != NULL ) {
        mem_free( printtxt );
    }
    if( seetext != NULL ) {
        mem_free( seetext );
    }

    ProcFlags.null_value = false;
    ProcFlags.post_ix = true;           // records use of index tag only if indexing is on

    scandata.s = scandata.e;
    return;
}

/***************************************************************************/
/*   I1, I2, I3                                                            */
/*                                                                         */
/*      Format: :In [id='id-name']                                         */
/*                  [pg=start                                              */
/*                      end                                                */
/*                      major                                              */
/*                      'character string']                                */
/*                  [refid='id-name'].<text line>                          */
/*         (n=1,2,3)                                                       */
/*                                                                         */
/* These tags will cause an index entry to be created.  Index entry tags   */
/* may be used at any point in the document after the :gdoc tag.  The text */
/* line with the index entry tag is used to create an index term for the   */
/* index entry.  The index command line option must be specified for the   */
/* index entry tags to be processed.  The :I1 tag is used to create a      */
/* primary index entry.  The :I2 tag is used to create an index subentry   */
/* for the previous primary index entry.  The :I3 creates an index         */
/* subentry for the previously specified :I2 tag.                          */
/*                                                 The id attribute        */
/* assigns an identifier name to the created index entry.  The identifier  */
/* name is used by other tags when processing an index reference, and must */
/* be unique within the document.                                          */
/*                                 The pg attribute determines the way in  */
/* which the page number for the index entry is presented.  If the         */
/* attribute value is start, the index entry will have a page range.  The  */
/* end attribute value on an index entry will mark the end of a previously */
/* started page range.  The attribute value major makes the page number    */
/* reference of higher priority than the other page references in the      */
/* index entry, and causes it to be listed first.  If a character string   */
/* is specified as the attribute value, the character string is placed in  */
/* the index instead of a page number.                                     */
/*                                      The refid attribute will cause the */
/* index entry to be associated with a specific higher level index entry   */
/* rather than the index entry which directly precedes it in the document. */
/* The refid attribute may be used with the :I2 and the :I3 tags.          */
/*                                                                         */
/***************************************************************************/

void    gml_i1( const gmltag * entry )
{
    gml_ixxx_common( entry, 1 );
}

void    gml_i2( const gmltag * entry )
{
    gml_ixxx_common( entry, 2 );
}

void    gml_i3( const gmltag * entry )
{
    gml_ixxx_common( entry, 3 );
}


/***************************************************************************/
/*   IH1, IH2, IH3                                                         */
/*                                                                         */
/*      Format: :IHn [id='id-name']                                        */
/*                   [ix=x]                                                */
/*                   [print='character string']                            */
/*                   [see='character string']                              */
/*                   [seeid='id-name'].<text line>                         */
/*                                                                         */
/*          (n=1,2,3)                                                      */
/*          (x=0 -> 8)                                                     */
/*                                                                         */
/* The index heading tags will cause an index entry to be created.  Index  */
/* headings may be used at any point in the document.                      */
/*                                                     The text line with  */
/* the index entry tag is used to create an index term for the index       */
/* entry.  The index heading tag does not generate a page number reference */
/* with the index term in the index.  The index command line option must   */
/* be specified for the index entry tags to be processed.  The :IH1 tag is */
/* used to create a primary index entry.  The :IH2 tag is used to create   */
/* an index subentry for the previous primary index entry.  The :IH3       */
/* creates an index subentry for the previously specified :IH2 tag.        */
/*                                                                   The   */
/* id attribute assigns an identifier name to the created index entry.     */
/* The identifier name is used by other tags when processing an index      */
/* reference, and must be unique within the document.                      */
/*                                                     The ix attribute    */
/* selects one of the index groups (from zero through eight), with zero    */
/* being the default.                                                      */
/*                     The print attribute causes the specified character  */
/* string to be displayed in the index instead of the index term.  The     */
/* index term is still used to determine where in the index the entry      */
/* should be placed.                                                       */
/*                    The see attribute will cause the supplied character  */
/* string to be used as a page number reference.  The character text "See" */
/* will prefix the character string in the index if there are no           */
/* references in the index entry.  If there are index subentries or page   */
/* references, the string "See also" will be prefixed to the character     */
/* string.  It is your responsibility to ensure that index entries         */
/* specified in the character string are actually in the index.  The see   */
/* attribute may only be used when the index entry is of level one or two. */
/*                                                                         */
/* The seeid attribute is used to reference an index entry.  The index     */
/* term created by the referenced index entry is used instead of a page    */
/* number.  If the referenced index entry has the print attribute          */
/* specified, the character string value of the print attribute will be    */
/* used instead of the index term.  The character string "See" will prefix */
/* the index term in the index if there are no page references in the      */
/* index entry.  If there are index subentries or page references, the     */
/* string "See also" will be prefixed to the index term.  The seeid        */
/* attribute may only be used when the index entry is of level one or two. */
/*                                                                         */
/***************************************************************************/

void    gml_ih1( const gmltag * entry )
{
    gml_ixxx_common( entry, 1 );
}

void    gml_ih2( const gmltag * entry )
{
    gml_ixxx_common( entry, 2 );
}

void    gml_ih3( const gmltag * entry )
{
    gml_ixxx_common( entry, 3 );
}


/***************************************************************************/
/*  IREF                                                                   */
/*  Format: :IREF refid='id-name'                                          */
/*               [pg=start                                                 */
/*                   end                                                   */
/*                   major                                                 */
/*                   'character string']                                   */
/*               [see='character string']                                  */
/*               [seeid='id-name'].                                        */
/*                                                                         */
/* This tag will cause an index entry to be created.  The entry will be    */
/* similar to the one referenced by the refid attribute, which must be     */
/* specified.  Index references may be placed anywhere in the document.    */
/* The index command line option must be specified for the index reference */
/* tag to be processed.                                                    */
/*                       The refid attribute is used to reference an index */
/* entry identified by the specified identifier name.                      */
/*                                                     The pg attribute    */
/* determines the way in which the page number for the index entry is      */
/* presented.  If the attribute value is start, the index entry will have  */
/* a page range.  The end attribute value on an index entry will mark the  */
/* end of a previously started page range.  The attribute value major      */
/* makes the page number reference of higher priority than the other page  */
/* references in the index entry, and causes it to be listed first.  If a  */
/* character string is specified as the attribute value, the character     */
/* string is placed in the index instead of a page number.                 */
/*                                                          The see        */
/* attribute will cause the supplied character string to be used as a page */
/* number reference.  The character text "See" will prefix the character   */
/* string in the index if there are no references in the index entry.  If  */
/* there are index subentries or page references, the string "See also"    */
/* will be prefixed to the character string.  It is your responsibility to */
/* ensure that index entries specified in the character string are         */
/* actually in the index.  The see attribute may only be used when the     */
/* index entry is of level one or two.                                     */
/*                                      The seeid attribute is used to     */
/* reference an index entry.  The index term created by the referenced     */
/* index entry is used instead of a page number.  If the referenced index  */
/* entry has the print attribute specified, the character string value of  */
/* the print attribute will be used instead of the index term.  The        */
/* character string "See" will prefix the index term in the index if there */
/* are no page references in the index entry.  If there are index          */
/* subentries or page references, the string "See also" will be prefixed   */
/* to the index term.  The seeid attribute may only be used when the index */
/* entry is of level one or two.                                           */
/*                                                                         */
/***************************************************************************/

void    gml_iref( const gmltag * entry )
{
    gml_ixxx_common( entry, 0 );
}

