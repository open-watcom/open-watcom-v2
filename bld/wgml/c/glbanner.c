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
* Description: WGML implement :BANNER :eBANNER  tags for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


banner_lay_tag  *   curr_ban;       // also needed for glbanreg.c
banner_lay_tag  *   del_ban;        // ... banner to be deleted


/***************************************************************************/
/*   :BANNER    attributes                                                 */
/***************************************************************************/
const   lay_att     banner_att[8] =
    { e_left_adjust, e_right_adjust, e_depth, e_place, e_refplace,
      e_docsect, e_refdoc, e_dummy_zero };

static  int             att_count = sizeof( banner_att ) - 1;   // omit e_dummy_zero from count
static  bool            count[sizeof( banner_att ) - 1];
static  int             sum_count;
static  banner_lay_tag  wk;          // for temp storage of banner attributes
static  bf_place        refplace;
static  ban_docsect     refdoc;

static  banner_lay_tag  *   prev_ban;
static  banner_lay_tag  *   ref_ban;    // referenced banner for copy values

/**********************************************************************************/
/* Defines a page banner.  A page banner appears at the top and/or bottom         */
/* of a page.  Information such as page numbers, running titles and the           */
/* current heading would be defined in a banner.  Banners may be defined          */
/* for the top and/or bottom of a page in each section of the document.           */
/* The banner attributes specify the size of the banner and the document          */
/* section in which it is to be used.  A banner definition begins with the        */
/* :banner tag and ends with the :ebanner tag.  The banner is divided into        */
/* a number of regions, each defined by the :banregion tag.  The banner           */
/* region definitions are placed after the banner attributes and before           */
/* the :ebanner tag.                                                              */
/*                                                                                */
/*                                                                                */
/*:BANNER                                                                         */
/*        left_adjust = 0                                                         */
/*        right_adjust = 0                                                        */
/*        depth = 3                                                               */
/*        place = bottom                                                          */
/*        refplace = bottom                                                       */
/*        docsect = head0                                                         */
/*        refdoc = body                                                           */
/*                                                                                */
/*left_adjust The left_adjust attribute accepts any valid horizontal space unit.  */
/*The left margin is set to the page left margin plus the specified left          */
/*adjustment.                                                                     */
/*                                                                                */
/*right_adjust The right_adjust attribute accepts any valid horizontal space unit.*/
/*The right margin is set to the page right margin minus the specified            */
/*right adjustment.                                                               */
/*                                                                                */
/*depth The depth attribute accepts as its value any valid vertical space unit.   */
/*It specifies the vertical depth of the banner.                                  */
/*                                                                                */
/*place The place attribute specifies where on the odd or even numbered           */
/*output page the banner is to be placed. The following values may                */
/*be specified for this attribute:                                                */
/*                                                                                */
/*    topodd      The top of odd pages.                                           */
/*    topeven     The top of even pages.                                          */
/*    botodd      The bottom of odd pages.                                        */
/*    boteven     The bottom of even pages.                                       */
/*                                                                                */
/*refplace The refplace attribute specifies the place value of an existing        */
/*banner.                                                                         */
/*                                                                                */
/*docsect The document section for which the banner will be used. The             */
/*following values may be specified for this attribute:                           */
/*    abstract    The banner will appear in the abstract section                  */
/*                of the document.                                                */
/*    appendix    The banner will appear in the appendix section                  */
/*                of the document.                                                */
/*    backm       The banner will appear in the back material                     */
/*                section of the document.                                        */
/*    body        The banner will appear in the body section of                   */
/*                the document.                                                   */
/*    figlist     The banner will appear in the figure list                       */
/*                section of the document.                                        */
/*    HEADn       The banner will appear when a heading of                        */
/*                level n, where n may have a value of zero                       */
/*                through six inclusive, appears on the output page.              */
/*    letfirst    The banner will appear on the first page of the                 */
/*                letter when the letter format is used. If the                   */
/*                letter has only one page, only the banner                       */
/*                defined for the top of the page will be used.                   */
/*                Even page banners are not allowed if letfirst is                */
/*                the document section value.                                     */
/*    letlast     The banner will appear on the last page of the                  */
/*                letter when the letter format is used. If the                   */
/*                letter has only one page, only the banner                       */
/*                defined for the bottom of the page will be                      */
/*                used.                                                           */
/*    letter      The banner will appear on the pages between                     */
/*                the first and last page of the letter when the                  */
/*                letter format is used.                                          */
/*    index       The banner will appear in the index section of                  */
/*                the document.                                                   */
/*    preface     The banner will appear in the preface section                   */
/*                of the document.                                                */
/*    toc         The banner will appear in the table of contents                 */
/*                section of the document.                                        */
/*                                                                                */
/*refdoc The refdoc attribute specifies the docsect value of an existing          */
/*banner.                                                                         */
/* The refplace and refdoc attributes are used in combination to specify          */
/* an existing banner.  The referenced banner is copied to the banner             */
/* being defined.  These attributes are most commonly used when                   */
/* duplicating a banner for an odd or even page.  When these attributes           */
/* are specified, only the place and docsect attributes are required.  All        */
/* other attributes will override the attribute values of the banner being        */
/* copied.  If the two reference attributes are not specified, all of the         */
/* other attributes are required.  To delete a banner, specify only the           */
/* place and docsect attributes, and delete the individual banner regions.        */
/*                                                                                */
/**********************************************************************************/

/***************************************************************************/
/*Mark the end of a banner definition.                                     */
/*                                                                         */
/*:eBANNER                                                                 */
/*                                                                         */
/***************************************************************************/


/***************************************************************************/
/*  init banner with no values                                             */
/***************************************************************************/

static  void    init_banner_wk( banner_lay_tag * ban )
{
    char        z0[2] = "0";
    int         k;

    ban->next = NULL;
    ban->region = NULL;
    ban->by_line = NULL;
    ban->ban_left_adjust = 0;
    ban->ban_right_adjust = 0;
    ban->ban_depth = 0;
    ban->next_refnum = 1;
    ban->style = no_content;

    lay_init_su( z0, &(ban->left_adjust) );
    lay_init_su( z0, &(ban->right_adjust) );
    lay_init_su( z0, &(ban->depth) );
    ban->place = no_place;
    ban->docsect = no_ban;

    refplace = no_place;
    refdoc = no_ban;
    curr_ban = NULL;
    prev_ban = NULL;
    ref_ban = NULL;
    del_ban = NULL;

    for( k = 0; k < att_count; k++ ) {
        count[k] = false;
    }
    sum_count = 0;
}


/***************************************************************************/
/*  lay_banner                                                             */
/***************************************************************************/

void    lay_banner( const gmltag * entry )
{
    banner_lay_tag  *   banwk;
    bf_place            match_place;
    bf_place            new_place;
    char            *   p;
    condcode            cc;
    int                 k;
    lay_att             curr;
    region_lay_tag  *   regwknew;
    region_lay_tag  *   regwknew2;
    region_lay_tag  *   regwkold;

    (void)entry;

    p = scan_start;
    rs_loc = banner_tag;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != el_banner ) {
        ProcFlags.lay_xxx = el_banner;
        ProcFlags.banner = true;
        init_banner_wk( &wk );
    }
    cc = get_attr_and_value();            // get att with value
    while( cc == pos ) {
        for( k = 0; k < att_count; k++ ) {
            curr = banner_att[k];

            if( strnicmp( att_names[curr], g_att_val.att_name, g_att_val.att_len ) == 0 ) {
                p = g_att_val.val_name;

                if( count[k] ) {
                    if( sum_count == att_count ) {  // all attributes found
                        xx_err( err_lay_text );     // per wgml 4.0: treat as text
                    } else {
                        xx_err( err_att_dup );      // per wgml 4.0: treat as duplicated attribute
                    }
                } else {
                    count[k] = true;
                    sum_count++;
                    switch( curr ) {
                    case e_left_adjust:
                        if( AttrFlags.left_adjust ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        i_space_unit( p, curr, &wk.left_adjust );
                        AttrFlags.left_adjust = true;
                        break;
                    case e_right_adjust:
                        if( AttrFlags.right_adjust ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        i_space_unit( p, curr, &wk.right_adjust );
                        AttrFlags.right_adjust = true;
                        break;
                    case e_depth:
                        if( AttrFlags.depth ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        i_space_unit( p, curr, &wk.depth );
                        AttrFlags.depth = true;
                        break;
                    case e_place:
                        if( AttrFlags.place ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        i_place( p, curr, &wk.place );
                        AttrFlags.place = true;
                        break;
                    case e_docsect:
                        if( AttrFlags.docsect ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        i_docsect( p, curr, &wk.docsect );
                        AttrFlags.docsect = true;
                        break;
                    case e_refplace:  // not stored in banner struct
                        if( AttrFlags.refplace ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        i_place( p, curr, &refplace );
                        AttrFlags.refplace = true;
                        break;
                    case e_refdoc:    // not stored in banner struct
                        if( AttrFlags.refdoc ) {
                            xx_line_err_ci( err_att_dup, g_att_val.att_name,
                                g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                        }
                        i_docsect( p, curr, &refdoc );
                        AttrFlags.refdoc = true;
                        break;
                    default:
                        internal_err( __FILE__, __LINE__ );
                    }
                }
                break;                  // break out of for loop
            }
        }
        cc = get_attr_and_value();            // get att with value
    }

    /*******************************************************/
    /* At this point, end-of-tag has been reached and all  */
    /* attributes provided have been found and processed.  */
    /* First ensure the required attributes are present.   */
    /*******************************************************/

    if( (wk.place == no_place) || (wk.docsect == no_ban) ) {    // both must be specified
        xx_err( err_att_missing );
    }

    /*******************************************************/
    /* Process a reference banner.                         */
    /* When completed, wk will have had any missing        */
    /* attribute values copied from the reference banner   */
    /* and sum_count is set to 5, as if all attributes     */
    /* had been specified.                                 */
    /*******************************************************/

    if( (refdoc != no_ban) || (refplace != no_place) ) {    // at least one was used
        if( ((refdoc == no_ban) && (refplace != no_place)) ||
                ((refdoc != no_ban) && (refplace == no_place)) ) {
            xx_err( err_both_refs );                        // both are required if either is used
        } else if( (refdoc == wk.docsect) && (refplace == wk.place) ) { // can't reference current banner
            xx_err( err_self_ref );
        } else if( (refdoc != no_ban) && (refplace != no_place) ) { // find referenced banner
            banwk = layout_work.banner;
            ref_ban = NULL;
            while( banwk != NULL ) {
                if( (banwk->place == refplace) && (banwk->docsect == refdoc) ) {
                    ref_ban = banwk;
                    break;
                } else {
                    banwk = banwk->next;
                }
            }
            if( ref_ban == NULL ) {                 // referenced banner not found
                xx_err( err_illegal_ban_ref );
            } else {                                // copy from referenced banner
                for( k = 0; k < att_count; ++k ) {
                    if( !count[k] ) {               // copy only unchanged values
                        count[k] = 1;               // treat as new value
                        switch( banner_att[k] ) {
                        case   e_left_adjust:
                            memcpy( &(wk.left_adjust), &(ref_ban->left_adjust),
                                    sizeof( wk.left_adjust ) );
                            break;
                        case   e_right_adjust:
                            memcpy( &(wk.right_adjust), &(ref_ban->right_adjust),
                                    sizeof( wk.right_adjust ) );
                            break;
                        case   e_depth:
                            memcpy( &(wk.depth), &(ref_ban->depth),
                                    sizeof( wk.depth ) );
                            break;
                        default:            // refdoc and refplace are not stored
                            break;          // docsect and place must be specified
                        }
                    }
                }
                // copy banregions too
                wk.next_refnum = ref_ban->next_refnum;
                regwkold = ref_ban->region;
                while( regwkold != NULL ) { // allocate + copy banregions
                    regwknew = mem_alloc( sizeof( region_lay_tag ) );
                    memcpy( regwknew, regwkold, sizeof( region_lay_tag ) );
                    if( wk.region == NULL ) {   // forward chain
                        wk.region = regwknew;
                    } else {
                        regwknew2->next = regwknew;
                    }
                    regwknew2 = regwknew;
                    regwkold = regwkold->next;
                }
                sum_count = 5;                  // process as if all attributes for new banner found
            }
        }
    }

    /*******************************************************/
    /* Locate and detach the current banner, if it exists. */
    /* If only refsect and place were given, assign it to  */
    /* del_ban; otherwise, assign it to curr_ban.          */
    /*******************************************************/

    for( banwk = layout_work.banner; banwk != NULL; banwk = banwk->next ) {
        if( (banwk->place == wk.place) && (banwk->docsect == wk.docsect) ) {
            for( k = 0; k < att_count; ++k ) {  // update banwk
                if( count[k] ) {                // copy only changed values
                    switch( banner_att[k] ) {
                    case   e_left_adjust:
                        memcpy( &(banwk->left_adjust), &(wk.left_adjust), sizeof( wk.left_adjust ) );
                        break;
                    case   e_right_adjust:
                        memcpy( &(banwk->right_adjust), &(wk.right_adjust), sizeof( wk.right_adjust ) );
                        break;
                    case   e_depth:
                        memcpy( &(banwk->depth), &(wk.depth), sizeof( wk.depth ) );
                        break;
                    default:                // refdoc and refplace are not stored
                        break;              // docsect and place must be specified
                    }
                }
            }
            if( wk.region != NULL ) {

                /*******************************************************/
                /* This is a copy of a reference banner.               */
                /* The found banner's regions must be deleted and this */
                /* banners regions moved over.                         */
                /*******************************************************/

                banwk->next_refnum = wk.next_refnum;
                regwkold = NULL;
                regwknew = banwk->region;
                while( regwknew != NULL ) { // allocate + copy banregions
                    regwkold = regwknew;
                    regwknew = regwknew->next;
                    mem_free( regwkold );
                }
                banwk->region = wk.region;
                wk.region = NULL;
                wk.next_refnum = 1;
            }
            if( prev_ban == NULL ) {                // first banner
                layout_work.banner = banwk->next;   // detach banner
            } else {
                prev_ban->next = banwk->next;       // detach banner
            }
            banwk->next = NULL;
            if( (sum_count == 2) && (wk.place != no_place) && (wk.docsect != no_ban) ) {
                del_ban = banwk;                    // mark banner for possible deletion
            } else {
                curr_ban = banwk;                   // mark banner for possible update
            }
            break;
        } else {
            prev_ban = banwk;
        }
    }
    if( (curr_ban == NULL) && (del_ban == NULL) ) { // not found: new banner definition
        if( sum_count != 5 ) {              // now we need all 5 non-ref attributes
            xx_err( err_all_ban_att_rqrd );
        } else {
            curr_ban = mem_alloc( sizeof( banner_lay_tag ) );
            memcpy( curr_ban, &wk, sizeof( banner_lay_tag ) );

            if( layout_work.banner == NULL ) {      // First banner initializes the list
                layout_work.banner = curr_ban;
            } else {

                /************************************************************************/
                /*  New banners can affect existing banners in the manner shown.        */
                /*  The effect is that, if topeven or topodd exists, top itself cannot, */
                /*  and similarly for bottom.                                           */
                /************************************************************************/

                match_place = no_place;
                new_place = no_place;
                switch( curr_ban->place ) {
                case boteven_place :
                    match_place = bottom_place;
                    new_place = botodd_place;
                    break;
                case botodd_place :
                    match_place = bottom_place;
                    new_place = boteven_place;
                    break;
                case topeven_place :
                    match_place = top_place;
                    new_place = topodd_place;
                    break;
                case topodd_place :
                    match_place = top_place;
                    new_place = topeven_place;
                    break;
                }
            }

            if( match_place != no_place ) {         // prevban used to preserve curr_ban value
                prev_ban = layout_work.banner;
                while( prev_ban->next != NULL ) {   // change the place, if found
                    if( (prev_ban->docsect == curr_ban->docsect) && (prev_ban->place == match_place) ) {
                        prev_ban->place = new_place;
                        break;
                    }
                    prev_ban = prev_ban->next;
                }
            }
        }
    }
    scan_start = scan_stop + 1;
    return;
}


/***************************************************************************/
/*  lay_ebanner                                                            */
/***************************************************************************/

void    lay_ebanner( const gmltag * entry )
{
    banner_lay_tag  *   banwk;
    region_lay_tag  *   reg;

    (void)entry;

    ProcFlags.lay_xxx = el_zero;        // banner no longer active
    rs_loc = 0;

    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.banner ) {            // are we inside banner
        ProcFlags.banner = false;

        /************************************************************************/
        /*  If no BANREGIONs were encountered, then a NULL curr_ban implies     */
        /*  that del_ban was set in lay_banner and no BANREGIONs were           */
        /*  encountered: the banner and all regions are to be deleted.          */
        /*  If any BANREGIONs were encountered, del_ban will be NULL and        */
        /*  curr_ban will not. In this case, if there are no regions, they are  */
        /*  flipped and the banner will be deleted.                             */
        /*  NOTE: at this point, the banner has been detached from the list     */
        /*  so deleting it amounts to releasing the memory and not appending it */
        /*  to the list.                                                        */
        /************************************************************************/

        if( (curr_ban != NULL) && (curr_ban->region == NULL) ) {  // flip if has no regions
            del_ban = curr_ban;
            curr_ban = NULL;
        }

        if( del_ban != NULL) {              // delete request
            while( del_ban->region != NULL) {
                reg = del_ban->region;
                del_ban->region = del_ban->region->next;
                mem_free( reg );
                reg = NULL;
            }

            mem_free( del_ban );
            del_ban = NULL;
        } else if( curr_ban != NULL) {      // append survivor to the end of the list

            /************************************************************************/
            /*  Surviving banners are added to the bottom of the list.              */
            /*  However, wgml 4.0 does something else in some circumstances         */
            /*  This code is focused on having our wgml end up with the same set of */
            /*  banners as wgml 4.0 does, even if CONVERT does not output them in   */
            /*  the same order for our wgml as it does for wgml 4.0.                */
            /************************************************************************/

            banwk = layout_work.banner;
            while( banwk->next != NULL ) {  // add at end of chain
                banwk = banwk->next;
            }
            banwk->next = curr_ban;
            curr_ban = NULL;
        }
    } else {
        xx_err_c( err_tag_expected, "BANNER" );
    }
    scan_start = scan_stop + 1;
    return;
}
