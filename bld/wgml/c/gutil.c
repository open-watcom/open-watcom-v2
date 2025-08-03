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
* Description:  some WGML utility functions
*
****************************************************************************/


#include "wgml.h"
#include <errno.h>

#include "clibext.h"


#define CHECK_WS(c,t)   (((c) == ' ') || (t) && ((c) == '\t'))

/***************************************************************************/
/*  parses in_su->su_txt to complete initialization of in_su               */
/*  Notes:                                                                 */
/*      att_val_to_su() uses this function to convert all values,          */
/*          including integers without unit designators                    */
/*      cw_val_to_su() uses this function to convert values with unit      */
/*          designators only                                               */
/*                                                                         */
/*  conversion routines for Horizontal / Vertical space units              */
/*  Accepted formats:                                                      */
/*       1234        integer (chars / lines) (no decimal digits allowed)   */
/*       1234m       Ems (no digitsare allowed after the "m")              */
/*       1234.12Dv   Device units (decimal digits are ignored)             */
/*       1234.12i    inch             (1 inch = 2.54cm)                    */
/*       1234C1234   Cicero  + points (12 points = 1C = 1/6 inch)          */
/*       1234p1234   Pica    + points (12 points = 1P = 1/6 inch)          */
/*       1234.12cm   centimeter                                            */
/*       1234.12mm   millimeter                                            */
/*                                                                         */
/*    the absolute units (the last 5) will be stored                       */
/*    in 0.0001 millimeter units and 0.0001 inch units,                    */
/*    the relative ones (the first 3) will not be converted.               */
/*                                                                         */
/*    the exact definition of Cicero and Pica (points) differs between     */
/*    Europe and the USA                                                   */
/*                                                                         */
/*    note that wgml 4.0 has at least 3 "inches":                          */
/*      whatever it uses for Cicero                                        */
/*      whatever it uses for Pica                                          */
/*      whatever it uses for, in particular, the base units                */
/*                                                                         */
/*    this code matches wgml 4.0 as closely as possible, which is why some */
/*      of the conversions may not match the official values seen on the   */
/*      Internet!                                                          */
/*                                                                         */
/*    returns cvterr: false on success (no conversion error)               */
/*                    true on error (conversion error occurred)            */
/***************************************************************************/

static const bool internal_to_su( su *in_su, bool tag, const char *base )
{
    bool            cvterr  = true;
    bool            is_cp   = false;
    char            *pd     = NULL;     // ptr to decimal point
    char            *pd1    = NULL;     // ptr to first decimal digit
    char            *pdn    = NULL;     // ptr to last digit +1
    char            *ps;                // destination for value text
    char            sign;
    char            unit[4];
    int             i;
    ldiv_t          div;
    int             k;
    int             wh;
    int             wd;

    ps = in_su->su_txt;

    /********************************************************************/
    /* extract and skip the sign, if present                            */
    /* att_val_to_su() and scr_val_to_su() deal with rejecting values   */
    /* formed incorrectly with regard to initial signs and to setting   */
    /* su_relative appropriately before invoking internal_to_su()       */
    /* lay_init_su() presupposes that the value has no initial sign     */
    /********************************************************************/

    if( (*ps == '+')
      || (*ps == '-') ) {
        sign = *ps;
        ps++;
    } else {
        sign = '+';
    }

    wh = 0;
    for( i = 0; i < 4 && my_isdigit( *ps ); i++ ) { // max four digits in whole part
        wh = (10 * wh) + (*ps - '0');
        ps++;
    }
    if( my_isdigit( *ps ) ) {           // too many digits in whole part
        val_parse_err_exit( base + (ps - in_su->su_txt), tag );
        /* never return */
    }

    wd = 0;
    if( *ps == '.' ) {                  // check for decimal point
        pd = ps++;
        pd1 = ps;                       // remember start of decimals
        for( i = 0; i < 2 && my_isdigit( *ps ); i++ ) { // max two digits in decimals
            wd = 10 * wd + *ps - '0';
            ps++;
        }
        if( my_isdigit( *ps ) ) {       // too many digits in decimals
            val_parse_err_exit( base + (ps - in_su->su_txt), tag );
            /* never return */
        }
        pdn = ps;
        if( pd1 == ps ) {               // no decimals
            pd1 = NULL;
            pdn = NULL;
        }
    }

    /***********************************************************************/
    /*  get max two characters in unit                                     */
    /***********************************************************************/

    unit[3] = '\0';
    unit[2] = '\0';
    unit[1] = '\0';
    unit[0] = '\0';
    if( my_isalpha( *ps ) )             // check first
        unit[0] = *ps++;                // save Unit
    if( my_isalpha( *ps ) )             // check second
        unit[1] = *ps++;                // save Unit
    if( my_isalpha( *ps ) ) {           // too many characters in unit
        val_parse_err_exit( base + (ps - in_su->su_txt), tag );
        /* never return */
    }

    /***********************************************************************/
    /*  check for valid unit                                               */
    /***********************************************************************/

    if( unit[1] == '\0' ) {             // single letter unit
        switch( unit[0] ) {
        case 'i' :
            in_su->su_u = SU_inch;
            break;
        case 'm' :
            in_su->su_u = SU_ems;
            if( pd != NULL ) {          // no decimals with "M"
                val_parse_err_exit( base + (ps - in_su->su_txt), tag );
                /* never return */
            }
            break;
        case 'c' :
            in_su->su_u = SU_cicero;
            is_cp = true;
            break;
        case 'p' :
            in_su->su_u = SU_pica;
            is_cp = true;
            break;
        case '\0' :                     // no unit is characters or lines
            in_su->su_u = SU_chars_lines;
            break;
        default:
            val_parse_err_exit( base + (ps - in_su->su_txt), tag );
            /* never return */
        }
    } else {                            // two letter unit
        if( unit[1] == 'm' ) {          // cm, mm ?
            if( unit[0] == 'c' ) {
                in_su->su_u = SU_cm;
            } else if( unit[0] == 'm' ) {
                in_su->su_u = SU_mm;
            } else {                    // invalid unit
                val_parse_err_exit( base + (ps - in_su->su_txt), tag );
                /* never return */
            }
        } else if( unit[0] == 'd' ) {   // dv ?
            if( unit[1] == 'v' ) {
                in_su->su_u = SU_dv;
            } else {                    // invalid unit
                val_parse_err_exit( base + (ps - in_su->su_txt), tag );
                /* never return */
            }
        } else {                        // invalid unit
            val_parse_err_exit( base + (ps - in_su->su_txt), tag );
            /* never return */
        }
    }

    if( is_cp ) {                       // "c" and "p" can be followed by max four digits
        for( i = 0; i < 4 && my_isdigit( *ps ); i++ ) {
            wd = (10 * wd) + (*ps - '0');
            ps++;
        }
        if( my_isdigit( *ps ) ) {       // too many digits after "c" or "p"
            val_parse_err_exit( base + (ps - in_su->su_txt), tag );
            /* never return */
        }
    }

    if( *ps != '\0' ) {                 // value continues on: it shouldn't
        val_parse_err_exit( base + (ps - in_su->su_txt), tag );
        /* never return */
    }
    in_su->su_whole = wh;
    in_su->su_dec   = wd;

    if( pd != NULL ) {                  // dec point found
        if( unit[0] == '\0' ) {         // need trailing unit
            val_parse_err_exit( base + (ps - in_su->su_txt - 1), tag );
            /* never return */
        }
    }

    in_su->su_inch = 0;
    in_su->su_mm   = 0;
    k = 1;
    if( pd1 != NULL ) {
        if( pdn - pd1 == 1 ) {
            k = 10;                     // only 0.1 digit
        }
    }
    switch( in_su->su_u ) {
    // the relative units are only stored, not converted
    case SU_chars_lines :
    case SU_ems :
    case SU_dv :
        break;
    case SU_inch :                      // inch, cm, mm valid with decimals
        in_su->su_mm   = (wh * 100L + wd * k) * 2540L;
        in_su->su_inch = (wh * 100L + wd * k) *  100L;
        break;
    case SU_cm :
        in_su->su_mm   = (wh * 100L + wd * k) * 1000L;
        in_su->su_inch = in_su->su_mm * 10L / 254L;
        break;
    case SU_mm :
        in_su->su_mm   = (wh * 100L + wd * k) *  100L;
        in_su->su_inch = in_su->su_mm * 10L / 254L;
        break;
    case SU_cicero :                    // cicero
        if( wd > 11 ) {
            div = ldiv( wd, 12L);
            wh += div.quot;
            wd = div.rem;
        }
        in_su->su_inch = wh * 10000L / 6L + wd * 10000L / 72L;
        in_su->su_inch = (int64_t)in_su->su_inch * 10656L / 10000L;
        in_su->su_mm = in_su->su_inch * 254L / 10L;
        break;
    case SU_pica :                      // pica
        if( wd > 11 ) {
            div = ldiv( wd, 12L);
            wh += div.quot;
            wd = div.rem;
        }
        in_su->su_inch = wh * 10000L / 6L + wd * 10000L / 72L;
        in_su->su_inch = (int64_t)in_su->su_inch * 9978L / 10000L;
        in_su->su_mm = in_su->su_inch * 254L / 10L;
        break;
    default:
        break;
    }
    if( sign == '-' ) {
        in_su->su_inch  = -in_su->su_inch;
        in_su->su_mm    = -in_su->su_mm;
        in_su->su_whole = -in_su->su_whole;
    }

    cvterr = false;
    return( cvterr );
}

/***************************************************************************/
/*  fills in "whole part" value if in_su->su_txt contains an integer value */
/*      (including an expression)                                          */
/*  returns true if an integer value or expression was found               */
/*          false otherwise (not necessarily an error)                     */
/***************************************************************************/

static bool su_expression( su *in_su )
{
    bool                retval  = true;
    condcode            cc;
    getnum_block        gn;

    gn.arg.e = gn.arg.s = in_su->su_txt;
    while( *gn.arg.e != '\0' )
        gn.arg.e++;
    gn.ignore_blanks = false;
    cc = getnum( &gn );

    if( cc == CC_notnum ) {
        retval = false;
    } else {
        in_su->su_u = SU_chars_lines;
        in_su->su_whole = gn.result;
        in_su->su_dec = 0;
        in_su->su_inch = 0;
        in_su->su_mm = 0;
    }

    return( retval );
}

/***************************************************************************/
/*  insert space characters into t_element as if they were text            */
/*  when concatenation is on, this is done at the end of the last          */
/*    text_line, in case of wrapping                                       */
/*    t_element must be a text element                                     */
/*  when concatenation is off, a new doc_element must be created           */
/*  NOTE: invoked by add_dt_space()                                        */
/*        used with items affected by DT/DD used inside a macro            */
/***************************************************************************/

static void add_spaces_t_element( char * spaces )
{
    font_number     font;
    unsigned        spc_cnt;
    text_chars  *   sav_chars;
    text_line   *   line;
    unsigned        start;

    spc_cnt = strlen( spaces );
    if( spc_cnt > 0 ) {                                 // must have something to add
        if( t_element != NULL ) {                       // t_element exists
            if( t_element->type != ELT_text ) {          // must be text
                internal_err_exit( __FILE__, __LINE__ );
                /* never return */
            }
            for( line = t_element->element.text.first; line->next != NULL; line = line->next )
                /* empty */;
            font = line->last->font;
            start = line->last->x_address + line->last->width;
            sav_chars = line->last;
            line->last->next = process_word( layout_work.note.spaces, spc_cnt, font, false );
            line->last = line->last->next;
            line->last->prev = sav_chars;
            line->last->type = TXT_norm;
            line->last->x_address = start;

            if( wgml_fonts[font].line_height > line->line_height ) {
                line->line_height = wgml_fonts[font].line_height;
            }
        } else {                                        // too complicated
            xx_line_warn_c( WNG_HDREF_CO_OFF, buff2 );
        }
    }
    return;
}

/***************************************************************************/
/* add space to DT text and re-evaluate DD text start position             */
/* Note: only called when DT/DD were used in a macro, and then only by     */
/*       those items that react to that event                              */
/***************************************************************************/

void add_dt_space( void )
{
    if( t_line == NULL ) {
        add_spaces_t_element( " " );            // DD text will be on new line
    } else {                                    // will need to re-evaluate DD text position
        ProcFlags.dd_space = false;             // reset context
        t_page.cur_width = t_line->last->x_address + t_line->last->width;
        insert_hard_spaces( " ", 1, t_line->last->font );
        if( t_page.cur_width + wgml_fonts[layout_work.dd.font].spc_width < t_page.cur_left ) {  // set for current line
            t_page.cur_width = t_page.cur_left;
            ProcFlags.zsp = true;
        } else if( nest_cb->dl_break ) {
            if( t_line != NULL ) {      // break not previously applied because only needed now
                process_line_full( t_line, ((ProcFlags.justify != JUST_off) &&
                    (ProcFlags.justify != JUST_on) && (ProcFlags.justify != JUST_half)) );
                t_line = NULL;              // commit term but as part of same doc_element as definition
                t_page.cur_width = t_page.cur_left;
                post_space = 0;
            }
        } else {                        // cur_width > cur_left and no break
            ProcFlags.dd_space = true;
            ProcFlags.zsp = false;
        }
    }
    return;
}

/***************************************************************************/
/* return length of string without trailing spaces                         */
/* return 1 for all blank string                                           */
/***************************************************************************/

unsigned len_to_trail_space( const char *p , unsigned len )
{
    while( (len > 0) && (p[--len] == ' ') )
        /* empty */;
    len++;
    if( len < 1 )
        len = 1;
    return( len );
}

/***************************************************************************/
/* find matching quote char, if present                                    */
/* will point to \0 if quote char not found                                */
/***************************************************************************/

char * skip_to_quote( char * p, char quote )
{
    while( *p != '\0' && quote != *p ) {
        p++;
    }
    return( p + 1 );
}

/***************************************************************************/
/*  extension for layout :BANREGION indent, hoffset and width attributes:  */
/*      symbolic units without a numeric value                             */
/*  returns true if an extended attribute value was found                  */
/*          false otherwise (not necessarily an error)                     */
/***************************************************************************/
static bool su_layout_special( su *in_su )
{
    if( strcmp( "left", in_su->su_txt ) == 0 ) {
        in_su->su_u = SU_lay_left;
    } else if( strcmp( "right", in_su->su_txt ) == 0 ) {
        in_su->su_u = SU_lay_right;
    } else if( strcmp( "center", in_su->su_txt ) == 0 ) {
        in_su->su_u = SU_lay_centre;
    } else if( strcmp( "centre", in_su->su_txt ) == 0 ) {
        in_su->su_u = SU_lay_centre;
    } else if( strcmp( "extend", in_su->su_txt ) == 0 ) {
        in_su->su_u = SU_lay_extend;
    } else {
        return( false );
    }
    in_su->su_whole    = 0;
    in_su->su_dec      = 0;
    in_su->su_inch     = 0;
    in_su->su_mm       = 0;
    return( true );
}

/***************************************************************************/
/*  initializes in_su->su_txt using attr_val->name/attr_val->len   */
/*  converts in_su->su_txt using su_layout_special() or internal_to_su()   */
/*  for use with tag attribute values, not control word operands           */
/*                                                                         */
/*  Note: in wgml 4.0, attribute values have these traits:                 */
/*      they can be delimited                                              */
/*      they can contain whitespace if delimited                           */
/*      they can never be expressions, even if they do not include a unit  */
/*      BANREGION indent, hoffset and width attributes can take special    */
/*          values ("left", "right", "center", "centre", and "extend")     */
/*                                                                         */
/*    returns cvterr: false on success (no conversion error)               */
/*                    true on error (conversion error occurred)            */
/***************************************************************************/

bool att_val_to_su( su *in_su, bool pos, att_val_type *attr_val, bool specval )
{
    char        *ps;        // destination for value text
    char        sign;

    if( attr_val->tok.l > MAX_SU_LENGTH ) {     // won't fit
        xx_line_err_exit_c( ERR_INV_ATT_VAL, attr_val->tok.s );
        /* never return */
    }
    /* attr_val->specval is lowercased already */
    strcpy( in_su->su_txt, attr_val->specval );

    in_su->su_u = SU_undefined;
    in_su->su_relative = false;

    ps = in_su->su_txt;
    if( *ps == '+' ) {                      // not allowed with tags
        xx_line_err_exit_c( ERR_INV_ATT_VAL, attr_val->tok.s );
        /* never return */
    }
    if( *ps == '-' ) {               // not relative, just negative
        if( pos ) {                         // value must be positive
            xx_line_err_exit_c( ERR_INV_ATT_VAL, attr_val->tok.s );
            /* never return */
        }
        sign = *ps;
        if( *(ps + 1) == '+'
          || *(ps + 1) == '-' ) {  // only one sign is allowed
            xx_line_err_exit_c( ERR_INV_ATT_VAL, attr_val->tok.s );
            /* never return */
        }
    } else {
        sign = '+';
    }
    if( *ps == '\0' ) {                     // value end reached, not valid
        xx_line_err_exit_c( ERR_INV_ATT_VAL, attr_val->tok.s );
        /* never return */
    }

    if( specval && su_layout_special( in_su ) )
        return( false );

    return( internal_to_su( in_su, true, attr_val->tok.s ) );
}

/***************************************************************************/
/*  initializes in_su->su_txt from scanp                                   */
/*  converts in_su->su_txt using su_expression() or internal_to_su()       */
/*  for use with control word operands, not tag attribute values           */
/*                                                                         */
/*  Note: in wgml 4.0, these operands have these traits:                   */
/*      they cannot be delimited                                           */
/*      they cannot contain whitespace                                     */
/*      they can be expressions, provided they do not include a unit       */
/*                                                                         */
/*    returns cvterr: false on success (no conversion error)               */
/*                    true on error (conversion error occurred)            */
/***************************************************************************/

bool cw_val_to_su( const char **scanp, su *in_su )
{
    const char  *p;     // source of value text
    const char  *pa;    // start of value text
    const char  *pb;    // start of value text
    char        *ps;    // destination for value text
    char        sign;
    unsigned    len;
    unsigned    i;

    ps = in_su->su_txt;
    *ps = '\0';

    p = *scanp;
    pa = p;
    SkipSpaces( p );            // just in case
    pb = p;
    SkipNonSpaces( p );
    len = p - pb;
    *scanp = p;                 // report back value of p
    if( len > MAX_SU_LENGTH ) {
        xx_line_err_exit_c( ERR_INV_CW_OP_VAL, pa );
        /* never return */
    }
    /* lowercase input string */
    for( i = 0; i < len; i++ ) {
        ps[i] = my_tolower( pb[i] );
    }
    ps[i] = '\0';

    in_su->su_u = SU_undefined;
    if( *ps == '+'
      || *ps == '-' ) {
        sign = *ps;
        in_su->su_relative = true;  // value is added / subtracted from old value
    } else {
        sign = '+';
        in_su->su_relative = false;         // value replaces old value
    }

    if( su_expression( in_su ) )
        return( false );

    return( internal_to_su( in_su, false, pa ) );
}

/***************************************************************************/
/*  initializes in_su->su_txt using p                                      */
/*  converts in_su->su_txt using su_layout_special() or internal_to_su()   */
/*  for use when initializing the layout                                   */
/*  no delimiters are expected, recognized, or ignored                     */
/*  p is not reset; typical initializers are string literals               */
/*                                                                         */
/*  Note: in wgml 4.0, attribute values have these traits:                 */
/*      they can be delimited                                              */
/*      they can contain whitespace if delimited                           */
/*      they can never be expressions, even if they do not include a unit  */
/*      BANREGION indent, hoffset and width attributes can take special    */
/*          values ("left", "right", "center", "centre", and "extend")     */
/*                                                                         */
/*    returns cvterr: false on success (no conversion error)               */
/*                    true on error (conversion error occurred)            */
/***************************************************************************/

bool lay_init_su( const char *p, su *in_su )
{
    const char      *pa;    // start of value text
    const char      *pb;
    char            *ps;    // destination for value text
    char            sign;
    int             len;
    int             i;

    ps = in_su->su_txt;
    *ps = '\0';

    pa = p;
    SkipSpaces( p );                // just in case
    pb = p;
    SkipNonSpaces( p );
    len = p - pb;

    if( len > MAX_SU_LENGTH ) {     // won't fit
        xx_line_err_exit_c( ERR_INV_ATT_VAL, pa );
        /* never return */
    }
    /* lowercase input string */
    for( i = 0; i < len; i++ ) {
        ps[i] = my_tolower( pb[i] );
    }
    ps[i] = '\0';

    in_su->su_u = SU_undefined;
    in_su->su_relative = false;

    if( *ps == '+' ) {              // not allowed with tags
        xx_line_err_exit_c( ERR_INV_ATT_VAL, pa );
        /* never return */
    }
    if( *ps == '-' ) {       // not relative, just negative
        sign = *ps;
        if( *(ps + 1) == '+'
          || *(ps + 1) == '-' ) {   // only one sign is allowed
            xx_line_err_exit_c( ERR_INV_ATT_VAL, pa );
            /* never return */
        }
    } else {
        sign = '+';
    }
    if( *ps == '\0' ) {             // value end reached, not valid
        xx_line_err_exit_c( ERR_INV_ATT_VAL, pa );
        /* never return */
    }

    if( su_layout_special( in_su ) )
        return( false );

    return( internal_to_su( in_su, true, pa ) );
}

/***************************************************************************/
/*  convert internal space units to device space units                     */
/*  return value is signed as space unit can be relative (+ -)             */
/***************************************************************************/

int conv_hor_unit( su *in_su, font_number font )
{
    int             ds;

    switch( in_su->su_u ) {
    case SU_chars_lines :
        ds = in_su->su_whole * (int)bin_device->horizontal_base_units / CPI;
        break;
    case SU_dv :
        ds = in_su->su_whole;
        break;
    case SU_ems :
        ds = in_su->su_whole * wgml_fonts[font].em_base;
        break;
    case SU_inch :
    case SU_cm :
    case SU_mm :
    case SU_cicero :
    case SU_pica :
        ds = (int64_t)in_su->su_inch * bin_device->horizontal_base_units / 10000L;
        break;
    default:
        ds = 0;
        break;
    }
    return( ds );
}

int conv_vert_unit( su *in_su, text_space text_spacing, font_number font )
{
    int             ds;
    int             fp;

    if( !( text_spacing > 0 ) ) {       // if spacing valid use it
        text_spacing = g_text_spacing;  // else default
    }
    switch( in_su->su_u ) {
    case SU_chars_lines :
    case SU_ems :
        // no decimals, use spacing, round negative values down
        ds = text_spacing * in_su->su_whole * wgml_fonts[font].line_height;
        if( ds < 0 ) {
            ds++;
        }
        break;
    case SU_dv :
        // no decimals, no spacing, round negative values down
        ds = in_su->su_whole;
        if( ds < 0 ) {
            ds++;
        }
        break;
    case SU_inch :
    case SU_cm :
    case SU_mm :
    case SU_cicero :
    case SU_pica :
        if ( in_su->su_inch == 0 ) {    // if the value is "0", ds is "0"
            ds = 0;
            break;
        }
        ds = (int64_t)in_su->su_inch * bin_device->vertical_base_units / 10000L;
        fp = (int64_t)in_su->su_inch * bin_device->vertical_base_units % 10000L;
        if( in_su->su_inch > 0 ) {
            if ( fp > 5000 ) {
                ds++;
            }
        } else {
            if ( -fp < 5000 ) { // fp is negative, but must compare as positive
                ds++;
            }
        }
        break;
    default:
        ds = 0;
        break;
    }
    return( ds );
}

/***************************************************************************/
/*  format a number according to the num_style                             */
/*                                                                         */
/*  returns ptr to string or NULL if error                                 */
/***************************************************************************/

char *format_num( unsigned n, char *res, unsigned ressize, num_style ns )
{
    unsigned    pos;
    unsigned    pos1;
    char        *p;
    char        *rp;
    char        temp[NUM2STR_LENGTH + 3 + 1]; // +3 for () and decimal point
    char        a1;
    char        a2;
    char        charbase;


    p = temp;
    pos = 0;
    if( ns & STYLE_xpa ) {
        *p++ = '(';                     // start number with left paren
        if( ++pos >= ressize ) {
            return( NULL );             // result field overflow
        }
    }
    if( ns & (STYLE_a | STYLE_b) ) {    // alphabetic limit 2 'digits'
        /************************************************************************/
        /*  Arbitrary limit Value 728 = 2 characters    extend if needed    TBD */
        /************************************************************************/
        if( n >= 27 * 27
          || (n < 1) ) {                // only 2 letters supported
            return( NULL );             // and numbers > zero
        }
        if( ns & STYLE_a ) {
            charbase = 'a' - 1;
        } else {
            charbase = 'A' - 1;
        }
    }
    switch( ns & STYLE_char1 ) {
    case STYLE_a:                      // lower case alphabetic
    case STYLE_b:                      // UPPER case alphabetic
        a1 = n / 27;
        a2 = n % 27;
        if( a1 > 0 ) {
            *p++ = charbase + a1;
            if( ++pos >= ressize ) {
                return( NULL );         // result field overflow
            }
            *p++ = charbase + 1 + a2;
            if( ++pos >= ressize ) {
                return( NULL );         // result field overflow
            }
        } else {
            *p++ = charbase + a2;
            if( ++pos >= ressize ) {
                return( NULL );         // result field overflow
            }
        }
        break;
    case STYLE_h:                      // arabic
        pos1 = sprintf( p, "%u", n );
        pos += pos1;
        if( pos >= ressize ) {
            return( NULL );             // result field overflow
        }
        p += pos1;
        break;
    case STYLE_r:                      // lower case roman
        rp = int_to_roman( n, p, ressize - pos, false );
        if( rp == NULL ) {
            return( NULL );             // field overflow
        }
        pos1 = strlen( rp );
        p += pos1;
        break;
    case STYLE_c:                      // UPPER case roman
        rp = int_to_roman( n, p, ressize - pos, true );
        if( rp == NULL ) {
            return( NULL );             // field overflow
        }
        strupr( p );
        pos1 = strlen( rp );
        p += pos1;
        break;
    default:
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    if( ns & STYLE_xd ) {
        *p++ = '.';                     // decimalpoint follows
        if( ++pos >= ressize ) {
            return( NULL );             // result field overflow
        }
    }
    if( ns & STYLE_xpb ) {
        *p++ = ')';                     // right paren follows
        if( ++pos >= ressize ) {
            return( NULL );             // result field overflow
        }
    }
    *p = '\0';                          // terminate string
    strcpy( res, temp );                  // copy temp string to result
    return( res );
}

/***************************************************************************/
/* get the start of the next potential attribute                           */
/* returns the start of the part of the line on which that potential       */
/*   attribute was found, thus preserving any preceding spaces in case it  */
/*   turns out that it is not an attribute at all but rather text          */
/***************************************************************************/

char *get_tag_attname( const char *p, char *attname )
{
    int     i;

    i = 0;
    while( is_tag_att_char( *p ) ) {
        if( i < TAG_ATT_NAME_LENGTH ) {
            attname[i++] = my_tolower( *p );
        }
        p++;
    }
    attname[i] = '\0';
    return( (char *)p );
}

static char *get_name_start( char *p, char **orig, bool layout )
{
    *orig = p;                      // save initial location
    while( CHECK_WS( *p, layout ) ) // over WS to attribute
        p++;
    return( p );
}

char *get_att_name_start( char *p, char **orig, bool layout )
{
    while( *(p = get_name_start( p, orig, layout )) == '\0' ) {
        if( input_cbs->fmflags & II_eof ) {
            return( p );
        }
        if( !get_line( true ) ) {       // next line for missing attribute
            return( p );
        }
        process_line();
        if( (*g_scandata.s == SCR_char)      // cw found: end-of-tag
          || (*g_scandata.s == GML_char) ) { // tag found: end-of-tag
            ProcFlags.reprocess_line = true;
            return( p );
        }
        p = g_scandata.s;                 // new line is part of current tag
    }
    if( *p == '.' ) {                   // end of tag
        ProcFlags.tag_end_found = true;
        p++;
        *orig = p;
    }
    return( p );
}

char *get_tag_att_name( char *p, char **orig, att_name_type *attr_name )
{
    p = get_att_name_start( p, orig, false );
    if( ProcFlags.reprocess_line )
        return( p );
    if( ProcFlags.tag_end_found )
        return( p );
    attr_name->tok.s = p;
    p = get_tag_attname( p, attr_name->attname.t );
    attr_name->tok.l = p - attr_name->tok.s;
    if( attr_name->tok.l < 2
      || attr_name->tok.l > TAG_ATT_NAME_LENGTH ) {
        xx_line_err_exit_c( ERR_ATT_NAME_INV, *orig );
        /* never return */
    }
    return( p );
}

/***************************************************************************/
/* get the attribute value and report tag-end ('.') if found               */
/*     [<white space>]=[<white space>]<value>                              */
/***************************************************************************/

void get_att_specval( att_val_type *attr_val )
{
    unsigned    i;
    unsigned    len;

    len = attr_val->tok.l;
    if( len > SPECVAL_LENGTH )
        len = SPECVAL_LENGTH;
    for( i = 0; i < len && is_su_char( attr_val->tok.s[i] ); i++) {
        attr_val->specval[i] = my_tolower( attr_val->tok.s[i] );
    }
    attr_val->specval[i] = '\0';
}

static char *get_value( char *p, att_val_type *attr_val, bool equ, bool layout )
{
    while( CHECK_WS( *p, layout ) )
        p++;
    if( equ ) {
        if( *p == '=' ) {
            p++;
            while( CHECK_WS( *p, layout ) ) {
                p++;
            }
        } else {
            xx_line_err_exit_c( ERR_EQ_MISSING, p );
            /* never return */
        }
    }
    if( (*p == '\0')
      || (*p == '.') ) { // value is missing
        if( *p == '.' ) {
            ProcFlags.tag_end_found = true;
        }
        xx_line_err_exit_c( ERR_ATT_VAL_MISSING, p );
        /* never return */
    }
    attr_val->tok.s = p;
    if( layout
      && is_quote_char( *p )
      || !layout
      && is_base_quote_char( *p ) ) {
        attr_val->quoted = *p++;        // over open quote
    } else {
        attr_val->quoted = ' ';
    }
    while( *p != '\0' && *p != attr_val->quoted && ( attr_val->quoted != ' ' || *p != '.' )) {
        p++;
    }
    attr_val->tok.l = p - attr_val->tok.s; // up to (not including) final quote
    if( attr_val->tok.l == 0 ) {
        attr_val->tok.s = NULL;
        attr_val->specval[0] = '\0';
    } else {
        if( attr_val->quoted != ' ' ) {
            if( *p != attr_val->quoted ) {  // terminating quote not found
                xx_line_err_exit_c( ERR_ATT_VAL_OPEN, attr_val->tok.s );
                /* never return */
            }
            p++;                        // over terminating quote
            attr_val->tok.s++;          // remove quotes from value info
            attr_val->tok.l--;
        }
        get_att_specval( attr_val );
    }
    if( *p == '.' ) {
        ProcFlags.tag_end_found = true;
        p++;                            // over ending '.'
    }
    return( p );
}

char *get_tag_value( char *p, att_val_type *attr_val )
{
    return( get_value( p, attr_val, false, false ) );
}

char *get_lay_value( char *p, att_val_type *attr_val )
{
    return( get_value( p, attr_val, true, true ) );
}

char *get_att_value( char *p, att_val_type *attr_val )
{
    return( get_value( p, attr_val, true, false ) );
}


/***************************************************************************/
/* report error involving improperly nested blocks                         */
/* placed here because it must return to the caller, which should probably */
/* be looked at at some point                                              */
/***************************************************************************/

void g_keep_nest( const char * cw_tag ) {
    switch( cur_group_type ) {
    case GRT_fb :
        keep_nest_err_exit( cw_tag, "a floating block" );
        /* never return */
    case GRT_fig :
        keep_nest_err_exit( cw_tag, "a figure" );
        /* never return */
    case GRT_fk :
        keep_nest_err_exit( cw_tag, "a floating keep" );
        /* never return */
    case GRT_fn :
        keep_nest_err_exit( cw_tag, "a footnote" );
        /* never return */
    case GRT_xmp :
        keep_nest_err_exit( cw_tag, "an example" );
        /* never return */
    }
    return;
}

/***************************************************************************/
/* parse and return a font number value                                    */
/***************************************************************************/

font_number get_font_number( const char *value, unsigned len )
{
    const char      *p;
    const char      *pb;
    unsigned long   wk;

    p = value;
    pb = p + len;

    while( my_isdigit( *p ) ) {                 // ensure entire token consists of decimal digits
        p++;
    }

    if( p != pb ) {                             // badly-formed token
        xx_line_err_exit_c( ERR_NUM_TOO_LARGE, value );
        /* never return */
    }

    wk = strtoul( value, NULL, 10 );
    if( errno == ERANGE
      || wk > 255 ) {
        wk = 0;
    }
    return( wk );
}

/***************************************************************************/
/*  convert integer to roman digits                                        */
/***************************************************************************/

char *int_to_roman( unsigned n, char *res, unsigned ressize, bool ucase )
{
    static const struct {
        unsigned    val;
        unsigned    val49;
        char        ch;
        char        ch49;
    } i_2_r[] = {
        { 1000, 900, 'm', 'c' },
        {  500, 400, 'd', 'c' },
        {  100,  90, 'd', 'x' },
        {   50,  40, 'l', 'x' },
        {   10,   9, 'x', 'i' },
        {    5,   4, 'v', 'i' },
        {    1,   1, 'i', 'i' }
    };

    unsigned    digit;
    unsigned    pos;
    char        *p;
    char        c;

    p = res;
    *p = '\0';
    if( (n < 1)
      || (n > 3999) ) {       // invalid out of range
        return( NULL );
    }

    digit = 0;
    pos = 0;
    do {
        while( n >= i_2_r[digit].val ) {
            c = i_2_r[digit].ch;
            *p++ = c;
            if( ++pos >= ressize ) {
                return( NULL );         // result field overflow
            }
            n -= i_2_r[digit].val;
        }
        if( n >= i_2_r[digit].val49 ) {
            c = i_2_r[digit].ch49;
            *p++ = ( ucase ) ? my_toupper( c ) : c;
            if( ++pos >= ressize ) {
                return( NULL );         // result field overflow
            }
            c = i_2_r[digit].ch;
            *p++ = ( ucase ) ? my_toupper( c ) : c;
            if( ++pos >= ressize ) {
                return( NULL );         // result field overflow
            }
            n -= i_2_r[digit].val49;
        }
        digit++;
    } while( n > 0 );
    *p = '\0';
    return( res );
}


/****************************************************************************/
/* return the page style for the current section                            */
/* used with both ff_entry and ix_e_blk                                     */
/****************************************************************************/

num_style find_pgnum_style( void )
{
    num_style   retval;

    /****************************************************/
    /* first restrict processing to those document      */
    /* sections that can have a page number style       */
    /* then use conditionals to set identify the entry  */
    /* in pgnum_style to copy to curr->style            */
    /****************************************************/

    switch( ProcFlags.doc_sect ) {
    case DSECT_abstract :
    case DSECT_preface :
    case DSECT_body :
    case DSECT_appendix :
    case DSECT_backm :
    case DSECT_index :
        if( ProcFlags.doc_sect == DSECT_abstract ) {
            retval = pgnum_style[PGNST_abstract];
        } else if( ProcFlags.doc_sect == DSECT_appendix ) {
            retval = pgnum_style[PGNST_appendix];
        } else if( ProcFlags.doc_sect == DSECT_backm ) {
            retval = pgnum_style[PGNST_backm];
        } else if( ProcFlags.doc_sect == DSECT_body ) {
            retval = pgnum_style[PGNST_body];
        } else if( ProcFlags.doc_sect == DSECT_index ) {
            retval = pgnum_style[PGNST_index];
        } else if( ProcFlags.doc_sect == DSECT_preface ) {
            retval = pgnum_style[PGNST_preface];
        }
        break;
    default :
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    return( retval );
}


/***************************************************************************/
/*  initalize an ffh_entry instance and append insert to the ffh_list      */
/*  Note: calling function must initialize ffh_list if it is NULL when the */
/*        function returns by setting it to point to the return value      */
/*  Note: function should be called immediately before use, so that the    */
/*        style field is set from correct and current information          */
/***************************************************************************/

ffh_entry * init_ffh_entry( ffh_entry * ffh_list, ffhflags flags )
{
    ffh_entry   *   curr;

    curr = ffh_list;
    if( curr == NULL ) {            // first entry
        curr = (ffh_entry *)mem_alloc( sizeof( ffh_entry ) );
    } else {
        while( curr->next != NULL ) {
            curr = curr->next;
        }
        curr->next = (ffh_entry *)mem_alloc( sizeof( ffh_entry ) );
        curr = curr->next;
    }
    curr->next = NULL;
    curr->pageno = 0;
    curr->number = 0;
    curr->prefix = NULL;
    curr->text = NULL;
    curr->flags = flags;
    curr->abs_pre = false;
    curr->style = find_pgnum_style();

    return( curr );
}

/***************************************************************************/
/*  initalize a fwd_ref instance and insert it (if new) in alpha order     */
/***************************************************************************/

fwd_ref *init_fwd_ref( fwd_ref *dict, const char *refid )
{
    fwd_ref *   curr;
    fwd_ref *   local;
    fwd_ref *   prev;

    prev = NULL;
    for( local = dict; local != NULL; local = local->next ) {
        if( strcmp( local->refid, refid ) >= 0 ) {
            break;
        }
        prev = local;
    }
    curr = (fwd_ref *)mem_alloc( sizeof( fwd_ref ) );
    curr->next = NULL;
    strcpy( curr->refid, refid );
    if( dict == NULL ) {
        dict = curr;           // first entry
    } else if( local == NULL ) {
        prev->next = curr;     // curr goes at end of list
    } else {
        if( prev == NULL ) {   // curr goes at start of list
            dict = curr;
        } else {
            prev->next = curr; // curr goes between two existing entries
        }
        curr->next = local;
    }
    return( dict );
}

/***************************************************************************/
/*  free the memory controlled by fig_list, fn_list, or hd_list            */
/***************************************************************************/

void free_ffh_list( ffh_entry * ffh_list )
{
    ffh_entry   *   curr;

    while( ffh_list != NULL ) {
        if( ffh_list->prefix != NULL ) {
            mem_free( ffh_list->prefix );
        }
        if ( ffh_list->text != NULL ) {
            mem_free( ffh_list->text );
        }
        curr = ffh_list;
        ffh_list = ffh_list->next;
        mem_free( curr );
    }
    return;
}

/***************************************************************************/
/*  free the memory controlled by fig_fwd_refs, fn_fwd_refs or hd_fwd_refs */
/***************************************************************************/

void free_fwd_refs( fwd_ref * fwd_refs )
{
    fwd_ref *   curr;

    while( fwd_refs != NULL ) {
        curr = fwd_refs;
        fwd_refs = fwd_refs->next;
        mem_free( curr );
    }
    return;
}


char *get_macro_name( const char *p, char *macname )
{
    int     i;

    i = 0;
    while( is_macro_char( *p ) ) {
        if( i < MAC_NAME_LENGTH ) {
            macname[i++] = my_tolower( *p );
        }
        p++;
    }
    macname[i] = '\0';
    return( (char *)p );
}

g_tags get_topn( g_tags tclo )
{
    g_tags      topn;

    switch( tclo ) {
    #define pick1(n,l,r,g,o,c)
    #define pick2(n1,l1,r1,g1,o1,c1,n2,l2,r2,g2,o2,c2)  case T_##n2: topn = T_##n1; break;
    #include "gtags.h"
    #undef pick2
    #undef pick1
    default:
        topn = tclo;
        break;
    }
    return( topn );
}

g_tags get_tclo( g_tags topn )
{
    g_tags      tclo;

    switch( topn ) {
    #define pick1(n,l,r,g,o,c)
    #define pick2(n1,l1,r1,g1,o1,c1,n2,l2,r2,g2,o2,c2)  case T_##n1: tclo = T_##n2; break;
    #include "gtags.h"
    #undef pick2
    #undef pick1
    default:
        tclo = topn;
        break;
    }
    return( tclo );
}
