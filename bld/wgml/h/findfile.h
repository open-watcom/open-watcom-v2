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
* Description:  Declares everything used by wgml to find and open files:
*                   dirseq
*                   ff_setup()
*                   ff_teardown()
*                   search_file_in_dirs()
*                   try_file_name
*                   try_fp
*
****************************************************************************/

#ifndef FINDFILE_H_INCLUDED
#define FINDFILE_H_INCLUDED

#include <stdio.h>

/* Extern enum declaration. */

/* Search sequences for various types of files. */

typedef enum {
    ds_opt_file = 1,    // wgml option files:     curdir, gmllib, gmlinc, path
    ds_doc_spec,        // wgml document source:  curdir, gmlinc, gmllib, path
    ds_bin_lib,         // wgml binary library:           gmllib, gmlinc, path
    ds_lib_src          // gendev source:         curdir, gmlinc
} dirseq;

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern  void        ff_teardown( void );
extern  void        ff_setup( void );
extern  bool        search_file_in_dirs( const char * filename, const char * defext, const char * altext, dirseq seq );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* FINDFILE_H_INCLUDED */

/* Extern variable declaration. */

/* This allows the same declarations to function as definitions.
 * Just #define global before including this file.
 */

#ifndef global
    #define global  extern
#endif

global  char    *   try_file_name;
global  FILE    *   try_fp;

/* Reset so can be reused with other headers. */

#undef global
