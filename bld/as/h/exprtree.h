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
    ET_FIRST_LEAF,
        ET_CONSTANT = ET_FIRST_LEAF,
        ET_FP_CONSTANT,
        ET_UNNAMED_RELOCATABLE,
        ET_RELOCATABLE,
    ET_LAST_LEAF = ET_RELOCATABLE,
    ET_FIRST_UNARY,
        ET_NOT = ET_FIRST_UNARY,
        ET_PARENS,
        ET_UNARY_MINUS,
    ET_LAST_UNARY = ET_UNARY_MINUS,
    ET_FIRST_BINARY,
        ET_TIMES = ET_FIRST_BINARY,
        ET_MOD,
        ET_DIV,
        ET_PLUS,
        ET_MINUS,
        ET_SHIFT_L,
        ET_SHIFT_R,
        ET_AND,
        ET_XOR,
        ET_OR,
    ET_LAST_BINARY = ET_OR,
    ET_LAST
} et_class;

typedef struct expr_tree {
    et_class            type;
    union {
        signed_32               value;          /* const leaves */
        double                  fp_value;
        struct {
            union {
                sym_handle      sym;            /* for ET_RELOCATABLE */
                int_32          num;            /* for ET_UNNAMED_RELOCATABLE */
            }                   target;
            asm_reloc_type      type;
        }                       reloc_info;     /* reloc leaves */
        struct expr_tree        *left;          /* binary or unary nodes */
    } u;
    union {
        struct expr_tree        *right;         /* binary nodes */
        signed_32               disp;           /* displacement for relocs */
    } v;
} expr_tree;

#define _IsLeaf( x )    ( (x) >= ET_FIRST_LEAF && (x)  <= ET_LAST_LEAF )
#define _IsUnary( x )   ( (x) >= ET_FIRST_UNARY && (x) <= ET_LAST_UNARY )
#define _IsBinary( x )  ( (x) >= ET_FIRST_BINARY && (x)<= ET_LAST_BINARY )
#define _IsConstant( x ) ((x) == ET_CONSTANT || (x) == ET_FP_CONSTANT )

#define CONST_VALUE( x )        ((x)->u.value)
#define FP_CONST_VALUE( x )     ((x)->u.fp_value)

#define RELOC_SYMBOL( x )       ((x)->u.reloc_info.target.sym)
#define RELOC_LABELNUM( x )     ((x)->u.reloc_info.target.num)
#define RELOC_TARGET( x )       ((x)->u.reloc_info.target)
#define RELOC_TYPE( x )         ((x)->u.reloc_info.type)
#define RELOC_DISP( x )         ((x)->v.disp)

#define UNARY_CHILD( x )        ((x)->u.left)

#define BINARY_LEFT( x )        ((x)->u.left)
#define BINARY_RIGHT( x )       ((x)->v.right)

extern expr_tree        *AsETReloc( asm_reloc_type, sym_handle );
extern expr_tree        *AsETNumLabelReloc( asm_reloc_type, int_32 );
extern expr_tree        *AsETConst( signed_32 );
extern expr_tree        *AsETFPConst( double );
extern expr_tree        *AsETBinary( et_class, expr_tree *left, expr_tree *right );
extern expr_tree        *AsETUnary( et_class, expr_tree *child );
extern expr_tree        *AsETBurn( expr_tree *tree );
extern void             AsETFree( expr_tree *tree );

#define ETReloc                 AsETReloc
#define ETNumLabelReloc         AsETNumLabelReloc
#define ETConst                 AsETConst
#define ETFPConst               AsETFPConst
#define ETBinary                AsETBinary
#define ETUnary                 AsETUnary
#define ETBurn                  AsETBurn
#define ETFree                  AsETFree
