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
* Description:  WGML tags :I1  :I2  :I3  :IREF     implementation
*                         :IH1 :IH2 :IH3
*
*                          ix=x (index group) attribute not implemented
*
*                         :IREF   not completely implemented
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include    "wgml.h"
#include    "gvars.h"

/***************************************************************************/
/*  find or create index header block                                      */
/*   returns created block                                                 */
/*                                                                         */
/***************************************************************************/
static ix_h_blk * find_create_ix_h_entry( ix_h_blk * * ixhwork,
                                          char * * printtxt, size_t printtxtlen,
                                          char * txt, size_t txtlen,
                                          int lvl )
{
    ix_h_blk * ixhwk;
    size_t  comp_len;       // compare length for searching existing entries
    int     comp_res;       // compare result
    bool    do_nothing;

    do_nothing = false;
    while( (*ixhwork != NULL) ) {       // find alfabetic point to insert
        comp_len = min( txtlen, (*ixhwork)->ix_term_len );
        comp_res = strnicmp( txt, (*ixhwork)->ix_term, comp_len + 1 );
        if( comp_res > 0 ) {            // new is later in alfabet
            ixhwork = &((*ixhwork)->next);
            continue;
        }
        if( comp_res == 0 ) {           // compared text equal
            if( txtlen > comp_len ) {   // new text longer than compared one
                ixhwork = &((*ixhwork)->next);
                continue;
            }
#if 0
            if( printtxtlen + (*ixhwork)->prt_term_len  > 0 ) {
                                // at least one printtxt specified
                if( printtxtlen != (*ixhwork)->prt_term_len ) {
                    ixhwork = &((*ixhwork)->next);  // different printtext
                    continue;
                } else {
                    if( !strnicmp( *printtxt,
                                   (*ixhwork)->prt_term, printtxtlen ) ) {

                        ixhwork = &((*ixhwork)->next); // different printtext
                        continue;
                    }
                }
            }
#endif
            do_nothing = true;          // entry already there
            break;
        }
        break;                          // insert point reached
    }
    if( do_nothing ) {
        ixhwk = *ixhwork;
        if( *printtxt != NULL ) {
            mem_free( *printtxt );
            *printtxt = NULL;
        }
    } else {                            // create block
        ixhwk = mem_alloc( sizeof( ix_h_blk ) );
        ixhwk->lower = NULL;
        ixhwk->entry = NULL;
        ixhwk->ix_lvl = lvl;
        ixhwk->ix_term_len = txtlen;
        ixhwk->ix_term = mem_alloc( txtlen + 1);
        strcpy_s( ixhwk->ix_term, txtlen + 1,txt );
        if( *printtxt != NULL ) {
            ixhwk->prt_term_len = printtxtlen;
            ixhwk->prt_term = *printtxt;
            *printtxt = NULL;
        } else {
            ixhwk->prt_term_len = 0;
            ixhwk->prt_term = NULL;
        }
        if( *ixhwork == NULL ) {
            *ixhwork = ixhwk;
            ixhwk->next = NULL;
        } else {
            ixhwk->next = *ixhwork;
            *ixhwork = ixhwk;
        }
    }
    return( ixhwk );
}


/***************************************************************************/
/*  :I1 - :I3  :IH1 - :IH3 :IREF common input processing                   */
/*   hx_lvl is 1 to 3 for :Ix and :IHx                                     */
/*         and 0      for :IREF                                            */
/***************************************************************************/
static  void    gml_ixxx_common( const gmltag * entry, int hx_lvl )
{
    bool          idseen;
    bool          refidseen;
    bool          seeseen;
    bool          seeidseen;
    bool          pgseen;
    bool          printseen;
    char      *   p;
    ereftyp       pgvalue;
    char      *   pgtext;
    size_t        pgtextlen;
    size_t        printtxtlen;
    size_t        seetextlen;
    char      *   printtxt;
    char      *   seetext;
    ref_entry *   refwork;

    ref_entry     reid;
    ref_entry *   rewk;

    ref_entry     refid;
    ref_entry *   refwk;

    ref_entry     reseeid;
    ref_entry *   rswk;

    ix_h_blk  *   ihm1;
    ix_e_blk  *   ixewk;
    ix_e_blk  *   ixewksav;
    ix_h_blk  *   ixhwk;
    ix_h_blk  * * ixhwork;
    uint32_t      wkpage;
    size_t        txtlen;
    char      *   txt;
    char          hxstring[TAG_NAME_LENGTH +1];
    char          lvlc;

    if( !GlobalFlags.index ) {          // index option not active
        scan_start = scan_stop + 1;     // ignore tag
        return;
    }
    lvlc = '0' + (char)hx_lvl;
    *hxstring = GML_char;         // construct tagname for possible error msg
    strcpy_s( (hxstring + 1), TAG_NAME_LENGTH, entry->tagname );

    if( (hxstring[2] == lvlc) &&        // :Ix tags not allowed before :GDOC
        !((ProcFlags.doc_sect >= doc_sect_gdoc) ||
        (ProcFlags.doc_sect_nxt >= doc_sect_gdoc)) ) {

        g_err( err_tag_before_gdoc, hxstring );
        err_count++;
        file_mac_info();
        scan_start = scan_stop + 1;
        return;
    }

    if( hx_lvl > 1 ) {          // test for missing previous parent index tag
        if( ixhtag[hx_lvl - 1] == NULL ) {
            g_err( err_parent_undef );
            err_count++;
            file_mac_info();
            scan_start = scan_stop + 1;
            return;
        }
    }

    idseen    = false;
    pgseen    = false;
    refidseen = false;
    printseen = false;
    seeseen   = false;
    seeidseen = false;

    ixewk     = NULL;
    pgvalue   = pgnone;
    pgtext    = NULL;
    pgtextlen = 0;
    printtxt  = NULL;
    printtxtlen = 0;
    seetext   = NULL;

    wkpage    = page + 1;
    p         = scan_start;
    ProcFlags.tag_end_found = false;

    /***********************************************************************/
    /*  Scan attributes  for :Ix  :IHx :IREF                               */
    /*                                                                     */
    /*  id=                                                                */
    /*  refid=                                                             */
    /*  pg=                                                                */
    /*  print=                                                             */
    /*  see=                                                               */
    /*  seeid=                                                             */
    /***********************************************************************/

    for( ;; ) {
        if( ProcFlags.tag_end_found ) {
            break;
        }
        while( is_space_tab_char( *p ) ) {
            p++;
        }
        if( *p == '.'  ) {
            ProcFlags.tag_end_found = true;
            break;
        }
        if( *p == '\0' ) {
            break;
        }

        /*******************************************************************/
        /*  ID='xxxxxxxx'   for :Ix :IHx                                   */
        /*******************************************************************/

        if( !strnicmp( "id", p, 2 ) ) {
            p += 2;
            p = get_refid_value( p );
            if( (val_start != NULL) && (val_len > 0) ) {
                if( hx_lvl > 0 ) {      // :Ix :IHx
                    idseen = true;      // id attribute found
                    init_ref_entry( &reid, val_start, val_len );
                    rewk = find_refid( iref_dict, reid.id );
                    if( rewk != NULL ) {
                        if( rewk->lineno != reid.lineno ) {
                            g_err( inf_id_duplicate );
                            err_count++;
                            file_mac_info();
                            scan_start = scan_stop + 1;
                            return;
                        }
                    }
                } else {                // not allowed for :IREF
                    g_err( err_refid_not_allowed, hxstring );
                    err_count++;
                    file_mac_info();
                    scan_start = scan_stop + 1;
                    return;
                }
            }
            scan_start = p;
            continue;
        }


        /*******************************************************************/
        /*  REFID='xxxxxxxx'  for :IREF :I2 :I3                            */
        /*******************************************************************/

        if( !strnicmp( "refid", p, 5 ) ) {
            p += 5;
            p = get_refid_value( p );
            if( val_start != NULL && val_len > 0 ) {
                if( (hx_lvl == 0) || ((hx_lvl > 1) && (hxstring[2] == lvlc)) ) {
                    fill_id( &refid, val_start, val_len );
                    refidseen = true;   // refid attribute found
                    refwk = find_refid( iref_dict, refid.id );
                    if( refwk == NULL ) {   // refid not in dict
                        if( GlobalFlags.lastpass ) {// this is an error
                            g_err( inf_id_unknown );// during lastpass
                            err_count++;
                            file_mac_info();
                            scan_start = scan_stop + 1;
                            return;
                        }
                    }
                } else {                // not allowed for :I1 and :IHx
                    g_err( err_refid_not_allowed, hxstring );
                    err_count++;
                    file_mac_info();
                    scan_start = scan_stop + 1;
                    return;
                }
            }
            scan_start = p;
            continue;
        }


        /*******************************************************************/
        /*  PG=          for :IREF :Ix                                     */
        /*******************************************************************/

        if( !strnicmp( "pg", p, 2 ) ) {
            p += 2;
            p = get_att_value( p );

            scan_start = p;
            if( val_start == NULL || val_len == 0 ) {   // no valid pg
                continue;               // ignore
            }

            if( quote_char == '\0' ) {  // value not quoted
                if( !strnicmp( "start", val_start, 5 ) ) {
                    pgvalue = pgstart;
                } else if( !strnicmp( "end", val_start, 3 ) ) {
                    pgvalue = pgend;
                } else if( !strnicmp( "major", val_start, 5 ) ) {
                    pgvalue = pgmajor;
                } else {
                    continue;           // ignore
                }
            } else {
                pgvalue = pgstring;
                pgtext = mem_alloc( val_len + 1 );
                strncpy( pgtext, val_start, val_len );// use text instead of pageno
                *(pgtext + val_len) = '\0';
                pgtextlen = val_len;
            }
            pgseen = true;
            continue;
        }


        /*******************************************************************/
        /*  PRINT=    for :IHx                                             */
        /*******************************************************************/

        if( !strnicmp( "print", p, 5 ) ) {
            p += 5;
            p = get_att_value( p );

            scan_start = p;
            if( val_start == NULL || val_len == 0 ) {
                continue;               // ignore
            }
            printtxt = mem_alloc( val_len + 1 );
            printtxtlen = val_len;
            strncpy( printtxt, val_start, val_len );
            *(printtxt + val_len) = '\0';
            printseen = true;
            continue;
        }


        /*******************************************************************/
        /*  SEE='xxxxxxxx'  for :IREF :IH1 :IH2                            */
        /*******************************************************************/

        if( !strnicmp( "see", p, 3 ) ) {

            p += 3;
            p = get_att_value( p );

            scan_start = p;
            if( (val_start != NULL) || val_len > 0 ) {
                if( hx_lvl == 0  ||
                    ((hx_lvl < 3) && (hxstring[3] == lvlc)) ) {// :IREF :IH1 :IH2
                    seetext = mem_alloc( val_len +1 );
                    strncpy( seetext, val_start, val_len );
                    *(seetext + val_len) = '\0';
                    seetextlen = val_len;
                    seeseen = true;
                } else {                // not allowed for :IH3, :Ix
                    g_err( err_refid_not_allowed, hxstring );
                    err_count++;
                    file_mac_info();
                    scan_start = scan_stop + 1;
                    return;
                }
            }
            continue;
        }


        /*******************************************************************/
        /*  SEEID='xxxxxxxx'  for :IREF :IH1 :IH2                          */
        /*******************************************************************/

        if( !strnicmp( "seeid", p, 5 ) ) {
            p += 5;
            p = get_refid_value( p );
            if( (val_start != NULL) && (val_len > 0) ) {
                if( (hx_lvl <= 3) && (hxstring[3] == lvlc) ) {
                    seeidseen = true;
                    fill_id( &reseeid, val_start, val_len );// copy lower id
                    rswk = find_refid( iref_dict, reseeid.id );
                    if( rswk == NULL ) {// not in dict, this is an error
                        if( GlobalFlags.lastpass ) {  // during lastpass
                            g_err( inf_id_unknown );
                            err_count++;
                            file_mac_info();
                            scan_start = scan_stop + 1;
                            return;
                        }
                    }
                } else {                // not allowed for :IH3, :Ix :IREF
                    g_err( err_refid_not_allowed, hxstring );
                    err_count++;
                    file_mac_info();
                    scan_start = scan_stop + 1;
                    return;
                }
            }
            scan_start = p;
            continue;
        }


        /*******************************************************************/
        /* no more valid attributes                                        */
        /*******************************************************************/
        break;
    }

    if( ProcFlags.tag_end_found ) {     // tag end ?
        p++;
        if( hx_lvl > 0 ) {              // we need a text line for :Ix :IHx
            if( !*p ) {
                get_line( true );
                p = buff2;
            }
        }
    }


    /***********************************************************************/
    /* process the found attributes and the text line                      */
    /***********************************************************************/
    txt = p;
    txtlen = strlen( p );

    if( !pgseen ) {
        pgvalue = pgpageno;             // set default
    }

    if( hx_lvl == 0 ) {                 // :IREF tag

    /***********************************************************************/
    /* processing for :IREF                                                */
    /***********************************************************************/

        if( !refidseen ) {              // refid= missing
            g_err( err_att_missing );
            err_count++;
            file_mac_info();
            scan_start = scan_stop + 1;
            return;
        }
        if( GlobalFlags.lastpass ) {
            if( refidseen && (refwk != NULL) ) {
                ixhwk = refwk->u.refb.hblk;
            } else {
                ixhwk = ixhtag[hx_lvl];
            }

            /***************************************************************/
            /* create index entry with page no / text                      */
            /***************************************************************/
            if( ixhwk->entry == NULL ) {    // first entry
                ixewk = fill_ix_e_blk( &(ixhwk->entry), ixhwk, pgvalue, pgtext, pgtextlen );
            } else {
                if( pgvalue == pgmajor ) {  // major becomes first in chain
                    ixewksav = ixhwk->entry;
                    ixhwk->entry = NULL;
                    ixewk = fill_ix_e_blk( &(ixhwk->entry), ixhwk, pgvalue,
                                           pgtext, pgtextlen );
                    ixewk->next = ixewksav;
                } else {
                    ixewk = ixhwk->entry;
                    if( pgvalue < pgstring ) {  // pageno variants
                        if( ixewk->entry_typ < pgstring ) {
                            while( ixewk->next != NULL ) {// insert before pgstring
                                if( ixewk->next->entry_typ >= pgstring ) {
                                    break;
                                }
                                ixewk = ixewk->next;
                            }
                        } else {
                            ixewksav = ixhwk->entry;
                            ixhwk->entry = NULL;
                            ixewk = fill_ix_e_blk( &(ixhwk->entry), ixhwk,
                                                   pgvalue, pgtext, pgtextlen );
                            ixewk->next = ixewksav;
                        }
                        if( ixewk->u.page_no != wkpage ) {
                            ixewksav = ixewk->next;
                            ixewk->next = NULL;
                            ixewk = fill_ix_e_blk( &(ixewk->next), ixhwk, pgvalue,
                                                   pgtext, pgtextlen );
                            ixewk->next = ixewksav;
                        }
                    } else {
                        while( ixewk->next != NULL ) {  // find last entry
                            ixewk = ixewk->next;
                        }
                        ixewk = fill_ix_e_blk( &(ixewk->next), ixhwk, pgvalue,
                                               pgtext, pgtextlen );
                    }
                }
            }
        }
    } else                              // :Ix :IHx tags

    if( ((hxstring[2] == lvlc) ) ) {    // test for :Ix

    /***********************************************************************/
    /* processing for :Ix                                                  */
    /***********************************************************************/

        switch( hx_lvl ) {              // processing for :I1 :I2 :I3
        case 1 :
            ixhwork = &index_dict;
            ixhwk = find_create_ix_h_entry( ixhwork, &printtxt, printtxtlen,
                                            txt, txtlen, hx_lvl );
            printtxt = NULL;
            ixhtag[hx_lvl] = ixhwk;
            break;
        case 2 :
        case 3 :
            ihm1 = ixhtag[hx_lvl - 1];
            if( refidseen && (refwk != NULL) ) {
                if( hx_lvl > refwk->u.refb.hblk->ix_lvl ) {
                    ixhwork = &(refwk->u.refb.hblk->lower);
                } else {
                    ixhwork = &(refwk->u.refb.hblk);
                }
            } else {
                ixhwork = &(ixhtag[hx_lvl - 1]->lower);
            }
            ixhwk = find_create_ix_h_entry( ixhwork, &printtxt, printtxtlen,
                                            txt, txtlen, hx_lvl );
            printtxt = NULL;
            if( !refidseen ) {
                ixhtag[hx_lvl] = ixhwk;
            }
            break;
        default:
            break;
        }

        /*******************************************************************/
        /* create index entry with page no / text                          */
        /*******************************************************************/
        if( ixhwk->entry == NULL ) {    // first entry
            ixewk = fill_ix_e_blk( &(ixhwk->entry), ixhwk, pgvalue, pgtext,
                                   pgtextlen );
        } else {
            if( pgvalue == pgmajor ) {  // major becomes first in chain
                ixewksav = ixhwk->entry;
                ixhwk->entry = NULL;
                ixewk = fill_ix_e_blk( &(ixhwk->entry), ixhwk, pgvalue, pgtext,
                                       pgtextlen );
                ixewk->next = ixewksav;
            } else {
                ixewk = ixhwk->entry;
                if( pgvalue < pgstring ) {  // pageno variants
                    if( ixewk->entry_typ < pgstring ) {
                        while( ixewk->next != NULL ) {// insert before pgstring
                            if( ixewk->next->entry_typ >= pgstring ) {
                                break;
                            }
                            ixewk = ixewk->next;
                        }
                    } else {
                        ixewksav = ixhwk->entry;
                        ixhwk->entry = NULL;
                        ixewk = fill_ix_e_blk( &(ixhwk->entry), ixhwk,
                                               pgvalue, pgtext, pgtextlen );
                        ixewk->next = ixewksav;
                    }
                    if( ixewk->u.page_no != wkpage ) {
                        ixewksav = ixewk->next;
                        ixewk->next = NULL;
                        ixewk = fill_ix_e_blk( &(ixewk->next), ixhwk, pgvalue,
                                               pgtext, pgtextlen );
                        ixewk->next = ixewksav;
                    }
                } else {
                    while( ixewk->next != NULL ) {  // find last entry
                        ixewk = ixewk->next;
                    }
                    ixewk = fill_ix_e_blk( &(ixewk->next), ixhwk, pgvalue, pgtext,
                                           pgtextlen );
                }
            }
        }
    } else                              // :IHx

    if( ((hxstring[3] == lvlc) ) ) {    // test for :IHx

    /***********************************************************************/
    /* processing for :IHx                                                 */
    /***********************************************************************/

        switch( hx_lvl ) {              // processing for :IH1 :IH2 :IH3
        case 1 :
            ixhwork = &index_dict;
            ixhwk = find_create_ix_h_entry( ixhwork, &printtxt, printtxtlen,
                                            txt, txtlen, hx_lvl );
            printtxt = NULL;
            ixhtag[hx_lvl] = ixhwk;
            break;
        case 2 :
        case 3 :
            ihm1 = ixhtag[hx_lvl - 1];
            ixhwork = &(ixhtag[hx_lvl - 1]->lower);
            ixhwk = find_create_ix_h_entry( ixhwork, &printtxt, printtxtlen,
                                            txt, txtlen, hx_lvl );
            printtxt = NULL;
            ixhtag[hx_lvl] = ixhwk;
            break;
        default:
            break;
        }
        if( seeseen ) {
            pgvalue = pgsee;
            if( ixhwk->entry == NULL ) {
                ixewk = fill_ix_e_blk( &(ixhwk->entry), ixhwk, pgvalue,
                                       seetext, seetextlen );
            } else {
                ixewk = ixhwk->entry;
                while( ixewk->next != NULL ) {  // find last entry
                    ixewk = ixewk->next;
                }
                ixewk = fill_ix_e_blk( &(ixewk->next), ixhwk, pgvalue,
                                       seetext, seetextlen );
            }
        } else {
            if( seeidseen ) {
                ix_e_blk  * * anchor;

                pgvalue = pgsee;
                if( ixhwk->entry == NULL ) {
                    anchor = &(ixhwk->entry);
                } else {
                    ixewk = ixhwk->entry;
                    while( ixewk->next != NULL ) {  // find last entry
                        ixewk = ixewk->next;
                    }
                    anchor = &(ixewk->next);
                }
                if( rswk->u.refb.hblk->prt_term != NULL ) {
                    ixewk = fill_ix_e_blk( anchor, ixhwk,
                                           pgvalue, rswk->u.refb.hblk->prt_term,
                                           rswk->u.refb.hblk->prt_term_len );
                } else {
                    ixewk = fill_ix_e_blk( anchor, ixhwk,
                                           pgvalue, rswk->u.refb.hblk->ix_term,
                                           rswk->u.refb.hblk->ix_term_len );
                }
            }
        }
    }
    if( idseen ) {                 // ID specified create reference entry
        reid.u.refb.hblk = ixhwk;
        reid.u.refb.eblk = ixewk;
        reid.flags = rf_ix;
        refwork = mem_alloc( sizeof( reid ) );
        memcpy( refwork, &reid, sizeof( reid ) );
        add_ref_entry( &iref_dict, refwork );
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
    scan_start = scan_stop + 1;
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

