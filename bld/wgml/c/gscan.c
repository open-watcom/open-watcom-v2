/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2010 The Open Watcom Contributors. All Rights Reserved.
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
    #define pickg( name, length, routine, gmlflags, locflags, classflags ) { #name, length, routine, gmlflags, locflags, classflags },
    #include "gtags.h"
    #undef pickg
    { "   ", 0, NULL, 0, 0 }            // end
};

#define GML_TAGMAX  (sizeof( gml_tags ) / sizeof( gml_tags[0] ) - 1)

/***************************************************************************/
/*    GML layout tags                                                      */
/***************************************************************************/

static const gmltag     lay_tags[] = {
    #define pick( name, length, routine, gmlflags, locflags ) { #name, length, routine, gmlflags, locflags },
    #include "gtagslay.h"
    #undef pick
    { "   ", 0, NULL, 0, 0 }            // end

};

#define LAY_TAGMAX  (sizeof( lay_tags ) / sizeof( lay_tags[0] ) - 1)


/***************************************************************************/
/*    SCRIPT control words                                                 */
/***************************************************************************/

static const scrtag     scr_kwds[] = {
    #define picks( name, routine, flags) { #name, routine, flags },
    #define picklab( name, routine, flags) { #name, routine, flags },
    #include "gscrcws.h"
    #undef picklab
    #undef picks
    { "  ", NULL, 0   }                 // end
};

#define SCR_KWDMAX  (sizeof( scr_kwds ) / sizeof( scr_kwds[0] ) - 1)

#define SCR_CW_LK_SIZE  (26 * 26)

static uint8_t  scr_lkup_tbl[26 * 26];
static uint8_t  scr_cw_label;
static uint8_t  scr_cw_hx;
static bool     scr_lkup_setup = false;

/***************************************************************************/
/*  Build a lookup table for SCRIPT control words. Since most of them have */
/*  a name consisting of two alphabetic characters, we can build a simple  */
/*  direct-lookup table that isn't obscenely large, and handle the few     */
/*  remaining control words as exceptions.                                 */
/***************************************************************************/

static void build_scr_cw_lookup( void )
{
    int             i;
    int             hash;
    const scrtag    *cw;

    // pre-fill lookup table with invalid values
    memset( scr_lkup_tbl, 0, sizeof( scr_lkup_tbl ) );

    // build a lookup table holding keyword table indices; note that
    // the indices are offset by one so that zero turns into an invalid
    // index (-1) during lookup.
    for( i = 0; i <= SCR_KWDMAX; ++i ) {
        cw = &scr_kwds[i];
        if( islower( cw->tagname[0] ) && islower( cw->tagname[1] ) ) {
            hash = (cw->tagname[0] - 'a') * 26 + (cw->tagname[1] - 'a');
            scr_lkup_tbl[hash] = i + 1;
        } else if( cw->tagname[0] == 'h' && cw->tagname[1] == '0' ) {
            hash = ('h' - 'a') * 26 + ('z' - 'a');  // fake it as .HZ
            scr_lkup_tbl[hash] = i + 1;
            scr_cw_hx = i;
        } else if( cw->tagname[0] == '.' && cw->tagname[1] == '.' ) {
            scr_cw_label = i;   // the ... label
        } else {
            // .H1 to .H9 -- ignored here
        }
    }
    scr_lkup_setup = true;
}


static int find_scr_cw( const char *str )
{
    int     hash;
    int     index = -1;

    if( !scr_lkup_setup )
        build_scr_cw_lookup();

    if( islower( str[0] ) && islower( str[1] ) ) {
        hash  = (str[0] - 'a') * 26 + (str[1] - 'a');
        index = scr_lkup_tbl[hash] - 1;
    } else if( str[0] == '.' && str[1] == '.' ) {
        index = scr_cw_label;
    } else if( str[0] == 'h' && isdigit( str[1] ) ) {
        index = scr_cw_hx + str[1] - '0';
    }

    return( index );
}


void set_overload( gtentry * in_gt )
{
    in_gt->overload = ( find_sys_tag( in_gt->name, strlen( in_gt->name ) ) != NULL );
}

/***************************************************************************/
/*  scan for gml tags                                                      */
/***************************************************************************/

static void scan_gml( void )
{
    inputcb         *cb;
    char            *p;
    int             toklen;
    int             k;
    char            csave;
    bool            processed;
    gtentry         *ge;                // GML user tag entry
    mac_entry       *me;                // script macro for processing GML tag
    char            tok_upper[BUF_SIZE];
    const gmltag    *tag;

    cb = input_cbs;

    p = scan_start + 1;
    tok_start = scan_start;
    while( (*p != ' ') && (*p != '.') && (*p != '\0') ) {   // search end of TAG
        p++;
    }
    toklen = p - tok_start - 1;

    /* If the token is longer than the maximum allowed tag name length,
     * it cannot be a valid tag name. Get out now so we don't have to watch
     * for token name buffer overflows.
     */
    if( toklen > TAG_NAME_LENGTH ) {
        return;
    }

    scan_start = p;                      // store argument start address
    csave = *p;
    *p = '\0';

    if( GlobalFlags.firstpass && (cb->fmflags & II_research) ) {

        if( stricmp( "cmt", tok_start + 1 ) != 0 ) {   // quiet for :cmt.

            if( cb->fmflags & II_tag_mac ) {
                printf_research( "L%d    %c%s tag found in macro %s(%d)\n\n",
                                 inc_level, GML_char, tok_start + 1,
                                 cb->s.m->mac->name, cb->s.m->lineno );
            } else {
                printf_research( "L%d    %c%s tag found in file %s(%d)\n\n",
                                 inc_level, GML_char, tok_start + 1,
                                 cb->s.f->filename, cb->s.f->lineno );
            }
        }
        add_GML_tag_research( tok_start + 1 );
    }

    if( ProcFlags.layout ) {
        ge = NULL;                      // no user tags within :LAYOUT
    } else {
        ge = find_user_tag( &tag_dict, tok_start + 1 );
    }
    processed = false;
    me = NULL;
    if( ge != NULL ) {                  // GML user defined Tag found
        if( ProcFlags.need_text ) {
            xx_err( err_text_not_tag_cw );
        }
        *p = csave;
        if( ge->tagflags & tag_off ) {  // inactive, treat as comment
            scan_start = scan_stop + 1;
            return;
        }
        me = find_macro( macro_dict, ge->macname );
        if( me == NULL ) {
            g_err_tag_mac( ge );
        } else {

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
    }
    if( me != NULL ) {                  // usertag and coresponding macro ok
        processed = process_tag( ge, me );
    } else {
        *p ='\0';
        for( k = 0; k <= toklen; k++ ) {
            tok_upper[k] = my_toupper( *(tok_start + 1 + k) );
        }
        tok_upper[k] = '\0';

        if( ProcFlags.layout ) {        // different tags within :LAYOUT
            tag = find_lay_tag( tok_upper, toklen );
            if( tag != NULL ) {
                *p = csave;
                lay_ind = -1;   // process tag not attribute

                if( rs_loc == 0 ) {
                    // no restrictions: do them all
                    tag->gmlproc( tag );
                } else if( tag->taglocs & rs_loc ) {
                    // tag allowed in this restricted location
                    tag->gmlproc( tag );
                } else if( (tag->tagflags & tag_is_general) != 0 ) {
                    // tag allowed everywhere
                    tag->gmlproc( tag );
                } else if( rs_loc == banner_tag ) {
                    xx_err_c( err_tag_expected, "eBANNER" );
                } else {    // rs_loc == banreg_tag
                    xx_err_c( err_tag_expected, "eBANREGION" );
                }
                processed = true;
                lay_ind = k;    // now process attributes if any
                SkipDot( scan_start );
            } else if( find_sys_tag( tok_upper, toklen ) != NULL ) {
                xx_err_c( err_gml_in_lay, tok_upper );
            }
        } else {                        // not within :LAYOUT
            tag = find_sys_tag( tok_upper, toklen );
            if( tag != NULL ) {
                if( GlobalFlags.firstpass
                  && strcmp( "LAYOUT", tok_upper ) == 0
                  && ProcFlags.fb_document_done ) {
                    xx_err( err_lay_too_late );
                }
                *p = csave;

                if( script_style.style != SCT_none ) {
                    scr_style_end();        // cancel BD, BI, US
                }

                ProcFlags.need_tag = false;

                /*******************************************************************/
                /*  When text occurs after certain blocks, it is processed as if   */
                /*  it were preceded by tag PC. This is cancelled when a tag comes */
                /*  before the text, but not if the tag starts or ends an inline   */
                /*  phrase or is an index tag (I1, I2, I3, IH1, IH2, IH3) or is    */
                /*  tag SET.                                                       */
                /*******************************************************************/

                if( (tag->tagclass & (ip_start_tag | ip_end_tag | index_tag)) == 0
                  && strcmp( "SET", tag->tagname ) != 0 ) {
                    ProcFlags.force_pc = false;
                }

                /*******************************************************************/
                /*  The Procflags must be cleared to prevent the error from being  */
                /*  reported for every tag until the proper end tag is found.      */
                /*  If the number of errors reported is limited at some point,     */
                /*  then those lines can be removed.                               */
                /*  The index tags (I1, I2, I3, IH1, IH2, IH3) are exceptions      */
                /*******************************************************************/

                if( ProcFlags.need_ddhd ) {
                    if( tag->tagclass & index_tag ) {
                        // tag is index tag
                        tag->gmlproc( tag );
                    } else if( tag->tagclass & def_tag ) {
                        // tag is DD, DDHD or GD
                        tag->gmlproc( tag );
                        ProcFlags.need_ddhd = false;
                    } else {
                        xx_err_c( err_tag_expected, "DDHD");
                    }
                } else if( ProcFlags.need_dd ) {
                    if( tag->tagclass & index_tag ) {
                        // tag is index tag
                        tag->gmlproc( tag );
                    } else if( tag->tagclass & def_tag ) {                                    // tag is DD, DDHD or GD
                        tag->gmlproc( tag );
                        ProcFlags.need_dd = false;
                    } else {
                        xx_err_c( err_tag_expected, "DD");
                    }
                } else if( ProcFlags.need_gd ) {
                    if( (tag->tagclass & index_tag) == 0 ) {
                        // tag is index tag
                        tag->gmlproc( tag );
                    } else if( tag->tagclass & def_tag ) {                                    // tag is DD, DDHD or GD
                        // tag is DD, DDHD or GD
                        tag->gmlproc( tag );
                        ProcFlags.need_gd = false;
                    } else {
                        xx_err_c( err_tag_expected, "GD");
                    }
                } else if( !nest_cb->in_list ) {
                    if( (tag->tagclass & list_tag) == 0 ) {
                        // tag is not a list tag
                        tag->gmlproc( tag );
                    } else {
                        xx_line_err_c( err_no_list, tok_start );
                    }
                } else if( ProcFlags.need_li_lp ) {
                    if( tag->tagclass & li_lp_tag ) {
                        // tag is LP or LI
                        tag->gmlproc( tag );
                    } else {
                        xx_nest_err( err_no_li_lp );
                    }
                } else if( ProcFlags.need_text ) {
                    xx_err( err_text_not_tag_cw );
                } else if( rs_loc == 0 ) {
                    // no restrictions: do them all
                    tag->gmlproc( tag );
                } else if( tag->taglocs & rs_loc ) {
                    // tag allowed in this restricted location
                    tag->gmlproc( tag );
                } else if( tag->tagflags & tag_is_general ) {
                    // tag allowed everywhere
                    tag->gmlproc( tag );
                } else {
                    start_doc_sect();   // if not already done
                    g_err_tag_rsloc( rs_loc, tok_start );
                }
                processed = true;
                SkipDot( scan_start );
            } else if( find_lay_tag( tok_upper, toklen ) != NULL ) {
                xx_err_c( err_lay_in_gml, tok_upper );
            }
        }
    }
    if( *p == '\0' ) {
        *p = csave;
    }
    if( !processed ) {                  // treat as text
        scan_start = tok_start;
    }
}


/*
 * search for (control word) separator in string outside of quotes
 *       returns ptr to sep char or NULL if not found
 *      quotes are single or double quotes only
 */

static char *   search_separator( char * str, char sep )
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
        return( str );
    } else {
        return( NULL );
    }
}


/*
 * Scan line with script control word
 *      uses scan_start ptr, but assumes this is in buff2
 */

static void     scan_script( void )
{
    inputcb     *   cb;
    mac_entry   *   me;
    char        *   p;
    char        *   pt;
    int             toklen;
    int             k;

    if( ProcFlags.need_text ) {
        xx_err( err_text_not_tag_cw );
    }

    cb = input_cbs;
    p = scan_start + 1;
    if( !ProcFlags.literal && (*p == '\0') ) {  // catch line with only "." in it
        if( ProcFlags.concat ) {
            *scan_start = ' ';                  // make line blank
        } else {
            scan_start = scan_stop + 1;         // treat as comment
        }
        return;
    }

    scan_restart = scan_start;

    if( *p == '*' ) {                       // early check for .*
        scan_start = scan_stop + 1;         // .* ignore comment up to EOL
        return;
    }

    if( *p == SCR_char && *(p+1) == SCR_char ) {
            pt = token_buf;
            *pt++ = SCR_char;               // special for ...label
            *pt++ = SCR_char;
            *pt   = '\0';
            me = NULL;
            scan_start++;
            toklen = 2;
    } else {
        if( *p == SCR_char ) {          // ..
            p++;
            ProcFlags.macro_ignore = true;
            me = NULL;
        } else {
            ProcFlags.macro_ignore = false;
        }
        if( *p == '\'' ) {                  // .' (or ..')
            p++;
            if( !ProcFlags.CW_force_sep ) {     // only change if not indented or all indents were .'
                ProcFlags.CW_sep_ignore = true;
            }
        } else {                            // no ': set per CW_sep_char
            if( CW_sep_char == '\0') {
                ProcFlags.CW_sep_ignore = true;// no separator char no split
            } else{
                ProcFlags.CW_sep_ignore = false;
            }
        }

        if( *p == '*' ) {                       // check for comment again; the following are
            scan_start = scan_stop + 1;         // all valid: .'* ..* ..'*
            return;
        }

        if( ProcFlags.literal ) {       // no macro or split line if literal
            ProcFlags.CW_sep_ignore = true;
            ProcFlags.macro_ignore = true;
        }
        if( !ProcFlags.CW_sep_ignore ) { // scan line for CW_sep_char
            char    *   pchar;

            pchar = search_separator( buff2, CW_sep_char );

            if( pchar != NULL ) {
                if( *(pchar + 1) != '\0' ) {    // only split if more follows
                    split_input( buff2, pchar + 1, II_none );   // split after CW_sep_char
                }
                *pchar= '\0';               // delete CW_sep_char
                buff2_lg = strlen( buff2 ); // new length of first part
            }
        }

        scan_start = p;

        pt = token_buf;
        toklen = 0;
        while( !is_space_tab_char( *p ) && (*p != '\0') ) {
           *pt++ = my_tolower( *p++ );      // copy lowercase to TokenBuf
           toklen++;
        }
        *pt = '\0';

        if( !ProcFlags.CW_sep_ignore &&
                ((*token_buf == '\0') || (*token_buf == ' ') || (toklen == 0)) ) {
            // no valid script controlword / macro, treat as text
            scan_start = scan_restart;
            return;
        }

        if( !ProcFlags.macro_ignore ) {
            me = find_macro( macro_dict, token_buf );
        } else {
            me = NULL;
        }
    }

    if( me != NULL ) {                  // macro found
        if( GlobalFlags.firstpass && (cb->fmflags & II_research) ) {
            if( cb->fmflags & II_tag_mac ) {
                printf_research( "L%d    %c%s macro found in macro %s(%d)\n\n",
                                 inc_level, SCR_char, token_buf,
                                 cb->s.m->mac->name, cb->s.m->lineno );
            } else {
                printf_research( "L%d    %c%s macro found in file %s(%d)\n\n",
                                 inc_level, SCR_char, token_buf,
                                 cb->s.f->filename, cb->s.f->lineno );
            }
            add_SCR_tag_research( token_buf );
        }
        add_macro_cb_entry( me, NULL );
        inc_inc_level();
        if( *p == '\0' ) {
            add_macro_parms( p );
        } else {
            add_macro_parms( p + 1 );
        }
        scan_restart = scan_stop + 1;
    } else if( !ProcFlags.literal ) {   // try script controlword if not in LI
        scan_start += SCR_KW_LENGTH;
        p = scan_start;
        if( (cb->fmflags & II_research) && GlobalFlags.firstpass ) {
            if( cb->fmflags & II_tag_mac ) {
                printf_research( "L%d    %c%s CW found in macro %s(%d)\n\n",
                                 inc_level, SCR_char, token_buf,
                                 cb->s.m->mac->name, cb->s.m->lineno );
            } else {
                printf_research( "L%d    %c%s CW found in file %s(%d)\n\n",
                                 inc_level, SCR_char, token_buf,
                                 cb->s.f->filename, cb->s.f->lineno );
            }
            add_SCR_tag_research( token_buf );
        }

        if( !token_buf[0] ) {   // lone . or .' -- ignored
            scan_start = scan_stop + 1;
            return;
        }

        k = find_scr_cw( token_buf );               // non-negative if valid
        if( k >= 0 ) {
            if( !ProcFlags.layout
              && !ProcFlags.fb_document_done
              && (scr_kwds[k].cwflags & cw_o_t) ) {

                /********************************************************/
                /* this is the first control word which produces output */
                /* start the document, the layout is done               */
                /* start_doc_sect() calls do_layout_end_processing()    */
                /********************************************************/

                start_doc_sect();
            }
            ProcFlags.CW_noblank = false;           // blank after CW is default
            if( ProcFlags.literal  ) {              // .li active
                if( strcmp( "li", token_buf ) == 0 ) {  // .li
                    ProcFlags.CW_noblank = (*p != ' ');
                    scan_start = p; // found, process
                    scr_kwds[k].tagproc();
                }
            } else {
                scan_start = p; // script controlword found, process
                if( scr_kwds[k].cwflags & cw_break ) {
                    ProcFlags.force_pc = false;
                    scr_process_break();// output incomplete line, if any
                }
                ProcFlags.CW_noblank = (*p != ' ');
                scr_kwds[k].tagproc();
            }
        } else {
            xx_err_c( err_cw_unrecognized, token_buf );
        }
    }
    scan_start = scan_restart;
}


/***************************************************************************/
/*  logic for decision on skipping or processing line depending on         */
/*  the current state of the .if .th .el .do  controlwords encountered     */
/*                                                                         */
/*  The //comments refer to labels in the ASM code  see comment in wgml.c  */
/*  file cbt284.011                                                        */
/***************************************************************************/

#define DEBTESTPROC             // to reduce test output don't define this

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

    cc = no;
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

            cc = pos;
        } else {

//mnif03a
            while( cb->if_level > 0 ) { // pop one level
                cb->if_level--;
                if( cb->if_flags[cb->if_level].ifdo ) {
                    break;
                }
            }
            cc = pos;                   // .do or all popped
        }

#ifdef DEBTESTPROC
        if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass
            && (start_level || cb->if_level) ) {
            char * txt = (cc == pos ? "EX1 pos" : "EX1 no" );

            show_ifcb( txt, cb );
        }
#endif
        return( cc );

    } else {                            // not .if

//mnif01 cont.
        if( cb->if_flags[cb->if_level].ifcwdo ) {   // if  .do
            cc = pos;
#ifdef DEBTESTPROC
        if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass
                && (start_level || cb->if_level) ) {
                char * txt = (cc == pos ? "Edo pos" : "Edo no" );

                show_ifcb( txt, cb );
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
                    cc = neg;
                } else {
                    cc = pos;
                }
            } else {
                if( cb->if_flags[cb->if_level].iffalse ) {// omit false for .th
                    cc = neg;
                } else {
                    cc = pos;
                }
            }
        } else {
            if( cb->if_flags[cb->if_level].ifcwte ) {
                cc = pos;
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
                                cc = neg;   // omit if true for .el
                            } else {
                                cc = pos;
                            }
                        } else {
                            if( cb->if_flags[cb->if_level].iffalse ) {
                                cc = neg;   // omit false for .th
                            } else {
                                cc = pos;
                            }
                        }
                        break;
                    }
                }
                if( cc == no ) {        // not set then process record
                    cc = pos;
                }
            }
        }
    }
    if( cc == no ) {                    // cc not set program logic error
        g_err_if_int();
    }
#ifdef DEBTESTPROC
    if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass
        && (start_level || cb->if_level) ) {
        char * txt = (cc == pos ? "EX3 pos" : "EX3 no" );

        show_ifcb( txt, cb );
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
    char            cw[9];
    char        *   p;
    char        *   pb;
    uint32_t        len;

    len = 0;
    p = cw;
    pb = buff2;
    if( *pb == SCR_char ) {              // only test script control words
        pb++;
        if( (*pb == SCR_char)  || (*pb == '\'') ) {
            pb++;                       // over ".." or ".'"
        }
        while( len < MAC_NAME_LENGTH ) {
            if( is_space_tab_char( *pb ) || (*pb == '\0') ) { // largest possible macro/cw
                break;
            }
           *p++ = my_tolower( *pb++ );      // copy lowercase to TokenBuf
           len++;
        }
        *p = '\0';
        if( !strncmp( cw, "if", SCR_KW_LENGTH ) ) {
            if( len > SCR_KW_LENGTH ) {
                cb->if_flags[cb->if_level].ifcwif = (find_macro( macro_dict, cw ) == NULL);
            } else {
                cb->if_flags[cb->if_level].ifcwif = true;
            }
        } else if( !strncmp( cw, "do", SCR_KW_LENGTH ) ) {
            if( len > SCR_KW_LENGTH ) {
                cb->if_flags[cb->if_level].ifcwdo = (find_macro( macro_dict, cw ) == NULL);
            } else {
                cb->if_flags[cb->if_level].ifcwdo = true;
            }
        } else if( !strncmp( cw, "th", SCR_KW_LENGTH ) || !strncmp( cw, "el", SCR_KW_LENGTH ) ) {
            if( len > SCR_KW_LENGTH ) {
                cb->if_flags[cb->if_level].ifcwte = (find_macro( macro_dict, cw ) == NULL);
            } else {
                cb->if_flags[cb->if_level].ifcwte = true;
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
    scan_start = buff2;
    scan_stop  = buff2 + buff2_lg;

    if( !ProcFlags.literal ) {
        set_if_then_do( cb );
        cc = test_process( cb );
    } else {
        cc = pos;
    }
    if( cc == pos ) {                   // process record
        if( ProcFlags.scr_cw ) {
            scan_script();              // script control line

        } else if( ProcFlags.gml_tag ) {
            scan_gml();                 // GML tags

        }

        /*******************************************************************/
        /*  here we arrive if no script keyword / GML tag recognized       */
        /*  or for unprocessed text in the input record                    */
        /*******************************************************************/

        if( (*scan_start != '\0') && (scan_start <= scan_stop) ) {
            if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass ) {
                g_info_lm( inf_text_line, scan_start );
            }
            if( ProcFlags.layout ) {    // LAYOUT active: should not happen
                internal_err( __FILE__, __LINE__ );
            } else {
                // processs (remaining) text
                if( rs_loc > 0 ) {
                    start_doc_sect();   // if not already done
                    // catch blank lines: not an error
                    while( scan_start < scan_stop ) {
                        if( (*scan_start != ' ') && (*scan_start != '\0') ) {
                            break;
                        }
                        scan_start++;
                    }
                    if( scan_start < scan_stop ) {
                        g_err_tag_rsloc( rs_loc, scan_start );
                    }
                } else {

                    /* This test skips blank lines at the top of xmp blocks inside macros */

                    if( !(ProcFlags.skip_blank_line && (*scan_start == ' ') &&
                            ((scan_stop - scan_start) == 1) &&
                            (input_cbs->fmflags & II_file)) ) {
                        if( ProcFlags.force_pc ) {
                            do_force_pc( scan_start );
                        } else {
                            process_text( scan_start, g_curr_font );
                        }
                    }
                    ProcFlags.skip_blank_line = false;
                }
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
                    t_page.cur_width = g_indent;
                }
                if( !ProcFlags.cont_char && !ProcFlags.para_has_text ) {
                    scr_process_break();
                }
            }
        }
    } else if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass ) {
        g_info_lm( inf_skip_line );     // show skipped line
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

const gmltag *find_sys_tag( char *token, size_t toklen )
{
    int k;

    for( k = 0; k < GML_TAGMAX; ++k ) {
        if( toklen == gml_tags[k].taglen ) {
            if( stricmp( gml_tags[k].tagname, token ) == 0 ) {
                return( &gml_tags[k] );
            }
        }
    }
    return( NULL );                     // not found
}


/***************************************************************************/
/*  search gml layout tag entry for given token                            */
/*  This is for layout tags only                                           */
/*  return ptr to entry if found, else NULL                                */
/***************************************************************************/

const gmltag *find_lay_tag( char *token, size_t toklen )
{
    int k;

    for( k = 0; k < LAY_TAGMAX; ++k ) {
        if( toklen == lay_tags[k].taglen ) {
            if( stricmp( lay_tags[k].tagname, token ) == 0 ) {
                return( &lay_tags[k] );
            }
        }
    }
    return( NULL );                     // not found
}


/***************************************************************************/
/*  find gml tag entry by e_tag value and determine if is an ip_start_tag  */
/*  ip_start_tags are CIT, HPx, Q, SF                                      */
/*  return true if offset is for an ip_start_tag, false otherwise          */
/*  NOTE: for some reason, an offset specified as, say "t_CIT" is actually */
/*        the offset for the gmltag object for tag eCIT, hence the         */
/*        adjustment                                                       */
/***************************************************************************/

bool is_ip_tag( e_tags tag )
{
    if( (tag < t_NONE) || (tag >= t_MAX) ) {  // catch invalid offset values
        internal_err( __FILE__, __LINE__ );
    } else if( tag != t_NONE ) {                 // t_NONE is valid, but is not an ip_start_tag
        return( gml_tags[tag - 1].tagclass & ip_start_tag );
    }
    return( false );                                // not found
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

char * get_text_line( char * p )
{
    bool            use_current = false;
    char            *tok_start = NULL;
    char            tok_txt[TAG_NAME_LENGTH + 1];
    gtentry         *ge;                                        // GML user tag entry
    size_t          toklen;

    if( !ProcFlags.reprocess_line  ) {  // still on last line of tag
        SkipSpaces( p );                // skip initial spaces
        SkipDot( p );                   // possible tag end
        if( *p == '\0' ) {              // get new line
            while( *p == '\0' ) {
                if( !(input_cbs->fmflags & II_eof) ) {
                    if( get_line( true ) ) {    // next line for text
                        process_line();
                        scan_start = buff2;
                        scan_stop  = buff2 + buff2_lg;
                        p = scan_start;
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
            classify_record( *p );      // sets ProcFlags used below if appropriate
            if( ProcFlags.scr_cw) {
                xx_err( err_text_not_tag_cw );  // control word, macro, or whatever
            } else if( ProcFlags.gml_tag ) {
                p++;
                tok_start = p;
                while( is_id_char( *p ) && p <= scan_stop ) {   // find end of TAG
                    p++;
                }
                toklen = p - tok_start;
                if( toklen <= TAG_NAME_LENGTH ) {    // possible tag
                    strncpy( &tok_txt, tok_start, toklen );
                    tok_txt[toklen] = '\0';
                    if( ProcFlags.layout ) {
                        ge = NULL;                  // no user tags within :LAYOUT
                    } else {
                        ge = find_user_tag( &tag_dict, tok_txt );
                    }
                    if( ge != NULL
                      || find_lay_tag( tok_txt, toklen ) != NULL
                      || find_sys_tag( tok_txt, toklen ) != NULL ) {
                        xx_err( err_text_not_tag_cw );  // control word, macro, or whatever
                    }
                }
            }
        }
    }

    return( p );
}

