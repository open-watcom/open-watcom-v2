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
* Description:  Check if a year is a leap year + associated arrays
*
****************************************************************************/

#include "variety.h"
#include <time.h>
#include "rtdata.h"
#include "timedata.h"

short const __based(__segname("_CONST")) __diyr[] = { /* days in normal year array */
    0,                                                          /* Jan */
    31,                                                         /* Feb */
    31 + 28,                                                    /* Mar */
    31 + 28 + 31,                                               /* Apr */
    31 + 28 + 31 + 30,                                          /* May */
    31 + 28 + 31 + 30 + 31,                                     /* Jun */
    31 + 28 + 31 + 30 + 31 + 30,                                /* Jul */
    31 + 28 + 31 + 30 + 31 + 30 + 31,                           /* Aug */
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,                      /* Sep */
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,                 /* Oct */
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,            /* Nov */
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,       /* Dec */
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31   /* Jan, next year */
};

short const __based(__segname("_CONST")) __dilyr[] = { /* days in leap year array */
    0,                                                          /* Jan */
    31,                                                         /* Feb */
    31 + 29,                                                    /* Mar */
    31 + 29 + 31,                                               /* Apr */
    31 + 29 + 31 + 30,                                          /* May */
    31 + 29 + 31 + 30 + 31,                                     /* Jun */
    31 + 29 + 31 + 30 + 31 + 30,                                /* Jul */
    31 + 29 + 31 + 30 + 31 + 30 + 31,                           /* Aug */
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31,                      /* Sep */
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30,                 /* Oct */
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,            /* Nov */
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,       /* Dec */
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31   /* Jan, next year */
};

int __leapyear( unsigned year )
{
    if( year & 3 )
        return( 0 );
    if( ( year % 100 ) != 0 )
        return( 1 );
    if( ( year % 400 ) == 0 )
        return( 1 );
    return( 0 );
}
