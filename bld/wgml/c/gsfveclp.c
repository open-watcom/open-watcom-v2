/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WGML implement multi letter functions  &'vecpos( )
*                                                  &'veclastpos( )
*
*               The optional parms START and CASE are not implemented
*               as they are not used in OW doc build system
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

static  bool    vec_pos;           // true if &'vecpos, false if &'veclastpos

/***************************************************************************/
/*  script string functions &'vecpos(                                      */
/*                          &'lastvecpos(                                  */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'vecpos(needle,haystack<,<start><,case>>):  The Vector Position func-  */
/*    tion returns the subscript number of  the first occurrence of 'nee-  */
/*    dle' in 'haystack'  set symbol.   The search for a  match starts at  */
/*    the first element  of 'haystack' but may be overridden  by adding a  */
/*    'start'  subscript in  'haystack'.    The default  case  of 'M'  or  */
/*    'Mixed' searches for  an element which matches exactly,   a case of  */
/*    'U' or  'Upper' will match the  'needle' and 'haystack'  element in  */
/*    uppercase.    If the  'needle'  string is  not  found,   a zero  is  */
/*    returned.                                                            */
/*      .se rodent(1) = 'Gerbil'                                           */
/*      .se rodent(2) = 'Hamster'                                          */
/*      .se rodent(3) = 'Mouse'                                            */
/*      .se rodent(4) = 'Rabbit'                                           */
/*      &'vecpos('Hamster',rodent) ==> 2                                   */
/*      &'vecpos(Dog,rodent) ==> 0                                         */
/*      &'vecpos(Hamster,rodent,3) ==> 0                                   */
/*      &'vecpos(HAMSTER,rodent) ==> 0                                     */
/*      &'vecpos(HAMSTER,rodent,1,U) ==> 2                                 */
/*      &'vecpos(Mouse) ==> error, too few operands                        */
/*                                                                         */
/* ! start and case are NOT implemented                                    */
/*                                                                         */
/***************************************************************************/


/***************************************************************************/
/* &'veclastpos(needle,haystack<,<start><,case>>):  The Vector Last Posi-  */
/*    tion function returns  the subscript number of  the last occurrence  */
/*    of 'needle'  in 'haystack'  set symbol.    The search  for a  match  */
/*    starts at the first element of  'haystack' but may be overridden by  */
/*    adding a 'start' subscript in 'haystack'.   The default case of 'M'  */
/*    or 'Mixed' searches for an element which matches exactly, a case of  */
/*    'U' or  'Upper' will match the  'needle' and 'haystack'  element in  */
/*    uppercase.    If the  'needle'  string is  not  found,   a zero  is  */
/*    returned.                                                            */
/*                                                                         */
/* ! start and case are NOT implemented                                    */
/*                                                                         */
/***************************************************************************/

static  condcode    scr_veclp( parm parms[MAX_FUN_PARMS], size_t parmcount,
                               char * * result, int32_t ressize )
{
    char            *   pneedle;
    char            *   pneedlend;
    char            *   phay;
    char            *   phayend;
    int                 rc;
    int                 index;
    int                 hay_len;
    int                 needle_len;
    char                c;
    sub_index           var_ind;
    symvar              symvar_entry;
    symsub          *   symsubval;
    symvar          *   psymvar;
    bool                suppress_msg;

    ressize = ressize;
    if( parmcount != 2 ) {
        return( neg );
    }

    pneedle = parms[0].a;
    pneedlend = parms[0].e;

    unquote_if_quoted( &pneedle, &pneedlend );
    needle_len = pneedlend - pneedle + 1;   // needle length

    phay = parms[1].a;
    phayend = parms[1].e;

    unquote_if_quoted( &phay, &phayend );
    hay_len = phayend - phay + 1;       // haystack length

    rc = 0;
    scan_err = false;
    index = 0;

    if( (hay_len > 0) ||                // not null string
        (needle_len > 0) ) {            // needle not null


        suppress_msg = ProcFlags.suppress_msg;
        ProcFlags.suppress_msg = true;
        scan_err = false;
        c = *(phayend + 1);
        *(phayend + 1) = '\0';

        scan_sym( phay, &symvar_entry, &var_ind );

        *(phayend + 1) = c;
        ProcFlags.suppress_msg = suppress_msg;;

        if( !scan_err ) {

            if( symvar_entry.flags & local_var ) {  // lookup var in dict
                rc = find_symvar_l( &input_cbs->local_dict, symvar_entry.name,
                                    var_ind, &symsubval );
            } else {
                rc = find_symvar( &global_dict, symvar_entry.name, var_ind,
                                  &symsubval );
            }
            if( rc > 0 ) {              // variable found
                psymvar = symsubval->base;
                if( psymvar->flags & subscripted ) {
                    c = *(pneedlend + 1);
                    *(pneedlend + 1) = '\0';   // make nul delimited
                    for( symsubval = psymvar->subscripts;
                         symsubval != NULL;
                         symsubval = symsubval->next ) {

                        if( !strcmp( symsubval->value, pneedle ) ) {
                           index = symsubval->subscript;
                           if( vec_pos ) {
                               break;// finished for vec_pos, go for veclastpos
                           }
                        }
                    }
                    *(pneedlend + 1) = c;
                }
            }
        }
    }

    *result += sprintf( *result, "%d", index );

    return( pos );
}


/*
 * &'vecpos(
 *
 */

condcode    scr_vecpos( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    vec_pos = true;
    return( scr_veclp( parms, parmcount, result, ressize ) );
}


/*
 * &'veclastpos(
 *
 */

condcode    scr_veclastpos( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    vec_pos = false;
    return( scr_veclp( parms, parmcount, result, ressize ) );
}
