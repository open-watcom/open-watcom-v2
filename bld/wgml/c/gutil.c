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
*               conv_hor_unit
*               conv_vert_unit
*               format_num
*               greater_su
*               int_to_roman
*               len_to_trail_space
*               skip_to_quote
*               su_layout_special
*               to_internal_SU
*               att_val_to_SU
*               start_line_with_string
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1       /* use safer C library             */

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/* return length of string without trailing spaces                         */
/* return 1 for all blank string                                           */
/***************************************************************************/

int32_t     len_to_trail_space( char *p , int32_t len )
{
    int32_t len_b = len;

    while( (len_b > 0) && (p[--len_b] == ' ') ) /* empty */;
    len_b++;
    return( max( len_b, 1 ) );
}


char    *skip_to_quote( char * p, char quote )
{
    while( *p && quote != *p ) {
        p++;
    }
    return( p + 1 );
}



/***************************************************************************/
/* extension for layout :BANREGION indent, hoffset and width attributes:   */
/*      symbolic units without a numeric value                             */
/***************************************************************************/
static  bool    su_layout_special( char * * scanp, su * converted )
{
    bool        converterror = false;
    char    *   p;
    char    *   ps;
    su      *   s;
    long        wh;
    long        wd;
    char        quote;

    s = converted;
    p = *scanp;
    ps = s->su_txt;
    *ps = '\0';
    wh = 0;
    wd = 0;
    quote = '\0';

    if( *p == '\'' || *p == '"' ) {     // ignore but remember quote
        quote = *p++;
    }
    if( !strnicmp( "left", p, 4 ) ) {
        s->su_u = SU_lay_left;
        strcpy( ps, "left" );
    } else if( !strnicmp( "right", p, 5 ) ) {
        s->su_u = SU_lay_right;
        strcpy( ps, "right" );
    } else if( !(strnicmp( "center", p, 6 )) && (strnicmp( "centre", p, 6 )) ) {
        s->su_u = SU_lay_centre;
        strcpy( ps, "centre" );
    } else if( !strnicmp( "extend", p, 6 ) ) {
        s->su_u = SU_lay_extend;
        strcpy( ps, "extend" );
    } else {
        converterror = true;
    }
    if( !converterror ) {
        s->su_whole = 0;
        s->su_dec   = 0;
        s->su_inch  = 0;
        s->su_mm    = 0;
        s->su_relative = false;
    }
    return( converterror );
}

/***************************************************************************/
/*  copies converted->su_txt from scanp                                    */
/*  conversion routines for Horizontal / Vertical space units              */
/*  Accepted formats:                                                      */
/*       1234        assume chars / lines                                  */
/*       8m          Ems                                                   */
/*       22Dv        Device units                                          */
/*                                                                         */
/*       12.34i      inch             (1 inch = 2.54cm)                    */
/*                      The exact definition of Cicero and Pica (points)   */
/*                      differs between Europe and USA,                    */
/*                      so until a better solution turns up:               */
/*       5C11        Cicero  + points (12 points = 1C = 1/6 inch)          */
/*       6p10        Pica    + points (12 points = 1P = 1/6 inch)          */
/*       5.23cm      centimeter                                            */
/*       6.75mm      millimeter                                            */
/*                                                                         */
/*    the absolute units (the last 5) will be stored                       */
/*    in 0.0001 millimeter units and 0.0001 inch units,                    */
/*    the relative ones (the first 3) will not be converted.               */
/*                                                                         */
/* extension for layout :BANREGION indent, hoffset and width attributes:   */
/*      symbolic units without a numeric value                             */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/*    returns  filled structure su, returncode false                       */
/*               or  returncode true in case of error                      */
/***************************************************************************/

bool    to_internal_SU( char * * scanp, su * converted )
{
    bool        converterror = true;
    char    *   p;
    char    *   ps;
    ldiv_t      div;
    su      *   s;
    long        wh;
    long        wd;
    char    *   pp;                  // ptr to decimal point or embedded unit
    char    *   pu;                     // ptr to trailing unit
    char    *   pd1;                    // ptr to 0.1 decimal
    char    *   pdn;                    // ptr to last digit +1
    char        unit[4];
    char        quote;
    long        k;
    char        sign;

    unit[3] = '\0';
    unit[2] = '\0';
    unit[1] = '\0';
    unit[0] = '\0';
    s = converted;
    p = *scanp;
    ps = s->su_txt;
    *ps = '\0';
    wh = 0;
    wd = 0;
    pp = NULL;
    quote = '\0';

    s->su_u = SU_undefined;
    if( *p == '\'' || *p == '"' ) {     // ignore but remember quote
        quote = *p++;
    }
    if( *p == '+' || *p == '-' ) {
        sign = *p;
        *ps++ = *p++;
        s->su_relative = true;  // value is added / subtracted from old value
    } else {
        sign = '+';
        s->su_relative = false;         // value replaces old value
    }

    /***********************************************************************/
    /*  Special for layout :BANREGION                                      */
    /***********************************************************************/

    if( quote == '\0' && isalpha( *p ) ) {
        converterror = su_layout_special( scanp, converted );
        if( !converterror ) {
            return( converterror );     // layout special ok
        }
    }
    while( *p >= '0' && *p <= '9' ) {   // whole part
        wh = 10 * wh + *p - '0';
        *ps++ = *p++;
        if( ps >= s->su_txt + sizeof( s->su_txt ) ) {
            if( quote ) {
                p = skip_to_quote( p, quote );
            }
            *scanp = p;
            return( converterror );
        }
    }
    pp = p;
    k = 0;
    while( *p && isalpha( *p ) ) {
        unit[k++] = tolower( *p );      // save Unit
        *ps++ = *p++;
        if( ps >= s->su_txt + sizeof( s->su_txt ) || k > 2 ) {
            if( quote ) {
                p = skip_to_quote( p, quote );
            }
            *scanp = p;
            return( converterror );
        }
    }
    if( p > pp + 2 ) {
         if( quote ) {
             p = skip_to_quote( p, quote );
         }
         *scanp = p;
         return( converterror );        // no unit has more than 2 chars
    }
    pd1 = NULL;
    pdn = NULL;

    if( p == pp && *p == '.' ) {        // no unit found, try dec point
        *ps++ = *p++;
        pd1 = p;                        // remember start of decimals
        if( ps >= s->su_txt + sizeof( s->su_txt ) ) {
            if( quote ) {
                p = skip_to_quote( p, quote );
            }
            *scanp = p;
            return( converterror );
        }
        while( *p >= '0' && *p <= '9' ) {   // try decimal part
            wd = 10 * wd + *p - '0';
            *ps++ = *p++;
            if( ps >= s->su_txt + sizeof( s->su_txt ) ) {
                if( quote ) {
                    p = skip_to_quote( p, quote );
                }
                *scanp = p;
                return( converterror );
            }
            if( p > pd1 + 2 ) {         // more than two digits
                if( quote ) {
                    p = skip_to_quote( p, quote );
                }
                *scanp = p;
                return( converterror );
            }
        }
        pdn = p;
    } else {
        if( k ) {                       // unit found
            pd1 = p;
            if( ps >= s->su_txt + sizeof( s->su_txt ) ) {
                if( quote ) {
                    p = skip_to_quote( p, quote );
                }
                *scanp = p;
                return( converterror );
            }
            while( *p >= '0' && *p <= '9' ) {   // try decimal part
                wd = 10 * wd + *p - '0';
                *ps++ = *p++;
                if( ps >= s->su_txt + sizeof( s->su_txt ) ) {
                    if( quote ) {
                        p = skip_to_quote( p, quote );
                    }
                    *scanp = p;
                    return( converterror );
                }
                if( p > pd1 + 2 ) {     // more than two digits
                    if( quote ) {
                        p = skip_to_quote( p, quote );
                    }
                    *scanp = p;
                    return( converterror );
                }
            }
            pdn = p;
        }
    }
    k = 0;
    pu = p;
    if( *p ) {                          // not yet at end
        while( *p && isalpha( *p ) ) {  // try trailing unit
            unit[k++] = tolower( *p );
            *ps++ = *p++;
            if( ps >= s->su_txt + sizeof( s->su_txt ) || k > 2 ) {
                if( quote ) {
                    p = skip_to_quote( p, quote );
                }
                *scanp = p;
                return( converterror );
            }
        }
    }

    *ps = '\0';

    s->su_whole = wh;
    s->su_dec   = wd;

    if( (quote && *p != quote ) || (!quote && *p == '\'') ) {
        if( quote ) {
            p = skip_to_quote( p, quote );
        }
        *scanp = p;
        return( converterror );
    }
    if( quote ) {
        p++;                            // over quote
    }

    *scanp = p;                         // report back scan position

    if( k == 0 ) {                      // no trailing unit
        pu = NULL;
    } else {
        if( pu == pp ) {                // no decimals, no unit
            pu = NULL;
        }
    }
    if( *pp == '.' ) {                  // dec point found
        if( pu == NULL ) {              // need trailing unit
            return( converterror );
        }
    } else {                            // no decimals
        if( pu != NULL ) {              // but unit specified twice?
            return( converterror );
        }
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
            break;
        case 'c' :
            s->su_u = SU_cicero;
            break;
        case 'p' :
            s->su_u = SU_pica;
            break;
        case '\0' :                     // no unit is characters or lines
            s->su_u = SU_chars_lines;
            break;
        default:
            return( converterror );
            break;
        }
    } else {                            // two letter unit
        if( unit[1] == 'm' ) {          // cm, mm ?
            if( unit[0] == 'c' ) {
                s->su_u = SU_cm;
            } else if( unit[0] == 'm' ) {
                s->su_u = SU_mm;
            } else {                    // invalid unit
                return( converterror );
            }
        } else if( unit[0] == 'd' ) {   // dv ?
            if( unit[1] == 'v' ) {
                s->su_u = SU_dv;
            }
        } else {                        // invalid unit
            return( converterror );
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
        if( wd != 0 ) {                 // no decimals allowed
            return( converterror );
        }
        break;
    case SU_dv :                        // decimals are allowed for dv
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
    converterror = false;
    return( converterror );
}

/***************************************************************************/
/*  use val_start/val_len to identify the value                            */
/*  only intended to work with tag attribute values, not control word data */
/*  conversion routines for Horizontal / Vertical space units              */
/*  Accepted formats:                                                      */
/*       1234        assume chars / lines                                  */
/*       8m          Ems                                                   */
/*       22Dv        Device units                                          */
/*                                                                         */
/*       12.34i      inch             (1 inch = 2.54cm)                    */
/*                      The exact definition of Cicero and Pica (points)   */
/*                      differs between Europe and USA,                    */
/*                      so until a better solution turns up:               */
/*       5C11        Cicero  + points (12 points = 1C = 1/6 inch)          */
/*       6p10        Pica    + points (12 points = 1P = 1/6 inch)          */
/*       5.23cm      centimeter                                            */
/*       6.75mm      millimeter                                            */
/*                                                                         */
/*    the absolute units (the last 5) will be stored                       */
/*    in 0.0001 millimeter units and 0.0001 inch units,                    */
/*    the relative ones (the first 3) will not be converted.               */
/*                                                                         */
/* extension for layout :BANREGION indent, hoffset and width attributes:   */
/*      symbolic units without a numeric value                             */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/*    returns  filled structure su, returncode false                       */
/*               or  returncode true in case of error                      */
/***************************************************************************/

bool    att_val_to_SU( su * converted, bool pos )
{
    bool        converterror    = true;
    bool        is_cp           = false;
    char    *   p               = NULL; // source of value text
    char    *   pd              = NULL; // ptr to decimal point
    char    *   pd1             = NULL; // ptr to 0.1 decimal
    char    *   pdn             = NULL; // ptr to last digit +1
    char    *   ps              = NULL; // destination for value text
    char    *   pu              = NULL; // ptr to trailing unit
    char        sign;
    char        unit[4];
    int         i;
    ldiv_t      div;
    long        k;
    long        wh;
    long        wd;
    su      *   s;

    if( (val_len + 1) > MAX_SU_CHAR ) {             // won't fit
        xx_line_err( err_inv_att_val, val_start );
        scan_start = scan_stop + 1;
        return( converterror );
    }
    unit[3] = '\0';
    unit[2] = '\0';
    unit[1] = '\0';
    unit[0] = '\0';
    s = converted;
    p = val_start;
    ps = s->su_txt;
    *ps = '\0';
    wd = 0;
    wh = 0;

    s->su_u = SU_undefined;
    if( *p == '+' ) {                   // not allowed with tags
        xx_line_err( err_inv_att_val, p );
        scan_start = scan_stop + 1;
        return( converterror );
    } else if( *p == '-' ) {            // not relative, just negative
        if( pos ) {                     // value must be positive
            xx_line_err( err_inv_att_val, p );
            scan_start = scan_stop + 1;
            return( converterror );
        }
        sign = *p;
        *ps++ = *p++;
        if( *p == '+' || *p == '-' ) {  // only one sign is allowed
            xx_line_err( err_inv_att_val, p );
            scan_start = scan_stop + 1;
            return( converterror );
        }
    } else {
        sign = '+';
    }
    if( (size_t)(p - val_start) >= val_len ) {  // value end reached, not valid
        xx_line_err( err_inv_att_val, p );
        scan_start = scan_stop + 1;
        return( converterror );
    }
    s->su_relative = false;             // no relative positioning with tags

    /***********************************************************************/
    /*  Special for layout :BANREGION                                      */
    /***********************************************************************/

    if( isalpha( *p ) ) {
        converterror = su_layout_special( &p, converted );
        if( !converterror ) {
            return( converterror );         // layout special ok
        }
    }

    for( i = 0; i < 4; i++ ) {              // max four digits in whole part
        if( (*p >= '0') && (*p <= '9') ) {
            wh = (10 * wh) + (*p - '0');
            *ps++ = *p++;
        } else {
            break;
        }
        if( (size_t)(p - val_start) > val_len ) {   // value end reached
            break;
        }
    }
    if( (*p >= '0') && (*p <= '9') ) {      // too many digits in whole part
        xx_line_err( err_inv_att_val, p );
        scan_start = scan_stop + 1;
        return( converterror );
    }

    if( ((size_t)(p - val_start) < val_len) && *p == '.' ) {   // check for decimal point
        pd = p;
        *ps++ = *p++;
        pd1 = p;                            // remember start of decimals
        for( i = 0; i < 2; i++ ) {          // max two digits in decimals
            if( (*p >= '0') && (*p <= '9') ) {
                wd = 10 * wd + *p - '0';
                *ps++ = *p++;
            } else {
                break;
            }
            if( (size_t)(p - val_start) > val_len ) {  // value end reached
                break;
            }
        }
        pdn = p;
        if( pd1 == p ) {                        // no decimals
            pd1 = NULL;
            pdn = NULL;
        }
        if( (*p >= '0') && (*p <= '9') ) {      // too many digits in decimals
            xx_line_err( err_inv_att_val, pdn );
            scan_start = scan_stop + 1;
            return( converterror );
        }
    }

    k = 0;
    pu = p;
    for( i = 0; i < 2; i++ ) {                  // max two characters in unit
        if( *p && isalpha( *p ) ) {
            unit[k++] = tolower( *p );          // save Unit
            *ps++ = *p++;
        } else {
            break;
        }
        if( (size_t)(p - val_start) > val_len ) {   // value end reached
            break;
        }
    }
    if( *p && isalpha( *p ) ) {             // too many characters in unit
        xx_line_err( err_inv_att_val, p );
        scan_start = scan_stop + 1;
        return( converterror );
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
                xx_line_err( err_inv_att_val, pd );
                scan_start = scan_stop + 1;
                return( converterror );
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
            xx_line_err( err_inv_att_val, pu );
            scan_start = scan_stop + 1;
            return( converterror );
            break;
        }
    } else {                            // two letter unit
        if( unit[1] == 'm' ) {          // cm, mm ?
            if( unit[0] == 'c' ) {
                s->su_u = SU_cm;
            } else if( unit[0] == 'm' ) {
                s->su_u = SU_mm;
            } else {                    // invalid unit
                xx_line_err( err_inv_att_val, pu );
                scan_start = scan_stop + 1;
                return( converterror );
            }
        } else if( unit[0] == 'd' ) {   // dv ?
            if( unit[1] == 'v' ) {
                s->su_u = SU_dv;
            }
        } else {                        // invalid unit
            xx_line_err( err_inv_att_val, pu );
            scan_start = scan_stop + 1;
            return( converterror );
        }
    }

    if( is_cp ) {       // "C" and "P" can be followed by max four digits
        for( i = 0; i < 4; i++ ) {
            if( (*p >= '0') && (*p <= '9') ) {
                wd = (10 * wd) + (*p - '0');
                *ps++ = *p++;
            }
            if( (size_t)(p - val_start) > val_len ) {   // value end reached
                break;
            }
        }
    }
    if( (*p >= '0') && (*p <= '9') ) {      // too many digits after "C" or "P"
        xx_line_err( err_inv_att_val, p );
        scan_start = scan_stop + 1;
        return( converterror );
    }

    *ps = '\0';
    if( (size_t)(p - val_start) < val_len ) {     // value continues on: it shouldn't
        xx_line_err( err_inv_att_val, p );
        scan_start = scan_stop + 1;
        return( converterror );
    }
    s->su_whole = wh;
    s->su_dec   = wd;

    if( k == 0 ) {                      // no trailing unit
        pu = NULL;
    }
    if( pd != NULL ) {                  // dec point found
        if( pu == NULL ) {              // need trailing unit
            xx_line_err( err_inv_att_val, pd );
            scan_start = scan_stop + 1;
            return( converterror );
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
    converterror = false;
    return( converterror );
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

int32_t conv_vert_unit( su * s, uint8_t spc )
{
    int32_t     ds;
    int32_t     fp;
    uint8_t     space;

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
/*  return the parameter which has the larger value                        */
/*  initially for LP, should also be needed for FIGCAP                     */
/*  these are intended to be vertical values                               */
/***************************************************************************/

su * greater_su( su * su_a, su * su_b, uint8_t spacing )
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

void    start_line_with_string( char * text, font_number font )
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

