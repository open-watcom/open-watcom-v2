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
* Description:  Definition of the 'name' union.
*
****************************************************************************/


/* aligned */
typedef enum {
        N_CONSTANT,
        N_MEMORY,
        N_TEMP,
        N_REGISTER,
        N_INDEXED,
        N_INITIAL,      /*  the next 3 used only by the scoreboarder */
        N_VOLATILE,
        N_ADDRESS
} name_class_def;

#define N_VISITED       0x80

typedef enum {
        X_SEGMENTED     = 0x0001,
        X_VOLATILE      = 0x0002,
        X_HIGH_BASE     = 0x0004,
        X_LOW_BASE      = 0x0008,
        X_FAKE_BASE     = 0x0010,
        X_BASE_IS_INDEX = 0x0020,
        X_CONSTANT      = 0x0040,
        X_LOW_ADDR_BASE = 0x0080,
        X_ALIGNED_1     = 0x0100,
        X_ALIGNED_2     = 0x0200,
        X_ALIGNED_4     = 0x0400,
        X_ALIGNED_8     = 0x0800,
} i_flags;

#define X_BASE          (X_HIGH_BASE+X_LOW_BASE)
#define HasTrueBase( op ) \
                ( (op)->i.base != NULL && !((op)->i.index_flags & X_FAKE_BASE) )
#define HasAlignment( op ) \
                ( (op)->i.index_flags & (X_ALIGNED_1|X_ALIGNED_2|X_ALIGNED_4|X_ALIGNED_8) )

typedef enum {
        USE_WITHIN_BLOCK        = 0x001,
        USE_IN_ANOTHER_BLOCK    = 0x002,
        DEF_WITHIN_BLOCK        = 0x004,
        USE_ADDRESS             = 0x008,
        USE_MEMORY              = 0x010,
        VAR_VOLATILE            = 0x020,
        HAS_MEMORY              = 0x040,
        NEEDS_MEMORY            = 0x080,
        VAR_CONSTANT            = 0x100,
        VAR_UNALIGNED           = 0x200,
} var_usage;

typedef enum {
        HAD_CONFLICT            = 0x0001,
        ONE_DEFINITION          = HAD_CONFLICT,
        ALIAS                   = 0x0002,
        VISITED                 = 0x0004,
        INDEXED                 = 0x0008,
        CAN_STACK               = 0x0010,
        STACK_PARM              = 0x0020,
        PUSH_LOCAL              = 0x0040,         /* 86 machines */
        FAR_LOCAL               = PUSH_LOCAL,     /* 370 machines */
        CROSSES_BLOCKS          = 0x0080,
        CONST_TEMP              = 0x0100,
        USED_AS_FD              = 0x0200,
        HIGH_ADDR               = 0x0400,         /* Alpha & RISC */
        MULT_DEFINITION         = 0x0800,         /* not ONE_DEFINITION no matter what other bit says :) */
        LOOP_DEFINITION         = 0x1000,         /* defined within a loop */
        CG_INTRODUCED           = 0x2000,         /* created under special circumstances by CG
                                                     so don't use normal size/range checks - this
                                                     is to allow weird t1-3 aliases made by rCYP_SEX */
} t_flags;

#define PERM_TEMP_FLAGS (STACK_PARM+CONST_TEMP)

typedef enum {
        CONS_ABSOLUTE,  /*  absolute constant (ie: 123e4) */
        CONS_ADDRESS,   /*  address constant (relocatable) */
        CONS_OFFSET,    /*  offset constant (relocatable) */
        CONS_SEGMENT,   /*  segment constant (relocatable) */
        CONS_TEMP_ADDR, /*  address constant of temp */
        CONS_HIGH_ADDR, /*  high part of a 32-bit address */
        CONS_LOW_ADDR,  /*  low part of a 32-bit address */
} constant_class;

typedef enum {
        MF_EMPTY                = 0x0000,
        MF_VISITED              = 0x0001,
} m_flags;

#include "bitset.h"
#define    EMPTY        0

typedef struct name_def {
        union name              *next_name;
        name_class_def          class;
        type_class_def          name_class;
        type_length             size;
} name_def;

typedef struct var_name {
        struct name_def         _n;
        struct conflict_node    *conflict;
        type_length             offset; /*  offset from symbol */ /*?????*/
        pointer                 symbol; /*  front end type symbol */
        var_usage               usage;
        var_usage               block_usage;
} var_name;

typedef struct constant_defn {
        struct constant_defn    *next_defn;
        pointer                 label;          /*  back end label for static */
        unsigned_16             value[4];       /*  in internal format */
        type_class_def          const_class;
} constant_defn;

typedef struct const_name {
        struct name_def         _n;
        pointer                 value;
        signed_32               int_value;
        signed_32               int_value_2;    /* high part of 64-bit const */
        struct constant_defn    *static_defn;
        constant_class          const_type;
} const_name;

typedef struct memory_name {            /*  global name value or address */
        struct var_name         _v;
        union name              *same_sym;
        cg_class                memory_type; /*  what the symbol points to */
        type_length             alignment;   /*  alignment - 0 if naturally aligned */
        m_flags                 memory_flags;
} memory_name;

#define NO_LOCATION     MAX_TYPE_LENGTH

typedef struct temp_name {
        struct var_name         _v;
        union name              *alias;
        type_length             location;
        union {
            int                 id;             /*  temporary identification */
            type_length         alt_location;
        } v;
        union {
            block_num           block_id;       /* AKA block_num */
            unsigned_16         ref_count;      /* for counting references */
        } u;
        t_flags                 temp_flags;
} temp_name;

typedef struct register_name {
        struct name_def         _n;
        hw_reg_set              reg;
        int                     reg_index;      /*  for scoreboarding */
        int                     arch_index;     /*  used for encoding on AXP */
} register_name;

typedef struct indexed_name {
        struct name_def         _n;
        union  name             *index;
        union  name             *base;
        type_length             constant;
        i_flags                 index_flags;
        unsigned char           scale;
} indexed_name;

typedef union name {
        struct  name_def        n;
        struct  var_name        v;
        struct  const_name      c;
        struct  memory_name     m;
        struct  temp_name       t;
        struct  register_name   r;
        struct  indexed_name    i;
        union   name            *_n;
} name;

#define _FrontEndTmp( op ) ( !( (op)->t.temp_flags & CONST_TEMP ) && \
                                (op)->v.symbol != NULL )

#define _ConstTemp( op ) ( ( (op)->n.class == N_TEMP ) && \
                           ( (op)->t.temp_flags & CONST_TEMP ) )
