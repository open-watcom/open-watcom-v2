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
 
#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */
 
#include "wgml.h"
#include "gvars.h"
 
 
    banner_lay_tag  *   curr_ban;       // also needed for glbanreg.c
    banner_lay_tag  *   del_ban;        // ... banner to be deleted
 
 
/***************************************************************************/
/*   :BANNER    attributes                                                 */
/***************************************************************************/
const   lay_att     banner_att[8] =
    { e_left_adjust, e_right_adjust, e_depth, e_place, e_refplace,
      e_docsect, e_refdoc, e_dummy_zero };
 
static  int             att_count = sizeof( banner_att );
static  int             count[sizeof( banner_att )];
static  int             sum_count;
static  banner_lay_tag  wk;          // for temp storage of banner attributes
static  bf_place        refplace;
static  ban_docsect     refdoc;
static  bool            banner_end_prepared = false;
static  bool            banner_delete_req   = false;
 
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
    char    *   p;
    char        z0[2] = "0";
    int         k;
 
    ban->next = NULL;
    ban->region = NULL;
    ban->top_line = NULL;
    ban->ban_left_adjust = 0;
    ban->ban_right_adjust = 0;
    ban->ban_depth = 0;
 
    p = z0;
    to_internal_SU( &p, &(ban->left_adjust) );
    p = z0;
    to_internal_SU( &p, &(ban->right_adjust) );
    p = z0;
    to_internal_SU( &p, &(ban->depth) );
    ban->place = no_place;
    ban->docsect = no_ban;
 
    refplace = no_place;
    refdoc = no_ban;
    curr_ban = NULL;
    prev_ban = NULL;
    ref_ban = NULL;
    del_ban = NULL;
 
    for( k = 0; k < att_count; k++ ) {
        count[k] = 0;
    }
    sum_count = 0;
    banner_end_prepared = false;
    banner_delete_req   = false;
}
 
 
/***************************************************************************/
/*  lay_banner                                                             */
/***************************************************************************/
 
void    lay_banner( const gmltag * entry )
{
    char        *   p;
    condcode        cc;
    int             k;
    lay_att         curr;
    att_args        l_args;
    bool            cvterr;
 
    p = scan_start;
    cvterr = false;
 
    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.lay_xxx != el_banner ) {
        ProcFlags.lay_xxx = el_banner;
        ProcFlags.banner = true;
        init_banner_wk( &wk );
    } else {
        if( !strnicmp( ":banner", buff2, sizeof( ":banner" ) ) ) {
            err_count++;                // nested :banner
            g_err( err_nested_tag, entry->tagname );
            file_mac_info();
 
            while( !ProcFlags.reprocess_line  ) {
                eat_lay_sub_tag();
                if( strnicmp( ":ebanner", buff2, sizeof( ":ebanner" ) ) ) {
                    ProcFlags.reprocess_line = false;  // not :ebanner, go on
                }
            }
            return;
        }
    }
    cc = get_lay_sub_and_value( &l_args );  // get att with value
    while( cc == pos ) {
        cvterr = true;
        for( k = 0; k < att_count; k++ ) {
            curr = banner_att[k];
 
            if( !strnicmp( att_names[curr], l_args.start[0], l_args.len[0] ) ) {
                p = l_args.start[1];
 
                if( count[k] ) {
                    cvterr = 1;         // attribute specified twice
                } else {
                    count[k] += 1;
                    sum_count++;
                    switch( curr ) {
                    case   e_left_adjust:
                        cvterr = i_space_unit( p, curr, &wk.left_adjust );
                        break;
                    case   e_right_adjust:
                        cvterr = i_space_unit( p, curr, &wk.right_adjust );
                        break;
                    case   e_depth:
                        cvterr = i_space_unit( p, curr, &wk.depth );
                        break;
                    case   e_place:
                        cvterr = i_place( p, curr, &wk.place );
                        break;
                    case   e_docsect:
                        cvterr = i_docsect( p, curr, &wk.docsect );
                        break;
                    case   e_refplace:  // not stored in banner struct
                        cvterr = i_place( p, curr, &refplace );
                        break;
                    case   e_refdoc:    // not stored in banner struct
                        cvterr = i_docsect( p, curr, &refdoc );
                        break;
                    default:
                        out_msg( "WGML logic error.\n");
                        cvterr = true;
                        break;
                    }
                }
                break;                  // break out of for loop
            }
        }
        if( cvterr ) {                  // there was an error
            err_count++;
            g_err( err_att_val_inv );
            file_mac_info();
        }
        cc = get_lay_sub_and_value( &l_args );  // get att with value
    }
    scan_start = scan_stop + 1;
    return;
}
 
/***************************************************************************/
/*  banner end processing (different from :eBANNER processing)             */
/*  triggered by next tag following :BANNER tag                            */
/***************************************************************************/
void    lay_banner_end_prepare( void )
{
    banner_lay_tag  *   banwk;
    region_lay_tag  *   regwknew;
    region_lay_tag  *   regwknew2;
    region_lay_tag  *   regwkold;
    int                 k;
 
 
    if( banner_end_prepared ) {
        return;                         // once is enough
    }
    banner_end_prepared = true;
 
    curr_ban = &wk;
    banwk = layout_work.banner;
    ref_ban = NULL;
    if( (refdoc != no_ban) && (refplace != no_place) ) {// search banner
        while( banwk != NULL ) {
            if( (banwk->place == refplace) && (banwk->docsect == refdoc) ) {
                ref_ban = banwk;
                break;
            } else {
                banwk = banwk->next;
            }
        }
        if( ref_ban != NULL ) {         // copy from referenced banner
            for( k = 0; k < att_count; ++k ) {
                if( count[k] == 0) {    // copy only unchanged values
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
        }
    }
    del_ban = NULL;
    if( (sum_count == 2) && (wk.place != no_place) && (wk.docsect != no_ban) ) {
                                        // banner delete request
        for( banwk = layout_work.banner;  banwk != NULL; banwk = banwk->next ) {
            if( (banwk->place == wk.place) && (banwk->docsect == wk.docsect) ) {
                del_ban = banwk;        // found banner to delete
                break;
            } else {
                prev_ban = banwk;
            }
        }
        banner_delete_req = true;     // remember delete request
    }
    if( !banner_delete_req ) {          // no delete request
        if( (ref_ban == NULL && sum_count != 5) ||  // not all atts specified
            (wk.place == no_place || wk.docsect == no_ban) ) {
            err_count++;
            g_err( err_att_missing );
            file_mac_info();
        }
    }
}
 
 
/***************************************************************************/
/*  lay_ebanner                                                            */
/***************************************************************************/
 
void    lay_ebanner( const gmltag * entry )
{
    banner_lay_tag  *   banwk;
    region_lay_tag  *   reg;
 
    ProcFlags.lay_xxx = el_zero;        // banner no longer active
 
    if( !GlobalFlags.firstpass ) {
        scan_start = scan_stop + 1;
        eat_lay_sub_tag();
        return;                         // process during first pass only
    }
    if( ProcFlags.banner ) {            // are we inside banner
        ProcFlags.banner = false;
 
        lay_banner_end_prepare();       // if not yet done
        if( banner_delete_req ) {       // delete request
            /* While the documentation requires the banner regions to be
             * deleted first, wgml 4.0 does not. It will delete a banner even
             * though the banner regions still exist.
             */
            if( del_ban != NULL ) {
 
                while( del_ban->region != NULL) {
                    reg = del_ban->region;
                    del_ban->region = del_ban->region->next;
                    mem_free( reg );
                    reg = NULL;
                }
 
                if( prev_ban != NULL ) {
                    prev_ban->next = del_ban->next;
                } else {
                    layout_work.banner = del_ban->next; // delete 1st banner
                }
                mem_free( del_ban );
                del_ban = NULL;
            }
        } else {
            banwk = mem_alloc( sizeof( banner_lay_tag ) );
            memcpy( banwk, curr_ban, sizeof( banner_lay_tag ) );
 
/***************************************************************************/
/*  Adding banner to the existing banners either as first or last in chain */
/*  Both give different order than wgml 4.0                                */
/*  This can be seen by :CONVERT output, sooooooo ...                 TBD  */
/*  If it really matters, more research has to be done                     */
/***************************************************************************/
#if 1
            if( layout_work.banner == NULL ) {
                layout_work.banner = banwk;
            } else {
                curr_ban = layout_work.banner;
                while( curr_ban->next != NULL ) {   // add at end of chain
                    curr_ban = curr_ban->next;
                }
                curr_ban->next = banwk;
            }
#else
            banwk->next = layout_work.banner;   // add as first element
            layout_work.banner = banwk;
#endif
            curr_ban = NULL;
        }
    } else {
        g_err( err_no_lay, &(entry->tagname[1]), entry->tagname );
        err_count++;
        file_mac_info();
    }
    scan_start = scan_stop + 1;
    return;
}
