/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Constants for a base langinfo implementation.  Many will
*               be mildly US-centric as placeholders; please don't take
*               it the wrong way...
*
* Author: J. Armstrong
*
****************************************************************************/

/* For simplicity and compatibility, we'll state that
 * we support UTF-8 everywhere.
 */
#define BASE_CODESET    "US-ASCII"

#define CURRENCY_OFFSET 0x900
static char *__li_currency[] = {"", NULL};

#define YESNO_OFFSET    0x800
static char *__li_yesno[] = {"^[yY]", "^[nN]", NULL};

#define NUMBER_OFFSET   0x700
static char *__li_number[] = {"", ".", ",", NULL};

#define ERA_OFFSET      0x600
static char *__li_era[] = {"", "", "", "", NULL};

#define ABMON_OFFSET    0x500
static char *__li_abmon[] = {"Jan", "Feb", "Mar", "Apr",
                             "May", "Jun", "Jul", "Aug",
                             "Sep", "Oct", "Nov", "Dec",
                             NULL};

#define MON_OFFSET      0x400
static char *__li_mon[] = {"January", "February", "March", "April",
                           "May", "June", "July", "August",
                           "September", "October", "November", "December",
                           NULL};

#define ABDAY_OFFSET    0x300
static char *__li_abday[] = {"Sun", "Mon", "Tue", "Wed",
                             "Thu", "Fri", "Sat", NULL};

#define DAY_OFFSET      0x200
static char *__li_day[] = {"Sunday", "Monday", "Tuesday",
                           "Wednesday", "Thursday",
                           "Friday", "Saturday", NULL};

#define FMT_OFFSET      0x100
static char *__li_fmt[] = {"%a %b %e %H:%M:%S %Y",
                           "%y-%m-%d",             /* Note: should be a nice compromise */
                           "%H:%M:%S",
                           "%I:%M:%S %p",
                           "AM",
                           "PM",
                           NULL};
