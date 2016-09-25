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


/*note before you use this for anything make sure that the values below
are large enough for your purposes (particularly the num sections and symbols.
I put in some saftey code in the string table because if the user declares
long function names (very long like > 512) it is possible that 1k will not
suffice for the string table size in import libraries.  the number of sections
and symbols however are fixed and suffice for import libraries  */

#define MAX_NUM_COFF_LIB_SECTIONS   8
#define MAX_NUM_COFF_LIB_SYMBOLS    32
#define INIT_MAX_SIZE_COFF_STRING_TABLE  1024

typedef struct {
    coff_file_header    header;
    coff_section_header section[MAX_NUM_COFF_LIB_SECTIONS];
    coff_symbol         symbol[MAX_NUM_COFF_LIB_SYMBOLS];
    size_t              string_table_size;
    size_t              max_string_table_size;
    char                *string_table;
} coff_lib_file;
