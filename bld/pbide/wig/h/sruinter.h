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


#ifndef INTERNAL_DATA_STRUCTS_INCLUDED
#define INTERNAL_DATA_STRUCTS_INCLUDED

#include "hashtab.h"
#include "srusuprt.h"
#include "list.h"

#define PB_NAME_LEN     40

#define ARRAY_SIMPLE    0x0001
#define ARRAY_MULTI_DIM 0x0002
#define ARRAY_RANGE     0x0004
#define ARRAY_DYNAMIC   0x0008

typedef struct {
    unsigned    flags;
    unsigned    elemcnt;
} ArrayInfo;

#define VAR_ARRAY       0x0001
#define VAR_BAD_DECL    0x0002

typedef struct {
    unsigned    flags;
    char        *name;
    ArrayInfo   array;
    BOOL        fake;
} VarInfo;

typedef struct {
    char        *name;
    BOOL        isref;
} TypeInfo;

typedef struct {                        // used for variables
    TypeInfo            type;           /* type of variable             */
    id_type             access_id;      /* protected, private or public */
    id_type             typ_id;         /* type id of variable          */
    List                *varlist;       /* list of variable information */
    BOOL                fake;           /* do not dump this out         */
} var_list;

typedef struct var_rec {                // used for parms
    struct var_rec      *next;          /* next variable (for parms)    */
    TypeInfo            type;           /* type of variable             */
    id_type             typ_id;         /* type id of variable          */
    char                *name;          /* name of variable             */
    BOOL                fake;           /* do not dump this out         */
    ArrayInfo           *array;
} var_rec;

typedef struct sp_header {
    TypeInfo            ret_type;       /* return type of sp            */
    id_type             typ_id;         /* typw id of type              */
    char                *name;          /* subprogram name              */
    id_type             typ;            /* subprogram type              */
    BOOL                user_code;      /* does it contain user code    */
    BOOL                fake;           /* do not dump this out         */
    BOOL                subroutine;     /* TRUE if this is a subroutine */
    var_rec             *parm_list;     /* its parameter list           */
    var_rec             *last_parm;     /* its last parameter           */
    struct statement    *ret_stmt;      /* pointer to return statement  */
} sp_header;

typedef struct sec_header {
    id_type     primary;        /* primary type of header               */
    id_type     secondary;      /* secondary type of header             */
} sec_header;

typedef union spec {
    var_list            vars;   /* variable data                        */
    sp_header           sp;     /* subprogram data                      */
    sec_header          sec;    /* section data                         */
} spec;

typedef struct statement {
    struct statement    *next;  /* next statment in the sru file        */
    struct statement    *link;  /* next specific type of statement      */
    BOOL                keep;   /* do not destroy after statement dump  */
    id_type             typ;    /* type of statement                    */
    spec                data;   /* statement related union of data      */
    char                stmt[1];/* statement                            */
} statement;

enum {          /* statement type */
    SRU_OTHER, /* must be 0     */
    SRU_VARIABLE,
    SRU_SUBPROG,
    SRU_SECTION
};

/* SRU flags */
#define CONSTRUCTOR_DEFINED     0x00000001
#define DESTRUCTOR_DEFINED      0x00000002
#define NEXT_TYPE_IS_REF        0x00000004
#define RETURN_STMT_PRESENT     0x00000008

typedef struct sru_file {
    char        *name;          /* name of sru target files             */
    statement   *head_stmt;     /* start of statements in file          */
    statement   *tail_stmt;     /* end of statements in file            */
    long        flags;          /* see above for flags settings         */
    hash_tab    typ_tab;        /* types hash table                     */
    char        *con_name;      /* constructor name to be used          */
    char        *des_name;      /* destructor name to be used           */
    id_type     curr_access;    /* default access specifier for data members */
    id_type     curr_typ;       /* current type of statement            */
    spec        curr;           /* union of statement data              */
    statement   *forward_prots; /* linked list of UO func prototypes    */
    statement   *cpp_prots;     /* linked list of external function names */
    hash_tab    type_prots;     /* hash table of cover functions        */
    statement   *subprog;       /* points to current subpprog statement */
    statement   *sections;      /* points to current section we are in  */
    statement   *type_sec;      /* points to type section of file       */
    statement   *var_sec;       /* points to variable section of file   */
    statement   *obj_vars;      /* linked list of instance variables    */
    statement   *shared_vars;   /* linked list of shared variables      */
} sru_file;

#endif
