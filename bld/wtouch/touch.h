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



typedef struct {
    uint_16     create_file : 1;
    uint_16     date_specified : 1;
    uint_16     time_specified : 1;
    uint_16     time_24hr : 1;
    uint_16     allow_read_only : 1;
    uint_16     increment_time : 1;
    uint_16     file_specified : 1;
    uint_16     usa_date_time : 1;
    uint_16     quiet : 1;
} touchflags;

typedef struct {
    uint_16     year;           /* full year (e.g., 1990) */
    uint_8      month;          /* 1-12 */
    uint_8      day;            /* 1-31 */
} datestruct;

typedef struct {
    uint_8      hours;          /* 0-23 */
    uint_8      minutes;        /* 0-59 */
    uint_8      seconds;        /* 0-59 */
} timestruct;

#define DEFAULT_TIME_SEPARATOR  ':'
#define DEFAULT_DATE_SEPARATOR  '-'

#define MINIMUM_SEC_INCREMENT   2       /* 2 seconds for DOS & OS/2 */
