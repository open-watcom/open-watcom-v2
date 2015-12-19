/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 2015 Open Watcom Contributors.
*    All Rights Reserved.
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
* Description:  A simple langinfo implementation
*
* Author: J. Armstrong
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <langinfo.h>
#include "li_base.h"

static char *__langinfo_array_get(int index, char *array[])
{
    int i;
    for(i=0; i < index && array[i] != NULL; i++);
    return array[i];
}

_WCRTLINK char *nl_langinfo(int __item)
{
    char *ret;

    ret = NULL;

    if(__item == CODESET)
        ret = BASE_CODESET;

    else if(__item >= CURRENCY_OFFSET)
        ret = __langinfo_array_get(__item - CURRENCY_OFFSET, __li_currency);

    else if(__item >= YESNO_OFFSET)
        ret = __langinfo_array_get(__item - YESNO_OFFSET, __li_yesno);

    else if(__item >= NUMBER_OFFSET)
        ret = __langinfo_array_get(__item - NUMBER_OFFSET, __li_number);

    else if(__item >= ERA_OFFSET)
        ret = __langinfo_array_get(__item - ERA_OFFSET, __li_era);

    else if(__item >= ABMON_OFFSET)
        ret = __langinfo_array_get(__item - ABMON_OFFSET, __li_abmon);

    else if(__item >= MON_OFFSET)
        ret = __langinfo_array_get(__item - MON_OFFSET, __li_mon);

    else if(__item >= ABDAY_OFFSET)
        ret = __langinfo_array_get(__item - ABDAY_OFFSET, __li_abday);

    else if(__item >= DAY_OFFSET)
        ret = __langinfo_array_get(__item - DAY_OFFSET, __li_day);

    else if(__item >= FMT_OFFSET)
        ret = __langinfo_array_get(__item - FMT_OFFSET, __li_fmt);

    /* POSIX says unsupported args should return empty strings */
    if(ret == NULL)
        ret = "";

    return ret;
}
