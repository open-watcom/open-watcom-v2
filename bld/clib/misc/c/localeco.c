/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "variety.h"
#include <limits.h>
#include <locale.h>


static struct lconv C_lconv = {
/*      char *decimal_point;        */  ".",
/*      char *thousands_sep;        */  "",
/*      char *int_curr_symbol;      */  "",
/*      char *currency_symbol;      */  "",
/*      char *mon_decimal_point;    */  "",
/*      char *mon_thousands_sep;    */  "",
/*      char *mon_grouping;         */  "",
/*      char *grouping;             */  "",
/*      char *positive_sign;        */  "",
/*      char *negative_sign;        */  "",
/*      char int_frac_digits;       */  CHAR_MAX,
/*      char frac_digits;           */  CHAR_MAX,
/*      char p_cs_precedes;         */  CHAR_MAX,
/*      char p_sep_by_space;        */  CHAR_MAX,
/*      char n_cs_precedes;         */  CHAR_MAX,
/*      char n_sep_by_space;        */  CHAR_MAX,
/*      char p_sign_posn;           */  CHAR_MAX,
/*      char n_sign_posn;           */  CHAR_MAX
};


_WCRTLINK struct lconv *localeconv()
    {
        return( &C_lconv );
    }
