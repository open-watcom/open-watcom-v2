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


#ifndef DWPRIV_H_INCLUDED
#define DWPRIV_H_INCLUDED

#include <assert.h>
#include <setjmp.h>
#include <string.h>
#include <watcom.h>
#include "dw.h"
#include "dwassert.h"
#include "dwarf.h"

typedef uint_32                 debug_ref;
#define WriteRef                WriteU32

#include "dwabort.h"
#include "dwline.h"
#include "dwdie.h"
#include "dwabbrev.h"
#include "dwcarve.h"
#include "dwhandle.h"

#define CLIReloc2( __s, __i )   cli->funcs.reloc( __s, __i )
#define CLIReloc3( __s, __i, __v )      \
                                cli->funcs.reloc( __s, __i, __v )
#define CLIReloc4( __s, __i, __v1, __v2 )      \
                                cli->funcs.reloc( __s, __i, __v1, __v2 )
#define CLIWrite( __s,  __b, __l )      \
                                cli->funcs.write( __s, __b, __l )
#define CLISeek( __s, __o, __t ) \
                                cli->funcs.seek( __s, __o, __t )
#define CLITell( __s )          cli->funcs.tell( __s )
#define CLIAlloc( __size )      cli->funcs.alloc( __size )
#define CLIFree( __ptr )        cli->funcs.free( __ptr )


struct handles_private {
    uint_32             num_handles;
    uint_32             forward;  // number of forward refs out
    struct handle_blk * block_head[ MAX_HANDLE_HEIGHT ];
    struct handle_blk **block_tail[ MAX_HANDLE_HEIGHT ];
    union handle_extra *extra_list;
    carve_t             extra_carver;
    carve_t             chain_carver;
    uint_8              max_height;
};


struct debug_line_private {
    dw_addr_offset      addr;
    dw_include_stack *  include_stack;
    dw_include *        files;
    dw_linenum          line;
    dw_column           column;
    uint_8              is_stmt : 1;
    uint_8              end_sequence : 1;
};

struct debug_abbrev_private {
    uint_8              emitted[ AB_BITVECT_SIZE ];
};

struct types_private {
    dw_size_t   byte_size;      // these used in enumerations.
    debug_ref   offset;
};

struct die_private {
    die_tree *          tree;
};

struct debug_loc_private {
    dw_loc_handle       handles;
    carve_t             label_carver;
};

struct decl_private {
    uint                file;
    dw_linenum          line;
    dw_column           column;
    uint_8              changed;
};

struct references_private {
    struct delayed_ref *delayed;
    carve_t             delay_carver;
    uint                scope;
    uint                delayed_file;
    dw_linenum          line;
    dw_column           column;
};

#pragma pack( 4 );
struct dw_client {
    jmp_buf             exception_handler;
    dw_funcs            funcs;
    char *              producer_name;
    uint_8              compiler_options;
    uint_8              language;
    debug_ref           section_base[ DW_DEBUG_MAX ];
    struct handles_private      handles;
    struct debug_line_private   debug_line;
    struct debug_abbrev_private debug_abbrev;
    struct types_private        types;
    struct die_private          die;
    struct debug_loc_private    debug_loc;
    struct decl_private         decl;
    struct references_private   references;
    uint_8                      offset_size;
    uint_8                      segment_size;
    dw_handle                   defset;
    dw_sym_handle               abbrev_sym;
    dw_sym_handle               dbg_pch;
};
#pragma pack();


#endif
