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


#ifndef CANSYMB_H
#define CANSYMB_H   1
/*
    The ordering of this enum is depended on in several places
*/
typedef enum cans_register_classes {
    CANS_REG_AL,
    CANS_REG_AH,
    CANS_REG_BL,
    CANS_REG_BH,
    CANS_REG_CL,
    CANS_REG_CH,
    CANS_REG_DL,
    CANS_REG_DH,
    CANS_REG_AX,
    CANS_REG_BX,
    CANS_REG_CX,
    CANS_REG_DX,
    CANS_REG_SI,
    CANS_REG_DI,
    CANS_REG_BP,
    CANS_REG_SP,
    CANS_REG_CS,
    CANS_REG_SS,
    CANS_REG_DS,
    CANS_REG_ES,
    CANS_REG_ST0,
    CANS_REG_ST1,
    CANS_REG_ST2,
    CANS_REG_ST3,
    CANS_REG_ST4,
    CANS_REG_ST5,
    CANS_REG_ST6,
    CANS_REG_ST7,
    CANS_REG_EAX,
    CANS_REG_EBX,
    CANS_REG_ECX,
    CANS_REG_EDX,
    CANS_REG_ESI,
    CANS_REG_EDI,
    CANS_REG_EBP,
    CANS_REG_ESP,
    CANS_REG_FS,
    CANS_REG_GS,
    CANS_REG_FLAGS
} register_type;
#define CANS_MAX_REGISTERS   4
typedef struct cansymb  *symb_handle;
#define CANS_NULL       ((symb_handle)0)

#include <watcom.h>
#include "canaddr.h"
#include "cantype.h"
#include "namemgr.h"

/*
    In the following,
        ()  indicate grouping
        |   indicates alternation
        *   indicates "zero or more of preceeding token"
        ?   indicates "zero or one of"

    The symbolic information for an object is in the following format:

    ( CHANGE_SEG | LOCAL | BLOCK )*

    A CHANGE_SEG consists of:
        CANS_CHANGE_SEG

    A BLOCK consists of:
        CANS_BLOCK
            PARM*           ; in left to right order
            PROLOG          ; to separate parms from locals
            RETURN?         ; location of the return value (depends on flag in
                              PROLOG)
            LOCAL*
            BLOCK*
            EPILOG
        CANS_BLOCK_END

    A LOCAL consists of:
        CANS_MEM_LOC | CANS_BP_OFFSET | CANS_REGISTER

    A PARM consists of:
        CANS_MEM_LOC | CANS_BP_OFFSET | CANS_REGISTER

    A RETURN consists of:
        CANS_MEM_LOC | CANS_BP_OFFSET | CANS_REGISTER | CANS_IND_REG

    A PROLOG consists of
        CANS_PROLOG

    An EPILOG consists of
        CANS_EPILOG

    FIXME: should be provision for CODE_LABELs (gotos), scoped constants,
    and scoped typedefs

    Provision for return_addr_offset (offset from bp/ebp to retn address on
    stack).  Provision for return_val_loc to specify where the return value
    will be.
*/


struct cansymb {
    symb_handle         fwd;
    symb_handle         bwd;
    enum {
        CANS_CHANGE_SEG,
        CANS_BLOCK,
        CANS_BP_OFFSET,
        CANS_MEM_LOC,
        CANS_REGISTER,
        CANS_PROLOG,
        CANS_EPILOG,
        CANS_BLOCK_END,
        CANS_IND_REG
    }                   class;
    uint_32             extra;
    union {
/*
    Some structures contain one of these at the beginning for uniformity
*/
        struct name_and_type {
            name_handle name_hdl;
            type_handle type_hdl;
        } nat;

/*
    CANS_CHANGE_SEG: change the code seg/offset for CANS_BLOCKs.
        The fixup must be seg-relative, and can only have one of the
        following loc methods: BASE, POINTER, POINTER386.
*/
        struct {
            addr_handle seg;
        } cseg;

/*
    CANS_BLOCK: procedure/inner block begin.
*/
        struct {
            struct name_and_type nat;
            symb_handle end;    /* the corresponding BLOCK_END */
            uint_32     start_offset;   /* offset from next most outer block */
                /* if no outer block then from last CHANGE_SET */
            uint_32     size;           /* size of block in bytes */
        } block;

/*
    CANS_BP_OFFSET: local variable at constant offset from BP.  Belongs to
        immediately enclosing BLOCK/BLOCK_END
*/
        struct {
            struct name_and_type nat;
            int_32 offset;  /* offset in bytes off of bp */
        } bpoff;

/*
    CANS_MEM_LOC: static locals, externs... anything with a static address.
        Belongs to immediately enclosing BLOCK/BLOCK_END, or to entire module
        if no enclosing BLOCK/BLOCK_END.

        Fixup must be a POINTER or POINTER386 style fixup.
*/
        struct {
            struct name_and_type nat;
            addr_handle mem_hdl;
            uint_8 is_static : 1;
        } memloc;

/*
    CANS_REGISTER: register variables... same scope rules as CANS_BP_OFFSET
        reg[ 0 ] -- least significant
        ...
        reg[ num_regs - 1 ] -- most significant
*/
        struct {
            struct name_and_type nat;
            uint_8 num_regs;
            register_type reg[ CANS_MAX_REGISTERS ];
        } reg;

/*
    CANS_PROLOG: information on size of prolog and location of return address
*/
        struct {
            uint_8  size;
            uint_8  has_ret_val  :1;/* is next local a return value location? */
            uint_8  has_ret_addr :1;/* does this block have a return addr? */
            int_32  ret_addr_offset;/* bp offset to ret address on stack */
        } prolog;

/*
    CANS_EPILOG: information on size of epilog
*/
        struct {
            uint_8  size;
        } epilog;

/*
    CANS_BLOCK_END: end of a block
*/
        struct {
            symb_handle start; /* corresponding start of block */
        } end;

/*
    CANS_IND_REG: used for structured return values
*/
        struct {
            struct name_and_type nat;
            register_type   off;
            register_type   seg;/* only if far_ptr == 1 */
            uint_8  calloc  :1;/* 1 means caller, 0 means routine called allocs
                                  memory and returns pointer to it in regs */
            uint_8  far_ptr :1;/* 1 means far ptr (2 regs), 0 near ptr (1 reg)*/
        } indr;

    } d;
};

void        CanSInit( void );
void        CanSFini( void );
symb_handle CanSGetHead( void );
void        CanSSetHead( symb_handle head );
symb_handle CanSFwd( symb_handle cursor );
#define     CanSFwd( cursor )   ((cursor)->fwd)
symb_handle CanSBwd( symb_handle cursor );
#define     CanSBwd( cursor )   ((cursor)->bwd)
symb_handle CanSIAfter( symb_handle cursor, symb_handle insert );
symb_handle CanSIBefore( symb_handle cursor, symb_handle insert );
symb_handle CanSNew( uint_8 class );

#endif
