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


#ifndef CANTYPE_H
#define CANTYPE_H   1
typedef uint_32     bitsize;        /* type for sizes of types in bits */
typedef uint_16     type_handle;    /* type for referencing other types */
#define CANT_NULL   ((type_handle)0)/* unused handle */
typedef struct cantype      cantype;
typedef struct enum_const   enum_const;
typedef struct struct_field struct_field;
typedef struct proc_parm    proc_parm;

#include <stddef.h>
#include <watcom.h>
#include "canaddr.h"
#include "namemgr.h"

/*
    The following struct represents a type.  Each cantype is a node in the
    complete type graph for an object file.

    See the note about CanTMunge for details as to when all of these fields
    are valid.
*/
struct cantype {
    type_handle     hdl;        /* type handle for this type                */
    /*
        There are arrays that depend on the ordering of this enum.
    */
    enum cantype_class {
        CANT_RESERVED,
        CANT_INTEGER,
        CANT_REAL,
        CANT_VOID,
        CANT_COMPLEX,
        CANT_TYPEDEF,
        CANT_SUBRANGE,
        CANT_ARRAY,
        CANT_ARRAY_ZERO,
        CANT_ARRAY_DESC,
        CANT_POINTER,
        CANT_ENUM,
        CANT_STRUCT,
        CANT_PROCEDURE,
        CANT_CHARBLOCK,
        CANT_CHARBLOCK_IND
    }               class;      /* class of this type                       */
    uint_8          sgned   :1; /* for integer types                        */
    uint_8          busy    :1; /* can be used while exploring type graph   */
    bitsize         size;       /* size of this type in bits                */
    uint_32         extra;      /* an extra field for use by parser or
                                    generator in any manner it wishes       */
    union {
/*
    The following types have no extra data:

        CANT_INTEGER, CANT_VOID, CANT_REAL, CANT_COMPLEX
*/

/*
    CANT_TYPEDEF: Attach a new name to an existing type
*/
        struct {
            type_handle     type;       /* type of this name */
            name_handle     name;       /* name of this type */
            enum {
                CANT_SCOPE_NULL = 0,
                CANT_SCOPE_STRUCT,      /* i.e., "struct name" */
                CANT_SCOPE_UNION,
                CANT_SCOPE_ENUM
            } scope;                    /* scope of this typedef */
        } typdef;

/*
    CANT_SUBRANGE:  Subrange of CANT_INTEGER, CANT_ENUM, or CANT_SUBRANGE
*/
        struct {
            type_handle     base_type;
            uint_32         low;
            uint_32         high;
        } subrng;

/*
    CANT_ARRAY:  Array -- bounds are determined by another type (known at
        compile time)

    NOTE: in each of these array types the base_type is the first structure
        element.  DON'T CHANGE THIS!
*/
        struct {
            type_handle     base_type;  /* base type of the array */
            type_handle     index_type; /* CANT_SUBRANGE type of indicies */
        } array;

/*
    CANT_ARRAY_ZERO:  Array -- bounds are of form 0..high (Use for C Arrays)
*/
        struct {
            type_handle     base_type;
            uint_32         high;
        } arrayz;

/*
    CANT_ARRAY_DESC:  Array -- bounds not known at compile-time
*/
        struct {
            type_handle     base_type;
            type_handle     lo_type;    /* data type of low bound */
            type_handle     hi_type;    /* data type of high bound */
            addr_handle     bounds;     /* memory that contains struct with low
                                           and high bounds in that order */
        } arrayd;

/*
    CANT_POINTER: pointer types
*/
        struct {
            type_handle     base_type;  /* type pointed to */
            enum {                      /* attributes for pointer  */
                CANT_PTR_FAR    = 0x01,
                CANT_PTR_HUGE   = 0x02,
                CANT_PTR_DEREF  = 0x04,
                CANT_PTR_386    = 0x08
            } class;
        } pointr;

/*
    CANT_ENUM: enumerated constants of type CANT_INTEGER, or CANT_SUBRANGE
*/
        struct {
            type_handle     base_type;  /* base data type */
            uint_16         num_consts; /* number of consts in array */
            struct enum_const {
                name_handle name;
                uint_32     value;
            } *consts;
        } enumr;

/*
    CANT_STRUCT: structures and unions
*/
        struct {
            uint_16         num_fields;
            struct struct_field {
                uint_32     bit_offset;
                type_handle type;
                name_handle name;
                uint_8      bitfield : 1;
            } *fields;
        } strct;


/*
    CANT_PROCEDURE: procedure return and parameter types

        parms[0] is leftmost parameter
        parms[1] is next parm
        ...
        parms[num_parms-1] is rightmost parameter
*/
        struct {
            type_handle     ret_type;   /* return type of procedure */
            enum {
                CANT_PROC_FAR   = 0x01, /* __far procedure bit field */
                CANT_PROC_386   = 0x02  /* 386 procedure */
            } class;
            uint_8          num_parms;  /* number of parameters */
            struct proc_parm {
                type_handle type;
            } *parms;
        } proc;

/*
    CANT_CHARBLOCK: FORTRAN character block type
*/
        struct {
            uint_32         length;     /* length of character block */
        } charb;

/*
    CANT_CHARBLOCK_IND: FORTRAN character block type
*/
        struct {
            addr_handle     length;     /* location of variable length value */
            type_handle     length_type;/* data type of length */
        } charbi;

    } d;
};


/*
    In my (DJG) opinion, the CHARBLOCK_IND and ARRAY_DESC types are implemented
    improperly.  There should be no addr_handles in the type graph.  It would
    be more appropriate if a CHARBLOCK_IND were simply:
        struct {
            type_handle     length_type;
        } charbi;
    Then there can be a CANS_(MEM_LOC|BP_OFFSET|REGISTER) that has the
    type_handle of this CHARBLOCK_IND.

    If WATCOM and Microsoft DBI were handled this way then we could do proper
    translation of Fortran programs...
*/


void        CanTInit( void );
void        CanTFini( void );

/*
    The following functions are used to construct the graph
*/
type_handle CanTReserve( void );
void        CanTReUse( type_handle hdl );
type_handle CanTInteger( bitsize size, int sgned );
type_handle CanTReal( bitsize size );
type_handle CanTComplex( bitsize size );
type_handle CanTVoid( void );
type_handle CanTTypeDef( type_handle base_type, name_handle name, uint_8 scope);
type_handle CanTSubRange( type_handle base_type, uint_32 lo, uint_32 hi );
type_handle CanTArray( type_handle base_type, type_handle index_type );
type_handle CanTArrayZ( type_handle base_type, uint_32 high );
type_handle CanTArrayD( type_handle base_type, type_handle lo_type,
                type_handle hi_type, addr_handle bounds );
type_handle CanTPointer( type_handle base_type, uint_8 class );
cantype *   CanTEnum( type_handle base_type, uint_16 num_consts );
cantype *   CanTStruct( uint_16 num_fields );
cantype *   CanTProcedure( type_handle ret_type,uint_8 class, uint_8 num_parms);
type_handle CanTCharB( uint_32 length );
type_handle CanTCharBI( type_handle length_type, addr_handle length );
type_handle CanTDupSize( type_handle type_hdl, bitsize newsize );
/*
    CanTGraph must be called with the type returned from CanTStruct,
    CanTProcedure, and CanTEnum after the fields/parms have been initialized.
*/
void        CanTGraph( cantype *type );

/*
    The following functions are used to traverse the graph.
*/
cantype *   CanTFind( type_handle hdl );
int         CanTWalk( void *parm, int (*func)( void *type, void *parm ) );
cantype *   CanTElimTypeDef( type_handle start_hdl );

/*
    When the graph is first constructed not all nodes are complete.  So
    CanTMunge is used to complete the nodes of the graph.  CanTMunge
    has the following post condition:

    for each node T in the graph {
        T->sized == 1;  ( hence T->size is valid )
        switch( T->class ) {

        case CANT_SUBRANGE:
            T->sgned is set according to T->d.subrng.base_type;
            if( T->sgned ) {
                T->d.subrng.low && T->d.subrng.high are sign extended
                from T->size bits to full int_32s.
            }
            break;

        case CANT_ARRAY:
            switch( T->d.array.index_type ) {
            case CANT_INTEGER:
                num_elms = 1 << index_type->size;
                break;
            case CANT_ENUM:
                num_elms = highest value in ENUM - lowest value in ENUM + 1;
                break;
            case CANT_SUBRANGE:
                num_elms = SUBRANGE high - SUBRANGE low + 1;
                break;
            }
            T->size = size of base_type * num_elms;
            break;

        case CANT_ENUM:
            T->sgned is set according to T->d.enumr.base_type;
            if( T->sgned ) {
                for each constant C in T->d.enumr.consts {
                    C->value is signed extended from T->size bits to int_32s
                }
            }
            The list of constants C is sorted in increasing order by value.
            break;

        case CANT_STRUCT:
            T->size is set to the maximum of the set of
                { F->bit_offset + sizeof F->type where F is any field of T }
            The list of fields F is sorted in increasing order by bit_offset,
                and for identical bit_offsets fields are sorted in order by
                increasing size.
            break;
        }
    }

    Basically this post condition guarantees that the graph is cohesive and
    useful for generating debugging information from.  There is no guarantee
    that any of these conditions are met prior to a call to CanTMunge.

    CanTMunge assumes that the graph is complete.  i.e., there are no missing
    nodes.  (Such as the base_type for a TYPEDEF.)

    The debugging parser should call CanTMunge as its last action when
    building the type graph.
*/
void        CanTMunge( void );

#endif
