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

#define HASH_TABLE_SIZE         211

static sym_handle       hashTable[ HASH_TABLE_SIZE ];

typedef struct sym_reloc_handle *sym_reloc_hdl_list;

struct sym_reloc_handle {
    sym_handle          hdl;
    sym_reloc_hdl_list  prev;
    sym_reloc_hdl_list  next;
};

typedef struct {
    sym_reloc_hdl_list  head;
    sym_reloc_hdl_list  tail;
} sym_reloc_hdls;

// These lists link all the sym_handles that have relocs hanging
// off of them, so that we can check for unemitted relocs at the end.
static sym_reloc_hdls  hi_reloc_hdls = { NULL, NULL };
static sym_reloc_hdls  lo_reloc_hdls = { NULL, NULL };

static void hdl_append( sym_reloc_hdls *hdls, sym_handle sym ) {
