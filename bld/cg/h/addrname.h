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


/* aligned */
#include "typedef.h"

typedef enum {
        CL_ADDR_GLOBAL,
        CL_ADDR_TEMP,
        CL_POINTER,
        CL_VALUE2,
        CL_VALUE4,
        CL_CONS2,
        CL_CONS4,
        CL_GLOBAL_INDEX,
        CL_TEMP_INDEX,
        CL_TEMP_OFFSET,
        NUM_CLASSES
} addr_class;


typedef enum {
        FL_NORMALIZED               = 0x0001,
        FL_STACKABLE                = 0x0002,
        FL_VOLATILE                 = 0x0004,
        FL_ADDR_OK_ACROSS_BLOCKS    = 0x0008,
        FL_ADDR_CROSSED_BLOCKS      = 0x0010,
        FL_ADDR_DEMOTED             = 0x0020,
        FL_NEVER_STACK              = 0x0040,
        FL_CONSTANT                 = 0x0080,
        FL_UNALIGNED                = 0x0100,
} addr_flags;


typedef enum {
        NF_ADDR,
        NF_NAME,
        NF_CONS,
        NF_BOOL,
        NF_INS
} an_formats;


typedef struct address_name {
        an_formats              format;
        struct address_name     *link;
        struct type_def         *tipe;
        addr_class              class;
        addr_flags              flags;
        union {
            union name          *name;
            struct instruction  *ins;
        } u;
        union name              *index;
        type_length              offset;
        union name              *base;
        type_length             alignment;
} address_name;

typedef struct bool_node {
        an_formats              format;
        block_pointer           *t;
        block_pointer           *f;
        label_handle            e;
} bool_node;

typedef struct parm_node {
        struct parm_node        *next;
        struct address_name     *name;
        struct instruction      *ins;
        hw_reg_set              regs;
        int                     num;
        type_length             alignment;
        type_length             offset;
} parm_node;

typedef struct call_node {
        struct call_state       *state;
        struct address_name     *name;
        struct instruction      *ins;
        struct type_def         *tipe;
        struct parm_node        *parms;
} call_node;

typedef struct call_node        *cn;
typedef struct parm_node        *pn;
typedef struct address_name     *an;
typedef struct bool_node        *bn;
