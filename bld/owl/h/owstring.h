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


/*
 * We use a patricia tree to keep track of our strings, with a
 * string pointer actually being a pointer to the node in the
 * tree which terminate the given string. When OWLStringEmit is
 * called, we calculate the offsets of each of the strings in
 * the string table and store this offset in the node.
 *
 * The code for the patricia table comes from "Algorithms in C"
 * by Sedgewick, and was modified by Anthony Scian.
 */

typedef struct patricia patricia;

struct patricia {
    patricia            *left;
    patricia            *right;
    uint_32             bit;
    owl_offset          offset;
    char                text[1];
};

typedef struct owl_string_table {
    owl_file_handle     file;
    uint_32             bytes;
    patricia            *tree;
} owl_string_table;

typedef patricia *owl_string_handle;

extern owl_string_table * OWLENTRY OWLStringInit( owl_file_handle file );
extern void OWLENTRY OWLStringFini( owl_string_table *table );
extern owl_string_handle OWLENTRY OWLStringAdd( owl_string_table *table, const char *string );
extern const char * OWLENTRY OWLStringText( owl_string_handle string );
extern owl_offset OWLENTRY OWLStringOffset( owl_string_handle string );
extern owl_offset OWLENTRY OWLStringTableSize( owl_string_table *table );
extern void OWLENTRY OWLStringEmit( owl_string_table *table, char *buffer );
extern void OWLENTRY OWLStringDump( owl_string_table *table );
