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
* Description: WGML implement :BANNER :eBANNER  tags for LAYOUT processing
*              and :BANREGION :eBANREGION  tags for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :BANNER    attributes                                                 */
/***************************************************************************/
static const lay_att    banner_att[] = {
    e_left_adjust, e_right_adjust, e_depth, e_place, e_refplace,
    e_docsect, e_refdoc
};

/***************************************************************************/
/*   :BANREGION attributes                                                 */
/***************************************************************************/
static const lay_att    banregion_att[] = {
    e_indent, e_hoffset, e_width, e_voffset, e_depth, e_font, e_refnum,
    e_region_position, e_pouring, e_script_format, e_contents
};

static bool             countb[TABLE_SIZE( banner_att )];
static bool             countr[TABLE_SIZE( banregion_att )];
static int              sum_countb;
static int              sum_countr;
static banner_lay_tag   wkb;            // for temp storage of banner attributes
static region_lay_tag   wkr;            // temp for input values
static region_lay_tag   *prev_reg;
static ban_place        refplace;
static ban_docsect      refdoc;

static banner_lay_tag   *curr_ban;
static banner_lay_tag   *del_ban;
static banner_lay_tag   *prev_ban;
static banner_lay_tag   *ref_ban;       // referenced banner for copy values


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

    for( k = 0; k < TABLE_SIZE( banner_att ); k++ ) {
        countb[k] = false;
    }
    sum_countb = 0;
}


/***************************************************************************/
/*  lay_banner                                                             */
/***************************************************************************/

void    lay_banner( const gmltag * entry )
{
    banner_lay_tag  *banwk;
    ban_place       match_place;
    ban_place       new_place;
    char            *p;
    condcode        cc;
    int             k;
    lay_att         curr;
    region_lay_tag  *regwk;
    region_lay_tag  *regwknew;
    region_lay_tag  *regwkprev;
    att_name_type   attr_name;
    att_val_type    attr_val;
    struct {
        unsigned    left_adjust     :1;
        unsigned    right_adjust    :1;
        unsigned    depth           :1;
        unsigned    place           :1;
        unsigned    docsect         :1;
        unsigned    refplace        :1;
        unsigned    refdoc          :1;
    } AttrFlags;

    p = g_scandata.s;
    rs_loc = TLOC_banner;

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != entry->u.layid ) {
        ProcFlags.lay_xxx = entry->u.layid;
        ProcFlags.banner = true;
        init_banner_wk( &wkb );
    }
    while( (cc = lay_attr_and_value( &attr_name, &attr_val )) == CC_pos ) {   // get att with value
        for( k = 0; k < TABLE_SIZE( banner_att ); k++ ) {
            curr = banner_att[k];
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                if( countb[k] ) {
                    if( sum_countb == TABLE_SIZE( banner_att ) ) {  // all attributes found
                        xx_err_exit( ERR_LAY_TEXT );     // per wgml 4.0: treat as text
                    } else {
                        xx_err_exit( ERR_ATT_DUP );      // per wgml 4.0: treat as duplicated attribute
                    }
                    /* never return */
                }
                countb[k] = true;
                sum_countb++;
                switch( curr ) {
                case e_left_adjust:
                    if( AttrFlags.left_adjust ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    i_space_unit( p, &attr_val, &wkb.left_adjust );
                    AttrFlags.left_adjust = true;
                    break;
                case e_right_adjust:
                    if( AttrFlags.right_adjust ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    i_space_unit( p, &attr_val, &wkb.right_adjust );
                    AttrFlags.right_adjust = true;
                    break;
                case e_depth:
                    if( AttrFlags.depth ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    i_space_unit( p, &attr_val, &wkb.depth );
                    AttrFlags.depth = true;
                    break;
                case e_place:
                    if( AttrFlags.place ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    i_place( p, &attr_val, &wkb.place );
                    AttrFlags.place = true;
                    break;
                case e_docsect:
                    if( AttrFlags.docsect ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    i_docsect( p, &attr_val, &wkb.docsect );
                    AttrFlags.docsect = true;
                    break;
                case e_refplace:  // not stored in banner struct
                    if( AttrFlags.refplace ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    i_place( p, &attr_val, &refplace );
                    AttrFlags.refplace = true;
                    break;
                case e_refdoc:    // not stored in banner struct
                    if( AttrFlags.refdoc ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    i_docsect( p, &attr_val, &refdoc );
                    AttrFlags.refdoc = true;
                    break;
                default:
                    internal_err_exit( __FILE__, __LINE__ );
                    /* never return */
                }
                break;                  // break out of for loop
            }
        }
    }

    /*******************************************************/
    /* At this point, end-of-tag has been reached and all  */
    /* attributes provided have been found and processed.  */
    /* First ensure the required attributes are present.   */
    /*******************************************************/

    if( (wkb.place == no_place)
      || (wkb.docsect == no_ban) ) {    // both must be specified
        xx_err_exit( ERR_ATT_MISSING );
        /* never return */
    }

    /*******************************************************/
    /* Process a reference banner.                         */
    /* When completed, wkb will have had any missing       */
    /* attribute values copied from the reference banner   */
    /* and sum_countb is set to 5, as if all attributes    */
    /* had been specified.                                 */
    /*******************************************************/

    if( (refdoc != no_ban)
      || (refplace != no_place) ) {    // at least one was used
        if( ((refdoc == no_ban)
          && (refplace != no_place))
          || ((refdoc != no_ban)
          && (refplace == no_place)) ) {
            xx_err_exit( ERR_BOTH_REFS );                        // both are required if either is used
            /* never return */
        }
        if( (refdoc == wkb.docsect)
          && (refplace == wkb.place) ) { // can't reference current banner
            xx_err_exit( ERR_SELF_REF );
            /* never return */
        }
        if( (refdoc != no_ban)
          && (refplace != no_place) ) { // find referenced banner
            ref_ban = NULL;
            for( banwk = layout_work.banner; banwk != NULL; banwk = banwk->next ) {
                if( (banwk->place == refplace)
                  && (banwk->docsect == refdoc) ) {
                    ref_ban = banwk;
                    break;
                }
            }
            if( ref_ban == NULL ) {                 // referenced banner not found
                xx_err_exit( ERR_ILLEGAL_BAN_REF );
                /* never return */
            }
            for( k = 0; k < TABLE_SIZE( banner_att ); ++k ) {
                if( !countb[k] ) {               // copy only unchanged values
                    countb[k] = 1;               // treat as new value
                    switch( banner_att[k] ) {
                    case e_left_adjust:
                        memcpy( &(wkb.left_adjust), &(ref_ban->left_adjust),
                                sizeof( wkb.left_adjust ) );
                        break;
                    case e_right_adjust:
                        memcpy( &(wkb.right_adjust), &(ref_ban->right_adjust),
                                sizeof( wkb.right_adjust ) );
                        break;
                    case e_depth:
                        memcpy( &(wkb.depth), &(ref_ban->depth),
                                sizeof( wkb.depth ) );
                        break;
                    default:            // refdoc and refplace are not stored
                        break;          // docsect and place must be specified
                    }
                }
            }
            // copy banregions too
            wkb.next_refnum = ref_ban->next_refnum;
            regwkprev = NULL;
            for( regwk = ref_ban->region; regwk != NULL; regwk = regwk->next ) { // allocate + copy banregions
                regwknew = mem_alloc( sizeof( region_lay_tag ) );
                memcpy( regwknew, regwk, sizeof( region_lay_tag ) );
                if( wkb.region == NULL ) {   // forward chain
                    wkb.region = regwknew;
                } else {
                    regwkprev->next = regwknew;
                }
                regwkprev = regwknew;
            }
            sum_countb = 5;                  // process as if all attributes for new banner found
        }
    }

    /*******************************************************/
    /* Locate and detach the current banner, if it exists. */
    /* If only refsect and place were given, assign it to  */
    /* del_ban; otherwise, assign it to curr_ban.          */
    /*******************************************************/

    for( banwk = layout_work.banner; banwk != NULL; banwk = banwk->next ) {
        if( (banwk->place == wkb.place)
          && (banwk->docsect == wkb.docsect) ) {
            for( k = 0; k < TABLE_SIZE( banner_att ); ++k ) {  // update banwk
                if( countb[k] ) {                // copy only changed values
                    switch( banner_att[k] ) {
                    case e_left_adjust:
                        memcpy( &(banwk->left_adjust), &(wkb.left_adjust), sizeof( wkb.left_adjust ) );
                        break;
                    case e_right_adjust:
                        memcpy( &(banwk->right_adjust), &(wkb.right_adjust), sizeof( wkb.right_adjust ) );
                        break;
                    case e_depth:
                        memcpy( &(banwk->depth), &(wkb.depth), sizeof( wkb.depth ) );
                        break;
                    default:                // refdoc and refplace are not stored
                        break;              // docsect and place must be specified
                    }
                }
            }
            if( wkb.region != NULL ) {

                /*******************************************************/
                /* This is a copy of a reference banner.               */
                /* The found banner's regions must be deleted and this */
                /* banners regions moved over.                         */
                /*******************************************************/

                banwk->next_refnum = wkb.next_refnum;
                while( (regwk = banwk->region) != NULL ) { // free copy of banregions
                    banwk->region = regwk->next;
                    mem_free( regwk );
                }
                banwk->region = wkb.region;
                wkb.region = NULL;
                wkb.next_refnum = 1;
            }
            if( prev_ban == NULL ) {                // first banner
                layout_work.banner = banwk->next;   // detach banner
            } else {
                prev_ban->next = banwk->next;       // detach banner
            }
            banwk->next = NULL;
            if( (sum_countb == 2)
              && (wkb.place != no_place)
              && (wkb.docsect != no_ban) ) {
                del_ban = banwk;                    // mark banner for possible deletion
            } else {
                curr_ban = banwk;                   // mark banner for possible update
            }
            break;
        } else {
            prev_ban = banwk;
        }
    }
    if( (curr_ban == NULL)
      && (del_ban == NULL) ) { // not found: new banner definition
        if( sum_countb != 5 ) {              // now we need all 5 non-ref attributes
            xx_err_exit( ERR_ALL_BAN_ATT_RQRD );
            /* never return */
        }
        curr_ban = mem_alloc( sizeof( banner_lay_tag ) );
        memcpy( curr_ban, &wkb, sizeof( banner_lay_tag ) );

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
            for( prev_ban = layout_work.banner; prev_ban->next != NULL; prev_ban = prev_ban->next ) {   // change the place, if found
                if( (prev_ban->docsect == curr_ban->docsect)
                  && (prev_ban->place == match_place) ) {
                    prev_ban->place = new_place;
                    break;
                }
            }
        }
    }
    g_scandata.s = g_scandata.e;
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

    ProcFlags.lay_xxx = TL_NONE;        // banner no longer active
    rs_loc = 0;

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

        if( (curr_ban != NULL)
          && (curr_ban->region == NULL) ) {  // flip if has no regions
            del_ban = curr_ban;
            curr_ban = NULL;
        }

        if( del_ban != NULL) {              // delete request
            while( (reg = del_ban->region) != NULL) {
                del_ban->region = reg->next;
                mem_free( reg );
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
        xx_err_exit_c( ERR_TAG_EXPECTED, "BANNER" );
        /* never return */
    }
    g_scandata.s = g_scandata.e;
    return;
}



/**************************************************************************************/
/*Define a banner region within a banner. Each banner region specifies a rectangular  */
/*section of the banner. A banner region begins with a :banregion tag and ends with an*/
/*:ebanregion tag. All banner regions are defined after the banner tag attributes and */
/*before the :ebanner tag.                                                            */
/*                                                                                    */
/*:BANREGION                                                                          */
/*        indent = 0                                                                  */
/*        hoffset = left                                                              */
/*        width = extend                                                              */
/*        voffset = 2                                                                 */
/*        depth = 1                                                                   */
/*        font = 0                                                                    */
/*        refnum = 1                                                                  */
/*        region_position = left                                                      */
/*        pouring = last                                                              */
/*        script_format = yes                                                         */
/*        contents = '/&$htext0.// &$pgnuma./'                                        */
/*                                                                                    */
/*indent The indent attribute accepts any valid horizontal space unit. The            */
/*specified space value is added to the value of the horizontal offset                */
/*attribute (hoffset) to determine the start of banner region in the                  */
/*banner if the horizontal offset is specified as left, centre, or center.            */
/*if the horizontal offset is specified as right, the indent value is                 */
/*subtracted from the right margin of the banner.                                     */
/*                                                                                    */
/*hoffset The hoffset attribute specifies the horizontal offset from the left side    */
/*of the banner where the banner region will start. The attribute value               */
/*may be any valid horizontal space unit, or one of the keywords left,                */
/*center, centre, or right. The keyword values remove the                             */
/*dependence upon the left and right adjustment settings of the banner                */
/*that occurs when using an absolute horizontal offset.                               */
/*                                                                                    */
/*width This attribute may be any valid horizontal space unit, or the                 */
/*keyword extend. If the width of the banner region is specified as                   */
/*Layout Tags 135?GML Reference                                                       */
/*extend, the width of the region will be increased until the start of                */
/*another banner region or the right margin of the banner is reached.                 */
/*                                                                                    */
/*voffset This attribute accepts any valid vertical space unit. It specifies the      */
/*vertical offset from the top of the banner for the start of the banner              */
/*region. A value of zero will be the first line of the banner, while the             */
/*value one will be the second line of the banner.                                    */
/*                                                                                    */
/*depth The depth attribute accepts a vertical space unit value. The attribute        */
/*value specifies the number of output lines or vertical space of the                 */
/*banner region.                                                                      */
/*                                                                                    */
/*font This attribute accepts a non-negative integer number. If a font                */
/*number is used for which no font has been defined, WATCOM                           */
/*Script/GML will use font zero. The font numbers from zero to three                  */
/*correspond directly to the highlighting levels specified by the                     */
/*highlighting phrase GML tags. The font attribute defines the font of                */
/*the banner region's contents.                                                       */
/*                                                                                    */
/*refnum This attribute accepts a positive integer number. Each banner region         */
/*must have a unique reference number. If this is the only attribute                  */
/*specified, the banner region is deleted from the banner.                            */
/*                                                                                    */
/*region_position This attribute specifies the position of the data within the        */
/*banner region. The attribute value may be one of the keywords left, center,         */
/*centre, or right.                                                                   */
/*                                                                                    */
/*pouring When the value of the contents attribute is a heading, and a heading        */
/*of the specified level does not appear on the output page, the                      */
/*contents can be 'poured' back to a previous heading level. When                     */
/*the attribute value none is specified, no pouring occurs. In this case,             */
/*the region will be empty. When the attribute value last is specified,               */
/*the last heading appearing in the document with the same level as                   */
/*the heading specified by the contents attribute is used. The attribute              */
/*value headn, where n may have a value of zero through six                           */
/*inclusive, may be specified. In this case, the last heading appearing               */
/*in the document which has a level between zero and the pouring                      */
/*value is used.                                                                      */
/*                                                                                    */
/*script_format This attribute determines if the contents region is processed as a    */
/*Script content string in the same way as the operand of a Script                    */
/*running title control word. If the attribute value is yes, then the                 */
/*value of the content attribute is treated as a Script format title string.          */
/*                                                                                    */
/*contents This attribute defines the content of the banner region. If the            */
/*content value does not fit in the banner region, the value is                       */
/*truncated. Symbols containing the values for each of the content                    */
/*keywords are also listed. Specifying these symbols as part of the                   */
/*string content may be used to create more complex banner region                     */
/*values. Note that when using a symbol in a content string of a                      */
/*banner definition, you will need to protect it from being substituted               */
/*during the definition with the &AMP symbol (ie                                      */
/*&AMP.AUTHOR.). The possible values are:                                             */
/*         author The first author of the document will be used.                      */
/*             The symbol $AUTHOR is also defined with                                */
/*             this value.                                                            */
/*         bothead The last heading on the output page is used.                       */
/*             The symbol $BOTHEAD is also defined with                               */
/*             this value.                                                            */
/*         date The current date will be used.                                        */
/*         docnum The document number will be the content of                          */
/*             the banner region. The symbol $DOCNUM is                               */
/*             also defined with this value.                                          */
/*         HEADn The last heading of level n, where n may have                        */
/*             a value of zero through six inclusive. Both the                        */
/*             heading number and heading text are both                               */
/*             used. The symbols $HEAD0 through                                       */
/*             $HEAD6 are also defined with this value.                               */
/*         HEADNUMn The heading number from the last heading of                       */
/*             level n, where n may have a value of zero                              */
/*             through six inclusive. The symbols $HNUM0                              */
/*             through $HNUM6 are also defined with this                              */
/*             value.                                                                 */
/*         HEADTEXTn The text of the heading from the last heading                    */
/*             of level n, where n may have a value of zero                           */
/*             through six inclusive. If the stitle attribute was                     */
/*             specified for the selected heading, the stitle                         */
/*             value is used. The symbols $HTEXT0 through                             */
/*             $HTEXT6 are also defined with this value.                              */
/*         none The banner region will be empty.                                      */
/*         pgnuma The content of the banner region will be the                        */
/*             page number of the output page in the                                  */
/*             hindu-arabic numbering style. The symbol                               */
/*             $PGNUMA is also defined with this value.                               */
/*         pgnumad The content of the banner region will be the                       */
/*             page number of the output page in the                                  */
/*             hindu-arabic numbering style followed by a                             */
/*             decimal point. The symbol $PGNUMAD is                                  */
/*             also defined with this value.                                          */
/*         pgnumr The content of the banner region will be the                        */
/*             page number of the output page in the lower                            */
/*             case roman numbering style. The symbol :                               */
/*             $PGNUMR is also defined with this value.                               */
/*         pgnumrd The content of the banner region will be the                       */
/*             page number of the output page in the lower                            */
/*             case roman numbering style followed by a :                             */
/*             decimal point. The symbol $PGNUMRD is                                  */
/*             also defined with this value.                                          */
/*         pgnumc The content of the banner region will be the                        */
/*             page number of the output page in the upper                            */
/*             case roman numbering style. The symbol :                               */
/*             $PGNUMC is also defined with this value.                               */
/*         pgnumcd The content of the banner region will be the                       */
/*             page number of the output page in the upper                            */
/*             case roman numbering style followed by a :                             */
/*             decimal point. The symbol $PGNUMCD is                                  */
/*             also defined with this value.                                          */
/*         rule The content of the banner region will be a rule                       */
/*             line which fills the entire region.                                    */
/*             sec The security value specified by the sec                            */
/*             attribute on the :gdoc tag is used. The symbol                         */
/*             $SEC is also defined with this value.                                  */
/*         stitle The stitle attribute value from the first title tag                 */
/*             specified in the front material of the document                        */
/*             is used. If the stitle attribute was not specified,                    */
/*             the title text is used. The symbol $STITLE is                          */
/*             also defined with this value.                                          */
/*         title The text of the first title tag specified in the                     */
/*             front material of the document is used. The                            */
/*             symbol $TITLE is also defined with this value.                         */
/*         string Any character string enclosed in quotation                          */
/*             marks.                                                                 */
/*         time The current time will be used.                                        */
/*         tophead The first heading on the output page is used.                      */
/*             The symbol $TOPHEAD is also defined with                               */
/*             this value.                                                            */
/*                                                                                    */
/*if a banner region does not already exist, then all attributes must be specified.   */
/*If you wish to modify an existing banner region, the refnum attribute will          */
/*uniquely identify the region. When the reference number is that of an               */
/*existing banner region, all other attributes will modify the values of the          */
/*existing banner region.                                                             */
/*To delete a banner region, specify only the refnum attribute. All banner            */
/*regions must be deleted before a banner definition will be removed.                 */
/**************************************************************************************/


/***************************************************************************/
/*  init banregion with no values                                          */
/***************************************************************************/

static void     init_banregion_wk( region_lay_tag *reg )
{
    int         k;
    char        z0[2] = "0";

    reg->next = NULL;
    reg->reg_indent = 0;
    reg->reg_hoffset = 0;
    reg->reg_width = 0;
    reg->reg_voffset = 0;
    reg->reg_depth = 0;
    lay_init_su( z0, &(reg->indent) );
    lay_init_su( z0, &(reg->hoffset) );
    lay_init_su( z0, &(reg->width) );
    lay_init_su( z0, &(reg->depth) );
    reg->font = FONT0;
    reg->refnum = 0;
    reg->region_position = PPOS_left;
    reg->pouring = no_pour;
    reg->script_format = false;
    reg->contents.content_type = string_content;
    reg->contents.string[0] = '\0';
    reg->script_region[0].len = 0;
    reg->script_region[1].len = 0;
    reg->script_region[2].len = 0;
    reg->script_region[0].string = NULL;
    reg->script_region[1].string = NULL;
    reg->script_region[2].string = NULL;
    reg->final_content[0].size = 0;
    reg->final_content[1].size = 0;
    reg->final_content[2].size = 0;
    reg->final_content[0].hoffset = 0;
    reg->final_content[1].hoffset = 0;
    reg->final_content[2].hoffset = 0;
    reg->final_content[0].string = NULL;
    reg->final_content[1].string = NULL;
    reg->final_content[2].string = NULL;

    for( k = 0; k < TABLE_SIZE( banregion_att ); k++ ) {
        countr[k] = false;
    }
    sum_countr = 0;
}


/***************************************************************************/
/*  lay_banregion                                                          */
/***************************************************************************/

void    lay_banregion( const gmltag *entry )
{
    char            *p;
    condcode        cc;
    int             k;
    lay_att         curr;
    region_lay_tag  *reg;
    att_name_type   attr_name;
    att_val_type    attr_val;
    struct {
        unsigned    indent          :1;
        unsigned    hoffset         :1;
        unsigned    width           :1;
        unsigned    voffset         :1;
        unsigned    depth           :1;
        unsigned    font            :1;
        unsigned    refnum          :1;
        unsigned    region_position :1;
        unsigned    pouring         :1;
        unsigned    script_format   :1;
        unsigned    contents        :1;
    } AttrFlags;

    p = g_scandata.s;
    rs_loc = TLOC_banreg;

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( del_ban != NULL ) {             // BANREGION cancels deleteable status
        curr_ban = del_ban;
        del_ban = NULL;
    }

    if( ProcFlags.lay_xxx != entry->u.layid ) {
        if( !ProcFlags.banner ) {               // not in BANNER/eBANNER block
            xx_err_exit_c( ERR_TAG_EXPECTED, "BANNER" );
            /* never return */
        }
        ProcFlags.lay_xxx = entry->u.layid;
        init_banregion_wk( &wkr );
    }

    while( (cc = lay_attr_and_value( &attr_name, &attr_val )) == CC_pos ) {   // get att with value
        for( k = 0; k < TABLE_SIZE( banregion_att ); k++ ) {
            curr = banregion_att[k];
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                if( countr[k] ) {
                    if( sum_countr == TABLE_SIZE( banregion_att ) ) {  // all attributes found
                        xx_err_exit( ERR_LAY_TEXT );     // per wgml 4.0: treat as text
                    } else {
                        xx_err_exit( ERR_ATT_DUP );      // per wgml 4.0: treat as duplicated attribute
                    }
                    /* never return */
                }
                countr[k] = true;
                sum_countr++;
                switch( curr ) {
                case e_indent:
                    if( AttrFlags.indent ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    i_space_unit( p, &attr_val, &wkr.indent );
                    AttrFlags.indent = true;
                    break;
                case e_hoffset:
                    if( AttrFlags.hoffset ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    i_space_unit( p, &attr_val, &wkr.hoffset );
                    AttrFlags.hoffset = true;
                    break;
                case e_width:
                    if( AttrFlags.width ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    i_space_unit( p, &attr_val, &wkr.width );
                    AttrFlags.width = true;
                    break;
                case e_voffset:
                    if( AttrFlags.voffset ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    i_space_unit( p, &attr_val, &wkr.voffset );
                    AttrFlags.voffset = true;
                    break;
                case e_depth:
                    if( AttrFlags.depth ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    i_space_unit( p, &attr_val, &wkr.depth );
                    AttrFlags.depth = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    i_font_number( p, &attr_val, &wkr.font );
                    AttrFlags.font = true;
                    break;
                case e_refnum:
                    if( AttrFlags.refnum ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    i_uint8( p, &attr_val, &wkr.refnum );
                    if( wkr.refnum == 0 ) {   // refnum must be greater than zero
                        xx_line_err_exit_c( ERR_NUM_ZERO, p );
                        /* never return */
                    }
                    AttrFlags.refnum = true;
                    break;
                case e_region_position:
                    if( AttrFlags.region_position ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    i_page_position( p, &attr_val, &wkr.region_position );
                    AttrFlags.region_position = true;
                    break;
                case e_pouring:
                    if( AttrFlags.pouring ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    i_pouring( p, &attr_val, &wkr.pouring );
                    AttrFlags.pouring = true;
                    break;
                case e_script_format:
                    if( AttrFlags.script_format ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    i_yes_no( p, &attr_val, &wkr.script_format );
                    AttrFlags.script_format = true;
                    break;
                case e_contents:
                    if( AttrFlags.contents ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    if( attr_val.quoted != ' ' ) {
                        wkr.contents.content_type = string_content;
                        i_xx_string( p, &attr_val, wkr.contents.string );
                    } else {
                        i_content( p, &attr_val, &wkr.contents );
                    }
                    AttrFlags.contents = true;
                    break;
                default:
                    internal_err_exit( __FILE__, __LINE__ );
                    /* never return */
                }
                break;                  // break out of for loop
            }
        }
    }

    /*******************************************************/
    /* At this point, end-of-tag has been reached and all  */
    /* attributes provided have been found and processed.  */
    /* First ensure the required attribute is present.     */
    /*******************************************************/

    if( !AttrFlags.refnum ) {                           // refnum was missing
        xx_err_exit( ERR_ATT_MISSING );
        /* never return */
    } else if( wkr.refnum > curr_ban->next_refnum ) {    // refnum must be, at most, the next value
        xx_err_exit( ERR_ILLEGAL_REG_REF );
        /* never return */
    }

    /*******************************************************/
    /* Find the region.                                    */
    /*******************************************************/

    prev_reg = NULL;
    for( reg = curr_ban->region; reg != NULL; reg = reg->next ) {
        if( reg->refnum == wkr.refnum ) {// found correct region
            break;
        }
        prev_reg = reg;
    }

    /*******************************************************/
    /* Process the region.                                 */
    /*******************************************************/

    if( reg != NULL ) {                 // region found
        if( sum_countr == 1 ) {          // banregion delete request
            if( prev_reg == NULL ) {
                curr_ban->region = reg->next;
            } else {
                prev_reg->next = reg->next;
            }
            mem_free( reg );
            reg = NULL;
        } else {                        // modify existing banregion
            for( k = 0; k < TABLE_SIZE( banregion_att ); ++k ) {
                if( countr[k] ) {        // change specified attribute
                    switch( banregion_att[k] ) {
                    case e_indent:
                        memcpy( &(reg->indent), &(wkr.indent), sizeof( wkr.indent ) );
                        break;
                    case e_hoffset:
                        memcpy( &(reg->hoffset), &(wkr.hoffset), sizeof( wkr.hoffset ) );
                        break;
                    case e_width:
                        memcpy( &(reg->width), &(wkr.width), sizeof( wkr.width ) );
                        break;
                    case e_voffset:
                        memcpy( &(reg->voffset), &(wkr.voffset), sizeof( wkr.voffset ) );
                        break;
                    case e_depth:
                        memcpy( &(reg->depth), &(wkr.depth), sizeof( wkr.depth ) );
                        break;
                    case e_font:
                        reg->font = wkr.font;
                        break;
                    case e_refnum:
                        reg->refnum = wkr.refnum;
                        break;
                    case e_region_position:
                        reg->region_position = wkr.region_position;
                        break;
                    case e_pouring:
                        reg->pouring = wkr.pouring;
                        break;
                    case e_script_format:
                        reg->script_format = wkr.script_format;
                        break;
                    case e_contents:
                        memcpy( &(reg->contents), &(wkr.contents), sizeof( wkr.contents ) );
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    } else {                            // new region
        if( sum_countr == 1 ) {          // banregion delete request

            /*******************************************************/
            /* If a non-existent region is supposed to be deleted, */
            /* then there is nothing to do.                        */
            /*******************************************************/

        } else if( (sum_countr != 11)
          && (AttrFlags.script_format
          && (sum_countr == 10)) ) {

            /*******************************************************/
            /* A new region must have values for all attributes    */
            /* except script_format.                               */
            /*******************************************************/

            xx_err_exit( ERR_ALL_REG_ATT_RQRD );
            /* never return */
        }
        reg = mem_alloc( sizeof( region_lay_tag ) );
        memcpy( reg, &wkr, sizeof( region_lay_tag ) );
        if( prev_reg == NULL ) {    // first region in banner
            curr_ban->region = reg;
        } else {
            prev_reg->next = reg;
        }
        curr_ban->next_refnum++;    // next expected refnum value
    }

    /*******************************************************/
    /* Check for attribute inconsistency.                  */
    /*******************************************************/

    if( reg != NULL ) {             // region not deleted
        if( reg->script_format
          && (reg->contents.content_type != string_content) ) {
            xx_err_exit( ERR_SCR_FMT );
            /* never return */
        }
    }

    g_scandata.s = g_scandata.e;
    return;
}


/***************************************************************************/
/*  lay_ebanregion                                                         */
/***************************************************************************/

void    lay_ebanregion( const gmltag * entry )
{
    rs_loc = TLOC_banner;
    if( ProcFlags.lay_xxx == TL_BANREGION ) {   // :banregion was last tag
        ProcFlags.lay_xxx = entry->u.layid;
    } else {
        xx_err_exit_cc( ERR_NO_LAY, &(entry->tagname[1]), entry->tagname );
        /* never return */
    }
    g_scandata.s = g_scandata.e;
    return;
}



/***************************************************************************/
/*  output a banner region                                                 */
/***************************************************************************/
static void     put_lay_region( FILE *fp, region_lay_tag *reg )
{
    int                 k;
    lay_att             curr;

    fprintf( fp, ":BANREGION\n" );

    for( k = 0; k < TABLE_SIZE( banregion_att ); k++ ) {
        curr = banregion_att[k];
        switch( curr ) {
        case e_indent:
            o_space_unit( fp, curr, &reg->indent );
            break;
        case e_hoffset:
            o_space_unit( fp, curr, &reg->hoffset );
            break;
        case e_width:
            o_space_unit( fp, curr, &reg->width );
            break;
        case e_voffset:
            o_space_unit( fp, curr, &reg->voffset );
            break;
        case e_depth:
            o_space_unit( fp, curr, &reg->depth );
            break;
        case e_font:
            o_font_number( fp, curr, &reg->font );
            break;
        case e_refnum:
            o_uint8( fp, curr, &reg->refnum );
            break;
        case e_region_position:
            o_page_position( fp, curr, &reg->region_position );
            break;
        case e_pouring:
            o_pouring( fp, curr, &reg->pouring );
            break;
        case e_script_format:
            o_yes_no( fp, curr, &reg->script_format );
            break;
        case e_contents:
            o_content( fp, curr, &reg->contents );
            break;
        default:
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
    }
    fprintf( fp, ":eBANREGION\n" );
}

/***************************************************************************/
/*  output a single banner with regions                                    */
/***************************************************************************/
static void    put_lay_single_ban( FILE *fp, banner_lay_tag *ban )
{
    int                 k;
    lay_att             curr;
    region_lay_tag  *   reg;

    fprintf( fp, ":BANNER\n" );

    for( k = 0; k < TABLE_SIZE( banner_att ); k++ ) {
        curr = banner_att[k];
        switch( curr ) {
        case e_left_adjust:
            o_space_unit( fp, curr, &ban->left_adjust );
            break;
        case e_right_adjust:
            o_space_unit( fp, curr, &ban->right_adjust );
            break;
        case e_depth:
            o_space_unit( fp, curr, &ban->depth );
            break;
        case e_place:
            o_place( fp, curr, &ban->place );
            break;
        case e_docsect:
            o_docsect( fp, curr, &ban->docsect );
            break;
        case e_refplace:
        case e_refdoc:
            /* no action these are only used for input */
            break;
        default:
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
    }
    for( reg = ban->region; reg != NULL; reg = reg->next ) {
        put_lay_region( fp, reg );
    }
    fprintf( fp, ":eBANNER\n" );
}


/***************************************************************************/
/*   :BANNER   output all banners                                          */
/***************************************************************************/
void    put_lay_banner( FILE *fp, layout_data * lay )
{
    banner_lay_tag  *ban;

    for( ban = lay->banner; ban != NULL; ban = ban->next ) {
        put_lay_single_ban( fp, ban );
    }
}
