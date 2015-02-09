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
* Description:  some WGML utility functions
*
*               att_val_to_su
*               conv_hor_unit
*               conv_vert_unit
*               cw_val_to_su
*               format_num
*               get_att_value
*               greater_su
*               int_to_roman
*               internal_to_su
*               lay_init_su
*               len_to_trail_space
*               skip_to_quote
*               start_line_with_string
*               su_expression
*               su_layout_special
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1       /* use safer C library             */

#include "wgml.h"
#include "gvars.h"

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

const bool internal_to_su( su *in_su, bool tag, const char *base )
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
    long        k;
    long        wh;
    long        wd;
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

    for( i = 0; i < 4; i++ ) {              // max four digits in whole part
        if( (*ps >= '0') && (*ps <= '9') ) {
            wh = (10 * wh) + (*ps - '0');
            ps++;
        } else {
            break;
        }
        if( !*ps ) {                           // value end reached
            break;
        }
    }
    if( (*ps >= '0') && (*ps <= '9') ) {      // too many digits in whole part
        val_parse_err( base + (ps - s->su_txt), tag );
        scan_start = scan_stop + 1;
        return( cvterr );
    }

    if( *ps && *ps == '.' ) {   // check for decimal point
        pd = ps;
        ps++;
        pd1 = ps;                            // remember start of decimals
        for( i = 0; i < 2; i++ ) {          // max two digits in decimals
            if( (*ps >= '0') && (*ps <= '9') ) {
                wd = 10 * wd + *ps - '0';
                ps++;
            } else {
                break;
            }
            if( !*ps ) {                     // value end reached
                break;
            }
        }
        pdn = ps;
        if( pd1 == ps ) {                        // no decimals
            pd1 = NULL;
            pdn = NULL;
        }
        if( (*ps >= '0') && (*ps <= '9') ) {      // too many digits in decimals
            val_parse_err( base + (ps - s->su_txt), tag );
            scan_start = scan_stop + 1;
            return( cvterr );
        }
    }

    k = 0;
    pu = ps;
    for( i = 0; i < 2; i++ ) {                  // max two characters in unit
        if( *ps && isalpha( *ps ) ) {
            unit[k++] = tolower( *ps );          // save Unit
            ps++;
        } else {
            break;
        }
        if( !*ps ) {                            // value end reached
            break;
        }
    }
    if( *ps && isalpha( *ps ) ) {             // too many characters in unit
        val_parse_err( base + (ps - s->su_txt), tag );
        scan_start = scan_stop + 1;
        return( cvterr );
    }

    /***********************************************************************/
    /*  check for valid unit                                               */
    /***********************************************************************/
    if( unit[1] == '\0' ) {           // single letter unit
        switch( unit[0] ) {
        case 'i' :
            s->su_u = SU_inch;
            break;
        case 'm' :
            s->su_u = SU_ems;
            if( pd != NULL ) {          // no decimals with "M"
                val_parse_err( base + (ps - s->su_txt), tag );
                scan_start = scan_stop + 1;
                return( cvterr );
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
        case '\0' :                     // no unit is characters or lines
            s->su_u = SU_chars_lines;
            break;
        default:
            val_parse_err( base + (ps - s->su_txt), tag );
            scan_start = scan_stop + 1;
            return( cvterr );
        }
    } else {                            // two letter unit
        if( unit[1] == 'm' ) {          // cm, mm ?
            if( unit[0] == 'c' ) {
                s->su_u = SU_cm;
            } else if( unit[0] == 'm' ) {
                s->su_u = SU_mm;
            } else {                    // invalid unit
                val_parse_err( base + (ps - s->su_txt), tag );
                scan_start = scan_stop + 1;
                return( cvterr );
            }
        } else if( unit[0] == 'd' ) {   // dv ?
            if( unit[1] == 'v' ) {
                s->su_u = SU_dv;
            } else {                    // invalid unit
                val_parse_err( base + (ps - s->su_txt), tag );
                scan_start = scan_stop + 1;
                return( cvterr );
            }
        } else {                        // invalid unit
            val_parse_err( base + (ps - s->su_txt), tag );
            scan_start = scan_stop + 1;
            return( cvterr );
        }
    }

    if( is_cp ) {       // "C" and "P" can be followed by max four digits
        for( i = 0; i < 4; i++ ) {
            if( (*ps >= '0') && (*ps <= '9') ) {
                wd = (10 * wd) + (*ps - '0');
                ps++;
            }
            if( *ps ) {    // value end reached
                break;
            }
        }
    }
    if( (*ps >= '0') && (*ps <= '9') ) {      // too many digits after "C" or "P"
        val_parse_err( base + (ps - s->su_txt), tag );
        scan_start = scan_stop + 1;
        return( cvterr );
    }

    if( *ps ) {                             // value continues on: it shouldn't
        val_parse_err( base + (ps - s->su_txt), tag );
        scan_start = scan_stop + 1;
        return( cvterr );
    }
    s->su_whole = wh;
    s->su_dec   = wd;

    if( k == 0 ) {                      // no trailing unit
        pu = NULL;
    }
    if( pd != NULL ) {                  // dec point found
        if( pu == NULL ) {              // need trailing unit
            val_parse_err( base + (ps - s->su_txt), tag );
            scan_start = scan_stop + 1;
            return( cvterr );
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
    getnum_block        value;

    value.ignore_blanks = false;
    value.argstart = p;
    while( *p ) {
        p++;
    }
    value.argstop = p;
    cc = getnum( &value );

    if( cc == notnum ) {
        retval = false;
    } else {
        in_su->su_u = SU_chars_lines;
        in_su->su_whole = value.result;
        in_su->su_dec = 0;
        in_su->su_inch = 0;
        in_su->su_mm = 0;
    }

    return( retval );
}


/***************************************************************************/
/* return length of string without trailing spaces                         */
/* return 1 for all blank string                                           */
/***************************************************************************/

size_t  len_to_trail_space( const char *p , size_t len )
{
    while( (len > 0) && (p[--len] == ' ') )
        /* empty */;
    len++;
    if( len < 1 )
        len = 1;
    return( len );
}


char    *skip_to_quote( char * p, char quote )
{
    while( *p && quote != *p ) {
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
static  bool    su_layout_special( su * in_su )
{
    bool        retval = true;
    char    *   ps;
    su      *   s;
    long        wh;
    long        wd;
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
    } else if( !(strnicmp( "center", ps, 6 )) && (strnicmp( "centre", ps, 6 )) ) {
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

    if( (val_len + 1) > MAX_SU_CHAR ) {             // won't fit
        xx_line_err( err_inv_att_val, val_start );
        scan_start = scan_stop + 1;
        return( cvterr );
    }
    memcpy_s( ps, MAX_SU_CHAR - 1, val_start, val_len );
    ps[val_len] = '\0';

    s->su_u = SU_undefined;
    if( *ps == '+' ) {                   // not allowed with tags
        xx_line_err( err_inv_att_val, ps );
        scan_start = scan_stop + 1;
        return( cvterr );
    } else if( *ps == '-' ) {            // not relative, just negative
        if( pos ) {                     // value must be positive
            xx_line_err( err_inv_att_val, ps );
            scan_start = scan_stop + 1;
            return( cvterr );
        }
        sign = *ps;
        if( *(ps + 1) == '+' || *(ps + 1) == '-' ) {  // only one sign is allowed
            xx_line_err( err_inv_att_val, ps );
            scan_start = scan_stop + 1;
            return( cvterr );
        }
    } else {
        sign = '+';
    }
    if( !*ps ) {                          // value end reached, not valid
        xx_line_err( err_inv_att_val, ps );
        scan_start = scan_stop + 1;
        return( cvterr );
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
    p = *scanp;;
    pa = p;
    ps = s->su_txt;
    *ps = '\0';
    while( *p && (*p == ' ' ) ) {   // just in case
        p++;    
    }
    while( *p && (*p != ' ' ) ) {
        p++;
    }    
    len = p - pa;
    *scanp = p;                     // report back value of p
    if( (len + 1) > MAX_SU_CHAR ) {
        xx_line_err( err_inv_cw_op_val, val_start );
        scan_start = scan_stop + 1;
        return( cvterr );
    }
    memcpy_s( ps, MAX_SU_CHAR - 1, pa, len );
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
    bool        cvterr  = true;
    const char    *   pa      = NULL; // start of value text
    char    *   ps      = NULL; // destination for value text
    char        sign;
    size_t      len;
    su      *   s;

    s = in_su;
    pa = p;
    ps = s->su_txt;
    *ps = '\0';

    while( *p && (*p == ' ' ) ) {   // just in case
        p++;    
    }
    while( *p && (*p != ' ' ) ) {
        p++;
    }    
    len = p - pa;

    if( (len + 1) > MAX_SU_CHAR ) {                 // won't fit
        xx_line_err( err_inv_att_val, val_start );
        scan_start = scan_stop + 1;
        return( cvterr );
    }
    memcpy_s( ps, MAX_SU_CHAR - 1, pa, len );
    ps[len] = '\0';

    s->su_u = SU_undefined;
    if( *ps == '+' ) {                   // not allowed with tags
        xx_line_err( err_inv_att_val, ps );
        scan_start = scan_stop + 1;
        return( cvterr );
    } else if( *ps == '-' ) {            // not relative, just negative
        sign = *ps;
        if( *(ps + 1) == '+' || *(ps + 1) == '-' ) {  // only one sign is allowed
            xx_line_err( err_inv_att_val, ps );
            scan_start = scan_stop + 1;
            return( cvterr );
        }
    } else {
        sign = '+';
    }
    if( !*ps ) {                          // value end reached, not valid
        xx_line_err( err_inv_att_val, ps );
        scan_start = scan_stop + 1;
        return( cvterr );
    }
    s->su_relative = false;             // no relative positioning with tags

    if( su_layout_special( in_su ) ) {
        cvterr = false;
    } else {
        cvterr = internal_to_su( in_su, true, pa );
    }

    return( cvterr );
}


/***************************************************************************/
/*  convert internal space units to device space units                     */
/*   use font 0 or current font???                              TBD        */
/*  return value is signed as space unit can be relative (+ -)             */
/***************************************************************************/

int32_t conv_hor_unit( su * s )
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
        ds = s->su_whole * wgml_fonts[g_curr_font].em_base;
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

int32_t conv_vert_unit( su *s, unsigned char spc )
{
    int32_t         ds;
    int32_t         fp;
    unsigned char   space;

    if( spc > 0 ) {                     // if spacing valid use it
        space = spc;
    } else {
        space = spacing;                // else default
    }
    switch( s->su_u ) {
    case SU_chars_lines :
    case SU_ems :
        // no decimals, use spacing, round negative values down
        ds = space * s->su_whole * wgml_fonts[g_curr_font].line_height;
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
char *  format_num( uint32_t n, char * r, size_t rsize, num_style ns )
{
    size_t      pos;
    size_t      pos1;
    char    *   p;
    char    *   rp;
    char        temp[MAX_L_AS_STR + 3]; // +3 for () and decimal point
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
        if( n >= 27*27 || (n < 1) ) {   // only 2 letters supported
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
        ultoa( n, p, 10 );
        pos1 = strlen( p );
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
        out_msg( "Logic error in gutil.c int_to_roman()\n" );
        err_count++;
        g_suicide();
        break;
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
/* get the attribute value and report tag-end ('.') if found               */
/*     [<white space>]=[<white space>]<value>                              */
/***************************************************************************/

char * get_att_value( char * p )
{
    char        quote;

    ProcFlags.tag_end_found = false;
    quote_char = '\0';
    val_start = NULL;
    val_len = 0;
    while( *p == ' ' ) {                // over WS to '='
        p++;
    }
    if( *p == '=' ) {
        p++;
        while( *p == ' ' ) {            // over WS to value
            p++;
        }
    } else {
        if( *p == '.' ) {
            ProcFlags.tag_end_found = true;
        }
        xx_line_err( err_eq_missing, p );
        scan_start = scan_stop + 1;
        return( p );
    }
    if( (*p == '\0') || (*p == '.') ) { // value is missing
        if( *p == '.' ) {
            ProcFlags.tag_end_found = true;
        }
        xx_line_err( err_att_val_missing, p );
        scan_start = scan_stop + 1;
        return( p );
    }
    if( *p == '"' || *p == '\'' ) {
        quote = *p;
        quote_char = *p;
        ++p;
        val_start = p;
        while( *p ) {
            if( *p == quote ) {
                if( *(p + 1) != quote ) {
                    break;
                }
                { // this should almost never be used
                    char    *   q;
                    char    *   r;
                    q = p;
                    r = p + 1;
                    while( *r ) {
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
            xx_line_err( err_att_val_open, val_start - 1 );
            scan_start = scan_stop + 1;
            return( p );
        }
        ++p;                        // over final quote
    } else {
        val_start = p;
        while( *p && *p != ' ' && *p != '.' ) {
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
/*  return the parameter which has the larger value                        */
/*  initially for LP, should also be needed for FIGCAP                     */
/*  these are intended to be vertical values                               */
/***************************************************************************/

su * greater_su( su *su_a, su *su_b, unsigned char spacing )
{
    uint32_t    val_a;
    uint32_t    val_b;

    val_a = conv_vert_unit( su_a, spacing );
    val_b = conv_vert_unit( su_b, spacing );

    if( val_a > val_b ) {
        return( su_a );
    } else {
        return( su_b );
    }
}


/***************************************************************************/
/*  convert integer to roman digits                                        */
/***************************************************************************/

char * int_to_roman( uint32_t n, char * r, size_t rsize )
{
    static const struct {
        uint32_t    val;
        uint32_t    val49;
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

/***************************************************************************/
/* for :NOTE :OL, ... tags                                                 */
/* trailing spaces are stripped                                            */
/* influencing the left margin for the paragraph                           */
/***************************************************************************/

void    start_line_with_string( const char *text, font_number font, bool leave_1space )
{
    text_chars          *   n_char;     // new text char
    size_t                  count;

    count = strlen( text );
    if( count == 0 ) {
        return;
    }
    post_space = 0;
    while( *(text + count - 1) == ' ' ) {   // strip trailing spaces
        post_space++;
        if( --count == 0 ) {
            break;
        }
    }
    if( leave_1space && post_space > 0 ) {// for ordered :LI keep 1 trailing space
        post_space--;
        count++;
    }

    n_char = alloc_text_chars( text, count, font );

    n_char->x_address = g_cur_h_start;
    ju_x_start = g_cur_h_start;
    input_cbs->fmflags &= ~II_sol;      // no longer start of line

    n_char->width = cop_text_width( n_char->text, n_char->count, font );
    /***********************************************************/
    /*  Test if word hits right margin                         */
    /***********************************************************/

    if( n_char->x_address + n_char->width > g_page_right ) {
        process_line_full( t_line, ProcFlags.concat );
        t_line = alloc_text_line();
        n_char->x_address = g_cur_h_start;
    }

    if( t_line == NULL ) {
        t_line = alloc_text_line();
    }

    if( t_line->first == NULL ) {        // first element in output line
        t_line->first = n_char;
        t_line->line_height = wgml_fonts[font].line_height;
        ju_x_start = n_char->x_address;
        ProcFlags.line_started = true;
    } else {
        t_line->last->next = n_char;
        n_char->prev = t_line->last;
    }
    t_line->last  = n_char;

    g_cur_h_start = n_char->x_address + n_char->width;
    post_space = post_space * wgml_fonts[layout_work.defaults.font].spc_width;
}


#if 0
int main( int argc, char *argv[] )      // standalone test routine
{
    bool    error;
    su      aus;
    char    ein1[] = "1.5I";
//  char    ein1[] = "3.81cm";
//  char    ein1[] = "38.1mm";
//  char    *ein1 = "'6p11'";
//  char    *ein1 = "'1C'";
//  char    *ein2 = "'1C12'";
    char    *ein2 = "'5C12'";
    char    *ein3 = "'0P73'";
    char    *ein4 = "'5P6'";

    char   *p = ein1;
    error = to_internal_SU( &p, &aus );

    p = ein2;
    error = to_internal_SU( &p, &aus );

    p = ein3;
    error = to_internal_SU( &p, &aus );

    p = ein4;
    error = to_internal_SU( &p, &aus );

    return(0);
}
#endif

