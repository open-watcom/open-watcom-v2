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


typedef struct owl_symbol_info  owl_symbol_info;
typedef uint_32                 owl_symbol_index;

typedef struct owl_symbol_table owl_symbol_table;
typedef struct owl_func_info    owl_func_info;

struct owl_symbol_table {
    unsigned            num_global_symbols;
    unsigned            num_local_symbols;
    owl_file_handle     file;
    owl_symbol_handle   head;
    owl_symbol_handle  *lnk;
};

typedef struct owl_func_file owl_func_file; // foward defn
struct  owl_func_file {
    owl_func_file  *next;           // next file
    owl_line_num    num_lines;      // num line for this file
    char            name[1];        // variable len path
};

typedef struct owl_func_info {
    owl_offset          start;                  // start of the function
    owl_offset          end;                    // end of the function
    owl_line_num        first_line;             // first line of function source
    owl_line_num        last_line;              // last line of function source
    owl_offset          linenum_offset;         // offset within section linenum buffer of first linenum record
    owl_line_num        num_lines;              // num lines for function start
    owl_func_file      *head;                   // list of file thins
    owl_func_file      **lnk;                   // last link
};

typedef struct {
    owl_func_info               *func;
    owl_symbol_info             *alt_sym;   // if weak symbol
    owl_section_handle          section;    // section meta-symbol
    owl_file_handle             file;       // file meta-symbol
} owl_sym_spec_info;

typedef enum {
    OWL_SYM_FLAG_COMDAT         = 0x0001,
    OWL_SYM_RAW_NAME            = 0x0002,       // ugly hack for easy imports
    OWL_SYM_LAZY                = 0x0004,       // search library
    OWL_SYM_EXPORT              = 0x0008,       // client wants this sym exported
    OWL_SYM_DEAD                = 0x0010,       // no longer needed
    OWL_SYM_COMDEF              = 0x0020,       // a comdef symbol
} owl_sym_flags;

struct owl_symbol_info {
    owl_symbol_info     *next;          // in order if defn
    owl_section_handle  section;        // section defined in (NULL for UNDEF'd)
    owl_string_handle   name;           // name of symbol
    owl_symbol_index    index;          // only valid after table has been emitted
    owl_symbol_index    comdat;         // index of the placeholder for this symbol (if comdat symbol)
    owl_sym_type        type;
    owl_sym_linkage     linkage;
    owl_offset          offset;         // offset within it's section
    unsigned            num_relocs;     // number of relocs to this symbol
    owl_sym_spec_info   x;
    owl_sym_flags       flags;
};

extern owl_symbol_table *       OWLENTRY OWLSymbolTableInit( owl_file_handle file );
extern void                     OWLENTRY OWLSymbolTableFini( owl_symbol_table *table );

extern owl_symbol_handle        OWLENTRY OWLSymbolInit( owl_file_handle file, const char *name );
extern void                     OWLENTRY OWLSymbolFini( owl_symbol_handle );

extern void                     OWLENTRY OWLSymbolDefine( owl_symbol_table *table,
                                                        owl_symbol_handle sym,
                                                        owl_section_handle section,
                                                        owl_offset offset,
                                                        owl_sym_type type,
                                                        owl_sym_linkage linkage );


extern owl_symbol_index         OWLENTRY OWLSymbolIndex( owl_symbol_handle symbol );

extern void                     OWLENTRY OWLSymbolEmit( owl_symbol_table *table );
