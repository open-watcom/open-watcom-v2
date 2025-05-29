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
* Description:  WGML implement multi letter function &'length( )
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string function &'length(                                       */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'length(string):  The Length function returns  the length of 'string'  */
/*    in characters.   The  'string' may be null,  in which  case zero is  */
/*    returned.                                                            */
/*      &'length(abc) ==> 3                                                */
/*      &'length('Time Flies') ==> 10                                      */
/*      abc&'length(time flies)xyz ==> abc10xyz                            */
/*      +&'length(one)*&'length(two) ==> +3*3                              */
/*      &'length(one,two,three) ==> too many operands                      */
/*                                                                         */
/***************************************************************************/

condcode    scr_length( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    tok_type        string;
    int             string_len;

    (void)ressize;

    if( parmcount < 1
      || parmcount > 1 )
        return( CC_neg );

    string = parms[0].arg;
    string_len = unquote_arg( &string );

    *result += sprintf( *result, "%d", string_len );

    return( CC_pos );
}
