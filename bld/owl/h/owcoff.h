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


#include "coff.h"

#pragma pack( 1 )

typedef uint_32         coff_offset;

typedef struct {
    unsigned_32         size;
    char                buffer[ 1 ];
} coff_string_table;

typedef struct coff_file_info {
    coff_offset         header_table_offset;
    coff_offset         symbol_table_offset;
    coff_offset         string_table_offset;
    coff_string_table   *string_table;
    owl_buffer_handle   exports;
} coff_file_info;

typedef struct coff_section_info {
    coff_offset         section_offset;
    coff_offset         relocs_offset;
    coff_offset         linenum_offset;
    coff_offset         alignment_padding;
} coff_section_info;

extern void COFFFileEmit( owl_file_handle );

#pragma pack()
