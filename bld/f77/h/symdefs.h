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

#ifndef _F77_SYMDEFS_H
#define _F77_SYMDEFS_H 1

#define MAX_DIM 7               //  maximum # of dimensions allowed
#define STD_SYMLEN      6       //  maximum length of a FORTRAN name (standard)
#define MAX_SYMLEN      32      //  maximum length of a FORTRAN name (WATFOR-77)

typedef struct com_eq {                 //  common/equivalence extension
    signed_32           offset;         //  offset into equivalence set
    signed_32           low;            //  low extent of equivalence set
    signed_32           high;           //  high extent of equivalence set
    sym_id              com_blk;        //  pointer to common block
    sym_id              link_com;       //  pointer to next common entry
    sym_id              link_eqv;       //  pointer to next equivalence entry
    unsigned_16         ec_flags;       //  common/equivalence processing flags
} com_eq;

#define DIM_CONST_LO_1  0x0001  // low bound of 1st dimension is constant
#define DIM_CONST_LO_2  0x0002  // low bound of 2nd dimension is constant
#define DIM_CONST_LO_3  0x0004  // low bound of 3rd dimension is constant
#define DIM_CONST_LO_4  0x0008  // low bound of 4th dimension is constant
#define DIM_CONST_LO_5  0x0010  // low bound of 5th dimension is constant
#define DIM_CONST_LO_6  0x0020  // low bound of 6th dimension is constant
#define DIM_CONST_LO_7  0x0040  // low bound of 7th dimension is constant
#define DIM_PVD         0x0080  // psuedo-variably dimensioned array
                                // (i.e. A(...,1) ==> assumed size
#define DIM_ASSUMED     0x0100  // assumed size array
#define DIM_USED_IN_IO  0x0200  // array used in i/o statement
#define DIM_VARIABLE    0x0400  // variably dimensioned array
#define DIM_ALLOCATABLE 0x0800  // allocatable array
#define DIM_EXTENDED    0x1000  // array has extended pointer
#define DIM_COUNT_1     0x2000  // bit used for number of dimensions
#define DIM_COUNT_2     0x4000  // bit used for number of dimensions
#define DIM_COUNT_3     0x8000  // bit used for number of dimensions
#define DIM_COUNT_SHIFT 13      // shift count to get number of dimensions

#define _LoConstBound( dim_flags, subs_no )     \
        ( dim_flags & ( (unsigned_16)1 << ( subs_no - 1 ) ) )
#define _SetLoConstBound( dim_flags, subs_no )  \
        ( dim_flags |= ( (unsigned_16)1 << ( subs_no - 1 ) ) )
#define _DimCount( dim_flags )                  \
        ( dim_flags >> DIM_COUNT_SHIFT )
#define _SetDimCount( dim_flags, dim_count )    \
        ( dim_flags |= dim_count << DIM_COUNT_SHIFT )
#define _AdvRequired( dim )                     \
        (dim->dim_flags & (DIM_ASSUMED|DIM_VARIABLE|DIM_PVD|DIM_ALLOCATABLE))
#define _Allocatable( sym )                     \
        (sym->ns.xflags & (SY_ALLOCATABLE))
#define _ArrElements( sym ) (sym->ns.si.va.dim_ext->num_elts)

typedef struct dim_list {
    union {
        warp_label      init_label;     // initialization label for arrays
        unsigned short  cg_typ;         // cg-type for local arrays on stack
    } l;
    unsigned_16 dim_flags;      //  dimension flags
    pointer     adv;            //  address of ADV
    unsigned_32 num_elts;       //  number of elements
} dim_list;

typedef struct act_dim_list {
    union {
        warp_label      init_label;     // initialization label for arrays
        unsigned short  cg_typ;         // cg-type for local arrays on stack
    } l;
    unsigned_16 dim_flags;      //  dimension flags
    pointer     adv;            //  address of ADV
    unsigned_32 num_elts;       //  number of elements
    intstar4    subs_1_lo;      //  low bound of 1st dimension
    intstar4    subs_1_hi;      //  high bound of 1st dimension
    intstar4    subs_2_lo;      //  low bound of 2nd dimension
    intstar4    subs_2_hi;      //  high bound of 2nd dimension
    intstar4    subs_3_lo;      //  low bound of 3rd dimension
    intstar4    subs_3_hi;      //  high bound of 3rd dimension
    intstar4    subs_4_lo;      //  low bound of 4th dimension
    intstar4    subs_4_hi;      //  high bound of 4th dimension
    intstar4    subs_5_lo;      //  low bound of 5th dimension
    intstar4    subs_5_hi;      //  high bound of 5th dimension
    intstar4    subs_6_lo;      //  low bound of 6th dimension
    intstar4    subs_6_hi;      //  high bound of 6th dimension
    intstar4    subs_7_lo;      //  low bound of 7th dimension
    intstar4    subs_7_hi;      //  high bound of 7th dimension
} act_dim_list;

typedef struct act_eq_entry {             //  encoding of EQUIVALENCE statement
    struct act_eq_entry *next_eq_entry;   //  pointer to next entry
    struct act_eq_entry *next_eq_set;     //  pointer to next equivalence set
    sym_id              name_equived;     //  name equivalenced
    byte                substr;           //  indicates whether substrung or not
    byte                subs_no;          //  number of subscripts
    intstar4            subscrs[MAX_DIM]; //  subscripts
    intstar4            substr1;          //  1st substring
    intstar4            substr2;          //  2nd substring
} act_eq_entry;

typedef struct eq_entry {               //  encoding of EQUIVALENCE statement
    struct eq_entry     *next_eq_entry; //  pointer to next entry
    struct eq_entry     *next_eq_set;   //  pointer to next equivalence set
    sym_id              name_equived;   //  name equivalenced
    byte                substr;         //  indicates whether substrung or not
    byte                subs_no;        //  number of subscripts
} eq_entry;

#endif
