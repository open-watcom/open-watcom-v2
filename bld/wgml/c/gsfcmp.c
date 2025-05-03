/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implement multi letter function &'compare( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'compare(                                      */
/*                                                                         */
/***************************************************************************/

/***************************************************************************
 *
 * &'compare(string1,string2<,pad>):  To return the position of the first
 *    character  that does  not match  in 'string1'  and 'string2'.    If
 *    required, the shorter string will be padded on the right with 'pad'
 *    which defaults to a blank.   If the strings are identical then zero
 *    is returned.
 *      &'compare('ABC','ABC') ==> 0
 *      &'compare(ABC,AP) ==> 2
 *      &'compare('AB ','AB') ==> 0
 *      &'compare('AB ','AB',' ') ==> 0
 *      &'compare('AB ','AB','X') ==> 3
 *      &'compare('AB-- ','AB','-') ==> 5
 *
 ***************************************************************************/

condcode    scr_compare( parm parms[MAX_FUN_PARMS], size_t parmcount, char **result, int32_t ressize )
{
    char            *string1s;
    char            *string1e;
    char            *string2s;
    char            *string2e;
    condcode        cc;
    int             i;
    int             index;
    size_t          len1;
    size_t          len2;
    size_t          len;
    char            padchar;

    (void)ressize;

    if( parmcount < 2
      || parmcount > 3 ) {
        cc = neg;
        return( cc );
    }

    string1s = parms[0].a;
    string1e = parms[0].e;
    unquote_if_quoted( &string1s, &string1e );
    len1 = string1e + 1 - string1s;   // string1 length

    string2s = parms[1].a;
    string2e = parms[1].e;
    unquote_if_quoted( &string2s, &string2e );
    len2 = string2e + 1 - string2s;   // string2 length

    len = len1;
    if( len < len2 )
        len = len2;

    index = 0;
    if( len > 0 ) {
        padchar = ' ';  /* default padding character ' ' */
        if( parmcount > 2 ) {
            char *parmxs;
            char *parmxe;
            parmxs = parms[2].a;
            parmxe = parms[2].e;
            unquote_if_quoted( &parmxs, &parmxe );
            if( parmxs <= parmxe ) {
                padchar = *parmxs;
            }
        }
        for( i = 0; i < len; i++ ) {
            char    c1;
            char    c2;

            if( i < len1 ) {
                c1 = string1s[i];
            } else {
                c1 = padchar;
            }
            if( i < len2 ) {
                c2 = string2s[i];
            } else {
                c2 = padchar;
            }
            if( c1 != c2 ) {
                index = i + 1;
                break;
            }
        }
    }
    *result += sprintf( *result, "%d", index );
    return( pos );
}
