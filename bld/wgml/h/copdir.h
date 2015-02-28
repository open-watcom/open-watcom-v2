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
* Description:  Declares functions used to manipulate .COP directory files:
*                   get_compact_entry()
*                   get_extended_entry()
*                   get_member_name()
*
* Note:         The field names are intended to correspond to the field names 
*               shown in the Wiki. The Wiki structs are named when the structs
*               defined here are defined; they are not identical.
*
****************************************************************************/

#ifndef COPDIR_H_INCLUDED
#define COPDIR_H_INCLUDED

#include <stdint.h>
#include <stdio.h>

/* Macro definition. */

#define DEFINED_NAME_MAX 78 // Per documentation, max length of a defined name.

/* Struct declaration. */

/* FILENAME_MAX is used for the member_name because gendev will embed member
 * names which exceed the space allowed by NAME_MAX or _MAX_FNAME in DOS if
 * such a member name is present in the source file. 
 */

/* To hold the data from either the CompactDirEntry struct or the
 * ExtendedDirEntry struct, since, in either case, only these two fields are used.
 */

typedef struct {
    char    defined_name[DEFINED_NAME_MAX + 1];
    char    member_name[FILENAME_MAX];
} directory_entry;

/* Enum declaration. */

typedef enum {
    valid_entry,        // Both defined_name and member_name were found.
    not_valid_entry     // The entry was not valid.
} entry_found;

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern entry_found  get_compact_entry( FILE * in_file, directory_entry * entry );
extern entry_found  get_extended_entry( FILE * in_file, directory_entry * entry );
extern char *       get_member_name( char const * in_name );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* COPDIR_H_INCLUDED */
