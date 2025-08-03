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
* Description:  WGML implement multi letter functions &'subword( )
*                                                     &'word( )
*                                                     &'words( )
*                                                     &'wordpos( )
****************************************************************************/


#include "wgml.h"


#define SKIP_SPACES(x)  while((x)->s < (x)->e){if(*(x)->s != ' ') break; (x)->s++;}
#define SKIP_WORD(x)    while((x)->s < (x)->e){if(*(x)->s == ' ') break; (x)->s++;}

/***************************************************************************/
/*  script string function &'subword(                                      */
/*                         &'word(                                         */
/***************************************************************************/

static  condcode    scr_xx_word( parm parms[MAX_FUN_PARMS], unsigned parmcount,
                                 char **result, unsigned ressize, bool is_word )
{
    tok_type        string;
    char            *ptok;
    condcode        cc;
    int             k;
    int             n;
    int             string_len;
    int             length;
    getnum_block    gn;

    string = parms[0].arg;
    string_len = unquote_arg( &string );

    if( string_len > 0 ) {                      // null string nothing to do
        gn.ignore_blanks = false;
        n = 0;                                  // default start pos
        if( parms[1].arg.s < parms[1].arg.e ) {// start pos specified
            gn.arg = parms[1].arg;
            cc = getnum( &gn );
            if( cc != CC_pos ) {
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_exit_c( ERR_FUNC_PARM, "2 (startword)" );
                    /* never return */
                }
                return( cc );
            }
            n = gn.result;
        }

        if( is_word ) {
            length = 1;                 // only one word
        } else {
            length = 0;                 // default all words
            if( parmcount > 2 ) {       // evalute word count
                if( parms[2].arg.s < parms[2].arg.e ) {
                    gn.arg = parms[2].arg;
                    cc = getnum( &gn );
                    if( (cc != CC_pos) || (gn.result == 0) ) {
                        if( !ProcFlags.suppress_msg ) {
                            xx_source_err_exit_c( ERR_FUNC_PARM, "3 (length)" );
                            /* never return */
                        }
                        return( cc );
                    }
                    length = gn.result;
                }
            }
        }

        g_scandata = string;
        k = 0;
        cc = CC_pos;
        while( (k < n) && (cc != CC_omit) ) {  // find start word
            cc = getarg();
            k++;
        }
        if( cc != CC_omit ) {                  // start word does not exist
            string.s = g_tok_start;         // start word
            if( length == 0 ) {             // default word count = to end of string
                ptok = string.e;
                for( ; string.s < ptok && ressize > 0; string.s++ ) { // copy rest of words
                    *(*result)++ = *string.s;
                    ressize--;
                }
            } else {
                k = 0;
                for( k = 0; k < length; k++ ) {
                    ptok = g_tok_start;
                    cc = getarg();
                    if( cc == CC_omit ) {
                        ptok = string.e;
                        break;
                    }
                }
                for( ; string.s < ptok && ressize > 0; string.s++ ) { // copy rest of words
                    *(*result)++ = *string.s;
                    ressize--;
                }
                if( string.s < g_tok_start && ( *string.s != ' ' ) ) {  // copy last word
                    for( ; string.s < g_tok_start && *string.s != ' ' && ressize > 0; string.s++ ) {
                        *(*result)++ = *string.s;
                        ressize--;
                    }
                }
            }
        }
    }

    **result = '\0';

    return( CC_pos );
}


/***************************************************************************/
/* &'word(string,n):  The  Word function returns  only the 'n'th  word in  */
/*    'string'.   The value of 'n' must be positive.   If there are fewer  */
/*    than 'n' blank delimited words in the 'string' then the null string  */
/*    is returned.                                                         */
/*      &'word('The quick brown fox',3) ==> "brown"                        */
/*      &'word('The quick brown fox',5) ==> ""                             */
/*      &'word('The quick brown fox',0) ==> error, too small               */
/*      &'word('The quick brown fox') ==> error, missing number            */
/*      &'word('',1) ==> ""                                                */
/***************************************************************************/

condcode    scr_word( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    if( parmcount < 2
      || parmcount > 2 )
        return( CC_neg );

    return( scr_xx_word( parms, parmcount, result, ressize, true ) );
}


/***************************************************************************/
/*                                                                         */
/* &'subword(string,n<,length>):  The Subword function  returns the words  */
/*    of 'string' starting at word number 'n'.   The value of 'n' must be  */
/*    positive.  If 'length' is omitted, it will default to the remainder  */
/*    of the string.  The result will include all blanks between selected  */
/*    words and all leading and trailing blanks are not included.          */
/*      &'subword('The quick brown  fox',2,2) ==> "quick brown"            */
/*      &'subword('The quick brown  fox',3) ==> "brown  fox"               */
/*      &'subword('The quick brown  fox',5) ==> ""                         */
/*      &'subword('The quick brown  fox',0) ==> error, number too small    */
/*      &'subword('The quick brown  fox') ==> error, missing number        */
/*      &'subword('',1) ==> ""                                             */
/*                                                                         */
/***************************************************************************/

condcode    scr_subword( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    if( parmcount < 2
      || parmcount > 3 )
        return( CC_neg );

    return( scr_xx_word( parms, parmcount, result, ressize, false ) );
}


/***************************************************************************/
/* &'words(string):  The Words function returns the number of words found  */
/*    in 'string'.                                                         */
/*      &'words('The quick brown fox') ==> 4                               */
/*      &'words(' ') ==> 0                                                 */
/*      &'words('') ==> 0                                                  */
/*      &'words('cat dot',1) ==> too many operands                         */
/***************************************************************************/

condcode    scr_words( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    tok_type        string;
    int             wc;

    (void)ressize;

    if( parmcount < 1
      || parmcount > 1 )
        return( CC_neg );

    wc = 0;

    string = parms[0].arg;
    unquote_arg( &string );

    while( string.s < string.e ) {              // for all chars in string
        SKIP_SPACES( &string );                                  // find next word
        if( string.s >= string.e ) {            // at end
            break;
        }
        wc++;                                   // start of word found
        SKIP_WORD( &string );                    // end of word found
    }

    *result += sprintf( *result, "%d", wc );

    return( CC_pos );
}

static int find_words_phrase_in_string( tok_type *phrase, tok_type *string, int index )
{
    char            *start;
    bool            inword;
    bool            found;

    inword = true;
    found = false;
    start = phrase->s;
    while( string->s < string->e ) {
        if( !inword ) {
            index++;
            inword = true;
        }
        if( phrase->s >= phrase->e ) {
            if( *string->s == ' ' || *string->s == '\0' )
                return( index + 1 );    // current word in string

            return( 0 );
        }
        if( *string->s != *phrase->s ) {
            phrase->s = start;          // start new compare
            SKIP_WORD( string );        // skip word
            SKIP_SPACES( string );      // find next word
            inword = false;
        } else if( *string->s != ' ' ) {
            string->s++;
            phrase->s++;
        } else {
            inword = false;             // word end
            SKIP_SPACES( string );      // find next word
            SKIP_SPACES( phrase );      // find next word
        }
    }
    /*
     * end of string, check end of phrase
     */
    SKIP_SPACES( phrase );
    /*
     * check end of phrase
     * if it is true then found
     * otherwise not found
     */
    if( phrase->s < phrase->e )         // not end of phrase
        return( 0 );

    return( index + 1 );                // current word in string
}

/***************************************************************************/
/* &'wordpos(phrase,string<,start>):  The Word  Position function returns  */
/*    the word  position of  the words  in 'phrase'  within the  words of  */
/*    'string'.   The  search begins with the  first word of  'string' by  */
/*    default which may be changed by  specifying a positive 'start' word  */
/*    number.   All interword blanks are treated  as a single blank.   If  */
/*    the 'phrase' cannot be found the result is zero.                     */
/*      &'wordpos('quick brown fox','The quick brown fox') ==> 2           */
/*      &'wordpos('quick    brown','The quick  brown fox') ==> 2           */
/*      &'wordpos('quick  fox ','The quick  brown fox') ==> 0              */
/*      &'wordpos('xyz','The quick  brown fox') ==> 0                      */
/*      &'wordpos('The quick brown fox') ==> error, missing string         */
/***************************************************************************/

condcode    scr_wordpos( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    tok_type            phrase;
    tok_type            string;
    int                 start;

    int                 index;
    condcode            cc;
    int                 k;
    getnum_block        gn;

    (void)ressize;

    if( parmcount < 2
      || parmcount > 3 )
        return( CC_neg );

    index = 0;

    phrase = parms[0].arg;
    string = parms[1].arg;

    if( unquote_arg( &phrase ) > 0      // null phrase or string, nothing to do
      && unquote_arg( &string ) > 0 ) {
        start = 0;                      // default start word index
        if( parmcount > 2 ) {           // evaluate start word
            if( parms[2].arg.s < parms[2].arg.e ) {
                gn.arg = parms[2].arg;
                gn.ignore_blanks = false;
                cc = getnum( &gn );
                if( (cc != CC_pos) || (gn.result == 0) ) {
                    if( !ProcFlags.suppress_msg ) {
                        xx_source_err_exit_c( ERR_FUNC_PARM, "3 (startword)" );
                        /* never return */
                    }
                    return( cc );
                }
                start = gn.result - 1;
            }
        }
        g_scandata = string;
        k = 0;
        cc = CC_pos;
        g_tok_start = string.s;
        while( ( k <= start ) && ( cc != CC_omit ) ) { // find start word (by index)
            cc = getarg();
            k++;
        }
        if( cc != CC_omit ) {                  // start word exists
            string.s = g_tok_start;         // set start position
            index = find_words_phrase_in_string( &phrase, &string, start );
        }
    }

    *result += sprintf( *result, "%d", index );

    return( CC_pos );
}

/***************************************************************************
 *
 * &'find(string,phrase):  The Find function returns the word position of
 *    the words in 'phrase' within the words of 'string'.   All interword
 *    blanks are treated  as a single blank.   If the  'phrase' cannot be
 *    found the result is zero.
 *      &'find('The quick brown fox','quick brown fox') ==> 2
 *      &'find('The quick  brown fox','quick    brown') ==> 2
 *      &'find('The quick  brown fox','quick  fox ') ==> 0
 *      &'find('The quick  brown fox','xyz') ==> 0
 *      &'find('The quick brown fox') ==> error, missing phrase
 *
 ***************************************************************************/

condcode    scr_find( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    tok_type        phrase;
    tok_type        string;
    int             index;

    (void)ressize;

    if( parmcount < 2
      || parmcount > 2 )
        return( CC_neg );

    index = 0;

    string = parms[0].arg;
    phrase = parms[1].arg;

    if( unquote_arg( &string ) > 0      // null phrase nothing to do
      && unquote_arg( &phrase ) > 0 ) { // null string nothing to do
        index = find_words_phrase_in_string( &phrase, &string, 0 );
    }

    *result += sprintf( *result, "%d", index );

    return( CC_pos );
}
