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


#include "as.h"

typedef struct numlab_reloc numlab_reloc;

struct numlab_reloc {
    owl_section_handle  section;        // location of the reloc
    owl_offset          offset;         // ...
    owl_reloc_type      type;
    numlab_reloc        *next;
};

typedef struct numlab numlab;

struct numlab {
    owl_section_handle  section;        // location of the numeric label
    owl_offset          offset;         // ...
    owl_sym_type        type;           // type of label
    numlab_reloc        *relocs;        // list of relocs to it
    numlab              *prev;          // prev defined label (same #)
    numlab              *next;          // next defined label (same #)
};

typedef struct {
    numlab              *first;         // First in the list
    numlab              *last;          // Last in the list
    numlab_reloc        *next_refs;     // List of [0-9]f relocs
} numeric_label_list;


static numeric_label_list numericLabels[10];    // 0: - 9:
static uint_32 numLabelCounts[10] = { 0 };  // to keep track of their parity

extern sym_handle AsNumLabelSymLookup( int_32 *label_num ) {
