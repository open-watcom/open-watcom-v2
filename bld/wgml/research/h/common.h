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
* Description:  Declares items common to all WGML files.
*
*               These functions must be implemented by each program
*               (or group of programs) that wishes to use them:
*                   parse_cmdline()
*                   print_banner()
*                   print_usage()
****************************************************************************/

#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "heapchk.h"    // Ensure is always available.

/* Function return values. */

#define FAILURE 0
#define SUCCESS 1

/* Global variable declarations. */

/* This allows the same declarations to function as definitions.
 * Just #define global before including this file.
 */

#ifndef global
    #define global  extern
#endif

/* Either '\', '/', or whatever DOS is using. */

global char         dos_switch_char;    

/* Reset so can be reused with other headers. */

#undef global

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

/* These functions must be defined by each program using them. */

extern  int         parse_cmdline( char * );
extern  void        print_banner( void );
extern  void        print_usage( void );

/* These functions are defined in common.c. */

extern  void        initialize_globals( void );
extern  char    *   skip_spaces( char * start );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* COMMON_H_INCLUDED */

