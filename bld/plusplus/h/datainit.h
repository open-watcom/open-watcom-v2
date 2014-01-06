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


#ifndef _DATAINIT_H

typedef enum                    // initialization state
{
    DS_EXPRESSION,              // expecting simple expr, ctor, brace or expr
    DS_OPEN_BRACE,              // expecting open brace
    DS_CLOSE_BRACE,             // expecting close brace
    DS_FINISH,                  // expecting finish indication
    DS_COMPLETED,               // initialization completed
    DS_ERROR,                   // error occurred, will be abandoned
    DS_ABANDONED,               // error occurred, initialization was abandoned
    DS_IGNORE                   // too many errors, initialization ignored
} INITIALIZE_STATE;

typedef enum                    // initialization occurrence location
{
    DL_IGNORE,                  // too many errors, initialization ignored
    DL_INTERNAL_AUTO,           // auto within a function
    DL_INTERNAL_STATIC,         // static within a function
    DL_EXTERNAL_STATIC,         // static outside a function
    DL_EXTERNAL_PUBLIC          // outside a function
} INITIALIZE_LOCATION;

typedef enum                    // target type
{
    DT_SCALAR,                  // scalar
    DT_ARRAY,                   // array
    DT_CLASS,                   // class
    DT_BITFIELD,                // bitfield
    DT_ERROR                    // error
} INITIALIZE_TARGET;

typedef enum                    // type of stack entry
{
    DE_ROOT_TYPE,               // root type
    DE_BRACE,                   // open brace
    DE_TYPE                     // nested type
} INITIALIZE_ENTRY;

typedef struct initialize_info INITIALIZE_INFO;
struct initialize_info {                // size and offset of current item
    INITIALIZE_INFO         *stack;     // pointer to next in stack
    INITIALIZE_INFO         *previous;  // pointer to previous type entry
    TYPE                    type;       // type of this stack entry
    union {
        struct {
            SYMBOL          stop;       // member limit
            SYMBOL          curr;       // current member
        } c;                            // - class specific
        struct {
            target_size_t   index;      // index in array
        } a;                            // - array specific
        struct {
            TYPE            type;       // type of bitfield
            target_ulong    mask;       // initialized bits
        } b;                            // - bitfield specific
        struct {
            bool            bitf;       // part of a bitfield
        } s;                            // - scalar specific
    } u;
    INITIALIZE_ENTRY        entry;      // what is the stack entry
    INITIALIZE_TARGET       target;     // what is the target
    target_size_t           base;       // offset of this nest in aggregate
    target_size_t           offset;     // offset within this entry
    target_size_t           mem_size;   // size of this nest entry (no padding)
    target_size_t           padded_size;// size of this nest entry (with padding)
};

typedef struct initialize_queue INITIALIZE_QUEUE;
struct initialize_queue {               // location and size to be ctor'd
    INITIALIZE_QUEUE        *ring;      // link for ring structure
    target_size_t           start;      // location to start
    target_size_t           size;       // number of bytes to cover
};

typedef struct initialize_data INITIALIZE_DATA;
struct initialize_data {                // state of current initialization
    INITIALIZE_DATA     *prev;          // previous initialization
    DECL_INFO           *dinfo;         // symbol being initialized (readonly)
    INITIALIZE_INFO     *nest;          // stack of nesting information
    INITIALIZE_QUEUE    *queue;         // queue of padding ctors
    SYMBOL              sym;            // symbol being initialized
    SYMBOL              auto_sym;       // symbol for static copy of auto
    SYMBOL              ctor_sym;       // symbol for default ctor if req'd
    TYPE                sym_type;       // type of symbol being initted (readonly)
    TYPE                base_type;      // type for class if ctor req'd
    TYPE                auto_type;      // array type for static copy of auto
    TOKEN_LOCN          simple_locn;    // location for simple inits
    target_size_t       size_type;      // size of class elem for ctor
    INITIALIZE_STATE    state;          // what are we expecting
    INITIALIZE_LOCATION location;       // where is initialization occurring
    unsigned            once_only_label;// handle for once only label
    unsigned            once_only : 1;  // flag once only code
    unsigned            label_done : 1; // flag need label
    unsigned            auto_static : 1;// flag static created for auto
    unsigned            need_storage: 1;// flag need to allocate storage
    unsigned            ctor_reqd : 1;  // flag padding requires ctor
    unsigned            dtor_reqd : 1;  // flag symbol requires dtor
    unsigned            dtor_done : 1;  // flag dtor done
    unsigned            const_int : 1;  // flag store const in symbol
    unsigned            emit_code : 1;  // flag side effect code emitted
    unsigned            bracketed : 1;  // flag init_beg / init_done
    unsigned            all_zero : 1;   // flag all storage set to zero
    unsigned            huge_sym : 1;   // flag symbol is huge
    unsigned            initted : 1;    // flag actual init work done
    unsigned            no_size : 1;    // flag array of [] zero size
    unsigned            const_object :1;// flag const object
    unsigned            simple_set : 1; // flag simple_locn has been set
    unsigned            use_simple : 1; // flag use simple_locn
};

extern void DataInitStart( INITIALIZE_DATA *, DECL_INFO * );
extern void DataInitFinish( INITIALIZE_DATA * );
extern void DataInitSimple( PTREE );
extern void DataInitSimpleLocn( TOKEN_LOCN * );
extern void DataInitConstructorParms( PTREE );
extern void DataInitExpr( PTREE );
extern void DataInitPush( void );
extern void DataInitPop( void );
extern DECL_INFO *DataInitNoInit( INITIALIZE_DATA *, DECL_INFO * );

#define _DATAINIT_H
#endif
