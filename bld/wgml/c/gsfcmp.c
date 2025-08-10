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
    int             string1_len;
    int             string2_len;
    int             len;
    char            padchar;

    (void)ressize;

    if( parmcount < 2
      || parmcount > 3 )
        return( CC_neg );

    string1 = parms[0].arg;
    string2 = parms[1].arg;

    string1_len = unquote_arg( &string1 );
    string2_len = unquote_arg( &string2 );
    len = string1_len;
    if( len < string2_len )
        len = string2_len;

    index = 0;
    if( len > 0 ) {
        padchar = ' ';  /* default padding character ' ' */
        if( parmcount > 2 ) {
            tok_type pad = parms[2].arg;
            if( unquote_arg( &pad ) > 0 ) {
                padchar = *pad.s;
            }
        }
        for( i = 0; i < len; i++ ) {
            char    c1;
            char    c2;

            if( i < string1_len ) {
                c1 = string1.s[i];
            } else {
                c1 = padchar;
            }
            if( i < string2_len ) {
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

    return( CC_pos );
}
