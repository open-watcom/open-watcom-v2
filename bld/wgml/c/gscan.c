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
* Description:  utility functions for wgml input line scanning
*
****************************************************************************/

#include "wgml.h"
#include "gvars.h"


/***************************************************************************/
/*    GML tags                                                             */
/***************************************************************************/

static const   gmltag  gml_tags[] = {
    #define pick( name, length, routine, gmlflags, locflags ) { #name, length, routine, gmlflags, locflags },
    #include "gtags.h"
    #undef pick
};


/***************************************************************************/
/*    GML layout tags                                                      */
/***************************************************************************/

static const   laytag  lay_tags[] = {
    #define pick( name, length, routine ) { #name, length, routine },
    #include "gtagslay.h"
    #undef pick
};


/***************************************************************************/
/*    SCR control words                                                    */
/***************************************************************************/
typedef enum {
    #define pick( name, routine, flags) SCR_TAG_##name,
    #include "gscrcws.h"
    #undef pick
    SCR_TAG_LABEL,
    SCR_TAGMAX
} scr_tag;

static  const   scrtag  scr_tags[] = {
    #define pick( name, routine, flags) { #name, routine, flags },
    #include "gscrcws.h"
    #undef pick
    { "..", scr_label, 0 },
};


static lay_tag      lay_ind = LAY_TAGMAX;   // index into lay_tab for attribute processing

const char *gml_tagname( gml_tag tag )
{
    return( gml_tags[tag].tagname );
}

const char *lay_tagname( lay_tag tag )
{
    return( lay_tags[tag].tagname );
}

void  lay_cmt( lay_tag tag )
{
    switch( tag ) {
    case LAY_TAG_CMT:
        gml_cmt( GML_TAG_CMT );
        break;
    default:
        scan_start = scan_stop;
        break;
    }
}

void  lay_include( lay_tag tag )
{
    switch( tag ) {
    case LAY_TAG_INCLUDE:
        gml_include( GML_TAG_INCLUDE );
        break;
    case LAY_TAG_IMBED:
        gml_include( GML_TAG_IMBED );
        break;
    default:
        scan_start = scan_stop;
        break;
    }
}

/***************************************************************************/
/*  scan for gml tags                                                      */
/***************************************************************************/

static void scan_gml( void )
{
    inputcb     *   cb;
    char        *   p;
    int             i;
    int             taglen;
    bool            processed;
    gtentry     *   ge;             // GML user tag entry
    mac_entry   *   me;             // script macro for processing GML tag
    char            linestr[MAX_L_AS_STR];
    char            tag_name[TAG_NAME_LENGTH + 1];

    cb = input_cbs;

    p = scan_start + 1;
    tok_start = scan_start;
    while( is_id_char( *p ) && p < scan_stop ) { // search end of TAG
        p++;
    }
    scan_start = p;                      // store argument start address
    taglen = p - tok_start - 1;
    if( taglen > TAG_NAME_LENGTH ) {
        char    *linestr1;

        linestr1 = malloc( taglen + 1 );
        memcpy( linestr1, tok_start + 1, taglen );
        linestr1[taglen] = '\0';
        err_count++;
        // SC--009 The tagname is too long
        if( cb->fmflags & II_tag_mac ) {
            ultoa( cb->s.m->lineno, linestr, 10 );
            g_err( err_tag_name, linestr1, linestr, "macro", cb->s.m->mac->name );
        } else {
            ultoa( cb->s.f->lineno, linestr, 10 );
            g_err( err_tag_name, linestr1, linestr, "file", cb->s.f->filename );
        }
        if( inc_level > 0 ) {
            show_include_stack();
        }
        free( linestr1 );
        scan_start = tok_start;         // process as text
        return;
    }

    for( i = 0; i < taglen; ++i ) {
        tag_name[i] = toupper( tok_start[i + 1] );
    }
    tag_name[i] = '\0';

    if( GlobalFlags.firstpass && cb->fmflags & II_research ) {

        if( taglen != 3 || memcmp( tag_name, "CMT", 3 ) ) {     // quiet for :cmt.

            if( cb->fmflags & II_tag_mac ) {
                printf_research( "L%d    %c%s tag found in macro %s(%d)\n\n",
                                 inc_level, GML_char, tag_name,
                                 cb->s.m->mac->name, cb->s.m->lineno );
            } else {
                printf_research( "L%d    %c%s tag found in file %s(%d)\n\n",
                                 inc_level, GML_char, tag_name,
                                 cb->s.f->filename, cb->s.f->lineno );
            }
        }
        add_GML_tag_research( tag_name );
    }

    if( ProcFlags.layout ) {
        ge = NULL;                      // no user tags within :LAYOUT
    } else {
        ge = find_tag( &tag_dict, tag_name );
    }
    processed = false;
    me = NULL;
    if( ge != NULL ) {                  // GML user defined Tag found
        if( ge->tagflags & tag_off ) {  // inactive, treat as text
            scan_start = tok_start;
            return;
        }
        me = find_macro( macro_dict, ge->macname );
        if( me == NULL ) {
            err_count++;
            // SC--037: The macro 'xxxxxx' for the gml tag 'yyyyy'
            //          is not defined
            if( cb->fmflags & II_tag_mac ) {
                ultoa( cb->s.m->lineno, linestr, 10 );
                g_err( err_tag_macro, ge->macname, ge->name,
                         linestr, "macro", cb->s.m->mac->name );
            } else {
                ultoa( cb->s.f->lineno, linestr, 10 );
                g_err( err_tag_macro, ge->macname, ge->name,
                         linestr, "file", cb->s.f->filename );
            }
            if( inc_level > 0 ) {
                show_include_stack();
            }
            scan_start = tok_start;         // process as text
            return;
        } else {

        /*******************************************************************/
        /*  The following is to prevent an endless loop                    */
        /*  Example from ow documentation:                                 */
        /*  .gt ZH1 add zh1                                                */
        /*  .gt H1 add zh1                                                 */
        /*  .dm zh1 begin                                                  */
        /*  ...                                                            */
        /*  :H1      <---- overridden gml tag                              */
        /*  ...                                                            */
        /*  .dm zh1 end                                                    */
        /*                                                                 */
        /*  we call the predefined :H1  instead                            */
        /*******************************************************************/

            if( (cb->fmflags & II_tag) && (cb->s.m->mac == me) ) {
                me = NULL;
            }
        }
    }
    if( me != NULL ) {                  // usertag and coresponding macro ok
        processed = process_tag( ge, me );
    } else {
        if( ProcFlags.layout ) {        // different tags within :LAYOUT
            lay_tag     ltag;
            for( ltag = 0; ltag < LAY_TAGMAX; ++ltag ) {
                if( taglen == lay_tags[ltag].taglen && !memcmp( lay_tags[ltag].tagname, tag_name, taglen ) ) {
                    lay_ind = LAY_TAGMAX;   // process tag not attribute

                    lay_tags[ltag].layproc( ltag );

                    processed = true;
                    lay_ind = ltag;           // now process attributes if any
                    if( *scan_start == '.' ) {
                        scan_start++;
                    }
                    break;
                }
            }
            if( !processed ) {          // check for gml only tag in :LAYOUT
                gml_tag     gtag;
                for( gtag = 0; gtag < GML_TAGMAX; ++gtag ) {
                    if( taglen == gml_tags[gtag].taglen && !memcmp( gml_tags[gtag].tagname, tag_name, taglen ) ) {
                        g_err( err_gml_in_lay, gml_tags[gtag].tagname );
                        err_count++;
                        file_mac_info();
                        processed = true;
                        scan_start = scan_stop;
                        break;
                    }
                }
            }
        } else {                        // not within :LAYOUT
            gml_tag     gtag;
            for( gtag = 0; gtag < GML_TAGMAX; ++gtag ) {
                if( taglen == gml_tags[gtag].taglen && !memcmp( gml_tags[gtag].tagname, tag_name, taglen ) ) {
                    if( GlobalFlags.firstpass && gtag == GML_TAG_LAYOUT && ProcFlags.fb_document_done  ) {
                        g_err( err_lay_too_late );
                        err_count++;
                        file_mac_info();
                        processed = true;
                        scan_start = scan_stop;
                        break;
                    }

                    if( (rs_loc == 0) && !ProcFlags.need_li_lp ) {
                        // no restrictions: do them all
                        gml_tags[gtag].gmlproc( gtag );
                    } else if( ProcFlags.need_li_lp &&
                            ((gml_tags[gtag].taglocs & li_lp_tag) != 0) ) {
                        // tag is LP or LI
                        gml_tags[gtag].gmlproc( gtag );
                    } else if( (gml_tags[gtag].taglocs & rs_loc) != 0 ) {
                        // tag allowed in this restricted location
                        gml_tags[gtag].gmlproc( gtag );
                    } else if( (gml_tags[gtag].tagflags & tag_is_general) != 0 ) {
                        // tag allowed everywhere
                        gml_tags[gtag].gmlproc( gtag );
                    } else {
                        start_doc_sect();   // if not already done
                        if( ProcFlags.need_li_lp ) {
                            xx_nest_err( err_no_li_lp );
                        } else {            // rs_loc > 0
                            g_err_tag_rsloc( rs_loc, tok_start );
                        }
                    }
                    processed = true;
                    if( *scan_start == '.' ) {
                        scan_start++;
                    }
                    break;
                }
            }
            if( !processed ) {         // check for layout tag in normal text
                lay_tag     ltag;
                for( ltag = 0; ltag < LAY_TAGMAX; ++ltag ) {
                    if( taglen == lay_tags[ltag].taglen ) {
                        if( !memcmp( lay_tags[ltag].tagname, tag_name, taglen ) ) {
                            g_err( err_lay_in_gml, lay_tags[ltag].tagname );
                            err_count++;
                            file_mac_info();
                            processed = true;
                            scan_start = scan_stop;
                            break;
                        }
                    }
                }
            }
        }
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
    bool            cwfound;

    cb = input_cbs;
    p = scan_start + 1;
    scan_restart = scan_start;

    if( (*p == '*') || !strnicmp( p, "CM ", 3 ) ) {
        scan_start = scan_stop;         // .CM  +++ ignore comment up to EOL
        return;                         // .*   +++ ignore comment up to EOL
    }

    if( *p == SCR_char && *(p+1) == SCR_char ) {
        pt = token_buf;
        *pt++ = SCR_char;               // special for ...label
        *pt++ = SCR_char;
        *pt   = '\0';
        me = NULL;
        scan_start = p + 2;
        toklen = 2;
    } else {
        if( *p == '\'' ) {                  // .'
            p++;
            ProcFlags.CW_sep_ignore = 1;
        } else {
            if( CW_sep_char == '\0') {
                ProcFlags.CW_sep_ignore = 1;// No separator char no split
            } else{
                ProcFlags.CW_sep_ignore = 0;
            }
            if( *p == SCR_char ) {          // ..
                p++;
                ProcFlags.macro_ignore = 1;
                me = NULL;
            } else {
                ProcFlags.macro_ignore = 0;
            }
        }
        if( ProcFlags.literal ) {       // no macro or split line if literal
            ProcFlags.CW_sep_ignore = 1;
            ProcFlags.macro_ignore = 1;
        }
        if( !ProcFlags.CW_sep_ignore ) { // scan line for CW_sep_char
            char    *   pchar;

            pchar = search_separator( buff2, CW_sep_char );

            if( pchar != NULL ) {
                if( *(pchar + 1) != '\0' ) {    // only split if more follows
                    split_input( buff2, pchar + 1, false );// ignore CW_sep_char
                }
                *pchar= '\0';               // delete CW_sep_char
                buff2_lg = strlen( buff2 ); // new length of first part
            }
        }

        scan_start = p;

        pt = token_buf;
        while( *p && is_macro_char( *p ) ) {  // end of controlword
           *pt++ = toupper( *p++ );         // copy uppercase to TokenBuf
        }
        *pt = '\0';

        toklen = pt - token_buf;

        if( *p && (*p != ' ') || toklen == 0 ) {// no valid script controlword / macro
//          if( !ProcFlags.literal ) {   // TBD
//             cw_err();
//          }
            scan_start = scan_restart;  // treat as text
            return;
        }

        if( toklen >= MAC_NAME_LENGTH ) {
            *(token_buf + MAC_NAME_LENGTH) = '\0';
        }
        if( !ProcFlags.macro_ignore ) {
            me = find_macro( macro_dict, token_buf );
        } else {
            me = NULL;
        }
    }

    if( me != NULL ) {                  // macro found
        if( GlobalFlags.firstpass && cb->fmflags & II_research ) {
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
        add_macro_parms( p );
        scan_restart = scan_stop;
    } else {                            // try script controlword
        cwfound = false;
        if( cb->fmflags & II_research && GlobalFlags.firstpass ) {
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

        if( toklen == SCR_KW_LENGTH ) {
            scr_tag stag;
            for( stag = 0; stag < SCR_TAGMAX; ++stag ) {
                if( !memcmp( scr_tags[stag].tagname, token_buf, SCR_KW_LENGTH ) ) {
#if 0
                    if( !ProcFlags.fb_document_done &&
                          scr_tags[stag].cwflags & cw_o_t ) {

                        /***************************************************/
                        /*  if this is the first cw  which produces output */
                        /* set page geometry and margins from layout       */
                        /***************************************************/
                        do_layout_end_processing();
                    }
#endif
                    if( !ProcFlags.layout && (scr_tags[stag].cwflags & cw_o_t) ) {

                        /********************************************************/
                        /* this is the first control word which produces output */
                        /* start the document, the layout is done               */
                        /* start_doc_sect() calls do_layout_end_processing()    */
                        /********************************************************/

                        start_doc_sect();
                    }
                    if( ProcFlags.literal  ) {  // .LI active
                        if( stag == SCR_TAG_LI ) { // .LI
                            scan_start = p;     // found, process
                            scr_tags[stag].tagproc();
                        }
                    } else {
                        scan_start = p; // script controlword found, process
                        if( scr_tags[stag].cwflags & cw_break ) {
                            scr_process_break();// output incomplete line, if any
                        }
                        scr_tags[stag].tagproc();
                    }
                    cwfound = true;
                    break;
                }
            }
        }
        if( !cwfound ) {
            cw_err();                   // unrecognized control word
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
    char        linestr[MAX_L_AS_STR];

#ifdef DEBTESTPROC
    int     start_level = cb->if_level;

    if( input_cbs->fmflags & II_research && GlobalFlags.firstpass
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
        if( input_cbs->fmflags & II_research && GlobalFlags.firstpass
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
        if( input_cbs->fmflags & II_research && GlobalFlags.firstpass
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
        if( input_cbs->fmflags & II_tag_mac ) {
            ultoa( input_cbs->s.m->lineno, linestr, 10 );
            g_err( err_if_intern, linestr, "macro", input_cbs->s.m->mac->name );
        } else {
            ultoa( input_cbs->s.f->lineno, linestr, 10 );
            g_err( err_if_intern,
                     linestr, "file", input_cbs->s.f->filename );
        }
        if( inc_level > 1 ) {
            show_include_stack();
        }
        err_count++;
    }
#ifdef DEBTESTPROC
    if( input_cbs->fmflags & II_research && GlobalFlags.firstpass
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
    char        cw_0;
    char        cw_1;
    char        c;

    if( *buff2 == SCR_char ) {          // only test script control words
        if( (*(buff2 + 1) == SCR_char)  ||  // ..CW
            (*(buff2 + 1) == '\'') ) {  // .'CW
            cw_0 = tolower( *(buff2 + 2) );// copy possible controlword
            cw_1 = tolower( *(buff2 + 3) );
            c = *(buff2 + 4);
        } else {                        // .CW
            cw_0 = tolower( *(buff2 + 1) );// copy possible controlword
            cw_1 = tolower( *(buff2 + 2) );
            c = *(buff2 + 3);
        }
        if( c == '\0' || c == ' ' ) {
            if( cw_0 == 'i' && cw_1 == 'f' ) {
                cb->if_flags[cb->if_level].ifcwif = true;
            } else if( cw_0 == 'd' && cw_1 == 'o' ) {
                cb->if_flags[cb->if_level].ifcwdo = true;
            } else if( cw_0 == 't' && cw_1 == 'h' || cw_0 == 'e' && cw_1 == 'l' ) {
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

    if( !ProcFlags.literal ) {
        set_if_then_do( cb );
        cc = test_process( cb );
    } else {
        if( !ProcFlags.ct ) {           // special for .ct .li construct
            if( (t_line != NULL) && (t_line->first != NULL) ) {
                scr_process_break();
            }
        }
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
        /*  or for attributes of LAYOUT tags                               */
        /*******************************************************************/

        if( (*scan_start != '\0') && (scan_start < scan_stop) ) {
            if( input_cbs->fmflags & II_research && GlobalFlags.firstpass ) {
                g_info_lm( inf_text_line, scan_start );
            }
            if( ProcFlags.layout ) {    // LAYOUT active: process attributes
                if( lay_ind < LAY_TAGMAX ) {
                    lay_tags[lay_ind].layproc( lay_ind );
                }
            } else {
                // processs (remaining) text
                if( rs_loc > 0 ) {
                    start_doc_sect();   // if not already done
                    g_err_tag_rsloc( rs_loc, scan_start );
                } else {
                    process_text( scan_start, g_curr_font );
                }
            }
        }

        /*******************************************************************/
        /* For .co off or :xmp and the last part of the line just processed*/
        /* ensure the line is output                                       */
        /*******************************************************************/

        if( !ProcFlags.layout && (input_cbs->fmflags & II_eol) ) {
            if( !ProcFlags.concat || ProcFlags.xmp_active ) {

                /*******************************************************************/
                /* This fixes a problem found when BX was implemented: when PA is  */
                /* used inside a box before a text line with CO OFF. But whether   */
                /* this is the best place or whether the restriction is needed     */
                /* remains to be determined                                        */
                /*******************************************************************/

                if( ProcFlags.in_bx_box ) {
                    g_cur_h_start = g_page_left_org + g_indent;
                }
                scr_process_break();
            }
        }
    } else if( input_cbs->fmflags & II_research && GlobalFlags.firstpass ) {
        g_info_lm( inf_skip_line );     // show skipped line
    }
    if( ProcFlags.literal ) {
        if( li_cnt < LONG_MAX ) {   // we decrement, do not wait for .li OFF
            if( li_cnt-- <= 0 ) {
                ProcFlags.literal = false;
            }
        }
        if( input_cbs->fmflags & II_eol ) {
            scr_process_break();        // ensure the line is output
        }
    }
}


/***************************************************************************/
/*  search gml tag entry for given token                                   */
/*  This is for system (predefined) tags only                              */
/*  return ptr to entry if found, else NULL                                */
/*                                                                         */
/***************************************************************************/

gmltag  const   *   find_sys_tag( char * token, size_t toklen )
{
    gml_tag gtag;

    for( gtag = 0; gtag < GML_TAGMAX; ++gtag ) {
        if( toklen == gml_tags[gtag].taglen ) {
            if( !stricmp( gml_tags[gtag].tagname, token ) ) {
                return( &gml_tags[gtag] );
            }
        }
    }
    return( NULL );                     // not found
}


/***************************************************************************/
/*  search gml layout tag entry for given token                            */
/*  This is for layout tags only                                           */
/*  return ptr to entry if found, else NULL                                */
/*                                                                         */
/***************************************************************************/

laytag  const   *find_lay_tag( char * token, size_t toklen )
{
    lay_tag ltag;

    for( ltag = 0; ltag < LAY_TAGMAX; ++ltag ) {
        if( toklen == lay_tags[ltag].taglen ) {
            if( !stricmp( lay_tags[ltag].tagname, token ) ) {
                return( &lay_tags[ltag] );
            }
        }
    }
    return( NULL );                     // not found
}
