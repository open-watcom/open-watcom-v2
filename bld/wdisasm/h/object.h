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


typedef enum {
    CL_LOBYTE,
    CL_OFFSET,
    CL_BASE,
    CL_POINTER,
    CL_HIBYTE,
    CL_LONG,            /* Phar-Lap */
    CL_LONG_POINTER     /* Phar-Lap */
}                       fixup_class;

typedef enum {
    TYPE_SEGMENT,
    TYPE_GROUP,
    TYPE_COMDAT,
    TYPE_FRAME,
    TYPE_MODULE,
    TYPE_IMPORT,        /* a symbol that appeared in an EXTDEF */
    TYPE_COMDEF         /* a symbol that appeared in a COMDEF */
}                       type_class;


#define ImportClass( cl ) ((cl)==TYPE_COMDAT    \
                          ||(cl)==TYPE_IMPORT   \
                          ||(cl)==TYPE_COMDEF)

typedef struct info     INFO;

typedef struct handle {
    INFO                *data;
    struct handle       *next_hndl;
} handle;

typedef struct info {
    type_class          class;
    char                *name;
    handle              *list;
} info;

typedef struct import_sym {
    type_class          class;
    char                *name;
    uint_16             type_id;
    struct import_sym   *next_imp;
    union {
        struct export_sym       *also_exp; /* when 'exported' is true */
        uint_32                 size;      /* for TYPE_COMDEF */
    }                   u;
    unsigned            far_common      : 1;
    unsigned            exported        : 1; /* also found PUBDEF */
    unsigned            public          : 1; /* vs. static */
} import_sym;

typedef struct export_sym {
    char                *name;
    uint_16             type_id;
    uint_32             address;
    struct export_sym   *next_exp;
    struct segment      *segment;
    unsigned            public  : 1;
    unsigned            dumped  : 1;
    unsigned            hidden  : 1;        /* UseORL only: hidden label */
} export_sym;

typedef struct symbol_list {
    char                *name;
    uint_32             address;
    void                *target;
    void                *frame;
    struct symbol_list  *next_sym;
} symbol_list;

typedef struct fixup {
    fixup_class         class;
    uint_32             address;
    void                *target;
    uint_32             imp_address;
    void                *frame;
    uint_32             seg_address;
    struct fixup        *next_fix;
} fixup;

typedef struct group {
    type_class          class;
    char                *name;
    struct group        *next_group;
    handle              *list;
    uint_16             id;
} group;

typedef struct scan_table {
    uint_32             starts;
    uint_32             ends;
    struct scan_table   *next;
} scan_table;

typedef struct segment {        /* also used for COMDAT's */
    type_class          class;
    char                *name;
    struct segment      *next_segment;
    group               *grouped;
    uint_32             address;
    uint_32             offset;
    uint_32             start;
    export_sym          *exports;
    export_sym          *exports_rover;
    export_sym          *last_export;
    fixup               *fixes;
    fixup               *fixes_rover;
    fixup               *last_fix;
    uint_32             size;
    void                *data;
    uint_32             curr;
    uint_16             id;
    uint_8              attr;
    union {
        /* for COMDAT's */
        struct {
            uint_8              align;
            struct segment      *seg;
            struct group        *grp;
        }               com;
        /* for SEGDEF's */
        struct {
            char                *class_name;
            uint_16             overlay_id;
        }               seg;
    }                   u;
    scan_table          *scan_tabs;
    void                *exp_lookup;
    unsigned            use_32      : 1;
    unsigned            access_valid: 1;    /* is next field valid          */
    unsigned            access_attr : 2;    /* EASY_OMF access attribute    */
    unsigned            data_seg    : 1;
    unsigned            dumped      : 1;
    unsigned            public      : 1;   /* for COMDAT's */
    unsigned            src_done    : 1;
} segment;

typedef struct line_num {
    int_16              num;
    uint_32             address;
    segment             *seg;
    struct line_num     *next_num;
} line_num;

typedef struct module {
    type_class          class;
    char                *name;
    handle              *list;
    group               *groups;
    segment             *segments;
    import_sym          *imports;
    void                *imp_lookup;
    symbol_list         *symbols;
    fixup               *start;
    line_num            *src;
    line_num            *src_rover;
    bool                main;
} module;

typedef struct thread {
    void                *datum;
    uint_32             address;
} thread;

#define _Class( var )           ( ( (info *) (var) )->class & 0x7f )
#define _Name( var )              ( (info *) (var) )->name
#define _List( var )              ( (info *) (var) )->list
#define _Size64K( var )         ( ( (segment *) (var) )->attr & 0x02 )
#define _SegAlign( var )      ( ( ( (segment *) (var) )->attr & 0xe0 ) >> 5 )
#define _SegCombine( var )    ( ( ( (segment *) (var) )->attr & 0x1c ) >> 2 )
#define _ComdatAlign( var )     ( ( (segment *) (var) )->u.com.align )
#define _ComdatMatch( var )   ( ( ( (segment *) (var) )->attr & 0xf0 ) >> 4 )
#define _ComdatAlloc( var )     ( ( (segment *) (var) )->attr & 0x0f )
