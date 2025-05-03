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
* Description:  some WGML utility functions
*
****************************************************************************/


#include "wgml.h"
#include <errno.h>

#include "clibext.h"


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
    bool        cvterr  = true;
    bool        is_cp   = false;
    char    *   pd      = NULL; // ptr to decimal point
    char    *   pd1     = NULL; // ptr to first decimal digit
    char    *   pdn     = NULL; // ptr to last digit +1
    char    *   ps      = NULL; // destination for value text
    char    *   pu      = NULL; // ptr to trailing unit
    char        sign;
    char        unit[4];
    int         i;
    ldiv_t      div;
    int         k;
    int         wh;
    int         wd;
    su      *   s;

    unit[3] = '\0';
    unit[2] = '\0';
    unit[1] = '\0';
    unit[0] = '\0';
    s = in_su;
    wh = 0;
    wd = 0;

    ps = s->su_txt;

    /********************************************************************/
    /* extract and skip the sign, if present                            */
    /* att_val_to_su() and scr_val_to_su() deal with rejecting values   */
    /* formed incorrectly with regard to initial signs and to setting   */
    /* su_relative appropriately before invoking internal_to_su()       */
    /* lay_init_su() presupposes that the value has no initial sign     */
    /********************************************************************/

    if( (*ps == '+') || (*ps == '-') ) {
        sign = *ps;
        ps++;
    } else {
        sign = '+';
    }

    for( i = 0; i < 4; i++ ) {                  // max four digits in whole part
        if( my_isdigit( *ps ) ) {
            wh = (10 * wh) + (*ps - '0');
            ps++;
        } else {
            break;
        }
        if( *ps == '\0' ) {                     // value end reached
            break;
        }
    }
    if( my_isdigit( *ps ) ) {                   // too many digits in whole part
        val_parse_err( base + (ps - s->su_txt), tag );
    }

    if( *ps == '.' ) {                          // check for decimal point
        pd = ps;
        ps++;
        pd1 = ps;                               // remember start of decimals
        for( i = 0; i < 2; i++ ) {              // max two digits in decimals
            if( my_isdigit( *ps ) ) {
                wd = 10 * wd + *ps - '0';
                ps++;
            } else {
                break;
            }
            if( *ps == '\0' ) {                 // value end reached
                break;
            }
        }
        pdn = ps;
        if( pd1 == ps ) {                       // no decimals
            pd1 = NULL;
            pdn = NULL;
        }
        if( my_isdigit( *ps ) ) {               // too many digits in decimals
            val_parse_err( base + (ps - s->su_txt), tag );
        }
    }

    k = 0;
    pu = ps;
    for( i = 0; i < 2; i++ ) {                  // max two characters in unit
        if( my_isalpha( *ps ) ) {
            unit[k++] = my_tolower( *ps );      // save Unit
            ps++;
        } else {
            break;
        }
        if( *ps == '\0' ) {                     // value end reached
            break;
        }
    }
    if( my_isalpha( *ps ) ) {                   // too many characters in unit
        val_parse_err( base + (ps - s->su_txt), tag );
    }

    /***********************************************************************/
    /*  check for valid unit                                               */
    /***********************************************************************/

    if( unit[1] == '\0' ) {                     // single letter unit
        switch( unit[0] ) {
        case 'i' :
            s->su_u = SU_inch;
            break;
        case 'm' :
            s->su_u = SU_ems;
            if( pd != NULL ) {                  // no decimals with "M"
                val_parse_err( base + (ps - s->su_txt), tag );
            }
            break;
        case 'c' :
            s->su_u = SU_cicero;
            is_cp = true;
            break;
        case 'p' :
            s->su_u = SU_pica;
            is_cp = true;
            break;
        case '\0' :                             // no unit is characters or lines
            s->su_u = SU_chars_lines;
            break;
        default:
            val_parse_err( base + (ps - s->su_txt), tag );
        }
    } else {                                    // two letter unit
        if( unit[1] == 'm' ) {                  // cm, mm ?
            if( unit[0] == 'c' ) {
                s->su_u = SU_cm;
            } else if( unit[0] == 'm' ) {
                s->su_u = SU_mm;
            } else {                            // invalid unit
                val_parse_err( base + (ps - s->su_txt), tag );
            }
        } else if( unit[0] == 'd' ) {           // dv ?
            if( unit[1] == 'v' ) {
                s->su_u = SU_dv;
            } else {                            // invalid unit
                val_parse_err( base + (ps - s->su_txt), tag );
            }
        } else {                                // invalid unit
            val_parse_err( base + (ps - s->su_txt), tag );
        }
    }

    if( is_cp ) {       // "C" and "P" can be followed by max four digits
        for( i = 0; i < 4; i++ ) {
            if( my_isdigit( *ps ) ) {
                wd = (10 * wd) + (*ps - '0');
                ps++;
            } else {
                break;
            }
            if( *ps == '\0' ) {                 // value end reached
                break;
            }
        }
        if( my_isdigit( *ps ) ) {     // too many digits after "C" or "P"
            val_parse_err( base + (ps - s->su_txt), tag );
        }
    }

    if( *ps != '\0' ) {                         // value continues on: it shouldn't
        val_parse_err( base + (ps - s->su_txt), tag );
    }
    s->su_whole = wh;
    s->su_dec   = wd;

    if( k == 0 ) {                      // no trailing unit
        pu = NULL;
    }
    if( pd != NULL ) {                  // dec point found
        if( pu == NULL ) {              // need trailing unit
            val_parse_err( base + (ps - s->su_txt - 1), tag );
        }
    }

    s->su_inch = 0;
    s->su_mm   = 0;
    k = 1;
    if( pd1 != NULL ) {
        if( pdn - pd1 == 1 ) {
            k = 10;                 // only 0.1 digit
        }
    }
    switch( s->su_u ) {
    // the relative units are only stored, not converted
    case SU_chars_lines :
    case SU_ems :
    case SU_dv :
        break;
    case SU_inch :                      // inch, cm, mm valid with decimals
        s->su_mm   = (wh * 100L + wd * k) * 2540L;
        s->su_inch = (wh * 100L + wd * k) *  100L;
        break;
    case SU_cm :
        s->su_mm   = (wh * 100L + wd * k) * 1000L;
        s->su_inch = s->su_mm * 10L / 254L;
        break;
    case SU_mm :
        s->su_mm   = (wh * 100L + wd * k) *  100L;
        s->su_inch = s->su_mm * 10L / 254L;
        break;
    case SU_cicero :                    // cicero
        if( wd > 11 ) {
            div = ldiv( wd, 12L);
            wh += div.quot;
            wd = div.rem;
        }
        s->su_inch = wh * 10000L / 6L + wd * 10000L / 72L;
        s->su_inch = (int64_t)s->su_inch * 10656L / 10000L;
        s->su_mm = s->su_inch * 254L / 10L;
        break;
    case SU_pica :                      // pica
        if( wd > 11 ) {
            div = ldiv( wd, 12L);
            wh += div.quot;
            wd = div.rem;
        }
        s->su_inch = wh * 10000L / 6L + wd * 10000L / 72L;
        s->su_inch = (int64_t)s->su_inch * 9978L / 10000L;
        s->su_mm = s->su_inch * 254L / 10L;
        break;
    default:
        break;
    }
    if( sign == '-' ) {
        s->su_inch  = -s->su_inch;
        s->su_mm    = -s->su_mm;
        s->su_whole = -s->su_whole;
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

static bool su_expression( su * in_su )
{
    bool                retval  = true;
    char            *   p       = in_su->su_txt;
    condcode            cc;
    getnum_block        gn;

    gn.ignore_blanks = false;
    gn.argstart = p;
    while( *p != '\0' )
        p++;
    gn.argstop = p - 1;
    cc = getnum( &gn );

    if( cc == notnum ) {
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
    size_t          spc_cnt;
    text_chars  *   sav_chars;
    text_line   *   line;
    uint32_t        start;

    spc_cnt = strlen( spaces );
    if( spc_cnt > 0 ) {                                 // must have something to add
        if( t_element != NULL ) {                       // t_element exists
            if( t_element->type != el_text ) {          // must be text
                internal_err( __FILE__, __LINE__ );
            }

            line = t_element->element.text.first;
            while( line->next != NULL ) {
                line = line->next;
            }
            font = line->last->font;
            start = line->last->x_address + line->last->width;
            sav_chars = line->last;
            line->last->next = process_word( layout_work.note.spaces, spc_cnt, font, false );
            line->last = line->last->next;
            line->last->prev = sav_chars;
            line->last->type = tx_norm;
            line->last->x_address = start;

            if( wgml_fonts[font].line_height > line->line_height ) {
                line->line_height = wgml_fonts[font].line_height;
            }
        } else {                                        // too complicated
            xx_line_warn_c( wng_hdref_co_off, buff2 );
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
                process_line_full( t_line, ((ProcFlags.justify != ju_off) &&
                    (ProcFlags.justify != ju_on) && (ProcFlags.justify != ju_half)) );
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

size_t len_to_trail_space( const char *p , size_t len )
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
static bool su_layout_special( su * in_su )
{
    bool        retval = true;
    char    *   ps;
    su      *   s;
    int         wh;
    int         wd;
    char        quote;

    s = in_su;
    ps = s->su_txt;
    wh = 0;
    wd = 0;
    quote = '\0';

    /********************************************************************/
    /* val_start will be NULL when called from lay_val_to_su().         */
    /* val_start will not be NULL when called from att_val_to_su().     */
    /* when not NULL, *(val_start - 1) should be a whitespace or "="    */
    /********************************************************************/

    if( (val_start != NULL) && ((*(val_start - 1) == '\'') || (*(val_start - 1) == '"' ) ||
        (*ps == '+') || (*ps == '-' )) ) {   // values must not be quoted or signed
        retval = false;
    } else if( !strnicmp( "left", ps, 4 ) ) {
        s->su_u = SU_lay_left;
        strcpy( ps, "left" );
    } else if( !strnicmp( "right", ps, 5 ) ) {
        s->su_u = SU_lay_right;
        strcpy( ps, "right" );
    } else if( !strnicmp( "center", ps, 6 ) ) {
        s->su_u = SU_lay_centre;
        strcpy( ps, "center" );
    } else if( !strnicmp( "centre", ps, 6 ) ) {
        s->su_u = SU_lay_centre;
        strcpy( ps, "centre" );
    } else if( !strnicmp( "extend", ps, 6 ) ) {
        s->su_u = SU_lay_extend;
        strcpy( ps, "extend" );
    } else {
        retval = false;
    }

    if( retval ) {
        s->su_whole = 0;
        s->su_dec   = 0;
        s->su_inch  = 0;
        s->su_mm    = 0;
        s->su_relative = false;
    }

    return( retval );
}

/***************************************************************************/
/*  initializes in_su->su_txt using val_start/val_len                      */
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

bool att_val_to_su( su * in_su, bool pos )
{
    bool        cvterr  = true;
    char    *   ps      = NULL; // destination for value text
    char        sign;
    su      *   s;

    s = in_su;
    ps = s->su_txt;
    *ps = '\0';

    if( val_len > MAX_SU_CHAR - 1 ) {     // won't fit
        xx_line_err_c( err_inv_att_val, val_start );
    }
    strncpy( ps, val_start, val_len );
    ps[val_len] = '\0';

    s->su_u = SU_undefined;
    if( *ps == '+' ) {                      // not allowed with tags
        xx_line_err_c( err_inv_att_val, val_start );
    } else if( *ps == '-' ) {               // not relative, just negative
        if( pos ) {                         // value must be positive
            xx_line_err_c( err_inv_att_val, val_start );
        }
        sign = *ps;
        if( *(ps + 1) == '+' || *(ps + 1) == '-' ) {  // only one sign is allowed
            xx_line_err_c( err_inv_att_val, val_start );
        }
    } else {
        sign = '+';
    }
    if( *ps == '\0' ) {                     // value end reached, not valid
        xx_line_err_c( err_inv_att_val, val_start );
    }
    s->su_relative = false;             // no relative positioning with tags

    if( su_layout_special( in_su ) ) {
        cvterr = false;
    } else {
        cvterr = internal_to_su( in_su, true, val_start );
    }

    return( cvterr );
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

bool cw_val_to_su( char * * scanp, su * in_su )
{
    bool        cvterr  = true;
    char    *   p       = NULL; // source of value text
    char    *   pa      = NULL; // start of value text
    char    *   ps      = NULL; // destination for value text
    char        sign;
    size_t      len;
    su      *   s;

    s = in_su;
    p = *scanp;
    pa = p;
    ps = s->su_txt;
    *ps = '\0';
    SkipSpaces( p );            // just in case
    SkipNonSpaces( p );
    len = p - pa;
    *scanp = p;                 // report back value of p
    if( len > MAX_SU_CHAR - 1 ) {
        xx_line_err_c( err_inv_cw_op_val, val_start );
    }
    strncpy( ps, pa, len );
    ps[len] = '\0';

    s->su_u = SU_undefined;
    if( *ps == '+' || *ps == '-' ) {
        sign = *ps;
        s->su_relative = true;  // value is added / subtracted from old value
    } else {
        sign = '+';
        s->su_relative = false;         // value replaces old value
    }

    if( su_expression( in_su ) ) {
        cvterr = false;
    } else {
        cvterr = internal_to_su( in_su, false, pa );
    }

    return( cvterr );
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

bool lay_init_su( const char * p, su * in_su )
{
    bool            cvterr  = true;
    const char  *   pa      = NULL; // start of value text
    char        *   ps      = NULL; // destination for value text
    char            sign;
    size_t          len;
    su          *   s;

    s = in_su;
    pa = p;
    ps = s->su_txt;
    *ps = '\0';

    SkipSpaces( p );                // just in case
    SkipNonSpaces( p );
    len = p - pa;

    if( len > MAX_SU_CHAR - 1 ) { // won't fit
        xx_line_err_c( err_inv_att_val, val_start );
    }
    strncpy( ps, pa, len );
    ps[len] = '\0';

    s->su_u = SU_undefined;
    if( *ps == '+' ) {                      // not allowed with tags
        xx_line_err_c( err_inv_att_val, ps );
    } else if( *ps == '-' ) {               // not relative, just negative
        sign = *ps;
        if( *(ps + 1) == '+' || *(ps + 1) == '-' ) {  // only one sign is allowed
            xx_line_err_c( err_inv_att_val, ps );
        }
    } else {
        sign = '+';
    }
    if( *ps == '\0' ) {                     // value end reached, not valid
        xx_line_err_c( err_inv_att_val, ps );
    }
    s->su_relative = false;                 // no relative positioning with tags

    if( su_layout_special( in_su ) ) {
        cvterr = false;
    } else {
        cvterr = internal_to_su( in_su, true, pa );
    }

    return( cvterr );
}

/***************************************************************************/
/*  initializes in_su->su_txt using g_att_val.val_name/g_att_val.val_len   */
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

bool value_to_su( su * in_su, bool pos )
{
    bool        cvterr  = true;
    char    *   ps      = NULL; // destination for value text
    char        sign;
    su      *   s;

    s = in_su;
    ps = s->su_txt;
    *ps = '\0';

    if( g_att_val.val_len > MAX_SU_CHAR - 1 ) {     // won't fit
        xx_line_err_c( err_inv_att_val, g_att_val.val_name );
    }
    strncpy( ps, g_att_val.val_name, g_att_val.val_len );
    ps[g_att_val.val_len] = '\0';

    s->su_u = SU_undefined;
    if( *ps == '+' ) {                      // not allowed with tags
        xx_line_err_c( err_inv_att_val, g_att_val.val_name );
    } else if( *ps == '-' ) {               // not relative, just negative
        if( pos ) {                         // value must be positive
            xx_line_err_c( err_inv_att_val, g_att_val.val_name );
        }
        sign = *ps;
        if( *(ps + 1) == '+' || *(ps + 1) == '-' ) {  // only one sign is allowed
            xx_line_err_c( err_inv_att_val, g_att_val.val_name );
        }
    } else {
        sign = '+';
    }
    if( *ps == '\0' ) {                     // value end reached, not valid
        xx_line_err_c( err_inv_att_val, g_att_val.val_name );
    }
    s->su_relative = false;             // no relative positioning with tags

    if( su_layout_special( in_su ) ) {
        cvterr = false;
    } else {
        cvterr = internal_to_su( in_su, true, g_att_val.val_name );
    }

    return( cvterr );
}

/***************************************************************************/
/*  convert internal space units to device space units                     */
/*  return value is signed as space unit can be relative (+ -)             */
/***************************************************************************/

int32_t conv_hor_unit( su * s, font_number font )
{
    int32_t     ds;

    switch( s->su_u ) {
    case SU_chars_lines :
        ds = s->su_whole * (int32_t)bin_device->horizontal_base_units / CPI;
        break;
    case SU_dv :
        ds = s->su_whole;
        break;
    case SU_ems :
        ds = s->su_whole * wgml_fonts[font].em_base;
        break;
    case SU_inch :
    case SU_cm :
    case SU_mm :
    case SU_cicero :
    case SU_pica :
        ds = (int64_t)s->su_inch * bin_device->horizontal_base_units / 10000L;
        break;
    default:
        ds = 0;
        break;
    }
    return( ds );
}

int32_t conv_vert_unit( su *s, text_space text_spacing, font_number font )
{
    int32_t         ds;
    int32_t         fp;

    if( !( text_spacing > 0 ) ) {       // if spacing valid use it
        text_spacing = g_text_spacing;  // else default
    }
    switch( s->su_u ) {
    case SU_chars_lines :
    case SU_ems :
        // no decimals, use spacing, round negative values down
        ds = text_spacing * s->su_whole * wgml_fonts[font].line_height;
        if( ds < 0 ) {
            ds++;
        }
        break;
    case SU_dv :
        // no decimals, no spacing, round negative values down
        ds = s->su_whole;
        if( ds < 0 ) {
            ds++;
        }
        break;
    case SU_inch :
    case SU_cm :
    case SU_mm :
    case SU_cicero :
    case SU_pica :
        if ( s->su_inch == 0 ) {    // if the value is "0", ds is "0"
            ds = 0;
            break;
        }
        ds = (int64_t)s->su_inch * bin_device->vertical_base_units / 10000L;
        fp = (int64_t)s->su_inch * bin_device->vertical_base_units % 10000L;
        if( s->su_inch > 0 ) {
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

char *format_num( unsigned n, char *r, size_t rsize, num_style ns )
{
    size_t      pos;
    size_t      pos1;
    char    *   p;
    char    *   rp;
    char        temp[NUM2STR_LENGTH + 3]; // +3 for () and decimal point
    char        a1;
    char        a2;
    char        charbase;


    p = temp;
    pos = 0;
    if( ns & xpa_style ) {
        *p++ = '(';                     // start number with left paren
        if( ++pos >= rsize ) {
            return( NULL );             // result field overflow
        }
    }
    if( ns & (a_style | b_style) ) {    // alphabetic limit 2 'digits'
        /************************************************************************/
        /*  Arbitrary limit Value 728 = 2 characters    extend if needed    TBD */
        /************************************************************************/
        if( n >= 27 * 27 || (n < 1) ) { // only 2 letters supported
            return( NULL );             // and numbers > zero
        }
        if( ns & a_style ) {
            charbase = 'a' - 1;
        } else {
            charbase = 'A' - 1;
        }
    }
    switch( ns & char1_style ) {
    case a_style :                      // lower case alphabetic
    case b_style :                      // UPPER case alphabetic
        a1 = n / 27;
        a2 = n % 27;
        if( a1 > 0 ) {
            *p++ = charbase + a1;
            if( ++pos >= rsize ) {
                return( NULL );         // result field overflow
            }
            *p++ = charbase + 1 + a2;
            if( ++pos >= rsize ) {
                return( NULL );         // result field overflow
            }
        } else {
            *p++ = charbase + a2;
            if( ++pos >= rsize ) {
                return( NULL );         // result field overflow
            }
        }
        break;
    case h_style :                      // arabic
        pos1 = sprintf( p, "%u", n );
        pos += pos1;
        if( pos >= rsize ) {
            return( NULL );             // result field overflow
        }
        p += pos1;
        break;
    case r_style :                      // lower case roman
        rp = int_to_roman( n, p, rsize - pos );
        if( rp == NULL ) {
            return( NULL );             // field overflow
        }
        pos1 = strlen( rp );
        p += pos1;
        break;
    case c_style :                      // UPPER case roman
        rp = int_to_roman( n, p, rsize - pos );
        if( rp == NULL ) {
            return( NULL );             // field overflow
        }
        strupr( p );
        pos1 = strlen( rp );
        p += pos1;
        break;
    default:
        internal_err( __FILE__, __LINE__ );
    }

    if( ns & xd_style ) {
        *p++ = '.';                     // decimalpoint follows
        if( ++pos >= rsize ) {
            return( NULL );             // result field overflow
        }
    }
    if( ns & xpb_style ) {
        *p++ = ')';                     // right paren follows
        if( ++pos >= rsize ) {
            return( NULL );             // result field overflow
        }
    }
    *p = '\0';                          // terminate string
    strcpy( r, temp );                  // copy temp string to result
    return( r );
}

/***************************************************************************/
/* get the start of the next potential attribute                           */
/* returns the start of the part of the line on which that potential       */
/*   attribute was found, thus preserving any preceding spaces in case it  */
/*   turns out that it is not an attribute at all but rather text          */
/* NOTE: ProcFlags.tag_end_found is cleared here rather than in            */
/*       get_att_value() to accomodate attributes "compact" and "break"    */
/*       which have no "value" but which must not return tag_end_found     */
/*       unless, of course, it is                                          */
/***************************************************************************/

char * get_att_start( char * p )
{
    char    * pa;

    static  char      buf[BUF_SIZE];

    ProcFlags.tag_end_found = false;
    for(;;) {                           // loop until potential attribute/rescan line found
        pa = p;                         // save initial location
        SkipSpaces( p );                // over WS to attribute
        if( *p == '.' ) {   // end-of-tag
            p++;
            pa = p;         // return next char after end-of-tag
            ProcFlags.tag_end_found = true;
            break;
        }
        if( *p == '\0' ) {              // end of line: get new line
            if( !(input_cbs->fmflags & II_eof) ) {
                if( get_line( true ) ) {// next line for missing attribute

                    /*******************************************************/
                    /* buff2 must be restored if it is to be reprocessed   */
                    /* so that any symbol substitutions will reflect any   */
                    /* changes made by the tag calling it                  */
                    /*******************************************************/

                    strcpy( buf, buff2 );
                    scan_start = buff2;
                    scan_stop  = buff2 + buff2_lg;
                    if( (*scan_start == SCR_char) ||    // cw found: end-of-tag
                        (*scan_start == GML_char) ) {   // tag found: end-of-tag
                        ProcFlags.reprocess_line = true;
                        break;
                    } else {
                        process_line();
                        p = scan_start; // new line is part of current tag
                        continue;
                    }
                }
            } else {
                break;  // eof() found: return for further processing
            }
        } else {
            break;      // potential next attribute found
        }
    }
    att_start = p;      // only valid if !ProcFlags.reprocess_line && !ProcFlags.tag_end_found
    return( pa );       // return initial location for current att_start
}

/***************************************************************************/
/* get the attribute value and report tag-end ('.') if found               */
/*     [<white space>]=[<white space>]<value>                              */
/***************************************************************************/

char * get_att_value( char * p )
{
    char        quote;

    quote_char = '\0';
    val_start = NULL;
    val_len = 0;
    SkipSpaces( p );                    // over WS to '='
    if( *p == '=' ) {
        p++;
        SkipSpaces( p );                // over WS to value
    } else {
        if( *p == '.' ) {
            ProcFlags.tag_end_found = true;
        }
        xx_line_err_c( err_eq_missing, p );
    }
    if( (*p == '\0') || (*p == '.') ) { // value is missing
        if( *p == '.' ) {
            ProcFlags.tag_end_found = true;
        }
        xx_line_err_c( err_att_val_missing, p );
    }
    if( *p == '"' || *p == '\'' || *p == '`' ) {
        quote = *p;
        quote_char = *p;
        ++p;
        val_start = p;
        while( *p != '\0' ) {
            if( *p == quote ) {
                if( *(p + 1) != quote ) {
                    break;
                }
                { // this should almost never be used
                    char    *   q;
                    char    *   r;
                    q = p;
                    r = p + 1;
                    while( *r != '\0' ) {
                        *q = *r;
                        q++;
                        r++;
                    }
                }
            }
            ++p;
        }
        val_len = p - val_start;    // up to (not including) final quote
        if( *p != quote ) {         // terminating quote not found
            xx_line_err_c( err_att_val_open, val_start - 1 );
        }
        ++p;                        // over final quote
    } else {
        val_start = p;
        while( *p != '\0' && *p != ' ' && *p != '.' ) {
            ++p;
        }
        val_len = p - val_start;
    }
    if( *p == '.' ) {
        ProcFlags.tag_end_found = true;
    }
    return( p );
}

/***************************************************************************/
/* get the start and length of the next potential attribute                */
/* returns the start of the part of the line on which that potential       */
/*   attribute was found, thus preserving any preceding spaces in case it  */
/*   turns out that it is not an attribute at all but rather text          */
/* NOTE: ProcFlags.tag_end_found is cleared here rather than in            */
/*       get_att_value() to accomodate attributes "compact" and "break"    */
/*       which have no "value" but which must not return tag_end_found     */
/*       unless, of course, it is                                          */
/***************************************************************************/

char * get_attribute( char * p )
{
    char    *   pa;
    int         i;

    static  char      buf[BUF_SIZE];

    ProcFlags.tag_end_found = false;
    for(;;) {                           // loop until potential attribute/rescan line found
        pa = p;                         // save initial location
        SkipSpaces( p );                // over WS to attribute
        if( *p == '.' ) {   // end-of-tag
            p++;
            pa = p;         // return next char after end-of-tag
            ProcFlags.tag_end_found = true;
            break;
        }
        if( *p == '\0' ) {              // end of line: get new line
            if( !(input_cbs->fmflags & II_eof) ) {
                if( get_line( true ) ) {// next line for missing attribute

                    /*******************************************************/
                    /* buff2 must be restored if it is to be reprocessed   */
                    /* so that any symbol substitutions will reflect any   */
                    /* changes made by the tag calling it                  */
                    /*******************************************************/

                    strcpy( buf, buff2 );
                    scan_start = buff2;
                    scan_stop  = buff2 + buff2_lg;
                    if( (*scan_start == SCR_char) ||    // cw found: end-of-tag
                        (*scan_start == GML_char) ) {   // tag found: end-of-tag
                        ProcFlags.reprocess_line = true;
                        break;
                    } else {
                        process_line();
                        p = scan_start; // new line is part of current tag
                        continue;
                    }
                }
            } else {
                break;  // eof() found: return for further processing
            }
        } else {
            break;      // potential next attribute found
        }
    }
    g_att_val.att_name = p; // only valid if !ProcFlags.reprocess_line && !ProcFlags.tag_end_found
    g_att_val.att_len = 0;
    for( i = 0; is_att_char( *(p + i) ); i++ ) {
        g_att_val.att_len++;
    }
    return( pa );           // return initial location for current att_start
}

/***************************************************************************/
/* get the attribute value and report tag-end ('.') if found               */
/*     [<white space>]=[<white space>]<value>                              */
/***************************************************************************/

char * get_value( char * p )
{
    char        quote;

    quote_char = '\0';
    g_att_val.val_name = NULL;
    g_att_val.val_len = 0;
    SkipSpaces( p );                    // over WS to '='
    if( *p == '=' ) {
        p++;
        SkipSpaces( p );                // over WS to value
    } else {
        if( *p == '.' ) {
            ProcFlags.tag_end_found = true;
        }
        xx_line_err_c( err_eq_missing, p );
    }
    if( (*p == '\0') || (*p == '.') ) { // value is missing
        xx_line_err_c( err_att_val_missing, p );
    }
    if( *p == '"' || *p == '\'' || *p == '`' ) {
        quote = *p;
        quote_char = *p;
        ++p;
        g_att_val.val_name = p;
        while( *p != '\0' ) {
            if( *p == quote ) {
                if( *(p + 1) != quote ) {
                    break;
                }
                { // this should almost never be used
                    char    *   q;
                    char    *   r;
                    q = p;
                    r = p + 1;
                    while( *r != '\0' ) {
                        *q = *r;
                        q++;
                        r++;
                    }
                }
            }
            ++p;
        }
        g_att_val.val_len = p - g_att_val.val_name; // up to (not including) final quote
        if( *p != quote ) {         // terminating quote not found
            xx_line_err_c( err_att_val_open, g_att_val.val_name - 1 );
        }
        ++p;                        // over final quote
    } else {
        g_att_val.val_name = p;
        while( *p != '\0' && *p != ' ' && *p != '.' ) {
            ++p;
        }
        g_att_val.val_len = p - g_att_val.val_name;
    }
    if( *p == '.' ) {
        ProcFlags.tag_end_found = true;
    }
    return( p );
}

/***************************************************************************/
/* report error involving improperly nested blocks                         */
/* placed here because it must return to the caller, which should probably */
/* be looked at at some point                                              */
/***************************************************************************/

void g_keep_nest( const char * cw_tag ) {
    switch( cur_group_type ) {
    case gt_fb :
        keep_nest_err( cw_tag, "a floating block" );
    case gt_fig :
        keep_nest_err( cw_tag, "a figure" );
    case gt_fk :
        keep_nest_err( cw_tag, "a floating keep" );
    case gt_fn :
        keep_nest_err( cw_tag, "a footnote" );
    case gt_xmp :
        keep_nest_err( cw_tag, "an example" );
    }
    return;
}

/***************************************************************************/
/* parse and return a font number value                                    */
/***************************************************************************/

font_number get_font_number( char * value, size_t len )
{
    char            *p;
    char            *pb;
    unsigned long   wk;

    p = value;
    pb = p + len;

    while( my_isdigit( *p ) ) {                 // ensure entire token consists of decimal digits
        p++;
    }

    if( p != pb ) {                             // badly-formed token
        xx_line_err_c( err_num_too_large, val_start );
    }

    wk = strtoul( value, NULL, 10 );
    if( errno == ERANGE || wk > 255 ) {
        wk = 0;
    }
    return( wk );
}

/***************************************************************************/
/* get the tag value and report tag-end ('.') if found                     */
/*     [<white space>]<value>                                              */
/*     used by INCLUDE to capture file names without the "file" attribute  */
/***************************************************************************/

char * get_tag_value( char * p )
{
    char        quote;

    ProcFlags.tag_end_found = false;
    quote_char = '\0';
    val_start = NULL;
    val_len = 0;
    SkipSpaces( p );                    // over WS to '='
    if( (*p == '\0') || (*p == '.') ) { // value is missing
        if( *p == '.' ) {
            ProcFlags.tag_end_found = true;
        }
        xx_line_err_c( err_att_val_missing, p );
    }
    if( *p == '"' || *p == '\'' || *p == '`' ) {
        quote = *p;
        quote_char = *p;
        ++p;
        val_start = p;
        while( *p != '\0' ) {
            if( *p == quote ) {
                if( *(p + 1) != quote ) {
                    break;
                }
                { // this should almost never be used
                    char    *   q;
                    char    *   r;
                    q = p;
                    r = p + 1;
                    while( *r != '\0' ) {
                        *q = *r;
                        q++;
                        r++;
                    }
                }
            }
            ++p;
        }
        val_len = p - val_start;    // up to (not including) final quote
        if( *p != quote ) {         // terminating quote not found
            xx_line_err_c( err_att_val_open, val_start - 1 );
        }
        ++p;                        // over final quote
    } else {
        val_start = p;
        while( *p != '\0' && *p != ' ' && *p != '.' ) {
            ++p;
        }
        val_len = p - val_start;
    }
    if( *p == '.' ) {
        ProcFlags.tag_end_found = true;
    }
    return( p );
}

/***************************************************************************/
/*  convert integer to roman digits                                        */
/***************************************************************************/

char *int_to_roman( unsigned n, char *r, size_t rsize )
{
    static const struct {
        unsigned    val;
        unsigned    val49;
        char        ch;
        char        ch49;
    } i_2_r[] =
                {
                    { 1000, 900, 'm', 'c' },
                    {  500, 400, 'd', 'c' },
                    {  100,  90, 'd', 'x' },
                    {   50,  40, 'l', 'x' },
                    {   10,   9, 'x', 'i' },
                    {    5,   4, 'v', 'i' },
                    {    1,   1, 'i', 'i' }
                };

    size_t digit;
    size_t pos;
    char    * p = r;

    *p = '\0';
    if( (n < 1) || (n > 3999) ) {       // invalid out of range
        return( NULL );
    }

    digit = 0;
    pos = 0;
    do {
        while( n >= i_2_r[digit].val ) {
            *p++ = i_2_r[digit].ch;
            if( ++pos >= rsize ) {
                return( NULL );         // result field overflow
            }
            n -= i_2_r[digit].val;
        }
        if( n >= i_2_r[digit].val49 ) {
            *p++ = i_2_r[digit].ch49;
            if( ++pos >= rsize ) {
                return( NULL );         // result field overflow
            }
            *p++ = i_2_r[digit].ch;
            if( ++pos >= rsize ) {
                return( NULL );         // result field overflow
            }
            n -= i_2_r[digit].val49;
        }
        digit++;
    } while( n > 0 );
    *p = '\0';
    return( r );
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
    case doc_sect_abstract :
    case doc_sect_preface :
    case doc_sect_body :
    case doc_sect_appendix :
    case doc_sect_backm :
    case doc_sect_index :
        if( ProcFlags.doc_sect == doc_sect_abstract ) {
            retval = pgnum_style[pns_abstract];
        } else if( ProcFlags.doc_sect == doc_sect_appendix ) {
            retval = pgnum_style[pns_appendix];
        } else if( ProcFlags.doc_sect == doc_sect_backm ) {
            retval = pgnum_style[pns_backm];
        } else if( ProcFlags.doc_sect == doc_sect_body ) {
            retval = pgnum_style[pns_body];
        } else if( ProcFlags.doc_sect == doc_sect_index ) {
            retval = pgnum_style[pns_index];
        } else if( ProcFlags.doc_sect == doc_sect_preface ) {
            retval = pgnum_style[pns_preface];
        }
        break;
    default :
        internal_err( __FILE__, __LINE__ );
        break;
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

ffh_entry * init_ffh_entry( ffh_entry * ffh_list )
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
    curr->flags = 0;
    curr->abs_pre = false;
    curr->style = find_pgnum_style();

    return( curr );
}

/***************************************************************************/
/*  initalize a fwd_ref instance and insert it (if new) in alpha order     */
/***************************************************************************/

fwd_ref * init_fwd_ref( fwd_ref * fr_dict, const char * fr_id )
{
    fwd_ref *   curr;
    fwd_ref *   local;
    fwd_ref *   prev;

    if( fr_dict == NULL ) {
        curr = (fwd_ref *)mem_alloc( sizeof( fwd_ref ) );
        curr->next = NULL;
        strcpy( curr->id, fr_id );
        fr_dict = curr;         // first entry
    } else {
        local = fr_dict;
        prev = NULL;
        while( (local != NULL) && (strcmp( local->id, fr_id ) < 0) ) {
            prev = local;
            local = local->next;
        }
        if( local == NULL ) {       // curr goes at end of list
            curr = (fwd_ref *)mem_alloc( sizeof( fwd_ref ) );
            curr->next = NULL;
            strcpy( curr->id, fr_id );
            prev->next = curr;
        } else if( strcmp( local->id, fr_id ) > 0 ) {   // note: duplicate id ignored
            curr = (fwd_ref *)mem_alloc( sizeof( fwd_ref ) );
            curr->next = NULL;
            strcpy( curr->id, fr_id );
            if( prev == NULL ) {    // curr goes at start of list
                fr_dict = curr;
            } else {
                prev->next = curr;  // curr goes between two existing entries
            }
            curr->next = local;
        }
    }

    return( fr_dict );
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
