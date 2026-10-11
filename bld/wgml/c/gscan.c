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
* Description:  utility functions for wgml input line scanning
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*    GML tags                                                             */
/***************************************************************************/

static const gmltag     gml_tags[] = {
    #define pick1(n,l,r,g,o,c) { { T_##n }, #n, l, r, g, o, c },
    #define pick2(n1,l1,r1,g1,o1,c1,n2,l2,r2,g2,o2,c2) \
                pick1(n1,l1,r1,g1,o1,c1) pick1(n2,l2,r2,g2,o2,c2)
    #include "gtags.h"
    #undef pick2
    #undef pick1
};

#define GML_TAGMAX      TABLE_SIZE( gml_tags )

/***************************************************************************/
/*    GML layout tags                                                      */
/***************************************************************************/

static const gmltag     lay_tags[] = {
    #define pick( name, length, funci, funco, gmlflags, locflags ) { { TL_##name }, #name, length, funci, gmlflags, locflags, TCLS_no_class },
    #include "gtagslay.h"
    #undef pick
};

#define LAY_TAGMAX      TABLE_SIZE( lay_tags )


/***************************************************************************/
/*    SCRIPT control words                                                 */
/***************************************************************************/

static const scrtag     scr_kwds[] = {
    #define pick( name, routine, flags) { #name, routine, flags },
    #include "gscrcws.h"
    #undef pick
};

#define SCR_KWDMAX      TABLE_SIZE( scr_kwds )

static uint8_t  scr_lkup_tbl[26 * 36];
static uint8_t  scr_lkup_label;
static bool     scr_lkup_setup = false;

/***************************************************************************/
/*  Build a lookup table for SCRIPT control words. Since most of them have */
/*  a name consisting of two alphabetic characters, we can build a simple  */
/*  direct-lookup table that isn't obscenely large, and handle the few     */
/*  remaining control words as exceptions.                                 */
/***************************************************************************/

static int get_bin_index( unsigned char c )
{
    if( c >= '0' && c <= '9' )
        return( c - '0' );
    if( c >= 'a' && c <= 'z' )
        return( c - 'a' + 10 );
    return( 0 );
}

static int get_index( const char *p )
{
    return( get_bin_index( p[0] ) + 26 * get_bin_index( p[1] ) );
}

static uint8_t *get_name_map_index( const char *p )
{
    if( p[0] == '.' && p[1] == '.' ) {
        return( &scr_lkup_label );
    } else {
        return( scr_lkup_tbl + get_index( p ) );
    }
}

#define INVALID_INDEX   255

static void build_scr_cw_lookup( void )
{
    int             i;

    // pre-fill lookup table with invalid values
    memset( scr_lkup_tbl, INVALID_INDEX, sizeof( scr_lkup_tbl ) );
    scr_lkup_label = INVALID_INDEX;

    // build a lookup table holding keyword table indices; note that
    // the indices are offset by one so that zero turns into an invalid
    // index (-1) during lookup.
    for( i = 0; i < SCR_KWDMAX; ++i ) {
        *get_name_map_index( scr_kwds[i].cwdname ) = i;
    }
    scr_lkup_setup = true;
}


static const scrtag *find_scr_cw( const char *cwdname )
{
    int             index;

    if( !scr_lkup_setup )
        build_scr_cw_lookup();

    index = *get_name_map_index( cwdname );
    if( index == INVALID_INDEX )
        return( NULL );

    return( scr_kwds + index );
}

#undef INVALID_INDEX

void set_overload( gtentry *in_gt )
{
    in_gt->overload = ( find_sys_tag( in_gt->tagname ) != NULL );
}

/***************************************************************************/
/*  scan for gml tags                                                      */
/***************************************************************************/

static void scan_gml( void )
{
    inputcb         *cb;
    char            *p;
    bool            processed;
    gtentry         *ge;                // GML user tag entry
    mac_entry       *me;                // script macro for processing GML tag
    char            tagname[TAG_NAME_LENGTH + 1];
    const gmltag    *tag;

    cb = input_cbs;

    g_tok_start = g_scandata.s;
    p = check_tagname( g_scandata.s, tagname );
    if( p == NULL ) {
        return;
    }

    g_scandata.s = p;                      // store argument start address

    if( GlobalFlags.firstpass && (cb->fmflags & II_research) ) {

        if( strcmp( "CMT", tagname ) != 0 ) {   // quiet for :cmt.

            if( cb->fmflags & II_tag_mac ) {
                printf_research( "L%d    %c%s tag found in macro %s(%d)\n\n",
                                 inc_level, GML_char, tagname,
                                 cb->s.m->mac->name, cb->s.m->lineno );
            } else {
                printf_research( "L%d    %c%s tag found in file %s(%d)\n\n",
                                 inc_level, GML_char, tagname,
                                 cb->s.f->filename, cb->s.f->lineno );
            }
        }
        add_GML_tag_research( tagname );
    }

    if( ProcFlags.layout ) {
        ge = NULL;                      // no user tags within :LAYOUT
    } else {
        ge = find_user_tag( &tags_dict, tagname );
    }
    processed = false;
    me = NULL;
    if( ge != NULL ) {                  // GML user defined Tag found
        if( ProcFlags.need_text ) {
            xx_err_exit( ERR_TEXT_NOT_TAG_CW );
            /* never return */
        }
        if( ge->tagflags & GTFLG_off ) {  // inactive, treat as comment
            g_scandata.s = g_scandata.e;
            return;
        }
        me = find_macro( macro_dict, ge->macname );
        if( me == NULL ) {
            g_tag_mac_err_exit( ge );
            /* never return */
        }
        /*******************************************************************/
        /*  When a user-defined tag which overloads a predefined tag (ie,  */
        /*  the tag names are the same) is used inside a macro, the        */
        /*  predefined tag is used instead by WGML 4.0.                    */
        /*                                                                 */
        /*  Note that this allows a user-defined tag, or the macros it     */
        /*  invokes, to use that tag without any danger of recursion.      */
        /*  It is far more general than simply preventing recursive        */
        /*  user-defined tag definitions.                                  */
        /*******************************************************************/

        if( (cb->fmflags & II_tag_mac) && ge->overload ) {
            me = NULL;
        }
    }
    if( me != NULL ) {                  // usertag and coresponding macro ok
        processed = process_tag( ge, me );
    } else {
        /*
         * If the token is longer than the maximum allowed tag name length,
         * it is valid tag name but we use internaly shortened name.
         */
        if( ProcFlags.layout || strcmp( "LAYOUT", tagname ) == 0 ) {        // different tags within :LAYOUT
            tag = find_lay_tag( tagname );
            if( tag != NULL ) {
                ProcFlags.tag_end_found = false;
                if( GlobalFlags.firstpass
                  || tag->u.layid == TL_LAYOUT
                  || tag->u.layid == TL_ELAYOUT ) {
                    if( rs_loc == 0 ) {
                        // no restrictions: do them all
                        tag->gmlproc( tag );
                    } else if( tag->taglocs & rs_loc ) {
                        // tag allowed in this restricted location
                        tag->gmlproc( tag );
                    } else if( (tag->tagflags & TFLG_is_general) != 0 ) {
                        // tag allowed everywhere
                        tag->gmlproc( tag );
                    } else if( rs_loc == TLOC_banner ) {
                        xx_err_exit_c( ERR_TAG_EXPECTED, "eBANNER" );
                        /* never return */
                    } else {    // rs_loc == TLOC_banreg
                        xx_err_exit_c( ERR_TAG_EXPECTED, "eBANREGION" );
                        /* never return */
                    }
                    SkipDot( g_scandata.s );
                } else {
                    g_scandata.s = g_scandata.e;
                    eat_lay_sub_tag();
                }
                processed = true;
            } else if( find_sys_tag( tagname ) != NULL ) {
                xx_err_exit_c( ERR_GML_IN_LAY, tagname );
                /* never return */
            }
        } else {                        // not within :LAYOUT
            tag = find_sys_tag( tagname );
            if( tag != NULL ) {
                /*******************************************************************/
                /*  If any of the BD/BI/US tags is active, then:                   */
                /*  The inline phrase start tags (CIT, Hn, Q, and SF) need to be   */
                /*  able to cancel this, and then the end tag must restore it.     */
                /*  The index tags (I1, I2, I3, IH1, IH2, and IH3) must not cancel */
                /*  this but other tags must do so.                                */
                /*******************************************************************/
                if( g_script_style.style != SCT_none ) {
                    if( (tag->tagclass & TCLS_ip_start) != 0 ) {
                        scr_style_copy( &g_script_style, &g_script_style_sav );
                    }
                    if( (tag->tagclass & TCLS_index) == 0 ) {
                        scr_style_end();
                    }
                }

                ProcFlags.need_tag = false;

                /*******************************************************************/
                /*  When text occurs after certain blocks, it is processed as if   */
                /*  it were preceded by tag PC. This is cancelled when a tag comes */
                /*  before the text, but not if the tag starts or ends an inline   */
                /*  phrase or is an index tag (I1, I2, I3, IH1, IH2, IH3) or is    */
                /*  tag SET.                                                       */
                /*******************************************************************/

                if( (tag->tagclass & (TCLS_ip_start | TCLS_ip_end | TCLS_index)) == 0
                  && tag->u.tagid != T_SET ) {
                    ProcFlags.force_pc = false;
                }

                /*******************************************************************/
                /*  The Procflags must be cleared to prevent the error from being  */
                /*  reported for every tag until the proper end tag is found.      */
                /*  If the number of errors reported is limited at some point,     */
                /*  then those lines can be removed.                               */
                /*  The index tags (I1, I2, I3, IH1, IH2, IH3) are exceptions      */
                /*******************************************************************/

                ProcFlags.tag_end_found = false;
                if( ProcFlags.need_ddhd ) {
                    if( tag->tagclass & TCLS_index ) {
                        // tag is index tag
                        tag->gmlproc( tag );
                    } else if( tag->tagclass & TCLS_def ) {
                        // tag is DD, DDHD or GD
                        tag->gmlproc( tag );
                        ProcFlags.need_ddhd = false;
                    } else {
                        xx_err_exit_c( ERR_TAG_EXPECTED, "DDHD");
                        /* never return */
                    }
                } else if( ProcFlags.need_dd ) {
                    if( tag->tagclass & TCLS_index ) {
                        // tag is index tag
                        tag->gmlproc( tag );
                    } else if( tag->tagclass & TCLS_def ) {                                    // tag is DD, DDHD or GD
                        tag->gmlproc( tag );
                        ProcFlags.need_dd = false;
                    } else {
                        xx_err_exit_c( ERR_TAG_EXPECTED, "DD");
                        /* never return */
                    }
                } else if( ProcFlags.need_gd ) {
                    if( (tag->tagclass & TCLS_index) == 0 ) {
                        // tag is index tag
                        tag->gmlproc( tag );
                    } else if( tag->tagclass & TCLS_def ) {                                    // tag is DD, DDHD or GD
                        // tag is DD, DDHD or GD
                        tag->gmlproc( tag );
                        ProcFlags.need_gd = false;
                    } else {
                        xx_err_exit_c( ERR_TAG_EXPECTED, "GD");
                        /* never return */
                    }
                } else if( !nest_cb->in_list ) {
                    if( (tag->tagclass & TCLS_list) == 0 ) {
                        // tag is not a list tag
                        tag->gmlproc( tag );
                    } else {
                        xx_line_err_exit_c( ERR_NO_LIST, g_tok_start );
                        /* never return */
                    }
                } else if( ProcFlags.need_li_lp ) {
                    if( tag->tagclass & TCLS_li_lp ) {
                        // tag is LP or LI
                        tag->gmlproc( tag );
                    } else {
                        xx_nest_err_exit( ERR_NO_LI_LP );
                        /* never return */
                    }
                } else if( ProcFlags.need_text ) {
                    xx_err_exit( ERR_TEXT_NOT_TAG_CW );
                    /* never return */
                } else if( rs_loc == 0 ) {
                    // no restrictions: do them all
                    tag->gmlproc( tag );
                } else if( tag->taglocs & rs_loc ) {
                    // tag allowed in this restricted location
                    tag->gmlproc( tag );
                } else if( tag->tagflags & TFLG_is_general ) {
                    // tag allowed everywhere
                    tag->gmlproc( tag );
                } else {
                    start_doc_sect();   // if not already done
                    g_tag_rsloc_err_exit( rs_loc, g_tok_start );
                    /* never return */
                }
                processed = true;
                SkipDot( g_scandata.s );
            } else if( find_lay_tag( tagname ) != NULL ) {
                xx_err_exit_c( ERR_LAY_IN_GML, tagname );
                /* never return */
            }
        }
    }
    if( !processed ) {                  // treat as text
        g_scandata.s = g_tok_start;
    }
}


/*
 * search for (control word) separator in string outside of quotes
 *       returns ptr to sep char or NULL if not found
 *      quotes are single or double quotes only
 */

static char *search_separator( const char *str, char sep )
{
    bool        instring = false;
    char        quote = '\0';

    while( *str != '\0' ) {
        if( instring ) {
            if( *str == quote ) {
                instring = false;
            }
        } else {
            if( (*str == '\"') || (*str == '\'') ) {
                instring = true;
                quote = *str;
            } else {
                if( *str == sep ) {
                    break;
                }
            }
        }
        str++;
    }
    if( *str == sep ) {
        return( (char *)str );
    } else {
        return( NULL );
    }
}


/*
 * Scan line with script control word
 *      uses g_scandata.s ptr, but assumes this is in buff2
 */

static void     scan_script( void )
{
    inputcb         *cb;
    mac_entry       *me;
    char            *p;
    char            *pt;
    const scrtag    *cwinfo;
    char            macname[MAC_NAME_LENGTH + 1];

    if( ProcFlags.need_text ) {
        xx_err_exit( ERR_TEXT_NOT_TAG_CW );
        /* never return */
    }

    cb = input_cbs;
    p = g_scandata.s + 1;
    if( !ProcFlags.literal && (*p == '\0') ) {  // catch line with only "." in it
        if( ProcFlags.concat ) {
            *g_scandata.s = ' ';                  // make line blank
        } else {
            g_scandata.s = g_scandata.e;            // treat as comment
        }
        return;
    }

    scan_restart = g_scandata.s;

    if( *p == '*' ) {                       // early check for .*
        g_scandata.s = g_scandata.e;             // .* ignore comment up to EOL
        return;
    }

    if( p[0] == SCR_char && p[1] == SCR_char ) {
        pt = macname;
        *pt++ = SCR_char;               // special for ...label
        *pt++ = SCR_char;
        *pt   = '\0';
        me = NULL;
        g_scandata.s++;
    } else {
        if( *p == SCR_char ) {          // ..
            p++;
            ProcFlags.macro_ignore = true;
            me = NULL;
        } else {
            ProcFlags.macro_ignore = false;
        }
        if( *p == '\'' ) {              // .' (or ..')
            p++;
            if( !ProcFlags.cw_force_sep ) {     // only change if not indented or all indents were .'
                ProcFlags.cw_sep_ignore = true;
            }
        } else {                        // no ': set per cw_sep_char
            if( cw_sep_char == '\0') {
                ProcFlags.cw_sep_ignore = true;// no separator char no split
            } else{
                ProcFlags.cw_sep_ignore = false;
            }
        }

        if( *p == '*' ) {               // check for comment again; the following are
            g_scandata.s = g_scandata.e;    // all valid: .'* ..* ..'*
            return;
        }

        if( ProcFlags.literal ) {       // no macro or split line if literal
            ProcFlags.cw_sep_ignore = true;
            ProcFlags.macro_ignore = true;
        }
        if( !ProcFlags.cw_sep_ignore ) { // scan line for cw_sep_char
            char    *   pchar;

            pchar = search_separator( buff2, cw_sep_char );

            if( pchar != NULL ) {
                if( *(pchar + 1) != '\0' ) { // only split if more follows
                    split_input( buff2, pchar + 1, II_none ); // split after cw_sep_char
                }
                *pchar= '\0';           // delete cw_sep_char
                buff2_lg = strlen( buff2 ); // new length of first part
            }
        }

        g_scandata.s = p;

        p = get_macro_name( p, macname );

        if( !ProcFlags.cw_sep_ignore && (*macname == '\0') ) {
            // no valid script controlword / macro, treat as text
            g_scandata.s = scan_restart;
            return;
        }

        if( !ProcFlags.macro_ignore ) {
            me = find_macro( macro_dict, macname );
        } else {
            me = NULL;
        }
    }

    if( me != NULL ) {                  // macro found
        if( GlobalFlags.firstpass && (cb->fmflags & II_research) ) {
            if( cb->fmflags & II_tag_mac ) {
                printf_research( "L%d    %c%s macro found in macro %s(%d)\n\n",
                                 inc_level, SCR_char, macname,
                                 cb->s.m->mac->name, cb->s.m->lineno );
            } else {
                printf_research( "L%d    %c%s macro found in file %s(%d)\n\n",
                                 inc_level, SCR_char, macname,
                                 cb->s.f->filename, cb->s.f->lineno );
            }
            add_SCR_tag_research( macname );
        }
        add_macro_cb_entry( me, NULL );
        inc_inc_level();
        if( *p == '\0' ) {
            add_macro_parms( p );
        } else {
            add_macro_parms( p + 1 );
        }
        scan_restart = g_scandata.e;
    } else if( !ProcFlags.literal ) {   // try script controlword if not in LI
        g_scandata.s += SCR_KW_LENGTH;
        p = g_scandata.s;
        if( (cb->fmflags & II_research) && GlobalFlags.firstpass ) {
            if( cb->fmflags & II_tag_mac ) {
                printf_research( "L%d    %c%s CW found in macro %s(%d)\n\n",
                                 inc_level, SCR_char, macname,
                                 cb->s.m->mac->name, cb->s.m->lineno );
            } else {
                printf_research( "L%d    %c%s CW found in file %s(%d)\n\n",
                                 inc_level, SCR_char, macname,
                                 cb->s.f->filename, cb->s.f->lineno );
            }
            add_SCR_tag_research( macname );
        }

        if( *macname == '\0' ) {        // lone . or .' -- ignored
            g_scandata.s = g_scandata.e;
            return;
        }

        cwinfo = find_scr_cw( macname ); // non-negative if valid
        if( cwinfo != NULL ) {
            if( !ProcFlags.layout
              && !ProcFlags.fb_document_done
              && (cwinfo->cwdflags & CW_out_text) ) {

                /********************************************************/
                /* this is the first control word which produces output */
                /* start the document, the layout is done               */
                /* start_doc_sect() calls do_layout_end_processing()    */
                /********************************************************/

                start_doc_sect();
            }
            ProcFlags.cw_noblank = false; // blank after CW is default
            if( ProcFlags.literal  ) {  // .li active
                if( strcmp( "li", macname ) == 0 ) {  // .li
                    ProcFlags.cw_noblank = (*p != ' ');
                    g_scandata.s = p;     // found, process
                    cwinfo->cwdproc();
                }
            } else {
                g_scandata.s = p;         // script controlword found, process
                if( cwinfo->cwdflags & CW_break ) {
                    ProcFlags.force_pc = false;
                    scr_process_break();// output incomplete line, if any
                }
                ProcFlags.cw_noblank = (*p != ' ');
                cwinfo->cwdproc();
            }
        } else {
            xx_err_exit_c( ERR_CW_UNRECOGNIZED, macname );
            /* never return */
        }
    }
    g_scandata.s = scan_restart;
}


/***************************************************************************/
/*  logic for decision on skipping or processing line depending on         */
/*  the current state of the .if .th .el .do  controlwords encountered     */
/*                                                                         */
/*  The //comments refer to labels in the ASM code  see comment in wgml.c  */
/*  file cbt284.011                                                        */
/***************************************************************************/

#define DEBTESTPROC                     // to reduce test output don't define this

condcode    test_process( ifcb * cb )
{
    condcode    cc;

#ifdef DEBTESTPROC
    int     start_level = cb->if_level;

    if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass
        && cb->if_level ) {
        show_ifcb( "Anf teif", cb );
    }
#endif

    cc = CC_no;
//mainif
    if( cb->if_flags[cb->if_level].iflast   // 1. rec after .if
        && !cb->if_flags[cb->if_level].ifcwte) {// not .th or .el

        cb->if_flags[cb->if_level].iflast = false;  // reset first switch
        cb->if_flags[cb->if_level].ifthen = true;   // treat as then
    }

//mnif01
    if( cb->if_flags[cb->if_level].ifcwif ) {   // .if
//mnif03
        if( cb->if_flags[cb->if_level].ifthen
            || cb->if_flags[cb->if_level].ifelse ) {// object of .th or .el

            cc = CC_pos;
        } else {

//mnif03a
            while( cb->if_level > 0 ) { // pop one level
                cb->if_level--;
                if( cb->if_flags[cb->if_level].ifdo ) {
                    break;
                }
            }
            cc = CC_pos;                   // .do or all popped
        }

#ifdef DEBTESTPROC
        if( (input_cbs->fmflags & II_research)
          && GlobalFlags.firstpass
          && (start_level || cb->if_level) ) {
            show_ifcb( (cc == CC_pos) ? "EX1 pos" : "EX1 no", cb );
        }
#endif
        return( cc );

    } else {                            // not .if

//mnif01 cont.
        if( cb->if_flags[cb->if_level].ifcwdo ) {   // if  .do
            cc = CC_pos;
#ifdef DEBTESTPROC
        if( (input_cbs->fmflags & II_research)
          && GlobalFlags.firstpass
          && (start_level || cb->if_level) ) {
                show_ifcb( (cc == CC_pos) ? "Edo pos" : "Edo no", cb );
            }
#endif
            return( cc );
        }

        if( cb->if_flags[cb->if_level].ifthen
            || cb->if_flags[cb->if_level].ifelse ) {// object of .th or .el
//mnif05
            if( cb->if_flags[cb->if_level].ifelse ) {   // object of .el
//mnif06
                if( cb->if_flags[cb->if_level].iftrue ) {// omit if true for .el
//mnif08
                    cc = CC_neg;
                } else {
                    cc = CC_pos;
                }
            } else {
                if( cb->if_flags[cb->if_level].iffalse ) {// omit false for .th
                    cc = CC_neg;
                } else {
                    cc = CC_pos;
                }
            }
        } else {
            if( cb->if_flags[cb->if_level].ifcwte ) {
                cc = CC_pos;
            } else {
//mnif02
                while( cb->if_level > 0 ) {
                    cb->if_level--;
                    if( cb->if_flags[cb->if_level].ifdo ) {
//mnif05
                        if( cb->if_flags[cb->if_level].ifelse ) {// object of .el
//mnif06
                            if( cb->if_flags[cb->if_level].iftrue ) {
//mnif08
                                cc = CC_neg;   // omit if true for .el
                            } else {
                                cc = CC_pos;
                            }
                        } else {
                            if( cb->if_flags[cb->if_level].iffalse ) {
                                cc = CC_neg;   // omit false for .th
                            } else {
                                cc = CC_pos;
                            }
                        }
                        break;
                    }
                }
                if( cc == CC_no ) {        // not set then process record
                    cc = CC_pos;
                }
            }
        }
    }
    if( cc == CC_no ) {                    // cc not set program logic error
        g_if_int_err_exit();
        /* never return */
    }
#ifdef DEBTESTPROC
    if( (input_cbs->fmflags & II_research)
      && GlobalFlags.firstpass
      && (start_level || cb->if_level) ) {
        show_ifcb( (cc == CC_pos) ? "EX3 pos" : "EX3 no", cb );
    }
#endif
    return( cc );

}
#undef DEBTESTPROC


/***************************************************************************/
/*  first pass at script control words .if .th .el .do                     */
/*                                                                         */
/*  This is needed for routine test_process above                          */
/*                                                                         */
/***************************************************************************/

void set_if_then_do( ifcb * cb )
{
    char            *p;
    char            macname[MAC_NAME_LENGTH + 1];

    p = buff2;
    if( *p == SCR_char ) {              // only test script control words
        p++;
        if( (*p == SCR_char)  || (*p == '\'') ) {
            p++;                       // over ".." or ".'"
        }
        p = get_macro_name( p, macname );
        if( macname[0] == 'i' && macname[1] == 'f' ) {
            if( macname[2] == '\0' ) {
                cb->if_flags[cb->if_level].ifcwif = true;
            } else {
                cb->if_flags[cb->if_level].ifcwif = (find_macro( macro_dict, macname ) == NULL);
            }
        } else if( macname[0] == 'd' && macname[1] == 'o' ) {
            if( macname[2] == '\0' ) {
                cb->if_flags[cb->if_level].ifcwdo = true;
            } else {
                cb->if_flags[cb->if_level].ifcwdo = (find_macro( macro_dict, macname ) == NULL);
            }
        } else if( macname[0] == 't' && macname[1] == 'h'
          || macname[0] == 'e' && macname[1] == 'l' ) {
            if( macname[2] == '\0' ) {
                cb->if_flags[cb->if_level].ifcwte = true;
            } else {
                cb->if_flags[cb->if_level].ifcwte = (find_macro( macro_dict, macname ) == NULL);
            }
        }
    }
}


/*
 *  scan_line look whether input is script / gml control line or text
 *  special for attributes during :LAYOUT processing
 */

void    scan_line( void )
{
    condcode        cc;
    ifcb        *   cb;

    cb         = input_cbs->if_cb;
    g_scandata.s = buff2;
    g_scandata.e = buff2 + buff2_lg;

    if( !ProcFlags.literal ) {
        set_if_then_do( cb );
        cc = test_process( cb );
    } else {
        cc = CC_pos;
    }
    if( cc == CC_pos ) {                   // process record
        if( ProcFlags.scr_cw ) {
            scan_script();              // script control line
        } else if( ProcFlags.gml_tag ) {
            scan_gml();                 // GML tags
        }

        /*******************************************************************/
        /*  here we arrive if no script keyword / GML tag recognized       */
        /*  or for unprocessed text in the input record                    */
        /*******************************************************************/

        if( (*g_scandata.s != '\0') && (g_scandata.s < g_scandata.e) ) {
            if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass ) {
                g_info_lm( INF_TEXT_LINE, g_scandata.s );
            }
            if( ProcFlags.layout ) {    // LAYOUT active: should not happen
                internal_err_exit( __FILE__, __LINE__ );
                /* never return */
            }
            // processs (remaining) text
            if( rs_loc > 0 ) {
                start_doc_sect();   // if not already done
                // catch blank lines: not an error
                while( g_scandata.s < g_scandata.e ) {
                    if( (*g_scandata.s != ' ') && (*g_scandata.s != '\0') ) {
                        break;
                    }
                    g_scandata.s++;
                }
                if( g_scandata.s < g_scandata.e ) {
                    g_tag_rsloc_err_exit( rs_loc, g_scandata.s );
                    /* never return */
                }
            } else {

                /* This test skips blank lines at the top of xmp blocks inside macros */

                if( !(ProcFlags.skip_blank_line && (*g_scandata.s == ' ') &&
                        ((g_scandata.e - g_scandata.s) == 1) &&
                        (input_cbs->fmflags & II_file)) ) {
                    if( ProcFlags.force_pc ) {
                        do_force_pc( g_scandata.s );
                    } else {
                        process_text( g_scandata.s, g_curr_font );
                    }
                }
                ProcFlags.skip_blank_line = false;
            }
        }

        /*******************************************************************/
        /* For .co off or :xmp and the last part of the line just processed*/
        /* ensure the line is output                                       */
        /*******************************************************************/

        if( !ProcFlags.layout && (input_cbs->hidden_head == NULL) ) {
            if( !ProcFlags.concat &&
                ((input_cbs->fmflags & II_file) || (input_cbs->fmflags & II_macro)) ) {

                /*******************************************************************/
                /* This fixes a problem found when BX was implemented: when PA is  */
                /* used inside a box before a text line with CO OFF. But whether   */
                /* this is the best place or whether the restriction is needed     */
                /* remains to be determined                                        */
                /*******************************************************************/

                if( ProcFlags.in_bx_box && !ProcFlags.keep_left_margin) {
                    t_page.cur_width = g_indentl;
                }
                if( !ProcFlags.cont_char && !ProcFlags.para_has_text ) {
                    scr_process_break();
                }
            }
        }
    } else if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass ) {
        g_info_lm( INF_SKIP_LINE );     // show skipped line
    }
    if( ProcFlags.literal ) {
        if( li_cnt < INT_MAX ) {   // we decrement, do not wait for .li OFF
            if( li_cnt-- <= 0 ) {
                ProcFlags.literal = false;
            }
        }
    }
}


/***************************************************************************/
/*  search gml tag entry for given token                                   */
/*  This is for system (predefined) tags only                              */
/*  return ptr to entry if found, else NULL                                */
/***************************************************************************/

const gmltag *find_sys_tag( const char *tagname )
{
    int             k;
    unsigned        taglen;
    const gmltag    *tag;

    taglen = strlen( tagname );
    tag = gml_tags;
    for( k = 0; k < GML_TAGMAX; ++k ) {
        if( taglen == tag->taglen ) {
            if( strcmp( tag->tagname, tagname ) == 0 ) {
                return( tag );
            }
        }
        tag++;
    }
    return( NULL );                     // not found
}


/***************************************************************************/
/*  search gml layout tag entry for given token                            */
/*  This is for layout tags only                                           */
/*  return ptr to entry if found, else NULL                                */
/***************************************************************************/

const gmltag *find_lay_tag( const char *tagname )
{
    int             k;
    unsigned        taglen;
    const gmltag    *tag;

    taglen = strlen( tagname );
    tag = lay_tags;
    for( k = 0; k < LAY_TAGMAX; ++k ) {
        if( taglen == tag->taglen ) {
            if( strcmp( tag->tagname, tagname ) == 0 ) {
                return( tag );
            }
        }
        tag++;
    }
    return( NULL );                     // not found
}


/***************************************************************************/
/*  find gml tag entry by e_tag value and determine if is an ip_start_tag  */
/*  ip_start_tags are CIT, HPx, Q, SF                                      */
/*  return true if tag id is for an ip_start_tag, false otherwise          */
/*  NOTE: for some reason, an offset specified as, say "T_CIT" is actually */
/*        the tag id for the gmltag object for tag eCIT, hence the         */
/*        adjustment                                                       */
/***************************************************************************/

bool is_ip_tag( g_tags tag )
{
    if( tag == T_NONE ) {               // T_NONE is valid, but is not an ip_start_tag
        return( false );                // not found
    }
    return( (gml_tags[tag].tagclass & TCLS_ip_start) != 0 );
}

/***************************************************************************/
/*  returns a pointer to the start of the <text line> for the tag or of    */
/*  the next logical input record to be reprocessed                        */
/*  the line is not a <text line> if it starts with a tag (whether normal, */
/*  layout, or user-defined) or if it beginswith the current SCR_char      */
/*  (which can be any number of things: control word, macro, attribute,    */
/*  function, none of the above)                                           */
/*  ProcFlags.reprocess_line is set to true if this is not a <text_line>   */
/***************************************************************************/

char *get_text_line( char *p )
{
    bool            use_current = false;
    char            tagname[TAG_NAME_LENGTH + 1];
    char            *p1;
    const gtentry   *ge;                                        // GML user tag entry

    if( !ProcFlags.reprocess_line  ) {  // still on last line of tag
        SkipSpaces( p );                // skip initial spaces
        SkipDot( p );                   // possible tag end
        if( *p == '\0' ) {              // get new line
            while( *p == '\0' ) {
                if( (input_cbs->fmflags & II_eof) == 0 ) {
                    if( get_line( true ) ) {    // next line for text
                        process_line();
                        p = g_scandata.s;
                        continue;
                    }
                } else {
                    break;              // eof found
                }
            }
        } else {                        // use text following tag on same line
            use_current = true;
        }
    } else {
        ProcFlags.reprocess_line = false;
    }
    if( !use_current ) {                // not on same line as tag
        SkipSpaces( p );                // skip initial spaces
        if( *p != '\0' ) {              // text exists
            classify_record( p );       // sets ProcFlags used below if appropriate
            if( ProcFlags.scr_cw) {
                xx_err_exit( ERR_TEXT_NOT_TAG_CW );  // control word, macro, or whatever
                /* never return */
            }
            if( ProcFlags.gml_tag ) {
                p1 = check_tagname( p, tagname );
                if ( p1 != NULL && ( p1 - p - 1 ) <= TAG_NAME_LENGTH ) { // valid tag
                    if( ProcFlags.layout ) {
                        ge = NULL;      // no user tags within :LAYOUT
                    } else {
                        ge = find_user_tag( &tags_dict, tagname );
                    }
                    if( ge != NULL
                      || find_lay_tag( tagname ) != NULL
                      || find_sys_tag( tagname ) != NULL ) {
                        xx_err_exit( ERR_TEXT_NOT_TAG_CW );  // control word, macro, or whatever
                        /* never return */
                    }
                }
            }
        }
    }
    return( p );
}
