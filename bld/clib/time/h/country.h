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


#ifndef _COUNTRY_H_INCLUDED
#define _COUNTRY_H_INCLUDED

#include "variety.h"

struct country {
        int     co_date_format;         /* date format */
        char    co_currency_symbol[5];  /* currency symbol */
        char    co_thousands_sep[2];    /* thousands separator */
        char    co_decimal_sep[2];      /* decimal separator */
        char    co_date_sep[2];         /* date separator */
        char    co_time_sep[2];         /* time separator */
        char    co_currency_format;     /* currency format */
        char    co_digits;              /* significant digits in currency */
        char    co_time;                /* time format */
        char    (_WCFAR *co_case_map)(char);/* case mapping function */
        char    co_data_sep[2];         /* data separator */
        char    co_reserved[10];        /* reserved */
};
#endif
