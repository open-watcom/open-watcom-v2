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
* Description:  Format specification descriptor for scanf family.
*
****************************************************************************/


#ifndef _SCANF_H_INCLUDED
#define _SCANF_H_INCLUDED

#include "variety.h"
#include "widechar.h"
#include <stdarg.h>

typedef struct _SCNF_SPECS {
#if defined(__HUGE__)
    int         (*cget_rtn)( struct _SCNF_SPECS _WCFAR *specs );          /* character get rtn */
    void        (*uncget_rtn)( int c, struct _SCNF_SPECS _WCFAR *specs ); /* unget a character rtn */
#else
    int         (*cget_rtn)( struct _SCNF_SPECS *specs );           /* character get rtn */
    void        (*uncget_rtn)( int c, struct _SCNF_SPECS *specs);   /* unget a character rtn */
#endif
    CHAR_TYPE   *ptr;               /* file or string pointer */
    int         width;              /* conversion field width */
    unsigned    assign         : 1; /* assignment flag for current argument */
    unsigned    eoinp          : 1; /* end of input reached */
    unsigned    far_ptr        : 1; /* F  - far pointer */
    unsigned    near_ptr       : 1; /* N  - near pointer */
    unsigned    char_var       : 1; /* hh - char variable */
    unsigned    short_var      : 1; /* h  - short variable */
    unsigned    long_var       : 1; /* l  - long variable */
    unsigned    long_long_var  : 1; /* ll - long long variable */
    unsigned    long_double_var: 1; /* L - long double variable */
    unsigned    p_format       : 1; /* %p (pointer conversion) */
} SCNF_SPECS;

#if defined(__HUGE__)
    #define PTR_SCNF_SPECS SCNF_SPECS _WCFAR *
#else
    #define PTR_SCNF_SPECS SCNF_SPECS *
#endif

#if defined( __STDC_WANT_LIB_EXT1__ ) && __STDC_WANT_LIB_EXT1__ == 1
  #if defined(__WIDECHAR__)
    extern int __wscnf_s( PTR_SCNF_SPECS, const CHAR_TYPE *, const char **msg, va_list );
  #else
    extern int __scnf_s( PTR_SCNF_SPECS, const CHAR_TYPE *, const char **msg, va_list );
  #endif
#else
  #if defined(__WIDECHAR__)
    extern int __wscnf( PTR_SCNF_SPECS, const CHAR_TYPE *, va_list );
  #else
    extern int __scnf( PTR_SCNF_SPECS, const CHAR_TYPE *, va_list );
  #endif
#endif

//#pragma off(unreferenced);
#endif
