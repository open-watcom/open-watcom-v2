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

condcode    scr_compare( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    tok_type        string1;
    tok_type        string2;
    int             i;
    int             index;
    size_t          len1;
    size_t          len2;
    size_t          len;
    char            padchar;

    (void)ressize;

    if( parmcount < 2
      || parmcount > 3 )
        return( neg );

    string1 = parms[0].arg;
    unquote_arg( &string1 );
    len1 = string1.e + 1 - string1.s;   // string1 length

    string2 = parms[1].arg;
    unquote_arg( &string2 );
    len2 = string2.e + 1 - string2.s;   // string2 length

    len = len1;
    if( len < len2 )
        len = len2;

    index = 0;
    if( len > 0 ) {
        padchar = ' ';  /* default padding character ' ' */
        if( parmcount > 2 ) {
            tok_type parmx;
            parmx = parms[2].arg;
            unquote_arg( &parmx );
            if( parmx.s <= parmx.e ) {
                padchar = *parmx.s;
            }
        }
        for( i = 0; i < len; i++ ) {
            char    c1;
            char    c2;

            if( i < len1 ) {
                c1 = string1.s[i];
            } else {
                c1 = padchar;
            }
            if( i < len2 ) {
                c2 = string2.s[i];
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
