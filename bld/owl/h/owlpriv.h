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


#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "owl.h"
#include "owstring.h"
#include "owbuffer.h"
#include "owsymbol.h"
#include "owelf.h"
#include "owcoff.h"
#include "log.h"

#ifndef min
#define min(x,y) (((x)<(y))?(x):(y))
#endif

typedef struct owl_info         owl_info;
typedef struct owl_file_info    owl_file_info;
typedef struct owl_section_info owl_section_info;
typedef struct owl_reloc_info   owl_reloc_info;
typedef struct owl_line_info    owl_line_info;

typedef uint_32                 owl_section_num;

#define FALSE                   0
#define TRUE                    1

struct owl_line_info {
    owl_offset                  addr;
    owl_line_num                line;
};

struct owl_reloc_info {
    owl_reloc_info              *next;
    owl_symbol_handle           symbol;
    owl_reloc_type              type;
    owl_offset                  location;
};

struct owl_info {
    owl_cpu                     cpu;
    owl_client_funcs            client_funcs;
    owl_file_info               *files;
};

struct owl_file_info {
    owl_file_info               *next;
    owl_handle                  info;
    owl_format                  format;
    owl_client_file             client_handle;
    owl_client_file             log;
    owl_file_type               type;
    owl_section_num             next_index;
    owl_string_table            *string_table;
    owl_symbol_table            *symbol_table;
    owl_section_info            *sections;
    owl_string_handle           name;
    unsigned                    num_symbols;
    union {
        elf_file_info           elf;            // format specific information
        coff_file_info          coff;
    } x;
};

struct owl_section_info {
    owl_section_info            *next;
    owl_file_handle             file;
    owl_string_handle           name;
    owl_section_type            type;
    owl_alignment               align;
    owl_section_num             index;          // not valid until header's written
    owl_buffer_handle           buffer;         // actual section contents
    owl_buffer_handle           linenum_buffer; // buffer for linenumber info
    owl_offset                  size;           // size of section in bytes
    owl_offset                  location;       // location next bytes to be written to
    unsigned                    num_relocs;     // number of relocs, if any
    unsigned                    num_linenums;   // number of line number records
    owl_symbol_handle           sym;            // symbol of section name
    owl_symbol_handle           comdat_sym;     // only global symbol for comdat section
    owl_section_info            *comdat_dep;    // comdat sections associate
    owl_reloc_info              *first_reloc;   // may be NULL of course
    owl_reloc_info              *last_reloc;
    union {
        elf_section_info        elf;
        coff_section_info       coff;
    } x;
};

#define _ClientAlloc( a, b )            ((a)->info->client_funcs.alloc( b ))
#define _ClientFree( a, b )             ((a)->info->client_funcs.free( b ))
#define _ClientWrite( a, b, c )         ((a)->info->client_funcs.write( (a)->client_handle, b, c ))
#define _ClientWriteFile( a, b, c, d )  ((a)->info->client_funcs.write( b, c, d ))
#define _ClientTell( a )                ((a)->info->client_funcs.tell( (a)->client_handle ))
#define _ClientSeek( a, b, c )          ((a)->info->client_funcs.lseek( (a)->client_handle, b, c ))
#define _ClientLabelName( a, b )        ((a)->info->client_funcs.lbl_name( b ))
#define _ClientLabelDone( a, b )        ((a)->info->client_funcs.lbl_done( b ))


extern void OWLENTRY OWLSectionFree( owl_section_handle section );
